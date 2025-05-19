#include "llvm/Transforms/LoopClone/LoopClone.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/InstSimplifyFolder.h"
#include "llvm/Analysis/LoopAccessAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/LoopVersioning.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"
#include <cstdint>
#include <vector>

using namespace llvm;

// Find the first store instructions (within the same basic block)
// that may alias with this load.
StoreInst *findStoreForLoad(LoadInst *LI, AliasAnalysis &AA) {
  for (Instruction &I : *LI->getParent()) {
    if (auto *SI = dyn_cast<StoreInst>(&I)) {
      if (AA.alias(MemoryLocation::get(LI), MemoryLocation::get(SI)) ==
          AliasResult::MayAlias) {
        return SI;
      }
    }
  }
  return nullptr;
}

Value *getBase(const RuntimeCheckingPtrGroup *CG, Instruction *Loc,
               SCEVExpander &Exp) {
  LLVMContext &Ctx = Loc->getContext();
  Type *PtrArithTy = PointerType::get(Ctx, CG->AddressSpace);

  Value *Base = Exp.expandCodeFor(CG->Low, PtrArithTy, Loc);
  if (CG->NeedsFreeze) {
    IRBuilder<> Builder(Loc);
    Base = Builder.CreateFreeze(Base, Base->getName() + ".fr");
  }
  return Base;
}

Value *
addBaseEqualityChecks(Instruction *Loc,
                      const SmallVectorImpl<RuntimePointerCheck> &PointerChecks,
                      SCEVExpander &Exp) {
  // If there are no pointer pairs to check, return nullptr
  if (PointerChecks.empty())
    return nullptr;

  LLVMContext &Ctx = Loc->getContext();
  IRBuilder<InstSimplifyFolder> ChkBuilder(Ctx,
                                           Loc->getModule()->getDataLayout());
  ChkBuilder.SetInsertPoint(Loc);

  Value *FinalCheck = nullptr;

  for (const auto &CheckPair : PointerChecks) {
    const RuntimeCheckingPtrGroup *GroupA = CheckPair.first;
    const RuntimeCheckingPtrGroup *GroupB = CheckPair.second;

    Value *BaseA = getBase(GroupA, Loc, Exp);
    Value *BaseB = getBase(GroupB, Loc, Exp);

    // Ensure the pointers are in the same address space (safety check)
    assert(BaseA->getType()->getPointerAddressSpace() ==
               BaseB->getType()->getPointerAddressSpace() &&
           "Base pointers must be in the same address space");

    // Create an equality comparison: BaseA == BaseB
    Value *EqCheck = ChkBuilder.CreateICmpEQ(BaseA, BaseB, "base.eq");

    // Combine with previous checks using OR
    if (!FinalCheck) {
      FinalCheck = EqCheck;
    } else {
      FinalCheck = ChkBuilder.CreateOr(FinalCheck, EqCheck, "base.conflict");
    }
  }

  return FinalCheck;
}

void addPHINodes(const SmallVectorImpl<Instruction *> &DefsUsedOutside,
                 Loop *VersionedLoop, Loop *NonVersionedLoop,
                 ValueToValueMapTy &VMap, ScalarEvolution *SE) {
  BasicBlock *PHIBlock = VersionedLoop->getExitBlock();
  assert(PHIBlock && "No single successor to loop exit block");
  PHINode *PN;

  // First add a single-operand PHI for each DefsUsedOutside if one does not
  // exists yet.
  for (auto *Inst : DefsUsedOutside) {
    // See if we have a single-operand PHI with the value defined by the
    // original loop.
    for (auto I = PHIBlock->begin(); (PN = dyn_cast<PHINode>(I)); ++I) {
      if (PN->getIncomingValue(0) == Inst) {
        SE->forgetValue(PN);
        break;
      }
    }
    // If not create it.
    if (!PN) {
      PN = PHINode::Create(Inst->getType(), 2, Inst->getName() + ".lver");
      PN->insertBefore(PHIBlock->begin());
      SmallVector<User *, 8> UsersToUpdate;
      for (User *U : Inst->users())
        if (!VersionedLoop->contains(cast<Instruction>(U)->getParent()))
          UsersToUpdate.push_back(U);
      for (User *U : UsersToUpdate)
        U->replaceUsesOfWith(Inst, PN);
      PN->addIncoming(Inst, VersionedLoop->getExitingBlock());
    }
  }

  // Then for each PHI add the operand for the edge from the cloned loop.
  for (auto I = PHIBlock->begin(); (PN = dyn_cast<PHINode>(I)); ++I) {
    assert(PN->getNumOperands() == 1 &&
           "Exit block should only have on predecessor");

    // If the definition was cloned used that otherwise use the same value.
    Value *ClonedValue = PN->getIncomingValue(0);
    auto Mapped = VMap.find(ClonedValue);
    if (Mapped != VMap.end())
      ClonedValue = Mapped->second;

    PN->addIncoming(ClonedValue, NonVersionedLoop->getExitingBlock());
  }
}

void versionLoop(Loop *VersionedLoop, LoopStandardAnalysisResults &AR,
                 LoopAccessInfo &LAI) {

  Value *MemRuntimeCheck;
  // Value *SCEVRuntimeCheck;
  Value *RuntimeCheck = nullptr;

  // Add the memcheck in the original preheader (this is empty initially).
  BasicBlock *RuntimeCheckBB = VersionedLoop->getLoopPreheader();
  const auto &RtPtrChecking = *LAI.getRuntimePointerChecking();

  SCEVExpander Exp2(*RtPtrChecking.getSE(),
                    VersionedLoop->getHeader()->getModule()->getDataLayout(),
                    "induction");
  MemRuntimeCheck =
      addBaseEqualityChecks(RuntimeCheckBB->getTerminator(),
                            LAI.getRuntimePointerChecking()->getChecks(), Exp2);

  // SCEVExpander Exp(AR.SE, RuntimeCheckBB->getModule()->getDataLayout(),
  //                  "scev.check");
  // const SCEVPredicate &Preds = LAI.getPSE().getPredicate();
  // SCEVRuntimeCheck =
  //     Exp.expandCodeForPredicate(&Preds, RuntimeCheckBB->getTerminator());

  IRBuilder<InstSimplifyFolder> Builder(
      RuntimeCheckBB->getContext(),
      InstSimplifyFolder(RuntimeCheckBB->getModule()->getDataLayout()));
  // if (MemRuntimeCheck && SCEVRuntimeCheck) {
  //   Builder.SetInsertPoint(RuntimeCheckBB->getTerminator());
  //   RuntimeCheck =
  //       Builder.CreateOr(MemRuntimeCheck, SCEVRuntimeCheck, "lver.safe");
  // } else
  //   RuntimeCheck = MemRuntimeCheck ? MemRuntimeCheck : SCEVRuntimeCheck;
  if (!MemRuntimeCheck) {
    outs() << "No memcheck found.\n";
    return;
  }
  RuntimeCheck = MemRuntimeCheck;

  assert(RuntimeCheck && "called even though we don't need "
                         "any runtime checks");

  // Rename the block to make the IR more readable.
  RuntimeCheckBB->setName(VersionedLoop->getHeader()->getName() +
                          ".lver.check");

  // Create empty preheader for the loop (and after cloning for the
  // non-versioned loop).
  BasicBlock *PH = SplitBlock(RuntimeCheckBB, RuntimeCheckBB->getTerminator(),
                              &AR.DT, &AR.LI, nullptr,
                              VersionedLoop->getHeader()->getName() + ".ph");

  // Clone the loop including the preheader.
  //
  // FIXME: This does not currently preserve SimplifyLoop because the exit
  // block is a join between the two loops.
  SmallVector<BasicBlock *, 8> NonVersionedLoopBlocks;
  /// This maps the instructions from VersionedLoop to their counterpart
  /// in NonVersionedLoop.
  ValueToValueMapTy VMap;
  Loop *NonVersionedLoop = cloneLoopWithPreheader(
      PH, RuntimeCheckBB, VersionedLoop, VMap, ".lver.orig", &AR.LI, &AR.DT,
      NonVersionedLoopBlocks);
  remapInstructionsInBlocks(NonVersionedLoopBlocks, VMap);

  // Insert the conditional branch based on the result of the memchecks.
  Instruction *OrigTerm = RuntimeCheckBB->getTerminator();
  Builder.SetInsertPoint(OrigTerm);
  Builder.CreateCondBr(RuntimeCheck, NonVersionedLoop->getLoopPreheader(),
                       VersionedLoop->getLoopPreheader());
  OrigTerm->eraseFromParent();

  // The loops merge in the original exit block.  This is now dominated by the
  // memchecking block.
  AR.DT.changeImmediateDominator(VersionedLoop->getExitBlock(), RuntimeCheckBB);

  // Adds the necessary PHI nodes for the versioned loops based on the
  // loop-defined values used outside of the loop.
  auto DefsUsedOutside = findDefsUsedOutsideOfLoop(VersionedLoop);
  addPHINodes(DefsUsedOutside, VersionedLoop, NonVersionedLoop, VMap, &AR.SE);
  formDedicatedExitBlocks(NonVersionedLoop, &AR.DT, &AR.LI, nullptr, true);
  formDedicatedExitBlocks(VersionedLoop, &AR.DT, &AR.LI, nullptr, true);
  assert(NonVersionedLoop->isLoopSimplifyForm() &&
         VersionedLoop->isLoopSimplifyForm() &&
         "The versioned loops should be in simplify form.");

  outs() << "Num of checks: " << LAI.getNumRuntimePointerChecks() << "\n";
}

PreservedAnalyses LoopClonePass::run(LoopNest &LN, LoopAnalysisManager &AM,
                                     LoopStandardAnalysisResults &AR,
                                     LPMUpdater &U) {
  Function *F = LN.getParent();
  outs() << "Running LoopClonePass on function: " << F->getName() << "\n";

  // AliasAnalysis &AA = AR.AA;
  Loop *InnerLoop = LN.getInnermostLoop();
  if (!InnerLoop) {
    outs() << "No innermost loop found.\n";
    return PreservedAnalyses::all();
  }
  // for (auto *BB : Loop->getBlocks()) {
  //   for (Instruction &I : *BB) {
  //     if (auto *LI = dyn_cast<LoadInst>(&I)) {
  //       if (StoreInst *SI = findStoreForLoad(LI, AA)) {
  //         outs() << "Found store that may alias with load: " << *LI << "\n";
  //         outs() << "Store: " << *SI << "\n";
  //       }
  //     }
  //   }
  // }
  LoopAccessInfo LAI{InnerLoop, &AR.SE, &AR.TLI, &AR.AA, &AR.DT, &AR.LI};
  // LoopVersioning LV{LAI, LAI.getRuntimePointerChecking()->getChecks(),
  //                   InnerLoop,      &AR.LI,
  //                   &AR.DT, &AR.SE};
  // LV.versionLoop();
  // versionLoop(InnerLoop, AR, LAI);

  return PreservedAnalyses::all();
}

llvm::PassPluginLibraryInfo getLoopClonePassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopClone", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // PB.registerLateLoopOptimizationsEPCallback(
            //     [](llvm::LoopPassManager &LPM, OptimizationLevel Level) {
            //       LPM.addPass(LoopClonePass());
            //     });
            // PB.registerPipelineParsingCallback(
            //     [](StringRef Name, llvm::LoopPassManager &LPM,
            //        ArrayRef<llvm::PassBuilder::PipelineElement>) {
            //       if (Name == "loop-clone") {
            //         LPM.addPass(LoopClonePass());
            //         return true;
            //       }
            //       return false;
            //     });
          }};
}

#ifndef LLVM_LOOPCLONEPASS_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopClonePassPluginInfo();
}
#endif
