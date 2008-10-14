/*
 * Copyright (c) 2008
 * DRAGON Project.
 * Created and modified by Qian Xu and Shujia Gong (George Mason University), 2004-2008
 * Debugged, modified and improved by Xi Yang (USC/ISI), 2008
 */
 
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
	lcl_end->out_CGlinks.push_back(this); //@@@@ mem leak?
	rmt_end=search_PCENCGNode(CGnodes, remoteNodeId);
	rmt_end->in_CGlinks.push_back(this); //@@@@ mem leak?
}

double PCENCGLink::PCENCGmetric ()
{
	// add by qian to return the channel graph link metric
	return this->cg_metric;
}


void PCENCGLink::SetPCENCGmetric (double x)
{
	this->cg_metric = x;
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

PCENCGNode* PCEN_MRN_CG::search_PCENCGNode(int lclID, int rmtID, u_char swtype, u_int32_t auxvar1=0, u_int32_t auxvar2=0)
{
	for (unsigned int i=0; i<CGnodes.size(); i++)
	{
		if ((CGnodes[i]->lcl_endID == lclID) &&(CGnodes[i]->rmt_endID == rmtID)&&(CGnodes[i]->swType==swtype))
		{
			if (auxvar1 !=0 && CGnodes[i]->auxvar1 != auxvar1)
				continue;
			if (auxvar2 !=0 && CGnodes[i]->auxvar2 != auxvar2)
				continue;
			return CGnodes[i];
		}
	}
	return NULL;
}

void PCEN_MRN_CG::DijkstraMRN(int source, int destination)
{

	PCENCGNode* headnode= NULL;//new PCENCGNode();
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
		nextnode->minCost=(*itLink)->PCENCGmetric();  // add by qian 03/16/2007
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
	headnode = *itNode; 
	headnode->nflg.nfg.visited=1; // shortest path to this node has been found
	ReachableNodes.erase(itNode); // shortest path found for this head node;


	
	for (;;) 
	{
		if (headnode==NULL) 
		{
			cout<<"break 1"<<endl;
			break;
		}

		// Go through all the outgoing links for the newly added node
		itLink=headnode->out_CGlinks.begin();
		while (itLink != headnode->out_CGlinks.end()) 
		{
			nextnode = (*itLink)->rmt_end;
			if ( (nextnode->nflg.nfg.visited != 1) && (nextnode->nflg.nfg.maskoff != 1) &&
				(nextnode->nflg.nfg.filteroff != 1) &&
				(nextnode->minCost > headnode->minCost + ((*itLink)->PCENCGmetric())) &&   // add by qian 03/16/2007
				((*itLink)->lflg.lfg.filteroff == 0) && 
				((*itLink)->lflg.lfg.maskoff == 0)) 
			{
				nextnode->minCost = (headnode->minCost+((*itLink)->PCENCGmetric()));   // add by qian 03/16/2007
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

		if (ReachableNodes.size()==0) 
		{	
			cout<<"break 2"<<endl;
			break;
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
		headnode= *itNode;
		headnode->nflg.nfg.visited=1; // shortest path to this node has been found

		ReachableNodes.erase(itNode); // shortest path found for this head node;
//		cout<<"after erasure, headnode:  "<<headnode->ref_num<<endl;
		headnodeID=headnode->ref_num;

		if(headnodeID == destination)
		{
			LOGF("Find the shortest path from source to destination in DijkstraMRN....\n");
			return;
		}
	} 

	LOGF("Can't find the shortest path from source to destination in DijkstraMRN....\n");
	//delete headnode; // lead to memory leak?
}

void PCEN_MRN_CG::AddCGNode (int nodeid,int lclID,int rmtID,u_char swtype,u_char encoding,float bandwidth,double vMetric,u_char type, u_int32_t lclIfAddr=0, u_int32_t rmtIfAddr=0)
{
	PCENCGNode* pceCGNode = new PCENCGNode(nodeid);  //@@@@ mem leak?
	pceCGNode->minCost=PCEN_INFINITE_COST;
	pceCGNode->nflg.flag=0;
	pceCGNode->lcl_endID = lclID;
	pceCGNode->rmt_endID = rmtID;
	pceCGNode->swType = swtype;
	pceCGNode->encoding = encoding;
	pceCGNode->bandwidth = bandwidth;
	pceCGNode->LinkMetric = vMetric;
	pceCGNode->linkType = type;
	pceCGNode->auxvar1=lclIfAddr;
	pceCGNode->auxvar2=rmtIfAddr;
	CGnodes.push_back(pceCGNode);	
}

void PCEN_MRN_CG::AddCGLink(int linkid, int localNodeId, int remoteNodeId, double metric, int passNode)
{
	PCENCGLink* pceNCGLink=new PCENCGLink(linkid, localNodeId, remoteNodeId, CGnodes);
	pceNCGLink->auxvar1=passNode;
	pceNCGLink->auxvar2=0;
	pceNCGLink->SetPCENCGmetric(metric);
//	cout<<"Add link: "<<linkid<<" with link metric: "<<pceNCGLink->PCENCGmetric()<<endl;
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

PCEN_MRN_CG::PCEN_MRN_CG(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, 
        float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, float bw_egress, u_int32_t opts, 
        u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id, u_int32_t tag, u_int32_t hopback, 
        u_int32_t src_lclid, u_int32_t dest_lclid, narb_lsp_vtagmask_tlv* vtm):
    PCEN_KSP(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) 
{

} 


//old one
/*PCEN_MRN_CG::PCEN_MRN_CG(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, 
        float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, float bw_egress, u_int32_t opts, 
        u_int32_t lspq_id, u_int32_t msg_seqnum, u_int32_t tag, narb_lsp_vtagmask_tlv* vtm):
    PCEN_KSP(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, lspq_id, msg_seqnum, tag, vtm) 
{

} */

PCEN_MRN_CG::~PCEN_MRN_CG() {
    //clean Router list
    /*
    vector<PCENNode*>::iterator node_iter;
    for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++)
        if (*node_iter)
            delete *node_iter;
    //clean Link list
    vector<PCENLink*>::iterator link_iter;
    for (link_iter = links.begin();  link_iter != links.end(); link_iter++)
        if (*link_iter)
            delete *link_iter;
    */

	LOGF("Comment all the cleans, check link point\n");
/*	vector<PCENCGLink*>:: iterator itLink;
	if(CGlinks.size() >0){
	    	for(itLink = CGlinks.begin(); itLink != CGlinks.end(); itLink++)
    		{
    			if( *itLink && (*itLink)->link )
				LOGF("The link point is not null\n");
    		}
	}
*/
    //clean CGnodes list

 	
    vector<PCENCGLink*>:: iterator itLink;
    for(itLink = CGlinks.begin(); itLink != CGlinks.end(); itLink++)
    	if(*itLink)
    		delete *itLink;
   CGlinks.clear();

   vector<PathT_MRN*>:: iterator path_mrn;
    for(path_mrn = KSP_MRN.begin();path_mrn != KSP_MRN.end(); path_mrn++)
    	if(*path_mrn)
    		delete *path_mrn;	
    KSP_MRN.clear();
/*	
  vector<PCENCGNode*>::iterator itNode;
    for(itNode = CGnodes.begin(); itNode != CGnodes.end(); itNode++)
    	if(*itNode)
    		delete *itNode;
    CGnodes.clear();
*/
 
}

void PCEN_MRN_CG::DisplayCGNodes()
{
	cout<<"Display channel graph nodes"<<endl;
	vector<PCENCGNode*>::iterator node_iter;
	list<PCENCGLink*>::iterator link_iter;
	for(node_iter = CGnodes.begin(); node_iter != CGnodes.end(); node_iter++)
	{
		cout<<"node: "<<(*node_iter)->ref_num<<" has been added, local end "<<(*node_iter)->lcl_endID<<" and rmt end "<<(*node_iter)->rmt_endID<<endl;
		cout<<"the switching type is: "<<(int)(*node_iter)->swType<<endl;
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
		cout<<"The pcemetric of this link is : "<<(*link_iter)->PCENCGmetric()<<endl;
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
		//(*node_iter)->auxvar2=0;
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
//			delete (*it); 
			delete *itLink;
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
			delete *itNode;
			CGnodes.erase(itNode);
			itNode = it;
		}
		itNode++;
	}
	cout<<endl;
}

bool PCEN_MRN_CG::BuildEndNodes(int source, int end, u_char swtype,u_char encoding,float bandwidth)//changed 07/14
{
	cout<<"In BuildEndNodes, source is"<<source<<" end is:"<<end<<" swtype is:"<<swtype<<" encoding is"<<encoding<<endl;
	vector<PCENCGNode*>::iterator cgnode_iter;
	int sourceid,endid,linkID;
	bool valid_source = false;
	bool valid_end = false;
	
	sourceid = (int)CGnodes.size() + 1;
	endid = (int)CGnodes.size() + 2;
	linkID = (int)CGlinks.size()+1;
	
	//add virtual node for source and destination 
	this->AddCGNode(sourceid,source,source,'0','0',0,0,0);
	this->AddCGNode(endid,end,end,'0','0',0,0,0);
	
	//add virtual links for virtual nodes
	cgnode_iter = CGnodes.begin();
	while(cgnode_iter != CGnodes.end())
	{
		if(source == (*cgnode_iter)->lcl_endID && swtype == (*cgnode_iter)->swType && encoding == (*cgnode_iter)->encoding && (*cgnode_iter)->bandwidth >= bandwidth)
		{
			this->AddCGLink(linkID,sourceid,(*cgnode_iter)->ref_num,(*cgnode_iter)->LinkMetric,(*cgnode_iter)->lcl_endID);
			cout<<"Add source virtual link from : "<<sourceid<<" to : "<<(*cgnode_iter)->ref_num<< " with metric: "<<(*cgnode_iter)->LinkMetric<<endl;
			linkID++;
			valid_source = true;
		}

	//	if(end == (*cgnode_iter)->rmt_endID && end!= (*cgnode_iter)->lcl_endID && encoding == (*cgnode_iter)->encoding && (*cgnode_iter)->bandwidth >= bandwidth)
		if(end == (*cgnode_iter)->rmt_endID && end!= (*cgnode_iter)->lcl_endID && (*cgnode_iter)->bandwidth >= bandwidth)
		{
			this->AddCGLink(linkID,(*cgnode_iter)->ref_num,endid,0,(*cgnode_iter)->rmt_endID);
			cout<<"Add destination virtual link from : "<<endid<<" to : "<<(*cgnode_iter)->ref_num<<" with metric: 0"<<endl;
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
	cout<<"display channel graph links"<<endl;
//	this->DisplayCGNodes();
//	this->DisplayCGLinks();
	return true;
}

void PCEN_MRN_CG::ShowERO()
{
/*	for(unsigned int i=0; i<links.size(); i++)
	{
		LOGF("linkID [%X]\n", links[i]->linkID);
		LOGF("link type [%X]\n", links[i]->link->linkType);
		LOGF("link lcl addr [%X]\n", links[i]->link->lclIfAddr);
		LOGF("link rmt addr [%X]\n", links[i]->link->rmtIfAddr);
	}
*/	
	if(ero.size()<=0)
	{
		LOGF("The ERO list is empty. \n");
		return;
	}
	list<ero_subobj>::iterator ero_iter;
	int count = 1;
	for(ero_iter=ero.begin(); ero_iter!=ero.end(); ero_iter++)
	{
		LOGF("Hop %d:\n", count);
		LOGF("The source address is: [%X]\n", (*ero_iter).addr.s_addr);
		LOGF("The hop type is: [%X]\n", (*ero_iter).hop_type);
		LOGF("The prefix length is: [%d]\n", (*ero_iter).prefix_len);
		LOGF("The interface id is: [%X]\n", (*ero_iter).if_id);
		LOGF("The switch type is: [%X]\n", (*ero_iter).sw_type);
		LOGF("The encoding type is: [%X]\n", (*ero_iter).encoding);
		LOGF("The bandwidth is: [%d]\n", (*ero_iter).bandwidth);
		LOGF("The vlan tag is: [%d]\n", (*ero_iter).l2sc_vlantag);
		LOGF("The first pad is: [%X]\n", (*ero_iter).pad[0]);
		LOGF("The second pad is: [%X]\n", (*ero_iter).pad[1]);
		count++;
	}
}


/*int PCEN_MRN_CG::SearchSLNPath(int source, int destination,  float bandwidth)
{
	// mask off links with bandwidth less than the requirement
	for(unsigned int i=0; i
}
*/


int PCEN_MRN_CG::SearchMRNKSP(int source, int destination, u_char swtype, u_char encoding, float bandwidth, int K)
{
	
	LOGF("Searching path.....\n");
	cout<<"Requested swtching type:  "<<swtype<<endl;
	ero.clear();
	this->CreateChannelGraph(bandwidth);

	//source and destination is in network graph
	if( !(this->BuildEndNodes(source,destination,swtype,encoding,bandwidth)) )//changed 07/14
		return ERR_PCEN_NO_ROUTE;
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
		return ERR_PCEN_NO_SRC;
	}

	// First line in YEN's
	this->DijkstraMRN(source, destination);

//#################  testing #########################
//LOGF("return error after DijkstraMRN and before create new PathT_MRN...\n");
//return ERR_PCEN_NO_ROUTE;
//###############################################

	if(destNode->path_mrn.size() == 0)
	{
		cout<<"Can't find one shortest path from source to destination"<<endl;
		LOGF("Can't find one shortest path from source to destination\n");
		return ERR_PCEN_NO_ROUTE;
	}

	
	PathT_MRN* nextpath=new PathT_MRN();
	(nextpath->path_mrn).assign(destNode->path_mrn.begin(),destNode->path_mrn.end());
	nextpath->cost = destNode->minCost;
	cout << "path cost is" << nextpath->cost<<endl;
	LOGF("Path cost is:[%d]\n",nextpath->cost);
	nextpath->DeviationNode = source;
	CandidatePaths.push_back(nextpath);
	KSP_MRN.push_back(nextpath);//the first shortest path
	KSPcounter++;

	vector<PathT_MRN*>::iterator itPath,secPath;//added by qian
	PathT_MRN* headpath;
	while ((CandidatePaths.size()>0) && (KSPcounter<=K) && (K > 1)) 
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
//		headpath->DisplayPath_MRN();
		if (KSPcounter>1) KSP_MRN.push_back(headpath);
		if (KSPcounter==K) 
			//return ERR_PCEN_NO_ERROR;
			break; //get out of main while loop

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

//			cout<<"Finding path from "<<(*itLink)->lcl_end->ref_num<<" to destination node: "<<destination<<endl;
			//mask node with same lcl_endID and swtype with deviation node.
			vector<PCENCGNode*>::iterator cgnode;
			cgnode = CGnodes.begin();
			while(cgnode != CGnodes.end())
			{
				if((*cgnode)->lcl_endID == (*itLink)->lcl_end->lcl_endID && (*cgnode)->swType == (*itLink)->lcl_end->swType && (*cgnode)->rmt_endID != (*itLink)->lcl_end->rmt_endID)
					(*cgnode)->MaskCGNode();
				cgnode++;
			}

//			this->ShowMarkedLinks();
			this->DijkstraMRN((*itLink)->lcl_end->ref_num, destination);
			// find SPF from Vk_i to destination node

			if(destNode->path_mrn.size() == 0)
			{
				cout<<"Can't find path from "<<(*itLink)->lcl_end->ref_num<<" to destination node: "<<destination<<endl;
			}
			if (destNode->path_mrn.size()>0) 
			{
				// concatenate subpk(s, vk_i) to shortest path found from vk_i to destination
				PathT_MRN* nextpath=new PathT_MRN(); //@@@@ mem leak
				if (itLink!=headpath->path_mrn.begin()) 
				{
					//deviationstart=--itLink;
					//(nextpath->path).assign(headpath->path.begin(),deviationstart);
					(nextpath->path_mrn).assign(headpath->path_mrn.begin(),itLink); //@@@@ mem leak?
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
					(nextpath->path_mrn).push_back(*halfpath); //@@@@ mem leak?
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
					(nextpath->MaskedLinkList_mrn).assign(headpath->MaskedLinkList_mrn.begin(),headpath->MaskedLinkList_mrn.end()); //@@@@ mem leak?
					(nextpath->MaskedLinkList_mrn).push_back(*itLink); //@@@@ mem leak?
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

	if(KSP_MRN.size() > 0)
	{
            //$$$$ PCEN_KSP::VerifyPathConstraints is called in ConstructNetworkPath--> Only constraint qualified paths are added
		this->ConstructNetworkPath(); 
		this->DisplayNetworkPath();
	}
	this->DeleteVirtualNodes();

	//LOGF("%d path has been found:\n", KSP_MRN.size());
	//cout<<KSP_MRN.size()<<" path has been found"<<endl;

	list< list<PCENLink*> >::iterator onePath;
	ConstraintTagSet head_vlan_tag_set, result_tag_set;
	int best_path_num = 0;
	u_int32_t best_vtag = 0;
	double minCost = PCEN_INFINITE_COST;
	int count = 1;
	for(onePath = Net_Paths.begin();onePath != Net_Paths.end(); onePath++, count++)
	{
		// dealing with vlan tag
		u_int32_t vlan_tag = 0;
		bool find_tag = false;
		if(is_e2e_tagged_vlan)
		{
			if(vtag_mask)
			{
				LOGF("Asking for all available vlan tags\n");
				head_vlan_tag_set.AddTags(vtag_mask->bitmask, ntohs(vtag_mask->length)*8);
				result_tag_set = FindVlanTagSetForPath(head_vlan_tag_set, (*onePath), false);
				SetVTagMask(result_tag_set);
				LOGF("The number of all available vlan tags is:[%d]\n", result_tag_set.TagSet().size());
			}
			if (vtag == ANY_VTAG)
			{
				//LOGF("Looking for any vlan tag: %d\n", vtag);
				head_vlan_tag_set.AddTag(vtag);
				result_tag_set = FindVlanTagSetForPath(head_vlan_tag_set, (*onePath), true);
				if(result_tag_set.IsEmpty())
				{
					cout<<"Can't find available vlan tag in path: "<<count<<endl;
					continue;
				}
				else
				{
					
					if(find_tag==false)
					{
						vlan_tag = result_tag_set.TagSet().front();
						find_tag = true;
					}
				}
				LOGF("\nThe vlan tag found is: [%d]\n\n", vlan_tag);
			}
			else
			{
				cout<<"The requested vlan tag is: "<<vtag<<endl;
				vlan_tag = FindSpecificTagForPath(vtag, (*onePath));
				if(vlan_tag == vtag)
				{
					cout<<"The requested vlan tag "<<vtag<<" is available on path "<<count<<" !"<<endl;
					if(find_tag==false)
					{
						find_tag = true;
					}
				}
				else
				{
					continue;
				}
			}
		}

            //getting lowest-cost path and the associated vlan tag
            double pathCost = GetPathCost((*onePath));
            if (minCost > pathCost)
            {
                minCost = pathCost;
                best_path_num = count;
                if (find_tag)
                    best_vtag = vlan_tag;
            }
	}

	// add the best cost path into ERO
	if(best_path_num != 0)
	{
		int path_counter = 1;
		list< list<PCENLink*> >:: iterator BestPath;
		list<PCENLink*>  ThePath;
		list<PCENLink*>:: iterator eachLink;
		
		for(BestPath = Net_Paths.begin(); BestPath != Net_Paths.end(); BestPath++)
		{
			if(path_counter == best_path_num)
			{
				ThePath = (*BestPath);
				break;
			}
			path_counter++;
		}
		for(eachLink = ThePath.begin(); eachLink != ThePath.end();eachLink++)
		{
			AddLinkToERO(*eachLink);
		}
		
	}

	if(is_e2e_tagged_vlan)
	{
		if(vtag == ANY_VTAG)
		{
			if(best_vtag != 0)
			{
				vtag = best_vtag;
				if (vtag > MAX_VLAN_NUM)
                			vtag = 0;
				LOGF("Set vlan tag with: [%d]\n", best_vtag);
				SetVTagToERO(best_vtag);
			}
			if(result_tag_set.TagSet().size() >1 && (options & LSP_OPT_REQ_ALL_VTAGS))
			{
				LOGF("Setting vtag-mask\n");
				SetVTagMask(result_tag_set);
			}
			 if (vtag_mask && (options & LSP_OPT_REQ_ALL_VTAGS))
			{
				LOGF("The number of all available vlan tags is:[%d]\n", result_tag_set.TagSet().size());
			}
		}
			
	}
	LOGF("The size of ero is: %d\n", ero.size());
	//	cout<<"the size of ero is: "<<ero.size()<<endl;
	if (ero.size() == 0)
	{
		LOGF("No route found. The ERO is empty.\n");
       	return ERR_PCEN_NO_ROUTE;
	}

	return ERR_PCEN_NO_ERROR;
}

void PCEN_MRN_CG::AddLinkToERO(PCENLink* pcen_link)
{

	LOGF("AddLinkToEROTrack.....\n");
	LOGF("linkID [%X]\n", pcen_link->linkID);
	LOGF("link type [%X]\n", pcen_link->link->type);
	LOGF("link lcl addr [%X]\n", pcen_link->link->lclIfAddr);
	LOGF("link rmt addr [%X]\n", pcen_link->link->rmtIfAddr);

	if(pcen_link->link->type == RTYPE_LOC_PHY_LNK)
		LOGF("link type 0 \n");
	else if (pcen_link->link->type == RTYPE_LOC_ABS_LNK)
		LOGF("link type 1 \n");
	else if (pcen_link->link->type == RTYPE_LOC_RTID)
		LOGF("link type 2 \n");
	else if (pcen_link->link->type == RTYPE_GLO_PHY_LNK)
		LOGF("link type 3 \n");
	else if (pcen_link->link->type == RTYPE_GLO_ABS_LNK)
		LOGF("link type 4 \n");
	else if (pcen_link->link->type == RTYPE_GLO_RTID)
		LOGF("link type 5 \n");
			
/*	LOGF("The link ID of the adding link is: [%d]\n",pcen_link->linkID);
	LOGF("The source address is: [%X]\n", pcen_link->link->lclIfAddr);
	LOGF("The interface id is 1: [%X]\n", pcen_link->link->lclRmtId[1]);
	LOGF("The interface id is 0: [%X]\n", pcen_link->link->lclRmtId[0]);
	LOGF("The switch type lcl is: [%X]\n", pcen_link->lcl_end->tspec.SWtype);
	LOGF("The encoding lcl type is: [%X]\n", pcen_link->lcl_end->tspec.ENCtype);
	LOGF("The bandwidth lcl is: [%d]\n", pcen_link->lcl_end->tspec.Bandwidth);
	LOGF("The switch type rmt is: [%X]\n", pcen_link->rmt_end->tspec.SWtype);
	LOGF("The encoding rmt type is: [%X]\n", pcen_link->rmt_end->tspec.ENCtype);
	LOGF("The bandwidth rmt is: [%d]\n", pcen_link->rmt_end->tspec.Bandwidth);
	LOGF("The type of the link is: [%X]\n", pcen_link->link->type );
*/
	ero_subobj subobj1, subobj2;
	memset(&subobj1, 0, sizeof(ero_subobj));
       subobj1.prefix_len = 32;
	subobj1.addr.s_addr = pcen_link->link->lclIfAddr;
   	if (subobj1.addr.s_addr == 0)
    	{
       	 subobj1.addr.s_addr = pcen_link->link->advRtId;
	        subobj1.if_id = pcen_link->link->lclRmtId[0];
    	}
	memset(&subobj2, 0, sizeof(ero_subobj));
	subobj2.prefix_len = 32;
	subobj2.addr.s_addr = pcen_link->link->rmtIfAddr;
 	if (subobj2.addr.s_addr == 0)
   	{
       	 subobj2.addr.s_addr = pcen_link->link->id;
	        subobj2.if_id = pcen_link->link->lclRmtId[1];
    	}

	

	if (pcen_link->link->type == RTYPE_LOC_PHY_LNK)
       	 subobj1.hop_type = subobj2.hop_type = ERO_TYPE_STRICT_HOP;
	else 
       	 subobj1.hop_type = subobj2.hop_type = ERO_TYPE_LOOSE_HOP;

	subobj1.sw_type = subobj2.sw_type = pcen_link->lcl_end->tspec.SWtype;
	subobj1.encoding = subobj2.encoding = pcen_link->lcl_end->tspec.ENCtype;
	subobj1.bandwidth= subobj2.bandwidth = pcen_link->lcl_end->tspec.Bandwidth;

    //@@@@ ?
	subobj2.sw_type = pcen_link->rmt_end->tspec.SWtype;
	subobj2.encoding = pcen_link->rmt_end->tspec.ENCtype;
	subobj2.bandwidth = pcen_link->rmt_end->tspec.Bandwidth;
    
    //E2E VLAN related
    if (is_e2e_tagged_vlan && subobj1.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->link)
    {
        if (ntohs(pcen_link->link->iscds.front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
        {
//        	LOGF("Addlinktoero 1: [%d]\n", vtag);
            subobj1.l2sc_vlantag = vtag; //*(u_int16_t *)subobj1.pad
        }
    } 
    if (is_e2e_tagged_vlan && subobj2.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->reverse_link && pcen_link->reverse_link->link)
    {
        if (pcen_link->reverse_link->link->iscds.front() && (ntohs(pcen_link->reverse_link->link->iscds.front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC))
        {
//        	LOGF("Addlinktoero 2:[%d]\n", vtag);
		subobj2.l2sc_vlantag = vtag; //*(u_int16_t *)subobj2.pad
        }
    } 

    
    ero.push_back(subobj1);
    ero.push_back(subobj2);
}

void PCEN_MRN_CG::SetVTagToERO(u_int16_t vlan_tag)
{
	if (vlan_tag > MAX_VLAN_NUM)
                vlan_tag = 0;
	
	list<ero_subobj>::iterator iter;

      for (iter = ero.begin(); iter != ero.end(); iter++) 
      {
       	    if ((*iter).sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && (*iter).if_id == 0)
       	    	{
//				LOGF("set vlan tag %d to ero.\n", vlan_tag);
				(*iter).l2sc_vlantag = vlan_tag;
       	    	}
     	}
}

void PCEN_MRN_CG::SetVTagMask(ConstraintTagSet& vtagset)
{
	if (!vtag_mask)
       	 vtag_mask = new (narb_lsp_vtagmask_tlv);
   	 memset(vtag_mask, 0, sizeof(narb_lsp_vtagmask_tlv));
	 vtag_mask->type = htons(TLV_TYPE_NARB_VTAG_MASK);
	 vtag_mask->length= htons(sizeof(narb_lsp_vtagmask_tlv) - 4);
 
    	list<u_int32_t>::iterator it = vtagset.TagSet().begin();
	for (; it != vtagset.TagSet().end(); it++)
    	{
       	 SET_VLAN(vtag_mask->bitmask, *it);
    	}
}

double PCEN_MRN_CG::GetPathCost(list<PCENLink*>& path)
{
	double cost = 0;
	list<PCENLink*>::iterator itLink;
	for(itLink = path.begin(); itLink != path.end(); itLink++)
	{
            cost += (*itLink)->PCENmetric();
	}
	return cost;
}

ConstraintTagSet PCEN_MRN_CG::FindVlanTagSetForPath(ConstraintTagSet head_vtagset, list<PCENLink*> path, bool any_tag)
{
//	LOGF("In function FindVlanTagSetForPath....\n");
	list<PCENLink*>::iterator itLink;
	ConstraintTagSet next_vtagset;
	int link_number = 1;
	
	for(itLink = path.begin(); itLink != path.end(); itLink++)
	{
		next_vtagset.TagSet().clear();
		next_vtagset = FindVtagSetInLink(link_number, head_vtagset, *itLink, any_tag);
//		LOGF("First vtag in next_vtagset is: %d\n", next_vtagset.TagSet().front());
		head_vtagset.TagSet().clear();
		list<u_int32_t>::iterator it;
		for(it=next_vtagset.TagSet().begin(); it != next_vtagset.TagSet().end(); it++)
		{
			head_vtagset.AddTag(*it);
		}
		link_number++;
	}
	return next_vtagset;
}

int PCEN_MRN_CG::FindSpecificTagForPath(int vtag, list<PCENLink*> path)
{
	list<PCENLink*>::iterator itLink;
//	cout<<"This path has "<<path.size()<<" links"<<endl;
	int link_number = 1;
	for(itLink = path.begin(); itLink != path.end(); itLink++)
	{
		if(FindTagInLink(vtag, link_number,(*itLink)) < 0)
		{
//			cout<<"The link from"<<(*itLink)->lcl_end->ref_num<<" to "<< (*itLink)->rmt_end->ref_num<<" don't has avaivlable tag"<<endl;
			return -1;
		}
		link_number++;
//		cout<<"The link from"<<(*itLink)->lcl_end->ref_num<<" to "<< (*itLink)->rmt_end->ref_num<<" has avaivlable tag"<<endl;
	}
//	cout<<"The return vlan tag from FindSpecificTagForPath function is:" <<vtag<<endl;
	return vtag;
}

ConstraintTagSet PCEN_MRN_CG::FindVtagSetInLink(int link_num, ConstraintTagSet head_set, PCENLink* oneLink, bool any_vtag_ok)
{
//	cout<<"Search vlan tags in link "<<oneLink->lcl_end->ref_num<<" to "<<oneLink->rmt_end->ref_num<<"......"<<endl;
	ConstraintTagSet vTagSet;
	list<ISCD*>::iterator it;
	ISCD* iscd;
	vTagSet.TagSet().clear();
	bool any_vlan_ok = (head_set.TagSet().size() > 0 && head_set.TagSet().front() == ANY_VTAG);
       bool non_vlan_link = true;

	// put all the available vlan tags in this link into vTagSet
	for(it = oneLink->link->Iscds().begin(); it != oneLink->link->Iscds().end(); it++)
	{
		iscd = *it;
		if(!iscd)
			continue;
		if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC
            || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC) // The non-L2SC layers are temoperaty here and yet to remove.
        	{
            		if (ntohs(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
            		{
                		non_vlan_link = false;
                		vTagSet.AddTags(iscd->vlan_info.bitmask, MAX_VLAN_NUM);
            		}
        	}
	}
	
	
	//remove all the vlan tags that are already being allocated to all the inlinks and outlinks of the lcl_end and rmt_end of this link
	list<PCENLink*>::iterator it_link;
	if(oneLink->lcl_end)
	{
		for(it_link = oneLink->lcl_end->in_links.begin(); it_link != oneLink->lcl_end->in_links.end(); it_link++)
		{
			(*it_link)->ExcludeAllocatedVtags(vTagSet);
		}
		for(it_link = oneLink->lcl_end->out_links.begin(); it_link != oneLink->lcl_end->out_links.end(); it_link++)
		{			
			if(*it_link != oneLink)
				(*it_link)->ExcludeAllocatedVtags(vTagSet);
		}
		
	}
	if(oneLink->rmt_end)
	{
		for(it_link = oneLink->rmt_end->in_links.begin(); it_link != oneLink->rmt_end->in_links.end(); it_link++)
		{
			if(*it_link != oneLink)
				(*it_link)->ExcludeAllocatedVtags(vTagSet);
		}
		for(it_link = oneLink->rmt_end->out_links.begin(); it_link != oneLink->rmt_end->out_links.end(); it_link++)
		{
			(*it_link)->ExcludeAllocatedVtags(vTagSet);
		}
	}

	if (non_vlan_link)
	        vTagSet = head_set;
    	else if (!any_vlan_ok)
       	 vTagSet.Intersect(head_set);	

	return vTagSet;
	
}

int PCEN_MRN_CG::FindTagInLink(int request_tag, int link_num, PCENLink* oneLink)
{
	ConstraintTagSet  vTagSet;
	list<ISCD*>::iterator it;
	ISCD* iscd;
	vTagSet.TagSet().clear();
	bool has_l2sc = false; // if the link has L2SC switching type, the value is true;

//	cout<<"Find the tag set for all the available vlan tags"<<endl;

	for(it = oneLink->link->Iscds().begin(); it != oneLink->link->Iscds().end(); it++)
	{
		iscd = *it;
		if(!iscd)
			continue;
		if(iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
			has_l2sc = true;
		if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC
           		 || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC) 
       	 {
            		if (ntohs(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
           		        vTagSet.AddTags(iscd->vlan_info.bitmask, MAX_VLAN_NUM);
            	 }
	}

	if(!has_l2sc && link_num > 1)
		return request_tag;
	
//	cout<<"The tag set size is "<<vTagSet.TagSet().size()<<endl;

	//display tag list
/*	cout << "Tags:";
            list<u_int32_t>::iterator itTag;
            for (itTag = vTagSet.TagSet().begin(); itTag != vTagSet.TagSet().end(); itTag++)
                cout << ' ' << *itTag;
            cout << endl;
	*/
	// if the available vlan tags don't include the request tag, return -1
	if(!vTagSet.HasTag(request_tag))
	{
		cout<<"The requested tag is not available in this link"<<endl;
		return -1;
	}
	
//	cout<<"Exclude the tags that are already being allocated"<<endl;
	list<PCENLink*>::iterator it_link;
	if(oneLink->lcl_end)
	{
		for(it_link = oneLink->lcl_end->in_links.begin(); it_link != oneLink->lcl_end->in_links.end(); it_link++)
			if(CheckAllocatedTags(*it_link, request_tag) < 0)
			{
//				cout<<"local end of inlinks has used tag "<<request_tag<<endl;
				return -1;
			}
		for(it_link = oneLink->lcl_end->out_links.begin(); it_link != oneLink->lcl_end->out_links.end(); it_link++)
			if(*it_link != oneLink)
				if(CheckAllocatedTags(*it_link, request_tag) < 0)
				{
//					cout<<"local end of outlinks has used tag "<<request_tag<<endl;
					return -1;
				}
		
	}
	if(oneLink->rmt_end)
	{
		for(it_link = oneLink->rmt_end->in_links.begin(); it_link != oneLink->rmt_end->in_links.end(); it_link++)
			if(*it_link != oneLink)
				if(CheckAllocatedTags(*it_link, request_tag) < 0)
				{
//					cout<<"remote end of inlinks has used tag "<<request_tag<<endl;
					return -1;
				}
		for(it_link = oneLink->rmt_end->out_links.begin(); it_link != oneLink->rmt_end->out_links.end(); it_link++)
			if(CheckAllocatedTags(*it_link, request_tag) < 0)
			{
//				cout<<"remote end of outlinks has used tag "<<request_tag<<endl;
				return -1;
			}
		
	}
//	cout<<"The return vlan tag from FindTagInLink is:" <<request_tag<<endl;
	return request_tag;
}

int PCEN_MRN_CG::CheckAllocatedTags(PCENLink* oneLink, int request_tag)
{
//	cout<<"In CheckAllocatedTags function "<<endl;
	list<ISCD*>::iterator it;
    	ISCD * iscd;
	ConstraintTagSet vTagSet;
	vTagSet.TagSet().clear();
	
	for (it = oneLink->link->Iscds().begin(); it != oneLink->link->Iscds().end(); it++)
    	{
       	 iscd = *it;
	        if (!iscd)
       	     continue;
	        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
       	 {
	       	if (ntohs(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC)
       	     	{
              		vTagSet.AddTags(iscd->vlan_info.bitmask_alloc, MAX_VLAN_NUM);
	            	}
       	 }
    	}
	if(vTagSet.HasTag(request_tag))
	{
//		cout<<"In CheckAllocatedTags function, the request tag is not available in link"<<oneLink->lcl_end->ref_num<<" to "<<oneLink->rmt_end->ref_num<< endl;
		return -1;
	}
	else 
		return request_tag;
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

list<PCENLink*> PCEN_MRN_CG::CGPathToNetPath(list<PCENCGLink*> cgPath)
{
	list<PCENLink*> netPath;
	netPath.clear();
	PCENLink* netLink[2];
	int oriLinkType[2];
	int i=0, lcl_end=-1, rmt_end=-1;
	list<PCENCGLink*>::iterator itLink = cgPath.begin();
	bool duplex=false;
	
	//cout<<"Size of the channel graph path is "<<cgPath.size()<<endl;
	
	while (itLink != cgPath.end())
	{
		if (lcl_end < 0)
			lcl_end = (*itLink)->auxvar1;
		else
			rmt_end = (*itLink)->auxvar1;
		
		if (lcl_end > 0 && rmt_end > 0)
		{

			int num=0;
			oriLinkType[0]=-1;
			oriLinkType[1]=-1;
			for(unsigned int i=0; i<links.size(); i++)
			{
//				if(netLinks[i]->lcl_end->ref_num == lcl_end && netLinks[i]->rmt_end->ref_num == rmt_end)
//					&& links[i]->link->type == (*itLink)->lcl_end->linkType)
				for(unsigned int j=0; j<chopLinks.size(); j++)
				{
					if(i == chopLinks[j])
					{
						duplex = true;
						break;
					}
				}
				if(duplex)
				{
					duplex = false;
					continue;
				}
				if(links[i]->lcl_end->ref_num == lcl_end && links[i]->rmt_end->ref_num == rmt_end)
				{
					netLink[num] =  links[i];
					oriLinkType[num] = (int)links[i]->link->type;
					num++;
					if (num == 2)
						break;
				}
			}
		
//			netLink = search_PCENLink(lcl_end,rmt_end);
			if(oriLinkType[1] == -1 && netLink[0])
			{
				LOGF("Add the following into network path\n");
				LOGF("linkID [%X]\n", netLink[0]->linkID);
				LOGF("link type [%X]\n", netLink[0]->link->type);
				LOGF("link lcl addr [%X]\n", netLink[0]->link->lclIfAddr);
				LOGF("link rmt addr [%X]\n", netLink[0]->link->rmtIfAddr);

				netPath.push_back(netLink[0]);
			}
			else if((oriLinkType[1] > -1) && (oriLinkType[0]-oriLinkType[1] <= 0) && netLink[0])
			{
				LOGF("Add the following into network path\n");
				LOGF("linkID [%X]\n", netLink[0]->linkID);
				LOGF("link type [%X]\n", netLink[0]->link->type);
				LOGF("link lcl addr [%X]\n", netLink[0]->link->lclIfAddr);
				LOGF("link rmt addr [%X]\n", netLink[0]->link->rmtIfAddr);
				netPath.push_back(netLink[0]);
			}
			else if (netLink[1]){
				LOGF("Add the following into network path\n");
				LOGF("linkID [%X]\n", netLink[1]->linkID);
				LOGF("link type [%X]\n", netLink[1]->link->type);
				LOGF("link lcl addr [%X]\n", netLink[1]->link->lclIfAddr);
				LOGF("link rmt addr [%X]\n", netLink[1]->link->rmtIfAddr);
				netPath.push_back(netLink[1]);
			}
			
		}
		if (i>0)
			lcl_end = rmt_end;
		i++;
		itLink++;
	}
	//cout<<endl;
	
	return netPath;
}

list<PCENLink*> PCEN_MRN_CG::CGPathToNetPath2(list<PCENCGLink*> cgPath)
{
	list<PCENLink*> netPath;
	netPath.clear();
	PCENCGNode *lcl_cgnode=NULL, *rmt_cgnode=NULL;
	list<PCENCGLink*>::iterator itLink = cgPath.begin();
	bool duplex=false;
	
	//cout<<"Size of the channel graph path is "<<cgPath.size()<<endl;
	
	while (itLink != cgPath.end())
	{
		assert (lcl_cgnode == NULL || lcl_cgnode == (*itLink)->lcl_end);

		rmt_cgnode = (*itLink)->rmt_end;
		if (lcl_cgnode != NULL && rmt_cgnode != NULL)
		{

			for(unsigned int i=0; i<links.size(); i++)
			{
				for(unsigned int j=0; j<chopLinks.size(); j++)
				{
					if(i == chopLinks[j])
					{
						duplex = true;
						break;
					}
				}
				if(duplex)
				{
					duplex = false;
					continue;
				}
				if(links[i]->lcl_end->ref_num == lcl_cgnode->lcl_endID && links[i]->rmt_end->ref_num == lcl_cgnode->rmt_endID 
					&& links[i]->link->LclIfAddr() == lcl_cgnode->auxvar1 && links[i]->link->RmtIfAddr() == lcl_cgnode->auxvar2)
				{
					LOGF("Add the following into network path\n");
					LOGF("linkID [%X]\n", links[i]->linkID);
					LOGF("link type [%X]\n", links[i]->link->type);
					LOGF("link lcl addr [%X]\n", links[i]->link->lclIfAddr);
					LOGF("link rmt addr [%X]\n", links[i]->link->rmtIfAddr);
					netPath.push_back(links[i]);
				}
			}
		}

		lcl_cgnode = rmt_cgnode;
		itLink++;
	}
	//cout<<endl;
	return netPath;
}

void PCEN_MRN_CG::ConstructNetworkPath()
{
	list<PCENLink*> path;
	 vector<PathT_MRN*>::iterator itPath =  KSP_MRN.begin();
	 while(itPath != KSP_MRN.end())
	 {
		path = CGPathToNetPath2((*itPath)->path_mrn);
		if (VerifyPathConstraints(path)) //PCEN_KSP function
		{
			Net_Paths.push_back(path); //only constraint qualified paths are added
		}
		itPath++;
	 }
}

void PCEN_MRN_CG::DisplayNetworkPath()
{
	int i=1;
	list< list<PCENLink*> >:: iterator itPath = Net_Paths.begin();
	while(itPath != Net_Paths.end())
	{
		list<PCENLink*>::iterator itLink = (*itPath).begin();
		cout<<"Path "<<i<<": "<<endl;
		while(itLink != (*itPath).end())
		{
			cout<<(*itLink)->lcl_end->ref_num<<"->"<<(*itLink)->rmt_end->ref_num<<" ";
			itLink++;
		}
		cout<<endl;
		itPath++;
		i++;
	}

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

void PCEN_MRN_CG::AssignRefNumber(vector<PCENNode*>& routers)
{	
	if(routers.size() == 0)
		return;
		
	PCENNode* pcen_node;
    	for (int i = 0; i < routers.size(); i++)
    	{
       	 pcen_node = routers[i];
		 pcen_node->ref_num = i+1;     
    	}    
	
}


void PCEN_MRN_CG::CreateChannelGraph(float bandwidth)//changed 07/14
{
	cout<<"Creating Channel graph......."<<endl;
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
	bool duplex = false;
	int index=0;
			
	//for the simplest case, consider the bandwidth on each link's iscd->max_lsp_bw[7] must >= the requested bandwidth
	//add nodes in channel graph
	itLink =links.begin();
	while (itLink!= links.end())
	{
		for(unsigned int i=0; i<chopLinks.size(); i++)
		{
			if(index == chopLinks[i])
			{
				duplex = true;
				break;
			}
		}
		if(duplex)
		{
			itLink++;
			index++;
			duplex = false;
			continue;
		}
		if((*itLink)->link->Iscds().size() > 0){
			iscd_iter = (*itLink)->link->Iscds().begin();
			while(iscd_iter != (*itLink)->link->Iscds().end())
			{
				if((*iscd_iter)->max_lsp_bw[7] >= bandwidth)
				{
					DisplayPCENLink(*itLink);
					this->AddCGNode(id,(*itLink)->lcl_end->ref_num,(*itLink)->rmt_end->ref_num,(*iscd_iter)->swtype,(*iscd_iter)->encoding,(*iscd_iter)->max_lsp_bw[7],(*itLink)->PCENmetric(),(*itLink)->link->type, (*itLink)->link->lclIfAddr, (*itLink)->link->rmtIfAddr);
					cout<<"Channel grpah node : "<<id<<" with local_end "<<(*itLink)->lcl_end->ref_num<<" and rmt_end "<<(*itLink)->rmt_end->ref_num<<"  and swtype "<<(int)(*iscd_iter)->swtype<<" node metric is: " <<(*itLink)->PCENmetric()<<endl;
					id++;
				}
				iscd_iter++;
			}
		}
		itLink++;
		index++;
		duplex = false;
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
//						cout<<"inlink "<<(*inlink)->linkID<<" with swtype "<<(int)(*iscd_in)->swtype<<" outlink "<<(*outlink)->linkID<<" with swtype"<<(*iscd_out)->swtype<<endl;
						if((*iscd_in)->swtype == (*iscd_out)->swtype)
						{
							start = search_PCENCGNode((*inlink)->lcl_end->ref_num,(*inlink)->rmt_end->ref_num,(*iscd_in)->swtype, (*inlink)->link->lclIfAddr, (*inlink)->link->rmtIfAddr);
							tail = search_PCENCGNode((*outlink)->lcl_end->ref_num,(*outlink)->rmt_end->ref_num,(*iscd_out)->swtype, (*outlink)->link->lclIfAddr, (*outlink)->link->rmtIfAddr);
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
	index = 0;
	duplex = false;
	while(itLink != links.end())
	{
		for(unsigned int i=0; i<chopLinks.size(); i++)
		{
			if(index == chopLinks[i])
			{
				duplex = true;
				break;
			}
		}
		if(duplex)
		{
			itLink++;
			index++;
			duplex = false;
			continue;
		}
		oneLink = (*itLink);
		//reverse = this->search_PCENLink(oneLink->rmt_end->ref_num, oneLink->lcl_end->ref_num);
		reverse = oneLink->reverse_link;

		//if both links have only one iscd
		if(reverse != NULL && oneLink->link->Iscds().size() == 1 && reverse->link->Iscds().size() == 1 && oneLink->link->Iscds().front()->swtype != reverse->link->Iscds().front()->swtype)
		{
			start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,oneLink->link->Iscds().front()->swtype, oneLink->link->lclIfAddr, oneLink->link->rmtIfAddr);
			adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
			out = adaptNode->out_links.begin();
			while(out != adaptNode->out_links.end())
			{
				if(oneLink->lcl_end->ref_num == (*out)->rmt_end->ref_num && oneLink->rmt_end->ref_num == (*out)->lcl_end->ref_num)
				{
					out++;
					continue;
				}
				//if((*out) != reverse) // case covered by the line above
				//{
				iscd_iter = (*out)->link->Iscds().begin();
				while(iscd_iter != (*out)->link->Iscds().end())
				{
					if((*iscd_iter)->swtype == reverse->link->Iscds().front()->swtype)
					{
						tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_iter)->swtype, (*out)->link->lclIfAddr, (*out)->link->rmtIfAddr);
						if (tail != NULL)
						{
							this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
							//cout<<"linkID : "<<id<<"find same swtype, link start from "<<start->ref_num<<" and end with "<<tail->ref_num<<" by pass node "<<start->rmt_endID<<endl;
							id++;
						}
					}
					iscd_iter++;
				}
				//}
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
						start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,adapt_swtype1, oneLink->link->lclIfAddr, oneLink->link->rmtIfAddr);
						adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
						for(out = adaptNode->out_links.begin();out != adaptNode->out_links.end();out++)
						{
							if((*out) != reverse)
							{
								for(iscd_out = (*out)->link->Iscds().begin(); iscd_out != (*out)->link->Iscds().end();iscd_out++)
								{
									if((*iscd_out)->swtype == adapt_swtype2)
									{
										tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_out)->swtype, (*out)->link->lclIfAddr, (*out)->link->rmtIfAddr);
										if (tail != NULL)
										{
											this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
											id++;
										}
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
					start = this->search_PCENCGNode(oneLink->lcl_end->ref_num,oneLink->rmt_end->ref_num,adapt_swtype1, oneLink->link->lclIfAddr, oneLink->link->rmtIfAddr);
					adaptNode = this->search_PCENNode(oneLink->rmt_end->ref_num);
					for(out = adaptNode->out_links.begin();out != adaptNode->out_links.end();out++)
					{
						if((*out) != reverse)
						{
							for(iscd_out = (*out)->link->Iscds().begin(); iscd_out != (*out)->link->Iscds().end();iscd_out++)
							{
								if((*iscd_out)->swtype == adapt_swtype2)
								{
									tail = this->search_PCENCGNode((*out)->lcl_end->ref_num,(*out)->rmt_end->ref_num,(*iscd_out)->swtype, (*out)->link->lclIfAddr, (*out)->link->rmtIfAddr);
									if (tail != NULL)
									{
										this->AddCGLink(id,start->ref_num,tail->ref_num,tail->LinkMetric,start->rmt_endID);
										id++;
									}
								}
							}
						}
					}
				}
				adapt = false;
			}
		}

		itLink++;
		index++;
		duplex = false;
	}
	
	cout<<"channel graph is created"<<endl;
//	this->DisplayCGNodes();
//	this->DisplayCGLinks();
}

bool PCEN_MRN_CG::PostBuildTopology()
{
    int i, j, k;
    PCENLink *pcen_link, *pcen_link2;
    PCENNode *pcen_node;
    list<ISCD*>::iterator iscd_iter;
    ISCD* iscd;
    list<PCENLink*>::iterator link_iter;
    RadixTree<Resource>* tree;
    RadixNode<Resource> *node;
    PCENLink* lclid_link_src = NULL, *lclid_link_dest = NULL;

    //$$$$ Local ID based source edge constraint take priority over hopback!
    if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)
    {
        hop_back = 0;
    }

    bool hop_back_attaching_to_source_verified = (hop_back == 0 ? true : false);
    PCENLink* hopBackInterdomainPcenLink = NULL;

    int rNum = routers.size(); 
    int lNum = links.size();

    //Init Tspec on nodes/routers 
    for (i = 0; i < rNum; i++)
    {
        pcen_node = routers[i];
        pcen_node->tspec.SWtype = switching_type_ingress;
        pcen_node->tspec.ENCtype = encoding_type_ingress;
        pcen_node->tspec.Bandwidth = bandwidth_ingress;
    }

    //Init reverseLink and verify hopback
    for (i = 0; i < lNum; i++)
    {
        pcen_link = links[i];
        assert(pcen_link);

        //verifying that the hop_back address is a localIfAddr of the source node...
        if (hop_back != 0 && pcen_link->link->LclIfAddr() == hop_back && pcen_link->link->AdvRtId() == source.s_addr)
        {
            hop_back_attaching_to_source_verified = true;
        }

        pcen_node = pcen_link->rmt_end;
        if (!pcen_node)
            continue;
        list<PCENLink *>::iterator it_link;
        for (it_link = pcen_node->out_links.begin(); it_link != pcen_node->out_links.end(); it_link++)
        {
            assert(*it_link);
            if ((*it_link)->rmt_end == pcen_link->lcl_end && (*it_link)->link->rmtIfAddr == pcen_link->link->lclIfAddr
			&& (*it_link)->link->rmtId == pcen_link->link->lclId && (*it_link)->link->type == pcen_link->link->type)
            {
                pcen_link->reverse_link = *it_link;
                break;
            }
        }
    }

    if (!hop_back_attaching_to_source_verified)
    {
        LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that the hopback interface (0x%x) is attaching to the source router(0x%x)\n", hop_back, source.s_addr);
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        return false;
    }

    // obtaining and verifying lclid_link_src and lclid_link_dest
    if (((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID ||  (dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID))
    {
        tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
        node = tree->Root();
        while (node)
        {
            Link* link = (Link*)node->Data();
            if (link == NULL || link->Iscds().size() == 0 ||
               (htons(link->Iscds().front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) == 0)
            {
                node = tree->NextNode(node);
                continue;
            }
            if (!lclid_link_src && link->AdvRtId() == source.s_addr
                 && link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((src_lcl_id >> 8) & 0xff))
            {
                link->removeDeltaByOwner(ucid, seqnum);
                link->deleteExpiredDeltas(); // handling expired link state deltas. (refer to class Link and class LSPHandler.)
                link = new Link(link);
                lclid_link_src = new PCENLink(link);
                lclid_link_src->link_self_allocated = true;
            }
            else if (!lclid_link_dest && link->AdvRtId() == destination.s_addr
                 && link->Iscds().front()->subnet_uni_info.subnet_uni_id == ((dest_lcl_id >> 8) & 0xff))
            {
                link->removeDeltaByOwner(ucid, seqnum);
                link->deleteExpiredDeltas(); // handling expired link state deltas. (refer to class Link and class LSPHandler.)
                link = new Link(link);
                lclid_link_dest = new PCENLink(link);
                lclid_link_dest->link_self_allocated = true;
            }

            node = tree->NextNode(node);
        }
        // verifying and adding the source lclid link into topology
        if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && !lclid_link_src)
        {
            LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that source  (0x%x) local-id is attaching to the topology\n", src_lcl_id);
            ReplyErrorCode(ERR_PCEN_INVALID_REQ);
            return false;
        }
        else if (lclid_link_src && lclid_link_src->link)
        {
            lclid_link_src->linkID = links.back()->linkID+1;
            links.push_back(lclid_link_src);
            lNum += 1;
            for (i = 0; i < rNum; i++) 
            {
                pcen_node = routers[i];
                if (pcen_node->router && pcen_node->router->Id() == lclid_link_src->link->AdvRtId())
                {
                    pcen_node->out_links.push_back(lclid_link_src);
                    lclid_link_src->lcl_end = pcen_node;
                }
            }
        }
        // verifying and adding the destination lclid link into topology
        if ((dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && !lclid_link_dest)
        {
            LOGF("ERROR: PCEN_MRN::PostBuildTopology cannot verify that destination  (0x%x) local-id is attaching to the topology\n", dest_lcl_id);
            ReplyErrorCode(ERR_PCEN_INVALID_REQ);
            return false;
        }
        else if (lclid_link_dest && lclid_link_dest->link)
        {
            lclid_link_dest->linkID = links.back()->linkID+1;
            links.push_back(lclid_link_dest);
            lNum += 1;
            for (i = 0; i < rNum; i++) 
            {
                pcen_node = routers[i];
                if (pcen_node->router && pcen_node->router->Id() == lclid_link_dest->link->AdvRtId())
                {
                    pcen_node->out_links.push_back(lclid_link_dest);
                    lclid_link_dest->lcl_end = pcen_node;
                }
            }
        }
    }
	
    // Building SubnetUNI 'jump' links to incorporate the UNI subnet (a Ciena SONET network in this specific implemenation)
    if (SystemConfig::should_incorporate_subnet)
    {
        PCENNode* new_pcen_source = NULL;
        PCENNode* new_pcen_destination = NULL;
        u_int32_t hopback_source = 0;

        // Handling source local-id constraint in the special 'subnet' topology
        if ((src_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)  //  special handling for lclid edge constraints
        {
            // costructing fake source and destination nodes for local-ids
            // the newly constructed source edge node use source local-id as router id
            RouterId* new_router = new RouterId(source.s_addr+src_lcl_id); 
            new_pcen_source = new PCENNode(new_router);
            new_pcen_source->router_self_allocated = true;
            new_pcen_source->tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
            routers.push_back(new_pcen_source);
            rNum++;
            PCENLink * pcen_link_src_backward = lclid_link_src;  // reusing the backward pcen_link
            Link* link_src_backward = pcen_link_src_backward->link;
            // cutomizing link_src_backward
            link_src_backward->id = new_pcen_source->router->id;
            link_src_backward->rmtIfAddr = get_slash30_peer(link_src_backward->lclIfAddr);
            pcen_link_src_backward->rmt_end = new_pcen_source;
            //pcen_link_src_backward->link_self_allocated = true;
            new_pcen_source->in_links.push_back(pcen_link_src_backward);
            // costructing fake source link (reverse direction)
            Link* link_src_forward = new Link(link_src_backward->Id(), link_src_backward->AdvRtId(), 
                link_src_backward->RmtIfAddr(), link_src_backward->LclIfAddr());
            // cutomizing link_src_forward with L2 ISCD
            iscd = new ISCD;
            if (link_src_backward->Iscds().size() == 2) //@@@@ TODO in OSPFd?
            {
                *iscd = *link_src_backward->iscds.back();  
            }
            else   // looking for corresponding inter-domain link and copy layer-2 specific information
            {
                Link* link_inter = NULL;
                tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
                node = tree->Root();
                while (node)
                {
                    Link* link = (Link*)node->Data();
                    if (link == NULL || link->Iscds().size() == 0 || link->Iscds().front()->swtype != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                       || (htons(link->Iscds().front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) == 0)
                    {
                        node = tree->NextNode(node);
                        continue;
                    }
                    if (link->lclIfAddr == link_src_backward->lclIfAddr)
                    {
                        link_inter = link;
                        break;
                    }
                    node = tree->NextNode(node);
                }
                if (link_inter != NULL)
                {
                    *iscd = *link_inter->iscds.front();
                }
                else // fake l2 info if unavailable
                {
                    memset(iscd, 0, sizeof(IfSwCapDesc));
                    iscd->swtype = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
                    iscd->encoding = LINK_IFSWCAP_SUBTLV_ENC_ETH;
                    for (i = 0; i < 8; i++)
                        iscd->max_lsp_bw[i] = link_src_backward->Iscds().front()->max_lsp_bw[i];
                }
            }
            link_src_forward->iscds.push_back(iscd);
            PCENLink* pcen_link_src_forward = new PCENLink(link_src_forward); //have to make new
            pcen_link_src_forward->link_self_allocated = true;
            pcen_link_src_forward->lcl_end = new_pcen_source;
            pcen_link_src_forward->rmt_end = pcen_link_src_backward->lcl_end;
            pcen_link_src_forward->reverse_link = pcen_link_src_backward;
            pcen_link_src_backward->reverse_link = pcen_link_src_forward;
            new_pcen_source->out_links.push_back(pcen_link_src_forward);
            pcen_link_src_backward->lcl_end->in_links.push_back(pcen_link_src_forward);
            pcen_link_src_forward->linkID = links.back()->linkID+1;
            links.push_back(pcen_link_src_forward);
            lNum++;
            // new source IP
            source.s_addr = source.s_addr+src_lcl_id;
        }

        // Handling destiantion local-id constraint in the special 'subnet' topology
        if ((dest_lcl_id >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID)  //  special handling for lclid edge constraints
        {            
            // the newly constructed destination edge node use destination local-id as router id
            RouterId* new_router = new RouterId(destination.s_addr+dest_lcl_id);
            new_pcen_destination = new PCENNode(new_router);
            new_pcen_destination->tspec.Update(switching_type_ingress, encoding_type_ingress, bandwidth_ingress);
            new_pcen_destination->router_self_allocated = true;
            routers.push_back(new_pcen_destination);
            rNum++;
            PCENLink * pcen_link_dest_forward = lclid_link_dest;  // reusing the forward pcen_link
            Link* link_dest_forward = pcen_link_dest_forward->link;
            //cutomizing link_dest_forward
            link_dest_forward->id = new_pcen_destination->router->id;
            link_dest_forward->rmtIfAddr = get_slash30_peer(link_dest_forward->lclIfAddr);
            pcen_link_dest_forward->rmt_end = new_pcen_destination;
            //pcen_link_dest_forward->link_self_allocated = true;
            new_pcen_destination->in_links.push_back(pcen_link_dest_forward);
            // costructing fake destination link (reverse direction)            
            Link* link_dest_backward = new Link(link_dest_forward->Id(), link_dest_forward->AdvRtId(), 
                link_dest_forward->RmtIfAddr(), link_dest_forward->LclIfAddr());
            // cutomizing link_dest_backward
            iscd = new ISCD;
            if (link_dest_forward->Iscds().size() == 2) //@@@@ TODO in OSPFd
            {
                *iscd = *link_dest_forward->iscds.back();  
            }
            else // looking for corresponding inter-domain link and copy layer-2 specific information
            {
                Link* link_inter = NULL;
                tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
                node = tree->Root();
                while (node)
                {
                    Link* link = (Link*)node->Data();
                    if (link == NULL || link->Iscds().size() == 0 || link->Iscds().front()->swtype != LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
                       || (htons(link->Iscds().front()->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) == 0)
                    {
                        node = tree->NextNode(node);
                        continue;
                    }
                    if (link->lclIfAddr == link_dest_forward->lclIfAddr)
                    {
                        link_inter = link;
                        break;
                    }
                    node = tree->NextNode(node);
                }
                if (link_inter != NULL)
                {
                    *iscd = *link_inter->iscds.front();
                }
                else // fake l2 info if unavailable
                {
                    memset(iscd, 0, sizeof(IfSwCapDesc));
                    iscd->swtype = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
                    iscd->encoding = LINK_IFSWCAP_SUBTLV_ENC_ETH;
                    for (i = 0; i < 8; i++)
                        iscd->max_lsp_bw[i] = link_dest_forward->Iscds().front()->max_lsp_bw[i];
                }
            }
            link_dest_backward->iscds.push_back(iscd);
            PCENLink* pcen_link_dest_backward = new PCENLink(link_dest_backward); //have to make new
            pcen_link_dest_backward->link_self_allocated = true;
            pcen_link_dest_backward->lcl_end = new_pcen_destination;
            pcen_link_dest_backward->rmt_end = pcen_link_dest_forward->lcl_end;
            pcen_link_dest_backward->reverse_link = pcen_link_dest_forward;
            pcen_link_dest_forward->reverse_link = pcen_link_dest_backward;
            new_pcen_destination->out_links.push_back(pcen_link_dest_backward);
            pcen_link_dest_forward->lcl_end->in_links.push_back(pcen_link_dest_backward);
            pcen_link_dest_backward->linkID = links.back()->linkID+1;
            links.push_back(pcen_link_dest_backward);
            lNum++;
            // new destination IP
            destination.s_addr = destination.s_addr+dest_lcl_id;
        }

        // Handling hop-back constraint in the special 'subnet' topology
        if (hop_back != 0) //  special handling for hop back
        {
            for (i = 0; i < lNum; i++)
            {
                pcen_link = links[i];
                if (pcen_link->link->lclIfAddr == hop_back && pcen_link->reverse_link != NULL && pcen_link->link->type == RTYPE_GLO_ABS_LNK)
                {
                    hopBackInterdomainPcenLink = pcen_link;
                }
            }
            //Moving source one hop back!!
            if (hopBackInterdomainPcenLink && hopBackInterdomainPcenLink->rmt_end && hopBackInterdomainPcenLink->rmt_end->router)
            {
                hopback_source = hopBackInterdomainPcenLink->rmt_end->router->id;
            }
            else
            {
                LOGF("Note: PCEN_MRN::PostBuildTopology did not change source address based on the hop_back interface 0x%x\n", hop_back);
            }

            tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
            node = tree->Root();
            while (node)
            {
                Link* link = (Link*)node->Data();
                if ( link && link->lclIfAddr != 0 && link->rmtIfAddr == 0 && link->iscds.size() > 0 
                    && link->iscds.front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC 
                    && (ntohs(link->iscds.front()->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) ) 
                { // if found an intra-domain subnet edge link
                    hopBackInterdomainPcenLink = NULL;
                    for (i = 0; i < lNum; i++)
                    {
                        pcen_link = links[i];
                        if (pcen_link->link->lclIfAddr == link->lclIfAddr && pcen_link->reverse_link != NULL && pcen_link->link->type == RTYPE_GLO_ABS_LNK)
                        {
                            //hop_back verified against the attaching-to subnet_vlsr, change source node now
                            if ( hopback_source !=0 && pcen_link->link->LclIfAddr() == hop_back && source.s_addr == pcen_link->link->AdvRtId())
                            {
                                source.s_addr = hopback_source;
                            }
                            //record the found hop back link candidate
                            hopBackInterdomainPcenLink = pcen_link;
                        }
                    }
                    // fake hop-back intra-domain links will be added to replace the interdomain (border) links
                    if (hopBackInterdomainPcenLink != NULL) // patten recoginized
                    {
                        //adding two new links into the PCEN topology
                        PCENLink* linkForward = NewTransitLink(links);
                        PCENLink* linkHopback = NewTransitLink(links);
                        lNum += 2;
                        PCENNode* nodeHead = hopBackInterdomainPcenLink->rmt_end;
                        PCENNode* nodeTail = hopBackInterdomainPcenLink->lcl_end;
                        assert(nodeHead && nodeTail);
            
                        // removing all links from nodeHead if it is a hop_back source
                        if (nodeHead->router->id == source.s_addr && hopBackInterdomainPcenLink->link->lclIfAddr == hop_back)
                        {
                            nodeHead->out_links.clear();
                            nodeHead->in_links.clear();
                        }
            
                        // allocating link resource and updating link parameters for forward link
                        assert(hopBackInterdomainPcenLink->reverse_link->link);
                        linkForward->link = new Link(hopBackInterdomainPcenLink->reverse_link->link);
                        linkForward->link_self_allocated = true;
                        linkForward->link->type = RTYPE_LOC_PHY_LNK;
                        linkForward->lcl_end = nodeHead;
                        linkForward->rmt_end = nodeTail;
                        nodeHead->out_links.push_back(linkForward);
                        nodeTail->in_links.push_back(linkForward);
            
                        // allocating link resource and updating link parameters for backward (hop back) link
                        linkHopback->link = new Link(link);
                        linkHopback->link_self_allocated = true;
                        linkHopback->link->rmtIfAddr = hopBackInterdomainPcenLink->link->rmtIfAddr;
                        linkHopback->lcl_end = nodeTail;
                        linkHopback->rmt_end = nodeHead;
                        nodeHead->in_links.push_back(linkHopback);
                        nodeTail->out_links.push_back(linkHopback);
            
                        // assigning reverse links for the links in both directions
                        linkHopback->reverse_link = linkForward;
                        linkForward->reverse_link = linkHopback;
            
                        //removing the links from the PCEN topology
                        hopBackInterdomainPcenLink->linkID = -1;
                        hopBackInterdomainPcenLink->reverse_link->linkID = -1;
                    }
                }
                node = tree->NextNode(node);
            }
        }

        // 'jump' link construction logic...
        for (i = 0; i < lNum; i++)
        {
            pcen_link = links[i];
            if (pcen_link && pcen_link->link && pcen_link->linkID != -1) 
            {
                iscd_iter = pcen_link->link->Iscds().begin();
                for ( ; iscd_iter != pcen_link->link->Iscds().end(); iscd_iter++)
                {
                    iscd = *iscd_iter;
                    if(!iscd)	
                        continue;

                    if ( (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC) && (ntohs(iscd->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) ) 
                    {
                        for (j = 0; j < rNum; j++)
                        {
                            pcen_node = routers[j];
                            if ( pcen_node->router && pcen_node->router->id == iscd->subnet_uni_info.nid_ipv4 &&  pcen_node->router->id != pcen_link->link->advRtId )
                            {
                                if (!pcen_link->reverse_link || !pcen_link->reverse_link->link)
                                    continue;
                                // skip the formed intra-domain links that are also local-id edge links
                                if ((lclid_link_src && lclid_link_src->link && pcen_link != lclid_link_src && pcen_link->link->LclIfAddr() == lclid_link_src->link->LclIfAddr())
                                    || (lclid_link_dest && lclid_link_dest->link && pcen_link!= lclid_link_dest && pcen_link->link->LclIfAddr() == lclid_link_dest->link->LclIfAddr()))
                                    continue;
                                // remove the links from RDB.
                                if (!pcen_link->link_self_allocated)
                                    RDB.Remove(pcen_link->link);
                                if (!pcen_link->reverse_link->link_self_allocated)
                                    RDB.Remove(pcen_link->reverse_link->link);

                                // change IDs of current RDB link and its reverse link as 'jump' links
                                pcen_link->link->advRtId = pcen_node->router->advRtId;
                                // link->id and reverse_link->advRtId unchanged
                                pcen_link->reverse_link->link->id = pcen_node->router->id;
                                // link and reverse_link data interface addresses unchanged

                                // setting homeVLSR ... for multi-home-vlsr only
                                // pcen_node->home_vlsr = pcen_link->lcl_end;

                                //change link and rlink switching capability
                                iscd->swtype = iscd->subnet_uni_info.swtype_ext;
                                iscd->encoding = iscd->subnet_uni_info.encoding_ext;

                                // remove current pcen_link and its reverse link from the original VLSR pce_node
                                assert(pcen_link->lcl_end);
                                link_iter = pcen_link->lcl_end->out_links.begin();
                                while (link_iter != pcen_link->lcl_end->out_links.end())
                                {
                                    if((*link_iter) == pcen_link)
                                        link_iter = pcen_link->lcl_end->out_links.erase(link_iter);
					 else
                                        link_iter++;
                                }
                                link_iter = pcen_link->lcl_end->in_links.begin();
                                while (link_iter != pcen_link->lcl_end->in_links.end())
                                {
                                    if((*link_iter) == pcen_link->reverse_link)
                                        link_iter = pcen_link->lcl_end->in_links.erase(link_iter);
                                    else
                                        link_iter++;
                                }

                                // connect current pcen_link and its reverse link to current pce_node
                                pcen_link->lcl_end = pcen_node;
                                pcen_link->reverse_link->rmt_end = pcen_node;

                                pcen_node->out_links.push_front(pcen_link);
                                pcen_node->in_links.push_front(pcen_link->reverse_link);

                                // change 'jump link' metric 
                                pcen_link->link->metric = 0;
                                pcen_link->reverse_link->link->metric = 0;

                                // Update link pointers in the links vector ...
                                for (k = 0; k < lNum; k++)
                                {
                                    pcen_link2 = links[k];
                                    if ( k == i || !pcen_link2 || !pcen_link2->link || pcen_link2->linkID == -1 )
                                        continue;
                                    if ( pcen_link2->link && pcen_link2->link->Index() == pcen_link->link->Index()) 
                                    {
                                        pcen_link2->linkID = -1; //indicating removal after the loop
                                        pcen_link2->reverse_link->linkID = -1;
                                    }
                                }

                                // re-plant the links into RDB.
                                // !!!! Note: The original links will be restored by OSPF refresh
                                if (!pcen_link->link_self_allocated)
                                    RDB.Update(pcen_link->link);
                                if (!pcen_link->reverse_link->link_self_allocated)
                                    RDB.Update(pcen_link->reverse_link->link);
                            }
                        }
                    }
                }
            }
        }

        // removing trashed pcen_links
        for (i = 0; i < links.size(); i++)
        {
            pcen_link = links[i];
            if ( pcen_link->linkID == -1 )
            {
               pcen_link->lcl_end->out_links.remove(pcen_link);
               pcen_link->rmt_end->in_links.remove(pcen_link);
               links.erase(links.begin()+i);
               delete pcen_link;
               if (i == links.size())
                   break;
               else
                   i--;
            }
        }
    }

    return true;

}

void PCEN_MRN_CG::DisplayPCENLink(PCENLink* theLink)
{
	in_addr ip, ip1, ip2;
	int linkMetric;
	ip.s_addr = theLink->link->advRtId;
	ip1.s_addr = theLink->link->lclIfAddr;
	ip2.s_addr = theLink->link->rmtIfAddr;
	linkMetric = theLink->link->metric;
	char addr[20], addr1[20], addr2[20];
	inet_ntop(AF_INET, &ip, addr, 20);
	inet_ntop(AF_INET, &ip1, addr1, 20);
	inet_ntop(AF_INET, &ip2, addr2, 20);
	cout << "LINK@" << addr << "[" << addr1 << "-" <<addr2 << "], "<<linkMetric<<endl;
}

void PCEN_MRN_CG::CreateNetLinks()
{	
	unsigned int k=0;
	bool duplex = false;

/*	vector<PCENLink*>::iterator net_link;
	for(net_link = netLinks.begin(); net_link != netLinks.end(); net_link++)
   		if(*net_link)
			delete *net_link;
	netLinks.clear();

/*	for(unsigned int i=0; i<links.size(); i++)
	{
		k=0;
		if(netLinks.size() > 0){
			for (unsigned int j=0; j<netLinks.size(); j++) 
			{
				if((netLinks[j]->link->lclIfAddr == links[i]->link->lclIfAddr) && (netLinks[j]->link->rmtIfAddr == links[i]->link->rmtIfAddr))
				{	
					cout<<"find duplex....."<<endl;
					duplex = true;
					k = j;
					break;
				}
			}
		}
		if(duplex &&  netLinks[k]->link->metric > links[i]->link->metric)
			netLinks[k]->link->metric = links[i]->link->metric;
		else if(!duplex)
			netLinks.push_back(links[i]);
		duplex = false;
	}
*/
	cout<<"In function CreateNetLinks......"<<links.size()<<endl;

	//	netLinks.reserve(links.size());

	chopLinks.clear();
	for(unsigned int i=0; i<links.size(); i++)
	{
		duplex = false;
		for (unsigned int j=0; j<links.size(); j++)
		{
			if(j == i)
				continue;
			if((links[j]->link->lclIfAddr == links[i]->link->lclIfAddr) && (links[j]->link->rmtIfAddr == links[i]->link->rmtIfAddr) && (links[j]->link->metric >= links[i]->link->metric))
			{
				chopLinks.push_back(j);
				cout<<"Chop link: "<<j<<endl;
				break;
			}
			
		}
	}
	
}


int PCEN_MRN_CG::CheckIfSingleLayer()
{
	u_char cmp_swtype = 0;
	list<ISCD*>::iterator iscd_iter;

	for(unsigned int j=0; j<links.size();j++)
		links[j]->lflg.lfg.maskoff=0;
	
	for(unsigned int i=0; i<links.size(); i++)
	{
		if(links[i]->link->iscds.size()==0)   //No switching type for this link, should not consider it for path computation
			links[i]->lflg.lfg.maskoff = 1;
		else if(links[i]->link->iscds.size() > 1)   //if the link has two switching types, which it is a multilayer network, return 0
			return 0;
		else
		{
			iscd_iter = links[i]->link->iscds.begin();
			if(cmp_swtype == 0)
				cmp_swtype = (*iscd_iter)->swtype;
			else if(cmp_swtype != (*iscd_iter)->swtype)    //if find different switching type, it is a multilayer network, return 0;
				return 0;
		}
	}
	return 1;
}



void PCEN_MRN_CG::Run()
{
   LOGF("In the Run() function of pcen_mrn_cg class. \n");
    int ret;
    Event::Run();
	
	
    if (is_e2e_tagged_vlan && vtag == 0)
    {
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        LOG("PCEN_MRN_CG::Run() : Requested vlan tag cannot be zero!" << endl);
        return;
    }

    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN_MRN_CG::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }
	

    ret = BuildTopology();

    if (ret != 0)
    {
        LOGF("PCEN_MRN_CG::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }
       AssignRefNumber(routers);

	 if (!PostBuildTopology())
        return;

    PCENNode* src_node = GetNodeByIp(routers,&source);
    PCENNode* dest_node = GetNodeByIp(routers,&destination);
    assert (src_node && dest_node);
    	
//    cout<<"source ID : "<<src_node->ref_num<<"destination ID:"<<dest_node->ref_num<<endl;
	LOGF("\nPCEN_MRN_CG class in latest code.....\n");
	LOGF("\nDisplaying network links before path computation.....\n");
	for(unsigned int i=0; i<links.size(); i++)
	{
		DisplayPCENLink(links[i]);
	}
	LOGF("End displaying links before path computation.....\n\n");
	
	CreateNetLinks();
/*	if(CheckIfSingleLayer())
	{
		LOGF("\nIt is a single layer network!!!\n");
		if((ret = SearchSLNPath(src_node->ref_num, dest_node->ref_num, bandwidth_egress)) != 0)
    		{
    	  		ReplyErrorCode(ret);
      	  		return;
    		}
	}
	else
		LOGF("\nIt is a multi-layer network!!!\n");
*/	

    if((ret = SearchMRNKSP(src_node->ref_num, dest_node->ref_num, switching_type_egress, encoding_type_egress, bandwidth_egress, SystemConfig::pce_k)) != 0)
    {
    	  LOGF("PCEN_MRN_CG::SearchMRNKSP() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
    	  ReplyErrorCode(ret);
      	  return;
    }
/*      LOGF("Displaying network links after path computation.....\n");
	for(unsigned int i=0; i<links.size(); i++)
	{
		LOGF("linkID [%X]\n", links[i]->linkID);
		LOGF("link type [%X]\n", links[i]->link->linkType);
		LOGF("link lcl addr [%X]\n", links[i]->link->lclIfAddr);
		LOGF("link rmt addr [%X]\n", links[i]->link->rmtIfAddr);
	}
*/	

	try
	{
		ShowERO();
		ReplyERO();
	}
	catch(...)
	{
		cout<<"exception catched in run() #######################"<<endl;
	}
    
}
