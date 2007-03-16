/*
 * Copyright (c) 2005
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2006
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
#ifndef __TDS_H__
#define __TDS_H__

#include "resource.hh"
#include "lsa.hh"
#include "event.hh"
using namespace std;
#include <vector>
#include <list>

#define MAX_ADDR_LEN 40

// data structure storing OSPFd server information
class ospfd_info 
{
public:
    char addr[MAX_ADDR_LEN];
    int port;
    int localport;
    in_addr ori_if;
    in_addr area;

    ospfd_info()
        {
            addr[0] = 0;
            port = 0;
            localport = 0;
            ori_if.s_addr = area.s_addr  = 0;
        }
};

// definitions for router_id_info::type and link_info::type
#define RT_TYPE_DEFAULT 0x0
#define RT_TYPE_HOST 0x01
#define RT_TYPE_BORDER 0x02

// data structure of an opaque router id generated in domain summary 
class router_id_info: public RouterId
{
public:
    u_int32_t opaque_id;
    bool hide;
    int rt_type; 
    router_id_info(u_int32_t domain_id, in_addr ip);
    virtual ~router_id_info() {}
};

// data structure of an opaque TE-link generated in domain summary 
class link_info: public Link
{
public:
    u_int32_t opaque_id;
    bool hide;
    u_int16_t info_flag;

#define ifswcap iscds.front()
    ISCD* GetISCD () { return iscds.front(); }
    link_info(u_int32_t domain_id, in_addr advId, in_addr linkId);
    virtual ~link_info() {}
};

#define LINK_PARA_FLAG_MAX_BW 0x1
#define LINK_PARA_FLAG_MAX_RSV_BW 0x2
#define LINK_PARA_FLAG_UNRSV_BW 0x4
#define LINK_PARA_FLAG_IFSW_CAP 0x10
#define LINK_PARA_FLAG_METRIC 0x20
#define LINK_PARA_FLAG_LOC_IF 0x40
#define LINK_PARA_FLAG_REM_IF 0x80
// DRAGON proprietary Speficit TLV's
#define LINK_PARA_FLAG_RESV 0x100
#define LINK_PARA_FLAG_IACD 0x200
#define LINK_PARA_FLAG_VLAN 0x400
//Flag indicating whether the link is originated by automatic probing 
//This is not indicating a TE parameter
#define LINK_PARA_FLAG_AUTO_PROBED 0x1000 
//macros to access link_info flag 
#define SET_LINK_PARA_FLAG(X, F) ((X) = (X) | (F))
#define LINK_PARA_FLAG(X, F) ((X) & (F))

// data structure describing associating of remote if_addr of an  inter-domain 
// TE-link with the narb in a neighboring domain
class if_narb_info
{
public:
    char addr[MAX_ADDR_LEN]; // narb address
    int port; // narb port
    list<in_addr> if_addr_list; // a list of interfaces associated with this narb

    if_narb_info()
        {
            addr[0] = 0;
            port = 0;
        }
};

// Data structure to probe indicating that a CSPF virtual link exists between 
// a source node and a bunch of destination nodes. Abstract Link TE LSAs will
// be originated based on these data.
class service_info
{
public:
    int service_id;
    int sw_type;
    int encoding;
    float max_bw;

    service_info()
        {
            service_id = 0;
            sw_type = 0;
            encoding = 0;
            max_bw = 0;
        }
};

class svc_probe
{
public:
    router_id_info* router;
    service_info* service;

    svc_probe()
        {
            memset(&router, 0, sizeof(router_id_info));
            memset(&service, 0, sizeof(service_info));
        }
};

// a super data structure that summarzes the domain
class ZebraOspfWriter;
class DomainInfo
{
public:
    // info of intra-domain OSPFd
    ospfd_info ospfd_intra;
    // info of intra-domain OSPFd
    ospfd_info ospfd_inter;
    // the domain identifier (other than the area id)
    u_int32_t domain_id;

    // topology information (RadixTree<router_id_info> RadixTree<link_info> stored in RDB
    RadixNode<Resource> * current_node;
    
    // a list of opaque te links (struct link_info) in this domain
    vector<link_info*> inter_domain_te_links;
    // a vector of (struct if_narb_info)
    vector<if_narb_info*> if_narb_table;
    // vector storing services
    vector<service_info*> services;
    // a vector of svc_probe
    vector<svc_probe*> svc_probes;

    DomainInfo()
        {
            memset(&ospfd_intra, 0, sizeof(ospfd_info));
            memset(&ospfd_inter, 0, sizeof(ospfd_info));
            domain_id = 0;
            current_node = NULL;
        }
    ~DomainInfo();

    service_info * ServiceLookupById(int id);
    void AddRouter(router_id_info * router);
    void AddLink(link_info* link);
    void AddPeerNarb (char *addr, int port, in_addr rmt_if);

    router_id_info* LookupRouterById(in_addr id);
    link_info* LookupLinkByLclIf(in_addr id);
    link_info* LookupLinkByRmtIf(in_addr id);
    link_info* LookupInterdomainLinkByLclIf(in_addr id);
    link_info* LookupInterdomainLinkByRmtIf(in_addr id);
    void SearchAndProcessInterdomainLink(list<ero_subobj*>&ero);
    if_narb_info * LookupNarbByAddr(char *addr);
    if_narb_info * LookupNarbByRmtIf(in_addr rmt_if);
    bool FromSameRouter(in_addr& id1, in_addr& id2);
    bool ToSameRouter(in_addr& id1, in_addr& id2);

    router_id_info * FirstRouterId();
    router_id_info * NextRouterId();
    link_info * FirstLink();
    link_info * NextLink();
    
    void ClearRouterIds();
    void ClearLinks();

    inline void* BuildTeLinkOpaqueData(link_info* link);
    int OriginateRouterId (ZebraOspfWriter* oc_writer, router_id_info* router);
    int OriginateTeLink (ZebraOspfWriter* oc_writer, link_info* link);
    int UpdateTeLink (ZebraOspfWriter* oc_writer, link_info* link);
    bool IsOriginateInterfaceReady (ZebraOspfWriter* oc_writer);
    int OriginateTopology (ZebraOspfWriter* oc_writer);
    int DeleteTopology (ZebraOspfWriter* oc_writer);
    void HideTopology ();
    void ExposeTopology ();
    link_info* VirtualTeLinkProbe(RCE_APIClient& rce, svc_probe *svc_probe, router_id_info *router);
    void AutoProbeVirtualLinks();
    void CleanupProbedVirtualLinks();
};

class ZebraOspfSync;
class DomainTopologyOriginator: public Timer
{
private:
    ZebraOspfSync * ospf_client;
public:
    DomainTopologyOriginator(int interval): Timer(interval, 0) { ospf_client = NULL;}
    virtual ~DomainTopologyOriginator() { }
    void SetOspfClient(ZebraOspfSync* oc) { ospf_client = oc; }
    virtual void Run();
};

    
#endif
 
