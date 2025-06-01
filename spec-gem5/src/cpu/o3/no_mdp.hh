/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
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

#ifndef __CPU_O3_STORE_SET_HH__
#define __CPU_O3_STORE_SET_HH__

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "base/statistics.hh"
#include "base/types.hh"
#include "cpu/inst_seq.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/limits.hh"
#include "params/BaseO3CPU.hh"
#include <cstdint>
#include <deque>
#include <bitset>
#include <stdlib.h>

namespace gem5
{

struct BaseO3CPUParams;

namespace o3
{

struct PredictionResult;

class MemDepUnit;

//#include "mem_dep_unit.hh"
class NoMDP
{
  public:
    /** Default constructor.  init() must be called prior to use. */
    NoMDP() { };

    NoMDP(const BaseO3CPUParams &params, MemDepUnit *_memDep);

    /** Default destructor. */
    ~NoMDP();

    void init(const BaseO3CPUParams &params, MemDepUnit *_memDep);

    /** Records a memory ordering violation between the younger load
     * and the older store. */
    void violation(Addr load_pc, InstSeqNum load_seq_num, InstSeqNum store_seq_num, Addr store_pc, std::ptrdiff_t storeQueueDistance, bool predicted, unsigned predictedPathInex, uint64_t predictedHash, BranchHistory branchHistory);

    void insertLoad(Addr load_PC, InstSeqNum load_seq_num) { return; }

    void insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid) { return; }

    /** Checks if the instruction with the given PC is dependent upon
     * any store.  @return Returns the sequence number of the store
     * instruction this PC is dependent upon.  Returns 0 if none.
     */
    PredictionResult checkInst(Addr PC, InstSeqNum load_seq_num, BranchHistory branchHistory, bool isLoad);

    void commit(Addr load_pc, Addr load_addr, unsigned load_size, Addr store_addr, unsigned store_size, unsigned path_index, uint64_t predictor_hash);

    /** Records this PC/sequence number as issued. */
    void issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store) { return; }

    /** Squashes for a specific thread until the given sequence number. */
    void squash(InstSeqNum squashed_num, ThreadID tid) { return; }

    /** Resets all tables. */
    void clear() { return; }

  private:

    unsigned depCheckShift;

    MemDepUnit *memDepUnit;
};

} // namespace o3
} // namespace gem5

#endif // __CPU_O3_STORE_SET_HH__
