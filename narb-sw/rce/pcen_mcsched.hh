/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */
 
#ifndef __RCE_MCSCHED_HH__
#define __RCE_MCSCHED_HH__

#include "pcen_mcbase.hh"

class PCEN_MCSched: public PCEN_MCBase
{
public:
    PCEN_MCSched(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN_MCBase(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) 
        {  }
    void SetSchedulingTime(u_int32_t tstart, u_int32_t tend)
    {
        thePath.start_time.tv_sec = tstart;
        thePath.end_time.tv_sec = tend;
    }
    virtual ~PCEN_MCSched() {}

    virtual bool PostBuildTopology();
    virtual int PickMCPCandidates(int M);
    virtual int PerformComputation();
    virtual void Run();
    virtual bool VerifyPathConstraints(list<PCENLink*>& path, u_int32_t& pathVtag, u_int32_t& pathWave);
    virtual inline void SortTwoPaths(PathM* &path1, PathM* &path2);

    void AdjustLinkResourceBySchedule(PCENLink *L, bool doAddOrDelete);
};

#define MAX_SCHEDULE_DURATION 3600
#define BANDWIDTH_TIME_FACTOR 0.5
inline u_int32_t OverLappingTime(struct timeval &st1, struct timeval &et1, struct timeval &st2, struct timeval &et2);
inline u_int32_t GetPathOverLappingTime(PathM* path1, PathM* path2);

#endif

