/*
 * Copyright (c) 2009
 * KDDI Path Computation Project.
 * Created by  Xi Yang (USC/ISI), 2009
 */
 
#ifndef __RCE_DORA_HH__
#define __RCE_DORA_HH__

#include "pcen_mcsched.hh"


#define PCEN_DORA_MAX_NODE_NUM 50
#define PCEN_DORA_MAX_LINK_NUM 200
#define BWP 0.7 

//Dynamic Online Routing Algorithm (DORA) module
class PCEN_DORA: public PCEN_MCSched
{
public:
    static bool doraInitiated;
    static list<PathT*>* DP[PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_NODE_NUM]; //S*PCEN_DORA_MAX_NODE_NUM + D
    static double PPV[PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_LINK_NUM]; //S*PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_LINK_NUM + D*PCEN_DORA_MAX_LINK_NUM + L

public:
    PCEN_DORA(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid, u_int32_t msg_seqnum, u_int32_t lspb_id = 0, u_int32_t tag = 0, u_int32_t hopback = 0, 
                 u_int32_t src_lclid = 0,  u_int32_t dest_lclid = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
        PCEN_MCSched(src, dest, sw_type_ingress, encoding_ingress, bw_ingress,  sw_type_egress, encoding_egress, bw_egress,opts, ucid, msg_seqnum, lspb_id, tag, hopback, src_lclid, dest_lclid, vtm) {}

    virtual ~PCEN_DORA() { }
    virtual bool PostBuildTopology();
    virtual int PerformComputation();
    virtual void Run();

    list<PathT*>* ComputeDPByKSP(int I, int J);
    double GetDPLinkCount(int I, int J, int L);
};

#define DP_ARRAY(X, Y) PCEN_DORA::DP[X*PCEN_DORA_MAX_NODE_NUM + Y]
#define PPV_ARRAY(X, Y, Z) PCEN_DORA::PPV[X*PCEN_DORA_MAX_NODE_NUM*PCEN_DORA_MAX_NODE_NUM + Y*PCEN_DORA_MAX_NODE_NUM +Z]

#endif

