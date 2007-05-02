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
#include "resource.hh"

ResourceDB ResourceDB::r_db;
RadixTree<Resource> ResourceDB::r_trees[8];

#ifdef HAVE_EXT_ATTR
TLP * MAKE_TLP (u_int16_t T,  u_int16_t L, void* P) 
{
    TLP* tlp = new (struct TLP); 
    tlp->t = T; 
    tlp->l = L;
    tlp->p = P;
    return tlp;
}

void Resource::SetAttribute(int attrIndex, u_int16_t type, u_int16_t len, void* data)
{
    if (attrIndex < 0)
        return;
    TLP *tlp = MAKE_TLP(type, len, data);
    SetAttribute(attrIndex, tlp);
}

void Resource::SetAttribute(int attrIndex, TLP *tlp)
{
    if (attrIndex < 0)
        return;

    if (attrTable.capacity() == 0)
        attrTable.resize(16);

    if (attrTable.size() < attrIndex+1)
    {
        attrTable.resize(attrIndex+1);
    }
    if (tlp->t == N_LIST || tlp->t == P_LIST || tlp->t == N_VECTOR || tlp->t == P_VECTOR)
    {
        if (attrTable[attrIndex].p == NULL)
        {
            attrTable[attrIndex] = *tlp;
            attrTable[attrIndex].p = new list<void*>;
        }
        ((list<void*> *)attrTable[attrIndex].p)->push_back(tlp->p);
    }
    else
    {
        attrTable[attrIndex] = *tlp;
    }
   
    delete tlp;
}

TLP* Resource::GetAttribute(int attrIndex)
{
    if (attrIndex < 0)
        return NULL;
    assert(attrTable.size() > attrIndex);
    return &attrTable[attrIndex];
}
#endif


Resource::~Resource()
{
    //clean and delete attrTable
}


Link::Link(Link* link):Resource(RTYPE_LOC_PHY_LNK,0, 0, 0)
{
    //Resource
    this->type = link->type;
    this->domainMask = link->domainMask;
    this->domainId = link->domainId;
    this->advRtId = link->advRtId;
    this->id = link->id;
    //Link
    this->linkType = link->linkType;
    this->lclIfAddr = link->lclIfAddr;
    this->rmtIfAddr = link->rmtIfAddr;
    this->metric = link->metric;
    this->maxBandwidth = link->maxBandwidth;
    this->maxReservableBandwidth = link->maxReservableBandwidth;
    this->minReservableBandwidth = link->minReservableBandwidth;
    for (int i = 0; i < 8; i++)
        this->unreservedBandwidth[i] = link->unreservedBandwidth[i];
    this->rcClass = link->rcClass;
    this->lclId = link->lclId;
    this->rmtId = link->rmtId;
    this->resvTable = link->resvTable;
    this->dependings = link->dependings;
    this->dependents= link->dependents;

    this->iscds.clear();
    list<ISCD*>::iterator it1;
    for (it1 = link->iscds.begin(); it1 != link->iscds.end(); it1++)
    {
        ISCD *iscd = new ISCD;
        *iscd = *(*it1);
        this->iscds.push_back(iscd);
    }
    this->iacds.clear();
    list<IACD*>::iterator it2;
    for (it2 = link->iacds.begin(); it2 != link->iacds.end(); it2++)
    {
        IACD *iacd = new IACD(*(*it2));
        this->iacds.push_back(iacd);
    }
    this->wavelenths.clear();
    list<Wavelength*>::iterator it3;
    for (it3 = link->wavelenths.begin(); it3 != link->wavelenths.end(); it3++)
    {
        Wavelength* wave = new Wavelength(*(*it3));
        this->wavelenths.push_back(wave);
    }

    //taking over pDeltaList
    this->pDeltaList = link->pDeltaList;
    link->pDeltaList = NULL;
}

Link::~Link()
{
    list<ISCD*>::iterator iter1;
    for (iter1 = iscds.begin(); iter1 != iscds.end(); iter1++)
    {
        assert (*iter1);
        delete *iter1;
    }
    
    list<IACD*>::iterator iter2;
    for (iter2 = iacds.begin(); iter2 != iacds.end(); iter2++)
    {
        assert (*iter2);
        delete *iter2;
    }

    list<Wavelength*>::iterator iter3;
    for (iter3 = wavelenths.begin(); iter3 != wavelenths.end(); iter3++)
    {
        assert (*iter3);
        delete *iter3;
    }

    if (pDeltaList)
    {
        list<LinkStateDelta*>::iterator iter4;
        for (iter4 = pDeltaList->begin(); iter4 != pDeltaList->end(); iter4++)
        {
            assert(*iter4);
            delete *iter4;
        }
    }
    
    // Clean up dependency?
}

Link& Link::operator+= (LinkStateDelta& delta)
{
    int i;

    if (delta.bandwidth > 0)
    {
        for (i = 0; i < 8; i++){
            this->unreservedBandwidth[i] += delta.bandwidth;
            if (this->unreservedBandwidth[i] > this->maxReservableBandwidth)
                this->unreservedBandwidth[i] = this->maxReservableBandwidth;
        }
    }

    list<ISCD*>::iterator iter = this->iscds.begin();
    for ( ; iter != this->iscds.end(); iter++ )
    {
        if (delta.bandwidth > 0)
        {
            for (i = 0; i < 8; i++)
            {
                (*iter)->max_lsp_bw[i] += delta.bandwidth;
                if ((*iter)->max_lsp_bw[i] > this->maxReservableBandwidth)
                    (*iter)->max_lsp_bw[i] = this->maxReservableBandwidth;
            }
        }

        if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
        {
            if ((delta.vlan_tag > 0 && delta.vlan_tag < MAX_VLAN_NUM) && (ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC))
            {
                SET_VLAN((*iter)->vlan_info.bitmask, delta.vlan_tag);
                if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                    RESET_VLAN((*iter)->vlan_info.bitmask_alloc, delta.vlan_tag);
            }
            if (ntohs((*iter)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI)
            {
                for (i = 0; i < MAX_TIMESLOTS_NUM/8; i++)
                    (*iter)->subnet_uni_info.timeslot_bitmask[i] |= delta.timeslots[i];
            }
        }

        if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC) // || (*iter)->swtype == MOVAZ_LSC
        {
            if (delta.wavelength > 0)
            {
                ;// adding to this->wavelenths;
            }
        }
    }

    return *this;
}

Link& Link::operator-= (LinkStateDelta& delta)
{
    int i;

    if (delta.bandwidth > 0)
    {
        for (i = 0; i < 8; i++){
            this->unreservedBandwidth[i] -= delta.bandwidth;
            if (this->unreservedBandwidth[i] < 0)
                this->unreservedBandwidth[i] = 0;
        }
    }

    list<ISCD*>::iterator iter = this->iscds.begin();
    for ( ; iter != this->iscds.end(); iter++ )
    {
        if (delta.bandwidth > 0)
        {
            for (i = 0; i < 8; i++) 
            {
                (*iter)->max_lsp_bw[i] -= delta.bandwidth;
                if ((*iter)->max_lsp_bw[i] < 0)
                    (*iter)->max_lsp_bw[i] = 0;
            }
        }
        if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
        {
            if ((delta.vlan_tag > 0 && delta.vlan_tag < MAX_VLAN_NUM) && (ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC))
            {
                RESET_VLAN((*iter)->vlan_info.bitmask, delta.vlan_tag);
                if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                    SET_VLAN((*iter)->vlan_info.bitmask_alloc, delta.vlan_tag);
            }
            if (ntohs((*iter)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI)
            {
                for (i = 0; i < MAX_TIMESLOTS_NUM/8; i++)
                    (*iter)->subnet_uni_info.timeslot_bitmask[i] &= (~delta.timeslots[i]);
            }
        }

        if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)  // || == MOVAZ_LSC(151)
        {
            if (delta.wavelength > 0)
            {
                ;// remove from this->wavelenths;
            }
        }
    }

    return *this;
}

void Link::hook_PreUpdate(Resource * oldResource)
{
    assert (oldResource != NULL);
    Link* oldLink = (Link*)oldResource;

    if (!oldLink->pDeltaList)
        return;

    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);

    //copy over
    this->pDeltaList = oldLink->pDeltaList;
    oldLink->pDeltaList = NULL;

    list<LinkStateDelta*>::iterator iter = this->pDeltaList->begin();
    for ( ; iter != this->pDeltaList->end(); iter++)
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        struct timeval timeDiff = timeNow - delta->create_time;
        if (!( timeDiff < delta->expiration))
        {//write off the expired delta
            delete delta; 
            iter = this->pDeltaList->erase(iter);
        }
        else if (modifiedTime < delta->create_time) //substract the delta
        {
            (*this) -= (*delta);
        }
    }

    //reset the modified time for this link
    modifiedTime = timeNow;
}

void Link::insertDelta(LinkStateDelta* delta, int expire_sec, int expire_usec)
{
    assert(delta);
    if (!pDeltaList)
    {
        pDeltaList = new list<LinkStateDelta*>;
    }

    list<LinkStateDelta*>::iterator iter = this->pDeltaList->begin();
    for ( ; iter != this->pDeltaList->end(); iter++)
    {
        if ((*iter)->owner_ucid == delta->owner_ucid && (*iter)->owner_seqnum == delta->owner_seqnum)
            return; //duplicate ??
    }

    gettimeofday(&delta->create_time, NULL);
    delta->expiration.tv_sec = expire_sec;
    delta->expiration.tv_usec = expire_usec;

     pDeltaList->push_back(delta);
    (*this) -= (*delta);
}

LinkStateDelta* Link::lookupDeltaByOwner(u_int32_t ucid, u_int32_t seqnum)
{
    if (!pDeltaList)
        return NULL;

    list<LinkStateDelta*>::iterator iter = pDeltaList->begin();
    for ( ; iter != pDeltaList->end(); iter++)
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        if (delta->owner_ucid == ucid && delta->owner_seqnum == seqnum)
            return delta;
    }

    return NULL;
}

LinkStateDelta* Link::removeDeltaByOwner(u_int32_t ucid, u_int32_t seqnum)
{
    if (!pDeltaList)
        return NULL;

    list<LinkStateDelta*>::iterator iter =pDeltaList->begin();
    for ( ; iter != pDeltaList->end(); iter++)
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        if (delta->owner_ucid == ucid && delta->owner_seqnum == seqnum)
        {
            this->pDeltaList->erase(iter);
            (*this) += (*delta);
            return delta;
        }
    }

    return NULL;
}

void Link::Dump()
{ 
    #ifdef HAVE_EXT_ATTR
        if (attrTable.size() == 0)
            return;
        if (type == RTYPE_GLO_ABS_LNK)
            LOGF("RTYPE_GLO_ABS_LNK(EXT_ATTR)::Link[%x->%x]: lclAddr(%x), rmtAddr(%x), metric(%d), number of ISCD (%d)\n", 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 2)].p?*(int*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 2)].p):0, advRtId, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 3)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 3)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 4)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 4)].p): 0,
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 5)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 5)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 15)].p? ((list<ISCD*>*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 15)].p))->size(): 0);
        else if (type == RTYPE_LOC_PHY_LNK)
            LOGF("RTYPE_LOC_PHY_LNK(EXT_ATTR)::Link[%x->%x]: lclAddr(%x), rmtAddr(%x), metric(%d), number of ISCD (%d)\n", 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 2)].p?*(int*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 2)].p):0, advRtId, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 3)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 3)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 4)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 4)].p): 0,
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 5)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 5)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 15)].p? ((list<ISCD*>*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 15)].p))->size(): 0);
    #else
        LOGF("Link[%x->%x]: lclAddr(%x), rmtAddr(%x)\n", id, advRtId, lclIfAddr, rmtIfAddr);
    #endif
}

ResourceDB::~ResourceDB() 
{
    ClearTree(RTYPE_LOC_PHY_LNK);
    ClearTree(RTYPE_LOC_ABS_LNK);
    ClearTree(RTYPE_LOC_RTID);
    ClearTree(RTYPE_GLO_PHY_LNK);
    ClearTree(RTYPE_GLO_ABS_LNK);
    ClearTree(RTYPE_GLO_RTID);

    list<RouterId2DomainId*>::iterator it;

    for (it = routerToDomainDirectory.begin(); it != routerToDomainDirectory.end(); it++)
        if (*it)
            delete (RouterId2DomainId*)(*it);
}

RadixNode<Resource>* ResourceDB::Allocate(ResourceType type, Prefix* prefix)
{
    assert(prefix);
    if (LookupByPrefix(type, prefix))
        return NULL;
    
    Resource *rc;
    switch (type)
    {
    case RTYPE_LOC_ABS_LNK:
    case RTYPE_LOC_PHY_LNK:
    case RTYPE_GLO_ABS_LNK:
    case RTYPE_GLO_PHY_LNK:
        rc = (Resource*)(new Link(type, 0, 0, 0));
        rc->domainId = prefix_word(*prefix, 0);
        rc->advRtId = prefix_word(*prefix, 1);
        rc->id = prefix_word(*prefix, 2);
        ((Link*)rc)->lclIfAddr = prefix_word(*prefix, 3);
        ((Link*)rc)->rmtIfAddr = prefix_word(*prefix, 4);
        break;
    case RTYPE_LOC_RTID:
    case RTYPE_GLO_RTID:
        rc = (Resource*)(new RouterId(type, 0, 0));
        rc->domainId = prefix_word(*prefix, 0);
        rc->advRtId = prefix_word(*prefix, 1);
        rc->id = prefix_word(*prefix, 2);
        break;
    default:
        return NULL;
    }

    RadixNode<Resource>* node =  r_trees[rc->Type()].InsertNode(prefix, rc);
    return node;
}

RadixNode<Resource>* ResourceDB::Insert(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    r_trees[rc->Type()].InsertNode(&index, rc); //Memory leakage here. Need Update function!
    return node;
}

RadixNode<Resource>* ResourceDB::Lookup(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    if (index.length == 0)
        return r_trees[rc->Type()].Root();
    node = r_trees[rc->Type()].LookupNode(&index);
    return node;
}

RadixNode<Resource>* ResourceDB::LookupByPrefix(ResourceType type, Prefix* prefix)
{
    RadixNode<Resource> *node;
    assert(prefix);
    node = r_trees[type].LookupNode(prefix);
    return node;
}

RadixNode<Resource>* ResourceDB::Update(Resource* rc)
{
    assert(rc);
    Prefix index = rc->Index();
    RadixNode<Resource> *node = r_trees[rc->Type()].GetNode(&index);
    if (node && node->Data())
    {
        rc->hook_PreUpdate((Resource*)node->Data());
        delete node->Data();
    }
    node->SetData(rc);
    return node;
}

void ResourceDB::Delete(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    Resource * data = r_trees[rc->Type()].DeleteNode(&index);
    if (data)
        delete data;
}

void ResourceDB::Remove(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    Resource * data = r_trees[rc->Type()].DeleteNode(&index);
}

void ResourceDB::DeleteByPrefix(ResourceType type, Prefix* prefix)
{
    RadixNode<Resource> *node;
    assert(prefix);
    Resource * data = r_trees[type].DeleteNode(prefix);
    if (data)
        delete data;
}

void ResourceDB::ClearTree(ResourceType type)
{
    r_trees[type].ClearTree();
}

Link* ResourceDB::LookupLinkByLclIf(ResourceType rcType, u_int32_t ipLcl)
{
    assert(rcType == RTYPE_GLO_ABS_LNK || rcType == RTYPE_LOC_PHY_LNK);
    RadixTree<Resource>* link_tree = RDB.Tree(rcType);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if (link = (Link*)node->Data())
            if (link->lclIfAddr == ipLcl)
                return link;
        node = link_tree->NextNode(node);
    }
    return NULL;
}

Link* ResourceDB::LookupNextLinkByLclIf(Link* prev_link)
{
    RadixTree<Resource>* link_tree = RDB.Tree(prev_link->Type());
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    bool found_prev_link = false;

    while (node)
    {
        if (link = (Link*)node->Data())
        {
            if (link->lclIfAddr == prev_link->lclIfAddr && found_prev_link)
                return link;
            if (link == prev_link)
                found_prev_link = true;
        }
        node = link_tree->NextNode(node);
    }
    return NULL;
}

void ResourceDB::WalkTree(ResourceType type)
{
    if (!Log::Debug())
        return;

    RadixTree<Resource> &tree = r_trees[type];
    RadixNode<Resource> *node = tree.Root();
    if (!node)
        return;
    do
    {
        if (!node->Data())
            continue;

        switch (type)
        {
        case RTYPE_GLO_ABS_LNK:
        case RTYPE_GLO_PHY_LNK:
        case RTYPE_LOC_ABS_LNK:
        case RTYPE_LOC_PHY_LNK:
            ((Link*)node->Data())->Dump();
            break;
        case RTYPE_GLO_RTID:
        case RTYPE_LOC_RTID:
            ((RouterId*)node->Data())->Dump();
            break;
        default:
            LOG_CERR<<"Unkown RESOURCE TYPE "<<type<<endl;
        }

        tree.UnlockNode(node);
    } while (node = tree.NextNodeHavingData(node)); 
}

