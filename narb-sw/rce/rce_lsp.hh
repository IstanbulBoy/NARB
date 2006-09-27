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

#ifndef __RCE_LSP_H__
#define __RCE_LSP_H__
#include "rce_api.hh"

// TLV's wrapped in the LSP querry message.
// The types of wrapped TLV's are indicated in msgtag[0].
#define LSP_TLV_NARB_REQ    0x01
// LSP computation options
#define LSP_OPT_STRICT  ((u_int32_t)(0x01 << 16)) //otherwise LSP_OPT_LOOSE
#define LSP_OPT_PREFERRED ((u_int32_t)(0x02 << 16)) //otherwise LSP_OPT_ONLY
#define LSP_OPT_MRN ((u_int32_t)(0x04 << 16))
//#define LSP_OPT_MRN_RELAY ((u_int32_t)0x08 << 16)  //relay path computation for multiregion multidomain networks
#define LSP_OPT_BIDIRECTIONAL ((u_int32_t)(0x10 << 16))
#define LSP_OPT_E2E_VTAG  ((u_int32_t)(0x20 << 16)) //otherwise Untgged VLAN for E2E Ethernet
#define LSP_OPT_VIA_MOVAZ  ((u_int32_t)(0x40 << 16)) //using MOVAZ proprietary TLVs for optical layer routing

#define ANY_VTAG 0xffff  //Indicating that LSP uses any available E2E VLAN Tag
#define ANY_WAVE 0xffff  //Indicating that LSP uses any available Wavelength for optical layer routing

struct narb_lsp_request_tlv
{
    u_int16_t type;
    u_int16_t length;
    struct in_addr src;
    struct in_addr dest;
    u_int8_t  encoding_type;
    u_int8_t  switching_type;
    u_int16_t gpid;
    float bandwidth;
};

class LSPHandler: public Event
{
private:
    in_addr source;
    in_addr destination;
    u_int8_t  encoding_type_ingress;
    u_int8_t  switching_type_ingress;   
    float bandwidth_ingress;
    u_int8_t  encoding_type_egress;
    u_int8_t  switching_type_egress;   
    float bandwidth_egress;

    u_int32_t options;
    u_int32_t tag;
    u_int32_t lspq_id;
    u_int32_t seqnum;
    u_int32_t uptime;
    u_int32_t duration;

    int caller_fd;

    APIWriter* api_writer;
    void Init ()
        {  source.s_addr = 0; destination.s_addr = 0; encoding_type_ingress = encoding_type_egress = 0; 
            switching_type_ingress = switching_type_egress = 0; bandwidth_ingress = bandwidth_egress =0;
            options = 0; tag = 0; lspq_id = seqnum = 0xffffffff; uptime = 0; duration = 0xffffffff; 
            api_writer = NULL;  }
public:
    LSPHandler(int fd): caller_fd(fd) { Init();}
    virtual ~LSPHandler() {}
    void Load(api_msg *msg);
    virtual void Run();
    void AssociateWriter (APIWriter *writer) { assert (writer); api_writer = writer; }
};

#endif
