/*
 * Copyright (c) 2007
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2007
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __RCE_LSP_H__
#define __RCE_LSP_H__
#include "rce_api.hh"
#include "resource.hh"

// TLV's wrapped in the LSP querry message.
// The types of wrapped TLV's are indicated in msgtag[0].
#define LSP_TLV_NARB_REQ    0x0001
#define LSP_OPT_REQ_ALL_VTAGS  0x0002 //Ask RCE to return a list of available vlan tags instead of just one.
#define LSP_OPT_REQ_ALL_WAVES  0x0004  //Ask RCE to return a list of available wavelengths instead of just one.

// Indicating layers excluded from path computation
#define LSP_OPT_EXCLUD_L1      0x0010
#define LSP_OPT_EXCLUD_TDM   0x0020
#define LSP_OPT_EXCLUD_L2      0x0040
#define LSP_OPT_EXCLUD_L3      0x0080

// LSP computation options
#define LSP_OPT_STRICT  ((u_int32_t)(0x0001 << 16)) //otherwise LSP_OPT_LOOSE
#define LSP_OPT_PREFERRED ((u_int32_t)(0x0002 << 16)) //otherwise LSP_OPT_ONLY
#define LSP_OPT_MRN ((u_int32_t)(0x0004 << 16))
//#define LSP_OPT_MRN_RELAY ((u_int32_t)0x08 << 16)  //relay path computation for multiregion multidomain networks
#define LSP_OPT_BIDIRECTIONAL ((u_int32_t)(0x0010 << 16))
#define LSP_OPT_E2E_VTAG  ((u_int32_t)(0x0020 << 16)) //otherwise Untgged VLAN for E2E Ethernet
#define LSP_OPT_VIA_MOVAZ  ((u_int32_t)(0x0040 << 16)) //using MOVAZ proprietary TLVs for optical layer routing
#define LSP_OPT_VTAG_MASK  ((u_int32_t)(0x0080 << 16)) //Privodes a 512 bytes vtag mask to constrain the available taggs
#define LSP_OPT_QUERY_HOLD  ((u_int32_t)(0x0100 << 16)) //holding resource upon query for a short period of time to resolve contention
// The below options are for NARB only
//#define LSP_OPT_QUERY_CONFIRM  ((u_int32_t)(0x0200 << 16)) //holding resource upon query and return confirmation ID (instead of strict ERO hops)
#define LSP_OPT_SUBNET_ERO  ((u_int32_t)(0x0400 << 16)) //returning subnet ERO TLV if any
#define LSP_OPT_SUBNET_DTL  ((u_int32_t)(0x0800 << 16)) //returning subnet DTL TLV if any
#define LSP_OPT_ALT_PATHS  ((u_int32_t)(0x1000 << 16)) //returning alternate paths (ERO + subnetERO)

#define ANY_VTAG 0xffff  //Indicating that LSP uses any available E2E VLAN Tag
#define ANY_WAVE 0xffff  //Indicating that LSP uses any available Wavelength for optical layer routing
#define ANY_TIMESLOT 0xff  //Timeslot in TDM layer

// the internal data type defined for the sub-object of ERO
struct ero_subobj
{
    struct in_addr addr;
    u_char hop_type;
    u_char prefix_len;
    u_char pad[2];
    u_int32_t if_id;
    //added parameters in the private, composite ERO sub-object
    u_char sw_type;
    u_char encoding;
    union {
        u_int16_t lsc_lambda;
        u_char tdm_indication;
        u_int16_t l2sc_vlantag;
        u_int16_t psc_mtu;
    };
    float bandwidth;
};

// $$$$ DCN-Subnet special handling
// Structure of DTL hop
#define MAX_DTL_NODENAME_LEN 19
struct dtl_hop 
{
    u_int8_t nodename[MAX_DTL_NODENAME_LEN+1]; //19-char C string
    u_int32_t linkid;  //link ID number
};

// definitions of loose/strict hop indicator
#define ERO_TYPE_STRICT_HOP 0
#define ERO_TYPE_LOOSE_HOP 1

#define LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL 0x0004 //the localid type for an intermediate tagged vlan link (ERO subobject)
#define LOCAL_ID_TYPE_SUBNET_IF_ID (u_int16_t)0x5
#define LOCAL_ID_TYPE_SUBNET_UNI_SRC (u_int16_t)0x10 	//Source (sender)
#define LOCAL_ID_TYPE_SUBNET_UNI_DEST (u_int16_t)0x11	//Destination (Recv)
    

enum  narb_tlv_type
{
    TLV_TYPE_NARB_REQUEST = 0x02,
    TLV_TYPE_NARB_ERO = 0x03,
    TLV_TYPE_NARB_ERROR_CODE = 0x04,
    TLV_TYPE_NARB_VTAG_MASK = 0x05,
    TLV_TYPE_NARB_HOP_BACK = 0x06,
    TLV_TYPE_NARB_LSPB_ID = 0x08,
    TLV_TYPE_NARB_SUBNET_ERO = 0x09,
    TLV_TYPE_NARB_LOCAL_ID = 0x0A,
    TLV_TYPE_NARB_SUBNET_DTL = 0x0B,
    TLV_TYPE_NARB_HOLDING_TIME = 0x10,
    TLV_TYPE_NARB_ALTERNATE_ERO = 0x11,
    TLV_TYPE_NARB_ALTERNATE_SUBNET_ERO = 0x12,
    TLV_TYPE_NARB_PEER_REQUEST = 0x41,
};

struct narb_lsp_request_tlv
{
    u_int16_t type;
    u_int16_t length;
    struct in_addr src;
    struct in_addr dest;
    u_int8_t  encoding_type;
    u_int8_t  switching_type;
    u_int16_t gpid;
    float bandwidth;
};

struct narb_lsp_vtagmask_tlv
{
    u_int16_t type;
    u_int16_t length;
    u_char bitmask[MAX_VLAN_NUM/8];
};

struct narb_lsp_hopback_tlv
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t ipv4;
};

struct narb_lsp_lspb_id_tlv
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t lspb_id;
};

struct narb_lsp_local_id_tlv
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t lclid_src;
    u_int32_t lclid_dest;
};

struct narb_lsp_subnet_ero_tlv
{
    u_int16_t type;
    u_int16_t length;
    ero_subobj subobjects[1]; //acually number of subobjects depends on length
};

struct narb_lsp_subnet_dtl_tlv
{
    u_int16_t type;
    u_int16_t length;
    dtl_hop hops[1]; //acually number of dtl_hops depends on length
};

struct narb_lsp_holding_time_tlv
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t seconds;
};

// data structure of an IPv4 prefix type ERO sub-object
struct ipv4_prefix_subobj
{
    u_char l_and_type;
    u_char length;
    u_char addr[4];
    u_char prefix_len;
    u_char resvd;
};

// data structure of an IPv4 prefix type ERO sub-object
struct unum_if_subobj
{
    u_char l_and_type;
    u_char length;
    u_char resvd[2];
    in_addr addr;
    u_int32_t ifid;
};

class LSPHandler: public Event
{
private:
    in_addr source;
    in_addr destination;
    u_int8_t  encoding_type_ingress;
    u_int8_t  switching_type_ingress;   
    float bandwidth_ingress;
    u_int8_t  encoding_type_egress;
    u_int8_t  switching_type_egress;   
    float bandwidth_egress;

    u_int32_t options;
    u_int32_t tag;
    u_int32_t ucid;
    u_int32_t seqnum;
    u_int32_t lspb_id;
    u_int32_t uptime;
    u_int32_t duration;

    narb_lsp_vtagmask_tlv* vtag_mask;
    u_int32_t hop_back;
    u_int32_t src_lcl_id;
    u_int32_t dest_lcl_id;
    list<ero_subobj> subnet_ero;
    bool is_subnet_ero2dtl_enabled;

    int caller_fd;

    APIWriter* api_writer;
    void Init ()
        {  source.s_addr = 0; destination.s_addr = 0; encoding_type_ingress = encoding_type_egress = 0; 
            switching_type_ingress = switching_type_egress = 0; bandwidth_ingress = bandwidth_egress =0;
            options = 0; tag = 0; ucid = seqnum = 0xffffffff; lspb_id = 0; uptime = 0; duration = 0xffffffff;
            api_writer = NULL;  vtag_mask = NULL; hop_back = 0; src_lcl_id = dest_lcl_id = 0; is_subnet_ero2dtl_enabled = false;}
public:
    LSPHandler(int fd): caller_fd(fd) { Init();}
    virtual ~LSPHandler() { if (vtag_mask) delete vtag_mask; }
    void Load(api_msg *msg);
    virtual void Run();
    void AssociateWriter (APIWriter *writer) { assert (writer); api_writer = writer; }
    void SetOptionalConstraints (api_msg* msg);

public:
    static void GetERO_RFCStandard(te_tlv_header* tlv, list<ero_subobj>& ero);
    static void GetDTL(te_tlv_header* tlv, list<dtl_hop>& dtl);
    static void HandleResvNotification(api_msg* msg);
    static void UpdateLinkStatesByERO(narb_lsp_request_tlv& req_data, list<ero_subobj>& ero_reply, u_int32_t ucid, u_int32_t seqnum,  bool is_bidir=true, u_int32_t lsp_vtag=0, u_int32_t lclid_src=0, u_int32_t lclid_dest=0, narb_lsp_vtagmask_tlv* vtag_mask=NULL, u_int32_t holding_time = 0);
    static void HandleLinkStateDelta(narb_lsp_request_tlv& req_data, Link* link1, u_int32_t ucid, u_int32_t seqnum, u_int32_t vtag=0, u_int32_t if_id=0, narb_lsp_vtagmask_tlv* vtag_mask=NULL, u_int32_t holding_time = 0);
};

#endif
