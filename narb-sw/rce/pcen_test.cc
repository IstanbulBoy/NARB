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

 #include "pcen_test.hh"

bool PCEN_TEST::PostBuildTopology()
{
    if (!PCEN_KSP::PostBuildTopology())
        return false;

    return true;
}

void PCEN_TEST::Run()
{
    int ret;
    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_TEST::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_TEST::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if (!PostBuildTopology())
    {
        LOGF("PCEN_TEST::PostBuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
        return;
    }

    PCENNode* src_node = GetNodeByIp(routers,&source);
    PCENNode* dest_node = GetNodeByIp(routers,&destination);
    assert (src_node && dest_node);

    PathT* bestPath = NULL;

    bestPath = SearchUpperLayerPath(src_node, dest_node);

    if (bestPath != NULL)
    {
        LOGF("Found an IP-layer path...\n");
        bestPath->GetERO(ero);
        ReplyERO();
        return;
    }

    bestPath = SearchLowerLayerPath(src_node, dest_node);
    
    if (bestPath != NULL)
    {
        LOGF("Found an IP-Optical-layer path...\n");
        bestPath->GetERO(ero);
        ReplyERO();
    }
    else
    {
        ReplyErrorCode(ERR_PCEN_NO_ROUTE);
    }
}

PathT* PCEN_TEST::SearchUpperLayerPath(PCENNode*  src_node, PCENNode*  dest_node)
{
    //cutting off crossing-layer links at source and destination
    list<PCENLink*>::iterator iterL;
    PCENLink* L;
    TSpec tspec;
    tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
    for (int i = 0; i < routers.size(); i++)
    {
        iterL=routers[i]->out_links.begin();
        while (iterL != routers[i]->out_links.end())
        {
            L = (*iterL);

            if (L->CrossingRegionBoundary(tspec))
            {
                cout << "Removing cross-layer link id: " << L->linkID << endl;
                L->rmt_end->in_links.remove(L);
                if (L->lcl_end == src_node || L->rmt_end == src_node || L->lcl_end == dest_node || L->rmt_end == dest_node)
                    links_backup.push_back(L);
                L->auxvar1 = 0xffffffff;
                iterL = src_node->out_links.erase(iterL);
            }
            else
                iterL++;
        }
    }
    vector<PCENLink*>::iterator iter = links.begin();
    while (iter != links.end())
    {
        if ((*iter)->auxvar1 == 0xffffffff)
        {
            (*iter)->auxvar1 = 0;
            iter = links.erase(iter);
        }
        else
            iter++;
    }

    Dijkstra(src_node->ref_num, dest_node->ref_num);

    if (dest_node->path.size() == 0)
        return NULL;

    PathT* nextpath=new PathT();
    (nextpath->path).assign(dest_node->path.begin(), dest_node->path.end());
    nextpath->cost= dest_node->minCost;
    KSP.push_back(nextpath);
    return nextpath;
}


PathT* PCEN_TEST::SearchLowerLayerPath(PCENNode*  src_node, PCENNode*  dest_node)
{
    list<PCENLink*>::iterator iterL;
    PCENLink* L;

    //recovering removed source and destination links
    while (links_backup.size() > 0)
    {
        L = links_backup.front();
        L->lcl_end->out_links.push_back(L);
        L->rmt_end->in_links.push_back(L);
        links.push_back(L);
        links_backup.pop_front();
    }
    
    //cutting off non-crossing-layer links at source and destination
    TSpec tspec;
    tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
    iterL=src_node->out_links.begin();
    while (iterL != src_node->out_links.end())
    {
        L = (*iterL);

        if (!L->CrossingRegionBoundary(tspec))
        {
            cout << "Removing source non-cross-layer link id: " << L->linkID << endl;
            L->rmt_end->in_links.remove(L);
            L->auxvar1 = 0xffffffff;
            iterL = src_node->out_links.erase(iterL);
        }
        else
            iterL++;
    }
    iterL=dest_node->out_links.begin();
    while (iterL != dest_node->out_links.end())
    {
        L = (*iterL);

        if (!L->CrossingRegionBoundary(tspec))
        {
            cout << "Removing destination non-cross-layer link id: " << L->linkID << endl;
            L->rmt_end->in_links.remove(L);
            L->auxvar1 = 0xffffffff;
            iterL = src_node->out_links.erase(iterL);
        }
        else
            iterL++;
    }

    vector<PCENLink*>::iterator iter = links.begin();
    while (iter != links.end())
    {
        if ((*iter)->auxvar1 == 0xffffffff)
        {
            (*iter)->auxvar1 = 0;
            iter = links.erase(iter);
        }
        else
            iter++;
    }

    SearchKSP(src_node->ref_num, dest_node->ref_num, SystemConfig::pce_k);
    LOGF("Found %d shortest paths...\n", KSP.size());

    return ConstrainKSPaths(KSP);
}


