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

        lspq = FindOwnerLSPQ(rce_msg);
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
        LOGF("Unkonwn msg type %d \n", rce_msg->header.type_8);
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


void RCE_APIClient::QueryLsp (msg_narb_cspf_request &cspf_req, u_int32_t ucid, u_int32_t options, u_int32_t vtag, u_int32_t hop_back, msg_app2narb_vtag_mask* vtag_bitmask)
{
    api_msg *rce_msg;
    char buf[1024];
    memcpy(buf, &(cspf_req.app_req_data), sizeof(cspf_req.app_req_data));
    if ((options & LSP_OPT_VTAG_MASK) && vtag_bitmask != NULL)
        memcpy(buf+sizeof(cspf_req.app_req_data), vtag_bitmask, sizeof(msg_app2narb_vtag_mask));
    u_int16_t mlen = sizeof(cspf_req.app_req_data) + (vtag_bitmask == NULL? 0 : sizeof(msg_app2narb_vtag_mask));
    if (hop_back != 0)
    {
        msg_app2narb_hop_back* hop_back_tlv = (msg_app2narb_hop_back*)(buf+mlen);
        hop_back_tlv->type = htons(TLV_TYPE_NARB_HOP_BACK);
        hop_back_tlv->length = htons(sizeof(msg_app2narb_hop_back)-4);
        hop_back_tlv->ipv4 = hop_back;
        mlen += sizeof(msg_app2narb_hop_back);
    }

    rce_msg = api_msg_new((u_char)MSG_LSP, (u_char)ACT_QUERY, mlen, buf, ucid, cspf_req.app_seqnum, vtag);
    rce_msg->header.options = htonl(options);

    SendMessage(rce_msg); 
}

void RCE_APIClient::QueryLsp_MRN (msg_narb_cspf_request &cspf_req, msg_app2narb_request &mrn_spec, u_int32_t ucid, u_int32_t options, u_int32_t vtag, u_int32_t hop_back, msg_app2narb_vtag_mask* vtag_bitmask)
{
    api_msg *rce_msg;
    char buf[1024];
    memcpy(buf, &(cspf_req.app_req_data), sizeof(cspf_req.app_req_data));
    memcpy(buf+sizeof(cspf_req.app_req_data), &mrn_spec, sizeof(mrn_spec));
    u_int16_t mlen = sizeof(msg_app2narb_request)*2;
    if ((options & LSP_OPT_VTAG_MASK) && vtag_bitmask != NULL) 
    {
        memcpy(buf+mlen, vtag_bitmask, sizeof(msg_app2narb_vtag_mask));
        mlen += sizeof(msg_app2narb_vtag_mask);
    }
    if (hop_back != 0)
    {
        msg_app2narb_hop_back* hop_back_tlv = (msg_app2narb_hop_back*)(buf+mlen);
        hop_back_tlv->type = htons(TLV_TYPE_NARB_HOP_BACK);
        hop_back_tlv->length = htons(sizeof(msg_app2narb_hop_back)-4);
        hop_back_tlv->ipv4 = hop_back;
        mlen += sizeof(msg_app2narb_hop_back);
    }

    rce_msg = api_msg_new((u_char)MSG_LSP, (u_char)ACT_QUERY_MRN, mlen, buf, ucid, cspf_req.app_seqnum, vtag);
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


