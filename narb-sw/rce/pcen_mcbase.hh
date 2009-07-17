/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */
 
#ifndef __RCE_MCBASE_HH__
#define __RCE_MCBASE_HH__

#include "pcen_ksp.hh"

class PathM {
public:
    in_addr source;
    in_addr destination;
    u_int32_t ucid;
    u_int32_t seqnum;
    list<Link*> path;
    double cost;
    float bandwidth;
    u_int32_t vlan_tag;
    u_int32_t wavelength;
    union PathFlag 
    {
        unsigned long flag;
        struct pathFlag 
        {
            unsigned visited: 1;
            unsigned filteroff: 1;
        } pfg;
    } pflg;

    PathM() {
        source.s_addr = destination.s_addr = 0;
        ucid = seqnum = 0;
        cost=0;
        path.clear();
        pflg.flag=0;
        bandwidth = 0;
        vlan_tag = 0;
        wavelength = 0;
    }

    inline PathM& operator=(const PathM& p);
    inline PathM& operator=(const PathT& p);
};


class PCEN_MCBase: public PCEN_KSP
{
protected:
    PathM thePath;
    vector<PathM*> MCPaths;
    vector<PathM> MC_KSP1;
    vector<PathM> MC_KSP2;
    vector<PathM> sortedMCPaths;

public:
    static vector<PathM*> allPaths;

public:
    PCEN_MCBase(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN_KSP(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) 
        { thePath.source.s_addr = source.s_addr; thePath.destination.s_addr = destination.s_addr; thePath.ucid = ucid; thePath.seqnum = seqnum;  thePath.bandwidth= bandwidth_ingress; }

    virtual ~PCEN_MCBase() {}
    virtual bool PostBuildTopology();
    virtual int PickMCPCandidates(int M); //new
    virtual int PerformComputation();
    virtual void Run(); //recording path into AllPaths
    virtual inline void SortTwoPaths(PathM* &path1, PathM* &path2);

    int GetBestTwoKSPaths(vector<PathT*>& KSP, PathM &path1, PathM &path2);
    void SortMPaths(vector<PathM*>& Paths);
    int GetPathERO(list<Link*>& path, list<ero_subobj>& ero);
    PCENLink* GetPCENLinkByLink(Link* link);
};

#endif

