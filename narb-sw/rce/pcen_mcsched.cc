/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */

#include "pcen_mcsched.hh"
#include "rce_movaz_types.hh"

bool PCEN_MCSched::PostBuildTopology()
{
    return PCEN_MCBase::PostBuildTopology();
}

int PCEN_MCSched::PickMCPCandidates(int M)
{
    //get simple hop length of requested path. CT_0 = HopLen X BW X 1.0.
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

    //go through allPaths to find up to (M-1) Paths that are closest to CT_0 by sorting CT_x (HopLenxBWxOverlapTime) 
    MCPaths.clear();
    if (allPaths.size() > 0)
        MCPaths.push_back(allPaths.front());
    for (int i = 1; i < allPaths.size(); i++)
    {
        //assign CT_x
        double overlap = GetPathOverLappingTime(&thePath, allPaths[i]);
        allPaths[i]->cost = allPaths[i]->path.size()*allPaths[i]->bandwidth
        	*(1.0-BANDWIDTH_TIME_FACTOR+BANDWIDTH_TIME_FACTOR*(double)overlap/(double)MAX_SCHEDULE_DURATION);
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

int  PCEN_MCSched::PerformComputation()
{
    assert(thePath.start_time.tv_sec != 0 && thePath.end_time.tv_sec > thePath.start_time.tv_sec);
    return PCEN_MCBase::PerformComputation();
}

void  PCEN_MCSched::Run()
{
    int ret;
    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_MCSched::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_MCSched::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if (!PostBuildTopology()) //pruning links
    {
        LOGF("PCEN_MCSched::PostBuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    //pick MCPaths
    if (PickMCPCandidates(SystemConfig::pce_m) < 0)
    {
        LOGF("PCEN_MCSched::PickMCPCandidates() failed to find a simple shortest path for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }
    
    //compute Path
    if ((ret = PerformComputation()) != 0 || thePath.path.size() ==0)
    {
        LOGF("PCEN_MCSched::PerformComputation() failed to find a path for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
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
    
    //replyERO w/o holding Link resource
    u_int32_t options_orig = options;
	options &= ~LSP_OPT_QUERY_HOLD;
    ReplyERO();
	options = options_orig;

    //add sheduling deltas w/o holding link resource
    thePath.QueryHold();
}

void PCEN_MCSched::AdjustLinkResourceBySchedule(PCENLink *L, bool doAddOrDelete)
{
    list<LinkStateDelta*>* pDeltaList;
    if (L != NULL && L->link != NULL && (pDeltaList = L->link->DeltaListPointer()) != NULL)
    {
        list<LinkStateDelta*>::iterator itd = pDeltaList->begin();
        LinkStateDelta* delta;
        for (; itd != pDeltaList->end(); itd++)
        {
            delta = (*itd);
            if (delta->flags & DELTA_SCHEDULING && OverLappingTime(thePath.start_time, thePath.end_time, delta->start_time, delta->end_time)> 0)
                if (doAddOrDelete)
                    *L->link += *delta;
                else
                    *L->link -= *delta;
        }
    }
}

bool PCEN_MCSched::VerifyPathConstraints(list<PCENLink*>& path, u_int32_t& pathVtag, u_int32_t& pathWave)
{
    PCENLink* L = NULL;
    list<PCENLink*>::iterator iterL;
    ConstraintTagSet head_vtagset(MAX_VLAN_NUM), next_vtagset(MAX_VLAN_NUM);
    ConstraintTagSet head_waveset(MAX_WAVE_NUM, 190000, 100), next_waveset(MAX_WAVE_NUM, 190000, 100);
    TSpec link_tspec;

    if (path.size() == 0)
        return false;

    // initializing VLAN tags and wavelengths
    if (vtag != 0)
        head_vtagset.AddTag(vtag);
    else         
        head_vtagset.Clear();
    head_waveset.Clear();        
    // verifying path
    for (iterL = path.begin(); iterL != path.end(); iterL++)
    {
        L = (*iterL);
        if (L == NULL || L->lcl_end == NULL || L->rmt_end ==NULL || L->link == NULL || L->link->Iscds().size() == 0)
            return false;

        AdjustLinkResourceBySchedule(L, false); //delete/hold scheduled resources

        if (!L->IsAvailableForTspec(L->lcl_end->tspec))
            goto _restore_and_quit;

        if (!head_vtagset.IsEmpty())
        {
            L->ProceedByUpdatingVtags(head_vtagset, next_vtagset);
            if (next_vtagset.IsEmpty())
                goto _restore_and_quit;
            head_vtagset = next_vtagset;
        }
        if (!head_waveset.IsEmpty())
        {
            L->ProceedByUpdatingWaves(head_waveset, next_waveset);
            if (next_waveset.IsEmpty())
                goto _restore_and_quit;
            head_waveset = next_waveset;
        }

        if (L->CrossingRegionBoundary(L->lcl_end->tspec))
        {
            L->GetNextRegionTspec(L->rmt_end->tspec);
            //$$$$ WDM subnet special handling
            if (has_wdm_layer && (L->rmt_end->tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || L->rmt_end->tspec.SWtype == MOVAZ_LSC))
            {
                MovazTeLambda tel;
                u_int32_t* p_freq;
                PCENLink * reverseLink = L->reverse_link;
                head_waveset.Clear();
                list<void*>::iterator it;
                bool has_wave = false;

                list<void*> *p_list = (list<void*>*)(reverseLink->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA"));
                if (reverseLink == NULL || p_list == NULL)
                    goto _restore_and_quit;
              
                for (it = p_list->begin(); it!= p_list->end(); it++)
                {
                    tel = *(MovazTeLambda*)(*it);
                    ntoh_telambda(tel);
                    if (tel.priority == 0x07)
                    {
                        if (valid_channel(tel.channel_id))
                            head_waveset.AddTag(tel.channel_id);
                        else
                            has_wave = true;
                    }
                }
                //$$$$ PSC/L2SC-->LSC link DRAGON speical handling
                p_freq = (u_int32_t*)(L->AttributeByTag("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA"));
                if (has_wave && p_freq)
                    head_waveset.AddTag(ntohl(*p_freq)); //$$ freq = ANY_WAVE ?
                if (head_waveset.IsEmpty())
                    goto _restore_and_quit;
                    //head_waveset.AddTag(ANY_WAVE);
            }
            else if (has_wdm_layer && (L->lcl_end->tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || L->lcl_end->tspec.SWtype == MOVAZ_LSC))
            {
                head_waveset.Clear();
            }
        }       
        else
        {
            L->rmt_end->tspec = L->lcl_end->tspec;
        }
    }

    pathVtag = next_vtagset.LowestTag();
    pathWave = next_waveset.LowestTag();

    return true;

_restore_and_quit:
    AdjustLinkResourceBySchedule(L, true); //add/release scheduled resources

}

u_int32_t OverLappingTime(struct timeval &st1, struct timeval &et1, struct timeval &st2, struct timeval &et2)
{
    if (st1.tv_sec == 0 && st2.tv_sec == 0)
        return MAX_SCHEDULE_DURATION;
    else if (st1.tv_sec != 0 && st2.tv_sec == 0)
        return (et1.tv_sec - st1.tv_sec) > MAX_SCHEDULE_DURATION ? MAX_SCHEDULE_DURATION : et1.tv_sec - st1.tv_sec;
    else if (st1.tv_sec == 0 && st2.tv_sec != 0)
        return (et2.tv_sec - st2.tv_sec) > MAX_SCHEDULE_DURATION ? MAX_SCHEDULE_DURATION : et2.tv_sec - st2.tv_sec;
    //else
    u_int32_t duration = 0;
     if (st1.tv_sec < st2.tv_sec && et1.tv_sec < et2.tv_sec)
         duration = st2.tv_sec - et1.tv_sec;
     else if (st1.tv_sec > st2.tv_sec && et1.tv_sec < et2.tv_sec)
        duration = et1.tv_sec - st1.tv_sec;
     else if (st1.tv_sec < st2.tv_sec && et1.tv_sec > et2.tv_sec)
        duration = et2.tv_sec - st2.tv_sec;
     else 
        duration = st1.tv_sec - et2.tv_sec;
    //return
     if (duration <= 0)
        return 0;
     if (duration > MAX_SCHEDULE_DURATION)
        return MAX_SCHEDULE_DURATION;
     return duration;
}


u_int32_t GetPathOverLappingTime(PathM* path1, PathM* path2)
{
     return OverLappingTime(path1->start_time, path1->end_time, path2->start_time, path2->end_time);
}


inline double SumOfBandwidthTimeWeightedCommonLinks(PathM* &P, vector<PathM>& Paths)
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
            u_int32_t overlap = GetPathOverLappingTime(path1, &Paths[i]);
            for (iter2 = Paths[i].path.begin(); iter2 != Paths[i].path.end(); iter2++)
            {
                if ((*iter1) == (*iter2))
                {
                    sum += (path1->bandwidth < Paths[i].bandwidth ? path1->bandwidth : Paths[i].bandwidth)*(1.0-BANDWIDTH_TIME_FACTOR+BANDWIDTH_TIME_FACTOR*(double)overlap/(double)MAX_SCHEDULE_DURATION);
                }
            }
        }
    }
    return sum;
}


// swap if (path1 < path2) according to porivsioning priority criteria
void PCEN_MCSched::SortTwoPaths(PathM* &path1, PathM* &path2)
{
    // criterion 1: bandwidth-weighted hop length
    if (path1->bandwidth*path1->path.size() < path2->bandwidth*path2->path.size())
        SwapPaths(path1, path2);
    // criterion 2: sum of bandwidth-weighted common links for best-path
    else if (SumOfBandwidthTimeWeightedCommonLinks(path1, MC_KSP1) < SumOfBandwidthTimeWeightedCommonLinks(path2, MC_KSP1))
        SwapPaths(path1, path2);
    // criterion 3: sum of bandwidth-weighted common links for second-path --> no_path first ...
    else if (SumOfBandwidthTimeWeightedCommonLinks(path1, MC_KSP2) < SumOfBandwidthTimeWeightedCommonLinks(path2, MC_KSP2))
        SwapPaths(path1, path2);
}

