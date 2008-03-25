/*
 * Copyright (c) 2007
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2007
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

#include "log.hh"
#include "lsa.hh"
#include "toolbox.hh"
#include "lsp_broker.hh"
#include "rce_apiclient.hh"
#include "narb_config.hh"


RCE_APIClient::~RCE_APIClient()
{
    //####//Close();
}

int RCE_APIClient::HandleMessage(api_msg * rce_msg)
{
    ero_subobj * subobj, * new_subobj;
    LSPQ* lspq;
    switch (rce_msg->header.type_8)
    {
     case MSG_LSP:
        if (rce_msg->header.action == ACT_ERROR)
        {
            LOGF("LSP query return error code %d\n", ntohl(*(u_int32_t*)(rce_msg->body + sizeof(te_tlv_header))));
        }
        else if (rce_msg->header.action == ACT_ACKDATA)
        {
            LOGF("RCE_APIClient::ReadMessage return ERO with %d hops\n", 
                ntohs(((te_tlv_header*)rce_msg->body)->length)/sizeof(ero_subobj));
        }

        lspq = FindOwnerLSPQLatest(rce_msg);
        if (!lspq)
        {
            api_msg_delete(rce_msg);
            return -1;
        }
        lspq->HandleRCEReply(rce_msg);
        return 0;
        break;
     case MSG_AAA:
        break;
     case MSG_RM:
        break;
    default:
        LOGF("Unknown msg type %d \n", rce_msg->header.type_8);
         api_msg_delete(rce_msg);
        return -1;
        break;
    }

    return 0;
}

bool RCE_APIClient::IsMatched(char* host, int port)
{
    if (_host == host && _port == port)
        return true;

    return false;
}


void RCE_APIClient::QueryLsp (msg_narb_cspf_request &cspf_req, u_int32_t ucid, u_int32_t options, u_int32_t vtag, u_int32_t hop_back, u_int32_t src_lcl_id, u_int32_t dest_lcl_id, 
    msg_narb_vtag_mask* vtag_bitmask, list<ero_subobj*>* p_subnet_ero)
{
    api_msg *rce_msg;
    char buf[1024];
    memcpy(buf, &(cspf_req.app_req_data), sizeof(cspf_req.app_req_data));
    if ((options & LSP_OPT_VTAG_MASK) && vtag_bitmask != NULL)
        memcpy(buf+sizeof(cspf_req.app_req_data), vtag_bitmask, sizeof(msg_narb_vtag_mask));
    u_int16_t mlen = sizeof(cspf_req.app_req_data) + (vtag_bitmask == NULL? 0 : sizeof(msg_narb_vtag_mask));
    if (cspf_req.lspb_id != 0)
    {
        msg_narb_lspb_id* lspb_id_tlv = (msg_narb_lspb_id*)(buf+mlen);
        lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspb_id_tlv->length = htons(sizeof(msg_narb_lspb_id) - TLV_HDR_SIZE);
        lspb_id_tlv->lspb_id = cspf_req.lspb_id;
        mlen += sizeof(msg_narb_lspb_id);
    }  
    if (hop_back != 0)
    {
        msg_narb_hop_back* hop_back_tlv = (msg_narb_hop_back*)(buf+mlen);
        hop_back_tlv->type = htons(TLV_TYPE_NARB_HOP_BACK);
        hop_back_tlv->length = htons(sizeof(msg_narb_hop_back) - TLV_HDR_SIZE);
        hop_back_tlv->ipv4 = hop_back;
        mlen += sizeof(msg_narb_hop_back);
    }
    if (src_lcl_id != 0 || dest_lcl_id != 0)
    {
        msg_narb_local_id* lcl_id_tlv = (msg_narb_local_id*)(buf+mlen);
        lcl_id_tlv->type = htons(TLV_TYPE_NARB_LOCAL_ID);
        lcl_id_tlv->length = htons(sizeof(msg_narb_local_id) - TLV_HDR_SIZE);
        lcl_id_tlv->lclid_src = htonl(src_lcl_id);
        lcl_id_tlv->lclid_dest = htonl(dest_lcl_id);
        mlen += sizeof(msg_narb_local_id);
    }
    if (p_subnet_ero != NULL && p_subnet_ero->size() > 0)
    {
        api_msg *subnet_ero_msg = narb_new_msg_reply_ero(0, 0, *p_subnet_ero, NULL, 0);
        memcpy(buf + mlen, subnet_ero_msg->body, ntohs(subnet_ero_msg->header.length));
        te_tlv_header* tlv = (te_tlv_header*)(buf + mlen);
        tlv->type = htons(TLV_TYPE_NARB_SUBNET_ERO);
        mlen += ntohs(subnet_ero_msg->header.length);
        options |= LSP_OPT_SUBNET_DTL;
    }
    rce_msg = api_msg_new((u_char)MSG_LSP, (u_char)ACT_QUERY, mlen, buf, ucid, cspf_req.app_seqnum, vtag);
    rce_msg->header.options = htonl(options);

    SendMessage(rce_msg); 
}

void RCE_APIClient::QueryLsp_MRN (msg_narb_cspf_request &cspf_req, msg_app2narb_request &mrn_spec, u_int32_t ucid, u_int32_t options, u_int32_t vtag, u_int32_t hop_back, msg_narb_vtag_mask* vtag_bitmask)
{
    api_msg *rce_msg;
    char buf[1024];
    memcpy(buf, &(cspf_req.app_req_data), sizeof(cspf_req.app_req_data));
    memcpy(buf+sizeof(cspf_req.app_req_data), &mrn_spec, sizeof(mrn_spec));
    u_int16_t mlen = sizeof(msg_app2narb_request)*2;
    if ((options & LSP_OPT_VTAG_MASK) && vtag_bitmask != NULL) 
    {
        memcpy(buf+mlen, vtag_bitmask, sizeof(msg_narb_vtag_mask));
        mlen += sizeof(msg_narb_vtag_mask);
    }
    if (cspf_req.lspb_id != 0)
    {
        msg_narb_lspb_id* lspb_id_tlv = (msg_narb_lspb_id*)(buf+mlen);
        lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspb_id_tlv->length = htons(sizeof(msg_narb_lspb_id) - TLV_HDR_SIZE);
        lspb_id_tlv->lspb_id = cspf_req.lspb_id;
        mlen += sizeof(msg_narb_lspb_id);
    }  
    if (hop_back != 0)
    {
        msg_narb_hop_back* hop_back_tlv = (msg_narb_hop_back*)(buf+mlen);
        hop_back_tlv->type = htons(TLV_TYPE_NARB_HOP_BACK);
        hop_back_tlv->length = htons(sizeof(msg_narb_hop_back) - TLV_HDR_SIZE);
        hop_back_tlv->ipv4 = hop_back;
        mlen += sizeof(msg_narb_hop_back);
    }

    rce_msg = api_msg_new((u_char)MSG_LSP, (u_char)ACT_QUERY_MRN, mlen, buf, ucid, cspf_req.app_seqnum, vtag);
    rce_msg->header.options = htonl(options);

    SendMessage(rce_msg); 
}

void RCE_APIClient::NotifyResvStateWithERO(u_int8_t type, u_int8_t action, msg_app2narb_request* msg_req, list<ero_subobj*>& ero_forward, 
    list<ero_subobj*>& subnet_ero, list<dtl_hop>& subnet_dtl,  u_int32_t ucid, u_int32_t seqnum, u_int32_t options, u_int32_t msgtag, u_int32_t src_lcl_id, u_int32_t dest_lcl_id, 
    msg_narb_vtag_mask* vtag_mask_tlv, u_int32_t holding_time)
{
    api_msg *rce_msg;
    char buf[1500];
    u_int16_t length = sizeof(msg_app2narb_request);
    memcpy(buf, msg_req, length);
    ((msg_app2narb_request*)buf)->type = ((type<<8) | action);
    api_msg *ero_msg = narb_new_msg_reply_ero(0, 0, ero_forward, NULL, 0);
    memcpy(buf + length, ero_msg->body, ntohs(ero_msg->header.length));
    length += ntohs(ero_msg->header.length);
    api_msg_delete(ero_msg);
    if (vtag_mask_tlv)
    {
        memcpy(buf + length, vtag_mask_tlv, sizeof(msg_narb_vtag_mask));
        length += sizeof(msg_narb_vtag_mask);
    }
    if (src_lcl_id != 0 || dest_lcl_id !=0)
    {
        msg_narb_local_id* local_id_tlv = (msg_narb_local_id*)(buf + length);
        local_id_tlv->type = htons(TLV_TYPE_NARB_LOCAL_ID);
        local_id_tlv->length = htons(sizeof(u_int32_t)*2);
        local_id_tlv->lclid_src= htonl(src_lcl_id);
        local_id_tlv->lclid_dest= htonl(dest_lcl_id);
        length += sizeof(msg_narb_local_id);
    }
    if (subnet_ero.size() > 0) //forward subnet_ero
    {
        api_msg *subnet_ero_msg = narb_new_msg_reply_ero(0, 0, subnet_ero, NULL, 0);
        memcpy(buf + length, subnet_ero_msg->body, ntohs(subnet_ero_msg->header.length));
        te_tlv_header* tlv = (te_tlv_header*)(buf + length);
        tlv->type = htons(TLV_TYPE_NARB_SUBNET_ERO);
        length += ntohs(subnet_ero_msg->header.length);
    }
    else if (subnet_dtl.size() > 0) // OR subnet_dtl (either should work)
    {
        msg_narb_subnet_dtl* subnet_dtl_tlv = (msg_narb_subnet_dtl*)(buf + length);
        subnet_dtl_tlv->type = htons(TLV_TYPE_NARB_SUBNET_DTL);
        subnet_dtl_tlv->length = htons(sizeof(struct dtl_hop)*subnet_dtl.size());
        list<dtl_hop>::iterator it = subnet_dtl.begin();
        for (int i = 0; it != subnet_dtl.end(); it++, i++)
            subnet_dtl_tlv->hops[i] = (*it);
        length += (sizeof(te_tlv_header)+sizeof(struct dtl_hop)*subnet_dtl.size());
    }
    if (holding_time != 0)
    {
        msg_narb_holding_time* holding_time_tlv = (msg_narb_holding_time*)(buf + length);
        holding_time_tlv->type = htons(TLV_TYPE_NARB_HOLDING_TIME);
        holding_time_tlv->length = htons(sizeof(u_int32_t));
        holding_time_tlv->seconds = htonl(holding_time);
        length += sizeof(msg_narb_holding_time);
    }
    rce_msg = api_msg_new(type, action, length, buf, ucid, seqnum, msgtag);
    rce_msg->header.options = htonl(options);
    SendMessage(rce_msg);    
}

///////////////////// RCE_APIClient_Factory /////////////////////  
RCE_APIClient_Factory RCE_APIClient_Factory::factory;

RCE_APIClient_Factory::~RCE_APIClient_Factory()
{
    list<RCE_APIClient*>::iterator it;
    if (clients.size() <= 0)
        return;
    for (it = clients.begin(); it != clients.end(); it++)
        if ((*it))
            delete *it;
    clients.clear();
}
    
RCE_APIClient * RCE_APIClient_Factory::Create(char *host, int port)
{
    RCE_APIClient * client = new RCE_APIClient(host, port);
    clients.push_back(client);

    return client;
}

RCE_APIClient * RCE_APIClient_Factory::GetClient(char *host, int port)
{
    list<RCE_APIClient*>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
        if ((*it) && (*it)->IsMatched(host, port))
        {
            if ((*it)->IsAlive())
                return *it;                
            clients.erase(it);
            delete (*it);
            break;
        }

    return Create(host, port);
}

void RCE_APIClient_Factory::RemoveClient(RCE_APIClient * client)
{
    list<RCE_APIClient*>::iterator it;
    for (it = clients.begin(); it != clients.end(); it++)
        if ((*it) == client)
        {
            eventMaster.Remove(client);
            clients.erase(it);
            break;
        }
}

