/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */

#include "pcen_mcbase.hh"
#include "rce_movaz_types.hh"

vector<PathM*> PCEN_MCBase::allPaths;

#define CROSS_LAYER_GROOMING 1

//#define SHOW_MCPATH 1

///////////////////  PathM /////////////////////////

PathM& PathM::operator=(const PathM& p) {
    this->source.s_addr = p.source.s_addr;
    this->destination.s_addr = p.destination.s_addr;
    this->ucid = p.ucid;
    this->seqnum = p.seqnum;
    this->start_time = p.start_time;
    this->end_time = p.end_time;
    this->path.assign(p.path.begin(), p.path.end());
    this->reverse_path.assign(p.reverse_path.begin(), p.reverse_path.end());
    this->cost = p.cost;
    this->bandwidth = p.bandwidth;
    this->vlan_tag = p.vlan_tag;
    this->wavelength = p.wavelength;
    this->pflg = p.pflg;
    return *this;
}

PathM& PathM::operator=(const PathT& p)
{
        this->source.s_addr = p.source.s_addr;
        this->destination.s_addr = p.destination.s_addr;
        this->ucid = p.ucid;
        this->seqnum = p.seqnum;
        this->cost = p.cost;
        this->bandwidth = p.bandwidth;
        this->vlan_tag = p.vlan_tag;
        this->wavelength = p.wavelength;
        this->pflg.flag = p.pflg.flag;
        this->path.clear();
        this->reverse_path.clear();
        list<PCENLink*>::const_iterator it = p.path.begin();
        for (; it != p.path.end(); it++)
            this->path.push_back((*it)->link);
        list<PCENLink*>::const_reverse_iterator rit = p.path.rbegin();
        for (; rit != p.path.rend(); rit++)
            if ((*rit)->reverse_link)
                this->reverse_path.push_back((*rit)->reverse_link->link);
        return *this;
}

void PathM::QueryHold()
{
    //Make delta for each link
    list<Link*>::iterator itl = path.begin();
    for (; itl != path.end(); itl++)
    {   
        LinkStateDelta* delta = new LinkStateDelta;
        memset(delta, 0, sizeof(LinkStateDelta));
        delta->owner_ucid = ucid;
        delta->owner_seqnum = seqnum;
        delta->bandwidth = bandwidth;
        delta->flags |= DELTA_QUERIED;
        if (wavelength != 0)
        {
            delta->flags |= DELTA_WAVELENGTH;
            delta->wavelength = wavelength;
        }
        bool doHoldOnLink = true;
        if (start_time.tv_sec != 0)
        {
            delta->flags |= DELTA_SCHEDULING;
            delta->start_time = start_time;
            delta->end_time = end_time;
            doHoldOnLink = false;
        }
        //insert delta
        (*itl)->insertDelta(delta, SystemConfig::delta_expire_query, 0, doHoldOnLink);
    }
    //do reverse links
    itl = reverse_path.begin();
    for (; itl != reverse_path.end(); itl++)
    {   
        LinkStateDelta* delta = new LinkStateDelta;
        memset(delta, 0, sizeof(LinkStateDelta));
        delta->owner_ucid = ucid;
        delta->owner_seqnum = seqnum;
        delta->bandwidth = bandwidth;
        delta->flags |= DELTA_QUERIED;
        if (wavelength != 0)
        {
            delta->flags |= DELTA_WAVELENGTH;
            delta->wavelength = wavelength;
        }
        bool doHoldOnLink = true;
        if (start_time.tv_sec != 0)
        {
            delta->flags |= DELTA_SCHEDULING;
            delta->start_time = start_time;
            delta->end_time = end_time;
            doHoldOnLink = false;
        }
        //insert delta
        (*itl)->insertDelta(delta, SystemConfig::delta_expire_query, 0, doHoldOnLink);
    }
}

void PathM::Release(bool doDelete) 
{
    list<Link*>::iterator itl = path.begin();
    for (; itl != path.end(); itl++)
    {   
        if ((*itl)->pDeltaList == NULL)
            continue;
        list<LinkStateDelta*>::iterator itd = (*itl)->pDeltaList->begin();
        for (; itd !=  (*itl)->pDeltaList->end(); itd++)
        {   
            if ((*itd)->owner_ucid == this->ucid && (*itd)->owner_seqnum == this->seqnum)
            {
                if (((*itd)->flags & DELTA_SCHEDULING) == 0) //$$$$ Link += Delta only for IR (immediate request)
                {
#ifdef CROSS_LAYER_GROOMING
                    //$$$$logic to update residual/dependent link
                    if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && (*itl)->dependents.size() > 0)
                    {
                        //adding bandwidth to residual link (only once)
                        if ((*itl)->dependents.front()->dependings.size() > 0 && (*itl)->dependents.front()->dependings.front() == (*itl))
                        {
                            for (int i = 0; i < 8; i++) 
                            {
                                (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] += (*itd)->bandwidth;
                                if ((*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] > (*itl)->dependents.front()->maxReservableBandwidth)
                                    (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] = (*itl)->dependents.front()->maxReservableBandwidth;
                            }
                        }
                    }
                    else
#endif
                        *(*itl) += *(*itd); 
                }

                if (doDelete)
                    (*itl)->pDeltaList->erase(itd);
                else
                    (*itd)->flags |= DELTA_MASKOFF;
                break;
            }
        }
    }
    // Do reverse links
    itl = reverse_path.begin();
    for (; itl != reverse_path.end(); itl++)
    {   
        if ((*itl)->pDeltaList == NULL)
            continue;
        list<LinkStateDelta*>::iterator itd = (*itl)->pDeltaList->begin();
        for (; itd !=  (*itl)->pDeltaList->end(); itd++)
        {   
            if ((*itd)->owner_ucid == this->ucid && (*itd)->owner_seqnum == this->seqnum)
            {
                if (((*itd)->flags & DELTA_SCHEDULING) == 0)
                {
#ifdef CROSS_LAYER_GROOMING
                    //$$$$logic to update residual/dependent links
                    if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && (*itl)->dependents.size() > 0)
                    {
                        //adding bandwidth to residual link (only once)
                        if ((*itl)->dependents.front()->dependings.size() > 0 && (*itl)->dependents.front()->dependings.front() == (*itl))
                        {
                            for (int i = 0; i < 8; i++) 
                            {
                                (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] += (*itd)->bandwidth;
                                if ((*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] > (*itl)->dependents.front()->maxReservableBandwidth)
                                    (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] = (*itl)->dependents.front()->maxReservableBandwidth;
                            }
                        }
                    }
                    else
#endif
                        *(*itl) += *(*itd); //$$$$ Link += Delta only for IR (immediate request)
                }

                if (doDelete)
                    (*itl)->pDeltaList->erase(itd);
                else
                    (*itd)->flags |= DELTA_MASKOFF;
                break;
            }
        }
    }
}

void PathM::Rehold() 
{
    list<Link*>::iterator itl = path.begin();
    for (; itl != path.end(); itl++)
    {
        if ((*itl)->pDeltaList == NULL)
            continue;
        list<LinkStateDelta*>::iterator itd = (*itl)->pDeltaList->begin();
        for (; itd !=  (*itl)->pDeltaList->end(); itd++)
        {
            if ((*itd)->owner_ucid == this->ucid && (*itd)->owner_seqnum == this->seqnum)
            {
                if (((*itd)->flags & DELTA_SCHEDULING) == 0) //$$$$ Link -= Delta only for IR (immediate request)
                {
#ifdef CROSS_LAYER_GROOMING
                    //$$$$logic to update residual/dependent links
                    if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && (*itl)->dependents.size() > 0)
                    {
                        //substracting bandwidth from residual link (only once)
                        if ((*itl)->dependents.front()->dependings.size() > 0 && (*itl)->dependents.front()->dependings.front() == (*itl))
                        {
                            for (int i = 0; i < 8; i++) 
                            {
                                (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] -= (*itd)->bandwidth;
                                if ((*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] < 0)
                                    (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] = 0;
                            }
                        }
                    }
                    else
#endif
                        *(*itl) -= *(*itd); 

                }

                (*itd)->flags &= (~DELTA_MASKOFF);
                break;
            }
        }
    }
    // Do reverse links
    itl = reverse_path.begin();
    for (; itl != reverse_path.end(); itl++)
    {
        if ((*itl)->pDeltaList == NULL)
            continue;
        list<LinkStateDelta*>::iterator itd = (*itl)->pDeltaList->begin();
        for (; itd !=  (*itl)->pDeltaList->end(); itd++)
        {
            if ((*itd)->owner_ucid == this->ucid && (*itd)->owner_seqnum == this->seqnum)
            {
                if (((*itd)->flags & DELTA_SCHEDULING) == 0) //$$$$ Link -= Delta only for IR (immediate request)
                {
#ifdef CROSS_LAYER_GROOMING
                    //$$$$logic to update residual/dependent links
                    if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && (*itl)->dependents.size() > 0)
                    {
                        //substracting bandwidth from residual link (only once)
                        if ((*itl)->dependents.front()->dependings.size() > 0 && (*itl)->dependents.front()->dependings.front() == (*itl))
                        {
                            for (int i = 0; i < 8; i++) 
                            {
                                (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] -= (*itd)->bandwidth;
                                if ((*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] < 0)
                                    (*itl)->dependents.front()->Iscds().front()->max_lsp_bw[i] = 0;
                            }
                        }
                    }
                    else
#endif
                        *(*itl) -= *(*itd); 
                }

                (*itd)->flags &= (~DELTA_MASKOFF);
                break;
            }
        }
    }
}

void PathM::Display() 
{
    char addr1[20], addr2[20];
    in_addr a1, a2;
    strcpy (addr1, inet_ntoa(source)); 
    strcpy (addr2, inet_ntoa(destination)); 
    printf("Path (src=%s,dst=%s,ucid=%d,seqnum=%d):\n", addr1, addr2, ucid, seqnum);
    list<Link*>::iterator itl = path.begin();
    for (; itl != path.end(); itl++)
    {
        Link* L = *itl;
        a1.s_addr = L->lclIfAddr;
        a2.s_addr = L->rmtIfAddr;
        strcpy (addr1, inet_ntoa(a1)); 
        strcpy (addr2, inet_ntoa(a2)); 
        printf("\tLink:%s--%s:", addr1, addr2);
        ISCD *iscd = L->Iscds().front();
        printf(" bandwidth=%g, ", iscd->max_lsp_bw[7]);
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && SystemConfig::wdm_subnet_on)
        { 
           printf ("\t    -- LSC-WDM specific information--\n\t       --> Available wavelengths:"); 
           TLP* tlp = L->GetAttribute(ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID"));
	    if (tlp && tlp->p)
	    {
               MovazWaveGrid* wavegrid = (MovazWaveGrid*)tlp->p;
	        u_int32_t base = ntohl(wavegrid->base);
	        u_int16_t interval = ntohs(wavegrid->interval);
	        u_int16_t size = ntohs(wavegrid->size);
	        for (int i = 0; i < (int)size/2; i++)
	        {
	            if ((wavegrid->out_channels[i] & 0xf0) == 0x70)
	                printf (" %d", base+i*2*interval);
	            if ((wavegrid->out_channels[i] & 0x0f) == 0x07)
	                printf (" %d", base+(i*2+1)*interval);
	        }
	    }
            printf("\n");
        } 
        else if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 && SystemConfig::wdm_subnet_on) 
        { 
	    TLP* tlp = L->GetAttribute(ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA"));
	    if (tlp && tlp->p)
	    {
               u_int32_t lambda = ntohl(*(u_int32_t*)tlp->p);
               printf ("\t    -- PSC4 specific information--\n\t       --> PSC-LSC adaptation lambda: %d\n", lambda); 
	    }
        }

        list<LinkStateDelta*>* pDeltaList = L->DeltaListPointer(); 
        if (pDeltaList) { 
            int k;
            list<LinkStateDelta*>::iterator it; 
            LinkStateDelta* delta; 
            for (k = 1, it = pDeltaList->begin(); it != pDeltaList->end(); k++, it++) { 
                delta = *it; 
                printf ("\n\t >>> Link State Delta [%d] - Status: %s%s%s%s (ucid=%d,seqnum=%d)\n", k, 
                    (delta->flags & DELTA_QUERIED) != 0 || (delta->expiration.tv_sec <= SystemConfig::delta_expire_query)  ? "Queried" : "", 
                    (delta->flags & DELTA_RESERVED) != 0 ? "-Reserved" : "", 
                    (delta->flags & DELTA_UPDATED) != 0 ? "-Updated" : "", 
                    (delta->flags & DELTA_MASKOFF) != 0 ? " (Maskoff)" : "", 
                    delta->owner_ucid, delta->owner_seqnum);
                printf ("\t    ---> Used Bandwidth: %g (Mbps)\n", delta->bandwidth); 
                if (delta->flags & DELTA_WAVELENGTH) 
                    printf ("\t    ---> Used wavelength: %d\n", delta->wavelength); 
                if ((delta->flags & DELTA_SCHEDULING) != 0)
                    printf ("\t    ---> Scheduled time: %d--%d\n", delta->start_time.tv_sec, delta->end_time.tv_sec); 
            } 
        } 
        printf("\n");
    }
}

//////////////////  PCEN_MCBase  ///////////////////

bool PCEN_MCBase::PostBuildTopology()
{
    //initiating reverse links and Tspec
    vector<PCENLink*>::iterator link_iter = links.begin();
    PCENLink* pcen_link;
    link_iter = links.begin();
    while (link_iter != links.end())
    {
        pcen_link = (*link_iter);

        PCENNode* pcen_node = pcen_link->rmt_end;
        if (!pcen_node)
            continue;
        list<PCENLink *>::iterator it_link;
        for (it_link = pcen_node->out_links.begin(); it_link != pcen_node->out_links.end(); it_link++)
        {
            if ((*it_link)->rmt_end == pcen_link->lcl_end && (*it_link)->link->rmtIfAddr == pcen_link->link->lclIfAddr
			&& (*it_link)->link->rmtId == pcen_link->link->lclId && (*it_link)->link->type == pcen_link->link->type)
            {
                //reverse link
                pcen_link->reverse_link = *it_link;
                break;
            }
        }
        //Tspec at head node
        if ((*it_link)->lcl_end)
            (*it_link)->lcl_end->tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
        link_iter++;
    }

    //assign router reference number
    for (int i =0 ; i < routers.size(); i++)
    {
        routers[i]->ref_num = i; 
    }


    //cleanup masked-off deltas
    for (int l = 0; l < links.size(); l++)
        if (links[l]->link)
            links[l]->link->cleanupMaskoffDeltas();

    return true;
}

int PCEN_MCBase::PickMCPCandidates(int M)
{
    //get simple hop length of requested path. C_0 = HopLenXBW.
    PCENNode* srcNode = GetNodeByIp(routers,&source);
    PCENNode* destNode = GetNodeByIp(routers,&destination);
    assert (srcNode && destNode);
    Dijkstra(srcNode->ref_num, destNode->ref_num);
    if (destNode->path.size() == 0)
        return -1;

    thePath.path.clear();
    list<PCENLink*>::iterator it1;
    for (it1 = destNode->path.begin(); it1 != destNode->path.end(); it1++)
        thePath.path.push_back((*it1)->link);
    thePath.cost = thePath.path.size()*thePath.bandwidth;
    
    //go through allPaths to find up to (M-1) Paths that are closest to C_0 by sorting C_x (HopLenxBW) 
    MCPaths.clear();
    if (allPaths.size() > 0)
        MCPaths.push_back(allPaths.front());
    for (int i = 1; i < allPaths.size(); i++)
    {
        //assign C_x (HopLenxBW)
        allPaths[i]->cost = allPaths[i]->path.size()*allPaths[i]->bandwidth;
        int j;
        for (j = 0; j < MCPaths.size(); j++)
        {
            if (ABS(allPaths[i]->cost - thePath.cost) < ABS(MCPaths[j]->cost - thePath.cost))
            {
                MCPaths.insert(MCPaths.begin()+j, allPaths[i]);
                break;
            }
        }
        if (j == MCPaths.size())
            MCPaths.push_back(allPaths[i]);
    }

    if (MCPaths.size() > M-1)
        MCPaths.erase(MCPaths.begin()+M-1, MCPaths.end());

    return MCPaths.size();
}

int PCEN_MCBase::PerformComputation()
{
    //initiating computation
    thePath.source.s_addr = this->source.s_addr;
    thePath.destination.s_addr = this->destination.s_addr;
    thePath.ucid = this->ucid;
    thePath.seqnum = this->seqnum;
    thePath.cost = 0;
    thePath.bandwidth = this->bandwidth_ingress;
    thePath.vlan_tag = 0;
    thePath.wavelength = 0;
    thePath.path.clear();
    MCPaths.push_back(&thePath);
    MC_KSP1.resize(MCPaths.size());
    MC_KSP2.resize(MCPaths.size());
    sortedMCPaths.resize(MCPaths.size());
    PCENNode* srcNode = GetNodeByIp(routers,&source);
    PCENNode* destNode = GetNodeByIp(routers,&destination);

    //run KSP for a single path if (allPaths.size() == 0), then return
    if (allPaths.size() == 0 || MCPaths.size() == 1)
    {
        SearchKSP(srcNode->ref_num, destNode->ref_num, SystemConfig::pce_k);
        if (KSP.size() == 0)
            return ERR_PCEN_NO_ROUTE;
        PathT* bestPath = ConstrainKSPaths(KSP);
        if (bestPath == NULL)
            return ERR_PCEN_NO_ROUTE;
        bestPath->source.s_addr = this->source.s_addr;
        bestPath->destination.s_addr = this->destination.s_addr;
        bestPath->ucid = this->ucid;
        bestPath->seqnum = this->seqnum;
        bestPath->bandwidth = this->bandwidth_ingress;
        thePath = *bestPath;
        return ERR_PCEN_NO_ERROR;
    }

    //maskoff deltas for every path in MCPaths --> release MCPaths resources
    int i, j;
    for (i = 0; i < MCPaths.size()-1; i++)
    {
        MCPaths[i]->Release();
    }

#ifdef SHOW_MCPATH
    printf("**** maskoff deltas for every path in MCPaths ****\n");
    for (i = 0; i < MCPaths.size()-1; i++)
    {
        MCPaths[i]->Display(); 
    }
#endif

    //M-Concurrent path computation algorithm for MCP
    //1. Run KSP for each path in MCPaths, including MCPaths.push_back(&thePath)
    //2. Pick (up to 2xM) best and second-to-best paths and place them into MC_KSP1 and MC_KSP2
    for (i = 0; i < MCPaths.size(); i++)
    {
        PCENNode* srcNode1 = GetNodeByIp(routers, &MCPaths[i]->source);
        PCENNode* destNode1 = GetNodeByIp(routers, &MCPaths[i]->destination);
        assert(srcNode1 && destNode1);
        SearchKSP(srcNode1->ref_num, destNode1->ref_num, SystemConfig::pce_k);
        if (GetBestTwoKSPaths(KSP, MC_KSP1[i], MC_KSP2[i]) == 0) //mark no_path
            //return ERR_PCEN_NO_ROUTE;
            continue;
        MC_KSP1[i].ucid = MC_KSP2[i].ucid = MCPaths[i]->ucid;
        MC_KSP1[i].seqnum = MC_KSP2[i].seqnum = MCPaths[i]->seqnum;
        MC_KSP1[i].source.s_addr = MC_KSP2[i].source.s_addr = MCPaths[i]->source.s_addr;
        MC_KSP1[i].destination.s_addr = MC_KSP2[i].destination.s_addr = MCPaths[i]->destination.s_addr;
    }

    //3. Sort MCPaths according to 'The Criteria' --> create sorted 'newPaths' list (identified by ucid+seqnum)
    vector<PathM*> sortedPaths;
    sortedPaths.assign(MCPaths.begin(), MCPaths.end());
    SortMPaths(sortedPaths);

#ifdef SHOW_MCPATH
    printf("**** SortMPaths before recompute/rehold MCPaths:****\n");
    for (i = 0; i < MCPaths.size(); i++)
    {
        MCPaths[i]->Display(); 
    }
    printf("**** sortedMCPaths:****\n");
    for (i = 0; i < sortedMCPaths.size(); i++)
    {
        sortedMCPaths[i].Display(); 
    }
#endif 
    //4. Run KSP for paths in newPaths
    for (i = 0; i < sortedPaths.size(); i++)
    {
        sortedMCPaths[i] = *sortedPaths[i];

        PCENNode* srcNode2 = GetNodeByIp(routers, &sortedMCPaths[i].source);
        PCENNode* destNode2 = GetNodeByIp(routers, &sortedMCPaths[i].destination);
        SearchKSP(srcNode2->ref_num, destNode2->ref_num, SystemConfig::pce_k);
        PathT* bestPath = ConstrainKSPaths(KSP);
        if (!bestPath)
        {
            //if any MCPaths[j] fails --> maskon all paths in MCPaths except for 'thePath'
            for (j = 0; j < MCPaths.size() - 1; j++)
            {
                MCPaths[j]->Rehold();
            }
            //also remove temporarily held deltas
            for (j = 0; j < i; j++)
            {
                sortedMCPaths[j].Release(true);
            }
            thePath.path.clear();
            return ERR_PCEN_NO_ROUTE;
        }

        bestPath->source.s_addr = sortedMCPaths[i].source.s_addr;
        bestPath->destination.s_addr = sortedMCPaths[i].destination.s_addr;
        bestPath->ucid = sortedMCPaths[i].ucid;
        bestPath->seqnum = sortedMCPaths[i].seqnum;
        bestPath->cost = sortedMCPaths[i].cost;
        bestPath->bandwidth = sortedMCPaths[i].bandwidth;
        bestPath->pflg.flag = sortedMCPaths[i].pflg.flag;
        sortedMCPaths[i] = *bestPath;

        //holding resources for sortedMCPaths[i]
        if (i < sortedPaths.size()-1)
        {
           /*
                    narb_lsp_request_tlv lsp_req;
                    lsp_req.type = ((MSG_LSP << 8) | ACT_QUERY);
                    lsp_req.length = sizeof(narb_lsp_request_tlv) - TLV_HDR_SIZE;
                    lsp_req.src.s_addr = sortedMCPaths[i].source.s_addr;
                    lsp_req.dest.s_addr = sortedMCPaths[i].destination.s_addr;
                    lsp_req.bandwidth = sortedMCPaths[i].bandwidth;
                    lsp_req.switching_type = this->switching_type_ingress;
                    lsp_req.encoding_type = this->encoding_type_ingress;
                    lsp_req.gpid = 0;
                    list<ero_subobj> ero;
                    u_int32_t w1 = wavelength;
                    wavelength = sortedMCPaths[i].wavelength;
                    GetPathERO(sortedMCPaths[i].path, ero);
                    wavelength = w1;
                    bool is_bidir = ((this->options & LSP_OPT_BIDIRECTIONAL) != 0);
                    //temporarily hold the new path --> Link::-= delta
                    if (ero.size() > 0)
                    //LSPHandler::UpdateLinkStatesByERO(lsp_req, ero, sortedMCPaths[i].ucid+1000, sortedMCPaths[i].seqnum+1000, is_bidir);
                    LSPHandler::UpdateLinkStatesByERO(lsp_req, ero, sortedMCPaths[i].ucid, sortedMCPaths[i].seqnum, is_bidir);
                    */
            sortedMCPaths[i].QueryHold();
#ifdef CROSS_LAYER_GROOMING
            if (sortedMCPaths[i].wavelength != 0)
                CreatePSCLinksFromLambdaPath(sortedMCPaths[i]);
#endif

#ifdef SHOW_MCPATH
            printf("**** Recompute and rehold sortedMCPaths[%d]****\n", i);
            sortedMCPaths[i].Display(); 
#endif
        }
    }

   //success!

    //remove old concurrent paths deltas no matter mask off or not
    for (i = 0; i < MCPaths.size() - 1; i++)
    {
        list<Link*>::iterator itl = MCPaths[i]->path.begin();
        for (; itl != MCPaths[i]->path.end(); itl++)
        {
            //just remove delta (w/o Link::-= delta as old paths has been 'released' by maskoff)
            //if both maskoff and queried deltas exist, only maskoff delta gets removed (ealier in list)
            (*itl)->removeDeltaByOwner(MCPaths[i]->ucid, MCPaths[i]->seqnum, false);
        }
        //Do reverse links
        for (itl = MCPaths[i]->reverse_path.begin(); itl != MCPaths[i]->reverse_path.end(); itl++)
        {
            //just remove delta (w/o Link::-= delta as old paths has been 'released' by maskoff)
            //if both maskoff and queried deltas exist, only maskoff delta gets removed
            (*itl)->removeDeltaByOwner(MCPaths[i]->ucid, MCPaths[i]->seqnum, false);
        }
    }


#ifdef SHOW_MCPATH
    printf("**** remove old concurrent paths deltas no matter mask off or not -- MCPaths:****\n");
    for (i = 0; i < MCPaths.size(); i++)
    {
        MCPaths[i]->Display(); 
    }
    printf("**** sortedMCPaths:****\n");
    for (i = 0; i < sortedMCPaths.size(); i++)
    {
        sortedMCPaths[i].Display(); 
    }
#endif

    //assign paths of newPaths to MCPaths[i] (including 'thePath')
    for (i = 0; i < MCPaths.size(); i++)
    {
        for (j = 0; j < sortedMCPaths.size(); j++)
        {
            if (MCPaths[i]->ucid == sortedMCPaths[j].ucid && MCPaths[i]->seqnum == sortedMCPaths[j].seqnum)
            {
                *MCPaths[i] = sortedMCPaths[j];
                /* TODO: update holding times ?*/
            }
        }
    }

#ifdef SHOW_MCPATH
    printf("**** Final: assign paths of newPaths to MCPaths[i] (including 'thePath') -- MCPaths:****\n");
    for (i = 0; i < MCPaths.size(); i++)
    {
        MCPaths[i]->Display(); 
    }
    printf("**** sortedMCPaths:****\n");
    for (i = 0; i < sortedMCPaths.size(); i++)
    {
        sortedMCPaths[i].Display(); 
    }
#endif
    return ERR_PCEN_NO_ERROR;
}

void PCEN_MCBase::Run()
{
    int ret;
    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_MCBase::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_MCBase::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if (!PostBuildTopology()) //pruning links
    {
        LOGF("PCEN_MCBase::PostBuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    //pick MCPaths
    if (PickMCPCandidates(SystemConfig::pce_m) < 0)
    {
        LOGF("PCEN_MCBase::PickMCPCandidates() failed to find a simple shortest path for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }
    
    //compute Path
    if ((ret = PerformComputation()) != 0 || thePath.path.size() ==0)
    {
        LOGF("PCEN_MCBase::PerformComputation() failed to find a path for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    //record the newly held path to allPaths (set ucid+seqnum)
    if (options & LSP_OPT_QUERY_HOLD)
    {
        PathM *newPath = new PathM;
        *newPath = thePath;
        allPaths.push_back(newPath);
    }

    //getPathERO
    if (has_wdm_layer)
        wavelength = thePath.wavelength;
    GetPathERO(thePath.path, this->ero);
    
    //replyERO (w/ holding logic)
    ReplyERO();
}

int PCEN_MCBase::GetBestTwoKSPaths(vector<PathT*>& KSP, PathM &path1, PathM &path2)
{
    double minCost = PCEN_INFINITE_COST;
    PathT* bestPath = NULL;
    PathT* secondPath = NULL;
    PathT* P;
    vector<PathT*>::iterator iterP;

    iterP = KSP.begin();
    while (iterP != KSP.end())
    {
        P = (*iterP);
        if (VerifyPathConstraints(P->path, P->vlan_tag, P->wavelength))
        {
            if (P->cost < minCost)
            {
                minCost = P->cost;
                if (bestPath)
                    secondPath = bestPath;
                bestPath = P;
            }
            iterP++;
        }
        else
        {
            delete (*iterP); //$$ release PathT memory
            iterP = KSP.erase(iterP);
        }
    }

    int ret = 0;
    if (bestPath)
    {
        path1 = *bestPath;
        ret++;
    }
    else
    {
        path1.pflg.pfg.filteroff = 1;
    }

    if (secondPath)
    {
        path2 = *secondPath;
        ret++;
    }
    else
    {
        path2.pflg.pfg.filteroff = 1;
    }

    return ret;
}

inline void SwapPaths(PathM* &path1, PathM* &path2)
{
    PathM* p;
    p= path1;
    path1 = path2;
    path2 = p;
}

inline double SumOfBandwidthWeightedCommonLinks(PathM* &P, vector<PathM>& Paths)
{
    if (P->path.size() == 0)
        return 0;
    int i, numPaths = Paths.size();

    for (i = 0; i < numPaths; i++)
    {
        if (P->ucid == Paths[i].ucid && P->seqnum == Paths[i].seqnum)
            break;
    }
    //assert (i < numPaths);
    if (i == numPaths) return 0;

    PathM* path1 = &Paths[i];

    if (path1->pflg.pfg.filteroff != 0)
        return PCEN_INFINITE_COST;

    double sum = 0;
    list<Link*>::iterator iter1, iter2;
    for (iter1 = path1->path.begin(); iter1 != path1->path.end(); iter1++)
    {
        for (i = 0; i < numPaths; i++)
        {
            if (path1->ucid == Paths[i].ucid && path1->seqnum == Paths[i].seqnum)
                continue;
            for (iter2 = Paths[i].path.begin(); iter2 != Paths[i].path.end(); iter2++)
            {
                if ((*iter1) == (*iter2))
                {
                    sum += (path1->bandwidth < Paths[i].bandwidth ? path1->bandwidth : Paths[i].bandwidth);
                }
            }
        }
    }
    return sum;
}

// swap if (path1 < path2) according to porivsioning priority criteria
void PCEN_MCBase::SortTwoPaths(PathM* &path1, PathM* &path2)
{

    // criterion 1: bandwidth-weighted hop length (created-time?)
    if (path1->bandwidth*path1->path.size() < path2->bandwidth*path2->path.size())
        SwapPaths(path1, path2);
    // criterion 2: sum of bandwidth-weighted common links for best-path
    else if (SumOfBandwidthWeightedCommonLinks(path1, MC_KSP1) < SumOfBandwidthWeightedCommonLinks(path2, MC_KSP1))
        SwapPaths(path1, path2);
    // criterion 3: sum of bandwidth-weighted common links for second-path --> no_path first ...
    else if (SumOfBandwidthWeightedCommonLinks(path1, MC_KSP2) < SumOfBandwidthWeightedCommonLinks(path2, MC_KSP2))
        SwapPaths(path1, path2);
}

// decreasing order according to porivsioning priority criteria
void PCEN_MCBase::SortMPaths(vector<PathM*>& Paths)
{
    int numPaths = Paths.size();

    for (int i = 0; i < numPaths; i++)
        for (int j = 0; j < numPaths; j++)
        {
            if (j > i)
                SortTwoPaths(Paths[i], Paths[j]);
        }
}

int PCEN_MCBase::GetPathERO(list<Link*>& path, list<ero_subobj>& ero)
{
    ero.clear();
    PCENLink* pcen_link;
    list<Link*>::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        pcen_link = GetPCENLinkByLink(*it);
        if (pcen_link == NULL)
            continue;
        AddLinkToEROTrack(ero, pcen_link);
    }
}

PCENLink* PCEN_MCBase::GetPCENLinkByLink(Link* link)
{
    for (int i = 0; i < links.size(); i++)
    {
        if (links[i]->link == link)
            return links[i];
    }
    return NULL;
}

bool PCEN_MCBase::CreatePSCLinksFromLambdaPath(PathM &pm)
{
    list<Link*>::iterator itl = pm.path.begin();
    list<Link*>::iterator itx = pm.path.end();
    list<Link*>::iterator ity = pm.path.end();
    for (; itl != pm.path.end(); itl++)
    {
        if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)
        {
            ity = itl;
            if (itx == pm.path.end())
                itx = itl;
        }
    }
    itx--;
    if (itx == pm.path.begin() || itx == pm.path.end())
        return false;
    if (ity == pm.path.end())
        return false;
    PCENLink* pcen_link_a = PCEN_MCBase::GetPCENLinkByLink(*itx);
    PCENLink* pcen_link_z = PCEN_MCBase::GetPCENLinkByLink(*ity);
    assert(pcen_link_a && pcen_link_z);

    PCENLink* pcen_link_psc1 = new PCENLink;
    pcen_link_psc1->linkID = links.back()->linkID+1;
    pcen_link_psc1->lcl_end = pcen_link_a->lcl_end;
    pcen_link_psc1->rmt_end = pcen_link_z->rmt_end;
    pcen_link_a->lcl_end->out_links.push_back(pcen_link_psc1);
    pcen_link_z->rmt_end->in_links.push_back(pcen_link_psc1);

    PCENLink* pcen_link_psc2 = new PCENLink;
    pcen_link_psc2->linkID = links.back()->linkID+2;
    pcen_link_psc2->lcl_end = pcen_link_z->rmt_end;
    pcen_link_psc2->rmt_end = pcen_link_a->lcl_end;
    pcen_link_z->rmt_end->out_links.push_back(pcen_link_psc2);
    pcen_link_a->lcl_end->in_links.push_back(pcen_link_psc2);

    pcen_link_psc1->reverse_link = pcen_link_psc2;
    pcen_link_psc2->reverse_link = pcen_link_psc1;

    Link* link1 = new Link(pcen_link_a->link->AdvRtId(), pcen_link_z->link->Id(), pcen_link_a->link->LclIfAddr(), pcen_link_z->link->RmtIfAddr());
    Link* link2 = new Link(pcen_link_z->link->AdvRtId(), pcen_link_a->link->Id(), pcen_link_z->link->RmtIfAddr(), pcen_link_a->link->LclIfAddr());
    link1->metric = link2->metric = pcen_link_a->link->metric;
    link1->maxBandwidth = link2->maxBandwidth = pcen_link_a->link->maxBandwidth;
    link1->maxReservableBandwidth = link2->maxReservableBandwidth = pcen_link_a->link->maxBandwidth - pm.bandwidth;
    for (int i = 0; i < 8; i++)
    {
        link1->unreservedBandwidth[i]= link2->unreservedBandwidth[i] = pcen_link_a->link->maxBandwidth - pm.bandwidth;
        link1->unreservedBandwidth[i]= pcen_link_a->link->maxBandwidth - pm.bandwidth;
    }
    ISCD* iscd1 = new ISCD;
    memset(iscd1, 0, sizeof(ISCD));
    memcpy(iscd1->max_lsp_bw, link1->unreservedBandwidth, 8*sizeof(float));   
    iscd1->encoding = (u_char)LINK_IFSWCAP_SUBTLV_ENC_PKT;
    iscd1->swtype = (u_char)LINK_IFSWCAP_SUBTLV_SWCAP_PSC4;
    link1->Iscds().push_back(iscd1);
    ISCD* iscd2 = new ISCD;
    *iscd2 = *iscd1;
    link2->Iscds().push_back(iscd2);

    for (itl = itx; itl != ity; itl++)
    {
        (*itl)->dependents.push_back(link1);
        link1->dependings.push_back(*itl);
    }
    itx == pm.reverse_path.end();
    for (itl = pm.reverse_path.begin(); itl != pm.reverse_path.end(); itl++)
    {
        if ((*itl)->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)
        {
            if (itx == pm.reverse_path.end())
                itx = itl;
            (*itl)->dependents.push_back(link2);
            link2->dependings.push_back(*itl);
        }
    }
    itx--;
    if (itx != pm.path.begin() && itx != pm.path.end())
    {
        (*itx)->dependents.push_back(link2);
        link2->dependings.push_back(*itx);
    }

    //$$$$ keep the new links even after PCEN topology destruction
    pcen_link_psc1->link = link1;
    //pcen_link_psc1->link_self_allocated = true;
    pcen_link_psc2->link = link2;
    //pcen_link_psc2->link_self_allocated = true;
    
    links.push_back(pcen_link_psc1);
    links.push_back(pcen_link_psc2);

    return true;
}

