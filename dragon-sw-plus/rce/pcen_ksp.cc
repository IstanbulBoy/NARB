#include "pcen_ksp.hh"

PCENNode* PCEN_KSP::search_PCENNode(int NodeId) 
{
    for (unsigned int i=0; i<routers.size(); i++) 
    {
        if (routers[i]->ref_num==NodeId) 
            return routers[i];
    }
    return NULL;
}


void PCEN_KSP::Dijkstra(int source, int destination) 
{
    PCENNode* headnode= NULL;
    vector<PCENNode*>::iterator itNode;
    vector<PCENNode*> ReachableNodes;
    PCENNode* sourceNode=search_PCENNode(source);
    PCENNode* destNode=search_PCENNode(destination);
    sourceNode->nflg.nfg.visited=1; // We don't find shortest path to a node itself
    PCENNode* nextnode;
    list<PCENLink*>::iterator itLink;

    itLink=sourceNode->out_links.begin();
    while (itLink!=sourceNode->out_links.end()) 
    {
        if (((*itLink)->lflg.lfg.maskoff==1) || ((*itLink)->lflg.lfg.filteroff==1)) 
        {
            itLink++;
            continue;
        }
        nextnode=(*itLink)->rmt_end;
        if ((nextnode->nflg.nfg.filteroff==1) || (nextnode->nflg.nfg.maskoff==1)) 
        {
            itLink++;
            continue;
        }
        ReachableNodes.push_back(nextnode); // found path to this node
        nextnode->minCost=(*itLink)->PCENmetric();
        nextnode->path.push_back(*itLink);
        itLink++;
    }
	
    vector<PCENNode*>::iterator start;
    vector<PCENNode*>::iterator end;
    if (ReachableNodes.size()==0) 
    {
    	cout<<"No reachable Nodes has been found"<<endl;
    	return;
    }

    start = ReachableNodes.begin();
    end = ReachableNodes.begin();
    start++;
    while(start != ReachableNodes.end())
    {
        if( (*end)->minCost > (*start)->minCost )
            end = start;
        start++;
    }
    itNode = end;
    headnode=*itNode;
    headnode->nflg.nfg.visited=1; // shortest path to this node has been found
    ReachableNodes.erase(itNode); // shortest path found for this head node;

    for (;;) 
    {
        if (headnode==NULL) 
            break;

    	// Go through all the outgoing links for the newly added node
    	itLink=headnode->out_links.begin();
        while (itLink!=headnode->out_links.end()) {
            nextnode=(*itLink)->rmt_end;
            if ((nextnode->nflg.nfg.visited!=1) && (nextnode->nflg.nfg.maskoff!=1) &&
                (nextnode->nflg.nfg.filteroff!=1) &&
                (nextnode->minCost > headnode->minCost+((*itLink)->PCENmetric())) && 
                ((*itLink)->lflg.lfg.filteroff==0) && ((*itLink)->lflg.lfg.maskoff==0)) 
            {
                nextnode->minCost=(headnode->minCost+((*itLink)->PCENmetric()));
                bool b=false;
                vector<PCENNode*>::iterator itRNode;
                itRNode=ReachableNodes.begin();
                while ((!b) && (itRNode!=ReachableNodes.end())) 
                {
                    if ((*itRNode)==nextnode) b=true;
                        itRNode++;
                }
                if (!b)
                {
                    ReachableNodes.push_back(nextnode);
                }

                list<PCENLink*>::iterator itPath;
                itPath=(headnode->path).begin();
                nextnode->path.clear();
                while (itPath!=(headnode->path).end()) 
                {
                    nextnode->path.push_back(*itPath);
                    itPath++;
                }
                nextnode->path.push_back(*itLink);
            }
            itLink++;
        }

        if (ReachableNodes.size()==0) 
            break;
        itNode=min_element(ReachableNodes.begin(), ReachableNodes.end());
        headnode= *itNode;
        headnode->nflg.nfg.visited=1; // shortest path to this node has been found
        ReachableNodes.erase(itNode); // shortest path found for this head node;
    }  
}

void PCEN_KSP::AddNode(int nodeid) {
    PCENNode* pcenNode=new PCENNode(nodeid);
    pcenNode->auxvar1=0;
    pcenNode->auxvar2=0;
    pcenNode->minCost=PCEN_INFINITE_COST;
    pcenNode->nflg.flag=0;
    routers.push_back(pcenNode);
}


void PCEN_KSP::AddLink(int linkid, int localNodeId, int remoteNodeId) {
    PCENLink* pcenLink=new PCENLink(linkid, localNodeId, remoteNodeId, routers);
    pcenLink->auxvar1=0;
    pcenLink->auxvar2=0;
    links.push_back(pcenLink);
}

void PCEN_KSP::AddLink(int linkid, int localNodeId, int remoteNodeId, double metric) {
    PCENLink* pcenLink=new PCENLink(linkid, localNodeId, remoteNodeId, routers);
    pcenLink->auxvar1=0;
    pcenLink->auxvar2=0;
    links.push_back(pcenLink);
}

PCEN_KSP::~PCEN_KSP() {
    //clean KSP vector
    vector<PathT*>::iterator path_iter;
    for (path_iter = KSP.begin();  path_iter != KSP.end(); path_iter++)
        if (*path_iter)
            delete *path_iter;	
}

void PCEN_KSP::DisplayNodes() {
    vector<PCENNode*>::iterator node_iter;
    for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) 
    {
        LOG("Node: "<<(*node_iter)->ref_num<<" has been added with cost"<<(*node_iter)->minCost<<endl);
        LOG("This Node mask is: "<<(*node_iter)->nflg.nfg.maskoff<<endl);
    }
}

void PCEN_KSP::DisplayLinks() {
    vector<PCENLink*>::iterator link_iter;
    for (link_iter = links.begin();  link_iter != links.end(); link_iter++) {
        LOG("Link: "<<(*link_iter)->linkID<<" has been added."<<endl);
        LOG("Headnode: "<<(*link_iter)->lcl_end->ref_num<<" Tailnode: ");
        LOG((*link_iter)->rmt_end->ref_num<<endl);
        LOG("This link mask is "<<(*link_iter)->lflg.lfg.maskoff<<endl);
    }
}

void PCEN_KSP::OutputGraph() {
	LOG("Output Graph..."<<endl);
	vector<PCENNode*>::iterator node_iter;
	list<PCENLink*>::iterator link_it;
	
	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		LOG("Node: "<<(*node_iter)->ref_num<<". incoming links:");
		link_it=((*node_iter)->in_links).begin();
		while (link_it!=((*node_iter)->in_links).end()) {
			LOG((*link_it)->linkID<<" ");
			link_it++;
		}
		LOG(endl<<"Node: "<<(*node_iter)->ref_num<<". outgoing links:");
		link_it=(*node_iter)->out_links.begin();
		while (link_it!=(*node_iter)->out_links.end()) {
			LOG((*link_it)->linkID<<" ");
			link_it++;
		}
		LOG(endl<<endl);
	}
}

void PCEN_KSP::RestoreGraph() {
	vector<PCENLink*>::iterator link_iter;
	vector<PCENNode*>::iterator node_iter;

	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		(*node_iter)->nflg.flag=0L;
		(*node_iter)->path.clear();
		(*node_iter)->auxvar1=0;
		(*node_iter)->auxvar2=0;
		(*node_iter)->minCost=PCEN_INFINITE_COST;
	}
	for (link_iter = links.begin();  link_iter != links.end(); link_iter++) {
		(*link_iter)->lflg.flag=0L;
	}
}

// This function is to reset costs for all the nodes in this graph
void PCEN_KSP::ResetCost() {
	vector<PCENNode*>::iterator node_iter;
	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		(*node_iter)->minCost=PCEN_INFINITE_COST;
	}
}

void PCEN_KSP::RestoreGraphKeepFilter() {
	vector<PCENLink*>::iterator link_iter;
	vector<PCENNode*>::iterator node_iter;

	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		(*node_iter)->nflg.flag=(*node_iter)->nflg.nfg.filteroff;
		(*node_iter)->path.clear();
		(*node_iter)->auxvar1=0;
		(*node_iter)->auxvar2=0;
		(*node_iter)->minCost=PCEN_INFINITE_COST;
	}
	for (link_iter = links.begin();  link_iter != links.end(); link_iter++) {
		(*link_iter)->lflg.flag=(*link_iter)->lflg.lfg.filteroff;
	}
}

void PCEN_KSP::ResetVisitedFlag() {
	vector<PCENNode*>::iterator node_iter;
	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		(*node_iter)->nflg.nfg.visited=0;
	}
}

void PCEN_KSP::ClearPath() {
	vector<PCENNode*>::iterator node_iter;
	for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++) {
		(*node_iter)->path.clear();
	}
}


void PathT::CalculatePathCost() {
	list<PCENLink*>::iterator itLink;
	cost=0;
	for (itLink=path.begin(); itLink!=path.end(); itLink++) {
		cost+=(*itLink)->PCENmetric();
	}
}

int PathT::GetERO(list<ero_subobj>& ero)
{
    list<ero_subobj>::iterator addr_iter = ero.begin();
    ero.clear();

    list<PCENLink*>::iterator it;
    PCENLink * pcen_link;
    ero_subobj subobj1, subobj2;
    for (it = path.begin(); it != path.end(); it++)
    {
        pcen_link = *it;
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
 
        ero.push_back(subobj1);
        ero.push_back(subobj2);
    }
}

       
// This Search KSP is using the straightforward implementation of YEN's algorithm
// Due to data structure reason, implementation is not following the paper
// "A new implementation of YEN's ranking loopless paths algorithm"
// But we are following the algorithm described before section 4.1 in section 4
void PCEN_KSP::SearchKSP(int source, int destination, int K) 
{
    int KSPcounter=0; // So far, 0 path has been found
    // before searching, clear the KSP list
    this->KSP.clear();
    // X stores the paths found so far
    vector<PathT*> CandidatePaths;
    CandidatePaths.clear();

    list<PCENLink*>::iterator itLink;
    list<PCENLink*>::iterator pathstart;
    list<PCENLink*>::iterator pathend;
    list<PCENLink*>::iterator deviationstart;

    // find shortest path between source and any other node
    PCENNode* destNode=search_PCENNode(destination);
    PCENNode* sourceNode=search_PCENNode(source);

    // First line in YEN's
    this->Dijkstra(source, destination);
    PathT* nextpath=new PathT();
    (nextpath->path).assign(destNode->path.begin(),destNode->path.end());
    nextpath->cost=destNode->minCost;
    nextpath->DeviationNode=source;
    CandidatePaths.push_back(nextpath);
    KSP.push_back(nextpath);
    KSPcounter++;

    vector<PathT*>::iterator itPath;
    while ((CandidatePaths.size()>0) && (KSPcounter<=K)) 
    {
        this->RestoreGraphKeepFilter();
        itPath=min_element(CandidatePaths.begin(), CandidatePaths.end());
        PathT* headpath= *itPath;
        CandidatePaths.erase(itPath); 

        // headpath will be the parent for the next path
        //headpath->DisplayPath();
        if (KSPcounter>1) 
            KSP.push_back(headpath);
        if (KSPcounter==K) 
            return;

        // while we have not found d(pk), we just go ahead
        itLink=headpath->path.begin();
        while ((*itLink)->lcl_end->ref_num != headpath->DeviationNode) 
        {
            (*itLink)->lcl_end->MaskNode();
            itLink++;
        }
        pathend=headpath->path.end();
        //pathend--; // penultimate node along path pk
        for ( ; itLink!=pathend; itLink++) 
        {
            MaskParentPath(headpath);
            (*itLink)->MaskLink();
            this->ResetCost();
            this->ResetVisitedFlag();
            this->ClearPath();
            //this->DisplayLinks();
            this->Dijkstra((*itLink)->lcl_end->ref_num, destination);
            // find SPF from Vk_i to destination node
            if (destNode->path.size()>0) 
            {
                // concatenate subpk(s, vk_i) to shortest path found from vk_i to destination
                nextpath=new PathT();
                if (itLink!=headpath->path.begin()) 
                {
                    (nextpath->path).assign(headpath->path.begin(),itLink);
                    nextpath->DeviationNode=(*itLink)->lcl_end->ref_num;
                } 
                else 
                {
                    nextpath->DeviationNode=source;
                } 
                //(nextpath->path).assign(headpath->path.begin(),headpath->path.end());
                list<PCENLink*>::iterator halfpath;
                halfpath=destNode->path.begin();
                while (halfpath!=destNode->path.end())
                {
                    (nextpath->path).push_back(*halfpath);
                    halfpath++;
                }
                // calculate the path cost
                nextpath->CalculatePathCost();
                // mask the parent path and current path.
                (nextpath->MaskedLinkList).assign(headpath->MaskedLinkList.begin(),headpath->MaskedLinkList.end());
                (nextpath->MaskedLinkList).push_back(*itLink);
                //nextpath->DisplayPath();
                CandidatePaths.push_back(nextpath);
            }
            (*itLink)->RestoreLinkMask();
            // The local end of this itlink should be masked. Added 06/06/05
            (*itLink)->lcl_end->MaskNode();
        }


        KSPcounter++;
        //if (CandidatePaths.size()==0) 
        //    LOG("********** path exhausted..."<<endl);
    }
}

PathT::PathT() {
    this->cost=0;
    this->DeviationNode=-1;
    this->path.clear();
    this->MaskedLinkList.clear();
    this->pflg.flag=0;
}

void PathT::DisplayPath() {
    cout<<"Display Path Object: ";
    cout<<"Deviation Node: "<<this->DeviationNode<<endl;
    cout<<"path vector: ";
    list<PCENLink*>::iterator itLink=path.begin();
    while (itLink!=path.end()) 
    {
        cout<<(*itLink)->linkID<<" ";
        itLink++;
    }
    cout<<endl;
    cout<<"Marked Links: ";
    itLink=this->MaskedLinkList.begin();
    while (itLink!=MaskedLinkList.end()) 
    {
        cout<<(*itLink)->linkID<<" ";
        itLink++;
    }
    cout<<endl<<"Path cost: "<<this->cost<<endl;
}

void PCEN_KSP::MaskParentPath(PathT *ParentPath){
    list<PCENLink*>::iterator itLink;
    // Masking all the links masked before the parent path was searched.
    itLink=ParentPath->MaskedLinkList.begin();
    while (itLink != ParentPath->MaskedLinkList.end()) 
    {
        (*itLink)->MaskLink();
        itLink++;
    }
}

void PCEN_KSP::Run()
{
    int ret;
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

    PCENNode* src_node = GetNodeByIp(routers,&source);
    PCENNode* dest_node = GetNodeByIp(routers,&destination);
    assert (src_node && dest_node);

    SearchKSP(src_node->ref_num, dest_node->ref_num, 10);
    LOGF("Found %d shortest paths...\n", KSP.size());
    
    if (KSP.size() > 0)
    {
        KSP[0]->GetERO(ero);
        ReplyERO();
     }
}

