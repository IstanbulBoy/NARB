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

#ifndef __RCE_PCENGINE_H__
#define __RCE_PCENGINE_H__

#include "rce_types.hh"
#include "resource.hh"
#include "rce_api.hh"
#include "rce_lsp.hh"
#include "rce_filter.hh"
using namespace std;

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

// definitions of loose/strict hop indicator
#define ERO_TYPE_STRICT_HOP 0
#define ERO_TYPE_LOOSE_HOP 1


#define LOCAL_ID_TYPE_SUBNET_UNI_SRC (u_int16_t)0x10 	//Source (sender)
#define LOCAL_ID_TYPE_SUBNET_UNI_DEST (u_int16_t)0x11	//Destination (Recv)
    

#define PCEN_MAX_GRAPH_SIZE 200
#define PCEN_INFINITE_COST PCEN_MAX_GRAPH_SIZE*PCEN_MAX_GRAPH_SIZE*1000

class TSpec
{
public:
    u_int8_t SWtype;
    u_int8_t ENCtype;
    float Bandwidth;
    TSpec ():SWtype(0), ENCtype(0), Bandwidth(0) {}
    void Update(u_int8_t sw_type, u_int8_t encoding, float bw)
        {
            SWtype = sw_type;
            ENCtype = encoding;
            Bandwidth = bw;
        }
    bool operator == (TSpec &t)
        {
            return (this->SWtype == t.SWtype && this->ENCtype == t.ENCtype && float_equal(this->Bandwidth, t.Bandwidth));
            //return (this->SWtype == t.SWtype && this->ENCtype == t.ENCtype); // @@@@
        }
    bool operator <= (TSpec &t)
        {
            return (this->SWtype == t.SWtype && this->ENCtype == t.ENCtype && this->Bandwidth <= t.Bandwidth);
            //return (this->SWtype == t.SWtype && this->ENCtype == t.ENCtype); // @@@@
        }

};

class ConstraintTagSet
{
private:
    list<u_int32_t> tag_list;

public:
    void AddTag(u_int32_t tag)
        {
            list<u_int32_t>::iterator it;
            for (it = tag_list.begin(); it != tag_list.end(); it++)
                if (*it == tag)
                    return;
            tag_list.push_back(tag);
        }
    void AddTags(int num, u_int32_t* tags)
        {
            assert(tags);
            for (int i = 0; i < num; i++)
                AddTag(tags[i]);
        }
    void AddTags(int num, u_int16_t* tags)
        {
            assert(tags);
            for (int i = 0; i < num; i++)
                AddTag((u_int32_t)tags[i]);
        }
    void AddTags(u_char* bitmask, int max_num)
        {
            assert(bitmask);
            for (int i = 1; i < max_num; i++)
                if (HAS_VLAN(bitmask, i))
                    AddTag(u_int32_t(i));
        }
    void DeleteTag(u_int32_t tag)
        {
            list<u_int32_t>::iterator it;
            for (it = tag_list.begin(); it != tag_list.end(); it++)
                if (*it == tag)
                {
                    tag_list.erase(it);
                    return;
                }
        }
    void DeleteTags(int num, u_int32_t* tags)
        {
            assert(tags);
            for (int i = 0; i < num; i++)
                DeleteTag(tags[i]);
        }
    void DeleteTags(int num, u_int16_t* tags)
        {
            assert(tags);
            for (int i = 0; i < num; i++)
                DeleteTag((u_int32_t)tags[i]);
        }
    void DeleteTags(u_char* bitmask, int max_num)
        {
            assert(bitmask);
            for (int i = 1; i < max_num; i++)
                if (HAS_VLAN(bitmask, i))
                    DeleteTag(u_int32_t(i));
        }
    bool HasTag(u_int32_t tag)
        {
            if (tag == ANY_VTAG)
            {
                if(tag_list.size() > 0)
                    return true;
                return false;
            }

            list<u_int32_t>::iterator it;
            for (it = tag_list.begin(); it != tag_list.end(); it++)
                if (*it == tag)
                    return true;
            return false;
        }
    bool IsEmpty()
        {
            return (tag_list.size() == 0);
        }
    list<u_int32_t>& TagSet() {return tag_list;}

    void Intersect(ConstraintTagSet & tagset)
        {
            list<u_int32_t>::iterator it = tag_list.begin();
            while (it != tag_list.end())
           	{
                if (tagset.HasTag(*it))
                    it++;
                else
                    it = tag_list.erase(it);
           	}
        }
    void DisplayTags()
        {
            if (tag_list.size() == 0) return;
            cout << "Tags:";
            list<u_int32_t>::iterator it;
            for (it = tag_list.begin(); it != tag_list.end(); it++)
                cout << ' ' << *it;
            cout << endl;
        }
};

class PCENLink;
class PCENNode
{
public:
    int ref_num;
    list<PCENLink*> out_links;
    list<PCENLink*> in_links;
    RouterId * router;

    //////Variables (sets) Indicating Search Progress//////
    TSpec tspec;
    ConstraintTagSet waveset;
    ConstraintTagSet vtagset;
    bool path_visited;
    
    ////////// Shujia's additions/////////
    union NodeFlag 
    {
        unsigned long flag;        // flag that indicates whether this node has been visited
        struct nodeFlag 
        {
            unsigned visited: 1;
            unsigned filteroff: 1; // not used
            unsigned maskoff: 1;// not used
        } nfg;
    } nflg;

    unsigned long auxvar1; // not used
    unsigned long auxvar2; // not used
    double minCost;				// Remember the minimal cost from source node to this node
    list<PCENLink*> path;			// Remember the path from source node to this node
    list<ero_subobj> ero_track;                   // Keep the track of path using a list of composite ero subobjects
    ///////////////////////////////////////

    void Init()
        {
	    auxvar1=0;
	    auxvar2=0;
	    minCost=PCEN_INFINITE_COST;
           path_visited = false;
	    nflg.flag=0;
        }

    PCENNode(): ref_num(-1) { Init(); }
    PCENNode(int id);
    PCENNode(RouterId *router_ptr): router(router_ptr), ref_num(-1)  { Init(); }

    u_int32_t DomainId ();
    PCENLink * GetOutLinkByIp(in_addr * ip);
    PCENLink * GetInLinkByIp(in_addr * ip);

    // when we do inter-domain routing, some nodes may be filtered out
    // when a node is filtered out, all the links connected to this node are also filtered out
    void FilterNode();			// mask off all the links connected to this node by setting filteroff=1
    void RestoreNodeFilter();	// restore all the links connected to this node by setting filteroff=0

    // when we find KSP, some nodes may be masked
    void MaskNode();			// mask off all the links connected to this node by setting maskoff=1
    void RestoreNodeMask();		// restore all the links connected to this node by setting maskoff=0

    bool operator< (const PCENNode& node) const { return minCost<node.minCost; }
    //////  Debuggin Code   //////
    void DisplayInfo();
    void ShowPath();
};


class PCENLink
{
public:
    int linkID;
    PCENNode* lcl_end;
    PCENNode* rmt_end;
    Link* link;
    PCENLink* reverse_link;
    bool link_self_allocated; //Link* link is self-allocated instead of pointing to external resource in RDB

    ////////// GMU additions //////////////--Begin
    union LinkFlag 
    {
        unsigned long flag;        // flag that whether this node has been visited
        struct linkFlag 
        {
            unsigned visited: 1;
            unsigned filteroff: 1; // not used
            unsigned maskoff: 1; // not used
        } lfg;
    } lflg;
    unsigned long auxvar1;	// this variable can be used as ID in WG or CG
    unsigned long auxvar2;	// this variable can be used as ID in WG or CG
    ////////// GMU additions //////////////--End

    double PCENmetric ();        // return the metric of this link assigned by PCEN
    void SetPCENmetric (u_int32_t x);
    void SetPCENmetric (double d);

    //////////////////////////////////////

    void Init() 
        {
            auxvar1=0;
            auxvar2=0;
            lflg.flag = 0;
            reverse_link = NULL;
            link_self_allocated = false;
        }

    PCENLink(int id);
    PCENLink(): linkID(-1), link(NULL), lcl_end(NULL), rmt_end(NULL) { Init(); }
    PCENLink(int id, int localNodeId, int remoteNodeId, vector<PCENNode*>& routers);
    PCENLink(Link *link_ptr):  linkID(-1), link(link_ptr), lcl_end(NULL), rmt_end(NULL) { Init(); }
    virtual ~PCENLink();

    u_int32_t HeadDomainId ();
    u_int32_t TailDomainId ();

#ifdef HAVE_EXT_ATTR
    void*  AttributeById (u_int16_t tlv_type);
    void*  AttributeByTag (const char * tag);
#endif

    bool IsAvailableForTspec(TSpec& tspec);
    bool CanBeEgressLink(TSpec& tspec);
    void ProceedByUpdatingWaves(ConstraintTagSet &head_waveset, ConstraintTagSet &next_waveset);
    void ProceedByUpdatingVtags(ConstraintTagSet &head_vtagset, ConstraintTagSet &next_vtagset);
    void ExcludeAllocatedVtags(ConstraintTagSet &vtagset);

    PCENNode* search_PCENNode(vector<PCENNode*>& routers, int NodeId);

    // when we do inter-domain routing, some links may be filtered out
    void FilterLink();		
    void RestoreLinkFilter();	

    // when we find KSP, some nodes may be masked
    void MaskLink();		// filter off all the links connected to this link by setting maskoff=1
    void RestoreLinkMask();		// restore all the links connected to this link by setting maskoff=0	

    //////  Debuggin Code   //////
    void DisplayInfo();
};

class Filter;

//Should return results to a public API writer queue!!!
class PCEN: public Event
{
protected:
    vector<PCENNode*> routers;
    vector<PCENLink*> links;
    list<Prefix*> prefix_list;
    list<Filter*>  filters;
    list<ero_subobj> ero; //Currently we do NOT support label ERO. Only IPv4 ERO is supported.

    in_addr source;
    in_addr destination;
    u_int8_t switching_type_ingress;
    u_int8_t encoding_type_ingress;
    float bandwidth_ingress;
    u_int8_t switching_type_egress;
    u_int8_t encoding_type_egress;
    float bandwidth_egress;

    narb_lsp_vtagmask_tlv* vtag_mask;
    u_int32_t hop_back;

    u_int32_t options;
    u_int32_t uptime;
    u_int32_t duration;
    u_int32_t ucid;
    u_int32_t seqnum;
    u_int32_t lspb_id;
    u_int32_t vtag;    
    bool is_bidirectional;
    bool is_e2e_tagged_vlan;
    bool is_via_movaz;

    float * gGraph;
    int gSize;
    int originalGraphSize;
    APIWriter* api_writer;

public:
    PCEN(in_addr src, in_addr dest, u_int8_t sw_type, u_int8_t encoding, float bw, u_int32_t opts, u_int32_t ucid1, u_int32_t msg_seqnum, u_int32_t lspb_id1 = 0, u_int32_t tag = 0, u_int32_t hopback = 0, narb_lsp_vtagmask_tlv* vtm = NULL):
            source(src), destination(dest), ucid(ucid1), seqnum(msg_seqnum), lspb_id(lspb_id1), options(opts), vtag(tag), hop_back(hopback), api_writer(NULL), gGraph (NULL), gSize (PCEN_MAX_GRAPH_SIZE)
            {
                switching_type_ingress = switching_type_egress = sw_type;
                encoding_type_ingress = encoding_type_egress = encoding;
                bandwidth_ingress = bandwidth_egress = bw;
                if ((opts&LSP_OPT_VTAG_MASK) && vtm)
                {
                    vtag_mask = new (struct narb_lsp_vtagmask_tlv);
                    *vtag_mask = *vtm;
                }
                else 
                {
                    vtag_mask = NULL;
                }
                is_bidirectional = ((opts & LSP_OPT_BIDIRECTIONAL) == 0 ? false : true);
                is_e2e_tagged_vlan = ((opts & LSP_OPT_E2E_VTAG) == 0 ? false : true);
                // VIA_MOVAZ and INCOPORATE_SUBNET modes are mutually exclusive and the later overrides the former...
                is_via_movaz = ((opts & LSP_OPT_VIA_MOVAZ) == 0 || SystemConfig::should_incorporate_subnet ? false : true);
            }
    PCEN(in_addr src, in_addr dest, u_int8_t sw_type_ingress, u_int8_t encoding_ingress, float bw_ingress, u_int8_t sw_type_egress, u_int8_t encoding_egress, 
                float bw_egress, u_int32_t opts, u_int32_t ucid1, u_int32_t msg_seqnum, u_int32_t lspb_id1 = 0, u_int32_t tag = 0, u_int32_t hopback = 0, narb_lsp_vtagmask_tlv* vtm = NULL ): 
                source(src), destination(dest), ucid(ucid1), seqnum(msg_seqnum), lspb_id(lspb_id1), options(opts), vtag(tag), hop_back(hopback), api_writer(NULL), gGraph (NULL), gSize (PCEN_MAX_GRAPH_SIZE)
            {
                switching_type_ingress = switching_type_egress = sw_type_ingress;
                encoding_type_ingress = encoding_type_egress = encoding_ingress;
                bandwidth_ingress = bandwidth_egress = bw_ingress;
                switching_type_egress = sw_type_egress;
                encoding_type_egress = encoding_egress;
                bandwidth_egress = bw_egress;
                if ((opts&LSP_OPT_VTAG_MASK) && vtm)
                {
                    vtag_mask = new (struct narb_lsp_vtagmask_tlv);
                    *vtag_mask = *vtm;
                }
                else
                {
                    vtag_mask = NULL;
                }
                is_bidirectional = ((opts & LSP_OPT_BIDIRECTIONAL) == 0 ? false : true);
                is_e2e_tagged_vlan = ((opts & LSP_OPT_E2E_VTAG) == 0 ? false : true);
                is_via_movaz = ((opts & LSP_OPT_VIA_MOVAZ) == 0 || SystemConfig::should_incorporate_subnet ? false : true);
            }
    virtual ~PCEN();

    void AssociateWriter (APIWriter *writer) { assert (writer); api_writer = writer; }
    PCENLink* search_PCENLink( int lcl_end,int rmt_end);

    // overloading functions
    virtual int VerifyRequest();
    virtual int BuildTopology();
    virtual int PerformComputation();
    virtual void Run();
    // public utility functions for TEDB access and path computation
    void BuildNodeVector(ResourceType type, Prefix *prefix);
    void BuildLinkVector(ResourceType type);
    void AllocateGraph(int size);
    void FreeGraph();
    void AddFilter(Filter *filter) { filters.push_back(filter); }
    void ClearFilters();
    PCENNode* NewTransitNode (vector<PCENNode*>& routers);
    PCENLink* NewTransitLink (vector<PCENLink*>& links);
    void SplitLink (vector<PCENNode*>& routers, vector<PCENLink*>& links, PCENLink * teLink);
    int HandleOverlappingLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links);
    void BuildGraph(vector<PCENNode*>& routers, vector<PCENLink*>& links);
    void DJhop(float *graphhop, int node_number, int source, int* path, int* hop, float* cost);
    void ReplyErrorCode(u_int32_t ret);
    void ReplyERO();
    void HoldLinkStatesUponQuery(narb_lsp_vtagmask_tlv* vtag_mask=NULL);

    static int GetLinkEndsByIndex(vector<PCENNode*>& routers, vector<PCENLink*>& links, int j, int k, in_addr* head_ip, in_addr* tail_ip);
    static PCENLink* GetLinkByIp(vector<PCENLink*>& links, in_addr* head_ip, in_addr* tail_ip);
    static PCENNode* GetNodeByIp(vector<PCENNode*>& routers, in_addr* ip);
    static u_int32_t GetRouterDomainId(vector<PCENNode*>& routers, in_addr* ip);
    static int GetDomainBorderRouters(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, list<PCENNode*>&borderRouters);
    static int GetInterDomainLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainIdHead, u_int32_t domainIdTail, list<PCENLink*>&interDomainLinks);
    static bool IsDomainBorderRouter(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, u_int32_t routerId);
    static bool IsLinkInDomain(PCENLink* pcenLink, u_int32_t domainId);

    // ... more ...
    //To get head and tail-end domain IDs for a TE link call GetRouterDomainId twice on advRtId and linkId
    //To get the intradomain network graph use BuildNodeVector(RTYPE_LOC_RTID) and BuildLinkVector(RTYPE_LOC_PHY_LNK)
    //To get the interdomain network graph use BuildNodeVector(RTYPE_GLO_RTID) and BuildLinkVector(RTYPE_GLO_ABS_LNK)
};


//Defining error codes...
enum PCENErrCode
{
    ERR_PCEN_NO_ERROR = 0,
    ERR_PCEN_NO_SRC = 1,
    ERR_PCEN_NO_DEST = 2,
    ERR_PCEN_NO_ROUTE = 3,
    ERR_PCEN_DATA_CORRUPT = 4,
    ERR_PCEN_INVALID_REQ = 5,
    ERR_PCEN_JUST_HOLDON = 6,
    ERR_PCEN_EXCEED_MAX_RETRAN = 7,
    ERR_PCEN_UNKNOWN = -10
};

#define  TLV_TYPE_NARB_ERO  0x03
#define  TLV_TYPE_NARB_ERROR_CODE  0x04

#endif

