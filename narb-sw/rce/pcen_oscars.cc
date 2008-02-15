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
            //$$$$ all timeslots ???? (@@@@ picked timeslots shoud be ignored by OSCARS!)
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
    /*
    if (SystemConfig::should_incorporate_subnet)
    {
        HandleSubnetUNIEROTrack(ero);
        if (ero.size() == 0)
            ReplyErrorCode(ERR_PCEN_NO_ROUTE);
    }
    ero_vlsr_alts.push_back(ero);
    ero_subnet_alts.push_back(subnet_ero);
    */

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
        //@@@@ReplyErrorCode(ret);
        ReplyAltPathEROs();
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
    FinishMaxDisjointPaths();

    //Generating and returninig the two diverse paths 
    //  if the initial shortest path is different than the two diverse paths return it too (three path set)
    //      --> Done in FinishMaxDisjointPaths();
    //   otherwise return the two path set
    ReplyAltPathEROs();
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
        //@@@@ Should have botain path from ero_alts[1];
        list<PCENLink*>& path = path_alts[i];
        list<PCENLink*>::iterator iter_link = path.begin();
        PCENLink* pcen_link;
        for (; iter_link != path.end(); iter_link++)
        {
            pcen_link = (*iter_link);
            pcen_link->link->SetMetric(PCEN_VERYBIG_COST);
            if (pcen_link->reverse_link && pcen_link->reverse_link->link && pcen_link->link->Metric() > 0)
                pcen_link->reverse_link->link->SetMetric(-pcen_link->link->Metric());
        }
    }
}


void PCEN_OSCARS::FinishMaxDisjointPaths()
{
    // store the initial CSPF path ...
    path_alts.push_back(path_alts[0]);
    ero_alts.push_back(ero_alts[0]);

    // locate and trim opposite-common segments, plus head-tail swapping
    // @@@@ only handling two paths at this moment
    while (TrimOppositeSharedSegmentAndSwapTail(ero_alts[0], ero_alts[1]))
        ;  

    // if the initial shortest path is different than the two diverse paths return it too (three path set)
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
        subnet_ero.clear(); //??
        ero_vlsr_alts.push_back(ero_alts[i]);
        if (SystemConfig::should_incorporate_subnet)
        {
            HandleSubnetUNIEROTrack(ero_vlsr_alts.back());
        }
        ero_subnet_alts.push_back(subnet_ero); // could be empty
    }
}

bool PCEN_OSCARS::TrimOppositeSharedSegmentAndSwapTail(list<ero_subobj>& ero1, list<ero_subobj>& ero2)
{
    list<ero_subobj>::iterator iter1a, iter2a;
    list<ero_subobj>::reverse_iterator iter1z, iter2z;
    list<ero_subobj> ero1_tail, ero2_tail;
    ero_subobj *subobj1, *subobj2;

    bool found1 = false;
    iter1a = ero1.begin();
    for (++iter1a; iter1a != ero1.end(); iter1a++)
    {
        subobj1 = &(*iter1a);
        iter2z = ero2.rbegin();
        for (++iter2z; iter2z != ero2.rend(); iter2z++)
        {
            subobj2 = &(*iter2z);
            if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
            {
                found1 = true;
                break;
            }
        }
        if (found1)  break;
    }

    bool found2 = false;
    iter2a = ero2.begin();
    for (++iter2a; iter2a != ero2.end(); iter2a++)
    {
        subobj1 = &(*iter2a);
        iter1z = ero1.rbegin();
        for (++iter1z; iter1z != ero1.rend(); iter1z++)
        {
            subobj2 = &(*iter1z);
            if (memcmp(subobj1, subobj2, sizeof(ero_subobj)) == 0)
            {
                found2 = true;
                break;
            }
        }
        if (found2)  break;
    }

    if (!found1 || !found2)
        return false;

    assert (memcmp(&(*iter1a), &(*iter1z), sizeof(ero_subobj)) != 0);
    assert (memcmp(&(*iter2a), &(*iter2z), sizeof(ero_subobj)) != 0);
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

void PCEN_OSCARS::ReplyAltPathEROs()
{
    //returning non-empty components in ero-vlsr-alts and ero-subnet-alts.

    //log info
    char addr[20];
    ero_subobj* subobj;
    list<ero_subobj>::iterator iter;
    LOGF("---------PCEN_OSCARS::ReplyAltPathEROs ouput---------\n");
    for (int i = 0; i < ero_vlsr_alts.size(); i++)
    {
        LOGF(">> Path #%d VLSR ERO:\n", i);        
        for (iter = ero_vlsr_alts[i].begin(); iter != ero_vlsr_alts[i].end(); iter++)
        {
            subobj = &(*iter);
            inet_ntop(AF_INET, &subobj->addr, addr, 20);
            LOGF("-->> HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag));
        }
        LOGF("++>> Path #%d Subnet ERO:n", i);        
        for (iter = ero_subnet_alts[i].begin(); iter != ero_subnet_alts[i].end(); iter++)
        {
            subobj = &(*iter);
            inet_ntop(AF_INET, &subobj->addr, addr, 20);
            LOGF("--++>> HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d): vtag:%d]\n", subobj->hop_type?"loose":"strict", addr,  ntohl(subobj->if_id), ntohl(subobj->if_id)>>16, (u_int16_t)ntohl(subobj->if_id), ntohs(subobj->l2sc_vlantag));
        }
    }

}

