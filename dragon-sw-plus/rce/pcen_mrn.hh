/*
 * Copyright (c) 2005
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2006
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __PCEN_MRN_HH__
#define __PCEN_MRN_HH__
#include "rce_types.hh"
#include "rce_pcen.hh"
#include <list>
using namespace std;

class PCEN_MRN: public PCEN
{
private:
    list<PCENNode*> PStack;
public:
    PCEN_MRN(in_addr src, in_addr dest, u_int8_t sw_type, u_int8_t enc_type, float bw, u_int32_t opts, u_int32_t lspq_id, u_int32_t msg_seqnum, u_int32_t vtag = 0);
    virtual ~PCEN_MRN();
    bool IsLoop(list<PCENLink*> &path, PCENNode* new_node);
    bool IsCrossingRegionBoundary(PCENLink* pcen_link, TSpec& tspec);
    int GetNextRegionTspec(PCENLink* pcen_link, TSpec& tspec);

    virtual int PerformComputation();
    void PostBuildTopology();
    virtual void Run();
};

#endif

