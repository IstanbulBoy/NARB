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

#include "radixtree.hh"
#include "resource.hh"
#include "rce_filter.hh"
#include "rce_pcen.hh"
#include "pcen_mrn.hh"
#include <math.h>

PCEN_MRN::PCEN_MRN(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, 
        float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, float bw_egress, u_int32_t opts, 
        u_int32_t lspq_id, u_int32_t msg_seqnum, u_int32_t tag)
    :   PCEN(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, lspq_id, msg_seqnum, tag) 
{
}

PCEN_MRN::~PCEN_MRN()
{

}

void PCEN_MRN::PostBuildTopology()
{
    int i;
    PCENLink *pcen_link;
    PCENNode *pcen_node;

    //Init Tspec
    for (i = 0; i < routers.size(); i++)
    {
        pcen_node = routers[i];
        pcen_node->tspec.SWtype = switching_type_ingress; //$$$$
        pcen_node->tspec.ENCtype = encoding_type_ingress; //$$$$
        pcen_node->tspec.Bandwidth = bandwidth_ingress; //$$$$
    }

    //Init reverseLink
    for (i = 0; i < links.size(); i++)
    {
        pcen_link = links[i];
        assert(pcen_link);

        pcen_node = pcen_link->rmt_end;
        if (!pcen_node)
            continue;
        list<PCENLink *>::iterator it_link;
        for (it_link = pcen_node->out_links.begin(); it_link != pcen_node->out_links.end(); it_link++)
        {
            assert(*it_link);
            if ((*it_link)->rmt_end == pcen_link->lcl_end && (*it_link)->link->rmtIfAddr == pcen_link->link->lclIfAddr
				&& (*it_link)->link->type == pcen_link->link->type)
            {
                pcen_link->reverse_link = *it_link;
                break;
            }
        }
    }
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

    PostBuildTopology();

    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN_MRN::PerformComputation() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
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
                tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
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

void PCEN_MRN::AddLinkToEROTrack(list<ero_subobj>& ero_track,  PCENLink* pcen_link)
{
    ero_subobj subobj1, subobj2;
    memset(&subobj1, 0, sizeof(ero_subobj));
    subobj1.prefix_len = 32;
    subobj1.addr.s_addr = pcen_link->link->lclIfAddr;
    memset(&subobj2, 0, sizeof(ero_subobj));
    subobj2.prefix_len = 32;
    subobj2.addr.s_addr = pcen_link->link->rmtIfAddr;

    if (pcen_link->link->type == RTYPE_LOC_PHY_LNK)
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_STRICT_HOP;
    else 
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_LOOSE_HOP;

    subobj1.sw_type = subobj2.sw_type = pcen_link->lcl_end->tspec.SWtype;
    subobj1.encoding = subobj2.encoding = pcen_link->lcl_end->tspec.ENCtype;
    subobj1.bandwidth= subobj2.bandwidth = pcen_link->lcl_end->tspec.Bandwidth;

    if (is_e2e_tagged_vlan)
    {
        if (pcen_link->link && pcen_link->link->iscds.front()->vlan_info.version == IFSWCAP_SPECIFIC_VLAN_VERSION)
            subobj1.l2sc_vlantag = vtag; //*(u_int16_t *)subobj1.pad
        if (pcen_link->reverse_link && pcen_link->reverse_link->link && pcen_link->reverse_link->link->iscds.front()
            && pcen_link->reverse_link->link->iscds.front()->vlan_info.version == IFSWCAP_SPECIFIC_VLAN_VERSION)
            subobj2.l2sc_vlantag = vtag; //*(u_int16_t *)subobj2.pad
    } 

    ero_track.push_back(subobj1);
    ero_track.push_back(subobj2);
}

void PCEN_MRN::SetVTagToEROTrack(list<ero_subobj>& ero_track,  u_int16_t vtag)
{
    list<ero_subobj>::iterator iter;

    for (iter = ero_track.begin(); iter != ero_track.end(); iter++) 
    {
          (*iter).l2sc_vlantag = vtag;
     }
}

void PCEN_MRN::PreserveScenceToStacks(PCENNode& node)
{
    TSpecStack.push_front(node.tspec);
    WaveSetStack.push_front(node.waveset);
    VtagSetStack.push_front(node.vtagset);
    MinCostStack.push_front(node.minCost);
    PathStack.push_front(node.path);
    EROTrackStack.push_front(node.ero_track);
}

void PCEN_MRN::RestoreScenceFromStacks(PCENNode& node)
{
    node.tspec = TSpecStack.front();
    TSpecStack.pop_front();
    node.waveset = WaveSetStack.front();
    WaveSetStack.pop_front();
    node.vtagset = VtagSetStack.front();
    VtagSetStack.pop_front();    
    node.minCost = MinCostStack.front();
    MinCostStack.pop_front();
    node.path = PathStack.front();
    PathStack.pop_front();
    node.ero_track = EROTrackStack.front();
    EROTrackStack.pop_front();
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
        srcNode->vtagset.AddTag(vtag);
    PStack.push_front(srcNode);
    PreserveScenceToStacks(*srcNode);

    while (PStack.size() > 0)
    {
        PCENNode *headNode = PStack.front();
        PStack.pop_front();
        assert(headNode);
        RestoreScenceFromStacks(*headNode);

#ifdef DISPLAY_ROUTING_DETAILS
        cout<<"Head Node ";
        headNode->DisplayInfo();
        cout<<endl;
#endif

        PCENLink *nextLink;
        PCENNode *nextNode;
        bool link_visited;
        ConstraintTagSet nextTagSet;
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

            // @@@@ 
            // If (headNode->tspec.SWtype == LSC)
            //handling Wavelength Continuity constraints

            nextTagSet.TagSet().clear();

            //Handling E2E Tagged VLAN constraint
            if (is_e2e_tagged_vlan)
            {
                // @@@@ bidirectional constraints (TODO)
#ifdef DISPLAY_ROUTING_DETAILS
                cout << "HeadNode ";
                headNode->vtagset.DisplayTags();
#endif
                nextLink->ProceedByUpdatingVtags(headNode->vtagset, nextTagSet); //nextNode->vtagset
#ifdef DISPLAY_ROUTING_DETAILS
                cout << "NextLink ";
                nextTagSet.DisplayTags();
#endif
                if (nextTagSet.IsEmpty())
                    continue;
            }
            
            if (nextNode == destNode && headNode != srcNode)
            {
                assert(headNode->path.front() && headNode->path.front()->link);
                if(!headNode->path.front()->lcl_end)
                {
                    LOGF("Wrong TE link [%x->%x]", headNode->path.front()->link->advRtId, headNode->path.front()->link->id);
                }

                if (!nextLink->CanBeEgressLink(tspec_egress))
                    continue;

                // @@@@ With VTAG constraint, the link can be egress only if its reverse link also satisfies the vtag constraint.
                // @@@@ Move above as part of the *birectional* contraint
                /*
                if (is_e2e_tagged_vlan) 
                    if (!nextNode->vtagset.HasTag(vtag))
                        continue;
                */
            }

            // check if headNode->path + nextNode makes a loop
            if (IsLoop(headNode->path, nextNode))
                continue;

            nextNode->tspec = headNode->tspec;


            // check if nextLink cross region boundary
            if (IsCrossingRegionBoundary(nextLink, nextNode->tspec))
            {
                // Update nextNode->tspec according to the next region constraints
                GetNextRegionTspec(nextLink, nextNode->tspec);
                // LOG("Crossing region boudary"<<endl);
            }

            if (is_bidirectional)
                if (!nextLink->reverse_link || !nextLink->reverse_link->IsAvailableForTspec(nextNode->tspec))
                    continue;

            if ((nextNode->nflg.nfg.visited == 1))
            {
                double new_cost = headNode->minCost+(nextLink->PCENmetric());
                if (nextNode->minCost > new_cost || !link_visited)
                    nextNode->minCost = new_cost;
                else 
                    continue;
            }
            else
            {
                nextNode->minCost = headNode->minCost + nextLink->PCENmetric();
                nextNode->nflg.nfg.visited = 1;
            }

            //proceed with new vlanTagSet
            nextNode->vtagset = nextTagSet;

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
            PreserveScenceToStacks(*nextNode);
        }
    }


    //Handling E2E Tagged VLAN constraint
    if (is_e2e_tagged_vlan)
    {
        if (destNode->vtagset.IsEmpty())
            return ERR_PCEN_NO_ROUTE;
        if (vtag == ANY_VTAG) {
            vtag  = destNode->vtagset.TagSet().front();
            SetVTagToEROTrack(destNode->ero_track, vtag);
        }
    }

    if (ero.size() == 0)
        return ERR_PCEN_NO_ROUTE;

    return ERR_PCEN_NO_ERROR;
}

