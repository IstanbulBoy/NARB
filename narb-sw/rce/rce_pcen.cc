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
#include "rce_lsa.hh"
#include "rce_movaz_types.hh"
#include <netinet/in.h>
#include <arpa/inet.h>

u_int32_t PCENNode::DomainId ()
{
    if (router == NULL)
        return DOMAIN_ID_UNKNOWN;

    list<RouterId2DomainId*>::iterator iter;
    RouterId2DomainId* r2d_conv;
    for (iter = RDB.routerToDomainDirectory.begin(); iter != RDB.routerToDomainDirectory.end(); iter++)
    {
        r2d_conv = *iter;
        if (router->advRtId == r2d_conv->router_id)
        {
            router->domainId = r2d_conv->domain_id;
            return router->domainId;
        }
    }
    
    router->domainId = DOMAIN_ID_UNKNOWN;
    return router->domainId;
}

PCENLink * PCENNode::GetOutLinkByIp(in_addr * ip)
{
    list<PCENLink*>::iterator iter;
    PCENLink * pcen_link;

    for (iter = out_links.begin(); iter != out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->link == NULL)
            continue;
        if (pcen_link->link->id == ip->s_addr || pcen_link->link->rmtIfAddr== ip->s_addr)
            return pcen_link;
    }
    return NULL;
}

PCENLink * PCENNode::GetInLinkByIp(in_addr * ip)
{
    list<PCENLink*>::iterator iter;
    PCENLink * pcen_link;

    for (iter = out_links.begin(); iter != out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->link == NULL)
            continue;
        if (pcen_link->link->advRtId == ip->s_addr || pcen_link->link->lclIfAddr== ip->s_addr)
            return pcen_link;
    }
    return NULL;    
}

u_int32_t PCENLink::HeadDomainId ()
{
    assert(lcl_end);
    return lcl_end->DomainId();
}

u_int32_t PCENLink::TailDomainId ()
{
    assert(rmt_end);
    return rmt_end->DomainId();
}

void PCENNode::DisplayInfo()
{
    if (!router)
    {
        cout << "NODE[empty] "<<endl;
        return;
    }

    in_addr ip;
    ip.s_addr = router->id;
    char addr[20];
    inet_ntop(AF_INET, &ip, addr, 20);
    cout <<"NODE[" << addr << "]" <<endl;
}

void PCENNode::ShowPath()
{
    if (path.size() == 0)
    {
        cout<<"Unreachable... "<<endl;
        return;
    }

    cout <<"Via Path: "<<endl;
    
    in_addr ip1, ip2;
    char addr1[20], addr2[20];
    Link* link;    
    list<PCENLink*>::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        link = (*it)->link;

        ip1.s_addr = link->lclIfAddr;
        ip2.s_addr = link->rmtIfAddr;

        inet_ntop(AF_INET, &ip1, addr1, 20);
        inet_ntop(AF_INET, &ip2, addr2, 20);
        cout<<"["<<addr1 <<"-" <<addr2 <<"] ";
    }
    
    cout << " at cost [" <<minCost<< "]" <<endl;
}

void  PCENLink::DisplayInfo()
{
    if (!link)
    {
        cout <<"LINK[void] "<<endl;
        return;
    }

    in_addr ip, ip1, ip2;
    ip.s_addr = link->advRtId;
    ip1.s_addr = link->lclIfAddr;
    ip2.s_addr = link->rmtIfAddr;
    char addr[20], addr1[20], addr2[20];
    inet_ntop(AF_INET, &ip, addr, 20);
    inet_ntop(AF_INET, &ip1, addr1, 20);
    inet_ntop(AF_INET, &ip2, addr2, 20);
    cout << "LINK@" << addr << "[" << addr1 << "-" <<addr2 << "] ";
}

#ifdef HAVE_EXT_ATTR
void*  PCENLink::AttributeById (u_int16_t tlv_type)
{
    int index = ATTR_INDEX(link->type, tlv_type);
    if (index < 0 || index > link->attrTable.size() - 1)
        return NULL;

    return link->attrTable[index].p;
}

void*  PCENLink::AttributeByTag (const char * tag)
{
    int index = ATTR_INDEX_BY_TAG(tag);
    if (index < 0 || index > link->attrTable.size() - 1)
        return NULL;

    return link->attrTable[index].p;
}
#endif


PCENNode::PCENNode(int id) 
{
    ref_num=id;
    Init();
}

void PCENNode::FilterNode() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=1;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=1;
        itLink++;
    }
    this->nflg.nfg.filteroff=1;
}

void PCENNode::RestoreNodeFilter() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=0;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=0;
        itLink++;
    }
    this->nflg.nfg.filteroff=0;
}

void PCENNode::MaskNode() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=1;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=1;
        itLink++;
    }
    this->nflg.nfg.maskoff=1;
}

void PCENNode::RestoreNodeMask() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=0;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
    	(*itLink)->lflg.lfg.maskoff=0;
    	itLink++;
    }
    this->nflg.nfg.maskoff=0;
}

bool PCENLink::IsAvailableForTspec(TSpec& tspec)
{
    TSpec tspec_link;
    ISCD * iscd;
    list<ISCD*>::iterator it;
    for (it = link->iscds.begin(); it != link->iscds.end(); it++)
    {
        iscd = *it;
        assert(iscd);
        tspec_link.Update(iscd->swtype, iscd->encoding, iscd->max_lsp_bw[7]);//?

        if (tspec == tspec_link)
            return true;

        //@@@@ A temporary matching (available) condition for LSC and FSC links
        //@@@@ A testing code for now. We need to further consider meaning of bandwidth parameters in LSC and FSC.
        if ( (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC)
            && tspec_link.SWtype == tspec.SWtype && tspec_link.ENCtype == tspec.ENCtype )
                return true;

        //@@@@ Movaz special handling
        if ( tspec.SWtype == MOVAZ_LSC && tspec_link.SWtype == MOVAZ_LSC 
            ||tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && tspec_link.SWtype == MOVAZ_LSC 
            ||tspec_link.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && tspec.SWtype == MOVAZ_LSC )
                return true;

        // $$$$ No bandwidth constraints for now. OK for MOVAZ special handling. 
        // $$$$ Wavelength (lambda) constraints will be taken care after this function call.
        //@@@@ End

        if (tspec <= tspec_link)
        {
            if (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM)
            {
               if (float_equal(iscd->min_lsp_bw, 0) ||float_evenly_dividable(tspec.Bandwidth, iscd->min_lsp_bw))
                    return true;
            }
            else if (tspec.SWtype >= LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 && 
                    tspec.SWtype <=  LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 ||
                    tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
                return true;
            else //tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || LINK_IFSWCAP_SUBTLV_SWCAP_FSC
                continue;
        }
    }

    return false;
}

bool PCENLink::CanBeEgressLink(TSpec& tspec)
{
    TSpec tspec_link;
    ISCD * iscd;
    list<ISCD*>::iterator it;

    //@@@@ link ==> reverse_link
    if (reverse_link == NULL || reverse_link->link == NULL)
        return false;
    for (it = reverse_link->link->iscds.begin(); it != reverse_link->link->iscds.end(); it++)
    {
        iscd = *it;
        assert(iscd);
        tspec_link.Update(iscd->swtype, iscd->encoding, iscd->max_lsp_bw[7]);//?
        if (tspec_link.ENCtype != tspec.ENCtype)
            continue;

        if (tspec == tspec_link)
            return true;

        if (tspec <= tspec_link)
        {
            if (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM )
            {
                if (float_equal(iscd->min_lsp_bw, 0) ||float_evenly_dividable(tspec.Bandwidth, iscd->min_lsp_bw))
                    return true;
            }
            else if (tspec.SWtype >= LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 && 
                    tspec.SWtype <=  LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 ||
                    tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
                return true;
            else //tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || LINK_IFSWCAP_SUBTLV_SWCAP_FSC
                continue;
        }
    }

    return false;
}

//$$$$ only Movaz specific handling for now
void PCENLink::ProceedByUpdatingWaves(ConstraintTagSet &head_waveset, ConstraintTagSet &next_waveset)
{
    next_waveset.TagSet().clear();
    bool any_wave_ok = (head_waveset.TagSet().size() > 0 && head_waveset.TagSet().front() == ANY_WAVE);

    //$$$$ Movaz specific TE info
    // Retieve available wavelength information based on TE Wavelength Grid (present in LSAs originated from ROADMs)
    MovazWaveGrid* wavegrid = (MovazWaveGrid*)(this->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID"));
    if (wavegrid != NULL)
    {
        ntoh_wavegrid(*wavegrid);
        int l;
        for (l = 0; l < sizeof(wavegrid->out_channels); l++)
        {
            if ((wavegrid->out_channels[l] & 0xf0) == 0x70) // f == channel unavailable; 0~7 == priority level
                next_waveset.AddTag(wavegrid->base+l*200);
            if ((wavegrid->out_channels[l] & 0x0f) == 0x07)
                next_waveset.AddTag(wavegrid->base+l*200+100);
        }
    }

    //$$$$ Movaz specific TE info
    // Retieve available wavelength information based on TE Lambda list (present in LSAs originated from REs)
    list<void*> *p_list = (list<void*>*)(this->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA"));
    if (p_list != NULL)
    {
        list<void*>::iterator it;
        MovazTeLambda tel;
        for (it = p_list->begin(); it!= p_list->end(); it++)
        {
            tel = *(MovazTeLambda*)(*it);
            ntoh_telambda(tel);
            if (tel.priority == 0x07)
                next_waveset.AddTag(tel.channel_id);
        }
    }

    if (!any_wave_ok)
        next_waveset.Intersect(head_waveset);
}

void PCENLink::ProceedByUpdatingVtags(ConstraintTagSet &head_vtagset, ConstraintTagSet &next_vtagset)
{
    next_vtagset.TagSet().clear();
    list<ISCD*>::iterator it;
    ISCD * iscd;
    bool any_vlan_ok = (head_vtagset.TagSet().size() > 0 && head_vtagset.TagSet().front() == ANY_VTAG);
    bool non_vlan_link = true;

    for (it = link->iscds.begin(); it != link->iscds.end(); it++)
    {
        iscd = *it;
        if (!iscd)
            continue;
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC
            || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC)
        {
            if (iscd->vlan_info.version == IFSWCAP_SPECIFIC_VLAN_VERSION)
            {
                non_vlan_link = false;
                next_vtagset.AddTags(iscd->vlan_info.bitmask, MAX_VLAN_NUM);
            }
        }
    }

    if (non_vlan_link)
        next_vtagset = head_vtagset;
    else if (!any_vlan_ok)
        next_vtagset.Intersect(head_vtagset);
}

PCENNode* PCENLink::search_PCENNode(vector<PCENNode*>& routers, int NodeId) 
{
    for (unsigned int i=0; i<routers.size(); i++) 
    {
        if (routers[i]->ref_num==NodeId)
            return routers[i];
    }
    return NULL;
}


PCENLink::PCENLink(int id)
{
	linkID=id;
	lcl_end=NULL;
	rmt_end=NULL;
	link = NULL;
}

PCENLink::PCENLink(int id, int localNodeId, int remoteNodeId, vector<PCENNode*>& routers) 
{
    linkID=id;
    lcl_end=search_PCENNode(routers, localNodeId);
    lcl_end->out_links.push_back(this);
    rmt_end=search_PCENNode(routers, remoteNodeId);
    rmt_end->in_links.push_back(this);
    Init();
}

void PCENLink::FilterLink() 
{
    this->lflg.lfg.filteroff=1;
}

void PCENLink::RestoreLinkFilter() 
{
    this->lflg.lfg.filteroff=0;
}

void PCENLink::MaskLink() 
{
    this->lflg.lfg.maskoff=1;
}
void PCENLink::RestoreLinkMask() 
{
    this->lflg.lfg.maskoff=0;
}

double PCENLink::PCENmetric ()
{
    if (!link)
        return PCEN_INFINITE_COST;

    return (double)link->metric;
}

void PCENLink::SetPCENmetric (u_int32_t x)
{
    if (!link)
        return;

    
    link->metric = x;
}

void PCENLink::SetPCENmetric (double d)
{
    if (!link)
        return;

    link->metric = (u_int32_t)d;
}

void PCEN::AllocateGraph(int size)
{
    if (gGraph && size > gSize)
    {
        FreeGraph();        
    }

    if (!gGraph)
    {
        if (size > gSize)
            gSize = size;
        gGraph = new float[gSize*gSize];
    }

    for (int i = 0; i < gSize; i++)
        for (int j = 0; j < gSize; j++)
            gGraph[i*gSize + j] = 1000000;
}

void PCEN::FreeGraph()
{
    if (!gGraph)
        return;

    delete []gGraph;

    gGraph = NULL;    
    gSize = 0;
}

void PCEN::ClearFilters()
{ 
    while (filters.size() > 0)
    {
        delete (Filter*)filters.front();
        filters.pop_front();
    }
}

PCENLink* PCEN::search_PCENLink(int lcl_end,int rmt_end)
{
	for(unsigned int i=0; i<links.size(); i++)
	{
		if(links[i]->lcl_end->ref_num == lcl_end && links[i]->rmt_end->ref_num == rmt_end)
			return links[i];
	}
	return NULL;
}

// Apply initial constraints, e.g. Routing Mode (loose/strict-only or partial..) and Domain Restriction...
int PCEN::VerifyRequest()
{
    //Check source and destination
    Prefix index1, index2;
    index1.length = index2.length = 64;
    prefix_word(index1, 0) = source.s_addr;
    prefix_word(index1, 1) = source.s_addr;
    prefix_word(index2, 0) = destination.s_addr;
    prefix_word(index2, 1) = destination.s_addr;

    //options  = options & (~LSP_OPT_STRICT_PREFERRED);
    //options  = options | LSP_OPT_LOOSE_ONLY;
    
    if (!(options & LSP_OPT_STRICT)) // && !(options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_GLO_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_GLO_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else if ((options & LSP_OPT_STRICT) && (options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index1) == NULL && RDB.LookupByPrefix(RTYPE_GLO_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index2) == NULL && RDB.LookupByPrefix(RTYPE_GLO_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else
    {
        LOGF("Incorrection Option Setting [%x]\n", options);
        return ERR_PCEN_INVALID_REQ;
    }

    // We want all routers
    Prefix *pPrefix = new (Prefix);
    memset(pPrefix, 0 , sizeof(Prefix));
    prefix_list.push_back(pPrefix);

    // Add the switching capability filter
    if (!(options & LSP_OPT_MRN))
    {
        SwitchingTypeFilter * swTypeFilter = new SwitchingTypeFilter(switching_type_egress);
        filters.push_back(swTypeFilter);
    }
    
    return 0;
}

void PCEN::BuildNodeVector(ResourceType type, Prefix *prefix)
{
    RadixTree<Resource>* tree = RDB.Tree(type);
    RadixNode<Resource>* node = tree->LookupNode(prefix);
    
    if (!node)
        return;

    Prefix index = node->Index();

    PCENNode * pcen_node;
    do 
    {
        if (node->Data())
        {
            int i;
            for (i = 0; i < routers.size(); i++)
            {
                if (routers[i]->router &&  routers[i]->router->id == (node->Data())->id)
                    break;
            }
            if (i < routers.size())
                continue;
            pcen_node = new PCENNode((RouterId*)node->Data());
            routers.push_back(pcen_node);
        }
    } while (node = tree->LookupNextMatchedNode(node, &index));
}

void PCEN::BuildLinkVector(ResourceType type)
{
    RadixTree<Resource>* tree = RDB.Tree(type);
    RadixNode<Resource>* node;
    PCENNode* pcen_node_lcl, *pcen_node_rmt;
    PCENLink* pcen_link;
    Prefix index;
    int ref_num = 0;

    vector<PCENNode*>::iterator iter_outer, iter_inner;
    for (iter_outer = routers.begin(); iter_outer != routers.end(); iter_outer++)
    {
        pcen_node_lcl = *iter_outer;
        index = pcen_node_lcl->router->Index();
        index.length = 32;
        node = tree->LookupMatchedNode(&index);
        if (!node)
            continue;
        do 
        {
            if (node->Data())
            {
                pcen_link = NULL; 
                // removing stub links
                for (iter_inner = routers.begin(); iter_inner != routers.end(); iter_inner++)
                {
                    pcen_node_rmt = *iter_inner;
                    if (pcen_node_rmt->router->id == ((Link*)node->Data())->id)
                    {
                        pcen_link = new PCENLink((Link *)node->Data());
                        break;
                    }
                }
                if (pcen_link)
                {
                    assert (pcen_node_lcl);
                    assert (pcen_node_rmt);
                    pcen_link->lcl_end = pcen_node_lcl;
                    pcen_link->rmt_end = pcen_node_rmt;

                    pcen_node_lcl->out_links.push_back(pcen_link);
                    pcen_node_rmt->in_links.push_back(pcen_link);

                    pcen_link->linkID = ref_num++;
                    links.push_back(pcen_link);
                 }
            }
        } while (node = tree->LookupNextMatchedNode(node, &index));
    }
}


////////////////Handling Overlapping Links//////////////////

//Add a transit node into the routers vector and return its pointer.
//ref_num has been assigned a valid value. router == NULL, which indicates
//the transit status.
PCENNode* PCEN::NewTransitNode (vector<PCENNode*>& routers)
{
    PCENNode * transitNode = new PCENNode;
    assert (routers.size() > 0);
    PCENNode *tailNode = routers[routers.size() -1];
    assert(tailNode);
    transitNode->ref_num = tailNode->ref_num + 1;
    routers.push_back(transitNode);
    return transitNode;
}

//Add a transit link into the links vector and return its pointer.
// link == NULL, which indicates the transit status.
PCENLink* PCEN::NewTransitLink (vector<PCENLink*>& links)
{
    PCENLink * transitLink = new PCENLink;
    PCENLink *tailLink = links[links.size() -1];
    assert(tailLink);
    transitLink->linkID = tailLink->linkID + 1;
    links.push_back(transitLink);
    return transitLink;
}

void PCEN::SplitLink (vector<PCENNode*>& routers, vector<PCENLink*>& links, PCENLink * teLink)
{
    assert(teLink); 
    assert(teLink->lcl_end);
    assert(teLink->rmt_end);

    // disconnect the original link from the remote node
    PCENLink* inLink;
    list<PCENLink*>::iterator iter;
    for (iter = teLink->rmt_end->in_links.begin(); iter != teLink->rmt_end->in_links.end(); iter++)
    {
        inLink = *iter;
        if (inLink->link == teLink->link)
        {
            teLink->rmt_end->out_links.erase(iter);
            break;
        }
     }
    assert (iter != teLink->rmt_end->in_links.end());

    PCENNode* transitNode = NewTransitNode(routers);
    PCENLink* transitLink = NewTransitLink(links);
    //let the transit node split the original link
    transitNode->in_links.push_back(teLink);
    transitNode->out_links.push_back(transitLink);
    //let the transit link bridge the transit node to the remote node
    transitLink->lcl_end = transitNode;
    transitLink->rmt_end = teLink->rmt_end;
    //let the orginal link point to the transit node
    teLink->rmt_end = transitNode;
}

int PCEN::HandleOverlappingLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links)
{
    int count_overlaps = 0;
    vector<PCENNode*> foundRouters;
    
    PCENNode* teNode;
    for (int i = 0; i < routers.size(); i++)
    {
        teNode = routers[i];

        foundRouters.clear();
        PCENLink* teLink;
        list<PCENLink*>::iterator iter;
        for (iter = teNode->out_links.begin(); iter != teNode->out_links.end(); iter++)
        {
            teLink = *iter;
            bool found = false;
            //check if there has been another link with the same remote end (node)
            for (int j = 0; j < foundRouters.size(); j++)
            {
                // if found such link has existed, split the current link
                if (foundRouters[j]->router == teLink->rmt_end->router)
                {
                    SplitLink(routers, links, teLink);
                    found = true;
                    count_overlaps++;
                    break;
                }
            }

            if (!found)
                foundRouters.push_back(teLink->rmt_end);
        }
    }

    return count_overlaps;
}

////////////////////////////////////////////////////////

void PCEN::BuildGraph(vector<PCENNode*>& routers, vector<PCENLink*>& links)
{
    originalGraphSize = routers.size();
    int numOverlapingLinks = HandleOverlappingLinks(routers, links);
    int numRouters = routers.size();
    int numLinks = links.size();

    //LOGF("Handled/splited a total of %d overlapping links\n", numOverlapingLinks);

    AllocateGraph(numRouters);

    int  i;
    PCENLink *pcen_link;
    for (i =0 ; i < numRouters; i++)
    {
        routers[i]->ref_num = i; 
    }
    float verySmallCost =  0.000001;
    for (i = 0; i < numLinks; i++)
    {
        pcen_link = links[i];
        gGraph[pcen_link->lcl_end->ref_num * numRouters + pcen_link->rmt_end->ref_num] = 
           ( pcen_link->link == NULL ? verySmallCost : ( pcen_link->link->metric == 0 ? 1 : pcen_link->link->metric ) );
    }
}


int PCEN::BuildTopology()
{
    // Buidling the initial router vector
    list<Prefix*>::iterator iter;
    for (iter = prefix_list.begin(); iter != prefix_list.end(); iter++)
    {
        if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
            BuildNodeVector(RTYPE_LOC_RTID, *iter);
        else
            BuildNodeVector(RTYPE_GLO_RTID, *iter);
    }
    if (routers.size() < 2 && (options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        return ERR_PCEN_DATA_CORRUPT;

    // Buidling the initial link vector
    if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        BuildLinkVector(RTYPE_LOC_PHY_LNK);
    else
        BuildLinkVector(RTYPE_GLO_ABS_LNK);
    if (links.size() < 1 && (options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        return ERR_PCEN_DATA_CORRUPT;

    if (((options & LSP_OPT_STRICT) && (options & LSP_OPT_PREFERRED)))
    {
        for (iter = prefix_list.begin(); iter != prefix_list.end(); iter++)
            BuildNodeVector(RTYPE_LOC_RTID, *iter);
        BuildLinkVector(RTYPE_LOC_PHY_LNK);        
    }

    // Applying filters in list<filters>   
    //      Modifying the router vector   
    //      Modifying the link vector
    list<Filter*>::iterator filter_iter;
    Filter* aFilter;
    for (filter_iter = filters.begin(); filter_iter != filters.end(); filter_iter++)
    {
        aFilter = *filter_iter;
        assert(aFilter);
        aFilter->Process(routers, links);
    }

    // Build graph matrix
    if (!(options & LSP_OPT_MRN))
    {
        BuildGraph(routers, links);
    }
    return 0;
}
    
void PCEN::DJhop(float *graphhop, int node_number, int source, int* path, int* hop, float* cost)
{
	int *node_inset = new int[node_number]; 	
	int i,j,k,m;
	float infcost = 1000000;
	float shortcost;

	for (i=0; i<node_number; i++)
		for (j=0; j<node_number; j++)
              {      
			path[i* node_number+j]=-1;
                     if (graphhop[i* node_number+j] == -1)
                        graphhop[i* node_number+j] = 1000000;
              }
	
	path[source*node_number+0]=source;
	

	for (i=0; i<node_number; i++) {
		node_inset[i]=0;
		hop[i]=node_number+1;
		cost[i]=infcost;
		if (graphhop[source*node_number+i]!=infcost) {
			hop[i]=2;
			cost[i]=graphhop[source*node_number+i];
		} 
		if (hop[i]<node_number+1) {
			path[i*node_number+0]=source;
			path[i*node_number+1]=i;
		} else {
			path[i*node_number+0]=-1;
		}
	}
	
	node_inset[source]=1;
	
	for (i=0; i<node_number; i++) {
		shortcost=infcost;
		k=-1;
		for (j=0; j<node_number; j++) {
			if ((!node_inset[j]) && (cost[j]<shortcost)) {
				shortcost=cost[j];
				k=j;
			}
		}
		if (k!=-1) {
			node_inset[k]=1;
			for (j=0; j<node_number; j++) {
				if ((!node_inset[j]) && (graphhop[k*node_number+j]!=infcost)) {
					if (cost[j]>cost[k]+graphhop[k*node_number+j]) {
						cost[j]=cost[k]+graphhop[k*node_number+j];
						for (m=0; m<node_number; m++)
							path[j*node_number+m]=path[k*node_number+m];
						hop[j]=hop[k]+1;
						path[j*node_number+hop[j]-1]=j;
					}
						
				}
			}
		}
	}

    delete []node_inset;
}

int PCEN::PerformComputation()
{
    // SPF computation
    //      Currently we do not have the time scheduling function.
    int node_number = routers.size();
    int src_index = -1;
    int ret = 0;
    int i, j, k;

    for (i = 0; i < node_number; i++)
    {
        if (routers[i]->router == NULL)
            continue;
        if (routers[i]->router->id == source.s_addr)
        {
            src_index = routers[i]->ref_num;
            break;
        }
    }
    if (src_index == -1) 
        return ERR_PCEN_NO_SRC;
    
    int dest_index = -1;
    for (int i = 0; i < node_number; i++)
    {
        if (routers[i]->router == NULL)
            continue;
        if (routers[i]->router->id == destination.s_addr)
        {
            dest_index = routers[i]->ref_num;
            break;
        }
    }
    if (dest_index == -1) 
        return ERR_PCEN_NO_DEST;

    
    int *hop = new int[node_number];
    float *cost = new float[node_number];
    int *path = new int[node_number*node_number];

    DJhop(gGraph, node_number, src_index, path, hop, cost);

    if (path[dest_index* node_number + 0]==-1) 
    {
        ret = ERR_PCEN_NO_ROUTE;
        goto _out;
    }

    assert(ero.size() == 0);

    for (i=0; i<hop[dest_index]-1; )  {
        j=path[dest_index * node_number + i];
        i++;
        k=path[dest_index * node_number + i];
        while (!routers[k]->router)
        {
            i++;
            k=path[dest_index * node_number + i];
        }
        ero_subobj local_if_ip, remote_if_ip;
        memset(&local_if_ip, 0, sizeof(ero_subobj));
        memset(&remote_if_ip, 0, sizeof(ero_subobj));
        if (ret = GetLinkEndsByIndex(routers, links, j, k, &local_if_ip.addr, &remote_if_ip.addr))
            goto _out;

        Prefix prefix; 
        prefix.length = 128;
        prefix_word(prefix, 0) = routers[j]->router->id;
        prefix_word(prefix, 1) = routers[k]->router->id;
        prefix_word(prefix, 2) = local_if_ip.addr.s_addr;
        prefix_word(prefix, 3) = remote_if_ip.addr.s_addr;
        if (RDB.LookupByPrefix(RTYPE_LOC_PHY_LNK, &prefix) != NULL)
            local_if_ip.hop_type = remote_if_ip.hop_type = ERO_TYPE_STRICT_HOP;
        else 
            local_if_ip.hop_type = remote_if_ip.hop_type = ERO_TYPE_LOOSE_HOP;
        
        local_if_ip.prefix_len = 32;
        if (local_if_ip.addr.s_addr != 0)
            ero.push_back(local_if_ip);
        remote_if_ip.prefix_len = 32;
        if (remote_if_ip.addr.s_addr != 0)
            ero.push_back(remote_if_ip);
    }    

_out:
    delete []hop;
    delete []cost;
    delete []path;
    return ret;
}

void PCEN::Run()
{
    int ret;
    Event::Run();

    if (is_e2e_tagged_vlan)
    {
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        LOG("PCEN::Run() : should have called PCEN_MRN::Run() to handle E2E Tagged VLAN request!" << endl);
        return;
    }

    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN::PerformComputation() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    ReplyERO();
}

void PCEN::ReplyErrorCode (u_int32_t code)
{
    assert (api_writer);

    te_tlv_header * tlv = (te_tlv_header *)(new char[TLV_HDR_SIZE + 4]);
    tlv->length = htons(4);
    tlv->type = htons(TLV_TYPE_NARB_ERROR_CODE);
    *(u_int32_t*)((char*)tlv + TLV_HDR_SIZE) = htonl(code);
    api_msg* msg = api_msg_new(MSG_LSP, ACT_ERROR, tlv, lspq_id, seqnum, TLV_HDR_SIZE + 4, vtag);
    delete [] (char*)tlv;
    api_writer->PostMessage(msg);
    return;
}

void PCEN::ReplyERO ()
{
    assert (api_writer);
    int len = sizeof(ero_subobj)*ero.size();
    assert (len);
    te_tlv_header * tlv = (te_tlv_header *)(new char[TLV_HDR_SIZE + len]);
    tlv->length = htons(len);
    tlv->type = htons(TLV_TYPE_NARB_ERO);
    ero_subobj* ero_hop = (ero_subobj*)((char*)tlv + TLV_HDR_SIZE);
    int i;
    list<ero_subobj>::iterator iter; 
    for (i = 0, iter = ero.begin(); iter != ero.end(); iter++, i++)
    {
        *(ero_hop+i) = (*iter);
    }
    api_msg* msg = api_msg_new(MSG_LSP, ACT_ACKDATA, tlv, lspq_id, seqnum, TLV_HDR_SIZE + len, vtag);
    delete [] (char*)tlv;
    api_writer->PostMessage(msg);    
}
    
PCEN::~PCEN()
{
    //clean Router list
    vector<PCENNode*>::iterator node_iter;
    for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++)
        if (*node_iter)
            delete *node_iter;

    //clean Link list
    vector<PCENLink*>::iterator link_iter;
    for (link_iter = links.begin();  link_iter != links.end(); link_iter++)
        if (*link_iter)
            delete *link_iter;

    //clean Filter list
    list<Filter*>::iterator filter_iter;
    for (filter_iter = filters.begin();  filter_iter != filters.end(); filter_iter++)
        if (*filter_iter)
            delete *filter_iter;

    //clean prefix list
    list<Prefix*>::iterator prefix_iter;
    for (prefix_iter = prefix_list.begin();  prefix_iter != prefix_list.end(); prefix_iter++)
        if (*prefix_iter)
            delete *prefix_iter;
    
    if (vtag_mask)
        delete vtag_mask;

    FreeGraph();
}

int PCEN::GetLinkEndsByIndex(vector<PCENNode*>& routers, vector<PCENLink*>& links, int j, int k, in_addr * head_ip, in_addr * tail_ip)
{
    assert(head_ip);
    assert(tail_ip);
    
    PCENNode *head = routers[j];
    PCENNode *tail = routers[k];
    PCENLink * pcen_link;
    list<PCENLink*>::iterator iter;
    for (iter = head->out_links.begin(); iter != head->out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->rmt_end->ref_num != tail->ref_num)
            continue;
        if (pcen_link->link == NULL) //transit link
        {
            assert(tail->router);
            head_ip->s_addr = 0;
            list<PCENLink*>::iterator iter_inner = pcen_link->lcl_end->in_links.begin();
            pcen_link = *iter_inner;
            assert(pcen_link->link);
            tail_ip->s_addr = pcen_link->link->rmtIfAddr;
        }
        else if (tail->router == NULL) // the link before transit node/link 
        {
            assert(pcen_link->link);
            head_ip->s_addr = pcen_link->link->lclIfAddr;    
            tail_ip->s_addr = 0;
        }
        else // regular link
        {
            assert(pcen_link->link && tail->router);
            assert(pcen_link->link->advRtId == routers[j]->router->id && pcen_link->link->id == routers[k]->router->id);
            head_ip->s_addr = pcen_link->link->lclIfAddr;    
            tail_ip->s_addr = pcen_link->link->rmtIfAddr;
        }
        return 0;
    }
    return ERR_PCEN_DATA_CORRUPT;
}

PCENLink* PCEN::GetLinkByIp(vector<PCENLink*>& links, in_addr* head_ip, in_addr* tail_ip)
{
    assert(head_ip);
    assert(tail_ip);
    PCENLink* pcen_link;
    for (int i = 0; i < links.size(); i++)
    {
        pcen_link = links[i];
        if (!pcen_link->lcl_end || !pcen_link->rmt_end)
            continue;
        if (pcen_link->lcl_end->router == NULL || pcen_link->rmt_end->router == NULL)
            continue;
        if (pcen_link->lcl_end->router->id == head_ip->s_addr && pcen_link->rmt_end->router->id == tail_ip->s_addr)
            return pcen_link;
    }

    return NULL;
}

PCENNode* PCEN::GetNodeByIp(vector<PCENNode*>& routers, in_addr* ip)
{
    assert(ip);
    PCENNode* pcen_node;
    for (int i = 0; i < routers.size(); i++)
    {
        pcen_node = routers[i];
        if (pcen_node->router == NULL)
            continue;
        if (pcen_node->router->id == ip->s_addr)
        {
            return pcen_node;
        }
    }    
    return NULL;
}

u_int32_t PCEN::GetRouterDomainId(vector<PCENNode*>& routers, in_addr* ip)
{
    assert(ip);
    PCENNode* pcen_node = GetNodeByIp(routers, ip);
    if (!pcen_node)
        return DOMAIN_MASK_GLOBAL;

    return pcen_node->DomainId();
}

int PCEN::GetDomainBorderRouters(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, list<PCENNode*>&borderRouters)
{
    assert (borderRouters.size() == 0);

    PCENNode * pcen_node;
    for (int i = 0; i < routers.size(); i++)
    {
        pcen_node = routers[i];
        if (pcen_node->DomainId() == domainId)
        {
            PCENLink * pcen_link;
            list<PCENLink*>::iterator iter;
            for (iter = pcen_node->out_links.begin(); iter != pcen_node->out_links.end(); iter++)
            {
                pcen_link = *iter;
                assert(pcen_link->HeadDomainId() == domainId);
                if (pcen_link->TailDomainId() != domainId)
                {
                    borderRouters.push_back(pcen_node);
                    break;
                }
            }
        }
    }

    return borderRouters.size();
}

int PCEN::GetInterDomainLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t headDomainId, u_int32_t tailDomainId, list<PCENLink*>&interDomainLinks)
{
    list<PCENNode*> headBorderNodes;

    if (GetDomainBorderRouters(routers, links, headDomainId, headBorderNodes) == 0)
        return 0;

    PCENNode * pcen_node;
    list<PCENNode*>::iterator iter_outer;
    for (iter_outer = headBorderNodes.begin(); iter_outer != headBorderNodes.end(); iter_outer++)
    {
        pcen_node = *iter_outer;
        PCENLink * pcen_link;
        list<PCENLink*>::iterator iter_inner;
        for (iter_inner = pcen_node->out_links.begin(); iter_inner != pcen_node->out_links.end(); iter_inner++)
        {
            pcen_link = *iter_inner;
            if (pcen_link->TailDomainId() != tailDomainId)
            {
                interDomainLinks.push_back(pcen_link);
                break;
            }
        }
    }

    return interDomainLinks.size();
}

bool PCEN::IsDomainBorderRouter(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, u_int32_t routerId)
{
    list<PCENNode*> borderRouters;
    list<PCENNode*>::iterator iter;
    PCENNode * pcen_node;
    if (GetDomainBorderRouters(routers, links, domainId, borderRouters) > 0)
    {
        for (iter = borderRouters.begin(); iter != borderRouters.end(); iter++)
        {
            pcen_node = *iter;
            assert(pcen_node);
            if (pcen_node->router && pcen_node->router->id == routerId)
                return true;
        }
    }
    return false;
}

bool PCEN::IsLinkInDomain(PCENLink* pcenLink, u_int32_t domainId)
{
    assert(pcenLink);
    if (pcenLink->lcl_end == NULL || pcenLink->rmt_end == NULL)
        return false;

    if (pcenLink->lcl_end->DomainId() == domainId || pcenLink->rmt_end->DomainId() == domainId)
        return true;

    return false;
}

/*
int PCEN::PruneDomain(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId)
{
    vector<PCENLink*>::iterator link_iter;
    PCENLink * pcen_link;
    vector<PCENNode*>::iterator node_iter;
    PCENNode * pcen_node;

    for (link_iter = links.begin(); link_iter != links.end(); link_iter++)
    {
        pcen_link = *link_iter;
        if (!pcen_link)
            continue;
        if (IsLinkInDomain(pcen_link, domainId))
        {
            pcen_link->lcl_end->ref_num = -2;
            pcen_link->rmt_end->ref_num = -2;

            links.erase(link_iter);
            if (links.size() == 0)
                break;
            link_iter--;
        }
    }

    for (node_iter = routers.begin(); node_iter != routers.end(); node_iter++)
    {
        pcen_node = *node_iter;
        if (!pcen_node)
            continue;
        if (pcen_node->DomainId() == domainId || pcen_node->ref_num == -2)
        {
            routers.erase(node_iter);
            if (routers.size() == 0)
                break;
            node_iter--;
        }
    }
    return 0;
}
*/
