/*
 * Copyright (c) 2008
 * DRAGON Project.
 * Created by Shujia Gong (George Mason University), 2004-2007
 * Modified by Qian Xu (George Mason University), 2007
 * Modified by Xi Yang (USC/ISI), 2008
 */
 
#ifndef __RCE_KSP_HH__
#define __RCE_KSP_HH__

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include "rce_pcen.hh"

using namespace std;

class PathT;

class PCEN_KSP: public PCEN
{
protected:
    vector<PathT*> KSP;

public:
    PCEN_KSP(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                 u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) {}
		
    virtual ~PCEN_KSP();
    void AddLink(int linkid, int localNodeId, int remoteNodeId);
    void AddLink(int linkid, int localNodeId, int remoteNodeId, double metric);
    void AddNode (int nodeid);
    void RestoreGraph();
    void ResetCost();
    void ResetVisitedFlag();
    void ClearPath();
    void OutputGraph();
    void DisplayNodes();
    void DisplayLinks();
    void Dijkstra(int source, int destination);
    void SearchKSP(int source, int destination, int K);
    PathT* ConstrainKSPaths(vector<PathT*>& KSP);
    bool VerifyPathConstraints(list<PCENLink*>& path, u_int32_t& pathVtag, u_int32_t& pathWave);
    PCENNode* search_PCENNode(int NodeId);
    void RestoreGraphKeepFilter();
    void MaskParentPath(PathT* ParentPath); // mark the link list of the parent path
    virtual bool PostBuildTopology();
    virtual void Run();
};

class PathT {
public:
    in_addr source;
    in_addr destination;
    u_int32_t ucid;
    u_int32_t seqnum;
    list<PCENLink*> path;				// Remember the path from source node to this node
    list<PCENLink*> MaskedLinkList;		// Remember the links masked before searching this path
    int DeviationNode;					// Node ID of deviation node along this path
    double cost;						// cost of this path
    float bandwidth;
    u_int32_t vlan_tag;
    u_int32_t wavelength;

    union PathFlag 
    {
        unsigned long flag;        // flag that indicates whether this path has been visited
        struct pathFlag 
        {
            unsigned visited: 1;
            unsigned filteroff: 1;
        } pfg;
    } pflg;
    PathT();
    void CalculatePathCost();
    void DisplayPath();
    bool operator< (const PathT& p) const { return cost<p.cost; }
    PathT& operator=(const PathT& p) {
        this->source.s_addr = p.source.s_addr;
        this->destination.s_addr = p.destination.s_addr;
        this->ucid = p.ucid;
        this->seqnum = p.seqnum;
        this->path.assign(p.path.begin(), p.path.end());
        this->MaskedLinkList.assign(p.MaskedLinkList.begin(), p.MaskedLinkList.end());
        this->DeviationNode = p.DeviationNode;
        this->cost = p.cost;
        this->bandwidth = p.bandwidth;
        this->vlan_tag = p.vlan_tag;
        this->wavelength = p.wavelength;
        this->pflg = p.pflg;
        return *this;
    }
    int GetERO(list<ero_subobj>& ero);
};

#endif

