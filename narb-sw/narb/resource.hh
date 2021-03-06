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
#ifndef __RESOURCE_H__
#define __RESOURCE_H__
#include <list>
#include <string>
#include "types.hh"
#include "log.hh"
#include "lsa.hh"
#include "radixtree.hh"

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

class LocalInfo
{
public:
    static u_int32_t domainMask;
    static u_int32_t domainId;
    static string sysDescription;
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
    friend class DomainInfo;
    friend class ConfigFile;
};

class RouterId: public Resource
{
public:
    RouterId(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t rtId): Resource(type_val, domain, advRt, rtId) {}
    RouterId(ResourceType type_val, u_int32_t domain, u_int32_t rtId): Resource(type_val, domain, rtId, rtId) {}
    RouterId(u_int32_t rtId): Resource(RTYPE_LOC_RTID, LocalInfo::domainMask, rtId, rtId) {}
   virtual ~RouterId() {}

    //debugging code
    void Dump()
        { 
            LOGF("RouterID[%x] advertised by [%x] in domain [%x]\n", id, advRtId, domainId);
        }

    friend class ResourceDB;
    friend class LSAHandler;
    friend class DomainInfo;
    friend class ConfigFile;
};

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
            u_int16_t	length;
            u_int16_t	version;
            u_char      bitmask[MAX_VLAN_NUM/8];
            u_char      bitmask_alloc[MAX_VLAN_NUM/8];
        }vlan_info;
        struct {
        	u_int16_t		length;
        	u_int16_t	 	version;
        	u_int8_t		subnet_uni_id;
        	u_int8_t		first_timeslot;
        	u_char		swtype_ext;
        	u_char		encoding_ext;
        	u_int32_t		tna_ipv4;
        	u_int32_t		nid_ipv4;
        	u_int32_t		data_ipv4;
        	u_int32_t		logical_port_number;
        	u_int32_t		egress_label_downstream;
        	u_int32_t		egress_label_upstream;
        	char			control_channel[12];
        	char			node_name[16];
        	u_int8_t		timeslot_bitmask[MAX_TIMESLOTS_NUM/8];
        } subnet_uni_info;
    }; // L2SC Specific Infor for E2E Tagged VLAN only...
};
#define ISCD_MADATORY_SIZE 36

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

    ResvTable& operator=(ResvTable& rt)
    {
        this->reserves.clear();
        list<Reservation*>::const_iterator it;
        for (it = rt.reserves.begin(); it != rt.reserves.end(); it++)
        {
            Reservation* resv = new Reservation(*(*it));
            this->reserves.push_back(resv);
        }
        return *this;
    }
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
            list<Reservation*>::iterator iter = reserves.begin();
            while (iter != reserves.end())
            {
                assert (*(iter));
                if ((*(iter))->uptime +  (*(iter))->duration <= current)
                {
                    iter = reserves.erase(iter);
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
protected:
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
    u_char vtagBitMask[MAX_VLAN_NUM/8]; //up to 4096 vlan ids, one per bit
    u_char vtagBitMaskAlloc[MAX_VLAN_NUM/8]; //up to 4096 vlan ids, one per bit

public:
    Link():Resource(RTYPE_LOC_PHY_LNK, LocalInfo::domainMask, 0, 0) { Init();}
    Link(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t lnkId): Resource(type_val, domain, advRt, lnkId), lclIfAddr(0), rmtIfAddr(0)  {Init();}
    Link(u_int32_t advRtId, u_int32_t lnkId): Resource(RTYPE_LOC_PHY_LNK, LocalInfo::domainMask, advRtId, lnkId), lclIfAddr(0), rmtIfAddr(0) {Init();}
    Link(ResourceType type_val, u_int32_t domain, u_int32_t advRt, u_int32_t lnkId, u_int32_t lclIf, u_int32_t rmtIf):
        Resource(type_val, domain, advRt, lnkId), lclIfAddr(lclIf), rmtIfAddr(rmtIf) {Init();}
    Link(u_int32_t advRtId, u_int32_t lnkId, u_int32_t lclIf, u_int32_t rmtIf): 
        Resource(RTYPE_LOC_PHY_LNK, LocalInfo::domainMask, advRtId, lnkId), lclIfAddr(lclIf), rmtIfAddr(rmtIf)  {Init();}
    virtual ~Link();
    u_int32_t LclIfAddr() {return lclIfAddr;}
    void SetLclIfAddr(u_int32_t x) { lclIfAddr = x;}
    u_int32_t RmtIfAddr() {return rmtIfAddr;}
    void SetRmtIfAddr(u_int32_t x) { rmtIfAddr = x;}
    u_int32_t Metric() {return metric;}
    void SetMetric(u_int32_t x) { metric = x;}
    float MaxBandwidth() {return maxBandwidth;}
    float MaxReservableBandwidth() {return maxReservableBandwidth;}
    void SetMaxBandwidth(float x) {maxBandwidth = x;}
    float* UnreservedBandwidth() { return unreservedBandwidth; }
    u_char* VtagBitMask() { return vtagBitMask; }
    bool IsVtagSet(u_int32_t vlanID) { return (vtagBitMask[(vlanID-1)/8] & (0x80 >> (vlanID-1)%8)) != 0; }
    bool IsVtagAllocated(u_int32_t vlanID) { return (vtagBitMaskAlloc[(vlanID-1)/8] & (0x80 >> (vlanID-1)%8)) != 0; }
    void SetVtag(u_int32_t vlanID) { vtagBitMask[(vlanID-1)/8] = vtagBitMask[(vlanID-1)/8] | (0x80 >> (vlanID-1)%8); }
    void ResetVtag(u_int32_t vlanID) { vtagBitMask[(vlanID-1)/8] = vtagBitMask[(vlanID-1)/8] & ~(0x80 >> (vlanID-1)%8); }
    void AllocateVtag(u_int32_t vlanID) { vtagBitMaskAlloc[(vlanID-1)/8] = vtagBitMaskAlloc[(vlanID-1)/8] | (0x80 >> (vlanID-1)%8); }
    void DeallocateVtag(u_int32_t vlanID) { vtagBitMaskAlloc[(vlanID-1)/8] = vtagBitMaskAlloc[(vlanID-1)/8] & ~(0x80 >> (vlanID-1)%8); }

    void Init()  
        { 
            linkType = 0; maxBandwidth = maxReservableBandwidth = minReservableBandwidth = 0; metric = 0;
            lclId = rmtId = 0; memset(unreservedBandwidth, 0, 32);
            memset(vtagBitMask, 0, MAX_VLAN_NUM/8);
            memset(vtagBitMaskAlloc, 0, MAX_VLAN_NUM/8);
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
    friend class DomainInfo;
    friend class ConfigFile;
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
    ~ResourceDB() 
        {
            ClearTree(RTYPE_LOC_PHY_LNK);
            ClearTree(RTYPE_LOC_ABS_LNK);
            ClearTree(RTYPE_LOC_RTID);
            ClearTree(RTYPE_GLO_PHY_LNK);
            ClearTree(RTYPE_GLO_ABS_LNK);
            ClearTree(RTYPE_GLO_RTID);
        }
public:
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
    static void ClearTree(ResourceType type);
    static list<RouterId2DomainId*> routerToDomainDirectory;
};

#define RDB (ResourceDB::Instance())
#endif
