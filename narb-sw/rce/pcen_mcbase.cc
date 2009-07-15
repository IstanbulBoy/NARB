/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */

#include "pcen_mcbase.hh"

vector<PathT*> PCEN_MCBase::allPaths;

bool PCEN_MCBase::PostBuildTopology()
{
    bool ret = PCEN_KSP::PostBuildTopology();

    //cleanup masked-off deltas
    for (int l = 0; l < links.size(); l++)
        if (links[l]->link)
            links[l]->link->cleanupMaskoffDeltas();

    return ret;
}

#define ABS(X) ((X >= 0.0) ? (X) : -(X))

int PCEN_MCBase::PickMCPCandidates(int M)
{
    //get simple hop length of requested path. C_0 = HopLenXBW.
    PCENNode* srcNode = GetNodeByIp(routers,&source);
    PCENNode* destNode = GetNodeByIp(routers,&destination);
    assert (srcNode && destNode);
    Dijkstra(srcNode->ref_num, destNode->ref_num);
    if (destNode->path.size() == 0)
        return -1;
    thePath.path.assign(destNode->path.begin(),destNode->path.end());
    thePath.cost = thePath.path.size()*thePath.bandwidth;
    
    //go through allPaths to find up to (M-1) Paths that are closest to C_0 by sorting C_x (HopLenxBW) 
    MCPaths.clear();
    if (allPaths.size() > 0)
        MCPaths.push_back(allPaths.front());
    for (int i = 0; i < allPaths.size(); i++)
    {
      //assign C_x (HopLenxBW)
       allPaths[i]->cost = allPaths[i]->path.size()*allPaths[i]->bandwidth;
        for (int j = 0; j < MCPaths.size(); j++)
        {
            if (ABS(allPaths[i]->cost - thePath.cost) < ABS(MCPaths[j]->cost - thePath.cost))
            {
                MCPaths.insert(MCPaths.begin()+j, allPaths[i]);
                break;
            }
        }
    }

    if (MCPaths.size() > M-1)
        MCPaths.erase(MCPaths.begin()+M-1, MCPaths.end());

    return MCPaths.size();
}

int PCEN_MCBase::PerformComputation()
{
    thePath.path.clear();

    PCENNode* srcNode = GetNodeByIp(routers,&source);
    PCENNode* destNode = GetNodeByIp(routers,&destination);

    //run KSP for a single path if (allPaths.size() == 0), then return
    if (allPaths.size() == 0 || MCPaths.size() == 0)
    {
        SearchKSP(srcNode->ref_num, destNode->ref_num, SystemConfig::pce_k);
        if (KSP.size() == 0)
            return false;
        PathT* bestPath = ConstrainKSPaths(KSP);
        if (bestPath == NULL)
            return false;
        thePath = *bestPath;
        return true;
    }

    //maskoff deltas for every path in MCPaths --> release MCPaths resources
    int i;
    for (i = 0; i < MCPaths.size(); i++)
    {
        narb_lsp_request_tlv lsp_req;
        lsp_req.type = ((MSG_LSP << 8) | ACT_MASKOFF);
        lsp_req.length = sizeof(narb_lsp_request_tlv) - TLV_HDR_SIZE;
        lsp_req.src.s_addr = MCPaths[i]->source.s_addr;
        lsp_req.dest.s_addr = MCPaths[i]->destination.s_addr;
        lsp_req.bandwidth = MCPaths[i]->bandwidth;
        lsp_req.switching_type = this->switching_type_ingress;
        lsp_req.encoding_type = this->encoding_type_ingress;
        lsp_req.gpid = 0;
        list<ero_subobj> ero;
        GetPathERO(MCPaths[i]->path, ero);
        bool is_bidir = ((this->options & LSP_OPT_BIDIRECTIONAL) != 0);
        if (ero.size() > 0)
            LSPHandler::UpdateLinkStatesByERO(lsp_req, ero, MCPaths[i]->ucid, MCPaths[i]->seqnum, is_bidir);
    }

    //M-Concurrent path computation algorithm for MCP
    //1. Run KSP for each path in MCPaths, including MCPaths.push_back(&thePath)
    //2. Pick (up to 2xM) best and second-to-best paths and place them into MC_KSP1 and MC_KSP2
    vector<PathT> MC_KSP1; MC_KSP1.reserve(MCPaths.size()+1);
    vector<PathT> MC_KSP2; MC_KSP2.reserve(MCPaths.size()+1);
    MCPaths.push_back(&thePath);
    for (i = 0; i < MCPaths.size(); i++)
    {
        PCENNode* srcNode = GetNodeByIp(routers,&MCPaths[i].source);
        PCENNode* destNode = GetNodeByIp(routers,&&MCPaths[i].destination);
        SearchKSP(srcNode->ref_num, destNode->ref_num, SystemConfig::pce_k);
        MC_KSP1[i] = *MCPaths[i];
        MC_KSP2[i] = *MCPaths[i];
        if (GetBestTwoKSPaths(KSP, MC_KSP1[i], MC_KSP2[i]) == 0) //mark no_path
            return false;
    }

    //3. Sort MCPaths according to 'The Criteria' --> create sorted 'newPaths' list (identified by ucid+seqnum)
    vector<PathT> sortedMCPaths;

    //4. Run KSP for paths in newPaths
    for (i = 0; i < sortedMCPaths.size(); i++)
    {
        PCENNode* srcNode = GetNodeByIp(routers,&MCPaths[i].source);
        PCENNode* destNode = GetNodeByIp(routers,&&MCPaths[i].destination);
        SearchKSP(srcNode->ref_num, destNode->ref_num, SystemConfig::pce_k);
        PathT* bestPath = ConstrainKSPaths(KSP);
        if (!bestPath)
        {
            //if any MCPaths[i] fails --> maskon all paths in MCPaths except for 'thePath'  (identified by ucid+seqnum)
            
            thePath.path.clear();
            return false;
        }
        bestPath->source.s_addr = sortedMCPaths[i]->source.s_addr;
        bestPath->destination.s_addr = sortedMCPaths[i]->destination.s_addr;
        bestPath->ucid = sortedMCPaths[i]->ucid;
        bestPath->seqnum = sortedMCPaths[i]->seqnum;
        bestPath->cost = sortedMCPaths[i]->cost;
        bestPath->bandwidth = sortedMCPaths[i]->bandwidth;
        bestPath->vlan_tag = sortedMCPaths[i]->vlan_tag;
        bestPath->wavelength = sortedMCPaths[i]->wavelength;
        bestPath->pflg = sortedMCPaths[i]->pflg;
        sortedMCPaths[i] = *bestPath;
    }

    //otherwise, assign paths of newPaths to MCPaths[i] (including 'thePath'), return success
    for (i = 0; i < MCPaths.size(); i++)
    {
        for (int j = 0; j < sortedMCPaths.size(); j++)
        {
            if (MCPaths[i]->ucid == sortedMCPaths[j]->ucid && MCPaths[i]->seqnum == sortedMCPaths[j]->seqnum)
                MCPaths[i]->path.assign(sortedMCPaths[j]->path.begin(), sortedMCPaths[j]->path.end());
        }
    }

    return true;
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
        PathT *newPath = new PathT;
        *newPath = thePath;
        allPaths.push_back(newPath);
    }

    //getPathERO
    GetPathERO(thePath.path, this->ero);
    
    //replyERO (w/ holding logic)
    ReplyERO();
}

int PCEN_MCBase::GetBestTwoKSPaths(vector<PathT*>& KSP, PathT &path1, PathT &path2)
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

