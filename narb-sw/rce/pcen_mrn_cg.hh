/*
 * Copyright (c) 2008
 * DRAGON Project.
 * Created by Shujia Gong and Qian Xu (George Mason University), 2004-2008
 * Modified by Xi Yang (USC/ISI), 2008
 */
 
#ifndef __PCEN_MRN_CG_HH__
#define __PCEN_MRN_CG_HH__

#include "rce_types.hh"
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include "rce_pcen.hh"
#include "pcen_ksp.hh"
using namespace std;

class PCENCGLink;
class PCEN_MRN_CG;
class PCENCGNode : public PCENNode
{
public:
    int lcl_endID;
    int rmt_endID;
    u_char linkType;
    u_char swType;
    u_char encoding;//changed 07/14
    float bandwidth;//changed 07/15
    double LinkMetric;
    list<PCENCGLink*> out_CGlinks;
    list<PCENCGLink*> in_CGlinks;
    list<PCENCGLink*> path_mrn;
    friend class PCEN_MRN_CG;
public:
    PCENCGNode(): PCENNode() {}
    PCENCGNode(int id) : PCENNode(id) {}

    void FilterCGNode();
    void RestoreCGNodeFilter();

    void MaskCGNode();
    void RestoreCGNodeMask();
    bool operator< (const PCENCGNode& node) const { return minCost<node.minCost; }
};

class PCENCGLink : public PCENLink
{
private:
	double cg_metric; //add by qian 03/16/2007 to record the metric of the PCENCGLink

public:
	PCENCGNode* lcl_end;
	PCENCGNode* rmt_end;

	double PCENCGmetric ();        // add by qian to return the channel graph link metric
	void SetPCENCGmetric (double x); 	// add by qian to set the channel graph link metric

    PCENCGLink(int id, int localNodeId, int remoteNodeId, vector<PCENCGNode *> &CGnodes);
    PCENCGNode* search_PCENCGNode(vector<PCENCGNode*> &CGnodes, int NodeId);
};

class PathT_MRN : public PathT
{
public:
    list<PCENCGLink*> path_mrn;
    list<PCENCGLink*> MaskedLinkList_mrn;

    PathT_MRN();
    void CalculatePathCost_MRN();
    void DisplayPath_MRN();
};

class PCEN_MRN_CG: public PCEN_KSP
{
protected:
    vector<PCENCGNode*> CGnodes; //added by qian, the channel graph node
    vector<PCENCGLink*> CGlinks; //added by qian, the channel graph link
    vector<int> chopLinks;
    vector<PathT_MRN*> KSP_MRN; //added by qian, the paths in channel graph
    list< list<PCENLink*> > Net_Paths;  // added by qian 03/20/2007, the path in network grpah

public:
   PCEN_MRN_CG(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                 u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL);

//old one
/*PCEN_MRN_CG(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t lspq_id, u_int32_t msg_seqnum, u_int32_t tag = 0, narb_lsp_vtagmask_tlv* vtm = NULL);*/
    virtual ~PCEN_MRN_CG();
    void AddCGLink(int linkid, int localNodeId, int remoteNodeId, double metric, int passNode);//added by qian
    void AddLink(int linkid, int localNodeId, int remoteNodeId, double metric, u_char swtype1);//added by qian
    void AddLink(int linkid, int localNodeId, int remoteNodeId, double metric, u_char swtype1,u_char encoding,float bandwidth);//changed 07/14
    void AddLink(int linkid, int localNodeId, int remoteNodeId, double metric, u_char swtype1,u_char encoding1,float bandwidth1,u_char swtype2,u_char encoding2,float bandwidth2, bool adapt);//added by qian
    void AddCGNode (int nodeid,int lclID,int rmtID,u_char swtype,u_char encoding,float bandwidth,double vMetric, u_char type);   
    void CreateNetLinks();
    bool BuildEndNodes(int source, int end, u_char swtype,u_char encoding,float bandwidth);//changed 07/14
    list<PCENLink*> CGPathToNetPath(list<PCENCGLink*> cgPath); // added by qian 20/03/2007 to transfer path from CG to NG
    void ConstructNetworkPath();  // added by qian 20/03/2007 to transfer paths from channel graph to network graph
    void DisplayNetworkPath(); // added by qian 20/03/2007 to display the path in network grpah
    void AddLinkToERO(PCENLink* pcen_link); //added by qian 09/04/2007 to add a network link to the ero
    void SetVTagToERO(u_int16_t vlan_tag);// added by qan 09/04/2007 to set one vlan tag to ERO
    void SetVTagMask(ConstraintTagSet& vtagset); // added by qan 09/04/2007 to set a vlan tag set to ERO
    void DeleteVirtualNodes();//added by qian
    void RestoreCGGraph();//added by qian
    void ResetCGCost();//added by qian
    void ResetCGVisitedFlag();//added by qian
    void ClearCGPath();//added by qian
    void DisplayCGNodes();//added by qian
    void DisplayCGLinks();//added by qian
    void DisplayPCENLink(PCENLink* theLink);
    void ShowMarkedLinks();//added by qian
    void DijkstraMRN(int source, int destination);//added by qian
    int SearchMRNKSP(int source, int destination, u_char swtype, u_char encoding, float bandwidth, int K);
//    int SearchSLNPath(int source, int destination, float bandwidth);
    PCENCGNode* search_PCENCGNode(int nodeid);//added by qian
    PCENCGNode* search_PCENCGNode(int lclID, int rmtID, u_char swtype);//added by qian
    void MaskParentPath(PathT* ParentPath); // mark the link list of the parent path
    void MaskParentPath_MRN(PathT_MRN* ParentPath);//added by qian
    void CreateChannelGraph(float bandwidth);//changed 07/14    
    void OutputKSP_MRN();
    void AssignRefNumber(vector<PCENNode*>& routers);
    int FindSpecificTagForPath(int vtag, list<PCENLink*> path);
    ConstraintTagSet FindVlanTagSetForPath(ConstraintTagSet head_vtagset, list<PCENLink*> path, bool any_tag);
    int FindTagInLink(int request_tag, int link_num,PCENLink* oneLink);
    ConstraintTagSet FindVtagSetInLink(int link_num, ConstraintTagSet head_set, PCENLink* oneLink, bool any_tag);
    int CheckAllocatedTags(PCENLink* oneLink, int request_tag);
    int CheckIfSingleLayer();  // added 03/20/2008
    void ShowERO();
    virtual bool PostBuildTopology();
    virtual void Run();
};

#endif

