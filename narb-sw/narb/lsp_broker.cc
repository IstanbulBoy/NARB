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

list<struct ConfirmationIDIndxedEROWithTimer*> LSP_Broker::qconf_id_indexed_ero_list;

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
    req_spec.length = htons(sizeof(req_spec) - TLV_HDR_SIZE);
    mrn_spec = req_spec;
    vtag_mask = NULL;
    suggested_vtag = NULL;
    previous_lspb_id = 0;
    hop_back = 0;
    is_recursive_req = false;
    is_qconf_mode = false;
}

LSPQ::~LSPQ()
{
    list<ero_subobj*>::iterator it;

    for (it = ero.begin(); it != ero.end(); it++)
        if (*it)
            delete (ero_subobj*)(*it);

    if (vtag_mask)
        delete vtag_mask;

    if (suggested_vtag)
        delete suggested_vtag;
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

void LSPQ::SetState(u_char s)
{
   if (state == STATE_RESV_CONFIRM && s < state)
   {
       LOGF("#### state == STATE_RESV_CONFIRM, to be changed into %d\n",  s);
   }

   state = s;
}

void LSPQ::GetERO(te_tlv_header* tlv, list<ero_subobj*>& ero)
{
    assert (tlv);
    ero.clear();
    int len = ntohs(tlv->length);
    assert( len > 0 && len% sizeof(ero_subobj) == 0);

    char addr[20]; //debug
    ero_subobj * subobj  = (ero_subobj *)((char *)tlv + TLV_HDR_SIZE);
    for (; len > 0 ;subobj++, len -= sizeof(ero_subobj))
    {
        inet_ntop(AF_INET, &subobj->addr, addr, 20); //debug
        LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag)); //debug
        ero_subobj * new_subobj = (ero_subobj*)new(struct ero_subobj);
        memcpy(new_subobj, subobj, sizeof(ero_subobj));
        ero.push_back(new_subobj);
    }

    if (Log::log_file)
    	Log::log_file->flush();
}

void LSPQ::GetERO_RFCStandard(te_tlv_header* tlv, list<ero_subobj*>& ero)
{
    char addr[20]; //debug
    assert (tlv);
    ero.clear();

    int len = ntohs(tlv->length);
    if (len <= 0)
        return;
    
    int offset = TLV_HDR_SIZE;

    ipv4_prefix_subobj* subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
    unum_if_subobj* subobj_unum;
    while (len > 0)
    {
        ero_subobj * new_subobj = (ero_subobj*)new(struct ero_subobj);
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
            LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d (%d,%d)]\n", (subobj_unum->l_and_type & (1<<7)) == 0?"strict":"loose", addr, ntohl(new_subobj->if_id), ntohl(new_subobj->if_id)>>16, (u_int16_t)ntohl(new_subobj->if_id)); //debug
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

    if (Log::log_file)
    	Log::log_file->flush();
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
        //removing duplicates
        if (ero_intra.begin() != ero_intra.end() && (*node_inter)->addr.s_addr == (*ero_intra.begin())->addr.s_addr)
        {
            node_inter++;
            continue;
        }

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

void LSPQ::SetVtagToERO(list<ero_subobj*>& ero, u_int32_t vtag)
{
    list<ero_subobj*>::iterator iter;
    ero_subobj *subobj;

    for (iter = ero.begin(); iter != ero.end(); iter++)
    {
        subobj = *iter;
        if (subobj->sw_type != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
            continue;
        if (subobj->l2sc_vlantag != 0)
            subobj->l2sc_vlantag = vtag;
        if ((subobj->if_id >> 16) == 0)
            subobj->if_id = vtag;
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////// LSPQ FSM Functions ////////////////////////
//////////////////////////////////////////////////////////////////


/////// STATE_APP_REQ ////////
int LSPQ::HandleLSPQRequest()
{
    SetState(STATE_APP_REQ);
    is_recursive_req = false;
    is_qconf_mode = ( (app_options & LSP_OPT_QUERY_CONFIRM) != 0  || SystemConfig::routing_mode == RT_MODE_MIXED_CONFIRMED );

    int ret; 

    //handling LSP request with Q-Conf ID
    if ((app_options & LSP_OPT_QUERY_CONFIRM) != 0)
    {
        ret = HandleStoredEROWithConfirmationID();
        if (ret >= 0)
            return ret;
        //else, continue in this function ...
    }

    //instantiating RCE client
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

    //return manual/static ERO based on CLI configuration
    if (SystemConfig::use_manual_ero && SystemConfig::manual_ero.size() > 0)
    {
        ero.clear();
        ero_subobj* subobj;
        list<ero_subobj*>::iterator it = SystemConfig::manual_ero.begin();
        for ( ; it != SystemConfig::manual_ero.end(); it++)
        {
            subobj = new (struct ero_subobj);
            memcpy(subobj, *it, sizeof(struct ero_subobj));
            ero.push_back( subobj);
        }
        return HandleCompleteERO();
    }

    msg_narb_cspf_request cspf_req;
    cspf_req.app_req_data = req_spec;
    cspf_req.app_seqnum = app_seqnum;
    cspf_req.lspb_id = broker->lspb_id; //not used (replaced by ucid)

    if (vtag_mask)
        app_options |= LSP_OPT_VTAG_MASK;
    if (req_vtag == ANY_VTAG || vtag_mask) // to make interdomain routing more acurate!
        app_options |= LSP_OPT_REQ_ALL_VTAGS;
    rce_client->QueryLsp(cspf_req, req_ucid, app_options | LSP_TLV_NARB_CSPF_REQ | (app_options & LSP_OPT_STRICT ? LSP_OPT_PREFERRED : 0)
        | (app_options & LSP_OPT_QUERY_HOLD) , req_vtag, hop_back, vtag_mask);
    return 0;
}

/////// STATE_REQ_RECURSIVE ////////
int LSPQ::HandleRecursiveRequest()
{
    SetState(STATE_REQ_RECURSIVE);
    is_recursive_req = true;
    is_qconf_mode = ( (app_options & LSP_OPT_QUERY_CONFIRM) != 0  || SystemConfig::routing_mode == RT_MODE_MIXED_CONFIRMED );

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
    cspf_req.lspb_id = broker->lspb_id; //not used, replaced by ucid

    if (vtag_mask)
        app_options |= LSP_OPT_VTAG_MASK;
    if (req_vtag == ANY_VTAG || vtag_mask)
        app_options |= LSP_OPT_REQ_ALL_VTAGS;
    rce_client->QueryLsp_MRN(cspf_req, mrn_spec, req_ucid, SystemConfig::rce_options | LSP_OPT_STRICT | LSP_OPT_PREFERRED 
	| (app_options & LSP_OPT_E2E_VTAG) |(app_options & LSP_OPT_VIA_MOVAZ) | (app_options & LSP_OPT_QUERY_HOLD)
	| (vtag_mask ? LSP_OPT_VTAG_MASK : 0) | (req_vtag == ANY_VTAG || vtag_mask ? LSP_OPT_REQ_ALL_VTAGS : 0)
	| (app_options & LSP_OPT_BIDIRECTIONAL), req_vtag, hop_back, vtag_mask); 
}

/////// STATE_RCE_REPLY  ////////
int LSPQ::HandleRCEReply(api_msg *msg)
{
    if (state == STATE_RESV_CONFIRM || state == STATE_RESV_RELEASE || state == STATE_RESV_RELEASE_CONFIRM)
    {
        api_msg_delete(msg);
        return 0;
    }

    //special handling for local rerouting based on stored, expired ERO, in which case only all-strict local hop ERO is acceptable
    bool local_rerouting_requires_all_strict_hops = false;
    if (is_qconf_mode && state == STATE_STORED_ERO_WITH_CONFIRMATION_ID)
    {
        ConfirmationIDIndxedEROWithTimer *qConfEROTimer = LSP_Broker::LookupEROWithConfirmationID(req_ucid, app_seqnum, req_spec.src.s_addr);
        if (qConfEROTimer && qConfEROTimer->GetBorderEgressLink())
            local_rerouting_requires_all_strict_hops = true;        
    }
    
    SetState(STATE_RCE_REPLY);

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
        if (suggested_vtag && vtag_mask && HAS_VLAN(vtag_mask->bitmask, suggested_vtag->suggested_vtag))
        {
            //try using suggested_vtag first
            req_vtag = suggested_vtag->suggested_vtag;
        }
        else if ((app_options & LSP_OPT_E2E_VTAG) && req_vtag == ANY_VTAG)
        {
            //otherwise use the vtag picked by RCE
            req_vtag = ntohl(msg->header.tag);
            if (req_vtag == ANY_VTAG)
            {
                LOG("The impossible happened: RCE returns ANY_VTAG." << endl);
                return HandleErrorCode(NARB_ERROR_NO_ROUTE);
            }
        }
    }
    api_msg_delete(msg);    
    
    if (ero.size() == 0)
        return HandleErrorCode(NARB_ERROR_INTERNAL);

    if (is_qconf_mode) // including "case RT_MODE_MIXED_CONFIRMED" 
    { 
        // NARB is in Qconf mode but the request is for query only
        if ((app_options & (LSP_OPT_QUERY_HOLD|LSP_OPT_QUERY_CONFIRM)) == 0)
        {
            // return whatever available without proceed to next domains ???
            return HandleCompleteERO();
        }
        else if (!is_all_strict_hops(ero))
            NarbDomainInfo.SearchAndProcessInterdomainLink(ero);

        if (is_all_strict_hops(ero))
        {
            // >>>> re-pick a vlan tag --> randomize?
            return HandleCompleteEROWithConfirmationID();
        }
        else if (!is_all_loose_hops(ero) && !is_all_strict_hops(ero) && !local_rerouting_requires_all_strict_hops)
        {
            //return HandleCompleteEROWithConfirmationID();
            return HandlePartialERO();
        }
        else //e.g. still all loose hops after SearchAndProcessInterdomainLink(ero)
        {
            return HandleErrorCode(NARB_ERROR_NO_ROUTE);
        }
    }
    else switch (SystemConfig::routing_mode)
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
        {
            NarbDomainInfo.SearchAndProcessInterdomainLink(ero);
            return HandleCompleteERO();
        }
         else
            return HandleErrorCode(NARB_ERROR_NO_ROUTE);
        break;            
    case RT_MODE_ALL_LOOSE_ALLOWED:
        if(!is_all_loose_hops(ero))
            NarbDomainInfo.SearchAndProcessInterdomainLink(ero);
        return HandleCompleteERO();
        break;
    }

    return 0;
}


/////// STATE_ERO_PARTIAL  ////////
int LSPQ::HandlePartialERO()
{
    SetState(STATE_ERO_PARTIAL);

    int ret; 

    assert (SystemConfig::routing_mode == RT_MODE_ALL_STRICT_ONLY
               ||SystemConfig::routing_mode == RT_MODE_MIXED_ALLOWED
               ||is_qconf_mode); 
    

    NarbDomainInfo.SearchAndProcessInterdomainLink(ero);

    if (is_all_strict_hops(ero))
        return HandleCompleteERO();

    ero_subobj* last_strict_hop_subobj = last_strict_hop(ero);
    if (!last_strict_hop_subobj)
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);

    //this should be the interdomain link; DRAGN assumes interface addresses are unique for interdomain links
    link_info * link = NarbDomainInfo.LookupLinkByRmtIf(last_strict_hop_subobj->addr);
    if (!link)
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);

    msg_narb_recursive_cspf_request rec_cspf_req;
    memset(&rec_cspf_req, 0, sizeof(struct msg_narb_recursive_cspf_request));
    rec_cspf_req.app_seqnum = app_seqnum;
    rec_cspf_req.lspb_id = broker->lspb_id; //not used (replaced by ucid)
    rec_cspf_req.app_req_data = req_spec;
    rec_cspf_req.app_req_data.type = htons(MSG_PEER_REQUEST);
    rec_cspf_req.rec_req_data = rec_cspf_req.app_req_data;
    rec_cspf_req.rec_req_data.src.s_addr = link->Id();

    NARB_APIClient * peer_narb = NarbFactory.GetClient(last_strict_hop_subobj->addr);

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
    ero_subobj* new_src_subobj = first_loose_hop(ero);
    if (new_src_subobj->sw_type != rec_cspf_req.app_req_data.switching_type ||
        new_src_subobj->encoding != rec_cspf_req.app_req_data.encoding_type)
    {
        rec_cspf_req.rec_req_data.switching_type = new_src_subobj->sw_type;
        rec_cspf_req.rec_req_data.encoding_type = new_src_subobj->encoding;
        rec_cspf_req.rec_req_data.bandwidth = new_src_subobj->bandwidth;
    }

    //$$$$ options LSP_OPT_QUERY_CONFIRM and LSP_OPT_QUERY_CONFIRM are forwarded 
    //$$$$ use last strict hop address into hop_back into recursive LSP query
    peer_narb->QueryLspRecursive(rec_cspf_req, req_ucid, app_options | LSP_OPT_STRICT & (~ LSP_OPT_PREFERRED) | (is_qconf_mode ? LSP_OPT_QUERY_CONFIRM : 0), 
        req_vtag, (last_strict_hop_subobj ? last_strict_hop_subobj->addr.s_addr : 0), vtag_mask);
}

/////// STATE_NEXT_HOP_NARB_REPLY  ////////
int LSPQ::HandleNextHopNARBReply(api_msg *msg)
{
    SetState(STATE_NEXT_HOP_NARB_REPLY);

    bool had_vtag_mask = (vtag_mask != NULL);
    HandleOptionalResponseTLVs(msg);

    list<ero_subobj*> rec_ero;
    te_tlv_header *tlv = (te_tlv_header*)msg->body;
    switch ( ntohs(msg->header.type) )
    {
    case MSG_REPLY_ERROR:
        assert (ntohs(tlv->type) == TLV_TYPE_NARB_ERROR_CODE);
        api_msg_delete(msg);
        //goto _CANNOT_EXPAND_ROUTE;
        break;

    case MSG_REPLY_ERO:
        assert (ntohs(tlv->type) == TLV_TYPE_NARB_ERO);
        GetERO_RFCStandard((te_tlv_header*)msg->body, rec_ero);
        if (msg->header.tag != 0 && ntohl(msg->header.tag) != ANY_VTAG) {
            req_vtag = ntohl(msg->header.tag);
            //The embeded vtag in ero LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL subojects must be equal to req_tag.
            list<ero_subobj*>::iterator it = ero.begin();
            for ( ; it != ero.end(); it++)
            {
                if ( (ntohl((*it)->if_id)>>16) == LOCAL_ID_TYPE_TAGGED_GROUP 
                    || (ntohl((*it)->if_id)>>16) == LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL)
                {
                    (*it)->if_id = htonl((ntohl((*it)->if_id) & 0xffff0000) | (req_vtag & 0xffff));
                }
                else if ((*it)->l2sc_vlantag != 0)
                {
                    (*it)->l2sc_vlantag = (u_int16_t)(req_vtag & 0xffff);
                }
            }
        }

        // $$$$ Should not receirve QUERY_CONFIRM option along with ERO from next hop NARB 
        assert ( (ntohl(msg->header.options) & LSP_OPT_QUERY_CONFIRM) == 0);

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
        break;

    case MSG_REPLY_CONFIRMATION_ID:
        assert(is_qconf_mode);
        // Verifying confirmation ID
        LOGF("HandleNextHopNARBReply:: receieved MSG_REPLY_CONFIRMATION_ID message(ucid=%x,seqnum=%x)\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
        if (ntohl(msg->header.options) & LSP_OPT_QUERY_CONFIRM == 0)
        {
            LOGF("HandleNextHopNARBReply::Error: no LSP_OPT_QUERY_CONFIRM option flag in MSG_REPLY_CONFIRMATION_ID message header.\n");
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }
        if (ntohl(msg->header.ucid) != req_ucid || ntohl(msg->header.seqnum) !=  app_seqnum)
        {
            LOGF("HandleNextHopNARBReply::Error: Confirmation ID in recursive LSPQ reply message mismatches with the local LSPQ (ucid=%x,seqnum=%x).\n", req_ucid, app_seqnum);
            return HandleErrorCode(NARB_ERROR_INTERNAL);
        }

        // Checking the vlan tag (against the returned picked one)
        if (ntohl(msg->header.tag) == ANY_VTAG)
        {
                LOGF("HandleNextHopNARBReply::Error: ANY_VTAG is received along with MSG_REPLY_CONFIRMATION_ID message.\n");
                return HandleErrorCode(NARB_ERROR_NO_ROUTE);                                
        }
        else if (msg->header.tag != 0) 
        {
            req_vtag = ntohl(msg->header.tag);
            // update vtag in all ero subojects.
            SetVtagToERO(ero, req_vtag);
            if (had_vtag_mask)
            {
                //if(vtag_mask && !HAS_VLAN(vtag_mask->bitmask, req_vtag))
                //{
                //    LOGF("HandleNextHopNARBReply::Error: The VLAN tag %d returned along with the confirmation ID message mismatches with the pre-calculated VLAN tag mask.\n", req_vtag);
                //    return HandleErrorCode(NARB_ERROR_NO_ROUTE);                    
                //}
                // @@@@ Verify if the picked VTAG is still available <== (needed only if vtag_mask holding is active!)
                
                if (!is_all_loose_hops(ero) && (app_options&LSP_OPT_QUERY_HOLD) != 0) // only if holding is requested!
                {
                    //notify the corresponding RCE server for LSP query confirmation that may have changed vlan tag assignment
                    RCE_APIClient* rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
                    int ret = 0;
                    if (!rce_client)
                    {
                        ret = -1;
                        LOGF("HandleNextHopNARBReply::Error: no RCE client available...\n");
                    }  
                    else if (!rce_client->IsAlive())
                    {
                        ret = rce_client->Connect();
                        if (ret < 0)
                        {
                            LOGF("HandleNextHopNARBReply::Error: RCE client died and connect failed...\n");
                        }
                    } //else ret == 0;
                    if (ret >= 0)
                    {
                        //single (picked) vlan tag instead of vtag_mask!==> (needed only if vtag_mask holding is active!)
                        rce_client->NotifyResvStateWithERO(MSG_LSP, ACT_UPDATE, &req_spec, ero, req_ucid, app_seqnum, app_options, req_vtag, NULL);
                    }
                }
            }
        }           

        // return complete ERO with confirmation ID (or confirmation ID alone)
        return HandleCompleteEROWithConfirmationID();
        api_msg_delete(msg);        
        break;
    }

    //Unsucessful recursive routing
_CANNOT_EXPAND_ROUTE:
    if ( SystemConfig::routing_mode == RT_MODE_MIXED_ALLOWED && !is_qconf_mode )
        return HandleCompleteERO();
    else 
        return HandleErrorCode(NARB_ERROR_NO_ROUTE);
}

/////// STATE_ERROR ////////
int LSPQ::HandleErrorCode(u_int32_t errcode)
{
    // If having proceeded into next domains but got error, release the currently held resources (delta deletion)...
    if (state ==STATE_NEXT_HOP_NARB_REPLY && !is_all_loose_hops(ero) && (app_options&LSP_OPT_QUERY_HOLD) != 0) // only if holding is requested!
    {
        //notify the corresponding RCE server for LSP query confirmation that may have changed vlan tag assignment
        RCE_APIClient* rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
        int ret = 0;
        if (!rce_client)
        {
            ret = -1;
            LOGF("HandleNextHopNARBReply::Error: no RCE client available...\n");
        }  
        else if (!rce_client->IsAlive())
        {
            ret = rce_client->Connect();
            if (ret < 0)
            {
                LOGF("HandleNextHopNARBReply::Error: RCE client died and connect failed...\n");
            }
        } //else ret == 0;
        if (ret >= 0)
        {
            //vtag and vtag_mask doesn't matter for delta deletion
            rce_client->NotifyResvStateWithERO(MSG_LSP, ACT_DELETE, &req_spec, ero, req_ucid, app_seqnum, app_options, 0, NULL);
        }
    }

    // regular Error Code handling
    SetState(STATE_ERROR);

    assert(broker);

    api_msg* rmsg;

    rmsg = narb_new_msg_reply_error(req_ucid, app_seqnum, errcode, is_recursive_req? previous_lspb_id : 0);
    assert(rmsg);
    rmsg->header.tag = htonl(req_vtag);

    broker->HandleReplyMessage(rmsg);
    return 0;
}

/////// STATE_ERO_COMPLETE ////////
int LSPQ::HandleCompleteERO()
{
    SetState(STATE_ERO_COMPLETE);

    int length;
    void * tlv_data;
    api_msg * rmsg;

    assert(broker);

    rmsg = narb_new_msg_reply_ero(req_ucid, app_seqnum, ero, (app_options & LSP_OPT_REQ_ALL_VTAGS) == 0 ? NULL : vtag_mask, is_recursive_req? previous_lspb_id : 0);
    if (!rmsg)
        HandleErrorCode(NARB_ERROR_INTERNAL);
    rmsg->header.tag = htonl(req_vtag);

    broker->HandleReplyMessage(rmsg);
    return 0;
}

// $$$$ Other state?
/////// STATE_ERO_COMPLETE ////////
int LSPQ::HandleCompleteEROWithConfirmationID()
{
    int length;
    void * tlv_data;
    api_msg * rmsg;

    assert(broker);

    if (is_recursive_req && state != STATE_STORED_ERO_WITH_CONFIRMATION_ID) // confirmation ID only (w/o ERO TLV)
    {
        ConfirmationIDIndxedEROWithTimer *qConfEROTimer = LSP_Broker::StoreEROWithConfirmationID(ero, req_ucid, app_seqnum, req_spec.src.s_addr);
        if (qConfEROTimer)
        {
            eventMaster.Schedule(qConfEROTimer);
        }

        char buf[8];
        int mlen = 0;
        if (previous_lspb_id != 0)
        {
            msg_app2narb_lspb_id *lspb_id_tlv = (msg_app2narb_lspb_id*)(buf + mlen);
            lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
            lspb_id_tlv->length = htons(sizeof(msg_app2narb_lspb_id) - TLV_HDR_SIZE);
            lspb_id_tlv->lspb_id = previous_lspb_id;
            mlen += sizeof(msg_app2narb_lspb_id);
        }
        
        rmsg = api_msg_new (MSG_REPLY_CONFIRMATION_ID, mlen, buf, req_ucid, app_seqnum, req_vtag);
        LOGF("HandleCompleteEROWithConfirmationID:: For recursive query, sending back confirmation ID only (ucid=%x,seqnum=%x), vtag=%d\n", req_ucid, app_seqnum, req_vtag);
    }
    else //ERO and confirmation ID (ucid, seqnum)
    {
        // handling the special case that a valid stored ERO shall be returned, which may have been locally rerouted
        if (state == STATE_STORED_ERO_WITH_CONFIRMATION_ID) 
        {
            ConfirmationIDIndxedEROWithTimer *qConfEROTimer = LSP_Broker::LookupEROWithConfirmationID(req_ucid, app_seqnum, req_spec.src.s_addr);
            link_info * link_bdr_egress;
            if (qConfEROTimer && (link_bdr_egress = qConfEROTimer->GetBorderEgressLink()) != NULL)
            {
                LOGF("HandleCompleteEROWithConfirmationID:: Local reroute for Q-Conf (ucid=%x,seqnum=%x) (src: 0x%x --> dest: 0x%x).\n", req_ucid, app_seqnum, req_spec.src.s_addr, req_spec.dest);
                //combine local re-route results with stored ERO
                ero_subobj* subobj; 
                //locating the border egress hop
                list<ero_subobj*>* ero_p = &qConfEROTimer->GetERO();
                assert (ero_p);
                list<ero_subobj*>::iterator iter = ero_p->begin();
                for ( ; iter != ero_p->end(); iter++ )
                {
                    subobj = *iter;
                    if (link_bdr_egress->RmtIfAddr() == subobj->addr.s_addr)
                    {
                        iter++;
                        break;
                    }
                }
                //appending the next-domain hops in the stored ERO
                for ( ; iter != ero_p->end(); iter++)
                {
                    subobj = new (struct ero_subobj);
                    *subobj = *(*iter);
                    ero.push_back(subobj);
                }
            }
        }

        // handling hop_back for the stored (and some rerouted) ERO ...
        bool hop_back_found = false;
        list<ero_subobj*>::iterator iter = ero.begin();
        for ( ; iter != ero.end(); iter++ )
        {
            if ((*iter)->addr.s_addr == hop_back)
            {
                hop_back_found = true;
                break;
            }
        }
        // adding hop_back to beginning of the stored ERO.
        if (hop_back != 0 && !hop_back_found)
        {
            ero_subobj* subobj = new (struct ero_subobj);
            *subobj = *ero.front();
            subobj->addr.s_addr = hop_back;
            ero.push_front(subobj);
        }

        rmsg = narb_new_msg_reply_ero(req_ucid, app_seqnum, ero, (app_options & LSP_OPT_REQ_ALL_VTAGS) == 0 ? NULL : vtag_mask, is_recursive_req ? previous_lspb_id : 0);
        LOGF("HandleCompleteEROWithConfirmationID:: For orignal LSPQuery, sending back  ERO with confirmation ID (ucid=%x,seqnum=%x)\n", req_ucid, app_seqnum);
    }

    SetState(STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID);
    
    if (!rmsg)
        HandleErrorCode(NARB_ERROR_INTERNAL);

    rmsg->header.tag = htonl(req_vtag);
    rmsg->header.options |= htonl(LSP_OPT_QUERY_CONFIRM);

    broker->HandleReplyMessage(rmsg);
    return 0;
}

static in_addr LookupPeerNarbByERO(list<ero_subobj*>& ero)
{
    in_addr ret_ip;
    ret_ip.s_addr = 0;
    if_narb_info * narb_info; 
    list<ero_subobj*>::iterator it;
    link_info *link1, *link2;

    for (it = ero.begin(); it != ero.end(); it++)
    {
        narb_info = NarbDomainInfo.LookupNarbByRmtIf((*it)->addr);
        if (narb_info && it != ero.begin()) 
        {
            --it;
            link1 = NarbDomainInfo.LookupLinkByLclIf((*it)->addr);
            ++it;
            link2 = NarbDomainInfo.LookupLinkByRmtIf((*it)->addr);
            if (link1 == link2 ) // this subojct must pair up with its previsous (not next) suboject to form a link from this domain.
            {
                ret_ip.s_addr = (*it)->addr.s_addr;

                ++it;
                if (it == ero.end())//last subobject ineligible
                    ret_ip.s_addr = 0;
                // $$$$ This condition does not allow RelayMessageToPeer if the ERO has no strict hops in next domains,
                // $$$$ which is usually necessary in hybrid routing mode!
                else if ((*it)->hop_type == ERO_TYPE_LOOSE_HOP) // should still have strict hops after the inter-domain link.
                    ret_ip.s_addr = 0;

                break;
            }
        }
    }
    return ret_ip;
    //note: assuming both local and remote interface addresses are unique for inter-domain te links
}

/////// STATE_STORED_ERO_WITH_CONFIRMATION_ID ////////
int LSPQ::HandleStoredEROWithConfirmationID()
{
    SetState(STATE_STORED_ERO_WITH_CONFIRMATION_ID);

    ConfirmationIDIndxedEROWithTimer* qConfEROTimer = LSP_Broker::LookupEROWithConfirmationID(req_ucid, app_seqnum, req_spec.src.s_addr);
    if (qConfEROTimer)
    {
        list<ero_subobj*>* ero_p = &qConfEROTimer->GetERO();
        list<ero_subobj*>::iterator iter;
        ero_subobj* subobj;

        if (qConfEROTimer->Expired())
        {
            LOGF("LSP_Broker::HandleStoredEROWithConfirmationID qconf ID (ucid=0x%x, seqnum=0x%x, source_ip=0x%x) has Expired!\n", req_ucid, app_seqnum, req_spec.src.s_addr);

            // identify the interdomian link iterface (hop) in the stored ERO that is originated from Egress border router in the current domain
            if_narb_info * narb_info;
            link_info * link_bdr_egress;
            for ( iter = ero_p->begin(); iter != ero_p->end(); iter++ )
            {
                subobj = (*iter);
                if (subobj->hop_type = ERO_TYPE_LOOSE_HOP && (narb_info = NarbDomainInfo.LookupNarbByRmtIf((*iter)->addr)) != NULL)
                {
                    if ((link_bdr_egress = NarbDomainInfo.LookupLinkByRmtIf((*iter)->addr)) != NULL)
                    {
                        // mark the border egress link
                        qConfEROTimer->SetBorderEgressLink(link_bdr_egress);
                        // change destination to the next-domain border router ID
                        req_spec.dest.s_addr = link_bdr_egress->ID();
                        return -1; // path recomputation towards next domain border router
                    }
                }
            }
            return -2; // path recomputation towards global destination
        }
        else // valid, active stored ERO
        {
            LOGF("LSP_Broker::HandleStoredEROWithConfirmationID qconf ID (ucid=0x%x, seqnum=0x%x, source_ip=0x%x) indexed ERO is returned!\n", req_ucid, app_seqnum, req_spec.src.s_addr);
            char addr[20];
            ero.clear();
            for ( iter = ero_p->begin(); iter != ero_p->end(); iter++ )
            {
                subobj = new (struct ero_subobj);
                *subobj = *(*iter);
                ero.push_back(subobj);
                // log message for debugging
                inet_ntop(AF_INET, &subobj->addr, addr, 20);
                LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  
                    ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag));
            }
        }
    }
    else 
    {
        // path recomputation towards original global destination
        return -2;
    }

    return HandleCompleteEROWithConfirmationID();
}

/////// STATE_RESV_CONFIRM ////////
int LSPQ::HandleResvConfirm(api_msg* msg)
{
    if (state != STATE_ERO_COMPLETE && state != STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID)
    {
        LOGF("Trying to confirm an uncomputed LSP: (ucid=0x%x, seqno=0x%x).\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
        return -1;
    }

    SetState(STATE_RESV_CONFIRM);

    msg_app2narb_confirm* app_msg = (msg_app2narb_confirm*)msg->body;

    list<ero_subobj*> ero_confirm;
    GetERO_RFCStandard(&app_msg->ero, ero_confirm);
    if(ero_confirm.size() <= 0)
    {
        LOG("LSPQ::HandleResvConfirm cannot handle empty ERO" << endl);
        return -1;
    }

    LOGF("HandleResvConfirm upating LSP link states: (ucid=0x%x, seqno=0x%x).\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));

    //get peer NARB address from ero_confirm list before it is reduced
    in_addr narb_ip = LookupPeerNarbByERO(ero_confirm);
    list<ero_subobj*>::iterator it;
    ero_subobj* subobj, *reverse_subobj;
    link_info *link1, *link2;
    link_info *reverse_link1, *reverse_link2;
    bool is_forward_link = false;
    u_int32_t lsp_vtag = 0;

    for (it = ero_confirm.begin(); it != ero_confirm.end();  it++)
    {
        if ( (ntohl((*it)->if_id) >> 16)  == LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL )
        {
            lsp_vtag = (ntohl((*it)->if_id) & 0xffff);
            break;
        }
        
        if ((lsp_vtag = (*it)->l2sc_vlantag) !=0)
        {
            break;
        }
    }

    for (it = ero_confirm.begin(); it != ero_confirm.end();  it++)
    {
        u_int32_t vtag = 0;
        subobj = *it;

        if (subobj->hop_type == ERO_TYPE_LOOSE_HOP)
        {
            continue;
        }

        if (subobj->addr.s_addr == app_msg->req.src.s_addr ||subobj->addr.s_addr == app_msg->req.dest.s_addr)
        {
            continue;
        }
        is_forward_link = (!is_forward_link);
        if (!is_forward_link && (app_options & LSP_OPT_BIDIRECTIONAL) == 0) //ignore reverse link for unidirectional request
        {
            continue;
        }
        link1 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr);
        while (link1 != NULL) // updating all links with the same local interface address
        {
            vtag = subobj->l2sc_vlantag;
            if (vtag == 0 && lsp_vtag != 0 && link1->info_flag & LINK_PARA_FLAG_VLAN &&
                ((ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST))
            {
                vtag = lsp_vtag;
            }
            if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    link1->UnreservedBandwidth()[i] -= app_msg->req.bandwidth;
                    if ( link1->UnreservedBandwidth()[i] < 0)
                         link1->UnreservedBandwidth()[i] = 0;
                    link1->GetISCD()->max_lsp_bw[i] -= app_msg->req.bandwidth;
                    if (link1->GetISCD()->max_lsp_bw[i] < 0)
                        link1->GetISCD()->max_lsp_bw[i] = 0;
                }
                if (vtag != 0)
                {
                    link1->ResetVtag(vtag);
                    link1->AllocateVtag(vtag);
                }
                // update the link to inter-domain topology if the narb working mode is dynamic
                if (SystemConfig::working_mode == WORKING_MODE_DYNAMIC_INTERDOMAIN_TOPOLOGY)
                {
                    NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), link1);
                }
            }
            link1 = NarbDomainInfo.LookupNextLinkByLclIf(link1);
        }
    }

    //forward the message to the corresponding RCE server as an LSP reservation confirmation
    RCE_APIClient* rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
    int ret = 0;
    if (!rce_client)
    {
        ret = -1;
        LOGF("HandleResvConfirm::Error: no RCE client available...\n");
    }  
    else if (!rce_client->IsAlive())
    {
        ret = rce_client->Connect();
        if (ret < 0)
        {
            LOGF("HandleResvConfirm::Error: dead RCE client died and connect failed...\n");
        }
    } //else ret == 0;
    if (ret >= 0)
    {
        rce_client->NotifyResvStateWithERO(MSG_LSP, ACT_CONFIRM, msg, ero_confirm);
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
            HandleErrorCode(NARB_ERROR_INTERNAL);
            SetState(STATE_RESV_CONFIRM);
            return -1;
        }
    }
    peer_narb->RelayMessageToPeer(MSG_APP_CONFIRM, msg, ero_confirm);

    return 0;
}

/////// STATE_RESV_RELEASE ////////
int LSPQ::HandleResvRelease(api_msg* msg)
{
    int ret = 0;
    NARB_APIClient * peer_narb = NULL;
    RCE_APIClient * rce_client = NULL;
    list<ero_subobj*>::iterator it;
    ero_subobj* subobj, *reverse_subobj;
    link_info *link1, *link2;
    link_info *reverse_link1, *reverse_link2;
    list<ero_subobj*> ero_confirm;
    msg_app2narb_confirm* app_msg;
    in_addr narb_ip;
    bool is_forward_link = false;
    u_int32_t lsp_vtag = 0;

    LOGF("HandleResvRelease upating LSP link states: (ucid=0x%x, seqno=0x%x).\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));

    //if (state != STATE_RESV_CONFIRM)
    if (state != STATE_ERO_COMPLETE && state != STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID && state != STATE_RESV_CONFIRM)
    {
        LOGF("Trying on an unconfirmed (state = %d) LSP (ucid=0x%x, seqno=0x%x).\n", state, ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
        // sending back relesae confirmation anyway
        //@@@@ TEMP COMMENT OUT
        return HandleResvReleaseConfirm();
    }

    SetState(STATE_RESV_RELEASE);

    app_msg = (msg_app2narb_confirm*)msg->body;

    GetERO_RFCStandard(&app_msg->ero, ero_confirm);

    //get peer NARB address from ero_confirm list before it is reduced
    narb_ip = LookupPeerNarbByERO(ero_confirm);

    if(ero_confirm.size() <= 0)
    {
        LOG("LSPQ::HandleResvRelease cannot handle empty ERO" << endl);
        ret = -1;
        goto _out;
    }

    for (it = ero_confirm.begin(); it != ero_confirm.end();  it++)
    {
        if ( (ntohl((*it)->if_id) >> 16)  == LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL )
        {
            lsp_vtag = (ntohl((*it)->if_id) & 0xffff);
            break;
        }
        
        if ((lsp_vtag = (*it)->l2sc_vlantag) !=0)
        {
            break;
        }
    }

    for (it = ero_confirm.begin(); it != ero_confirm.end(); it++)
    {
        u_int32_t vtag = 0;
        subobj = *it;

        if (subobj->hop_type == ERO_TYPE_LOOSE_HOP)
        {
            continue;
        }

        if (subobj->addr.s_addr == app_msg->req.src.s_addr ||subobj->addr.s_addr == app_msg->req.dest.s_addr)
        {
            continue;
        }
        is_forward_link = (!is_forward_link);
        if (!is_forward_link && (app_options & LSP_OPT_BIDIRECTIONAL) == 0) //ignore reverse link for unidirectional request
        {
            continue;
        }
        link1 = NarbDomainInfo.LookupLinkByLclIf(subobj->addr);
        while (link1 != NULL)
        {
            vtag = subobj->l2sc_vlantag;
            if (vtag == 0 && lsp_vtag != 0 && link1->info_flag & LINK_PARA_FLAG_VLAN &&
                ((ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST))
            {
                vtag = lsp_vtag;
            }
            if(zebra_client && zebra_client->GetWriter() && zebra_client->GetWriter()->Socket() > 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    link1->UnreservedBandwidth()[i] += app_msg->req.bandwidth;
                    if (link1->UnreservedBandwidth()[i] > link1->MaxBandwidth())
                        link1->UnreservedBandwidth()[i] = link1->MaxBandwidth();
                    link1->GetISCD()->max_lsp_bw[i] += app_msg->req.bandwidth;
                    if (link1->GetISCD()->max_lsp_bw[i] > link1->MaxBandwidth())
                        link1->GetISCD()->max_lsp_bw[i] = link1->MaxBandwidth();
                }
                if (vtag != 0)
                {
                    link1->SetVtag(vtag);
                    link1->DeallocateVtag(vtag);
                }
                // update the link to inter-domain topology if the narb working mode is dynamic
                if (SystemConfig::working_mode == WORKING_MODE_DYNAMIC_INTERDOMAIN_TOPOLOGY)
                {
                    NarbDomainInfo.UpdateTeLink(zebra_client->GetWriter(), link1);
                }
            }
            link1 = NarbDomainInfo.LookupNextLinkByLclIf(link1);
        }
    }

    //forward the message to the corresponding RCE server as an LSP release notification
    rce_client  = RceFactory.GetClient((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
    if (!rce_client)
    {
        ret = -1;
        LOGF("HandleResvRelease::Error: no RCE client available...\n");
    }  
    else if (!rce_client->IsAlive())
    {
        ret = rce_client->Connect();
        if (ret < 0)
        {
            LOGF("HandleResvRelease::Error: dead RCE client died and connect failed...\n");
        }
    }
    else 
    {
        ret = 0;
    }
    if (ret >= 0)
    {
        rce_client->NotifyResvStateWithERO(MSG_LSP, ACT_DELETE, msg, ero_confirm);
    }

    //proceed to the next domain if any
    if(narb_ip.s_addr != 0)
    {
        peer_narb = NarbFactory.GetClient(narb_ip);
    }
    else
        goto _out;
    if(!peer_narb)
    {
        ret = -1;
        goto _out;
    }
    if (!peer_narb->IsAlive())
    {
        ret = peer_narb->Connect();
        if (ret < 0)
        {
            NarbFactory.RemoveClient(peer_narb);
            HandleErrorCode(NARB_ERROR_INTERNAL);
            goto _out;
        }
    }
    peer_narb->RelayMessageToPeer(MSG_APP_REMOVE, msg, ero_confirm);

_out:

    //if (NarbDomainInfo.LookupRouterById(app_msg->req.src) && ret == 0)
    if (ret == 0)
        return HandleResvReleaseConfirm();
    else
        return ret;
}

int LSPQ::HandleResvReleaseConfirm()
{
   SetState(STATE_RESV_RELEASE_CONFIRM);

    assert(broker);

    api_msg* rmsg;

    rmsg = narb_new_msg_reply_release_confirm(req_ucid, app_seqnum, previous_lspb_id);

    assert(rmsg);

    LOGF("Sending release confirmmation for LSP (ucid=0x%x, seqno=0x%x).\n", req_ucid, app_seqnum);

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
    api_msg * msg = ReadMessage();
    if (!msg)
    {
        LOGF("LSP_Broker::ReadMessage failed. Closing the socket %d.\n", fd);
        if (api_writer != NULL)
            api_writer->Close();
        Close();
        return;
    }

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

    int ret = HandleMessage(msg);
}

int LSP_Broker::HandleMessage(api_msg * msg)
{
    LSPQ* lspq;
    msg_app2narb_request * app_req;
	
    if (ntohs(msg->header.type) != NARB_MSG_LSPQ)
    {
        LOGF("LSP_Broker:: The impossible happened:  Received a non-NARB_MSG_LSPQ message type: %d (ucid=0x%x, seqno=0x%x).\n",
            ntohs(msg->header.type), ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
        goto _abnormal_out;
    }

    lspq = LspqLookup(ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
    app_req = (msg_app2narb_request*)msg->body;

    switch (ntohs(app_req->type))
    {
    case MSG_APP_REQUEST:
       {
            LOGF("LSP_Broker::MSG_APP_REQUEST: The LSPQ (ucid=0x%x, seqno=0x%x).\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
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
                else if (lspq->State() == STATE_ERO_COMPLETE || lspq->State() == STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID)
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
            LOGF("LSP_Broker::MSG_PEER_REQUEST: The LSPQ (ucid=0x%x, seqno=0x%x).\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));

            //checking ping-pong effect (the LSP has been handled by other LSPBroker or PingPong happened)
            int pingPongCount = NARB_APIServer::LspqCount(ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
            //allowing for up to one count of ping-pong in order to support this kind of paths: RON->Backbone->SameRon...
            if (pingPongCount == 1) //we can have up to two lspq's of same (ucid, seqnum), including this current one to be created...
            {
                LOGF("LSP_Broker:: The LSPQ  (ucid=0x%x, seqno=0x%x) has been handled once by this NARB --> Still a valid PingPong...\n",
                    ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
            }
            if (pingPongCount > 1) //we can have up to two lspq's of same (ucid, seqnum), including this current one to be created...
            {
                LOGF("LSP_Broker:: The LSPQ  (ucid=0x%x, seqno=0x%x) has been handled more than once by this NARB --> invalid PingPong(s).\n",
                    ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
                u_int32_t lspb_id = narb_get_msg_lspb_id(msg);
                api_msg* rmsg = narb_new_msg_reply_error(ntohl(msg->header.ucid), ntohl(msg->header.seqnum), NARB_ERROR_INVALID_REQ, lspb_id);
                if (rmsg)
                    HandleReplyMessage(rmsg);
                goto _abnormal_out;
            }

            msg_app2narb_request * mrn_req = app_req + 1;
            if (!lspq)
            {
                //creating new LSPQ
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
                LOGF("LSP_Broker::MSG_APP_CONFIRM_EVENT: The LSPQ (ucid=0x%x, seqno=0x%x) no longer exists.\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
		        goto _abnormal_out;
            }

            lspq->HandleResvConfirm(msg);
        }
        break;
    case MSG_APP_REMOVE:
        {
            if (!lspq)
            {
                LOGF("LSP_Broker::MSG_APP_REMOVE_EVENT: The LSPQ (ucid=0x%x, seqno=0x%x) no longer exists.\n", ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
		        goto _abnormal_out;
            }

            lspq->HandleResvRelease(msg);
        }
        break;
    default:
        LOGF("Unknow APP->NARB message type (%d)\n", ntohs(app_req->type));
    }

    return 0;

_abnormal_out:
    api_msg_delete(msg);
	return -1;
}

void LSPQ::HandleOptionalRequestTLVs(api_msg* msg)
{
    int msg_len = ntohs(msg->header.length);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    int tlv_len;
    msg_app2narb_vtag_mask* vtagMask;

    if ((app_options & LSP_OPT_VTAG_MASK) == 0 && vtag_mask)
    {
        delete vtag_mask;
        vtag_mask = NULL;
    }

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type)) 
        {
        case MSG_APP_REQUEST:
        case MSG_PEER_REQUEST:
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
        case TLV_TYPE_NARB_HOP_BACK:
            tlv_len = sizeof(msg_app2narb_hop_back);
            hop_back = ((msg_app2narb_hop_back*)tlv)->ipv4;
            if (hop_back == 0)
                LOGF("Warning: LSPQ::HandleOptionalRequestTLVs: hop_back_tlv->ipv4 == 0\n");
            break;
        case TLV_TYPE_NARB_SUGGESTED_VTAG:
            tlv_len = sizeof(msg_app2narb_suggested_vtag);
            if (!suggested_vtag)
                suggested_vtag = new (struct msg_app2narb_suggested_vtag);
            else if (suggested_vtag->suggested_vtag != ((msg_app2narb_suggested_vtag*)tlv)->suggested_vtag)
                LOGF("Warning: LSPQ::HandleOptionalRequestTLVs: existing suggested_vtag (%d) does not match the received (%d)\n",
                    suggested_vtag->suggested_vtag, ((msg_app2narb_suggested_vtag*)tlv)->suggested_vtag);
            memcpy(suggested_vtag, tlv, sizeof(msg_app2narb_suggested_vtag));
            break;
        case TLV_TYPE_NARB_LSPB_ID:
            tlv_len = sizeof(msg_app2narb_lspb_id);
            previous_lspb_id = ((msg_app2narb_lspb_id*)tlv)->lspb_id;
            break;
        default:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
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
    bool hasVtagMask = false;

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type)) 
        {
        case TLV_TYPE_NARB_ERO:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            ; //do nothing
            break;
        case TLV_TYPE_NARB_ERROR_CODE:
            tlv_len = TLV_HDR_SIZE + 4;
            ; //do nothing
            break;
        case TLV_TYPE_NARB_VTAG_MASK:
            hasVtagMask = true;
            tlv_len = sizeof(msg_app2narb_vtag_mask);
            vtagMask = (msg_app2narb_vtag_mask*)tlv;
            if (!vtag_mask)
                vtag_mask = new (struct msg_app2narb_vtag_mask);
            memcpy(vtag_mask, vtagMask, sizeof(msg_app2narb_vtag_mask));
            break;
        case TLV_TYPE_NARB_LSPB_ID:
            tlv_len = sizeof(msg_app2narb_lspb_id);
            returned_lspb_id = ((msg_app2narb_lspb_id*)tlv)->lspb_id;
            break;
        /* RCE does not suggest vtag in the current implemention
        case TLV_TYPE_NARB_SUGGESTED_VTAG:
            if (!suggested_vtag)
                suggested_vtag = new (struct msg_app2narb_suggested_vtag);
            memcpy(suggested_vtag, tlv, sizeof(msg_app2narb_suggested_vtag));
            break;
        */
        default:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            break;
        }
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }

    if (!hasVtagMask && vtag_mask && ((app_options & LSP_OPT_VTAG_MASK) || (app_options & LSP_OPT_REQ_ALL_VTAGS)))
    {
        delete vtag_mask;
        vtag_mask = NULL;
    }
}

// searching for a request data record on lspq_list using msg ucid and sequence number
LSPQ * LSP_Broker::LspqLookup (u_int32_t ucid, u_int32_t seqnum)
{
    list<LSPQ*>::iterator it;

    for (it = lspq_list.begin(); it != lspq_list.end(); it++)
    {
        if (!(*it))
            continue;

        if (ucid == (*it)->req_ucid && seqnum == (*it)->app_seqnum)
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


// Static/Global functions ...
u_int32_t LSP_Broker::get_unique_lspb_id()
{
    static u_int32_t id = htonl(NarbDomainInfo.domain_id);
    return id++;
}

ConfirmationIDIndxedEROWithTimer* LSP_Broker::StoreEROWithConfirmationID(list<ero_subobj*>& ero, u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id)
{
    ConfirmationIDIndxedEROWithTimer* qconfEROTimer = RemoveEROWithConfirmationID(ucid, seqnum, src_id); // remove instead of lookup
    if (qconfEROTimer)
    {
        LOGF("LSP_Broker::StoreEROWithConfirmationID finds existing qconf ID (ucid=0x%x, seqnum=0x%x, source_ip=0x%x) --> removed the old one...\n",
            ucid, seqnum, src_id);
    }

    ConfirmationIDIndxedEROWithTimer* qconf_ero_entry = new ConfirmationIDIndxedEROWithTimer(ero, ucid, seqnum, src_id,
        SystemConfig::confirmed_ero_expire_secs, SystemConfig::confirmed_ero_trash_secs);
    qconf_id_indexed_ero_list.push_front(qconf_ero_entry);
    LOGF("LSP_Broker::StoreEROWithConfirmationID enqueued qconf ID (ucid=0x%x, seqnum=0x%x, source_ip=0x%x)\n", ucid, seqnum, src_id);
    return qconf_ero_entry;
}

ConfirmationIDIndxedEROWithTimer* LSP_Broker::LookupEROWithConfirmationID(u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id)
{
    list<ConfirmationIDIndxedEROWithTimer*>::iterator iter = qconf_id_indexed_ero_list.begin();
    for ( ; iter != qconf_id_indexed_ero_list.end(); iter++ )
    {
        if ((*iter)->ConfirmationIDMatched(ucid, seqnum, src_id))
            return (*iter);
    }
    return NULL;
}

ConfirmationIDIndxedEROWithTimer* LSP_Broker::RemoveEROWithConfirmationID(u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id)
{
    ConfirmationIDIndxedEROWithTimer* qconfEROTimer = NULL;
    list<ConfirmationIDIndxedEROWithTimer*>::iterator iter = qconf_id_indexed_ero_list.begin();
    for ( ; iter != qconf_id_indexed_ero_list.end(); iter++ )
    {
        if ((*iter)->ConfirmationIDMatched(ucid, seqnum, src_id))
        {
            qconfEROTimer = (*iter);
            qconf_id_indexed_ero_list.erase(iter);
            LOGF("LSP_Broker::RemoveEROWithConfirmationID dequeued qconf ID (ucid=0x%x, seqnum=0x%x, source_ip=0x%x)\n", ucid, seqnum, src_id);
            break;
        }
    }
    return qconfEROTimer;
}

