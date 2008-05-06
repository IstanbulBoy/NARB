/*
 * Copyright (c) 2008
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2008
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
#include "rce_pcen.hh"
#include "pcen_mrn.hh"
#include "pcen_oscars.hh"

bool operator==(list<ero_subobj>& path1, list<ero_subobj>& path2)
{
    list<ero_subobj>::iterator iter1, iter2;
    if (path1.size() != path2.size())
        return false;
    for (iter1 = path1.begin(), iter2 = path2.begin(); iter1 != path1.end(); iter1++, iter2++)
    {
        if (memcmp(&(*iter1), &(*iter2), sizeof(ero_subobj)) != 0)
            return false;
    }
    return true;
}

bool PCEN_OSCARS::PostBuildTopology()
{
    if (!PCEN_MRN::PostBuildTopology())
        return false;

    //special topology manipulations for PCEN_OSCARS
    //relaxing Bandwidth and VLAN Tag constraints...
    int i,j, lNum = links.size();
    PCENLink *pcen_link;

    for (i = 0; i < lNum; i++)
    {
        pcen_link = links[i];
        pcen_link->link = new Link(pcen_link->link);
        //The Link resource is self-allcated will will be deleted in ~PCENLink()...
        pcen_link->link_self_allocated = true;
        if (pcen_link->link->maxBandwidth > 0)
        {
            for (j = 0; j < 7; j++)
            {
                if (pcen_link->link->maxBandwidth > pcen_link->link->unreservedBandwidth[j])
                    pcen_link->link->unreservedBandwidth[j] = pcen_link->link->maxBandwidth;
            }
        }
        list<ISCD*>::iterator it = pcen_link->link->iscds.begin();
        IfSwCapDesc* iscd;
        for (; it != pcen_link->link->iscds.end(); it++)
        {
            iscd = *it;
            //$$$$ using max bandwidth
            for (j = 0; j < 7; j++)
            {
                if (pcen_link->link->maxBandwidth > iscd->max_lsp_bw[j] )
                    iscd->max_lsp_bw[j] = pcen_link->link->maxBandwidth;
            }
            //$$$$ all possible vtagss
            if ((htons(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) != 0)
            {
                for (j = 0; j < MAX_VLAN_NUM/8; j++)
                {
                    iscd->vlan_info.bitmask[j] = (iscd->vlan_info.bitmask[j] | iscd->vlan_info.bitmask_alloc[j]);
                    iscd->vlan_info.bitmask_alloc[j] = 0;
                }
            }
            //$$$$ all timeslots ???? (--> picked timeslots shoud be ignored by OSCARS!)
            if ((htons(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0)
            {
                iscd->subnet_uni_info.first_timeslot = 0;
                for (j = 0; j < MAX_TIMESLOTS_NUM/8; j++)
                {
                    iscd->subnet_uni_info.timeslot_bitmask[j] = 0xff;
                }
            }            
        }
            
    }

    return true;
}

int PCEN_OSCARS::PerformComputation()
{
    return PCEN_MRN::PerformComputation();
}

int PCEN_OSCARS::VerifyPathWithERO()
{
    // there must be user supplied ero
    if (user_ero.size() ==0)
        return -1;
    // there must be prepared topology
    if (routers.size() == 0 || links.size() ==0)
        return -2;

    bool should_verify_subnet_ero = false;

    //handling stub subnet interface subobjects
    int src_1st_ts = 0, dest_1st_ts = 0;
    if ((htonl(user_ero.front().if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC)
    {
        src_lcl_id = ((LOCAL_ID_TYPE_SUBNET_IF_ID<<16) | htonl(user_ero.front().if_id)&0xffff);
        user_ero.pop_front();
    }
    if ((htonl(user_ero.back().if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST)
    {
        dest_lcl_id = ((LOCAL_ID_TYPE_SUBNET_IF_ID<<16) | htonl(user_ero.front().if_id)&0xffff);
        user_ero.pop_back();
    }

    int ln;
    PCENLink* pcen_link = NULL;
    PCENNode* headNode = NULL;
    list<ero_subobj>::iterator iter;
    for (iter = user_ero.begin(); iter != user_ero.end(); iter++)
    {
        ero_subobj* subobj1 = &(*iter);
        ++iter;
        if (iter == user_ero.end())   return 1; //unpaired ero subobj
        ero_subobj* subobj2 = &(*iter);

        for ( pcen_link = NULL, ln = 0;  ln < links.size(); ln++ )
        {
            if (headNode != NULL && links[ln]->lcl_end == headNode
                && links[ln]->link->lclIfAddr == subobj1->addr.s_addr
                && links[ln]->link->rmtIfAddr == subobj2->addr.s_addr
                 && links[ln]->rmt_end != NULL)
            {
                pcen_link = links[ln];
                headNode = pcen_link->rmt_end;
                break;
            }
        }
        if (pcen_link == NULL)
            return 2; //continued link unfound!

        //verifying capacity
        if ((ntohl(subobj1->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_DEST)
        { // subnet interface as an intermediate subobj
            dest_1st_ts = CheckTimeslotsAvailability(pcen_link, bandwidth_ingress);
            if (dest_1st_ts == 0)
                return 3; //no sufficient subnet ingress timeslots
            //update 1st time slot
            subobj1->if_id = htonl((LOCAL_ID_TYPE_SUBNET_UNI_DEST << 16) | ntohl(subobj2->if_id)&0xff00 | dest_1st_ts&0xff);
            should_verify_subnet_ero = true;
        }
        else if (pcen_link->link->MaxReservableBandwidth() < bandwidth_ingress)
        { // regular vlsr level subobj
            return 4; //no sufficient bandwidth in forward direction
        }

        if ((ntohl(subobj2->if_id) >> 16) == LOCAL_ID_TYPE_SUBNET_UNI_SRC)
        { // subnet interface as an intermediate subobj
            src_1st_ts = CheckTimeslotsAvailability(pcen_link, bandwidth_ingress);
            if (src_1st_ts == 0)
                return 5; //no sufficient subnet egress timeslots
            //update 1st time slot
            subobj2->if_id = htonl((LOCAL_ID_TYPE_SUBNET_UNI_SRC << 16) | ntohl(subobj2->if_id)&0xff00 | src_1st_ts&0xff);
            should_verify_subnet_ero = true;
        }
        else if (pcen_link->reverse_link == NULL || pcen_link->reverse_link->link->MaxReservableBandwidth() < bandwidth_ingress)
        { // regular vlsr level subobj
            return 6; //no sufficient bandwidth in backward direction
        }
    }

    //handling subnet interface local ids
    if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        for ( pcen_link = NULL, ln = 0;  ln < links.size(); ln++ )
        {
            if (links[ln]->link->advRtId == source.s_addr && links[ln]->link->Iscds().size() > 0 
                && links[ln]->link->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                && (htons(links[ln]->link->Iscds().front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0 
                && links[ln]->link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((src_lcl_id >> 8) & 0xff))
            {
                pcen_link = links[ln];
                break;
            }
        }
        if (pcen_link == NULL)
            return 2; //no link found
        src_1st_ts = CheckTimeslotsAvailability(pcen_link, bandwidth_ingress);
        if (src_1st_ts == 0)
            return 5; //no sufficient subnet ingress timeslots
        ero_subobj subobj1;
        memset(&subobj1, 0, sizeof(ero_subobj));
        subobj1.hop_type = ERO_TYPE_STRICT_HOP;
        subobj1.prefix_len = 32;
        subobj1.addr.s_addr = pcen_link->link->LclIfAddr();
        subobj1.if_id = htonl((LOCAL_ID_TYPE_SUBNET_UNI_SRC << 16) | src_lcl_id&0xff00 | src_1st_ts&0xff);
        user_ero.push_front(subobj1);
        should_verify_subnet_ero = true;
    }
    if ((dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        for ( pcen_link = NULL, ln = 0;  ln < links.size(); ln++ )
        {
            if (links[ln]->link->advRtId == source.s_addr && links[ln]->link->Iscds().size() > 0 
                && links[ln]->link->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                && (htons(links[ln]->link->Iscds().front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0 
                && links[ln]->link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((dest_lcl_id >> 8) & 0xff))
            {
                pcen_link = links[ln];
                break;
            }
        }
        if (pcen_link == NULL)
            return 2; //no link found
        dest_1st_ts = CheckTimeslotsAvailability(pcen_link, bandwidth_ingress);
        if (dest_1st_ts == 0)
            return 3; //no sufficient subnet egress timeslots
        ero_subobj subobj2;
        memset(&subobj2, 0, sizeof(ero_subobj));
        subobj2.hop_type = ERO_TYPE_STRICT_HOP;
        subobj2.prefix_len = 32;
        subobj2.addr.s_addr = pcen_link->link->LclIfAddr();
        subobj2.if_id = htonl((LOCAL_ID_TYPE_SUBNET_UNI_DEST << 16) | dest_lcl_id&0xff00 | dest_1st_ts&0xff);
        user_ero.push_back(subobj2);
        should_verify_subnet_ero = true;
    }

    // success depends on further verification
    if (should_verify_subnet_ero)
        return VerifySubnetERO(); 

    // success
    return 0;
}

int PCEN_OSCARS::VerifySubnetERO()
{
    // there must be subnet ero
    if (subnet_ero.size() ==0)
        return -3;
    // there must be incorporated subnet topology
    if (!SystemConfig::should_incorporate_subnet)
        return -4;

    return 0;
}

void PCEN_OSCARS::Run()
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
        LOGF("PCEN_OSCARS::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_OSCARS::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    //modifying topology 
    //  1. subnet handling in PCEN_MRN::PostBuildTopology
    //  2. relaxing constraints for OSCARS requests
    //  -->augmenting bandwidth and vtag set wherever needed
    if (!PostBuildTopology())
        return;

    //PerformComputation --> Generating initial CSPF path (shortest)
    //  return error code if failed
    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN_OSCARS::PerformComputation() failed (source[%X]-destination[%X])\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    //Storing initial CSPF path 
    //destNode->path
    //  and ERO + SubnetERO
    PCENNode * destNode = GetNodeByIp(routers, &destination);
    path_alts.push_back(destNode->path);
    ero_alts.push_back(ero);

    //Cleaning up and modifying topology
    //Reset node tspec and other interim variables
    CleanUpTopology();

    //Trim links on forward path and assign VERY_BIG metric value to links on reverse path 
    //    This should work since we only look at the metric value (cost) of forward links
    //    Using destNode->path ($$$$: Using ERO and SubnetERO together to revisit the path)
    PrepareLinkDisjointSearch();

    //PerformComputation --> Generating maximally CSPF diverse path
    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN_OSCARS::PerformComputation() failed for max-diverse path (source[%X]-destination[%X])\n", source.s_addr, destination.s_addr);
        ReplyEROWithAltPaths();
        return;
    }

    //Storing max-diverse CSPF path (best-disjoint)
    path_alts.push_back(destNode->path);
    ero_alts.push_back(ero);
    /*
    ero_vlsr_alts.push_back(ero);
    ero_subnet_alts.push_back(subnet_ero);
    */

    //Cutting off opposite-common links and exchanging head and tail for the two paths
    CreateMaxDisjointPaths();

    //Generating and returninig the two diverse paths 
    //  if the initial shortest path is different than the two diverse paths return it too (three path set)
    //      --> Done in CreateMaxDisjointPaths();
    //   otherwise return the two path set
    ReplyEROWithAltPaths();
}

void PCEN_OSCARS::CleanUpTopology()
{
    int rNum = routers.size(); 
    int lNum = links.size();
    int i;

    //resetting Tspec and other interim varibles on nodes/routers 
    PCENNode* pcen_node;
    for (i = 0; i < rNum; i++)
    {
        pcen_node = routers[i];
        pcen_node->tspec.SWtype = switching_type_ingress;
        pcen_node->tspec.ENCtype = encoding_type_ingress;
        pcen_node->tspec.Bandwidth = bandwidth_ingress;
        pcen_node->ero_track.clear();
        pcen_node->path.clear();
        pcen_node->vtagset.TagSet().clear();
        pcen_node->waveset.TagSet().clear();
        pcen_node->minCost=PCEN_INFINITE_COST;
        pcen_node->path_visited = false;
        pcen_node->nflg.flag=0;
        pcen_node->auxvar1=0;
        pcen_node->auxvar2=0;

    }

    //resetting interim varibles on links 
    PCENLink* pcen_link;
    for (i = 0; i < lNum; i++)
    {
        pcen_link = links[i];
        pcen_link->lflg.flag=0;
        pcen_link->auxvar1=0;
        pcen_link->auxvar2=0;
    }

    //cleanning up PCEN stacks
    PStack.clear();
    TSpecStack.clear();
    WaveSetStack.clear();
    VtagSetStack.clear();
    MinCostStack.clear();
    PathVisitedStack.clear();
    PathStack.clear();
    EROTrackStack.clear();

    //cleanning up ERO and SubnetERO
    ero.clear();
    subnet_ero.clear();
}

void PCEN_OSCARS::PrepareLinkDisjointSearch()
{
    for (int i = 0; i < path_alts.size(); i++)
    {
        // At the end of each round of path computation, the path vector on destNode should be the optimal 
        // that corresponds to the final ERO. There for the following changes are made precisely on target links.
        list<PCENLink*>& path = path_alts[i];
        list<PCENLink*>::iterator iter_link = path.begin();
        PCENLink* pcen_link;
        for (; iter_link != path.end(); iter_link++)
        {
            pcen_link = (*iter_link);
            pcen_link->link->SetMetric(PCEN_VERYBIG_COST);
            if (pcen_link->reverse_link && pcen_link->reverse_link->link && pcen_link->reverse_link->link->Metric() > 0)
                pcen_link->reverse_link->link->SetMetric(-pcen_link->reverse_link->link->Metric());
        }
    }
}


void PCEN_OSCARS::CreateMaxDisjointPaths()
{
    // store the initial CSPF path ...
    path_alts.push_back(path_alts[0]);
    ero_alts.push_back(ero_alts[0]);

    // locate and trim opposite-common segments, plus head-tail swapping
    // $$$$ only handling two paths at this moment --> can do more in future!
    assert(ero_alts.size() == 3);
    while (TrimOppositeSharedSegmentAndSwapTail(ero_alts[1], ero_alts[2]))
        ;  

    // if the initial shortest path is different than the two diverse paths return it too (three path set)
    // otherwise the duplicate is removed (two path set)
    if (ero_alts[0] ==  ero_alts.back())
    {
        ero_alts.pop_back();
        path_alts.pop_back();
    }

    // generating vlsr_ero's and subnet_ero's !!
    ero_vlsr_alts.clear();
    ero_subnet_alts.clear();
    for (int i = 0; i < ero_alts.size(); i++)
    {
        subnet_ero.clear();
        ero_vlsr_alts.push_back(ero_alts[i]);
        if (SystemConfig::should_incorporate_subnet)
        {
            HandleSubnetUNIEROTrack(ero_vlsr_alts.back());
            ero_subnet_alts.push_back(subnet_ero); // could be empty
        }
    }
}

bool PCEN_OSCARS::TrimOppositeSharedSegmentAndSwapTail(list<ero_subobj>& ero1, list<ero_subobj>& ero2)
{
    list<ero_subobj>::iterator iter1a, iter2a;
    list<ero_subobj>::reverse_iterator iter1z, iter2z, iter1z_x, iter2z_x;
    list<ero_subobj> ero1_tail, ero2_tail;
    ero_subobj *subobj1, *subobj2;

    bool found1 = false, found2 = false;

    //skip common segment at the source end (usually one or two subobjects)
    iter1a = ero1.begin();
    iter2a = ero2.begin();
    while (iter1a != ero1.end() && iter2a != ero2.end() && memcmp(&(*iter1a), &(*iter2a), sizeof(ero_subobj)) == 0)
    {
        ++iter1a; 
        ++iter2a; 
    }

    //skip common segment at the destination end (usually one or two subobjects)
    iter1z_x = ero1.rbegin();
    iter2z_x = ero2.rbegin();
    while (iter1z_x != ero1.rend() && iter2z_x != ero2.rend() && memcmp(&(*iter1z_x), &(*iter2z_x), sizeof(ero_subobj)) == 0)
    {
        ++iter1z_x; 
        ++iter2z_x; 
    }

    //locate starting and ending points for the opposite common path segment
    for (++iter1a; iter1a != ero1.end(); iter1a++)
    {
        subobj1 = &(*iter1a);
        iter2z = iter2z_x;
        for (++iter2z; iter2z != ero2.rend(); iter2z++)
        {
            subobj2 = &(*iter2z);
            if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
            {
                ++iter1a;
                ++iter2z;
                if (iter1a == ero1.end() || iter2z == ero2.rend())
                {
                    break;
                }                
                subobj1 = &(*iter1a);
                subobj2 = &(*iter2z);
                --iter1a;
                --iter2z;
                if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
                {
                    found1 = true;
                    break;
                }
            }
        }
        if (found1 || iter1a == ero1.end())
        {
            break;
        }
    }
    for (++iter2a; iter2a != ero2.end(); iter2a++)
    {
        subobj1 = &(*iter2a);
        iter1z = iter1z_x;
        for (++iter1z; iter1z != ero1.rend(); iter1z++)
        {
            subobj2 = &(*iter1z);
            if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
            {
                ++iter2a;
                ++iter1z;
                if (iter2a == ero2.end() || iter1z == ero1.rend())
                {
                    break;
                }
                subobj1 = &(*iter2a);
                subobj2 = &(*iter1z);
                --iter2a;
                --iter1z;
                if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
                {
                    found2 = true;
                    break;
                }
            }
        }
        if (found2 || iter2a == ero2.end())  
        {
                break;
        }
    }
    if (!found1 || !found2)
        return false;
    assert (memcmp(&(*iter1a), &(*iter1z), sizeof(ero_subobj)) != 0);
    assert (memcmp(&(*iter2a), &(*iter2z), sizeof(ero_subobj)) != 0);

    //swap tail
    ero1_tail.assign(ero1.rbegin(), iter1z);
    ero1_tail.reverse();
    ero2_tail.assign(ero2.rbegin(), iter2z);
    ero2_tail.reverse();
    ero1.erase(iter1a, ero1.end());
    ero2.erase(iter2a, ero2.end());
    ero1.splice(ero1.end(), ero2_tail);
    ero2.splice(ero2.end(), ero1_tail);

    return true;
}

void PCEN_OSCARS::ReplyEROWithAltPaths()
{
    //returning non-empty components in ero-vlsr-alts and ero-subnet-alts.
    //ERO TLV with ero and SubnetERO TLV with subnet_ero
    assert (api_writer);

    // returnig the First/Shortest path as ero/subnet_ero (but carrying vtag_mask etc. from the last path)
    this->ero.assign(ero_vlsr_alts[0].begin(), ero_vlsr_alts[0].end());
    if (ero_subnet_alts.size() > 0)
        this->subnet_ero.assign(ero_subnet_alts[0].begin(), ero_subnet_alts[0].end());
    // Some other TLVs such as vtag_mask belong to the last path computation and may be inconsistent 
    // with the first path (shortest path). But they are not a concen here. When those TLVs are really
    // cared by OSCARS we can store and retrieve them with extra data structure...

    api_msg* msg = NewEROReplyMessage();
    assert (msg != NULL);
    //plus altanative EROs TLVs (ero_alts[1] and ero_alts[2] (if any)) 
    LOGF("##--DEBUG--## PCEN_OSCARS::ReplyEROWithAltPaths for LSP request (ucid=0x%x, seqnum=0x%x): Src 0x%x Dest 0x%x Bandwidth %g--##\n",  
        ucid, seqnum, source, destination, bandwidth_ingress);
    char buf[1000];
    te_tlv_header *ero_tlv = (te_tlv_header*)buf;
    char addr[20];
    ero_subobj* subobj;
    list<ero_subobj>::iterator iter;
    //  ero_vlsr_alts[0] is already included in ero (and subnet_ero if any)
    for (int i = 1; i < ero_vlsr_alts.size(); i++)
    {
        //$$$$ Special handling for HOP BACK ...
        list<ero_subobj>::iterator iter; 
        if (hop_back != 0)
        {
            for (iter = ero.begin(); iter != ero.end(); iter++)
            {
                if ((*iter).addr.s_addr == hop_back && iter != ero.begin())
                {
                    iter = ero.erase(ero.begin(), iter);
                    break;
                }
            }
            if (iter == ero.end()) // add a back_hop to the beginning
            {
                ero_subobj subobj_hopback = *(ero.begin());
                if (subobj_hopback.hop_type == ERO_TYPE_STRICT_HOP &&
                    (subobj_hopback.if_id >> 16) != LOCAL_ID_TYPE_SUBNET_UNI_SRC && 
                    (subobj_hopback.if_id >> 16) != LOCAL_ID_TYPE_SUBNET_UNI_DEST )
                {
                    subobj_hopback.addr.s_addr = hop_back;
                    ero.push_front(subobj_hopback);
                }
            }
        }

        LOGF(">> Alternate Path #%d VLSR ERO:\n", i);
        //$$$$ Making AltVlsrERO TLV
        ero_tlv->type = htons(TLV_TYPE_NARB_ALTERNATE_ERO);
        ero_tlv->length = htons(sizeof(ero_subobj)*ero_vlsr_alts[i].size());
        ero_subobj* ero_hop = (ero_subobj*)((char*)ero_tlv + TLV_HDR_SIZE);
        int j =0;
        for (iter = ero_vlsr_alts[i].begin(); iter != ero_vlsr_alts[i].end(); iter++, j++)
        {
            subobj = &(*iter);
            inet_ntop(AF_INET, &subobj->addr, addr, 20);
            LOGF("-->> HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag));
            *(ero_hop+j) = (*subobj);
        }
        msg = api_msg_append_tlv(msg, ero_tlv);

        if (SystemConfig::should_incorporate_subnet && ero_subnet_alts[i].size() > 0)
        {
            LOGF("++>> Alternate Path #%d Subnet ERO:\n", i);
            //$$$$ Making AltSubnetERO TLV
            ero_tlv->type = htons(TLV_TYPE_NARB_ALTERNATE_SUBNET_ERO);
            ero_tlv->length = htons(sizeof(ero_subobj)*ero_subnet_alts[i].size());
            ero_hop = (ero_subobj*)((char*)ero_tlv + TLV_HDR_SIZE);
            j = 0;
            for (iter = ero_subnet_alts[i].begin(); iter != ero_subnet_alts[i].end(); iter++, j++)
            {
                subobj = &(*iter);
                inet_ntop(AF_INET, &subobj->addr, addr, 20);
                LOGF("--++>> HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag));
                *(ero_hop+j) = (*subobj);
            }
            msg = api_msg_append_tlv(msg, ero_tlv);
        }
    }

    LOGF("##---END---## PCEN_OSCARS::ReplyEROWithAltPaths ---------------##\n");
    api_writer->PostMessage(msg);
}

