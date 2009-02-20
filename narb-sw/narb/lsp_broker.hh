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
#ifndef __LSPBROKER_H__
#define __LSPBROKER_H__

#include "types.hh"
#include "apiserver.hh"
#include "narb_apiserver.hh"
#include "lsa.hh"
#include "rce_apiclient.hh"
using namespace std;
#include <vector>

// definition of NARB API msg types
enum  app2narb_request_type
{
    MSG_APP_REQUEST = 0x01,
    MSG_REPLY_ERO = 0x21,
    MSG_REPLY_ERROR = 0x22,
    MSG_REPLY_REMOVE_CONFIRM = 0x23,
    MSG_REPLY_CONFIRMATION_ID = 0x24,
    MSG_PEER_REQUEST = 0x41,
};

// each NARB<->APP contains a TLV in its message body
enum  narb_tlv_type
{
    TLV_TYPE_NARB_REQUEST = 0x02,
    TLV_TYPE_NARB_ERO = 0x03,
    TLV_TYPE_NARB_ERROR_CODE = 0x04,
    TLV_TYPE_NARB_VTAG_MASK = 0x05,
    TLV_TYPE_NARB_HOP_BACK = 0x06,
    TLV_TYPE_NARB_SUGGESTED_VTAG = 0x07,
    TLV_TYPE_NARB_LSPB_ID = 0x08,
    TLV_TYPE_NARB_SUBNET_ERO = 0x09,
    TLV_TYPE_NARB_LOCAL_ID = 0x0A,
    TLV_TYPE_NARB_SUBNET_DTL = 0x0B,
    TLV_TYPE_NARB_HOLDING_TIME = 0x10,
    TLV_TYPE_NARB_ALTERNATE_ERO = 0x11,
    TLV_TYPE_NARB_ALTERNATE_SUBNET_ERO = 0x12,
    TLV_TYPE_NARB_USER_SUPPLIED_ERO = 0x13,
    TLV_TYPE_NARB_APP_CONFIRM = 0x031,
    TLV_TYPE_NARB_APP_REMOVE = 0x032,
    TLV_TYPE_NARB_PEER_REQUEST = 0x41,
    TLV_TYPE_NARB_PCE_SPEC = 0x42,
};

// data structure of APP->NARB request message
struct msg_app2narb_request
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

// data structure of APP->NARB confirmation and removal messages
struct msg_narb_confirm
{
    msg_app2narb_request req;
    te_tlv_header ero;
};

// data structure of APP->NARB special PCE requirement (optional)
struct msg_narb_pce_spec
{
    u_int16_t type;
    u_int16_t length;
    char module_name[16]; //c-string of length up to 15 characters
    union {
    u_int32_t flags;
    u_int8_t reserved[16]; //reserved bytes for future extension
    };
};

// APP->NARB optional constraint structs
struct msg_narb_vtag_mask
{
    u_int16_t type;
    u_int16_t length;
    u_char bitmask[MAX_VLAN_NUM/8];
};

struct msg_narb_hop_back
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t ipv4;
};

struct msg_narb_suggested_vtag
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t suggested_vtag;
};

struct msg_narb_lspb_id
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t lspb_id;
};

struct msg_narb_local_id
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t lclid_src;
    u_int32_t lclid_dest;
};


struct msg_narb_holding_time
{
    u_int16_t type;
    u_int16_t length;
    u_int32_t seconds;
};


// $$$$ DCN-Subnet special handling
// Structure of DTL hop
#define MAX_DTL_NODENAME_LEN 19
struct dtl_hop 
{
    u_int8_t nodename[MAX_DTL_NODENAME_LEN+1]; //19-char C string
    u_int32_t linkid;  //link ID number
};

struct msg_narb_subnet_dtl
{
    u_int16_t type;
    u_int16_t length;
    dtl_hop hops[1]; //actual number of dtl_hops depends on length
};

#define msg_app2narb_release msg_narb_confirm

class LSP_Broker;
struct ero_subobj;
class LSPQ//app_lspq_data (FSM)
{
private:
    LSP_Broker *broker;  //the LSP Broker handling this LSPQ
    u_int32_t req_ucid;
    u_int32_t app_seqnum; //app information; can be used to identify the LSPQ
    u_int32_t app_options;
    u_int32_t req_vtag;
    msg_app2narb_request req_spec;    // information extracted from the request message
    msg_app2narb_request mrn_spec;    // for multi-region networks
    msg_narb_pce_spec* pce_spec;
    msg_narb_vtag_mask* vtag_mask;
    msg_narb_suggested_vtag* suggested_vtag;
    u_int32_t previous_lspb_id;
    u_int32_t returned_lspb_id;
    u_int32_t src_lcl_id;
    u_int32_t dest_lcl_id;
    u_int32_t hop_back;
    list<ero_subobj*> user_ero; // Optional user spplied ERO for TLV_TYPE_NARB_USER_SUPPLIED_ERO
    list<ero_subobj*> subnet_ero; // Optional subnet ERO for TLV_TYPE_NARB_SUBNET_ERO
    list<dtl_hop> subnet_dtl; // Optional subnet ERO for TLV_TYPE_NARB_SUBNET_DTL
    bool is_recursive_req;
    bool is_qconf_mode;
    
    //state of the current LSPQ request, values defined below
    u_char state;
#define STATE_IDLE 0
#define STATE_APP_REQ 1
#define STATE_RCE_REPLY 2
#define STATE_ERO_PARTIAL 3
#define STATE_NEXT_HOP_NARB_REPLY 4
#define STATE_ERROR 5
#define STATE_ERO_COMPLETE 6
#define STATE_REQ_RECURSIVE 7
#define STATE_RESV_CONFIRM 8
#define STATE_RESV_RELEASE 9
#define STATE_RESV_RELEASE_CONFIRM 10
#define STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID 11
#define STATE_STORED_ERO_WITH_CONFIRMATION_ID 12

    // sub states values are defined below
    u_char ero_state;
#define ERO_NONE 0
#define ERO_ALL_STRICT 1
#define ERO_ALL_LOOSE 2
#define ERO_HYBRID 3
#define ERO_INCOMPLETE 4
    // Used in joint with narb_apiserver->routing_mode

    list<ero_subobj*> ero; // A list of type ero_subobj generated from RCE's reply

    // the number of request retransmissions, [0, MAX_REQ_RETRAN]
    int req_retran_counter;
#define MAX_REQ_RETRAN 3

public:
    LSPQ(LSP_Broker* b, in_addr s, in_addr d, u_int32_t bw, u_char swt, u_char enc, u_int32_t seq, u_int32_t op = 0);
    LSPQ(LSP_Broker* b, msg_app2narb_request& r, u_int32_t seq, u_int32_t op = 0);
    LSPQ(LSP_Broker* b, msg_app2narb_request& r,  msg_app2narb_request& mr, u_int32_t seq, u_int32_t op = 0);
    ~LSPQ();
    void Init();
    u_int32_t ReqUcid() { return req_ucid; }
    void SetReqUcid(u_int32_t x) { req_ucid = x; }
    void SetReqVtag(u_int32_t x) { req_vtag = x; }
    void SetReqOptions(u_int32_t x) { app_options = x; }
    void SetReqAppMsg(msg_app2narb_request& r) { req_spec = r; }
    void SetReqMrnMsg(msg_app2narb_request& mr) { mrn_spec = mr; }
    u_char State() {  return state;  }
    void SetState(u_char s);
    void HandleOptionalRequestTLVs (api_msg* msg);
    void HandleOptionalResponseTLVs (api_msg* msg);
    void DescribeLSP(string& desc);
    void DescribeLSPDetail(vector<string>& lsp_detail_v);
    void DescribeLSPWithDTL(vector<string>& lsp_dtl_v);
    u_int32_t SeqNum() {  return app_seqnum;  }
    static void GetERO(te_tlv_header* ero_tlv, list<ero_subobj*>& ero);
    static void GetERO_RFCStandard(te_tlv_header* ero_tlv, list<ero_subobj*>& ero);
    static int MergeERO(list<ero_subobj*>& ero_inter, list<ero_subobj*>& ero_intra);
    static int ForceMergeERO(list<ero_subobj*>& ero_inter, list<ero_subobj*>& ero_intra);
    void SetVtagToERO(list<ero_subobj*>& ero, u_int32_t vtag);
    void GetDTL(te_tlv_header* tlv, list<dtl_hop>& dtl);

    // FSM state handlers ...
    
    ////// STATE_REQ ////
    int HandleLSPQRequest();

    ////// STATE_RECURSIVE_REQ ////
    int HandleRecursiveRequest();
    
    ////// STATE_RCE_REPLY  ////
    int HandleRCEReply(api_msg *msg);

    ////// STATE_ERO_PARTIAL ////
    int HandlePartialERO();

    ///// STATE_NEXT_HOP_NARB_REPLY  //////
    int HandleNextHopNARBReply(api_msg *msg);

    ////// STATE_ERROR ////
    int HandleErrorCode(u_int32_t errcode);

    ////// STATE_ERO_COMPLETE ////
    int HandleCompleteERO();
    
    ////// STATE_ERO_COMPLETE_WITH_CONFIRMATION_ID ////
    int HandleCompleteEROWithConfirmationID();
    
    ////// STATE_STORED_ERO_WITH_CONFIRMATION_ID ////
    int HandleStoredEROWithConfirmationID();

    ////// STATE_RESV_CONFIRM ////
    int HandleResvConfirm(api_msg* msg);

    ////// STATE_RESV_RELEASE ////
    int HandleResvRelease(api_msg* msg);

    ////// STATE_RESV_RELEASE_CONFIRM ////
    int HandleResvReleaseConfirm();

    friend class LSP_Broker;
};


////////////////////////////////////////////////

class NARB_APIServer;
class ConfirmationIDIndxedEROWithTimer;
class LSP_Broker: public APIReader
{
private:
    u_int32_t lspb_id;    // broker's ID

    // Note that an API Writer pointer is stored in APIReader...

    // pointing to the information for a recursive NARB 
    //  struct if_narb_info * rec_narb;
    //  int rec_narb_fd;

    LSP_Broker():APIReader(-1, NULL) {}

protected:
    list<LSPQ*> lspq_list;    // request querry list

public:
    static list<ConfirmationIDIndxedEROWithTimer*> qconf_id_indexed_ero_list;

public:
    LSP_Broker(int sock, NARB_APIServer* server);
    virtual ~LSP_Broker();
    virtual void Run();
    virtual int HandleMessage(api_msg * msg);
    virtual int HandleReplyMessage (api_msg* msg);
    u_int32_t LspbId() { return lspb_id; }
    list<LSPQ*>& LspqList() { return lspq_list; }
    LSPQ * LspqLookup (u_int32_t ucid, u_int32_t seqnum);
    LSPQ * LspqLookupByReqSpec (u_int32_t ucid, u_int32_t seqnum, msg_app2narb_request* req_spec, msg_app2narb_request* mrn_spec);
    void DescribeLSPbyState(u_char state, vector<string>& desc_v);

    static u_int32_t get_unique_lspb_id();

    // global functions to manage qconf_id_indexed_ero_list
    static ConfirmationIDIndxedEROWithTimer* StoreEROWithConfirmationID(list<ero_subobj*>& ero, u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id);
    static ConfirmationIDIndxedEROWithTimer* LookupEROWithConfirmationID(u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id);
    static ConfirmationIDIndxedEROWithTimer* RemoveEROWithConfirmationID(u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id);

    friend class LSPQ;
};


////////////////////////////////////////////////

// structure of NARB->RCE CSPF request message (msg body only)
struct msg_narb_cspf_request 
{
    u_int32_t lspb_id;
    u_int32_t app_seqnum;
    struct in_addr area_id;
    struct msg_app2narb_request app_req_data;
}; 

// structure of RCE->NARB CSPF reply message (msg body only)
struct msg_narb_cspf_reply
{
    u_int32_t narb_apiserv_id;
    u_int32_t app_seqnum;
    struct in_addr src;
    struct in_addr dest;
    struct te_tlv_header tlv;
};

struct msg_narb_recursive_cspf_request 
{
    u_int32_t lspb_id;
    u_int32_t app_seqnum;
    struct in_addr area_id;
    struct msg_app2narb_request app_req_data;
    struct msg_app2narb_request rec_req_data;    // data structure for relaying mrn-multidomain tspec
}; 

// the internal data type defined for the node data of ERO list
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

// definitions of NARB error code as proccessing a request fails
enum  narb_error_code
{
    NARB_ERROR_NO_SRC = 1,
    NARB_ERROR_NO_DEST = 2,  
    NARB_ERROR_NO_ROUTE = 3,
    NARB_ERROR_INTERNAL = 4,
    NARB_ERROR_INVALID_REQ = 5,
    NARB_ERROR_JUST_HOLDON = 6,
    NARB_ERROR_EXCEED_MAX_RETRAN = 7
};

////////////////////////////////////////////////

class link_info;
class ConfirmationIDIndxedEROWithTimer: public Timer
{
private:
    u_int32_t qconf_ucid;
    u_int32_t qconf_seqnum;
    u_int32_t qconf_source;
    list<ero_subobj*> qconf_ero;
    int trash_seconds; // time before deletion after expired
    bool expired;
    link_info* link_border_egress;

    ConfirmationIDIndxedEROWithTimer(): Timer(0, 0) { }

public:
    ConfirmationIDIndxedEROWithTimer(list<ero_subobj*>& ero, u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id,
        int expire_secs=10, int trash_secs=0) : Timer(expire_secs, 0), qconf_ucid(ucid), qconf_seqnum(seqnum), qconf_source(src_id),
            trash_seconds(trash_secs), expired(false), link_border_egress(NULL)
        {
            list<ero_subobj*>::iterator iter = ero.begin();
            for ( ; iter != ero.end(); iter++ )
            {
                ero_subobj* subobj = new (struct ero_subobj);
                *subobj = *(*iter);
                qconf_ero.push_back(subobj);
            }
        }
    ~ConfirmationIDIndxedEROWithTimer()
        {
            while (qconf_ero.size() > 0)
            {
                delete qconf_ero.front();
                qconf_ero.pop_front();
            }
            LSP_Broker::RemoveEROWithConfirmationID(qconf_ucid, qconf_seqnum, qconf_source);
        }

    bool ConfirmationIDMatched (u_int32_t ucid, u_int32_t seqnum, u_int32_t src_id)
        {
            return (qconf_ucid == ucid && qconf_seqnum == seqnum && qconf_source == src_id);
        }
    bool Expired() { return expired; }
    list<ero_subobj*>& GetERO() { return qconf_ero; }
    void SetERO(list<ero_subobj*>& ero) 
        {
            qconf_ero.clear();
            list<ero_subobj*>::iterator iter = ero.begin();
            for ( ; iter != ero.end(); iter++ )
            {
                ero_subobj* subobj = new (struct ero_subobj);
                *subobj = *(*iter);
                qconf_ero.push_back(subobj);
            }
        }
    link_info* GetBorderEgressLink() { return link_border_egress; } 
    void SetBorderEgressLink(link_info* link) { link_border_egress = link; } 
    virtual void Run()
        {
            if (!expired)
                expired = true;
            else
                trash_seconds = 0;

            if (trash_seconds > 0)
            {
                SetInterval(trash_seconds, 0);
                SetRepeats(1);
                trash_seconds = 0;
            }
            else
            {
                SetObsolete(true);
                SetAutoDelete(true);
            }
        }
};

// macro to combine loose hop indicator and type into an octet
#define L_AND_TYPE(L, T) ((((u_char)L)<<7) |((u_char)T))

// definitions of loose/strict hop indicator
#define ERO_TYPE_STRICT_HOP 0
#define ERO_TYPE_LOOSE_HOP 1

// macro to generate a NARB->OSPF sequence number manually
#define NARB_OSPF_SEQ_NUM(X, Y) ((X & 0xffff0000) | (Y & 0x0000ffff))

#define ANY_VTAG 0xffff  //Indicating that LSP uses any available E2E VLAN Tag

#define LOCAL_ID_TYPE_PORT (u_int16_t)0x1
#define LOCAL_ID_TYPE_GROUP (u_int16_t)0x2
#define LOCAL_ID_TYPE_TAGGED_GROUP (u_int16_t)0x3
#define LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL (u_int16_t)0x4
#define LOCAL_ID_TYPE_SUBNET_INTERFACE (u_int16_t)0x5
#define LOCAL_ID_TYPE_SUBNET_UNI_SRC (u_int16_t)0x10
#define LOCAL_ID_TYPE_SUBNET_UNI_DEST (u_int16_t)0x11

#define HAS_VLAN(P, VID) ((P[(VID-1)/8] & (0x80 >> (VID-1)%8)) != 0)

#endif
 
