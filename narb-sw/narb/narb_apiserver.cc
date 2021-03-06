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
#include <arpa/inet.h>
#include "narb_apiserver.hh"
#include "lsp_broker.hh"

list<LSP_Broker*> NARB_APIServer::lsp_brokers;

NARB_APIServer::~NARB_APIServer()
{
    list<LSP_Broker*>::iterator it = lsp_brokers.begin();
    while (it != lsp_brokers.end())
   	{
        if ((*it)->Obsolete())
        {
            delete (*it);
            it = lsp_brokers.erase(it);
			continue;
        }
		it++;
   	}
}

void NARB_APIServer::Run()
{
    struct sockaddr_in sa_in;
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    socklen_t len = sizeof(struct sockaddr_in);
    int new_sock = accept (fd, (struct sockaddr *)&sa_in, &len);
    if (new_sock < 0)
    {
        LOGF("NARB_APIServer::Run: cannot accept socket on %d\n", fd);
        return;
    }

    LSP_Broker* lspb = new LSP_Broker(new_sock, this);
    lspb->SetAutoDelete(true);
    lspb->SetRepeats(FOREVER);

    lsp_brokers.push_back(lspb);
    eventMaster.Schedule(lspb);

    char addr[20];
    LOGF("Accepted API connection from %s port %d on socket %d for LSP Query\n",
        inet_ntop(AF_INET, &sa_in.sin_addr, addr, sizeof(addr)),
        ntohs(sa_in.sin_port), new_sock);
}

LSP_Broker* NARB_APIServer::LspBrokerLookup (u_int32_t id)
{
    list<LSP_Broker*>::iterator it;
    LSP_Broker *broker;

    for (it = lsp_brokers.begin(); it != lsp_brokers.end(); it++)
    {
        broker = *it;
        if (!broker)
            continue;

        if (id == broker->LspbId() && !broker->Obsolete())
            return broker;
    }

    return NULL;
}

LSP_Broker* NARB_APIServer::LspBrokerLookupBySocket (int sock)
{
    list<LSP_Broker*>::iterator it;

    assert(sock > 0);
    for (it = lsp_brokers.begin(); it != lsp_brokers.end(); it++)
    {
        if (!(*it))
            continue;

        if (sock == (*it)->Socket() && !(*it)->Obsolete())
            return *it;
    }

    return NULL;
}

LSPQ* NARB_APIServer::LspqLookup (u_int32_t ucid, u_int32_t seqnum, u_int32_t lspb_id)
{
    list<LSP_Broker*>::iterator it1;
    LSP_Broker *broker;
    LSPQ* lspq;

    for (it1 = lsp_brokers.begin(); it1 != lsp_brokers.end(); it1++)
    {
        broker = *it1;
        if (!broker || (lspb_id != 0 && broker->LspbId() != lspb_id))
            continue;
        if ((lspq = broker->LspqLookup(ucid, seqnum)) != NULL)
            return lspq;
    }

    return NULL;
}

LSPQ* NARB_APIServer::LspqLookupLatest (u_int32_t ucid, u_int32_t seqnum, u_int32_t lspb_id)
{
    list<LSP_Broker*>::reverse_iterator it1;
    LSP_Broker *broker;
    LSPQ* lspq;

    for (it1 = lsp_brokers.rbegin(); it1 != lsp_brokers.rend(); it1++)
    {
        broker = *it1;
        if (!broker || (lspb_id != 0 && broker->LspbId() != lspb_id))
            continue;
        if ((lspq = broker->LspqLookup(ucid, seqnum)) != NULL)
            return lspq;
    }

    return NULL;
}

LSPQ* NARB_APIServer::LspqLookupByReqSpec(u_int32_t ucid, u_int32_t seqnum, msg_app2narb_request* req_spec, msg_app2narb_request* mrn_spec)
{
    list<LSP_Broker*>::iterator it1;
    LSP_Broker *broker;
    LSPQ* lspq;

    for (it1 = lsp_brokers.begin(); it1 != lsp_brokers.end(); it1++)
    {
        broker = *it1;
        if (broker && (lspq = broker->LspqLookupByReqSpec(ucid, seqnum, req_spec, mrn_spec)) != NULL)
            return lspq;
    }

    return NULL;
}


/*
int NARB_APIServer::LspqCount (u_int32_t ucid, u_int32_t seqnum)
{
    list<LSP_Broker*>::iterator it1;
    LSP_Broker *broker;
    LSPQ* lspq;
    int ret = 0;

    for (it1 = lsp_brokers.begin(); it1 != lsp_brokers.end(); it1++)
    {
        broker = *it1;
        if (!broker)
            continue;
        if ((lspq = broker->LspqLookup(ucid, seqnum)) != NULL)
            ret++;
    }

    return ret;
}
*/

api_msg * narb_new_msg_reply_error (u_int32_t ucid, u_int32_t seqnr, u_int32_t errorcode, u_int32_t prev_lspb_id)
{
    api_msg *msg;

    char buf[16];
    int mlen = 0;
    te_tlv_header *tlv = (te_tlv_header*)buf;
    tlv->type = htons(TLV_TYPE_NARB_ERROR_CODE); 
    tlv->length = htons(4);
    * (u_int32_t *)((char *)tlv + 4) = htonl(errorcode);
    mlen += 8;

    if (prev_lspb_id != 0)
    {
        msg_narb_lspb_id* lspq_id_tlv = (msg_narb_lspb_id *)((char *)tlv + mlen);
        lspq_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspq_id_tlv->length = htons(sizeof(msg_narb_lspb_id) - TLV_HDR_SIZE);
        lspq_id_tlv->lspb_id = prev_lspb_id;
        mlen += sizeof(msg_narb_lspb_id);
    }

    msg = api_msg_new (MSG_REPLY_ERROR, mlen, buf, ucid, seqnr);
  
    return msg;
}

api_msg * narb_new_msg_reply_release_confirm (u_int32_t ucid, u_int32_t seqnr, u_int32_t prev_lspb_id)
{
    api_msg *msg;

    char buf[8];
    int mlen = 0;
    if (prev_lspb_id != 0)
    {
        msg_narb_lspb_id *lspb_id_tlv = (msg_narb_lspb_id*)(buf + mlen);
        lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspb_id_tlv->length = htons(sizeof(msg_narb_lspb_id) - TLV_HDR_SIZE);
        lspb_id_tlv->lspb_id = prev_lspb_id;
        mlen += sizeof(msg_narb_lspb_id);
    }
     
    msg = api_msg_new (MSG_REPLY_REMOVE_CONFIRM, mlen, buf, ucid, seqnr);
  
    return msg;
}

static int make_ero_tlv_from_list(te_tlv_header* tlv, list<ero_subobj*>& ero, int offset)
{
    int subobj_size;
    int tlv_offset = sizeof(te_tlv_header);

    list<ero_subobj*>::iterator it;
    for (it = ero.begin(); it != ero.end(); it++)
    {
        ero_subobj * subobj_narb = *it;
        if (subobj_narb->l2sc_vlantag == 0)
        {
            if (subobj_narb->if_id != 0)
            {
                unum_if_subobj * subobj_unum = (unum_if_subobj *)((char *)tlv + tlv_offset);
                subobj_unum->l_and_type = L_AND_TYPE(subobj_narb->hop_type, 0x04);
                subobj_unum->length = sizeof(unum_if_subobj);
                subobj_unum->addr.s_addr = subobj_narb->addr.s_addr;
                subobj_unum->ifid = subobj_narb->if_id;
                subobj_unum->resvd[0] = subobj_unum->resvd[1] = 0;
                subobj_size = sizeof(unum_if_subobj);
            }
            else
            {
                ipv4_prefix_subobj * subobj_ipv4 = (ipv4_prefix_subobj *)((char *)tlv + tlv_offset);
                subobj_ipv4->l_and_type = L_AND_TYPE(subobj_narb->hop_type, 0x01);
                memcpy(subobj_ipv4->addr, &subobj_narb->addr, 4);
                subobj_ipv4->length = sizeof(ipv4_prefix_subobj);
                subobj_ipv4->prefix_len = 32;
                subobj_ipv4->resvd = 0;
                subobj_size = sizeof(ipv4_prefix_subobj);
            }
        }
        else
        {//Generating UNumIf Subobjects for E2E Tagged VLAN 
            assert (subobj_narb->l2sc_vlantag != ANY_VTAG);
            unum_if_subobj * subobj_unum = (unum_if_subobj *)((char *)tlv + tlv_offset);
            subobj_unum->l_and_type = L_AND_TYPE(subobj_narb->hop_type, 0x04);
            subobj_unum->length = sizeof(unum_if_subobj);
            subobj_unum->addr.s_addr = subobj_narb->addr.s_addr;
            //#define LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL (u_int16_t)0x4
            //subobj_unum->ifid = htonl((0x4 << 16) |(*(u_int16_t*)subobj_narb->pad));
            subobj_unum->ifid = htonl((0x4 << 16) |(subobj_narb->l2sc_vlantag));
            subobj_unum->resvd[0] = subobj_unum->resvd[1] = 0;
            subobj_size = sizeof(unum_if_subobj);
        }
        tlv_offset += subobj_size;
    }

    tlv->length = htons(tlv_offset - TLV_HDR_SIZE);
    tlv->type = htons(TLV_TYPE_NARB_ERO);
    offset += tlv_offset;

    return offset;
}

api_msg * narb_new_msg_reply_ero (u_int32_t ucid, u_int32_t seqnr, list<ero_subobj*>& ero, 
    msg_narb_vtag_mask* vtagmask, u_int32_t prev_lspb_id, list<ero_subobj*>* p_subnet_ero, list<dtl_hop>* p_subnet_dtl,
    list< list<ero_subobj*> >* p_alt_eros, list< list<ero_subobj*> >* p_alt_subnet_eros)
{
    api_msg *msg;
    int offset;
    int subobj_size;
    char buf[API_MAX_MSG_SIZE];
    te_tlv_header * tlv = (te_tlv_header *)buf;

    if (ero.size() ==0)
        return NULL;

    offset = make_ero_tlv_from_list(tlv, ero, 0);

    if (vtagmask)
    {
        memcpy(buf+offset, vtagmask, sizeof(msg_narb_vtag_mask));
        offset += sizeof(msg_narb_vtag_mask);
    }

    if (prev_lspb_id != 0)
    {
        msg_narb_lspb_id* lspq_id_tlv = (msg_narb_lspb_id *)(buf+offset);
        lspq_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspq_id_tlv->length = htons(sizeof(msg_narb_lspb_id) - TLV_HDR_SIZE);
        lspq_id_tlv->lspb_id = prev_lspb_id;
        offset += sizeof(msg_narb_lspb_id);
    }

    if (p_subnet_ero != NULL && p_subnet_ero->size() > 0)
    {
        te_tlv_header* subnet_ero_tlv = (te_tlv_header *)(buf+offset);
        offset = make_ero_tlv_from_list(subnet_ero_tlv, *p_subnet_ero, offset);
        subnet_ero_tlv->type = htons(TLV_TYPE_NARB_SUBNET_ERO);
    }

    if (p_alt_eros != NULL && p_alt_eros->size() > 0)
    {
        list< list<ero_subobj*> >::iterator it = p_alt_eros->begin();
        for (; it != p_alt_eros->end(); it++) {
            te_tlv_header* alt_ero_tlv = (te_tlv_header *)(buf+offset);
            offset = make_ero_tlv_from_list(alt_ero_tlv, (*it), offset);
            alt_ero_tlv->type = htons(TLV_TYPE_NARB_ALTERNATE_ERO);
        }
    }

    if (p_alt_subnet_eros != NULL && p_alt_subnet_eros->size() > 0)
    {
        list< list<ero_subobj*> >::iterator it = p_alt_subnet_eros->begin();
        for (; it != p_alt_subnet_eros->end(); it++) {
            te_tlv_header* alt_subnet_ero_tlv = (te_tlv_header *)(buf+offset);
            offset = make_ero_tlv_from_list(alt_subnet_ero_tlv, (*it), offset);
            alt_subnet_ero_tlv->type = htons(TLV_TYPE_NARB_ALTERNATE_SUBNET_ERO);
        }
    }

    if (p_subnet_dtl != NULL && p_subnet_dtl->size() > 0)
    {
        msg_narb_subnet_dtl* subnet_dtl_tlv = (msg_narb_subnet_dtl *)(buf+offset);
        subnet_dtl_tlv->type = htons(TLV_TYPE_NARB_SUBNET_DTL);
        subnet_dtl_tlv->length = htons(p_subnet_dtl->size()*sizeof(dtl_hop));
        list<dtl_hop>::iterator it = p_subnet_dtl->begin();
        for (int i=0; it != p_subnet_dtl->end(); it++, i++)
        {
            subnet_dtl_tlv->hops[i] = (*it);
        }
        offset += (ntohs(subnet_dtl_tlv->length)+TLV_HDR_SIZE);
    }

    msg = api_msg_new (MSG_REPLY_ERO, offset, buf, ucid, seqnr);
    
    return msg;
}

u_int32_t narb_get_msg_lspb_id(api_msg* msg)
{
    int msg_len = ntohs(msg->header.length);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    int tlv_len;

    while (msg_len > 0)
    {
        if (ntohs(tlv->type) == TLV_TYPE_NARB_LSPB_ID) 
        {
            return ((msg_narb_lspb_id*)tlv)->lspb_id;
        }
        tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }

    return 0;
}

void narb_extract_ero_tlv (te_tlv_header& ero_tlv, list<ero_subobj*>& ero)
{
    ero.clear();
    int length = ntohs(ero_tlv.length) + TLV_HDR_SIZE;
    if (length  <= 4)
        return;
    
    int ero_num = (length - sizeof(te_tlv_header)) / sizeof(ipv4_prefix_subobj);
    if ((length - sizeof(te_tlv_header)) % sizeof(ipv4_prefix_subobj) == 0)
        return;
    
    ipv4_prefix_subobj * subobj = (ipv4_prefix_subobj*)((char*)&ero_tlv + sizeof(te_tlv_header));
    
    for (int i = 0; i < ero_num; i++, subobj++)
    {
        ero_subobj* subobj_narb = new ero_subobj;
        subobj_narb->hop_type = (subobj->l_and_type >> 7 == 0) ? ERO_TYPE_STRICT_HOP : ERO_TYPE_LOOSE_HOP;
        subobj_narb->prefix_len = 32;
        memcpy(&subobj_narb->addr, subobj->addr, 4);
        ero.push_back(subobj_narb);
    }
}

