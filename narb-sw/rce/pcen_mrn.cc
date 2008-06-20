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

#include "radixtree.hh"
#include "resource.hh"
#include "rce_filter.hh"
#include "rce_pcen.hh"
#include "pcen_mrn.hh"
#include "rce_movaz_types.hh"
#include <math.h>

PCEN_MRN::PCEN_MRN(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, 
        float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, float bw_egress, u_int32_t opts, 
        u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id, u_int32_t tag, u_int32_t hopback, 
        u_int32_t src_lclid, u_int32_t dest_lclid, narb_lsp_vtagmask_tlv* vtm):
        PCEN(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) 
{
}

PCEN_MRN::~PCEN_MRN()
{

}

bool PCEN_MRN::PostBuildTopology()
{
    int i, j, k;
    PCENLink *pcen_link, *pcen_link2;
    PCENNode *pcen_node;
    list<ISCD*>::iterator iscd_iter;
    ISCD* iscd;
    list<PCENLink*>::iterator link_iter;
    RadixTree<Resource>* tree;
    RadixNode<Resource> *node;
    PCENLink* lclid_link_src = NULL, *lclid_link_dest = NULL;

    //$$$$ Local ID based source edge constraint take priority over hopback!
    if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        hop_back = 0;
    }

    bool hop_back_attaching_to_source_verified = (hop_back == 0 ? true : false);
    PCENLink* hopBackInterdomainPcenLink = NULL;

    int rNum = routers.size(); 
    int lNum = links.size();

    //Init Tspec on nodes/routers 
    for (i = 0; i < rNum; i++)
    {
        pcen_node = routers[i];
        pcen_node->tspec.SWtype = switching_type_ingress;
        pcen_node->tspec.ENCtype = encoding_type_ingress;
        pcen_node->tspec.Bandwidth = bandwidth_ingress;
    }

    //Init reverseLink and verify hopback
    for (i = 0; i < lNum; i++)
    {
        pcen_link = links[i];
        assert(pcen_link);

        //verifying that the hop_back address is a localIfAddr of the source node...
        if (hop_back != 0 && pcen_link->link->LclIfAddr() == hop_back && pcen_link->link->AdvRtId() == source.s_addr)
        {
            hop_back_attaching_to_source_verified = true;
        }

        pcen_node = pcen_link->rmt_end;
        if (!pcen_node)
            continue;
        list<PCENLink *>::iterator it_link;
        for (it_link = pcen_node->out_links.begin(); it_link != pcen_node->out_links.end(); it_link++)
        {
            assert(*it_link);
            if ((*it_link)->rmt_end == pcen_link->lcl_end && (*it_link)->link->rmtIfAddr == pcen_link->link->lclIfAddr
			&& (*it_link)->link->rmtId == pcen_link->link->lclId && (*it_link)->link->type == pcen_link->link->type)
            {
                pcen_link->reverse_link = *it_link;
                break;
            }
        }
    }

    if (!hop_back_attaching_to_source_verified)
    {
        LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that the hopback interface (0x%x) is attaching to the source router(0x%x)\n", hop_back, source.s_addr);
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        return false;
    }

    // obtaining and verifying lclid_link_src and lclid_link_dest
    if (((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID ||  (dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID))
    {
        tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
        node = tree->Root();
        while (node)
        {
            Link* link = (Link*)node->Data();
            if (link == NULL || link->Iscds().size() == 0 ||
               (htons(link->Iscds().front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) == 0)
            {
                node = tree->NextNode(node);
                continue;
            }
            if (!lclid_link_src && link->AdvRtId() == source.s_addr
                 && link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((src_lcl_id >> 8) & 0xff))
            {
                link->removeDeltaByOwner(ucid, seqnum);
                link->deleteExpiredDeltas(); // handling expired link state deltas. (refer to class Link and class LSPHandler.)
                link = new Link(link);
                lclid_link_src = new PCENLink(link);
                lclid_link_src->link_self_allocated = true;
            }
            else if (!lclid_link_dest && link->AdvRtId() == destination.s_addr
                 && link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((dest_lcl_id >> 8) & 0xff))
            {
                link->removeDeltaByOwner(ucid, seqnum);
                link->deleteExpiredDeltas(); // handling expired link state deltas. (refer to class Link and class LSPHandler.)
                link = new Link(link);
                lclid_link_dest = new PCENLink(link);
                lclid_link_dest->link_self_allocated = true;
            }

            node = tree->NextNode(node);
        }
        // verifying and adding the source lclid link into topology
        if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && !lclid_link_src)
        {
            LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that source  (0x%x) local-id is attaching to the topology\n", src_lcl_id);
            ReplyErrorCode(ERR_PCEN_INVALID_REQ);
            return false;
        }
        else if (lclid_link_src && lclid_link_src->link)
        {
            lclid_link_src->linkID = links.back()->linkID+1;
            links.push_back(lclid_link_src);
            lNum += 1;
            for (i = 0; i < rNum; i++) 
            {
                pcen_node = routers[i];
                if (pcen_node->router && pcen_node->router->Id() == lclid_link_src->link->AdvRtId())
                {
                    pcen_node->out_links.push_back(lclid_link_src);
                    lclid_link_src->lcl_end = pcen_node;
                }
            }
        }
        // verifying and adding the destination lclid link into topology
        if ((dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && !lclid_link_dest)
        {
            LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that destination  (0x%x) local-id is attaching to the topology\n", dest_lcl_id);
            ReplyErrorCode(ERR_PCEN_INVALID_REQ);
            return false;
        }
        else if (lclid_link_dest && lclid_link_dest->link)
        {
            lclid_link_dest->linkID = links.back()->linkID+1;
            links.push_back(lclid_link_dest);
            lNum += 1;
            for (i = 0; i < rNum; i++) 
            {
                pcen_node = routers[i];
                if (pcen_node->router && pcen_node->router->Id() == lclid_link_dest->link->AdvRtId())
                {
                    pcen_node->out_links.push_back(lclid_link_dest);
                    lclid_link_dest->lcl_end = pcen_node;
                }
            }
        }
    }
	
    // Building SubnetUNI 'jump' links to incorporate the UNI subnet (a Ciena SONET network in this specific implemenation)
    if (SystemConfig::should_incorporate_subnet)
    {
        PCENNode* new_pcen_source = NULL;
        PCENNode* new_pcen_destination = NULL;
        u_int32_t hopback_source = 0;

        // Handling source local-id constraint in the special 'subnet' topology
        if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)  //  special handling for lclid edge constraints
        {
            // costructing fake source and destination nodes for local-ids
            // the newly constructed source edge node use source local-id as router id
            RouterId* new_router = new RouterId(source.s_addr+src_lcl_id); 
            new_pcen_source = new PCENNode(new_router);
            new_pcen_source->router_self_allocated = true;
            new_pcen_source->tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
            routers.push_back(new_pcen_source);
            rNum++;
            PCENLink * pcen_link_src_backward = lclid_link_src;  // reusing the backward pcen_link
            Link* link_src_backward = pcen_link_src_backward->link;
            // cutomizing link_src_backward
            link_src_backward->id = new_pcen_source->router->id;
            link_src_backward->rmtIfAddr = get_slash30_peer(link_src_backward->lclIfAddr);
            pcen_link_src_backward->rmt_end = new_pcen_source;
            //pcen_link_src_backward->link_self_allocated = true;
            new_pcen_source->in_links.push_back(pcen_link_src_backward);
            // costructing fake source link (reverse direction)
            Link* link_src_forward = new Link(link_src_backward->Id(), link_src_backward->AdvRtId(), 
                link_src_backward->RmtIfAddr(), link_src_backward->LclIfAddr());
            // cutomizing link_src_forward with L2 ISCD
            iscd = new ISCD;
            if (link_src_backward->Iscds().size() == 2) //@@@@ TODO in OSPFd?
            {
                *iscd = *link_src_backward->iscds.back();  
            }
            else   // looking for corresponding inter-domain link and copy layer-2 specific information
            {
                Link* link_inter = NULL;
                tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
                node = tree->Root();
                while (node)
                {
                    Link* link = (Link*)node->Data();
                    if (link == NULL || link->Iscds().size() == 0 || link->Iscds().front()->swtype != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                       || (htons(link->Iscds().front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) == 0)
                    {
                        node = tree->NextNode(node);
                        continue;
                    }
                    if (link->lclIfAddr == link_src_backward->lclIfAddr)
                    {
                        link_inter = link;
                        break;
                    }
                    node = tree->NextNode(node);
                }
                if (link_inter != NULL)
                {
                    *iscd = *link_inter->iscds.front();
                }
                else // fake l2 info if unavailable
                {
                    memset(iscd, 0, sizeof(IfSwCapDesc));
                    iscd->swtype = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
                    iscd->encoding = LINK_IFSWCAP_SUBTLV_ENC_ETH;
                    for (i = 0; i < 8; i++)
                        iscd->max_lsp_bw[i] = link_src_backward->Iscds().front()->max_lsp_bw[i];
                }
            }
            link_src_forward->iscds.push_back(iscd);
            PCENLink* pcen_link_src_forward = new PCENLink(link_src_forward); //have to make new
            pcen_link_src_forward->link_self_allocated = true;
            pcen_link_src_forward->lcl_end = new_pcen_source;
            pcen_link_src_forward->rmt_end = pcen_link_src_backward->lcl_end;
            pcen_link_src_forward->reverse_link = pcen_link_src_backward;
            pcen_link_src_backward->reverse_link = pcen_link_src_forward;
            new_pcen_source->out_links.push_back(pcen_link_src_forward);
            pcen_link_src_backward->lcl_end->in_links.push_back(pcen_link_src_forward);
            pcen_link_src_forward->linkID = links.back()->linkID+1;
            links.push_back(pcen_link_src_forward);
            lNum++;
            // new source IP
            source.s_addr = source.s_addr+src_lcl_id;
        }

        // Handling destiantion local-id constraint in the special 'subnet' topology
        if ((dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)  //  special handling for lclid edge constraints
        {            
            // the newly constructed destination edge node use destination local-id as router id
            RouterId* new_router = new RouterId(destination.s_addr+dest_lcl_id);
            new_pcen_destination = new PCENNode(new_router);
            new_pcen_destination->tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
            new_pcen_destination->router_self_allocated = true;
            routers.push_back(new_pcen_destination);
            rNum++;
            PCENLink * pcen_link_dest_forward = lclid_link_dest;  // reusing the forward pcen_link
            Link* link_dest_forward = pcen_link_dest_forward->link;
            //cutomizing link_dest_forward
            link_dest_forward->id = new_pcen_destination->router->id;
            link_dest_forward->rmtIfAddr = get_slash30_peer(link_dest_forward->lclIfAddr);
            pcen_link_dest_forward->rmt_end = new_pcen_destination;
            //pcen_link_dest_forward->link_self_allocated = true;
            new_pcen_destination->in_links.push_back(pcen_link_dest_forward);
            // costructing fake destination link (reverse direction)            
            Link* link_dest_backward = new Link(link_dest_forward->Id(), link_dest_forward->AdvRtId(), 
                link_dest_forward->RmtIfAddr(), link_dest_forward->LclIfAddr());
            // cutomizing link_dest_backward
            iscd = new ISCD;
            if (link_dest_forward->Iscds().size() == 2) //@@@@ TODO in OSPFd
            {
                *iscd = *link_dest_forward->iscds.back();  
            }
            else // looking for corresponding inter-domain link and copy layer-2 specific information
            {
                Link* link_inter = NULL;
                tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
                node = tree->Root();
                while (node)
                {
                    Link* link = (Link*)node->Data();
                    if (link == NULL || link->Iscds().size() == 0 || link->Iscds().front()->swtype != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                       || (htons(link->Iscds().front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) == 0)
                    {
                        node = tree->NextNode(node);
                        continue;
                    }
                    if (link->lclIfAddr == link_dest_forward->lclIfAddr)
                    {
                        link_inter = link;
                        break;
                    }
                    node = tree->NextNode(node);
                }
                if (link_inter != NULL)
                {
                    *iscd = *link_inter->iscds.front();
                }
                else // fake l2 info if unavailable
                {
                    memset(iscd, 0, sizeof(IfSwCapDesc));
                    iscd->swtype = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
                    iscd->encoding = LINK_IFSWCAP_SUBTLV_ENC_ETH;
                    for (i = 0; i < 8; i++)
                        iscd->max_lsp_bw[i] = link_dest_forward->Iscds().front()->max_lsp_bw[i];
                }
            }
            link_dest_backward->iscds.push_back(iscd);
            PCENLink* pcen_link_dest_backward = new PCENLink(link_dest_backward); //have to make new
            pcen_link_dest_backward->link_self_allocated = true;
            pcen_link_dest_backward->lcl_end = new_pcen_destination;
            pcen_link_dest_backward->rmt_end = pcen_link_dest_forward->lcl_end;
            pcen_link_dest_backward->reverse_link = pcen_link_dest_forward;
            pcen_link_dest_forward->reverse_link = pcen_link_dest_backward;
            new_pcen_destination->out_links.push_back(pcen_link_dest_backward);
            pcen_link_dest_forward->lcl_end->in_links.push_back(pcen_link_dest_backward);
            pcen_link_dest_backward->linkID = links.back()->linkID+1;
            links.push_back(pcen_link_dest_backward);
            lNum++;
            // new destination IP
            destination.s_addr = destination.s_addr+dest_lcl_id;
        }

        // Handling hop-back constraint in the special 'subnet' topology
        if (hop_back != 0) //  special handling for hop back
        {
            for (i = 0; i < lNum; i++)
            {
                pcen_link = links[i];
                if (pcen_link->link->lclIfAddr == hop_back && pcen_link->reverse_link != NULL && pcen_link->link->type == RTYPE_GLO_ABS_LNK)
                {
                    hopBackInterdomainPcenLink = pcen_link;
                }
            }
            //Moving source one hop back!!
            if (hopBackInterdomainPcenLink && hopBackInterdomainPcenLink->rmt_end && hopBackInterdomainPcenLink->rmt_end->router)
            {
                hopback_source = hopBackInterdomainPcenLink->rmt_end->router->id;
            }
            else
            {
                LOGF("Note: PCEN_MRN::PostBuildTopology did not change source address based on the hop_back interface 0x%x\n", hop_back);
            }

            tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
            node = tree->Root();
            while (node)
            {
                Link* link = (Link*)node->Data();
                if ( link && link->lclIfAddr != 0 && link->rmtIfAddr == 0 && link->iscds.size() > 0 
                    && link->iscds.front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC 
                    && (ntohs(link->iscds.front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) ) 
                { // if found an intra-domain subnet edge link
                    hopBackInterdomainPcenLink = NULL;
                    for (i = 0; i < lNum; i++)
                    {
                        pcen_link = links[i];
                        if (pcen_link->link->lclIfAddr == link->lclIfAddr && pcen_link->reverse_link != NULL && pcen_link->link->type == RTYPE_GLO_ABS_LNK)
                        {
                            //hop_back verified against the attaching-to subnet_vlsr, change source node now
                            if ( hopback_source !=0 && pcen_link->link->LclIfAddr() == hop_back && source.s_addr == pcen_link->link->AdvRtId())
                            {
                                source.s_addr = hopback_source;
                            }
                            //record the found hop back link candidate
                            hopBackInterdomainPcenLink = pcen_link;
                        }
                    }
                    // fake hop-back intra-domain links will be added to replace the interdomain (border) links
                    if (hopBackInterdomainPcenLink != NULL) // patten recoginized
                    {
                        //adding two new links into the PCEN topology
                        PCENLink* linkForward = NewTransitLink(links);
                        PCENLink* linkHopback = NewTransitLink(links);
                        lNum += 2;
                        PCENNode* nodeHead = hopBackInterdomainPcenLink->rmt_end;
                        PCENNode* nodeTail = hopBackInterdomainPcenLink->lcl_end;
                        assert(nodeHead && nodeTail);
            
                        // removing all links from nodeHead if it is a hop_back source
                        if (nodeHead->router->id == source.s_addr && hopBackInterdomainPcenLink->link->lclIfAddr == hop_back)
                        {
                            nodeHead->out_links.clear();
                            nodeHead->in_links.clear();
                        }
            
                        // allocating link resource and updating link parameters for forward link
                        assert(hopBackInterdomainPcenLink->reverse_link->link);
                        linkForward->link = new Link(hopBackInterdomainPcenLink->reverse_link->link);
                        linkForward->link_self_allocated = true;
                        linkForward->link->type = RTYPE_LOC_PHY_LNK;
                        linkForward->lcl_end = nodeHead;
                        linkForward->rmt_end = nodeTail;
                        nodeHead->out_links.push_back(linkForward);
                        nodeTail->in_links.push_back(linkForward);
            
                        // allocating link resource and updating link parameters for backward (hop back) link
                        linkHopback->link = new Link(link);
                        linkHopback->link_self_allocated = true;
                        linkHopback->link->rmtIfAddr = hopBackInterdomainPcenLink->link->rmtIfAddr;
                        linkHopback->lcl_end = nodeTail;
                        linkHopback->rmt_end = nodeHead;
                        nodeHead->in_links.push_back(linkHopback);
                        nodeTail->out_links.push_back(linkHopback);
            
                        // assigning reverse links for the links in both directions
                        linkHopback->reverse_link = linkForward;
                        linkForward->reverse_link = linkHopback;
            
                        //removing the links from the PCEN topology
                        hopBackInterdomainPcenLink->linkID = -1;
                        hopBackInterdomainPcenLink->reverse_link->linkID = -1;
                    }
                }
                node = tree->NextNode(node);
            }
        }

        // 'jump' link construction logic...
        for (i = 0; i < lNum; i++)
        {
            pcen_link = links[i];
            if (pcen_link && pcen_link->link && pcen_link->linkID != -1) 
            {
                iscd_iter = pcen_link->link->Iscds().begin();
                for ( ; iscd_iter != pcen_link->link->Iscds().end(); iscd_iter++)
                {
                    iscd = *iscd_iter;
                    if(!iscd)	
                        continue;

                    if ( (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC) && (ntohs(iscd->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) ) 
                    {
                        for (j = 0; j < rNum; j++)
                        {
                            pcen_node = routers[j];
                            if ( pcen_node->router && pcen_node->router->id == iscd->subnet_uni_info.nid_ipv4 &&  pcen_node->router->id != pcen_link->link->advRtId )
                            {
                                if (!pcen_link->reverse_link || !pcen_link->reverse_link->link)
                                    continue;
                                // skip the formed intra-domain links that are also local-id edge links
                                if ((lclid_link_src && lclid_link_src->link && pcen_link != lclid_link_src && pcen_link->link->LclIfAddr() == lclid_link_src->link->LclIfAddr())
                                    || (lclid_link_dest && lclid_link_dest->link && pcen_link!= lclid_link_dest && pcen_link->link->LclIfAddr() == lclid_link_dest->link->LclIfAddr()))
                                    continue;
                                // remove the links from RDB.
                                if (!pcen_link->link_self_allocated)
                                    RDB.Remove(pcen_link->link);
                                if (!pcen_link->reverse_link->link_self_allocated)
                                    RDB.Remove(pcen_link->reverse_link->link);

                                // change IDs of current RDB link and its reverse link as 'jump' links
                                pcen_link->link->advRtId = pcen_node->router->advRtId;
                                // link->id and reverse_link->advRtId unchanged
                                pcen_link->reverse_link->link->id = pcen_node->router->id;
                                // link and reverse_link data interface addresses unchanged

                                // setting homeVLSR ... for multi-home-vlsr only
                                // pcen_node->home_vlsr = pcen_link->lcl_end;

                                //change link and rlink switching capability
                                iscd->swtype = iscd->subnet_uni_info.swtype_ext;
                                iscd->encoding = iscd->subnet_uni_info.encoding_ext;

                                // remove current pcen_link and its reverse link from the original VLSR pce_node
                                assert(pcen_link->lcl_end);
                                link_iter = pcen_link->lcl_end->out_links.begin();
                                while (link_iter != pcen_link->lcl_end->out_links.end())
                                {
                                    if((*link_iter) == pcen_link)
                                        link_iter = pcen_link->lcl_end->out_links.erase(link_iter);
					 else
                                        link_iter++;
                                }
                                link_iter = pcen_link->lcl_end->in_links.begin();
                                while (link_iter != pcen_link->lcl_end->in_links.end())
                                {
                                    if((*link_iter) == pcen_link->reverse_link)
                                        link_iter = pcen_link->lcl_end->in_links.erase(link_iter);
                                    else
                                        link_iter++;
                                }

                                // connect current pcen_link and its reverse link to current pce_node
                                pcen_link->lcl_end = pcen_node;
                                pcen_link->reverse_link->rmt_end = pcen_node;

                                pcen_node->out_links.push_front(pcen_link);
                                pcen_node->in_links.push_front(pcen_link->reverse_link);

                                // change 'jump link' metric 
                                pcen_link->link->metric = 0;
                                pcen_link->reverse_link->link->metric = 0;

                                // Update link pointers in the links vector ...
                                for (k = 0; k < lNum; k++)
                                {
                                    pcen_link2 = links[k];
                                    if ( k == i || !pcen_link2 || !pcen_link2->link || pcen_link2->linkID == -1 )
                                        continue;
                                    if ( pcen_link2->link && pcen_link2->link->Index() == pcen_link->link->Index()) 
                                    {
                                        pcen_link2->linkID = -1; //indicating removal after the loop
                                        pcen_link2->reverse_link->linkID = -1;
                                    }
                                }

                                // re-plant the links into RDB.
                                // !!!! Note: The original links will be restored by OSPF refresh
                                if (!pcen_link->link_self_allocated)
                                    RDB.Update(pcen_link->link);
                                if (!pcen_link->reverse_link->link_self_allocated)
                                    RDB.Update(pcen_link->reverse_link->link);
                            }
                        }
                    }
                }
            }
        }

        // removing trashed pcen_links
        for (i = 0; i < links.size(); i++)
        {
            pcen_link = links[i];
            if ( pcen_link->linkID == -1 )
            {
               pcen_link->lcl_end->out_links.remove(pcen_link);
               pcen_link->rmt_end->in_links.remove(pcen_link);
               links.erase(links.begin()+i);
               delete pcen_link;
               if (i == links.size())
                   break;
               else
                   i--;
            }
        }
    }

    return true;
}
    
void PCEN_MRN::Run()
{
    int ret;
    Event::Run();
    
    if (is_e2e_tagged_vlan && vtag == 0)
    {
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        LOG("PCEN_MRN::Run() : Requested vlan tag cannot be zero!" << endl);
        return;
    }


    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_MRN::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_MRN::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if (!PostBuildTopology())
    {
        LOGF("PCEN_MRN::PostBuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN_MRN::PerformComputation() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    //@@@@ moved here from end of PerformComputation
    if (SystemConfig::should_incorporate_subnet)
    {
        HandleSubnetUNIEROTrack(ero);
        if (ero.size() == 0)
            ReplyErrorCode(ERR_PCEN_NO_ROUTE);
    }

    ReplyERO();
}

bool PCEN_MRN::IsLoop(list<PCENLink*> &path, PCENNode* new_node)
{
    assert(new_node);

    if (path.size() == 0)
        return false;

    list<PCENLink*>::iterator it;
    PCENLink* pcen_link;
    for (it = path.begin(); it != path.end(); it++)
    {
        pcen_link = *it;
        assert(pcen_link && pcen_link->lcl_end && pcen_link->rmt_end);

        if (new_node == pcen_link->lcl_end || new_node == pcen_link->rmt_end)
            return true;
    }

    return false;
}

bool PCEN_MRN::IsCrossingRegionBoundary(PCENLink* pcen_link, TSpec& tspec)
{
    assert(pcen_link && pcen_link->link);

    // Check adaptation defined by IACD(s)
    list<IACD*>::iterator it_iacd;
    for (it_iacd = pcen_link->link->iacds.begin(); it_iacd != pcen_link->link->iacds.end(); it_iacd++)
    {
        //crossing from lower layer to upper layer
        if ((*it_iacd)->swtype_lower == tspec.SWtype && (*it_iacd)->encoding_lower == tspec.ENCtype)
            return true;
        //crossing from upper layer to lower layer
        if ((*it_iacd)->swtype_upper == tspec.SWtype && (*it_iacd)->encoding_upper == tspec.ENCtype)
            return true;

         // @@@@ bandwidth criteria to be considered in the future.
    }

    // Check implicit adaptation
    if (pcen_link->reverse_link && pcen_link->reverse_link->link && 
        pcen_link->link->iscds.size() == 1 && pcen_link->reverse_link->link->iscds.size() == 1)
    {
        //@@@@ Does encoding matter?
        if (pcen_link->link->iscds.front()->swtype != pcen_link->reverse_link->link->iscds.front()->swtype
            || pcen_link->link->iscds.front()->encoding != pcen_link->reverse_link->link->iscds.front()->encoding)
            return true;
    }

    return false;
}

bool PCEN_MRN::IsInExcludedLayer(PCENNode* node)
{
    u_int32_t layer_excluded;
    switch (node->tspec.SWtype)
    {
    case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
    case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
    case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
    case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
        layer_excluded = LSP_OPT_EXCLUD_L3;
        break;
    case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
        layer_excluded = LSP_OPT_EXCLUD_L2;
        break;
    case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
        layer_excluded = LSP_OPT_EXCLUD_TDM;
        break;
    case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
    case LINK_IFSWCAP_SUBTLV_SWCAP_FSC:
        layer_excluded = LSP_OPT_EXCLUD_L1;
        break;
    default:
        layer_excluded = 0;
    }
    if ((layer_excluded & this->options) != 0)
        return true;
    return false;
}

int PCEN_MRN::GetNextRegionTspec(PCENLink* pcen_link, TSpec& tspec)
{
    assert(pcen_link && pcen_link->link);

    // Check adaptation defined by IACD(s)
    list<IACD*>::iterator it_iacd;
    for (it_iacd = pcen_link->link->iacds.begin(); it_iacd != pcen_link->link->iacds.end(); it_iacd++)
    {
        //crossing from lower layer to upper layer
        if ((*it_iacd)->swtype_lower == tspec.SWtype && (*it_iacd)->encoding_lower == tspec.ENCtype)
        {
            tspec.SWtype = (*it_iacd)->swtype_upper;
            tspec.ENCtype = (*it_iacd)->encoding_upper;
            // Bandwidth adaptation for lower->upper layer
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            }
            return 0;
        }
        
        //crossing from upper layer to lower layer
        if ((*it_iacd)->swtype_upper == tspec.SWtype && (*it_iacd)->encoding_upper == tspec.ENCtype)
        {
            tspec.SWtype = (*it_iacd)->swtype_lower;
            tspec.ENCtype = (*it_iacd)->encoding_lower;
            // Bandwidth adaptation for upper->lower layer
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_FSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            }
            return 0;
        }
    }


    // Check implicit adaptation
    if (pcen_link->reverse_link && pcen_link->reverse_link->link && 
        pcen_link->link->iscds.size() == 1 && pcen_link->reverse_link->link->iscds.size() == 1)
    {
        //@@@@ Does encoding matter?
        if (pcen_link->link->iscds.front()->swtype != pcen_link->reverse_link->link->iscds.front()->swtype
            || pcen_link->link->iscds.front()->encoding != pcen_link->reverse_link->link->iscds.front()->encoding)
        {
            tspec.SWtype = pcen_link->reverse_link->link->iscds.front()->swtype;
            tspec.ENCtype = pcen_link->reverse_link->link->iscds.front()->encoding;
            //Bandwidth adaptation
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //@@@@ bandwidth constraint unchanged
                /*
                if (!float_equal(pcen_link->reverse_link->link->iscds.front()->min_lsp_bw, 0))
                    tspec.Bandwidth = ceilf(tspec.Bandwidth /pcen_link->reverse_link->link->iscds.front()->min_lsp_bw)
                    * pcen_link->reverse_link->link->iscds.front()->min_lsp_bw;
                */
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //@@@@ bandwidth constraint unchanged
                //tspec.Bandwidth = pcen_link->reverse_link->link->iscds.front()->max_lsp_bw[7];//?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_FSC:
                //@@@@ bandwidth constraint unchanged
                //tspec.Bandwidth = pcen_link->reverse_link->link->iscds.front()->max_lsp_bw[7];//?
                break;
            }
            return 0;
        }
    }

    return -1;
}

int PCEN_MRN::CheckTimeslotsAvailability(PCENLink* pcen_link, float bandwidth)
{
    // find the ISCD with subnetUniIf specific information
    ISCD * iscd = NULL;
    list<ISCD*>::iterator it;
    for (it = pcen_link->link->iscds.begin(); it != pcen_link->link->iscds.end(); it++)
    {
        if (ntohs((*it)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI)
        {
            iscd = *it;
            break;
        }
    }

    if (!iscd)
        return 0; // no subnetIf ISCD? never mind...

    int min_num_ts = SystemConfig::MapBandwidthToNumberOfTimeslots(bandwidth);
    u_int8_t ts, ts_num = 0;
    for (ts = 1; ts <= MAX_TIMESLOTS_NUM; ts++)
    {
        if (HAS_TIMESLOT(iscd->subnet_uni_info.timeslot_bitmask, ts))
            ts_num++;
        else
            ts_num = 0;
        if (ts_num >= min_num_ts)
            return (ts - min_num_ts+1);
    }

    return 0;
}
    
int PCEN_MRN::InitiateMovazWaves(ConstraintTagSet& waveset, PCENLink* nextLink)
{
    MovazTeLambda tel;
    PCENLink * reverseLink = nextLink->reverse_link;
    list<void*> *p_list = (list<void*>*)(reverseLink->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA"));
    if (reverseLink == NULL || p_list == NULL)
      return -1;
  
    waveset.TagSet().clear();
    list<void*>::iterator it;
    bool has_wave = false;
    for (it = p_list->begin(); it!= p_list->end(); it++)
    {
        tel = *(MovazTeLambda*)(*it);
        ntoh_telambda(tel);
        if (tel.priority == 0x07)
        {
            if (valid_channel(tel.channel_id))
                waveset.AddTag(tel.channel_id);
            else
                has_wave = true;
        }
    }

    //$$$$ --->Movaz_RE speical handling
    u_int32_t* p_freq = (u_int32_t*)(nextLink->AttributeByTag("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA"));
    if (has_wave && p_freq)
        waveset.AddTag(ntohl(*p_freq)); //$$ freq = ANY_WAVE ?

    if (waveset.IsEmpty())
        return -1;
    return 0;
}

void PCEN_MRN::AddLinkToEROTrack(list<ero_subobj>& ero_track,  PCENLink* pcen_link)
{
    ero_subobj subobj1, subobj2;
    memset(&subobj1, 0, sizeof(ero_subobj));
    subobj1.prefix_len = 32;
    subobj1.addr.s_addr = pcen_link->link->lclIfAddr;
    if (subobj1.addr.s_addr == 0)
    {
        subobj1.addr.s_addr = pcen_link->link->advRtId;
        subobj1.if_id = pcen_link->link->lclRmtId[0];
    }
    memset(&subobj2, 0, sizeof(ero_subobj));
    subobj2.prefix_len = 32;
    subobj2.addr.s_addr = pcen_link->link->rmtIfAddr;
    if (subobj2.addr.s_addr == 0)
    {
        subobj2.addr.s_addr = pcen_link->link->id;
        subobj2.if_id = pcen_link->link->lclRmtId[1];
    }

    if (pcen_link->link->type == RTYPE_LOC_PHY_LNK)
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_STRICT_HOP;
    else 
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_LOOSE_HOP;

    subobj1.sw_type = subobj2.sw_type = pcen_link->lcl_end->tspec.SWtype;
    subobj1.encoding = subobj2.encoding = pcen_link->lcl_end->tspec.ENCtype;
    subobj1.bandwidth= subobj2.bandwidth = pcen_link->lcl_end->tspec.Bandwidth;

    //The second subobj in a pair takes remote-end TE parameters.
    subobj2.sw_type = pcen_link->rmt_end->tspec.SWtype;
    subobj2.encoding = pcen_link->rmt_end->tspec.ENCtype;
    subobj2.bandwidth = pcen_link->rmt_end->tspec.Bandwidth;
    
    //E2E VLAN related
    if (is_e2e_tagged_vlan && subobj1.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->link)
    {
        if (ntohs(pcen_link->link->iscds.front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
        {
            subobj1.l2sc_vlantag = vtag; //*(u_int16_t *)subobj1.pad
        }
    } 
    if (is_e2e_tagged_vlan && subobj2.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->reverse_link && pcen_link->reverse_link->link)
    {
        if (pcen_link->reverse_link->link->iscds.front() && (ntohs(pcen_link->reverse_link->link->iscds.front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC))
            subobj2.l2sc_vlantag = vtag; //*(u_int16_t *)subobj2.pad
    } 

    //Subnet UNI related
    u_int8_t ts = 0;  //one-based
    if (SystemConfig::should_incorporate_subnet && pcen_link->link 
        && (ntohs(pcen_link->link->iscds.front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0 )
    {
        u_int8_t ts_start = 1, ts_num = 0;
        if ((dest_lcl_id & 0xff) > 0 &&  (dest_lcl_id & 0xff) <= MAX_TIMESLOTS_NUM && ((dest_lcl_id >> 8) & 0xff) == pcen_link->link->iscds.front()->subnet_uni_info.subnet_uni_id)
            ts_start = (dest_lcl_id & 0xff);
        for (ts = ts_start; ts <= MAX_TIMESLOTS_NUM; ts++)
        {
            if (HAS_TIMESLOT(pcen_link->link->iscds.front()->subnet_uni_info.timeslot_bitmask, ts))
            {
                ts_num++;
            }
            else
            {
                ts_num = 0;
            }
            if (ts_num >= SystemConfig::MapBandwidthToNumberOfTimeslots(pcen_link->rmt_end->tspec.Bandwidth))
            {
                break;
            }
        }
        if (ts > MAX_TIMESLOTS_NUM) 
        {
            ts = 0;
        }
        else 
        {
            ts = ts-ts_num+1;
        }
        subobj1.if_id = htonl( (LOCAL_ID_TYPE_SUBNET_UNI_DEST << 16) |(pcen_link->link->iscds.front()->subnet_uni_info.subnet_uni_id << 8) | ts );
        subobj1.l2sc_vlantag = 0;
    }

    if ( SystemConfig::should_incorporate_subnet && pcen_link->reverse_link && pcen_link->reverse_link->link 
        && (ntohs(pcen_link->reverse_link->link->iscds.front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0 )
    {
        u_int8_t ts_start = 1, ts_num = 0;
        if ((src_lcl_id & 0xff) > 0 &&  (src_lcl_id & 0xff) <= MAX_TIMESLOTS_NUM && ((src_lcl_id >> 8) & 0xff) == pcen_link->reverse_link->link->iscds.front()->subnet_uni_info.subnet_uni_id)
            ts_start = (src_lcl_id & 0xff);
        for (ts = ts_start; ts <= MAX_TIMESLOTS_NUM; ts++)
        {
            if (HAS_TIMESLOT(pcen_link->reverse_link->link->iscds.front()->subnet_uni_info.timeslot_bitmask, ts))
            {
                ts_num++;
            }
            else
            {
                ts_num = 0;
            }
            if (ts_num >= SystemConfig::MapBandwidthToNumberOfTimeslots(pcen_link->lcl_end->tspec.Bandwidth))
            {
                break;
            }
        }
        if (ts > MAX_TIMESLOTS_NUM) 
        {
            ts = 0;
        }
        else 
        {
            ts = ts-ts_num+1;
        }
        subobj2.if_id = htonl( (LOCAL_ID_TYPE_SUBNET_UNI_SRC << 16) |(pcen_link->reverse_link->link->iscds.front()->subnet_uni_info.subnet_uni_id <<8) | ts );
        subobj2.l2sc_vlantag = 0;
    }

    ero_track.push_back(subobj1);
    ero_track.push_back(subobj2);
}

void PCEN_MRN::SetVTagMask(ConstraintTagSet& vtagset)
{
    if (!vtag_mask)
        vtag_mask = new (narb_lsp_vtagmask_tlv);
    memset(vtag_mask, 0, sizeof(narb_lsp_vtagmask_tlv));
    vtag_mask->type = htons(TLV_TYPE_NARB_VTAG_MASK);
    vtag_mask->length= htons(sizeof(narb_lsp_vtagmask_tlv) - TLV_HDR_SIZE);
 
    list<u_int32_t>::iterator it = vtagset.TagSet().begin();
    for (; it != vtagset.TagSet().end(); it++)
    {
        SET_VLAN(vtag_mask->bitmask, *it);
    }
}

void PCEN_MRN::SetVTagToEROTrack(list<ero_subobj>& ero_track,  u_int16_t vtag)
{
    list<ero_subobj>::iterator iter;

    for (iter = ero_track.begin(); iter != ero_track.end(); iter++) 
    {
          if ((*iter).sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && (*iter).if_id == 0)
              (*iter).l2sc_vlantag = vtag;
     }
}

void PCEN_MRN::HandleMovazEROTrack(list<ero_subobj>& ero_track,  u_int16_t vtag)
{
    if (ero_track.size() == 0)
        return;

    list<ero_subobj>::iterator iter = ero_track.begin();
    list<ero_subobj>::iterator last_iter = iter;
    bool in_l2sc = ((*iter).sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC);
    for (; iter != ero_track.end(); iter++) 
    {
        //@@@@VLSR<--->TDM/LSC-SW Numbered Link special handling
        //Leaving L2SC (VLSR via numbered link)
        if (in_l2sc && (*iter).sw_type != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
        {
            (*last_iter).l2sc_vlantag = 0;
            (*last_iter).if_id = 0;
            in_l2sc = false;
        }
        //@@@@VLSR<--->TDM/LSC-SW Numbered Link special handling
        //Enterig L2SC (VLSR via numbered link)
        else if (!in_l2sc && ((*iter).sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
            || (*iter).if_id == 0 && (*iter).l2sc_vlantag ==0))
        {
            ++iter;
            if (iter != ero_track.end())
            {
                (*iter).l2sc_vlantag = vtag;
                (*iter).sw_type = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
            }
            else
                break;
            in_l2sc = true;
        }
        last_iter = iter;
    }
    
}


void PCEN_MRN::HandleSubnetUNIEROTrack(list<ero_subobj>& ero_track)
{
    list<ero_subobj> subnet_ero_tmp;

    if (ero_track.size() == 0)
        return;

    // removing ero_subojs from constructed src/dest local-id links
    if ( (src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        if (ero_track.front().sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
            ero_track.pop_front();
        if (ero_track.size() == 0)
            return;
    }
    if ( (dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        if (ero_track.back().sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
            ero_track.pop_back();
        if (ero_track.size() == 0)
            return;
    }

    list<ero_subobj>::iterator iter = ero_track.begin();
    list<ero_subobj>::iterator uni_src = ero_track.end();
    list<ero_subobj>::iterator uni_dest = ero_track.end();

    for (; iter != ero_track.end(); iter++) 
    {
        if ( (ntohl((*iter).if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC && uni_src == ero_track.end())
            uni_src = iter;
        if ( (ntohl((*iter).if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST )
            uni_dest = iter;
    }

    //for multi-vlsr-home only
    iter = uni_dest;
    if ( (--iter) != uni_src && (*iter).sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC )
    {
        uni_dest = iter;
    }

    subnet_ero_tmp.clear(); //SUBNET_ERO is optional TLV defiend in class rce_pcen.hh
    if (uni_src != ero_track.end() && uni_dest != ero_track.end())
    {
        if ((++uni_src) != uni_dest)
        {
            subnet_ero_tmp.assign(uni_src, uni_dest);
            ero_track.erase(uni_src, uni_dest);
        }
    }

    list<PCENLink*> path_loop_trace;
    list<PCENLink*>::iterator iter_path_loop;
    list<ero_subobj>::iterator iter_ero_loop;
    // handling arbitrary number of subnet home vlsrs baed on the size of subnet_ero_tmp
    iter = subnet_ero_tmp.begin();
    while (iter != subnet_ero_tmp.end())
    {
        int i;
        u_int32_t subnet_node_lcl = 0;
        u_int32_t subnet_node_rmt = 0;
        u_int32_t home_vlsr_lcl = 0;
        u_int32_t home_vlsr_rmt = 0;
        PCENLink* home_vlsr_link = NULL;

        // two ends of the link between CDs
        u_int32_t subnet_link_lcl = (*iter).addr.s_addr;
        iter++;
        if (iter == subnet_ero_tmp.end())
            break;
        u_int32_t subnet_link_rmt = (*iter).addr.s_addr;
        iter++;

        for (i = 0; i < links.size(); i++)
        {
            if (links[i]->link->lclIfAddr == subnet_link_lcl && links[i]->link->rmtIfAddr == subnet_link_rmt)
            {
                subnet_node_lcl = links[i]->lcl_end->router->id;
                subnet_node_rmt = links[i]->rmt_end->router->id;
            }
        }
        // corresponding CD rotuer IDs (must be non-zero)
        if (subnet_node_lcl == 0 || subnet_node_rmt == 0)
            continue;
        // corresponding home VLSR router IDs (must be non-zero and non-equal)
        home_vlsr_lcl = SystemConfig::FindHomeVlsrByRouterId(subnet_node_lcl);
        home_vlsr_rmt = SystemConfig::FindHomeVlsrByRouterId(subnet_node_rmt);
        if (home_vlsr_lcl == 0 || home_vlsr_rmt == 0 || home_vlsr_lcl == home_vlsr_rmt)
            continue;
        // TE link between two home VLSRs
        for (i = 0; i < links.size(); i++)
        {
            if (links[i]->link->advRtId == home_vlsr_lcl && links[i]->link->id == home_vlsr_rmt)
            {
                home_vlsr_link = links[i];
                break;
            }
        }
        // add two ends of the TE link into ERO as IPv4 subobjects
        if (home_vlsr_link != NULL)
        {
            ero_subobj subobj1, subobj2;
            memset(&subobj1, 0, sizeof(ero_subobj));
            subobj1.hop_type = ERO_TYPE_STRICT_HOP;
            subobj1.prefix_len = 32;
            subobj1.addr.s_addr = home_vlsr_link->link->lclIfAddr;
            ero_track.insert(uni_dest, subobj1);

            memset(&subobj2, 0, sizeof(ero_subobj));
            subobj2.hop_type = ERO_TYPE_STRICT_HOP;
            subobj2.prefix_len = 32;
            subobj2.addr.s_addr = home_vlsr_link->link->rmtIfAddr;
            ero_track.insert(uni_dest, subobj2);    

            // $$$$ logic to remove loops from the vlsr-level ero/path
            path_loop_trace.push_back(home_vlsr_link);
            // search for loop (advRtID of a previous link == ID or this link)
            for (iter_path_loop = path_loop_trace.begin(); (*iter_path_loop) != home_vlsr_link; iter_path_loop++)
            {
                // if found, get the iter headend location of the loop in ero_track
                if ((*iter_path_loop)->link->AdvRtId() == home_vlsr_link->link->Id())
                {
                    list<ero_subobj>::iterator iter_ero_track_end = uni_dest;
                    iter_ero_track_end--; iter_ero_track_end--;
                    for (iter_ero_loop = ero_track.begin(); iter_ero_loop != iter_ero_track_end; iter_ero_loop++)
                    {
                        if ((*iter_ero_loop).addr.s_addr == (*iter_path_loop)->link->LclIfAddr())
                            break;
                    }
                    if (iter_ero_loop != iter_ero_track_end)
                    {
                        //remove (iter, uni_dest); (also remote the loop in path)
                        path_loop_trace.erase(iter_path_loop, path_loop_trace.end());
                        ero_track.erase(iter_ero_loop, uni_dest);
                        break;
                    }
                }
            }
        }
    }

    if (!is_subnet_ero2dtl_enabled)
    {
        subnet_ero.assign(subnet_ero_tmp.begin(), subnet_ero_tmp.end());
    }
}

void PCEN_MRN::PreserveSceneToStacks(PCENNode& node)
{
    TSpecStack.push_front(node.tspec);
    WaveSetStack.push_front(node.waveset);
    VtagSetStack.push_front(node.vtagset);
    MinCostStack.push_front(node.minCost);
    PathStack.push_front(node.path);
    EROTrackStack.push_front(node.ero_track);
    PathVisitedStack.push_front(node.path_visited);
}

void PCEN_MRN::RestoreSceneFromStacks(PCENNode& node)
{
    PCENNode * destNode = GetNodeByIp(routers, &destination);

    if (&node != destNode)
      node.tspec = TSpecStack.front();
    TSpecStack.pop_front();
    if (&node != destNode)
      node.waveset = WaveSetStack.front();
    WaveSetStack.pop_front();
    if (&node != destNode)
      node.vtagset = VtagSetStack.front();
    VtagSetStack.pop_front();
    if (&node != destNode)
      node.minCost = MinCostStack.front();
    MinCostStack.pop_front();
    if (&node != destNode)
      node.path = PathStack.front();
    PathStack.pop_front();
    if (&node != destNode)
      node.ero_track = EROTrackStack.front();
    EROTrackStack.pop_front();
    if (&node != destNode)
      node.path_visited = PathVisitedStack.front();
    PathVisitedStack.pop_front();
}
    
int PCEN_MRN::PerformComputation()
{
    double final_cost = PCEN_INFINITE_COST;
    ero.clear();
    // Get the source and destination nodes
    PCENNode * srcNode = GetNodeByIp(routers, &source);
    PCENNode * destNode = GetNodeByIp(routers, &destination);
    srcNode->minCost = 0;

    // Initialization
    if (is_e2e_tagged_vlan)
    {
        if (vtag_mask)
        {
            srcNode->vtagset.AddTags(vtag_mask->bitmask, ntohs(vtag_mask->length)*8);
        }
        else
        {
            srcNode->vtagset.AddTag(vtag);
        }
    }
    PStack.push_front(srcNode);
    PreserveSceneToStacks(*srcNode);

    // Searching...
    while (PStack.size() > 0)
    {
        PCENNode *headNode = PStack.front();
        PStack.pop_front();
        assert(headNode);
        RestoreSceneFromStacks(*headNode);

        // checking excluded swcap layers
        if (IsInExcludedLayer(headNode))
            continue;

#ifdef DISPLAY_ROUTING_DETAILS
        cout<<"Head Node ";
        headNode->DisplayInfo();
        cout<<endl;
#endif

        PCENLink *nextLink;
        PCENNode *nextNode;
        bool link_visited;
        ConstraintTagSet nextWaveSet;
        ConstraintTagSet nextVtagSet;
        TSpec tspec_egress;
        tspec_egress.Update(switching_type_egress, encoding_type_egress, bandwidth_egress);

        list<PCENLink*>::iterator it_nextLink;
        for (it_nextLink = headNode->out_links.begin(); it_nextLink != headNode->out_links.end(); it_nextLink++)
        {
            nextLink = *it_nextLink;
            link_visited = (nextLink->lflg.lfg.visited == 1);
            nextLink->lflg.lfg.visited = 1;
            nextNode = nextLink->rmt_end;
            if (!nextNode)
                continue;
#ifdef DISPLAY_ROUTING_DETAILS
            cout<<"Trying next: ";
            nextLink->DisplayInfo();
            nextNode->DisplayInfo();
            cout<<endl;
#endif

            // check if nextLink satisfies Tspec constraints of headNode
            if (!nextLink->IsAvailableForTspec(headNode->tspec))
                continue;

            // @@@@ handling Wavelength Continuity constraints
            // A wavelength set has existed in headNode, now match it with the set of available waves on the link.
            //$$$$ Special handling for Movaz/ADVA private wavelength constraints
            nextWaveSet.TagSet().clear();
            if (is_via_movaz &&  headNode->tspec.SWtype == MOVAZ_LSC)
            {
#ifdef DISPLAY_ROUTING_DETAILS
                cout << "HeadNode->waveset: ";
                headNode->waveset.DisplayTags();
#endif
                nextLink->ProceedByUpdatingWaves(headNode->waveset, nextWaveSet); 
                //nextNode->waveset to be updaed later
#ifdef DISPLAY_ROUTING_DETAILS
                cout << "NextLink ";
                nextWaveSet.DisplayTags();
#endif
                if (nextWaveSet.IsEmpty())
                    continue;
            }
            
            //Handling E2E Tagged VLAN constraint
            nextVtagSet.TagSet().clear();
            if (is_e2e_tagged_vlan)
            {
                // @@@@ bidirectional constraints (TODO)
#ifdef DISPLAY_ROUTING_DETAILS
                cout << "HeadNode->vtagset: ";
                headNode->vtagset.DisplayTags();
#endif

                nextLink->ProceedByUpdatingVtags(headNode->vtagset, nextVtagSet);
                //nextNode->vtagset to be updaed later

#ifdef DISPLAY_ROUTING_DETAILS
                cout << "NextLink ";
                nextVtagSet.DisplayTags();
#endif
                //$$$$Note that nextVtagSet should be equal to headNodeTagSet if next link is not a tagged VLAN link
                if (nextVtagSet.IsEmpty())
                    continue;
            }
            
            if (nextNode == destNode && headNode != srcNode) //signle link path does not need these checkings
            {
                assert(headNode->path.front() && headNode->path.front()->link);
                if(!headNode->path.front()->lcl_end)
                {
                    LOGF("Wrong TE link [%x->%x]", headNode->path.front()->link->advRtId, headNode->path.front()->link->id);
                }

                if (!nextLink->CanBeEgressLink(tspec_egress))
                    continue;

                // @@@@ With VTAG constraint, the link can be egress only if its reverse link also satisfies the vtag constraint.
                // @@@@ Move to above as part of the *birectional* contraint ???
                //Excluding allocated VLAN tags on the destination node (last hop).
                if (is_e2e_tagged_vlan)
                {
                    list<PCENLink*>::iterator it_link;
                    for (it_link = destNode->in_links.begin(); it_link != destNode->in_links.end(); it_link++)
                    {
                        (*it_link)->ExcludeAllocatedVtags(nextVtagSet);
                    }
                    for (it_link = destNode->out_links.begin(); it_link != destNode->out_links.end(); it_link++)
                    {
                        if (*it_link != nextLink)
                            (*it_link)->ExcludeAllocatedVtags(nextVtagSet);
                    }
                    if (nextVtagSet.IsEmpty())
                        continue;
                }
            }

            // check if headNode->path + nextNode makes a loop
            if (IsLoop(headNode->path, nextNode))
                continue;

            nextNode->tspec = headNode->tspec;

            // check if nextLink cross region boundary
            if (IsCrossingRegionBoundary(nextLink, nextNode->tspec))
            {
                //$$$$ VLSR<->RayExpress crossing should be properly recognized
                // Update nextNode->tspec according to the next region constraints
                GetNextRegionTspec(nextLink, nextNode->tspec);

                //$$$$ Movaz subnet special handling
                if (is_via_movaz && nextNode->tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)
                {
                    //$$$$ change MOVAZ Switching Type to 151 (MOVAZ_LSC instead of standard LSC (150)) ???
                    //$$$$ Get MOVAZ proprietary information, e.g. TE lambda's. 
                    //$$$$ Picka set of lambda's by mapping capacity/encoding, requiring proper configuration of port rate
                    //$$$$ ---> using a fixed mapping table ...

                    nextNode->tspec.SWtype = MOVAZ_LSC;
                    //Setting intial TE lambda/wavelength information into nextNode->waveset
                    if (InitiateMovazWaves(nextWaveSet, nextLink) < 0)
                        continue;
                }

                //$$$$ TDM (Ciena CDs) subnet special handling 
                //@@@@ We might make a special Ciena sw_type like CIENA_TDM (101) ...
                if ( SystemConfig::should_incorporate_subnet ) 
                {
                    // $$$$ Checking available timeslots on the region-border interface getting out of TDM region
                    if (headNode->tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM)
                    {                    
                        if (CheckTimeslotsAvailability(nextLink, nextNode->tspec.Bandwidth) == 0)
                            continue;
                    }
                    // getting into TDM region
                    else if (nextNode->tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM  && nextLink->reverse_link)
                    {
                        if (CheckTimeslotsAvailability(nextLink->reverse_link, headNode->tspec.Bandwidth) == 0)
                            continue;
                    }
                }
            }

            if (is_bidirectional)
                if (!nextLink->reverse_link || !nextLink->reverse_link->IsAvailableForTspec(nextNode->tspec))
                    continue;

            if ((nextNode->nflg.nfg.visited == 1))
            {
                double new_cost = headNode->minCost+(nextLink->PCENmetric());
                if (nextNode->minCost <= new_cost && nextNode == destNode)
                    continue; //always keep the lowest-cost path on the destination node
                else if (nextNode->minCost > new_cost || !link_visited || !headNode->path_visited)
                    nextNode->minCost = new_cost; //allowing for new path from unvisited links even it has higher cost.
                else 
                    continue;
            }
            else
            {
                nextNode->minCost = headNode->minCost + nextLink->PCENmetric();
                nextNode->nflg.nfg.visited = 1;
            }

            //keeping the trace to indicate whether any (even one) link in the current path has been visited
            headNode->path_visited = (headNode->path_visited || link_visited);

            //proceed with new wavelengthSet
            nextNode->waveset = nextWaveSet;

            //proceed with new vlanTagSet
            nextNode->vtagset = nextVtagSet;

            (nextNode->path).assign(headNode->path.begin(), headNode->path.end());
            nextNode->path.push_back(nextLink);

            (nextNode->ero_track).assign(headNode->ero_track.begin(), headNode->ero_track.end());
            AddLinkToEROTrack(nextNode->ero_track, nextLink);

            //keep the best result
            if (nextNode == destNode && final_cost > nextNode->minCost)
            {
                final_cost = destNode->minCost;
                ero.assign(destNode->ero_track.begin(), destNode->ero_track.end());
            }
#ifdef DISPLAY_ROUTING_DETAILS
            nextNode->DisplayInfo();
            nextNode->ShowPath();
            cout << endl;
#endif
            PStack.push_front(nextNode);
            PreserveSceneToStacks(*nextNode);
        }
    }


    //Handling E2E Tagged VLAN constraint
    if (is_e2e_tagged_vlan)
    {
        if (destNode->vtagset.IsEmpty())
            return ERR_PCEN_NO_ROUTE;

        if (vtag == ANY_VTAG) {
		//TODO: New VTAG selection logic to facilitate Q-Conf process (for example, random selection)
		//$$$$ This logic would be better implemented in NARB...
            vtag  = destNode->vtagset.TagSet().front();
            if (vtag > MAX_VLAN_NUM)
                vtag = 0;
            SetVTagToEROTrack(ero, vtag);
            if ( destNode->vtagset.TagSet().size() >1 && (options & LSP_OPT_REQ_ALL_VTAGS) ) //?LSP_OPT_VTAG_MASK
                SetVTagMask(destNode->vtagset);
        }

        if (is_via_movaz)
            HandleMovazEROTrack(ero, vtag);
    }

    if (ero.size() == 0)
        return ERR_PCEN_NO_ROUTE;

    return ERR_PCEN_NO_ERROR;
}

