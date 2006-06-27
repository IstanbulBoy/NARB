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
    PCEN_KSP(in_addr src, in_addr dest, u_int8_t sw_type, u_int8_t encoding, float bw, u_int32_t opts, u_int32_t lspq_id, u_int32_t msg_seqnum):
        PCEN(src, dest, sw_type, encoding, bw, opts, lspq_id,  msg_seqnum) {}
    ~PCEN_KSP();
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
    PCENNode* search_PCENNode(int NodeId);
    void RestoreGraphKeepFilter();
    void MaskParentPath(PathT* ParentPath); // mark the link list of the parent path
    virtual void Run();
};

class PathT {
public:
    list<PCENLink*> path;				// Remember the path from source node to this node
    list<PCENLink*> MaskedLinkList;		// Remember the links masked before searching this path
    int DeviationNode;					// Node ID of deviation node along this path
    double cost;						// cost of this path
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
    int GetERO(list<ero_subobj>& ero);
};

