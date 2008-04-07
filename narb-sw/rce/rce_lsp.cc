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

#include "toolbox.hh"
#include "event.hh"
#include "rce_pcen.hh"
#include "pcen_ksp.hh"
#include "pcen_mrn.hh"
#include "pcen_mrn_cg.hh"
#include "pcen_oscars.hh"
#include "rce_lsp.hh"

void LSPHandler::Load(api_msg *msg)
{
    assert(msg);

    ucid = ntohl(msg->hdr.ucid);
    seqnum = ntohl(msg->hdr.msgseq);
    options = ntohl(msg->hdr.msgtag[0]);
    tag = ntohl(msg->hdr.msgtag[1]);
    te_tlv_header * tlv = (te_tlv_header*)msg->body;

    if (options & LSP_TLV_NARB_REQ)
    {
        narb_lsp_request_tlv*  narb_req_tlv = (narb_lsp_request_tlv*)tlv;
        narb_lsp_request_tlv*  mrn_spec_tlv = (narb_lsp_request_tlv*)((char*)tlv+sizeof(narb_lsp_request_tlv));

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
        // continue to get optional TLVs if applicable ...
        SetOptionalConstraints(msg);
    }
    LOGF("Accepted LSP request (ucid=0x%x, seqnum=0x%x): Src 0x%x Dest 0x%x Bandwidth %g\n",  
        ucid, seqnum, source, destination, bandwidth_ingress);
    api_msg_delete(msg);
}

void LSPHandler::SetOptionalConstraints(api_msg* msg)
{
    int msg_len = ntohs(msg->hdr.msglen);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    int tlv_len;
    subnet_ero.clear();
    is_subnet_ero2dtl_enabled = false;

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type))
        {
        case TLV_TYPE_NARB_REQUEST:
        case TLV_TYPE_NARB_PEER_REQUEST:
            tlv_len = sizeof(narb_lsp_request_tlv);
            ; //do nothing
            break;
        case TLV_TYPE_NARB_VTAG_MASK:
            tlv_len = sizeof(narb_lsp_vtagmask_tlv);
            if (ntohl(msg->hdr.options) & LSP_OPT_VTAG_MASK)
            {
                if (!vtag_mask)
                    vtag_mask = new (struct narb_lsp_vtagmask_tlv);
                memcpy(vtag_mask, tlv, sizeof(narb_lsp_vtagmask_tlv));
            }        
            break;
        case TLV_TYPE_NARB_HOP_BACK:
            tlv_len = sizeof(narb_lsp_hopback_tlv);
            hop_back = ((narb_lsp_hopback_tlv*)tlv)->ipv4;
            if (hop_back == 0)
                LOGF("Warning: LSPHandler::SetOptionalConstraints: narb_lsp_hopback_tlv->ipv4 == 0\n");
            break;
        case TLV_TYPE_NARB_LSPB_ID:
            tlv_len = sizeof(narb_lsp_lspb_id_tlv);
            lspb_id = ((narb_lsp_lspb_id_tlv*)tlv)->lspb_id;
            break;
        case TLV_TYPE_NARB_LOCAL_ID:
            tlv_len = sizeof(narb_lsp_local_id_tlv);
            src_lcl_id = ntohl(((narb_lsp_local_id_tlv*)tlv)->lclid_src);
            dest_lcl_id = ntohl(((narb_lsp_local_id_tlv*)tlv)->lclid_dest);
            break;
        case TLV_TYPE_NARB_SUBNET_ERO:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            GetERO_RFCStandard(tlv, subnet_ero);
            if (options & LSP_OPT_SUBNET_DTL)
                is_subnet_ero2dtl_enabled = true;
            break;
        default:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            break;
        }
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }
}

void LSPHandler::Run()
{
    PCEN * pcen_event;

    if (SystemConfig::pce_algorithm == PCE_NONE)
    {
        //enforce all requests being handled by MRN routing module
        options |= LSP_OPT_MRN;
        if ((options & LSP_OPT_MRN) == 0)
        {
            /*
            pcen_event = new PCEN(source, destination, switching_type_egress, encoding_type_egress, bandwidth_egress, options, 
                ucid, seqnum, lspb_id, tag, hop_back, src_lcl_id, dest_lcl_id);
            */
            pcen_event = new PCEN_MRN_CG(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress,
                switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                src_lcl_id, dest_lcl_id, vtag_mask);
        }
        else if ((options & LSP_OPT_MRN) != 0 && (options & LSP_OPT_ALT_PATHS) != 0)
        {
            pcen_event = new PCEN_OSCARS(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress, 
                switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                src_lcl_id, dest_lcl_id, vtag_mask);
        }
        else
        {
            pcen_event = new PCEN_MRN(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress, 
                switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                src_lcl_id, dest_lcl_id, vtag_mask);
        }
    }
    else
    {
        switch (SystemConfig::pce_algorithm)
        {
            case SPF:
                pcen_event = new PCEN(source, destination, switching_type_egress, encoding_type_egress, bandwidth_egress, options, 
                    ucid, seqnum, lspb_id, tag, hop_back, src_lcl_id, dest_lcl_id);
                break;
            case MRN_CG:
                options |= LSP_OPT_MRN;
                pcen_event = new PCEN_MRN_CG(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress,
                    switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                    src_lcl_id, dest_lcl_id, vtag_mask);
                break;
             case MRN_OSCARS:
                options |= LSP_OPT_MRN;
                pcen_event = new PCEN_OSCARS(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress, 
                    switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                    src_lcl_id, dest_lcl_id, vtag_mask);
                break;
            case MRN_DEFAULT:
            default:
                options |= LSP_OPT_MRN;
                pcen_event = new PCEN_MRN(source, destination, switching_type_ingress, encoding_type_ingress, bandwidth_ingress, 
                    switching_type_egress, encoding_type_egress, bandwidth_egress, options, ucid, seqnum, lspb_id, tag, hop_back,
                    src_lcl_id, dest_lcl_id, vtag_mask);
                break;
        }
    }
    
    pcen_event->AssociateWriter (api_writer);
    pcen_event->SetAutoDelete(true);
    if (is_subnet_ero2dtl_enabled)
    {
        pcen_event->EnableConvSubnetERO2DTL();
        pcen_event->SetSubnetERO(subnet_ero);
    }
    eventMaster.Schedule(pcen_event);
}

void LSPHandler::GetERO_RFCStandard(te_tlv_header* tlv, list<ero_subobj>& ero)
{
    assert (tlv);
    ero.clear();

    int len = ntohs(tlv->length);
    if (len <= 0)
        return;
    
    int offset = sizeof(struct te_tlv_header);

    ero_subobj new_subobj;
    ipv4_prefix_subobj* subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
    unum_if_subobj* subobj_unum;
    while (len > 0)
    {
        memset(&new_subobj, 0, sizeof(ero_subobj));
        if ((subobj_ipv4->l_and_type & 0x7f) == 4)
            subobj_unum = (unum_if_subobj *)((char *)tlv + offset);
        else
            subobj_unum = NULL;

        if (subobj_unum)
        {
            memcpy(&new_subobj.addr, &subobj_unum->addr, 4);
            new_subobj.hop_type = (subobj_unum->l_and_type >> 7) ? ERO_TYPE_LOOSE_HOP : ERO_TYPE_STRICT_HOP;
            new_subobj.prefix_len = 32;
            new_subobj.if_id = subobj_unum->ifid;
            if ((ntohl(subobj_unum->ifid)>>16) == LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL && (ntohl(subobj_unum->ifid) & 0xffff) > 0  
                    && (ntohl(subobj_unum->ifid) & 0xffff) < 4906)
                new_subobj.l2sc_vlantag = (u_int16_t)ntohl(subobj_unum->ifid);
            len -= sizeof(unum_if_subobj);
            offset += sizeof(unum_if_subobj);
        }
        else
        {
            memcpy(&new_subobj.addr, subobj_ipv4->addr, 4);
            new_subobj.hop_type = (subobj_ipv4->l_and_type >> 7) ? ERO_TYPE_LOOSE_HOP : ERO_TYPE_STRICT_HOP;
            new_subobj.prefix_len = 32;
            len -= sizeof(ipv4_prefix_subobj);
            offset += sizeof(ipv4_prefix_subobj);
        }
        ero.push_back(new_subobj);
        subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
    }
}

void LSPHandler::GetDTL(te_tlv_header* tlv, list<dtl_hop>& dtl)
{
    assert (tlv);
    dtl.clear();
    int len = ntohs(tlv->length);
    assert( len > 0 && len% sizeof(dtl_hop) == 0);

    dtl_hop * hop  = (dtl_hop *)((char *)tlv + TLV_HDR_SIZE);
    for (; len > 0 ;hop++, len -= sizeof(dtl_hop))
    {
        dtl.push_back(*hop);
    }
}


void LSPHandler::HandleResvNotification(api_msg* msg)
{
    //parse message
    ////requested LSP parameters
    u_int32_t ucid = ntohl(msg->hdr.ucid);
    u_int32_t seqnum = ntohl(msg->hdr.msgseq);
    bool is_bidir = ((ntohl(msg->hdr.options) & LSP_OPT_BIDIRECTIONAL) != 0);
    narb_lsp_vtagmask_tlv* vtag_mask_tlv = NULL;
    u_int32_t src_lcl_id = 0, dest_lcl_id = 0;
    u_int32_t holding_time = 0;
    list<ero_subobj> ero;
    list<ero_subobj> subnet_ero;
    list<dtl_hop> subnet_dtl;

    int msg_len = ntohs(msg->hdr.msglen);
    te_tlv_header* tlv = (te_tlv_header*)(msg->body);
    narb_lsp_request_tlv* lsp_req_tlv = (narb_lsp_request_tlv*)tlv; // mandatory
    int tlv_len;

    while (msg_len > 0)
    {
        switch (ntohs(tlv->type))
        {
        case TLV_TYPE_NARB_VTAG_MASK:
            tlv_len = sizeof(narb_lsp_vtagmask_tlv);
            vtag_mask_tlv =(narb_lsp_vtagmask_tlv*)tlv; 
            break;
        case TLV_TYPE_NARB_ERO:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            GetERO_RFCStandard(tlv, ero);
            break;
        case TLV_TYPE_NARB_LOCAL_ID:
            tlv_len = sizeof(narb_lsp_local_id_tlv);
            src_lcl_id = ntohl(((narb_lsp_local_id_tlv*)tlv)->lclid_src);
            dest_lcl_id = ntohl(((narb_lsp_local_id_tlv*)tlv)->lclid_dest);
            break;
        case TLV_TYPE_NARB_HOLDING_TIME:
            tlv_len = sizeof(narb_lsp_holding_time_tlv);
            holding_time = ntohl(((narb_lsp_holding_time_tlv*)tlv)->seconds);
            break;
        case TLV_TYPE_NARB_SUBNET_ERO:
            tlv_len = ntohs(tlv->length);
            if (subnet_ero.size() == 0)
            {
                GetERO_RFCStandard(tlv, subnet_ero);
            }
            break;
        case TLV_TYPE_NARB_SUBNET_DTL:
            tlv_len = ntohs(tlv->length);
            if (subnet_ero.size() == 0)
            {
                GetDTL(tlv, subnet_dtl);
                PCEN::TranslateSubnetDTLIntoERO(subnet_dtl, subnet_ero);
            }
            break;
        default:
            tlv_len = ntohs(tlv->length) + TLV_HDR_SIZE;
            break;
        }
        tlv = (te_tlv_header*)((char*)tlv + tlv_len);
        msg_len -= tlv_len;
    }

    if (ero.size() > 0)
        UpdateLinkStatesByERO(*lsp_req_tlv, ero, ucid, seqnum,  is_bidir, ntohl(msg->hdr.tag), src_lcl_id, dest_lcl_id, vtag_mask_tlv); //using default reserve_time
    if (subnet_ero.size() > 0)
        UpdateLinkStatesByERO(*lsp_req_tlv, subnet_ero, ucid, seqnum,  is_bidir, ntohl(msg->hdr.tag), src_lcl_id, dest_lcl_id, vtag_mask_tlv, holding_time ==0 ? SystemConfig::delta_expire_subnet_reserve : holding_time);

    api_msg_delete(msg);
}

//TODO: adding holding_time logic!
void LSPHandler::UpdateLinkStatesByERO(narb_lsp_request_tlv& req_data, list<ero_subobj>& ero_reply, u_int32_t ucid, u_int32_t seqnum,  bool is_bidir, u_int32_t lsp_vtag, u_int32_t lclid_src, u_int32_t lclid_dest, narb_lsp_vtagmask_tlv* vtag_mask, u_int32_t holding_time)
{
    Link *link1;
    ero_subobj* subobj;
    bool is_forward_link, is_starting_backward = false;
    u_int32_t vtag;
    list<ero_subobj>::iterator it;

    if (ero_reply.size() < 2)
        return;

    //alignment for forward/reverse subobjects (special case: hop-back request)
    it = ero_reply.begin();
    subobj = &(*it);
    link1 = RDB.LookupLinkByLclIf(RTYPE_LOC_PHY_LNK, subobj->addr);
    if (link1 && link1->RmtIfAddr() == 0)
    {
        is_starting_backward = true;
    }

    //e2e vlan tag
    for (it = ero_reply.begin(); it != ero_reply.end();  it++)
    {
        if ( (ntohl((*it).if_id) >> 16)  == LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL )
        {
            lsp_vtag = (ntohl((*it).if_id) & 0xffff);
            break;
        }

        vtag = (*it).l2sc_vlantag;
        if (vtag !=0 && vtag != ANY_VTAG)
        {
            lsp_vtag = vtag;
            break;
        }
    }

    //mapping ero_subobj to strict hop links (a.k.a. intradomain physical links)
    is_forward_link = is_starting_backward;
    for (it = ero_reply.begin(); it != ero_reply.end();  it++)
    {
        subobj = &(*it);
        vtag = 0;

        if (subobj->addr.s_addr == req_data.src.s_addr)
        {
            //$$$$ special handling for subnet-interface local-id subobject
            if ((lclid_src >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
            {
                if ((link1 = RDB.LookupLinkByLocalId(subobj->addr, lclid_src)) != NULL)
                {
                    HandleLinkStateDelta(req_data, link1, ucid, seqnum, vtag, ntohl(subobj->if_id), NULL, holding_time);
                }
            }
            continue;
        }
        if (subobj->addr.s_addr == req_data.dest.s_addr)
        {
            //$$$$ special handling for subnet-interface local-id subobject
            if ((lclid_dest >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
            {
                if ((link1 = RDB.LookupLinkByLocalId(subobj->addr, lclid_dest)) != NULL)
                {
                    HandleLinkStateDelta(req_data, link1, ucid, seqnum, vtag, ntohl(subobj->if_id), NULL, holding_time);
                }
            }
            continue;
        }
        if (subobj->hop_type == ERO_TYPE_LOOSE_HOP)
        {
            break;
        }
        is_forward_link = (!is_forward_link);
        if (!is_forward_link && !is_bidir) //ignore reverse link for unidirectional request
        {
            continue;
        }
        link1 = RDB.LookupLinkByLclIf(RTYPE_LOC_PHY_LNK, subobj->addr);
        while (link1 != NULL) // updating all links with the same local interface address
        {
            vtag = subobj->l2sc_vlantag;
            if (vtag == 0 && lsp_vtag != 0 && lsp_vtag != ANY_VTAG
                && ((ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST))
            {
                vtag = lsp_vtag;
            }

            HandleLinkStateDelta(req_data, link1, ucid, seqnum, vtag, ntohl(subobj->if_id), vtag_mask, holding_time);
            link1 = RDB.LookupNextLinkByLclIf(link1);
        }
    }

    //mapping ero_subobj to loose hop links (a.k.a. interdomain abstract links)
    is_forward_link = is_starting_backward;
    for (it = ero_reply.begin(); it != ero_reply.end();  it++)
    {
        subobj = &(*it);
        vtag = 0;

        if (subobj->addr.s_addr == req_data.src.s_addr || subobj->addr.s_addr == req_data.dest.s_addr)
        {
            continue;
        }

        //@@@@ this condition restricts holding of global/abstract resources to the strict path segment only!
        if (subobj->hop_type == ERO_TYPE_LOOSE_HOP)
        {
            break;
        }
       
        is_forward_link = (!is_forward_link);
        if (!is_forward_link && !is_bidir) //ignore reverse link for unidirectional request
        {
            continue;
        }
        link1 = RDB.LookupLinkByLclIf(RTYPE_GLO_ABS_LNK, subobj->addr);
        while (link1 != NULL) // updating all links with the same local interface address
        {
            vtag = subobj->l2sc_vlantag;
            if (vtag == 0 && lsp_vtag != 0 && lsp_vtag != ANY_VTAG
                && ((ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC 
                    || (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST))
            {
                vtag = lsp_vtag;
            }

            if ((((lclid_src >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC)
                || (lclid_dest >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && (ntohl(subobj->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST)
                && link1->Iscds().size() > 0 && link1->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
            {
                //$$$$ special case: update inter-domain links based on subnet-interface local-id constraints
                HandleLinkStateDelta(req_data, link1, ucid, seqnum, vtag, 0, NULL, holding_time);
            }
            else
            {
                HandleLinkStateDelta(req_data, link1, ucid, seqnum, vtag, ntohl(subobj->if_id), vtag_mask, holding_time);
            }
            link1 = RDB.LookupNextLinkByLclIf(link1);
        }
    }

}

void LSPHandler::HandleLinkStateDelta(narb_lsp_request_tlv& req_data, Link* link1, u_int32_t ucid, u_int32_t seqnum, u_int32_t vtag, u_int32_t if_id, narb_lsp_vtagmask_tlv* vtag_mask, u_int32_t holding_time)
{
    assert(link1);
    u_int8_t reqtype = req_data.type >> 8;
    assert(reqtype == MSG_LSP);
    
    u_int8_t action = req_data.type & 0xff;
    LinkStateDelta* delta = NULL;
    
    switch (action)
    {
    case ACT_QUERY:
        delta = link1->lookupDeltaByOwner(ucid, seqnum);
        if (delta)
        {
            LOGF("Warning: on Link[0x%x--0x%x], delta of same ucid-seqnum (ucid=0x%x, seqnum=0x%x, create_time=%d.%d) has already existed: bandwidth: %g vtag: %d\n", 
                link1->AdvRtId(), link1->Id(), delta->owner_ucid, delta->owner_seqnum, delta->create_time.tv_sec, delta->create_time.tv_usec, delta->bandwidth, delta->vlan_tag);
        }
        delta = new LinkStateDelta;
        memset(delta, 0, sizeof(LinkStateDelta));
        delta->owner_ucid = ucid;
        delta->owner_seqnum = seqnum;
        delta->bandwidth = req_data.bandwidth;
        delta->flags |= DELTA_QUERIED;
        if ((if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
        {
            delta->flags |= DELTA_TIMESLOTS;
            int ts_1st = (int)(if_id & 0xff);
            int ts_num = (int)(SystemConfig::MapBandwidthToNumberOfTimeslots(delta->bandwidth));
            for (int ts = 0; ts < ts_num && ts_1st+ts <= MAX_TIMESLOTS_NUM; ts++)
            {
                SET_TIMESLOT(delta->timeslots, ts_1st+ts);
            }
        }
        else if (vtag_mask)
        {
            delta->flags |= DELTA_VTAGMASK;
            memcpy(delta->vtag_mask, vtag_mask->bitmask, MAX_VLAN_NUM/8);
        }
        else if (vtag != 0)
        {
            delta->flags |= DELTA_VLANTAG;
            delta->vlan_tag = vtag;
        }
        link1->insertDelta(delta, SystemConfig::delta_expire_query, 0);
        break;

    case ACT_CONFIRM:
        if (holding_time == 0)
            holding_time = SystemConfig::delta_expire_reserve;
        if (holding_time == 0)
        {
            delta = link1->lookupDeltaByOwner(ucid, seqnum);
            if (delta)
            {
                //simply removing without putting back the held resources
                link1->DeltaListPointer()->remove(delta); 
                delete delta;
            }
            break; // not to contniue if holding_time for reservation is zero
        }
        // removing the query delta and puting back held resources.
        delta = link1->removeDeltaByOwner(ucid, seqnum);
        if (delta)
            delete delta;
        delta = new LinkStateDelta;
        memset(delta, 0, sizeof(LinkStateDelta));
        delta->owner_ucid = ucid;
        delta->owner_seqnum = seqnum;
        delta->bandwidth = req_data.bandwidth;
        delta->flags |= DELTA_RESERVED;
        if ((if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
        {
            delta->flags |= DELTA_TIMESLOTS;
            int ts_1st = (int)(if_id & 0xff);
            int ts_num = (int)(SystemConfig::MapBandwidthToNumberOfTimeslots(delta->bandwidth));
            for (int ts = 0; ts < ts_num && ts_1st+ts <= MAX_TIMESLOTS_NUM; ts++)
            {
                SET_TIMESLOT(delta->timeslots, ts_1st+ts);
            }
        }
        else if (vtag_mask)
        {
            delta->flags |= DELTA_VTAGMASK;
            memcpy(delta->vtag_mask, vtag_mask->bitmask, MAX_VLAN_NUM/8);
        }
        else if (vtag != 0)
        {
            delta->flags |= DELTA_VLANTAG;
            delta->vlan_tag = vtag;
        }
        link1->insertDelta(delta, holding_time, 0);
        break;

    case ACT_UPDATE:
        delta = link1->removeDeltaByOwner(ucid, seqnum);
        if (!delta)
            delta = new LinkStateDelta;
        memset(delta, 0, sizeof(LinkStateDelta));
        delta->owner_ucid = ucid;
        delta->owner_seqnum = seqnum;
        delta->bandwidth = req_data.bandwidth;
        delta->flags |= DELTA_UPDATED;
        if ((if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST || (if_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
        {
            delta->flags |= DELTA_TIMESLOTS;
            int ts_1st = (int)(if_id & 0xff);
            int ts_num = (int)(SystemConfig::MapBandwidthToNumberOfTimeslots(delta->bandwidth));
            for (int ts = 0; ts < ts_num && ts_1st+ts <= MAX_TIMESLOTS_NUM; ts++)
            {
                SET_TIMESLOT(delta->timeslots, ts_1st+ts);
            }
        }
        else if (vtag_mask)
        {
            delta->flags |= DELTA_VTAGMASK;
            memcpy(delta->vtag_mask, vtag_mask->bitmask, MAX_VLAN_NUM/8);
        }
        else if (vtag != 0)
        {
            delta->flags |= DELTA_VLANTAG;
            delta->vlan_tag = vtag;
        }
        link1->insertDelta(delta, SystemConfig::delta_expire_query, 0);
        break;
    case ACT_DELETE:
        delta = link1->removeDeltaByOwner(ucid, seqnum);
        if (delta)
            delete delta;
        break;
    }
    //DONE
}


