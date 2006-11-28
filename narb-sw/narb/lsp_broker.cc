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
#include <arpa/inet.h>
#include "lsp_broker.hh"
#include "narb_config.hh"
#include "dts.hh"
#include "toolbox.hh"
#include "narb_apiclient.hh"
#include "zebra_ospfclient.hh"

extern ZebraOspfSync* zebra_client;

LSPQ::LSPQ(LSP_Broker* b, in_addr s, in_addr d, u_int32_t bw, u_char swt, u_char enc, u_int32_t seq, u_int32_t op): 
broker(b), app_seqnum(seq), app_options(op)
{ 
    Init(); 
    req_spec.src.s_addr = s.s_addr; 
    req_spec.dest.s_addr = d.s_addr; 
    req_spec.bandwidth = bw; 
    req_spec.encoding_type = enc;  
    req_spec.switching_type = swt; 
    req_spec.type = htons(MSG_APP_REQUEST); 
    req_spec.length = htons(sizeof(req_spec));
    mrn_spec = req_spec; //@@@@
}

LSPQ::LSPQ(LSP_Broker* b, msg_app2narb_request& r, u_int32_t seq, u_int32_t op):
broker(b), app_seqnum(seq), app_options(op)
{ 
    Init(); 
    req_spec = r; 
    mrn_spec = r; //@@@@
}

LSPQ::LSPQ(LSP_Broker* b, msg_app2narb_request& r, msg_app2narb_request& mr, u_int32_t seq, u_int32_t op):
broker(b), app_seqnum(seq), app_options(op)
{ 
    Init(); 
    req_spec = r; 
    mrn_spec = mr;
}

void LSPQ::Init()
{
    state = STATE_IDLE;
    req_ucid = 0;
    req_vtag = 0;
    ero_state = ERO_NONE;
    req_retran_counter = MAX_REQ_RETRAN;
    memset(&req_spec, 0, sizeof(req_spec));
    req_spec.type = htons(TLV_TYPE_NARB_REQUEST);
    req_spec.length = htons(sizeof(req_spec) - 4);
    mrn_spec = req_spec;
    vtag_mask = NULL;
}

LSPQ::~LSPQ()
{
    list<ero_subobj*>::iterator it;

    for (it = ero.begin(); it != ero.end(); it++)
        if (*it)
            delete (ero_subobj*)(*it);

    if (vtag_mask)
        delete vtag_mask;
}

void LSPQ::DescribeLSP(string& desc)
{
    char buf[100], buf1[20], buf2[20], buf3[20];
    strcpy(buf1, inet_ntoa(req_spec.src));
    strcpy(buf2, inet_ntoa(req_spec.dest));
    switch (state)
    {
        case STATE_ERO_COMPLETE:
            strcpy(buf3, "responded");
            break;
        case STATE_ERROR:
            strcpy(buf3, "error");
            break;
        case STATE_RESV_CONFIRM:
            strcpy(buf3, "established");
            break;
        case STATE_RESV_RELEASE_CONFIRM:
            strcpy(buf3, "deleted");
            break;
        default:
            strcpy(buf3, "in process");
            break;
    }
    
    sprintf(buf, "%-15s %-15s %-9.2f %-6d %-7d %-6d %-10s", buf1, buf2, req_spec.bandwidth,
        req_spec.switching_type, req_spec.encoding_type, req_vtag, buf3);
    desc = buf;
}


void LSPQ::GetERO(te_tlv_header* tlv, list<ero_subobj*>& ero)
{
    assert (tlv);
    ero.clear();
    int len = ntohs(tlv->length);
    assert( len > 0 && len% sizeof(ero_subobj) == 0);

    char addr[20]; //debug
    ero_subobj * subobj  = (ero_subobj *)((char *)tlv + sizeof(struct te_tlv_header));
    for (; len > 0 ;subobj++, len -= sizeof(ero_subobj))
    {
        inet_ntop(AF_INET, &subobj->addr, addr, 20); //debug
        LOGF("HOP-TYPE [%s]: %s\n", subobj->hop_type?"loose":"strict", addr); //debug
        ero_subobj * new_subobj = (ero_subobj*)malloc(sizeof(ero_subobj));
        memcpy(new_subobj, subobj, sizeof(ero_subobj));
        ero.push_back(new_subobj);
    }
}

void LSPQ::GetERO_RFCStandard(te_tlv_header* tlv, list<ero_subobj*>& ero)
{
    char addr[20]; //debug
    assert (tlv);
    ero.clear();

    int len = ntohs(tlv->length) ;
    assert( len > 0);
    int offset = sizeof(struct te_tlv_header);

    ipv4_prefix_subobj* subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
    unum_if_subobj* subobj_unum;
    while (len > 0)
    {
        ero_subobj * new_subobj = (ero_subobj*)malloc(sizeof(ero_subobj));
        memset(new_subobj, 0, sizeof(ero_subobj));
        if ((subobj_ipv4->l_and_type & 0x7f) == 4)
            subobj_unum = (unum_if_subobj *)((char *)tlv + offset);
        else
            subobj_unum = NULL;

        if (subobj_unum)
        {
            inet_ntop(AF_INET, &subobj_unum->addr, addr, 20); //debug printout
            memcpy(&new_subobj->addr, &subobj_unum->addr, 4);
            new_subobj->hop_type = (subobj_unum->l_and_type >> 7) ? ERO_TYPE_LOOSE_HOP : ERO_TYPE_STRICT_HOP;
            new_subobj->prefix_len = 32;
            new_subobj->if_id = subobj_unum->ifid;
            if ((ntohl(subobj_unum->ifid)>>16) == 0x0004 && (ntohl(subobj_unum->ifid) & 0xffff) > 0  
            && (ntohl(subobj_unum->ifid) & 0xffff) < 4906) //0x0004 == LOCAL_ID_TAGGED_GROUP_GLOBAL
                new_subobj->l2sc_vlantag = (u_int16_t)ntohl(subobj_unum->ifid);
            LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d]\n", (subobj_unum->l_and_type & (1<<7)) == 0?"strict":"loose", addr, new_subobj->if_id);
            len -= sizeof(unum_if_subobj);
            offset += sizeof(unum_if_subobj);
        }
        else
        {
            inet_ntop(AF_INET, (in_addr*)subobj_ipv4->addr, addr, 20); //debug printout
            memcpy(&new_subobj->addr, subobj_ipv4->addr, 4);
            new_subobj->hop_type = (subobj_ipv4->l_and_type >> 7) ? ERO_TYPE_LOOSE_HOP : ERO_TYPE_STRICT_HOP;
            new_subobj->prefix_len = 32;
            LOGF("HOP-TYPE [%s]: %s\n", (subobj_ipv4->l_and_type & (1<<7)) == 0?"strict":"loose", addr);
            len -= sizeof(ipv4_prefix_subobj);
            offset += sizeof(ipv4_prefix_subobj);
        }
        ero.push_back(new_subobj);
        subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
    }
}

int LSPQ::MergeERO(list<ero_subobj*>& ero_inter, list<ero_subobj*>& ero_intra)
{
    list<ero_subobj*>::iterator node_inter, node_intra, src_tmp, dest_tmp;
    in_addr src_intra, dest_intra;
    ero_subobj* nodedata;
    in_addr *router_id1, *router_id2;
    list<ero_subobj*> merged_ero;
    
    assert (ero_inter.size() > 0);
    assert (ero_intra.size() > 0);
  
    src_intra = (ero_intra.front())->addr;
    dest_intra = (ero_intra.back())->addr;
  
    router_id1 = &src_intra;

    for (node_inter = ero_inter.begin(); node_inter != ero_inter.end(); node_inter++)
    {
        router_id2 = &((*node_inter)->addr);
        if (router_id1->s_addr == router_id2->s_addr)
            break;
    }
  
    if (node_inter == ero_inter.end())  // src_intra must exist in ero_inter list 
      return -1;
    
    src_tmp = node_inter;
  
    router_id1 = &dest_intra;

    node_inter++;
    while (node_inter != ero_inter.end())
    {
        router_id2 = &((*node_inter)->addr);
        if (router_id1->s_addr == router_id2->s_addr)
            break;
        node_inter++;
    }
    if (node_inter == ero_inter.end())  // dest_intra must exist in ero_inter list 
        return -1;
    dest_tmp = node_inter;
  
    node_inter = ero_inter.begin();
  
    // add nodes in ero_inter before ero_intra->head
    do
    {
        assert(*node_inter != NULL);
        nodedata = new ero_subobj(*(*node_inter));
        merged_ero.push_back(nodedata);
        node_inter++;
    } while (node_inter !=src_tmp);
    
    // add nodes in ero_intra
    node_intra = ero_intra.begin();
    while (node_intra != ero_intra.end())
    {
        assert(*node_intra != NULL);
        nodedata = new ero_subobj(*(*node_intra));
        merged_ero.push_back(nodedata);
        node_intra++;
    }
  
    // add nodes in ero_inter after ero_intra->tail
    node_inter = ero_inter.begin();
    while (node_inter++ != dest_tmp)
        ;
  
    while (node_inter != ero_inter.end())
    {
        assert(*node_inter != NULL);
        nodedata = new ero_subobj(*(*node_inter));
        merged_ero.push_back(nodedata);
        merged_ero.push_back(nodedata);
        node_inter++;
      } 

    for (node_inter = ero_inter.begin(); node_inter != ero_inter.end(); node_inter++)
    {
        if (*node_inter)
            delete (ero_subobj*)(*node_inter);
    }
    ero_inter.clear();

    for (node_intra = ero_intra.begin(); node_intra != ero_intra.end(); node_intra++)
    {
        if (*node_intra)
            delete (ero_subobj*)(*node_intra);
    }
    ero_intra.clear();

    ero_inter = merged_ero;
    return 0;
}

int LSPQ::ForceMergeERO(list<ero_subobj*>& ero_inter, list<ero_subobj*>& ero_intra)
{
    list<ero_subobj*>::iterator node_inter, node_intra;
    ero_subobj* nodedata;
    list<ero_subobj*> merged_ero;
    
    assert (ero_inter.size() > 0);
    assert (ero_intra.size() > 0);

    if (is_all_strict_hops(ero_inter))
        return -1;

    ero_subobj* merge_point = first_loose_hop(ero_inter);
    if (merge_point == NULL)
        return -1;
     
    // add nodes in ero_inter before merge_point
    node_inter = ero_inter.begin();
    do
    {
        assert(*node_inter != NULL);
        nodedata = new ero_subobj(*(*node_inter));
        merged_ero.push_back(nodedata);
        node_inter++;
    } while (*node_inter != merge_point);
    
    // add nodes in ero_intra
    node_intra = ero_intra.begin();
    while (node_intra != ero_intra.end())
    {
        assert(*node_intra != NULL);
        nodedata = new ero_subobj(*(*node_intra));
        merged_ero.push_back(nodedata);
        node_intra++;
    }
  
    for (node_inter = ero_inter.begin(); node_inter != ero_inter.end(); node_inter++)
    {
        if (*node_inter)
            delete (ero_subobj*)(*node_inter);
    }
    ero_inter.clear();

    for (node_intra = ero_intra.begin(); node_intra != ero_intra.end(); node_intra++)
    {
        if (*node_intra)
            delete (ero_subobj*)(*node_intra);
    }
    ero_intra.clear();

    ero_inter = merged_ero;
    return 0;
}


//////////////////////////////////////////////////////////////////
//////////////////////// LSPQ FSM Functions ////////////////////////
//////////////////////////////////////////////////////////////////

/////// STATE_APP_REQ ////////
int LSPQ::HandleLSPQRequest()
{
    state = STATE_APP_REQ;

    int ret; 

    RCE_APIClient * rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);

    if (!rce_client)
    {
        rce_client = RceFactory.GetClient((char*)SystemConfig::rce_sec_host.c_str(), SystemConfig::rce_sec_port);
        if (!rce_client)
            return HandleErrorCode(NARB_ERROR_INTERNAL);
    }

    if(!rce_client->IsAlive())
    {
        ret = rce_client->Connect();
        if (ret < 0)
        {
            RceFactory.RemoveClient(rce_client);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
    }

    msg_narb_cspf_request cspf_req;
    cspf_req.app_req_data = req_spec;
    cspf_req.app_seqnum = app_seqnum;
    cspf_req.lspb_id = broker->lspb_id;
    rce_client->QueryLsp(cspf_req, app_options | LSP_TLV_NARB_CSPF_REQ | (app_options & LSP_OPT_STRICT ? LSP_OPT_PREFERRED : 0), req_vtag, vtag_mask);
    return 0;
}

/////// STATE_REQ_RECURSIVE ////////
int LSPQ::HandleRecursiveRequest()
{
    state = STATE_REQ_RECURSIVE;

    int ret; 

    RCE_APIClient * rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
    if (!rce_client)
    {
        rce_client = RceFactory.GetClient((char*)SystemConfig::rce_sec_host.c_str(), SystemConfig::rce_sec_port);
        if (!rce_client)
            return HandleErrorCode(NARB_ERROR_INTERNAL);
    }
    
    if (!rce_client->IsAlive())
    {
        ret = rce_client->Connect();
        if (ret < 0)
        {
            RceFactory.RemoveClient(rce_client);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
    }

    msg_narb_cspf_request cspf_req;
    cspf_req.app_req_data = req_spec;
    cspf_req.app_seqnum = app_seqnum;
    cspf_req.lspb_id = broker->lspb_id;

    if (req_vtag == ANY_VTAG || vtag_mask)
        app_options |= LSP_OPT_REQ_ALL_VTAGS;
    rce_client->QueryLsp_MRN(cspf_req, mrn_spec, SystemConfig::rce_options | LSP_OPT_STRICT | LSP_OPT_PREFERRED 
	|  (app_options & LSP_OPT_E2E_VTAG? LSP_OPT_E2E_VTAG : 0) | (app_options & LSP_OPT_VIA_MOVAZ? LSP_OPT_VIA_MOVAZ : 0), req_vtag, vtag_mask); 
}

/////// STATE_RCE_REPLY  ////////
int LSPQ::HandleRCEReply(api_msg *msg)
{
    state = STATE_RCE_REPLY;

    assert(msg && msg->header.type_8 == MSG_LSP);

    HandleOptionalResponseTLVs(msg);

    if (msg->header.action == ACT_ERROR)
    {
        u_int32_t err_code = ntohl(*(u_int32_t *)((char *)msg->body + sizeof(struct te_tlv_header)));
        api_msg_delete(msg);

        return HandleErrorCode(err_code);
    }
    else if (msg->header.action == ACT_ACKDATA)
    {
        GetERO((te_tlv_header*)msg->body, ero);
        if ((app_options & LSP_OPT_E2E_VTAG) && req_vtag == ANY_VTAG)
            req_vtag = ntohl(msg->header.tag);
    }

    //@@@@ Additional info in msg->header.nsgtag[], indicating MRN status?

    api_msg_delete(msg);    
    
    if (ero.size() == 0)
        return HandleErrorCode(NARB_ERROR_INTERNAL);

    switch (SystemConfig::routing_mode)
    {
    case RT_MODE_ALL_STRICT_ONLY:
    case RT_MODE_MIXED_ALLOWED:
        if(is_all_loose_hops(ero)) 
        {
            //if the source node is a border node
            list<ero_subobj*>::iterator it = ero.begin();
            it++;
            assert (it != ero.end());
            if_narb_info *narb = NarbDomainInfo.LookupNarbByRmtIf((*it)->addr);
            if (narb)
                return HandlePartialERO();
            // otherwise
            return HandleErrorCode(NARB_ERROR_NO_ROUTE);
        }
        else if (is_all_strict_hops(ero))
            return HandleCompleteERO();
        else
            return HandlePartialERO();
        break;
    case RT_MODE_MIXED_PREFERRED:
        if(!is_all_loose_hops(ero))
            return HandleCompleteERO();
        else
            return HandleErrorCode(NARB_ERROR_NO_ROUTE);
        break;            
    case RT_MODE_ALL_LOOSE_ALLOWED:
        return HandleCompleteERO();
        break;
    }

    return 0;
}


/////// STATE_ERO_PARTIAL  ////////
int LSPQ::HandlePartialERO()
{
    state = STATE_ERO_PARTIAL;

    int ret; 

    assert (SystemConfig::routing_mode == RT_MODE_ALL_STRICT_ONLY ||
                SystemConfig::routing_mode == RT_MODE_MIXED_ALLOWED);
    

    NarbDomainInfo.SearchAndProcessInterdomainLink(ero);

    if (is_all_strict_hops(ero))
        return HandleCompleteERO();

    ero_subobj *new_src_subobj = last_strict_hop(ero);
    if (!new_src_subobj)
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);

    link_info * link = NarbDomainInfo.LookupLinkByRmtIf(new_src_subobj->addr);
    if (!link)
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);

    msg_narb_recursive_cspf_request rec_cspf_req;
    memset(&rec_cspf_req, 0, sizeof(struct msg_narb_recursive_cspf_request));
    rec_cspf_req.app_seqnum = app_seqnum;
    rec_cspf_req.lspb_id = broker->lspb_id;
    rec_cspf_req.app_req_data = req_spec;
    rec_cspf_req.app_req_data.type = htons(MSG_PEER_REQUEST);
    rec_cspf_req.rec_req_data = rec_cspf_req.app_req_data;
    rec_cspf_req.rec_req_data.src.s_addr = link->Id();

    NARB_APIClient * peer_narb = NarbFactory.GetClient(new_src_subobj->addr);

    if(!peer_narb)
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);

    if (!peer_narb->IsAlive())
    {
        ret = peer_narb->Connect();
        if (ret < 0)
        {
            NarbFactory.RemoveClient(peer_narb);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
    }

    //multi-region network
    new_src_subobj = first_loose_hop(ero);
    if (new_src_subobj->sw_type != rec_cspf_req.app_req_data.switching_type ||
        new_src_subobj->encoding != rec_cspf_req.app_req_data.encoding_type)
    {
        rec_cspf_req.rec_req_data.switching_type = new_src_subobj->sw_type;
        rec_cspf_req.rec_req_data.encoding_type = new_src_subobj->encoding;
        rec_cspf_req.rec_req_data.bandwidth = new_src_subobj->bandwidth;
        //LSP_OPT_MRN_RELAY//@@@@
    }

    peer_narb->QueryLspRecursive(rec_cspf_req, app_options | LSP_OPT_STRICT & (~ LSP_OPT_PREFERRED), req_vtag, vtag_mask);
}

/////// STATE_NEXT_HOP_NARB_REPLY  ////////
int LSPQ::HandleNextHopNARBReply(api_msg *msg)
{
    state = STATE_NEXT_HOP_NARB_REPLY;

    HandleOptionalResponseTLVs(msg);

    list<ero_subobj*> rec_ero;
    te_tlv_header *tlv = (te_tlv_header*)msg->body;
    if (ntohs(tlv->type) == TLV_TYPE_NARB_ERROR_CODE)
    {
        api_msg_delete(msg);
        goto _CANNOT_EXPAND_ROUTE;
    }
    else if (ntohs(tlv->type) == TLV_TYPE_NARB_ERO)
    {
        GetERO_RFCStandard((te_tlv_header*)msg->body, rec_ero);
    }

    api_msg_delete(msg);
    if (ero.size() == 0 || rec_ero.size() == 0)
        return HandleErrorCode(NARB_ERROR_INTERNAL);

    if (SystemConfig::forced_merge && is_all_strict_hops(rec_ero))
    {
        if (ForceMergeERO(ero, rec_ero) == 0) // force-merged succssfully
            return HandleCompleteERO();
    }
    else if (MergeERO(ero, rec_ero) == 0) // merged succssfully
        return HandleCompleteERO();

    //Unsucessful recursive routing
_CANNOT_EXPAND_ROUTE:
    if (SystemConfig::routing_mode == RT_MODE_MIXED_ALLOWED)
        return HandleCompleteERO();
    else 
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);
}

/////// STATE_ERROR ////////
int LSPQ::HandleErrorCode(u_int32_t errcode)
{
    state = STATE_ERROR;

    assert(broker);

    api_msg* rmsg;

    rmsg = narb_new_msg_reply_error(req_ucid, app_seqnum, errcode);
    assert(rmsg);
    rmsg->header.tag = htonl(req_vtag);

    broker->HandleReplyMessage(rmsg);
    return 0;
}

/////// STATE_ERO_COMPLETE ////////
int LSPQ::HandleCompleteERO()
{
    state = STATE_ERO_COMPLETE;

    int length;
    void * tlv_data;
    api_msg * rmsg;

    assert(broker);

    rmsg = narb_new_msg_reply_ero(req_ucid, app_seqnum, ero);
    if (!rmsg)
        HandleErrorCode(NARB_ERROR_NO_ROUTE);
    rmsg->header.tag = htonl(req_vtag);

    broker->HandleReplyMessage(rmsg);
    return 0;
}

static in_addr LookupPeerNarbByERO(list<ero_subobj*>& ero)
{
    in_addr ret_ip;
    ret_ip.s_addr = 0;
    if_narb_info * narb_info; 
    list<ero_subobj*>::iterator it;
    for (it = ero.begin(); it != ero.end(); it++)
    {
        narb_info = NarbDomainInfo.LookupNarbByRmtIf((*it)->addr);
        if (narb_info)
            return (*it)->addr;
    }
    return ret_ip;
}


/////// STATE_RESV_CONFIRM ////////
int LSPQ::HandleResvConfirm(api_msg* msg)
{
    if (state != STATE_ERO_COMPLETE)
    {
        LOG("Trying to confirm an uncomputed LSP: seqnum=" << this->app_seqnum << endl);
        return -1;
    }

    state = STATE_RESV_CONFIRM;

    msg_app2narb_confirm* app_msg = (msg_app2narb_confirm*)msg->body;

    list<ero_subobj*> ero_confirm;
    GetERO_RFCStandard(&app_msg->ero, ero_confirm);
    if(ero.size() <= 0)
    {
        LOG("LSPQ::HandleResvConfirm cannot handle empty ERO" << endl);
        return -1;
    }

    //get peer NARB address from ero_confirm list before it is reduced
    in_addr narb_ip = LookupPeerNarbByERO(ero_confirm);

    list<ero_subobj*>::iterator it;
    ero_subobj* subobj, *reverse_subobj;
    link_info *link1, *link2;
    link_info *reverse_link1, *reverse_link2;

    for (it = ero_confirm.begin(); it != ero_confirm.end();  it++)
    {
        u_int32_t vtag = 0;
        subobj = *it;
        if (subobj->addr.s_addr == app_msg->req.src.s_addr ||subobj->addr.s_addr == app_msg->req.dest.s_addr)
        {
            //remove loopback-address subobj from the ero list.
            //subobj->hop_type = 0xff; 
            continue;
        }
 
        link1 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr);
        reverse_link1 = NarbDomainInfo.LookupLinkByRmtIf(subobj->addr);
        if (!link1 && !reverse_link1)
        {
            continue;
        }
        else
        {
            it++;
            if (it == ero_confirm.end())
                break;
            reverse_subobj = subobj;
            subobj = *it;
            link2 = NarbDomainInfo.LookupLinkByRmtIf(subobj->addr); 
            reverse_link2 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr); 
            //reverse_subobj->hop_type = 0xff;
            //subobj->hop_type = 0xff;
            if (link1 != link2 && reverse_link1 != reverse_link2)
                continue;
        }

        if (zebra_client && !zebra_client->GetWriter() && zebra_client->Alive())
        {
            if (zebra_client->RunWithoutSyncLsdb() < 0)
                LOG("RunWithoutSyncLsdb failed"<<endl);
            // Register type (10, 1) Opaque-TE LSA.
            int ret = zebra_client->GetWriter()->RegisterOpqaueType(10, 1);
            if (ret < 0)
            {
                LOG("RegisterOpqaueType[10, 1] failed !"<<endl);
            }
        }

        //found a corresponding abstract link in the forward direction
        if (link1 != NULL && link1 == link2)
        {
            vtag = subobj->l2sc_vlantag;
            if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    link1->UnreservedBandwidth()[i] -= app_msg->req.bandwidth;
                    link1->GetISCD()->max_lsp_bw[i] -= app_msg->req.bandwidth;
                }
                if (vtag != 0)
                    link1->ResetVtag(vtag);
                NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), link1);
            }
        }

        //handling the link state updates in the reverse direction
        if (reverse_link1 != NULL && reverse_link1 == reverse_link2 && (app_options & LSP_OPT_BIDIRECTIONAL) != 0)
        {
            vtag = reverse_subobj->l2sc_vlantag;
            if (reverse_link1 != NULL)
            {
                if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        reverse_link1->UnreservedBandwidth()[i] -= app_msg->req.bandwidth;
                        reverse_link1->GetISCD()->max_lsp_bw[i] -= app_msg->req.bandwidth;
                    }
                    if (vtag != 0)
                        reverse_link1->ResetVtag(vtag);
                    NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), reverse_link1);
                }
            }
        }
    }

    //proceed to the next domain if any
    if(narb_ip.s_addr == 0)
        return 0;
    NARB_APIClient * peer_narb = NarbFactory.GetClient(narb_ip);
    if(!peer_narb)
        return -1;

    if (!peer_narb->IsAlive())
    {
        int ret = peer_narb->Connect();
        if (ret < 0)
        {
            NarbFactory.RemoveClient(peer_narb);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
    }

    //peer_narb->SendMessage(msg);
    peer_narb->RelayMessageToPeer(MSG_APP_CONFIRM, msg, ero_confirm);
    return 0;
}

/////// STATE_RESV_RELEASE ////////
int LSPQ::HandleResvRelease(api_msg* msg)
{
    if (state != STATE_RESV_CONFIRM)
    {
        LOG("Trying to release an unconfirmed LSP: seqnum=" << this->app_seqnum << endl);
        return -1; //HandleResvReleaseConfirm();
    }
    
    state = STATE_RESV_RELEASE;

    msg_app2narb_confirm* app_msg = (msg_app2narb_confirm*)msg->body;

    list<ero_subobj*> ero_confirm;
    GetERO_RFCStandard(&app_msg->ero, ero_confirm);
    if(ero.size() <= 0)
    {
        LOG("LSPQ::HandleResvRelease cannot handle empty ERO" << endl);
        return -1;
    }

    //get peer NARB address from ero_confirm list before it is reduced
    in_addr narb_ip = LookupPeerNarbByERO(ero_confirm);

    list<ero_subobj*>::iterator it;
    ero_subobj* subobj, *reverse_subobj;
    link_info *link1, *link2;
    link_info *reverse_link1, *reverse_link2;

    for (it = ero_confirm.begin(); it != ero_confirm.end(); it++)
    {
        u_int32_t vtag = 0;
        subobj = *it;
        if (subobj->addr.s_addr == app_msg->req.src.s_addr ||subobj->addr.s_addr == app_msg->req.dest.s_addr)
        {
            //remove loopback-address subobj from the ero list.
            //subobj->hop_type = 0xff; 
            continue;
        }
 
        link1 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr);
        reverse_link1 = NarbDomainInfo.LookupLinkByRmtIf(subobj->addr);
        if (!link1 && !reverse_link1)
        {
            continue;
        }
        else
        {
            it++;
            if (it == ero_confirm.end())
                break;
            reverse_subobj = subobj;
            subobj = *it;
            link2 = NarbDomainInfo.LookupLinkByRmtIf(subobj->addr); 
            reverse_link2 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr); 
            //reverse_subobj->hop_type = 0xff;
            //subobj->hop_type = 0xff;
            if (link1 != link2 && reverse_link1 != reverse_link2)
                continue;
        }

        if (zebra_client && !zebra_client->GetWriter() && zebra_client->Alive())
        {
            if (zebra_client->RunWithoutSyncLsdb() < 0)
                LOG("RunWithoutSyncLsdb failed"<<endl);
            // Register type (10, 1) Opaque-TE LSA.
            int ret = zebra_client->GetWriter()->RegisterOpqaueType(10, 1);
            if (ret < 0)
            {
                LOG("RegisterOpqaueType[10, 1] failed !"<<endl);
            }
        }

        //found a corresponding abstract link in the forward direction
        if (link1 != NULL && link1 == link2)
        {
            vtag = subobj->l2sc_vlantag;
            if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    link1->UnreservedBandwidth()[i] += app_msg->req.bandwidth;
                    link1->GetISCD()->max_lsp_bw[i] += app_msg->req.bandwidth;
                }
                if (vtag != 0)
                    link1->SetVtag(vtag);
                NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), link1);
            }
        }

        //handling the link state updates in the reverse direction
        if (reverse_link1 != NULL && reverse_link1 == reverse_link2 && (app_options & LSP_OPT_BIDIRECTIONAL) != 0)
        {
            vtag = reverse_subobj->l2sc_vlantag;
            if (reverse_link1 != NULL)
            {
                if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        reverse_link1->UnreservedBandwidth()[i] += app_msg->req.bandwidth;
                        reverse_link1->GetISCD()->max_lsp_bw[i] += app_msg->req.bandwidth;
                    }
                    if (vtag != 0)
                        reverse_link1->SetVtag(vtag);
                    NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), reverse_link1);
                }
            }
        }
    }

    //proceed to the next domain if any
    if(narb_ip.s_addr == 0)
        return 0;
    NARB_APIClient * peer_narb = NarbFactory.GetClient(narb_ip);
    if(!peer_narb)
        return -1;

    if (!peer_narb->IsAlive())
    {
        int ret = peer_narb->Connect();
        if (ret < 0)
        {
            NarbFactory.RemoveClient(peer_narb);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
    }

    //peer_narb->SendMessage(msg);
    peer_narb->RelayMessageToPeer(MSG_APP_REMOVE, msg, ero_confirm);

    if (NarbDomainInfo.LookupRouterById(app_msg->req.src))
        return HandleResvReleaseConfirm();
    else
        return 0;
}

int LSPQ::HandleResvReleaseConfirm()
{
   state = STATE_RESV_RELEASE_CONFIRM;

    assert(broker);

    api_msg* rmsg;

    rmsg = narb_new_msg_reply_release_confirm(req_ucid, app_seqnum);

    assert(rmsg);

    broker->HandleReplyMessage(rmsg);
    return 0;
   
}

//////////////////////////////////////////////////////////////////
/////////////////////   LSP_Broker Definitions ////////////////////////
//////////////////////////////////////////////////////////////////

LSP_Broker::LSP_Broker(int sock, NARB_APIServer * server): APIReader(sock, (APIServer*)server)
{
    api_writer = NULL;
    lspb_id = get_unique_lspb_id();
}

LSP_Broker::~LSP_Broker()
{
    if (api_writer && api_writer->Obsolete() == false)
    {
        api_writer->Close();
        delete api_writer;
    }

    list<LSPQ*>::iterator it;
    for (it = lspq_list.begin(); it != lspq_list.end(); it++)
        if (*it)
            delete *it;
    NARB_APIServer::lsp_brokers.remove(this);
}

void LSP_Broker::Run()
{
    if (api_writer == NULL)
    {
        api_writer = new APIWriter(fd, server);
        this->SetWriter(api_writer);
        api_writer->SetAutoDelete(false);
        api_writer->SetObsolete(false);
        api_writer->SetRepeats(0);
        api_writer->SetReader(this);
        //eventMaster.Schedule(api_writer);
    }

    api_msg * msg = ReadMessage();

    if (!msg)
    {
        Close();
        api_writer->Close();
        return;
    }

    int ret = HandleMessage(msg);
}

int LSP_Broker::HandleMessage(api_msg * msg)
{
    assert (ntohs(msg->header.type) == NARB_MSG_LSPQ);

    LSPQ* lspq = LspqLookup(ntohl(msg->header.seqnum));
    msg_app2narb_request * app_req = (msg_app2narb_request*)msg->body;

    switch (ntohs(app_req->type))
    {
    case MSG_APP_REQUEST:
       {
            if (!lspq)
            {
                lspq = new LSPQ(this, *app_req, ntohl(msg->header.seqnum), ntohl(msg->header.options));
                lspq_list.push_back(lspq);
                lspq->SetReqUcid(ntohl(msg->header.ucid));
                lspq->SetReqVtag(ntohl(msg->header.tag));
                lspq->HandleOptionalRequestTLVs(msg);
                lspq->HandleLSPQRequest();
            }
            else // retransmission of lsp query
            {
                lspq->SetReqAppMsg(*app_req);
                lspq->SetReqMrnMsg(*app_req); //the same *app_req
                lspq->SetReqUcid(ntohl(msg->header.ucid));
                lspq->SetReqVtag(ntohl(msg->header.tag));
                lspq->SetReqOptions(ntohl(msg->header.options));
                lspq->HandleOptionalRequestTLVs(msg);
                if (lspq->State() == STATE_ERROR)
                {
                    if (lspq->req_retran_counter-- > 0)
                    {
                        lspq->HandleLSPQRequest();
                    }
                    else
                    {
                        lspq->HandleErrorCode(NARB_ERROR_EXCEED_MAX_RETRAN);
                    }
                }
                else if (lspq->State() == STATE_ERO_COMPLETE)
                {
                    // Re-reply to app with the resulting ERO
                    lspq->HandleLSPQRequest();
                    //@@@@lspq->HandleCompleteERO();
                }
            }
            api_msg_delete(msg);
        }
        break;
    case MSG_PEER_REQUEST:
       {
            msg_app2narb_request * mrn_req = app_req + 1;
            if (!lspq)
            {
                lspq = new LSPQ(this, *app_req, *mrn_req, ntohl(msg->header.seqnum), ntohl(msg->header.options));
                lspq->SetReqUcid(ntohl(msg->header.ucid));
                lspq->SetReqVtag(ntohl(msg->header.tag));
                lspq->HandleOptionalRequestTLVs(msg);
                lspq_list.push_back(lspq);
                lspq->HandleRecursiveRequest();
            }
            else // retransmission of lsp query
            {
                lspq->SetReqAppMsg(*app_req);
                lspq->SetReqMrnMsg(*mrn_req);
                lspq->SetReqUcid(ntohl(msg->header.ucid));
                lspq->SetReqVtag(ntohl(msg->header.tag));
                lspq->SetReqOptions(ntohl(msg->header.options));
                lspq->HandleOptionalRequestTLVs(msg);

                //@@@@ Always retry / recompute
                //if (lspq->State() == STATE_ERROR)
                //{
                //    if (lspq->req_retran_counter-- > 0)
                //    {
                //        lspq->HandleRecursiveRequest();
                //    }
                //}
                //else if (lspq->State() == STATE_ERO_COMPLETE)
                //{
                    // Re-reply to peer NARB with the resulting ERO
                lspq->HandleRecursiveRequest();
                    //@@@@lspq->HandleCompleteERO();
                //}
            }
            api_msg_delete(msg);
        }
        break;
    case MSG_APP_CONFIRM:
        {
            if (!lspq)
            {
                LOGF("LSP_Broker::MSG_APP_CONFIRM_EVENT: The LSPQ (seqnum = %d) no longer exists.", ntohl(msg->header.seqnum));
                return -1;
            }

            lspq->HandleResvConfirm(msg);
        }
        break;
    case MSG_APP_REMOVE:
        {
            if (!lspq)
            {
                LOGF("LSP_Broker::MSG_APP_REMOVE_EVENT: The LSPQ (seqnum = %d) no longer exists.", ntohl(msg->header.seqnum));
                return -1;
            }

            lspq->HandleResvRelease(msg);
        }
        break;
    default:
        LOGF("Unknow APP->NARB message type (%d)\n", app_req->type);
    }

    return 0;
}

void LSPQ::HandleOptionalRequestTLVs(api_msg* msg)
{
    int msg_len = ntohs(msg->header.length);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    int tlv_len;
    msg_app2narb_vtag_mask* vtagMask;

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type)) 
        {
        case TLV_TYPE_NARB_REQUEST:
            tlv_len = sizeof(msg_app2narb_request);
            ; //do nothing
            break;
        case TLV_TYPE_NARB_VTAG_MASK:
            tlv_len = sizeof(msg_app2narb_vtag_mask);
            vtagMask = (msg_app2narb_vtag_mask*)tlv;
            if (ntohl(msg->header.options) & LSP_OPT_VTAG_MASK)
            {
                if (!vtag_mask)
                    vtag_mask = new (struct msg_app2narb_vtag_mask);
                memcpy(vtag_mask, vtagMask, sizeof(msg_app2narb_vtag_mask));
            }
            break;
        default:
            break;
        }
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }
}

void LSPQ::HandleOptionalResponseTLVs(api_msg* msg)
{
    int msg_len = ntohs(msg->header.length);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    int tlv_len;
    msg_app2narb_vtag_mask* vtagMask;

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type)) 
        {
        case TLV_TYPE_NARB_ERO:
            tlv_len = TLV_HDR_SIZE + ntohs(tlv->length);
            ; //do nothing
            break;
        case TLV_TYPE_NARB_ERROR_CODE:
            tlv_len = TLV_HDR_SIZE + 4;
            ; //do nothing
            break;
        case TLV_TYPE_NARB_VTAG_MASK:
            tlv_len = sizeof(msg_app2narb_vtag_mask);
            vtagMask = (msg_app2narb_vtag_mask*)tlv;
            if (ntohl(msg->header.options) & LSP_OPT_VTAG_MASK)
            {
                if (!vtag_mask)
                    vtag_mask = new (struct msg_app2narb_vtag_mask);
                memcpy(vtag_mask, vtagMask, sizeof(msg_app2narb_vtag_mask));
            }
            break;
        default:
            break;
        }
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }
}

// searching for a request data record on lspq_list using msg sequence number
LSPQ * LSP_Broker::LspqLookup (u_int32_t seqnum)
{
    list<LSPQ*>::iterator it;

    for (it = lspq_list.begin(); it != lspq_list.end(); it++)
    {
        if (!(*it))
            continue;

        if (seqnum == (*it)->app_seqnum)
            return *it;
    }

    return NULL;
}

int LSP_Broker::HandleReplyMessage (api_msg* rmsg)
{
    int ret = -1;

    if (rmsg) {
        api_writer->PostMessage(rmsg);
        ret = 0;
    }

    return ret;
}


void LSP_Broker::DescribeLSPbyState(u_char state, vector<string> & desc_v)
{
    string desc;
    list<LSPQ*>::iterator it;
    for (it = lspq_list.begin(); it != lspq_list.end(); it++)
    {
        if (state == 0xff || (*it)->State() == state)
        {
            (*it)->DescribeLSP(desc);
            desc_v.push_back(desc);
        }
    }    
}


u_int32_t LSP_Broker::get_unique_lspb_id()
{
    static u_int32_t id = 1;
    return id++;
}
