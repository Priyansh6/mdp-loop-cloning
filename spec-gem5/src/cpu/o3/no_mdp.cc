/*
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu/o3/no_mdp.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "cpu/o3/inst_queue.hh"


namespace gem5
{

namespace o3
{

NoMDP::NoMDP(const BaseO3CPUParams &params, MemDepUnit *_memDep)
    : memDepUnit(_memDep), depCheckShift(params.LSQDepCheckShift)
{
    if (!std::getenv("NOMDP")) { std::cout << "Error: Running no MDP without env variable set!\n"; exit(1); }
}

NoMDP::~NoMDP()
{
}

void
NoMDP::init(const BaseO3CPUParams &params, MemDepUnit *_memDep)
{
    memDepUnit = _memDep;
    depCheckShift = params.LSQDepCheckShift;
    if (!std::getenv("NOMDP")) { std::cout << "Error: Running no MDP without env variable set!\n"; exit(1); }
}

PredictionResult NoMDP::checkInst(Addr PC, InstSeqNum load_seq_num, BranchHistory branchHistory, bool isLoad)
{

    struct PredictionResult prediction = {0,0,0,0};

    if (!isLoad) return prediction;

    InstSeqNum youngestUnresolvedStore = memDepUnit->findUnresolvedStore(load_seq_num);

    prediction.seqNum = youngestUnresolvedStore;

    return prediction;

}

void NoMDP::violation(Addr load_PC, InstSeqNum load_seq_num, InstSeqNum store_seq_num, Addr store_PC, std::ptrdiff_t storeQueueDistance, bool predicted, unsigned predictedPathInex, uint64_t predictedHash, BranchHistory branchHistory)
{

    std::cout << "NoMDP found violation on " << load_PC << " (" << load_seq_num << ")!\n";
    //exit(1);

}

void NoMDP::commit(Addr load_pc, Addr load_addr, unsigned load_size, Addr store_addr, unsigned store_size, unsigned path_index, uint64_t predictor_hash) {

    bool misprediction;
    Addr load_eff_addr1 = load_addr >> depCheckShift;
    Addr load_eff_addr2 = (load_addr + load_size - 1) >> depCheckShift;
    Addr store_eff_addr1 = store_addr >> depCheckShift;
    Addr store_eff_addr2 = (store_addr + store_size - 1) >> depCheckShift;
    if (store_eff_addr2 >= load_eff_addr1 && store_eff_addr1 <= load_eff_addr2)
        misprediction = false;
    else
        misprediction = true;

    if (misprediction) ++(memDepUnit->stats.PHASTMispredictions);
    else ++(memDepUnit->stats.PHASTCorrectPredictions);
}

} // namespace o3
} // namespace gem5
