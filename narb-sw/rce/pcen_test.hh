/*
 * Copyright (c) 2008
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2008
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
 
#ifndef __RCE_TEST_HH__
#define __RCE_TEST_HH__

#include <iostream>
#include <vector>
#include <list>
#include "rce_pcen.hh"
#include "pcen_ksp.hh"

using namespace std;

class PathT;
class PCENNode;
class PCEN_TEST: public PCEN_KSP
{
protected:
    list<PCENLink*> links_backup;

public:
    PCEN_TEST(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                 u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN_KSP(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) {}
    virtual ~PCEN_TEST() {}

    virtual bool PostBuildTopology();
    virtual void Run();

    PathT* SearchUpperLayerPath(PCENNode*  src_node, PCENNode*  dest_node);
    PathT* SearchLowerLayerPath(PCENNode*  src_node, PCENNode*  dest_node);
};

#endif

