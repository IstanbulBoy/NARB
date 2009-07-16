/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */
 
#ifndef __RCE_MCBASE_HH__
#define __RCE_MCBASE_HH__

#include "pcen_ksp.hh"

class PCEN_MCBase: public PCEN_KSP
{
protected:
    PathT thePath;
    vector<PathT*> MCPaths;
    vector<PathT> MC_KSP1;
    vector<PathT> MC_KSP2;
    vector<PathT> sortedMCPaths;

public:
    static vector<PathT*> allPaths;

public:
    PCEN_MCBase(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN_KSP(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, 
        bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) 
        { thePath.source.s_addr = source.s_addr; thePath.destination.s_addr = destination.s_addr; thePath.ucid = ucid; thePath.seqnum = seqnum;  thePath.bandwidth= bandwidth_ingress; }

    virtual ~PCEN_MCBase();
    virtual bool PostBuildTopology();
    virtual int PickMCPCandidates(int M); //new
    virtual int PerformComputation();
    virtual void Run(); //recording path into AllPaths
    virtual inline void SortTwoPaths(PathT* &path1, PathT* &path2);
    int GetBestTwoKSPaths(vector<PathT*>& KSP, PathT &path1, PathT &path2);
    void SortMPaths(vector<PathT*>& Paths);
};

#endif

