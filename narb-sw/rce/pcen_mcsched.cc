/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */

#include "pcen_mcsched.hh"

bool PCEN_MCSched::PostBuildTopology()
{
    return PCEN_MCBase::PostBuildTopology();
}

int PCEN_MCSched::PickMCPCandidates(int M)
{
    return 0;
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
    
    //$$$$ TODO: replyERO but only add sheduling deltas (w/o holding Link resource)
}

bool PCEN_MCSched::VerifyPathConstraints(list<PCENLink*>& path, u_int32_t& pathVtag, u_int32_t& pathWave)
{
    return true;
}

u_int32_t GetOverLappingTime(PathM* path1, PathM* path2)
{
    if (path1->start_time.tv_sec == 0 && path2->start_time.tv_sec == 0)
        return MAX_SCHEDULE_DURATION;
    else if (path1->start_time.tv_sec != 0 && path2->start_time.tv_sec == 0)
        return (path1->end_time.tv_sec - path1->start_time.tv_sec) > MAX_SCHEDULE_DURATION ? MAX_SCHEDULE_DURATION : path1->end_time.tv_sec - path1->start_time.tv_sec;
    else if (path1->start_time.tv_sec == 0 && path2->start_time.tv_sec != 0)
        return (path2->end_time.tv_sec - path2->start_time.tv_sec) > MAX_SCHEDULE_DURATION ? MAX_SCHEDULE_DURATION : path2->end_time.tv_sec - path2->start_time.tv_sec;
    //else
    u_int32_t duration = 0;
     if (path1->start_time.tv_sec < path2->start_time.tv_sec && path1->end_time.tv_sec < path2->end_time.tv_sec)
         duration = path2->start_time.tv_sec - path1->end_time.tv_sec;
     else if (path1->start_time.tv_sec > path2->start_time.tv_sec && path1->end_time.tv_sec < path2->end_time.tv_sec)
        duration = path1->end_time.tv_sec - path1->start_time.tv_sec;
     else if (path1->start_time.tv_sec < path2->start_time.tv_sec && path1->end_time.tv_sec > path2->end_time.tv_sec)
        duration = path2->end_time.tv_sec - path2->start_time.tv_sec;
     else 
        duration = path1->start_time.tv_sec - path2->end_time.tv_sec;
    //return
     if (duration <= 0)
        return 1;
     if (duration > MAX_SCHEDULE_DURATION)
        return MAX_SCHEDULE_DURATION;
     return duration;
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
            u_int32_t overlap = GetOverLappingTime(path1, &Paths[i]);
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

