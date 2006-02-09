#include "pcen_mrn_cg.hh"

void PCENCGNode::FilterCGNode()
{
	list<PCENCGLink*>::iterator itLink;
	itLink=out_CGlinks.begin();
	while (itLink!=out_CGlinks.end()) 
	{
		(*itLink)->lflg.lfg.filteroff=1;
//		cout<<"link "<<(*itLink)->linkID<<" is filtered out"<<endl;
		itLink++;
	}
	itLink=in_CGlinks.begin();
	while (itLink!=in_CGlinks.end()) 
	{
		(*itLink)->lflg.lfg.filteroff=1;
//		cout<<"link "<<(*itLink)->linkID<<" is filtered out"<<endl;
		itLink++;
	}
	this->nflg.nfg.filteroff=1;
//	cout<<"node "<<this->ref_num<<" is filtered out"<<endl;
}

void PCENCGNode::RestoreCGNodeFilter()
{
	list<PCENCGLink*>::iterator itLink;
	itLink=out_CGlinks.begin();
	while (itLink!=out_CGlinks.end()) {
		(*itLink)->lflg.lfg.filteroff=0;
		itLink++;
	}
	itLink=in_CGlinks.begin();
	while (itLink!=in_CGlinks.end()) {
		(*itLink)->lflg.lfg.filteroff=0;
		itLink++;
	}
	this->nflg.nfg.filteroff=0;
}

void PCENCGNode::MaskCGNode()
{
	list<PCENCGLink*>::iterator itLink;	
	itLink=out_CGlinks.begin();
	while (itLink!=out_CGlinks.end()) 
	{//mask all the out_links of this node
		(*itLink)->lflg.lfg.maskoff=1;
//		cout<<"Link "<<(*itLink)->lcl_end->ref_num<<"-"<<(*itLink)->rmt_end->ref_num<<" has been masked"<<endl;
		itLink++;
	}
	itLink=in_CGlinks.begin();
	while (itLink!=in_CGlinks.end()) 
	{//mask all the in_links of this node
		(*itLink)->lflg.lfg.maskoff=1;
//		cout<<"Link "<<(*itLink)->lcl_end->ref_num<<"-"<<(*itLink)->rmt_end->ref_num<<" has been masked"<<endl;
		itLink++;
	}
	this->nflg.nfg.maskoff=1;
}

void PCENCGNode::RestoreCGNodeMask()
{
	list<PCENCGLink*>::iterator itLink;
	itLink=out_CGlinks.begin();
	while (itLink!=out_CGlinks.end()) 
	{
		(*itLink)->lflg.lfg.maskoff=0;
		itLink++;
	}
	itLink=in_CGlinks.begin();
	while (itLink!=in_CGlinks.end()) 
	{
		(*itLink)->lflg.lfg.maskoff=0;
		itLink++;
	}
	this->nflg.nfg.maskoff=0;
}

PCENCGNode* PCENCGLink::search_PCENCGNode(vector<PCENCGNode*>& CGnodes,int NodeId)
{
	for (unsigned int i=0; i<CGnodes.size(); i++) {
		if (CGnodes[i]->ref_num==NodeId) return CGnodes[i];
	}
	return NULL;
}

PCENCGLink::PCENCGLink(int id, int localNodeId, int remoteNodeId, vector<PCENCGNode *>& CGnodes)
: PCENLink(id)
{
	lcl_end=search_PCENCGNode(CGnodes,localNodeId);
	lcl_end->out_CGlinks.push_back(this);
	rmt_end=search_PCENCGNode(CGnodes, remoteNodeId);
	rmt_end->in_CGlinks.push_back(this);
}

PCENCGNode* PCEN_MRN_CG::search_PCENCGNode(int nodeid)
{
	for (unsigned int i=0; i<CGnodes.size(); i++)
	{
		if(CGnodes[i]->ref_num == nodeid)
				return CGnodes[i];
	}
	return NULL;
}

PCENCGNode* PCEN_MRN_CG::search_PCENCGNode(int lclID, int rmtID, u_char swtype)
{
	for (unsigned int i=0; i<CGnodes.size(); i++)
	{
		if ((CGnodes[i]->lcl_endID == lclID) &&(CGnodes[i]->rmt_endID == rmtID)&&(CGnodes[i]->swType==swtype)) 
			return CGnodes[i];
	}
	return NULL;
}

void PCEN_MRN_CG::DijkstraMRN(int source, int destination)
{
	PCENCGNode* headnode= new PCENCGNode();
	int headnodeID;

	vector<PCENCGNode*>::iterator itNode;
	
	vector<PCENCGNode*> ReachableNodes;
	PCENCGNode* sourceNode=search_PCENCGNode(source);
	PCENCGNode* destNode=search_PCENCGNode(destination);
//	cout<<"source node id: "<<sourceNode->ref_num<<" destination node id : "<<destNode->ref_num<<endl;
	sourceNode->nflg.nfg.visited=1; // We don't find shortest path to a node itself
	PCENCGNode* nextnode;
	list<PCENCGLink*>::iterator itLink;
	
	itLink=sourceNode->out_CGlinks.begin();
	while (itLink!=sourceNode->out_CGlinks.end()) 
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
//		cout<<"rmt_end node ID of the source node"<<nextnode->ref_num<<endl;
		ReachableNodes.push_back(nextnode); // found path from source to this node
		nextnode->minCost=(*itLink)->PCENmetric();
		nextnode->path_mrn.push_back(*itLink);
		itLink++;
    }
	
	vector<PCENCGNode*>::iterator start;
	vector<PCENCGNode*>::iterator end;
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
		if (headnode==NULL) break;

		// Go through all the outgoing links for the newly added node
		itLink=headnode->out_CGlinks.begin();
		while (itLink != headnode->out_CGlinks.end()) 
		{
			nextnode = (*itLink)->rmt_end;
			if ( (nextnode->nflg.nfg.visited != 1) && (nextnode->nflg.nfg.maskoff != 1) &&
				(nextnode->nflg.nfg.filteroff != 1) &&
				(nextnode->minCost > headnode->minCost + ((*itLink)->PCENmetric())) && 
				((*itLink)->lflg.lfg.filteroff == 0) && 
				((*itLink)->lflg.lfg.maskoff == 0)) 
			{
				nextnode->minCost = (headnode->minCost+((*itLink)->PCENmetric()));
				bool b = false;
				vector<PCENCGNode*>::iterator itRNode;

				//if nextnode is not in ReachableNodes list, add it in
				itRNode = ReachableNodes.begin();
				while ((!b) && (itRNode!=ReachableNodes.end())) 
				{
					if ((*itRNode)==nextnode) b=true;
					itRNode++;
				}
				if (!b) 
				{
					ReachableNodes.push_back(nextnode);
//					cout<<"new node found: "<<nextnode->ref_num<<endl;
				}

				list<PCENCGLink*>::iterator itPath;
				itPath=(headnode->path_mrn).begin();
				nextnode->path_mrn.clear();
				while (itPath!=(headnode->path_mrn).end()) 
				{
					nextnode->path_mrn.push_back(*itPath);
					itPath++;
				}
				nextnode->path_mrn.push_back(*itLink);
			}
			itLink++;
		}

		if (ReachableNodes.size()==0) break;
	
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
		headnode= *itNode;
		headnode->nflg.nfg.visited=1; // shortest path to this node has been found

		ReachableNodes.erase(itNode); // shortest path found for this head node;
//		cout<<"after erasure, headnode:  "<<headnode->ref_num<<endl;
		headnodeID=headnode->ref_num;

		if(headnodeID == destination)
			return;
	} 
}

void PCEN_MRN_CG::AddCGNode (int nodeid,int lclID,int rmtID,u_char swtype,u_char encoding,float bandwidth,double vMetric)
{
	PCENCGNode* pceCGNode = new PCENCGNode(nodeid);
	pceCGNode->auxvar1=0;
	pceCGNode->auxvar2=0;
	pceCGNode->minCost=PCEN_INFINITE_COST;
	pceCGNode->nflg.flag=0;
	pceCGNode->lcl_endID = lclID;
	pceCGNode->rmt_endID = rmtID;
	pceCGNode->swType = swtype;
	pceCGNode->encoding = encoding;
	pceCGNode->bandwidth = bandwidth;
	pceCGNode->LinkMetric = vMetric;
	CGnodes.push_back(pceCGNode);
	
}

void PCEN_MRN_CG::AddCGLink(int linkid, int localNodeId, int remoteNodeId, double metric, int passNode)
{
	PCENCGLink* pceNCGLink=new PCENCGLink(linkid, localNodeId, remoteNodeId, CGnodes);
	pceNCGLink->auxvar1=passNode;
	pceNCGLink->auxvar2=0;
	pceNCGLink->SetPCENmetric(metric);
	CGlinks.push_back(pceNCGLink);
}

void PCEN_MRN_CG:: AddLink(int linkid, int localNodeId, int remoteNodeId, double metric, u_char swtype1,u_char encoding,float bandwidth)
    {
	PCENLink* pceLink=new PCENLink(linkid, localNodeId, remoteNodeId, routers);
	ISCD* iscd = new (ISCD);
	pceLink->auxvar1=0;
	pceLink->auxvar2=0;
	pceLink->SetPCENmetric(metric);
	iscd->swtype = swtype1;
	iscd->encoding = encoding;
	for(unsigned int i=0;i<7;i++)
	{
		iscd->max_lsp_bw[i] = 0.0;
	}
	iscd->max_lsp_bw[7] = bandwidth;
	pceLink->link->Iscds().push_back(iscd);
	links.push_back(pceLink);	
}

void PCEN_MRN_CG::AddLink(int linkid, int localNodeId, int remoteNodeId, double metric, u_char swtype1,u_char encoding1,float bandwidth1,u_char swtype2,u_char encoding2,float bandwidth2, bool adapt)
{
	PCENLink* pceLink=new PCENLink(linkid, localNodeId, remoteNodeId, routers);
	ISCD* iscd = new (ISCD);
	pceLink->auxvar1=0;
	pceLink->auxvar2=0;
	pceLink->SetPCENmetric(metric);
	iscd->swtype = swtype1;
	iscd->encoding = encoding1;
	for(unsigned int i=0;i<7;i++)
	{
		iscd->max_lsp_bw[i] = 0.0;
	}
	iscd->max_lsp_bw[7] = bandwidth1;
	pceLink->link->Iscds().push_back(iscd);
	iscd=new (ISCD);
	iscd->swtype = swtype2;
	iscd->encoding = encoding2;
	for(unsigned int i=0;i<7;i++)
	{
		iscd->max_lsp_bw[i] = 0.0;
	}
	iscd->max_lsp_bw[7] = bandwidth2;
	pceLink->link->Iscds().push_back(iscd);
	if(adapt)
	{
		IACD* iacd = new (IACD);
		iacd->swtype_lower = swtype1;
		iacd->encoding_lower = encoding1;
		iacd->swtype_upper = '1';
		iacd->encoding_upper = encoding2;
		pceLink->link->Iacds().push_back(iacd);
	}
	
	links.push_back(pceLink);	
}

PCEN_MRN_CG::PCEN_MRN_CG(in_addr src, in_addr dest, u_int8_t sw_type, u_int8_t encoding, 
    float bw, u_int32_t opts, u_int32_t lspq_id, u_int32_t msg_seqnum):
    PCEN_KSP(src, dest, sw_type, encoding, bw, opts, lspq_id, msg_seqnum) 
{

}

PCEN_MRN_CG::~PCEN_MRN_CG() {
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

    //clean CGnodes list
    vector<PCENCGNode*>::iterator itNode;
    for(itNode = CGnodes.begin(); itNode != CGnodes.end(); itNode++)
    	if(*itNode)
    		delete *itNode;
    vector<PCENCGLink*>:: iterator itLink;
    for(itLink = CGlinks.begin(); itLink != CGlinks.end(); itLink++)
    	if(*itLink)
    		delete *itLink;
    vector<PathT_MRN*>:: iterator path_mrn;
    for(path_mrn = KSP_MRN.begin();path_mrn != KSP_MRN.end(); path_mrn++)
    	if(*path_mrn)
    		delete *path_mrn;	
}

void PCEN_MRN_CG::DisplayCGNodes()
{
	vector<PCENCGNode*>::iterator node_iter;
	list<PCENCGLink*>::iterator link_iter;
	for(node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		cout<<"node: "<<(*node_iter)->ref_num<<" has been added, local end "<<(*node_iter)->lcl_endID<<" and rmt end "<<(*node_iter)->rmt_endID<<endl;
		cout<<"the switching type is: "<<(*node_iter)->swType<<endl;
		cout<<"the metric of this node is "<<(*node_iter)->LinkMetric<<endl;
		cout<<"Outgoing link list is: ";
		link_iter = (*node_iter)->out_CGlinks.begin();
		while (link_iter!=(*node_iter)->out_CGlinks.end()) 
		{
			cout<<" "<<(*link_iter)->rmt_end->ref_num;
			link_iter++;
		}
		cout<<" Ingoing link list is: ";
		link_iter= (*node_iter)->in_CGlinks.begin();
		while(link_iter != (*node_iter)->in_CGlinks.end())
		{
			cout<<" "<<(*link_iter)->lcl_end->ref_num;
			link_iter++;
		}		
		cout<<endl;
	}
}

void PCEN_MRN_CG::DisplayCGLinks()
{
	vector<PCENCGLink*> :: iterator link_iter;
	for(link_iter = CGlinks.begin(); link_iter != CGlinks.end(); link_iter++)
	{
		cout<<"Link: "<<(*link_iter)->linkID<<" has been added."<<endl;
		cout<<"Headnode: "<<(*link_iter)->lcl_end->ref_num<<" Tailnode: ";
		cout<<(*link_iter)->rmt_end->ref_num<<" maskoff: "<<(*link_iter)->lflg.lfg.maskoff<<" filteroff "<<(*link_iter)->lflg.lfg.filteroff<<endl;
		cout<<"The pcemetric of this link is : "<<(*link_iter)->PCENmetric()<<endl;
	}
}
    
void PCEN_MRN_CG::RestoreCGGraph()
{
	vector<PCENCGNode*>::iterator node_iter;
	vector<PCENCGLink*>::iterator link_iter;
	
	for (node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		(*node_iter)->nflg.flag=0L;
		(*node_iter)->path_mrn.clear();
		(*node_iter)->auxvar2=0;
		(*node_iter)->minCost = PCEN_INFINITE_COST;
	}
	for(link_iter = CGlinks.begin(); link_iter != CGlinks.end(); link_iter++)
	{
		(*link_iter)->lflg.flag=0L;
	}

}

void PCEN_MRN_CG::ResetCGCost()
{
	vector<PCENCGNode*>::iterator node_iter;
	for(node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		(*node_iter)->minCost = PCEN_INFINITE_COST;
	}
}

void PCEN_MRN_CG::ResetCGVisitedFlag()
{
	vector<PCENCGNode*>::iterator node_iter;
	for(node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		(*node_iter)->nflg.nfg.visited=0;
	}
}

void PCEN_MRN_CG::ClearCGPath()
{
	vector<PCENCGNode*>::iterator node_iter;
	for(node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		(*node_iter)->path_mrn.clear();
	}
}

void PathT_MRN::CalculatePathCost_MRN()
{
	list<PCENCGLink*>::iterator itLink;
	cost = 0;
	for(itLink = path_mrn.begin(); itLink != path_mrn.end();itLink++)
	{
		cost+= (*itLink)->lcl_end->LinkMetric;
	}
}

void PCEN_MRN_CG::DeleteVirtualNodes()
{
	vector<PCENCGNode*>::iterator itNode;
	vector<PCENCGLink*>::iterator itLink;
	itLink = CGlinks.begin();
	while(itLink != CGlinks.end())
	{
		if((*itLink)->lcl_end->lcl_endID == (*itLink)->lcl_end->rmt_endID || (*itLink)->rmt_end->lcl_endID == (*itLink)->rmt_end->rmt_endID)
		{
			vector<PCENCGLink*>::iterator it;
			it = itLink;
			it--;
			CGlinks.erase(itLink);
			itLink = it;
		}
		itLink++;
	}
	cout<<endl;
	itNode = CGnodes.begin();
	while(itNode != CGnodes.end())
	{
		if((*itNode)->lcl_endID == (*itNode)->rmt_endID)
		{
			vector<PCENCGNode*>::iterator it;
			it = itNode;
			it--;
			CGnodes.erase(itNode);
			itNode = it;
		}
		itNode++;
	}
	cout<<endl;
}

bool PCEN_MRN_CG::BuildEndNodes(int source, int end, u_char swtype,u_char encoding,float bandwidth)//changed 07/14
{
	vector<PCENCGNode*>::iterator cgnode_iter;
	int sourceid,endid,linkID;
	bool valid_source = false;
	bool valid_end = false;
	
	sourceid = (int)CGnodes.size() + 1;
	endid = (int)CGnodes.size() + 2;
	linkID = (int)CGlinks.size()+1;
	
	//add virtual node for source and destination 
	this->AddCGNode(sourceid,source,source,'0','0',0,0);
	this->AddCGNode(endid,end,end,'0','0',0,0);
	
	//add virtual links for virtual nodes
	cgnode_iter = CGnodes.begin();
	while(cgnode_iter != CGnodes.end())
	{
		if(source == (*cgnode_iter)->lcl_endID && swtype == (*cgnode_iter)->swType && encoding == (*cgnode_iter)->encoding && (*cgnode_iter)->bandwidth >= bandwidth)
		{
			this->AddCGLink(linkID,sourceid,(*cgnode_iter)->ref_num,(*cgnode_iter)->LinkMetric,(*cgnode_iter)->lcl_endID);
			cout<<"Add virtual link from : "<<sourceid<<" to : "<<(*cgnode_iter)->ref_num<<endl;
			linkID++;
			valid_source = true;
		}

		if(end == (*cgnode_iter)->rmt_endID && end!= (*cgnode_iter)->lcl_endID && encoding == (*cgnode_iter)->encoding && (*cgnode_iter)->bandwidth >= bandwidth)
		{
			this->AddCGLink(linkID,(*cgnode_iter)->ref_num,endid,0,(*cgnode_iter)->rmt_endID);
			cout<<"Add virtual link from : "<<endid<<" to : "<<(*cgnode_iter)->ref_num<<endl;
			linkID++;
			valid_end = true;
		}
		cgnode_iter++;
	}
	
	if( !valid_source || !valid_end )
	{
		cout<<"Can't find valid source or destination node!"<<endl;
		return false;
	}
//	cout<<"display channel graph links"<<endl;
	this->DisplayCGNodes();
//	this->DisplayCGLinks();
	return true;
}

void PCEN_MRN_CG::SearchMRNKSP(int source, int destination, u_char swtype, u_char encoding, float bandwidth, int K)
{
	this->CreateChannelGraph(bandwidth);

	//source and destination is in network graph
	if( !(this->BuildEndNodes(source,destination,swtype,encoding,bandwidth)) )//changed 07/14
		return;
	this->RestoreCGGraph();
//	this->DisplayCGNodes();
	source = (int)CGnodes.size()-1;
	destination = (int)CGnodes.size();
	
	int KSPcounter=0; // So far, 0 path has been found
	// before searching, clear the KSP_MRN list
	this->KSP_MRN.clear();
	// X stores the paths found so far
	vector<PathT_MRN*> CandidatePaths;
	CandidatePaths.clear();

	list<PCENCGLink*>::iterator itLink;
	list<PCENCGLink*>::iterator pathstart;
	list<PCENCGLink*>::iterator pathend;
	list<PCENCGLink*>::iterator deviationstart;
	PCENCGNode* destNode = NULL;
	PCENCGNode* sourceNode = NULL;
		
	// find shortest path between source and any other node
	destNode = search_PCENCGNode(destination);
	sourceNode = search_PCENCGNode(source);
	
	if((sourceNode == NULL) || (destNode == NULL))
	{
		cout<<"Can't find the source node or destination node"<<endl;
		return;
	}

	// First line in YEN's
	this->DijkstraMRN(source, destination);
	PathT_MRN* nextpath=new PathT_MRN();
	
	if(destNode->path_mrn.size() == 0)
	{
		cout<<"Can't find one shortest path from source to destination"<<endl;
		return;
	}

	(nextpath->path_mrn).assign(destNode->path_mrn.begin(),destNode->path_mrn.end());
	nextpath->cost = destNode->minCost;
	nextpath->DeviationNode = source;
	CandidatePaths.push_back(nextpath);
	KSP_MRN.push_back(nextpath);//the first shortest path
	KSPcounter++;

	vector<PathT_MRN*>::iterator itPath,secPath;//added by qian
	PathT_MRN* headpath;
	while ((CandidatePaths.size()>0) && (KSPcounter<=K)) 
	{
		this->RestoreCGGraph();
		//added by qian to find a shortest path from the candidate path list and save as the headpath
		itPath = CandidatePaths.begin();
		secPath = CandidatePaths.begin();
		itPath++;
		while(itPath!=CandidatePaths.end())
		{
			if( (*secPath)->cost > (*itPath)->cost )
				secPath = itPath;
				//headpath = (*itPath);
			itPath++;
		}
		headpath = (*secPath);
		itPath = secPath;
		
	//	PathT* headpath= *itPath;
		CandidatePaths.erase(itPath); 

		// headpath will be the parent for the next path
		cout<<"****************** headpath is: ";
		headpath->DisplayPath_MRN();
		if (KSPcounter>1) KSP_MRN.push_back(headpath);
		if (KSPcounter==K) return;

		// while we have not found d(pk), we just go ahead
//		cout<<"The deviation node of this path is node "<<headpath->DeviationNode<<endl;
		itLink=headpath->path_mrn.begin();
		while((*itLink)->lcl_end->ref_num != headpath->DeviationNode)
		{
//			cout<<"The head node of this link is node "<<(*itLink)->lcl_end->ref_num<<endl;
			vector<PCENCGNode*>::iterator mask;
			mask = CGnodes.begin();
			while(mask != CGnodes.end())
			{
				if((*mask)->lcl_endID == (*itLink)->lcl_end->lcl_endID && (*mask)->swType == (*itLink)->lcl_end->swType)
					(*mask)->MaskCGNode();
				if((*mask)->lcl_endID == (*itLink)->lcl_end->rmt_endID && (*mask)->swType == (*itLink)->lcl_end->swType)
					(*mask)->MaskCGNode();
				mask++;
			}
//			(*itLink)->lcl_end->MaskCGNode();
//			cout<<"node "<<(*itLink)->lcl_end->ref_num<<"is masked.";
			itLink++;
		}

		pathend=headpath->path_mrn.end();
		pathend--;
		//pathend--; // penultimate node along path pk
		for ( ; itLink!=pathend; itLink++) 
		{
			//mask the link from deviation node to the end node and try to find the shortest instead
			MaskParentPath_MRN(headpath);
			(*itLink)->MaskLink();
	
			this->ResetCGCost();
			this->ResetCGVisitedFlag();
			this->ClearCGPath();

			cout<<"Finding path from "<<(*itLink)->lcl_end->ref_num<<" to destination node: "<<destination<<endl;
			//mask node with same lcl_endID and swtype with deviation node.
			vector<PCENCGNode*>::iterator cgnode;
			cgnode = CGnodes.begin();
			while(cgnode != CGnodes.end())
			{
				if((*cgnode)->lcl_endID == (*itLink)->lcl_end->lcl_endID && (*cgnode)->swType == (*itLink)->lcl_end->swType && (*cgnode)->rmt_endID != (*itLink)->lcl_end->rmt_endID)
					(*cgnode)->MaskCGNode();
				cgnode++;
			}

			this->ShowMarkedLinks();
			this->DijkstraMRN((*itLink)->lcl_end->ref_num, destination);
			// find SPF from Vk_i to destination node

			if(destNode->path_mrn.size() == 0)
			{
				cout<<"Can't find path from "<<(*itLink)->lcl_end->ref_num<<" to destination node: "<<destination<<endl;
			}
			if (destNode->path_mrn.size()>0) 
			{
				// concatenate subpk(s, vk_i) to shortest path found from vk_i to destination
				PathT_MRN* nextpath=new PathT_MRN();
				if (itLink!=headpath->path_mrn.begin()) 
				{
					//deviationstart=--itLink;
					//(nextpath->path).assign(headpath->path.begin(),deviationstart);
					(nextpath->path_mrn).assign(headpath->path_mrn.begin(),itLink);
					// (nextpath->path).pop_back();
					
					nextpath->DeviationNode=(*itLink)->lcl_end->ref_num;
				} 
				else 
				{
					nextpath->DeviationNode=source;
				} 
				//(nextpath->path).assign(headpath->path.begin(),headpath->path.end());
				list<PCENCGLink*>::iterator halfpath;
				halfpath=destNode->path_mrn.begin();
				while (halfpath!=destNode->path_mrn.end())
				{
					(nextpath->path_mrn).push_back(*halfpath);
					halfpath++;
				}
				//check loop
				list<PCENCGLink*>::iterator link1,link2,last;
				bool find_loop = false;
				last = nextpath->path_mrn.end();
				last--;
				for(link1 = nextpath->path_mrn.begin(); link1 != last; link1++)
				{
					link2 = link1;	link2++;
					for(;link2 != nextpath->path_mrn.end();link2++)
					{
						if((*link1)->lcl_end->rmt_endID == (*link2)->lcl_end->rmt_endID && (*link1)->lcl_end->swType == (*link2)->lcl_end->swType)
						{
							find_loop = true;
							cout<<"Find loop path: "<<endl;
							nextpath->DisplayPath_MRN();
						}
					}
				}

				// calculate the path cost
				if(!find_loop)
				{	nextpath->CalculatePathCost_MRN();
				// mask the parent path and current path.
					(nextpath->MaskedLinkList_mrn).assign(headpath->MaskedLinkList_mrn.begin(),headpath->MaskedLinkList_mrn.end());
					(nextpath->MaskedLinkList_mrn).push_back(*itLink);
					nextpath->DisplayPath_MRN();
					CandidatePaths.push_back(nextpath);
				}
			}
			(*itLink)->RestoreLinkMask();
			// The local end of this itlink should be masked. Added 06/06/05
			vector<PCENCGNode*>::iterator mask1;
			mask1 = CGnodes.begin();
			while(mask1 != CGnodes.end())
			{
				if((*mask1)->lcl_endID == (*itLink)->lcl_end->lcl_endID && (*mask1)->swType == (*itLink)->lcl_end->swType)
					(*mask1)->MaskCGNode();
				
				mask1++;
			}
		}


		KSPcounter++;
		if (CandidatePaths.size()==0) {
			cout<<"********** path exhausted..."<<endl;
		}
	}
	this->DeleteVirtualNodes();
}

PathT_MRN::PathT_MRN()
{
	this->path_mrn.clear();
	this->MaskedLinkList_mrn.clear();
}

void PathT_MRN::DisplayPath_MRN()
{
	cout<<"Display Path Object: ";
	cout<<"Deviation Node: "<<this->DeviationNode<<endl;
	cout<<"path vector: ";
	int i = 0;
	list<PCENCGLink*>::iterator itLink=path_mrn.begin();
	while (itLink!=path_mrn.end()) 
	{
		if(i == 0)
			cout<<(*itLink)->auxvar1;
		else
			cout<<" ("<<(*itLink)->lcl_end->swType<<") "<<(*itLink)->auxvar1;
	//	cout<<" local end: "<<(*itLink)->lcl_end->ref_num<<" remote end: "<<(*itLink)->rmt_end->ref_num;
		i++;
		itLink++;
	}
	cout<<endl;

	cout<<"Link list is: ";
	itLink = path_mrn.begin();
	while(itLink != path_mrn.end())
	{
		cout<<(*itLink)->lcl_end->ref_num<<"-"<<(*itLink)->rmt_end->ref_num<<" ";
		itLink++;
	}
	cout<<endl;

	cout<<"Marked Links: ";
	itLink=this->MaskedLinkList_mrn.begin();
	while (itLink!=MaskedLinkList_mrn.end()) 
	{
		cout<<(*itLink)->lcl_end->ref_num<<"_"<<(*itLink)->rmt_end->ref_num<<" ";
		itLink++;
	}
	cout<<endl<<"Path cost: "<<this->cost<<endl;
}

void PCEN_MRN_CG::MaskParentPath_MRN(PathT_MRN* ParentPath)
{
	list<PCENCGLink*>::iterator itLink;
	if(ParentPath->MaskedLinkList_mrn.size() == 0)
		return;
	itLink = ParentPath->MaskedLinkList_mrn.begin();
	while (itLink != ParentPath->MaskedLinkList_mrn.end())
	{
		(*itLink)->MaskLink();
		itLink++;
	}
}

void PCEN_MRN_CG::OutputKSP_MRN()
{
	vector<PathT_MRN*>::iterator itKSP;
	cout<<"In total,"<<(int)KSP_MRN.size()<<" path(s) found. "<<endl;
	itKSP=KSP_MRN.begin();
	while(itKSP!= KSP_MRN.end())
	{
		(*itKSP)->DisplayPath_MRN();
		itKSP++;
	}
}

void PCEN_MRN_CG::ShowMarkedLinks()
{	//added by qian
	cout<<"Show masked link and node ..."<<endl;
	vector<PCENCGNode*> :: iterator itNode;
	vector <PCENCGLink*> :: iterator itLink;
	itLink = CGlinks.begin();
	while (itLink != CGlinks.end())
	{
		if((*itLink)->lflg.lfg.maskoff == 1)
			cout<<(*itLink)->lcl_end->ref_num<<"-"<<(*itLink)->rmt_end->ref_num<<" ";
		itLink++;
	}
	itNode = CGnodes.begin();
	while(itNode != CGnodes.end())
	{
		if((*itNode)->nflg.nfg.maskoff == 1)
			cout<<(*itNode)->ref_num<<" ";
		itNode++;
	}
	cout<<endl;

}


void PCEN_MRN_CG::CreateChannelGraph(float bandwidth)//changed 07/14
{
	list<ISCD*>::iterator iscd_iter,iscd_in,iscd_out;
	list<IACD*>::iterator iacd_iter;
	list <PCENLink*> :: iterator inlink,outlink;
	vector <PCENLink*> :: iterator itLink;
	vector <PCENNode*> :: iterator itNode;
	PCENCGNode* start;
	PCENCGNode* tail;
	PCENLink* oneLink;
	PCENLink* reverse;
	int id = 1;
			
	//for the simplest case, consider the bandwidth on each link's iscd->max_lsp_bw[7] must >= the requested bandwidth
	//add nodes in channel graph
	itLink = links.begin();
	while (itLink!= links.end())
	{
		iscd_iter = (*itLink)->link->Iscds().begin();
		while(iscd_iter != (*itLink)->link->Iscds().end())
		{
			if((*iscd_iter)->max_lsp_bw[7] >= bandwidth)
			{
				this->AddCGNode(id,(*itLink)->lcl_end->ref_num,(*itLink)->rmt_end->ref_num,(*iscd_iter)->swtype,(*iscd_iter)->encoding,(*iscd_iter)->max_lsp_bw[7],(*itLink)->PCENmetric());
//				cout<<"Channel grpah node : "<<id<<" with local_end "<<(*itLink)->lcl_end->ref_num<<" and rmt_end "<<(*itLink)->rmt_end->ref_num<<"  and swtype "<<(*iscd_iter)->swtype<<endl;
				iscd_iter++;
				id++;
			}
		}
		itLink++;
	}

	//add links in channel graph
	id = 1;
	itNode = routers.begin();
	while(itNode != routers.end())
	{
//		cout<<"check node : "<<(*itNode)->ref_num<<endl;
		for(inlink = (*itNode)->in_links.begin(); inlink !=(*itNode)->in_links.end(); inlink++)
		{
			for(iscd_in = (*inlink)->link->Iscds().begin(); iscd_in != (*inlink)->link->Iscds().end(); iscd_in++)
			{
				for(outlink = (*itNode)->out_links.begin(); outlink != (*itNode)->out_links.end();outlink++)
				{
					if((*inlink)->lcl_end->ref_num == (*outlink)->rmt_end->ref_num && (*inlink)->rmt_end->ref_num == (*outlink)->lcl_end->ref_num)
						continue;
					iscd_out = (*outlink)->link->Iscds().begin();
					while(iscd_out != (*outlink)->link->Iscds().end())
					{
//						cout<<"inlink "<<(*inlink)->linkID<<" with swtype "<<(*iscd_in)->swtype<<" outlink "<<(*outlink)->linkID<<" with swtype"<<(*iscd_out)->swtype<<endl;
						if((*iscd_in)->swtype == (*iscd_out)->swtype)
						{
							start = search_PCENCGNode((*inlink)->lcl_end->ref_num,(*inlink)->rmt_end->ref_num,(*iscd_in)->swtype);
							tail = search_PCENCGNode((*outlink)->lcl_end->ref_num,(*outlink)->rmt_end->ref_num,(*iscd_out)->swtype);
							if(start && tail)
							{
								this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric, start->rmt_endID);
//								cout<<"linkID : "<<id<<"find same swtype, link start from "<<start->ref_num<<" and end with "<<tail->ref_num<<" by pass node "<<start->rmt_endID<<endl;
								id++;
							}
						}
						iscd_out++;
					}
				}
			}
		}
		itNode++;
	}
	
	//add adaptation link
	PCENNode* adaptNode;
	list<PCENLink*>::iterator out;
	u_char adapt_swtype1,adapt_swtype2;
	bool adapt = false;

	itLink = links.begin();
	while(itLink != links.end())
	{
		oneLink = (*itLink);
		reverse = this->search_PCENLink(oneLink->rmt_end->ref_num, oneLink->lcl_end->ref_num);

		//if both links have only one iscd
		if(reverse != NULL && oneLink->link->Iscds().size() == 1 && reverse->link->Iscds().size() == 1 && oneLink->link->Iscds().front()->swtype != reverse->link->Iscds().front()->swtype)
		{
			start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,oneLink->link->Iscds().front()->swtype);
			adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
			out = adaptNode->out_links.begin();
			while(out != adaptNode->out_links.end())
			{
				if((*out) != reverse)
				{
					iscd_iter = (*out)->link->Iscds().begin();
					while(iscd_iter != (*out)->link->Iscds().end())
					{
						if((*iscd_iter)->swtype == reverse->link->Iscds().front()->swtype)
						{
							tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_iter)->swtype);
							this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
//							cout<<"linkID : "<<id<<"find same swtype, link start from "<<start->ref_num<<" and end with "<<tail->ref_num<<" by pass node "<<start->rmt_endID<<endl;
							id++;
						}
						iscd_iter++;
					}
				}
				out++;
			}
		}

		//if the IACD of oneLink is not empty
		if(reverse != NULL && oneLink->link->Iacds().size()> 0)
		{
			for(iscd_iter = oneLink->link->Iscds().begin();iscd_iter != oneLink->link->Iscds().end();iscd_iter++)
			{
				for(iacd_iter = oneLink->link->Iacds().begin();iacd_iter != oneLink->link->Iacds().end();iacd_iter++)
				{
					if((*iscd_iter)->swtype == (*iacd_iter)->swtype_lower)
					{
						adapt_swtype1 = (*iscd_iter)->swtype;
						adapt_swtype2 = (*iacd_iter)->swtype_upper;
						adapt = true;
					}
					else if((*iscd_iter)->swtype == (*iacd_iter)->swtype_upper)
					{
						adapt_swtype1 = (*iscd_iter)->swtype;
						adapt_swtype2 = (*iacd_iter)->swtype_lower;
						adapt = true;
					}
					if(adapt)
					{
						start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,adapt_swtype1);
						adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
						for(out = adaptNode->out_links.begin();out != adaptNode->out_links.end();out++)
						{
							if((*out) != reverse)
							{
								for(iscd_out = (*out)->link->Iscds().begin(); iscd_out != (*out)->link->Iscds().end();iscd_out++)
								{
									if((*iscd_out)->swtype == adapt_swtype2)
									{
										tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_out)->swtype);
										this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
										id++;
									}
								}
							}
						}
					
					}
					adapt = false;
				}

			}
		}
		else if (reverse != NULL && oneLink->link->Iacds().size() == 0 && reverse->link->Iacds().size()>0)
		{
			adapt_swtype1 = oneLink->link->Iscds().front()->swtype;
			for(iacd_iter = reverse->link->Iacds().begin();iacd_iter != reverse->link->Iacds().end();iacd_iter++)
			{
				if((*iacd_iter)->swtype_lower == adapt_swtype1)
				{
					adapt_swtype2 = (*iacd_iter)->swtype_upper;
					adapt = true;
				}
				else if((*iacd_iter)->swtype_upper == adapt_swtype1)
				{
					adapt_swtype2 = (*iacd_iter)->swtype_lower;
					adapt = true;
				}
				if(adapt)
				{
					start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,adapt_swtype1);
					adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
					for(out = adaptNode->out_links.begin();out != adaptNode->out_links.end();out++)
					{
						if((*out) != reverse)
						{
							for(iscd_out = (*out)->link->Iscds().begin(); iscd_out != (*out)->link->Iscds().end();iscd_out++)
							{
								if((*iscd_out)->swtype == adapt_swtype2)
								{
									tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_out)->swtype);
									this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
									id++;
								}
							}
						}
					}
				}
				adapt = false;
			}
		}

		itLink++;
	}
	
	cout<<"channel graph is created"<<endl;
	this->DisplayCGNodes();
//	this->DisplayCGLinks();
}


void PCEN_MRN_CG::Run()
{
    int ret;
    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_MRN_CG::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN_MRN_CG::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    PCENNode* src_node = GetNodeByIp(routers,&source);
    PCENNode* dest_node = GetNodeByIp(routers,&destination);
    assert (src_node && dest_node);

    SearchMRNKSP(src_node->ref_num, dest_node->ref_num, switching_type_egress, encoding_type_egress, bandwidth_egress, 10);
    LOGF("Found %d shortest paths...\n", KSP_MRN.size());
    
    if (KSP_MRN.size() > 0)
    {
        KSP_MRN[0]->GetERO(ero);
        ReplyERO();
     }
}
