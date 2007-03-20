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
#include "toolbox.hh"
#include "narb_apiclient.hh"
#include "narb_config.hh"
#include "dts.hh"


NARB_APIClient::~NARB_APIClient()
{
    if (api_writer)
    {
        delete api_writer;
        api_writer = NULL;
    }
}

int NARB_APIClient::HandleMessage(api_msg* msg)
{
    switch (ntohs(msg->header.type))
    {
     case MSG_REPLY_ERO:
     case MSG_REPLY_ERROR:
        {
            LSPQ * lspq = FindOwnerLSPQ(msg);
            if (!lspq)
            {
                api_msg_delete(msg);
                return -1;
            }
            lspq->HandleNextHopNARBReply(msg);
            return 0;
        }
        break;
    case MSG_REPLY_REMOVE_CONFIRM:
        LOGF("Peer NARB release confirmation (ucid = 0x%x, seqnp = 0x%x)  %d \n", ntohl(msg->header.ucid), htonl(msg->header.seqnum));
        api_msg_delete(msg);
        break;
    default:
        LOGF("Peer NARB returned unkonwn msg type %d \n", ntohs(msg->header.type));
         api_msg_delete(msg);
        return -1;
        break;
    }

    return 0;
}

bool NARB_APIClient::IsMatched(char* host, int port)
{
    if (_host == host && _port == port)
        return true;

    return false;
}

bool NARB_APIClient::IsAssociated(in_addr ip)
{
    list<in_addr>::iterator it;
    for (it = associated_addrs.begin(); it != associated_addrs.end(); it++)
        if ((*it).s_addr == ip.s_addr)
            return true;

    return false;
}

void NARB_APIClient::AssociateTEAddr(in_addr ip)
{
    if (IsAssociated(ip))
        return;

    associated_addrs.push_back(ip);
}

void NARB_APIClient::QueryLspRecursive (msg_narb_recursive_cspf_request &rec_cspf_req, u_int32_t options, u_int32_t vtag, msg_app2narb_vtag_mask* vtag_mask)
{
    char msgbody[1024];
    api_msg *narb_msg;
    int msglen = sizeof(rec_cspf_req) - 12;
    memcpy(msgbody, ((char*)&rec_cspf_req) + 12, msglen);
    if (vtag_mask && (options & LSP_OPT_REQ_ALL_VTAGS)) 
   {
        memcpy(msgbody+msglen, vtag_mask, sizeof(msg_app2narb_vtag_mask));
        msglen += sizeof(msg_app2narb_vtag_mask);
        options |= LSP_OPT_VTAG_MASK;
        vtag = ANY_VTAG;
    }
    
    narb_msg = api_msg_new(NARB_MSG_LSPQ, msglen, (void*)msgbody, rec_cspf_req.lspb_id, rec_cspf_req.app_seqnum, vtag);
    narb_msg->header.options = htonl(options);
    SendMessage(narb_msg); 
}

void NARB_APIClient::RelayMessageToPeer(u_int16_t type, api_msg* msg, list<ero_subobj*>& ero_confirm)
{
    api_msg *narb_msg;
    char buf[1500];
    u_int16_t length = sizeof(msg_app2narb_request);
    memcpy(buf, msg->body, length);
    ((msg_app2narb_request*)buf)->type = htons(type);
    api_msg *ero_msg = narb_new_msg_reply_ero(0, 0, ero_confirm, NULL);
    memcpy(buf + length, ero_msg->body, ntohs(ero_msg->header.length));
    length += ntohs(ero_msg->header.length);
    narb_msg = api_msg_new(NARB_MSG_LSPQ, length, buf, ntohl(msg->header.ucid), ntohl(msg->header.seqnum), ntohl(msg->header.tag));
    SendMessage(narb_msg);    
}

///////////////////// NARB_APIClient_Factory /////////////////////  
NARB_APIClient_Factory NARB_APIClient_Factory::factory;

NARB_APIClient_Factory::~NARB_APIClient_Factory()
{
    list<NARB_APIClient*>::iterator it;
    if (peers.size() <= 0)
        return;
    for (it = peers.begin(); it != peers.end(); it++)
        if ((*it))
            delete *it;
    peers.clear();
}
    
NARB_APIClient * NARB_APIClient_Factory::Create(char *host, int port)
{
    NARB_APIClient * client = new NARB_APIClient(host, port);
    peers.push_back(client);

    return client;
}

NARB_APIClient * NARB_APIClient_Factory::GetClient(char *host, int port)
{
    list<NARB_APIClient*>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++)
        if ((*it) && (*it)->IsMatched(host, port))
        {
            if (!(*it)->Obsolete())
                return *it;                
            peers.erase(it);
            delete (*it);
            break;
        }

    return Create(host, port);
}

NARB_APIClient * NARB_APIClient_Factory::GetClient(in_addr ip)
{
    if_narb_info * narb = NarbDomainInfo.LookupNarbByRmtIf(ip);
    if (!narb)
        return NULL;
    list<NARB_APIClient*>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++)
        if ((*it) && (*it)->IsMatched(narb->addr, narb->port))
        {
            if (!(*it)->Obsolete())
                return *it;                
            peers.erase(it);
            delete (*it);
            break;
        }

    return Create(narb->addr, narb->port);
}


void NARB_APIClient_Factory::RemoveClient(NARB_APIClient * client)
{
    list<NARB_APIClient*>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++)
        if ((*it) == client)
        {
            peers.erase(it);
            break;
        }
}

NARB_APIClient * NARB_APIClient_Factory::Lookup(in_addr ip)
{
    list<NARB_APIClient*>::iterator it;
    for (it = peers.begin(); it != peers.end(); it++)
        if ((*it) && (*it)->IsAssociated(ip))
            return *it;

    return NULL;
}

