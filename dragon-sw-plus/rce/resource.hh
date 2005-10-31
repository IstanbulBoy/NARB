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
#ifndef __RESOURCE_H__
#define __RESOURCE_H__
#include <list>
#include <string>
#include "rce_types.hh"
#include "rce_log.hh"
#include "rce_lsa.hh"
#include "radixtree.hh"
#include "rce_schema.hh"
#include "rce_config.hh"

using namespace std;

enum ResourceType
{
    RTYPE_LOC_PHY_LNK = 0x0,
    RTYPE_LOC_ABS_LNK = 0x01,
    RTYPE_LOC_RTID = 0x02,
    RTYPE_GLO_PHY_LNK = 0x03,
    RTYPE_GLO_ABS_LNK = 0x04,
    RTYPE_GLO_RTID = 0x05,
};

#ifdef HAVE_EXT_ATTR
struct TLP
{
    u_int16_t t;
    u_int16_t l;
    void* p;
};

extern TLP * MAKE_TLP (u_int16_t T,  u_int16_t L, void* P);
#endif

class Resource
{
protected:
    ResourceType type;
    u_int32_t domainMask;
    u_int32_t domainId;
    u_int32_t advRtId;

    u_int32_t id; //link or resource id;

#ifdef HAVE_EXT_ATTR
    vector<TLP> attrTable;
#endif

public:
    Resource(ResourceType type_val, u_int32_t dmask, u_int32_t advRt, u_int32_t id_val): 
        type(type_val), domainMask(dmask), advRtId(advRt), id(id_val) { domainId = 0; }
    virtual ~Resource();
    virtual Prefix Index() 
        { 
            Prefix index; 
            index.length = 64; 
            //prefix_word(index, 0) = domainId; 
            prefix_word(index, 0) = advRtId;
            prefix_word(index, 1) = id; 
            return index;
        }
    ResourceType Type() {return type;}
    u_int32_t ID() { return id; }
    u_int32_t Id() { return id; }
    void SetId(u_int32_t x) { id = x; }
    u_int32_t AdvRtId() { return advRtId; }
    void SetAdvRtId(u_int32_t x) { advRtId = x; }
    u_int32_t DomainMask() {return domainMask;}
    u_int32_t DomainID() {return domainId;}
    u_int32_t DomainId() {return domainId;}
    void SetDomainId(u_int32_t x) { domainId = x; }

#ifdef HAVE_EXT_ATTR
    void SetAttribute(int attrIndex, u_int16_t type, u_int16_t len, void* data);
    void SetAttribute(int attrIndex, TLP *tlp);
    TLP* GetAttribute(int attrIndex);
#endif

    friend class ResourceDB;
    friend class LSAHandler;
    friend class PCEN;
    friend class PCENNode;
    friend class PCENLink;
    friend class PCEN_KSP;
    friend class PCEN_MRN;
};

class RouterId: public Resource
{
public:
    RouterId(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t rtId): Resource(type_val, domain, advRt, rtId) {}
    RouterId(ResourceType type_val, u_int32_t domain, u_int32_t rtId): Resource(type_val, domain, rtId, rtId) {}
    RouterId(u_int32_t rtId): Resource(RTYPE_LOC_RTID, SystemConfig::domainMask, rtId, rtId) {}
   virtual ~RouterId() {}

    //debugging code
    void Dump()
        { 
            LOGF("RouterID[%x] advertised by [%x] in domain [%x]\n", id, advRtId, domainId);
        }

    friend class ResourceDB;
    friend class LSAHandler;
    friend class PCEN;
    friend class PCENNode;
    friend class PCENLink;
    friend class PCEN_KSP;
    friend class PCEN_MRN;
};

#define MAX_NUM_VLANS 23
struct IfSwCapDesc
{
    u_char	swtype;
    u_char	encoding;
    u_char	reserved[2];
    float max_lsp_bw[8];
// ISCD specific information defined below
    union {
        float min_lsp_bw;
        struct {
            u_char vlan_version;
            u_char vlan_num;
            u_int16_t vlan_id[MAX_NUM_VLANS];
        }; // L2SC Specific Infor for E2E Tagged VLAN only...
    };
};
#define ISCD_MADATORY_SIZE 36
#define IFSWCAP_SPECIFIC_VLAN_VERSION 0x2

struct IfAdptCapDesc
{
    u_char	swtype_lower;
    u_char	encoding_lower;
    u_char	swtype_upper;
    u_char	encoding_upper;
    float max_lsp_bw[8];
// IACD specific information defined below
};

#define ISCD IfSwCapDesc
#define IACD IfAdptCapDesc

struct Wavelength
{
    u_int32_t lambda;
    u_int32_t ifAddr;
    float bandwidth;
    u_char holdPriority; //holdPriority = 0, not available; 7, available; otherwise, preemptable.
    u_char reserved[3];
// ISCD specific information defined below
};

enum ResourceStatus
{
    RSTAT_IDLE = 0,
    RSTAT_FROZ,
    RSTAT_RESV,
    RSTAT_INSVC
};

struct Reservation
{
    u_int32_t domain_id;
    u_int32_t lsp_id;
    u_int32_t uptime;
    u_int32_t duration;
    float bandwidth;
    u_int32_t status;
};

#define RESV_SIZE (sizeof(struct Reservation))

class ResvTable
{
public:
    list<Reservation*> reserves;

    Reservation* operator [](u_int32_t i)
        {
            list<Reservation*>::iterator iter;
            for (iter = reserves.begin(); iter != reserves.end(); iter++)
            {
                assert (*(iter));
                if ((*(iter))->uptime <= i && 
                        i - (*(iter))->uptime <= (*(iter))->duration)
                    return *iter;
            }
            return NULL;
        }
    void Move(u_int32_t current)
        {
            list<Reservation*>::iterator iter;
            for (iter = reserves.begin(); iter != reserves.end(); iter++)
            {
                assert (*(iter));
                if ((*(iter))->uptime +  (*(iter))->duration <= current)
                {
                    reserves.erase(iter);
                 }
                else
                    return;
            }
        }
    bool IsAvailable(u_int32_t uptime, u_int32_t duration=1)
        {
            list<Reservation*>::iterator iter;
            for (iter = reserves.begin(); iter != reserves.end(); iter++)
            {
                assert (*(iter));
                if ((*(iter))->uptime -uptime < duration && 
                       uptime - (*(iter))->uptime < (*(iter))->duration)
                    return false;
            }
            return true;
        }
    Reservation* Allocate(u_int32_t uptime, u_int32_t duration)
        {
            list<Reservation*>::iterator iter;
            for (iter = reserves.begin(); iter != reserves.end(); iter++)
            {
                assert (*(iter));
                if ((*(iter))->uptime -uptime < duration && 
                       uptime - (*(iter))->uptime < (*(iter))->duration)
                    return NULL;
                if ((*(iter))->uptime >= uptime + duration)
                    break;
            }
            Reservation* pResv = new (Reservation);
            memset(pResv, 0, sizeof(Reservation));
            pResv->uptime = uptime;
            pResv->duration = duration;
            reserves.insert(iter, pResv);
            return pResv;
        }
};

class Link: public Resource
{
private:
    u_int32_t linkType; //tlv type 1 // id -> 2
    u_int32_t lclIfAddr; // 3
    u_int32_t rmtIfAddr;  // 4
    u_int32_t metric; // 5
    float maxBandwidth; //6
    float maxReservableBandwidth; // 7
    float minReservableBandwidth; //?
    float unreservedBandwidth[8]; //8
    u_int32_t rcClass; // 9
    u_int32_t lclRmtId[2]; // 11
    #define lclId lclRmtId[0]
    #define rmtId lclRmtId[1]

    //WDM specific attribute ...
    list<Wavelength*> wavelenths;

    list<ISCD*> iscds; // 15
    list<IACD*> iacds;
    list<Link*> dependings; //@@@ need to check dependence...
    list<Link*> dependents; //@@@ need to check dependence...
    ResvTable resvTable;

public:
    Link(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t lnkId): Resource(type_val, domain, advRt, lnkId) {Init();}
    Link(u_int32_t advRtId, u_int32_t lnkId): Resource(RTYPE_LOC_PHY_LNK, SystemConfig::domainMask, advRtId, lnkId) {Init();}
    Link(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t lnkId, u_int32_t lclIf, u_int32_t rmtIf):
        Resource(type_val, domain, advRt, lnkId), lclIfAddr(lclIf), rmtIfAddr(rmtIf) {Init();}
    Link(u_int32_t advRtId, u_int32_t lnkId, u_int32_t lclIf, u_int32_t rmtIf): 
        Resource(RTYPE_LOC_PHY_LNK, SystemConfig::domainMask, advRtId, lnkId), lclIfAddr(lclIf), rmtIfAddr(rmtIf)  {Init();}
    virtual ~Link();
    u_int32_t LclIfAddr() {return lclIfAddr;}
    void SetLclIfAddr(u_int32_t x) { lclIfAddr = x;}
    u_int32_t RmtIfAddr() {return rmtIfAddr;}
    void SetRmtIfAddr(u_int32_t x) { rmtIfAddr = x;}
    u_int32_t Metric() {return metric;}
    void SetMetric(u_int32_t x) { metric = x;}
    float MaxBandwidth() {return maxBandwidth;}
    void SetMaxBandwidth(float x) {maxBandwidth = x;}
    void Init()  
        { 
            linkType = 0; maxBandwidth = maxReservableBandwidth = minReservableBandwidth = 0; metric = 0;
            lclId = rmtId = 0; lclIfAddr = rmtIfAddr = 0; memset(unreservedBandwidth, 0, 32); rcClass = 0;
        }
    virtual Prefix Index() 
        { 
            Prefix index; 
            index.length = 128;
            //prefix_word(index, 0) = domainId; 
            prefix_word(index, 0) = advRtId;
            prefix_word(index, 1) = id;
            prefix_word(index, 2) = lclIfAddr;
            prefix_word(index, 3) = rmtIfAddr;
            return index;
        }

    list<ISCD*>& Iscds() {return iscds;}
    list<IACD*>& Iacds() {return iacds;}

    //debugging code
    void Dump();

    friend class ResourceDB;
    friend class LSAHandler;
    friend class PCEN;
    friend class PCENNode;
    friend class PCENLink;
    friend class PCEN_KSP;
    friend class PathT;
    friend class PCEN_MRN;
};

//defining domain mask
#define DOMAIN_MASK_GLOBAL 0xffffffff
#define DOMAIN_MASK_LOCAL 0x0
#define DOMAIN_ID_UNKNOWN 0xffffffff

struct RouterId2DomainId
{
   u_int32_t router_id; 
   u_int32_t domain_id;
};

class ResourceDB
{
private:
    static ResourceDB r_db;
    static RadixTree<Resource> r_trees[8];
    #define dbLocPhyLnk r_trees[RTYPE_LOC_PHY_LNK]
    #define dbLocAbsLnk r_trees[RTYPE_LOC_ABS_LNK]
    #define dbLocRtid r_trees[RTYPE_LOC_RTID]
    #define dbForPhyLnk r_trees[RTYPE_GLO_PHY_LNK]
    #define dbForAbsLnk r_trees[RTYPE_GLO_ABS_LNK]
    #define dbForRtid r_trees[RTYPE_GLO_RTID]
    ResourceDB () {}
    ~ResourceDB();

public:
    list<RouterId2DomainId*> routerToDomainDirectory;

    static ResourceDB& Instance() {return r_db;}
    static RadixTree<Resource> * Tree(ResourceType type) {  return &r_trees[type];  }
    static RadixNode<Resource>* Allocate(ResourceType type, Prefix* prefix);
    static RadixNode<Resource>* Insert(Resource* rc);
    static RadixNode<Resource>* Update(Resource* rc);
    static RadixNode<Resource>* Lookup(Resource* rc);
    static RadixNode<Resource>* LookupByPrefix(ResourceType type, Prefix* prefix);
    static void Delete(Resource* rc);
    static void DeleteByPrefix(ResourceType type, Prefix* prefix);
    //testing code
    static void WalkTree(ResourceType type);
    //end testing
    static void ClearTree(ResourceType type);
};

#define RDB (ResourceDB::Instance())
#endif
