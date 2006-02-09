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

#include "toolbox.hh"
#include "event.hh"
#include "rce_pcen.hh"
#include "pcen_ksp.hh"
#include "pcen_mrn.hh"
#include "rce_lsp.hh"

void LSPHandler::Load(api_msg *msg)
{
    assert(msg);

    lspq_id = ntohl(msg->hdr.ucid);
    seqnum = ntohl(msg->hdr.msgseq);
    options = ntohl(msg->hdr.msgtag[0]);
    tag = ntohl(msg->hdr.msgtag[1]);
    te_tlv_header * tlv = (te_tlv_header*)msg->body;

    if (options & LSP_TLV_NARB_REQ)
    {
        narb_lsp_request_tlv*  narb_req_tlv = (narb_lsp_request_tlv*)tlv;
        tlv += sizeof(narb_lsp_request_tlv);
        narb_lsp_request_tlv*  mrn_spec_tlv = (narb_lsp_request_tlv*)tlv;

        switch (msg->hdr.action)
        {
        case ACT_QUERY:
            source = narb_req_tlv->src;
            destination = narb_req_tlv->dest;
            encoding_type_ingress = encoding_type_egress = narb_req_tlv->encoding_type;
            switching_type_ingress = switching_type_egress = narb_req_tlv->switching_type;
            bandwidth_ingress = bandwidth_egress = narb_req_tlv->bandwidth;
            break;
        case ACT_QUERY_MRN:
            source = mrn_spec_tlv->src;
            destination = narb_req_tlv->dest;
            encoding_type_ingress = mrn_spec_tlv->encoding_type; 
            encoding_type_egress = narb_req_tlv->encoding_type;
            switching_type_ingress = mrn_spec_tlv->switching_type;
            switching_type_egress = narb_req_tlv->switching_type;
            bandwidth_ingress = mrn_spec_tlv->bandwidth;
            bandwidth_egress = narb_req_tlv->bandwidth;            
            break;
        }
    }
    // continue to get second tlv if applicable ...
    api_msg_delete(msg);
}

void LSPHandler::Run()
{
    PCEN * pcen_event;
    //@@@@ Enforce all requests to be handled by MRN routing module
    options |= LSP_OPT_MRN;
    if ((options & LSP_OPT_MRN) == 0)
    {
        pcen_event = new PCEN(source, destination, switching_type_egress, encoding_type_egress, bandwidth_egress, options, lspq_id, seqnum, tag);
    }
    else
    {
        pcen_event = new PCEN_MRN(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress, 
            switching_type_egress, encoding_type_egress, bandwidth_egress, options, lspq_id, seqnum, tag);
    }

    pcen_event->AssociateWriter (api_writer);
    pcen_event->SetAutoDelete(true);
    eventMaster.Schedule(pcen_event);
}

