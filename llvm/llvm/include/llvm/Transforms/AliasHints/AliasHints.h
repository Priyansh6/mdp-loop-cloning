#ifndef LLVM_TRANSFORMS_ALIASHINTS_ALIASHINTS_H
#define LLVM_TRANSFORMS_ALIASHINTS_ALIASHINTS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
//#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Pass.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopInterchange.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopCacheAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopNestAnalysis.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include <cassert>
#include <utility>
#include <vector>
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopAccessAnalysis.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/ConstantRange.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/IR/Metadata.h"
#include <cstdint>
#include <memory>

using namespace llvm;

namespace llvm{
class LPMUpdater;
class LoopNest;
struct AAMDNodes;

#define PREDICT_ALIAS_ADDRESS_SPACE 420
#define PREDICT_NO_ALIAS_ADDRESS_SPACE 69
#define CACHE_LINE_SIZE 8

typedef llvm::MemoryDepChecker::Dependence MemDep;

enum AliasHint{
    Predict,
    PredictNone,
    Unchanged
};

class AliasHintsPass : public PassInfoMixin<AliasHintsPass> {
public:
    bool Debug = false;
    PreservedAnalyses run(LoopNest &LN, LoopAnalysisManager &AM,
                          LoopStandardAnalysisResults &AR, LPMUpdater &U);
    void markLoads(LoopNest &LN, DependenceInfo &DI, LoopStandardAnalysisResults &AR, LLVMContext &Ctx);
    bool isProblematicDep(LoadInst *Load, Dependence *Dep, LoopInfo &LI, ScalarEvolution &SE, AAResults &AA);
    AliasHint determineHint(LoadInst *Load, SmallVector<StoreInst *> all_stores,
                            SmallVector<CallInst *> all_calls, std::map<Loop *, LoopAccessInfo *> LAIInstances, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, DependenceInfo DI, ScalarEvolution &SE,
                            AAResults &AA, LoopInfo &LI);
    void changeAddrSpace(LoadInst *Load, unsigned int Addrspace);
    SmallVector<std::pair<Loop *, Loop *>, 2> findVersionedLoops(LoopNest &LN, SmallVector<BasicBlock *, 1> GeneratedChecks, LoopInfo &LI, DominatorTree &DT);
    bool withinSameVersion(LoadInst *Load, Instruction *DepInst, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, LoopInfo &LI);
    void markConstantAccesses(Function &F, AAResults &AA, LLVMContext &Ctx);
};
}
#endif
