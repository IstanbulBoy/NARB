/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */

#include "pcen_dora.hh"

bool PCEN_DORA::doraInitiated = false;
list<PathT*>* PCEN_DORA::DP[PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_NODE_NUM];
double PCEN_DORA::PPV[PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_LINK_NUM];

bool PCEN_DORA::PostBuildTopology()
{
    if (!PCEN_KSP::PostBuildTopology())
        return false;

    int i, j, l;
    int router_num = routers.size();
    int link_num = links.size();

    //initiating request
    if (!doraInitiated)
    {
        doraInitiated = true;

        for (i = 0; i < router_num; i++)
        {
            for (j = 0; j < router_num; j++)
            {
                if (i == j)
                    DP_ARRAY(i,j) = NULL;
                else
                    DP_ARRAY(i,j) = ComputeDPByKSP(i, j);
            }
        }

        double biggest_ppv = 0;
        for (i = 0; i < router_num; i++)
        {
            for (j = 0; j < router_num; j++)
            {
                for (l = 0; l < link_num; l++)
                {
                    if (i == j)
                        PPV_ARRAY(i,j,l) = 0;
                    else
                        PPV_ARRAY(i,j,l) = GetDPLinkCount(i, j, l);
                    if (PPV_ARRAY(i,j,l) > biggest_ppv)
                        biggest_ppv = PPV_ARRAY(i,j,l);
                }                
            }
        }
        //normalize PPV
        for (i = 0; i < router_num; i++)
            for (j = 0; j < router_num; j++)
                for (l = 0; l < link_num; l++)
                    PPV_ARRAY(i,j,l) = PPV_ARRAY(i,j,l)/biggest_ppv*100.0;

        return false;
    }

    return true;
}

int PCEN_DORA::PerformComputation()
{
    PCENNode* srcNode = GetNodeByIp(routers,&source);
    PCENNode* destNode = GetNodeByIp(routers,&destination);

    //assign DORA cost metric
    int i, j, l;
    int link_num = links.size();
    double bw_metric, biggest_metric = 0;
    for (l = 0; l < link_num; l++)
    {
        if (links[l]->link == NULL || links[l]->link->Iscds().size() == 0)
            continue;
        bw_metric = links[l]->link->Iscds().front()->max_lsp_bw[7]/links[l]->link->MaxReservableBandwidth()*links[l]->PCENmetric();
        links[l]->SetPCENmetric(bw_metric);
        if (bw_metric > biggest_metric)
            biggest_metric = bw_metric;
    }
    //normalize costs
    
    for (l = 0; l < link_num; l++)
    {
        if (links[l]->link == NULL || links[l]->link->Iscds().size() == 0)
            continue;
        bw_metric = links[l]->PCENmetric()/biggest_metric*100.0;
        links[l]->SetPCENmetric(PPV_ARRAY(srcNode->ref_num, destNode->ref_num, links[l]->linkID)*(1-BWP) + bw_metric*BWP);
    }

    //now run KSP
    SearchKSP(srcNode->ref_num, destNode->ref_num, SystemConfig::pce_k);
    LOGF("Found %d shortest paths...\n", KSP.size());

    PathT* bestPath = ConstrainKSPaths(KSP);
    if (!bestPath)
    {
        LOGF("PCEN_DORA::PerformComputation: KSP failed to find a path\n");
        return ERR_PCEN_NO_ROUTE;
    }

    bestPath->source.s_addr = this->source.s_addr;
    bestPath->destination.s_addr = this->destination.s_addr;
    bestPath->ucid = this->ucid;
    bestPath->seqnum = this->seqnum;
    bestPath->bandwidth = this->bandwidth_ingress;
    thePath = *bestPath;

    return ERR_PCEN_NO_ERROR;
}

void PCEN_DORA::Run()
{
    int ret;
    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_DORA::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_DORA::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if (!PostBuildTopology())
    {
        LOGF("PCEN_DORA::PostBuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    //compute Path
    if ((ret = PerformComputation()) != 0 || thePath.path.size() ==0)
    {
        LOGF("PCEN_DORA::PerformComputation() failed to find a path for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
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

list<PathT*>* PCEN_DORA::ComputeDPByKSP(int I, int J)
{
    list<PathT*>* pList = new list<PathT*>;
    
    SearchKSP(I, J, SystemConfig::pce_k);
    PathT* bestPath = ConstrainKSPaths(KSP);
    int M = SystemConfig::pce_m;
    while (bestPath && M > 0)
    {
        M --;
        pList->push_back(bestPath);
        //1. adjust cost on bestPath
        list<PCENLink*>::iterator itl = bestPath->path.begin();
        for (; itl != bestPath->path.end(); itl++)
            (*itl)->SetPCENmetric((double)PCEN_VERYBIG_COST);
        //2. remove bestPath from KSP
        //3. and recalculate path cost
        vector<PathT*>::iterator itp = KSP.begin();
        for (; itp != KSP.end(); itp++)
        {
            if ((*itp) == bestPath)
            {
                itp = KSP.erase(itp);
                if (itp == KSP.end())
                    break;
                else
                    continue;
            }
            (*itp)->CalculatePathCost();
        }
        //4. get another bestPath
        bestPath = ConstrainKSPaths(KSP);
    }

    return pList;
}

double PCEN_DORA::GetDPLinkCount(int I, int J, int L)
{
    double ret = 0.0;
    int i, j, l;
    int router_num = routers.size();
    int link_num = links.size();
    for (i = 0; i < router_num; i++)
    {
        for (j = 0; j < router_num; j++)
        {
            if (i == j)
                continue;
            int count = 0;
            list<PathT*>::iterator itp = DP_ARRAY(i, j)->begin();
            for (; itp != DP_ARRAY(i, j)->end(); itp++)
            {
                list<PCENLink*>::iterator itl = (*itp)->path.begin();
                for (; itl != (*itp)->path.end(); itl++)
                {
                    if ((*itl)->linkID == L)
                        count++;
                }
            }
            if (i == I && j == J)
                ret -= (double)count;// substract link count;
            else
                ret += (double)count;// add link count;
        }
    }
    return ret;
}


