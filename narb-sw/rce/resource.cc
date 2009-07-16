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
#include "resource.hh"
#include "rce_lsp.hh"
#include "rce_movaz_types.hh"

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

void Resource::SetAttribute(int attrIndex, u_int16_t type, u_int16_t len, void* data, ResourceIndexingElement*pe)
{
    if (attrIndex < 0)
        return;
    TLP *tlp = MAKE_TLP(type, len, data);
    SetAttribute(attrIndex, tlp, pe);
}

void Resource::SetAttribute(int attrIndex, TLP *tlp, ResourceIndexingElement*pe)
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

        if (pe == NULL || pe->dataLen > tlp->l)
        {
            char* p_tmp = new char[tlp->l];
            memcpy(p_tmp, tlp->p, tlp->l);
            ((list<void*> *)attrTable[attrIndex].p)->push_back(p_tmp);
        }
        else
        {
            for (int i = 0; i < tlp->l / pe->dataLen; i++)
            {
                char* p_tmp = new char[pe->dataLen];
                memcpy(p_tmp, (char*)tlp->p+(pe->dataLen*i), pe->dataLen);
                ((list<void*> *)attrTable[attrIndex].p)->push_back((void*)(p_tmp));
            }
        }
    }
    else
    {
        attrTable[attrIndex] = *tlp;
        attrTable[attrIndex].p = new char[tlp->l];
        memcpy(attrTable[attrIndex].p, tlp->p, tlp->l);
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
#ifdef HAVE_EXT_ATTR
    for (int i = 0; i < attrTable.size(); i++)
    {
        TLP *tlp = &attrTable[i];
        if (tlp->t == N_LIST || tlp->t == P_LIST || tlp->t == N_VECTOR || tlp->t == P_VECTOR)
        {
            /*@@@@ currently only ISCD and TE_LAMBDA are of LIST type; ISCD list should not be removed but TE_LAMBDA has to.
            if (tlp->p)
            {
                list<void*>::iterator iter = ((list<void*> *)tlp->p)->begin();
                while (iter != ((list<void*> *)tlp->p)->end())
                {
                    if ((*iter) !=NULL)
                        delete[] (char*)(*iter);
                    iter++;
                }
                delete ((list<void*> *)tlp->p);
            }
            */
        }
        else if(tlp->p)
        {
            delete[] (char*)tlp->p;
        }
    }
#endif
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

    if (link->pDeltaList)
    {
        this->pDeltaList = new list<LinkStateDelta*>;
        list<LinkStateDelta*>::iterator it4 = link->pDeltaList->begin();
        for ( ; it4 != link->pDeltaList->end(); it4++)
        {
            LinkStateDelta* delta1 = new LinkStateDelta;
            LinkStateDelta* delta2 = *it4;
            memcpy(delta1, delta2, sizeof(LinkStateDelta));
            this->pDeltaList->push_back(delta1);
        }
   }
   else
       this->pDeltaList = NULL;

#ifdef HAVE_EXT_ATTR
    this->attrTable.assign(link->attrTable.begin(), link->attrTable.end());
    for (int i = 0; i < link->attrTable.size(); i++)
    {
        TLP* tlp1 = &link->attrTable[i];
        TLP* tlp2 = &this->attrTable[i]; 
        char* pData;
        if (tlp1->t == N_LIST || tlp1->t == P_LIST || tlp1->t == N_VECTOR || tlp1->t == P_VECTOR)
        {
            if (tlp1->p)
            {
                tlp2->p = new list<void*>;
                list<void*>::iterator iter = ((list<void*> *)tlp1->p)->begin();
                while (iter != ((list<void*> *)tlp1->p)->end())
                {
                    if ((*iter) !=NULL)
                    {
                        char* p_tmp = new char[tlp1->l];
                        memcpy(p_tmp, tlp1->p, tlp1->l);
                        ((list<void*> *)tlp2->p)->push_back(p_tmp);
                    }
                    iter++;
                }
            }
        }
        else if (tlp1->p)
        {
            tlp2->p = new char[tlp1->l];
            memcpy(tlp2->p, tlp1->p, tlp1->l);
        }
    }
#endif
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
        delete pDeltaList;
    }
    
    // Clean up dependency?
}

Link& Link::operator=(Link& link)
{
    //Resource
    this->type = link.type;
    this->domainMask = link.domainMask;
    this->domainId = link.domainId;
    this->advRtId = link.advRtId;
    this->id = link.id;
    //Link
    this->linkType = link.linkType;
    this->lclIfAddr = link.lclIfAddr;
    this->rmtIfAddr = link.rmtIfAddr;
    this->metric = link.metric;
    this->maxBandwidth = link.maxBandwidth;
    this->maxReservableBandwidth = link.maxReservableBandwidth;
    this->minReservableBandwidth = link.minReservableBandwidth;
    for (int i = 0; i < 8; i++)
        this->unreservedBandwidth[i] = link.unreservedBandwidth[i];
    this->rcClass = link.rcClass;
    this->lclId = link.lclId;
    this->rmtId = link.rmtId;
    this->resvTable = link.resvTable;
    this->dependings = link.dependings;
    this->dependents= link.dependents;

    this->iscds.clear();
    list<ISCD*>::iterator it1;
    for (it1 = link.iscds.begin(); it1 != link.iscds.end(); it1++)
    {
        ISCD *iscd = new ISCD;
        *iscd = *(*it1);
        this->iscds.push_back(iscd);
    }
    this->iacds.clear();
    list<IACD*>::iterator it2;
    for (it2 = link.iacds.begin(); it2 != link.iacds.end(); it2++)
    {
        IACD *iacd = new IACD(*(*it2));
        this->iacds.push_back(iacd);
    }
    this->wavelenths.clear();
    list<Wavelength*>::iterator it3;
    for (it3 = link.wavelenths.begin(); it3 != link.wavelenths.end(); it3++)
    {
        Wavelength* wave = new Wavelength(*(*it3));
        this->wavelenths.push_back(wave);
    }

    if (link.pDeltaList)
    {
        this->pDeltaList = new list<LinkStateDelta*>;
        list<LinkStateDelta*>::iterator it4 = link.pDeltaList->begin();
        for ( ; it4 != link.pDeltaList->end(); it4++)
        {
            LinkStateDelta* delta1 = new LinkStateDelta;
            LinkStateDelta* delta2 = *it4;
            memcpy(delta1, delta2, sizeof(LinkStateDelta));
            this->pDeltaList->push_back(delta1);
        }
   }
   else
       this->pDeltaList = NULL;

#ifdef HAVE_EXT_ATTR
    this->attrTable.assign(link.attrTable.begin(), link.attrTable.end());
    for (int i = 0; i < link.attrTable.size(); i++)
    {
        TLP* tlp1 = &link.attrTable[i];
        TLP* tlp2 = &this->attrTable[i]; 
        char* pData;
        if (tlp1->t == N_LIST || tlp1->t == P_LIST || tlp1->t == N_VECTOR || tlp1->t == P_VECTOR)
        {
            if (tlp1->p)
            {
                tlp2->p = new list<void*>;
                list<void*>::iterator iter = ((list<void*> *)tlp1->p)->begin();
                while (iter != ((list<void*> *)tlp1->p)->end())
                {
                    if ((*iter) !=NULL)
                    {
                        char* p_tmp = new char[tlp1->l];
                        memcpy(p_tmp, tlp1->p, tlp1->l);
                        ((list<void*> *)tlp2->p)->push_back(p_tmp);
                    }
                    iter++;
                }
            }
        }
        else if (tlp1->p)
        {
            tlp2->p = new char[tlp1->l];
            memcpy(tlp2->p, tlp1->p, tlp1->l);
        }
    }
#endif
}

void Link::SetWavelength(u_int32_t lambda, bool deleting)
{
#ifdef HAVE_EXT_ATTR
    movaz_tlvdata_te_lambda_info lambda_info;
    movaz_tlvdata_te_passthrough_wavegrid wavegrid;
    memset(&lambda_info, 0, sizeof(movaz_tlvdata_te_lambda_info));
    lambda_info.priority = 0x07;
    memcpy(lambda_info.sw_cap, "\000\020\000\00d", 4);
    lambda_info.data_rate= 0x4E9502F9;
    memset(&wavegrid, 0, sizeof(movaz_tlvdata_te_passthrough_wavegrid));
    wavegrid.base = htonl(192000);
    wavegrid.interval = htons(100);
    wavegrid.size = htons(40);

    if (this->Iscds().size() > 0 && this->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)
    {
        ResourceIndexingElement *pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA");
        int a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA");
        if (pe == NULL || a_index == 0)
            return;

        int l = (lambda-192000)/100; 
        if (l < 0 || l >= 40)
        {
           LOGF("Link::AddWavelength cannot take wavelength frequency %d \n", lambda);
           return;
        }

        lambda_info.channel_id = htonl(lambda);
        list<void*> *p_list;
        if (attrTable.size() > a_index && (p_list = (list<void*>*)(attrTable[a_index].p)) != NULL)
        {
            list<void*>::iterator iter = p_list->begin();
            MovazTeLambda tel;
            for (; iter != p_list->end(); iter++)
            {
                tel = *(MovazTeLambda*)(*iter);
                if (tel.channel_id == lambda_info.channel_id)
                {
                    if (deleting)
                    {
                        p_list->erase(iter);
                    }
                    break;
                }
            }
            if (!deleting && iter == p_list->end())
                SetAttribute(a_index, pe->dataType, sizeof(movaz_tlvdata_te_lambda_info), (char*)&lambda_info, pe);
        }

        pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID");
        a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID");
        if (deleting)
        {
            wavegrid.in_channels[l/2] &= ~((0x70) >> ((l%2)*4));
            wavegrid.out_channels[l/2] &= ~((0x70) >> ((l%2)*4));
        }
        else
        {
            wavegrid.in_channels[l/2] |= ((0x70) >> ((l%2)*4)); // l even: 0xf0; l odd: ox0f;
            wavegrid.out_channels[l/2] |= ((0x70) >> ((l%2)*4)); // l even: 0xf0; l odd: ox0f;
        }
        if (attrTable.size() > a_index && attrTable[a_index].p)
            SetAttribute(a_index, pe->dataType, sizeof(movaz_tlvdata_te_passthrough_wavegrid), (char*)&wavegrid, pe);
    }
    
    if (this->Iscds().size() > 0 && this->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC)
    {
        ResourceIndexingElement *pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA");
        int a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA");
        if (attrTable.size() > a_index && attrTable[a_index].p)
        {
            if (deleting)
                attrTable[a_index].p = NULL;
            else if (this->Iscds().size() > 0 && this->Iscds().front()->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_PSC4)
                SetAttribute(a_index, pe->dataType, 4, (char*)&lambda, pe);
        }
    }
#endif
}

Link& Link::operator+= (LinkStateDelta& delta)
{
    int i;

    if (delta.flags & DELTA_MASKOFF)
        return *this;

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
            if (ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
            {
                if (delta.flags & DELTA_VLANTAG && (delta.vlan_tag > 0 && delta.vlan_tag < MAX_VLAN_NUM))
                {
                    SET_VLAN((*iter)->vlan_info.bitmask, delta.vlan_tag);
                    if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                        RESET_VLAN((*iter)->vlan_info.bitmask_alloc, delta.vlan_tag);
                }
                else if (delta.flags & DELTA_VTAGMASK)
                {
                    for (i = 0; i < MAX_VLAN_NUM/8; i++)
                    {
                        (*iter)->vlan_info.bitmask[i] |= delta.vtag_mask[i]; //set
                        if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                            (*iter)->vlan_info.bitmask_alloc[i] &= (~delta.vtag_mask[i]); //reset
                    }
                }
            }
        }
        else if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && (ntohs((*iter)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI))
        {
            if (delta.flags & DELTA_TIMESLOTS)
            {
                for (i = 0; i < MAX_TIMESLOTS_NUM/8; i++)
                    (*iter)->subnet_uni_info.timeslot_bitmask[i] |= delta.timeslots[i];
            }
        }
        else if (delta.flags & DELTA_WAVELENGTH && delta.wavelength != 0)
        {
            this->AddWavelength(delta.wavelength);
        }
    }

    return *this;
}

Link& Link::operator-= (LinkStateDelta& delta)
{
    int i;

    if (delta.flags & DELTA_MASKOFF)
        return *this;

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
            if (ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
            {
                if (delta.flags & DELTA_VLANTAG && (delta.vlan_tag > 0 && delta.vlan_tag < MAX_VLAN_NUM))
                {
                    RESET_VLAN((*iter)->vlan_info.bitmask, delta.vlan_tag);
                    if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                        SET_VLAN((*iter)->vlan_info.bitmask_alloc, delta.vlan_tag);
                }
                else if (delta.flags & DELTA_VTAGMASK)
                {
                    for (i = 0; i < MAX_VLAN_NUM/8; i++)
                    {
                        (*iter)->vlan_info.bitmask[i] &= (~delta.vtag_mask[i]);//reset
                        if ((ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC))
                            (*iter)->vlan_info.bitmask_alloc[i] |= delta.vtag_mask[i]; //set
                    }
                }
            }
        }
        else if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && (ntohs((*iter)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI))
        {
                if (delta.flags & DELTA_TIMESLOTS)
                {
                    for (i = 0; i < MAX_TIMESLOTS_NUM/8; i++)
                        (*iter)->subnet_uni_info.timeslot_bitmask[i] &= (~delta.timeslots[i]);
                }
        }
        else if (delta.flags & DELTA_WAVELENGTH && delta.wavelength != 0)
        {
            this->RemoveWavelength(delta.wavelength);
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

    //eliminating double holding by zero'ing bandwidth of the deltas owned by GRI's in OSPF TE link updates
    int iAttr = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_GRI");
    if (iAttr > 0 && iAttr < this->attrTable.size())
    {
        list<GRI*> *p_list = (list<GRI*>*)this->attrTable[iAttr].p;
        if (p_list != NULL)
        {
            list<GRI*>::iterator iter_gri;
            for (iter_gri = p_list->begin(); iter_gri != p_list->end(); iter_gri++)
            {
                LinkStateDelta* delta = this->removeDeltaByOwner(ntohl((*iter_gri)->ucid), ntohl((*iter_gri)->seqnum), false);
                if (delta != NULL) delete delta;
            }
        }
    }
    
    //removing expired deltas
    list<LinkStateDelta*>::iterator iter = this->pDeltaList->begin();
    while (iter != this->pDeltaList->end())
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        struct timeval timeDiff = timeNow - delta->create_time;
        if (timeDiff < delta->expiration)
        { // keep the delta
            (*this) -= (*delta);
            iter++;
        }
        else //$$$$ xx if (modifiedTime < delta->create_time)
        {//write off the expired delta, and no need to add back.
            delete delta; 
            iter = this->pDeltaList->erase(iter);
            if (this->pDeltaList->size() == 0)
                break;
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

    gettimeofday(&delta->create_time, NULL);
    delta->expiration.tv_sec = expire_sec;
    delta->expiration.tv_usec = expire_usec;

    pDeltaList->push_back(delta);

    int iAttr = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_GRI");
    if (iAttr > 0 && iAttr < this->attrTable.size())
    {
        list<GRI*> *p_list = (list<GRI*>*)this->attrTable[iAttr].p;
        if (p_list != NULL)
        {
            list<GRI*>::iterator iter_gri;
            for (iter_gri = p_list->begin(); iter_gri != p_list->end(); iter_gri++)
            {
                if (ntohl((*iter_gri)->ucid) == delta->owner_ucid && ntohl((*iter_gri)->seqnum) == delta->owner_seqnum)
                    return; //a TE Link LSA update for this GRI has arrived, no need to reduce the resource by this delta
            }
        }
    }
    //reducing link resource by the delta
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

LinkStateDelta* Link::removeDeltaByOwner(u_int32_t ucid, u_int32_t seqnum, bool doAddition)
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
            if (doAddition) 
            {
                delta->flags &= ~DELTA_MASKOFF;
                (*this) += (*delta);
            }
            return delta;
        }
    }

    return NULL;
}

void Link::deleteExpiredDeltas()
{
    if (!pDeltaList) return;

    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);

    list<LinkStateDelta*>::iterator iter =pDeltaList->begin();
    while (iter != pDeltaList->end())
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        struct timeval timeDiff = timeNow - delta->create_time;
        if (!( timeDiff < delta->expiration))
        {
            // $$$$ write off the expired delta with states added back to the link
            // only add back the resources held by 'query'
            // 'reserved' resources won't be implicitly added back --> only by release message
            if (delta->expiration.tv_sec == SystemConfig::delta_expire_query || delta->expiration.tv_sec == SystemConfig::delta_expire_subnet_reserve)
            {
                delta->flags &= ~DELTA_MASKOFF;
                (*this) += (*delta);
            }
            delete delta; 
            iter = this->pDeltaList->erase(iter);
            modifiedTime = timeNow;
        }
        else
            iter++;
    }
}

void Link::cleanupMaskoffDeltas()
{
    if (!pDeltaList) return;

    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);

    list<LinkStateDelta*>::iterator iter =pDeltaList->begin();
    while (iter != pDeltaList->end())
    {
        LinkStateDelta* delta = *iter;
        assert(delta);
        if ((delta->flags & DELTA_MASKOFF) != 0)
        {
            (*this) += (*delta);
            delta->flags &= ~DELTA_MASKOFF;
        }
    }
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
    ClearTree(RTYPE_LOC_PHY_LNK_INCOMPLETE);
    ClearTree(RTYPE_GLO_ABS_LNK_INCOMPLETE);

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

Link* ResourceDB::LookupLinkByLclIf(ResourceType rcType, in_addr  lclIf)
{
    assert(rcType == RTYPE_GLO_ABS_LNK || rcType == RTYPE_LOC_PHY_LNK);
    RadixTree<Resource>* link_tree = RDB.Tree(rcType);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if (link = (Link*)node->Data())
            if (link->lclIfAddr == lclIf.s_addr)
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

Link* ResourceDB::LookupLinkByLclRmtIf(ResourceType rcType, in_addr lclIf, in_addr rmtIf)
{
    assert(rcType == RTYPE_GLO_ABS_LNK || rcType == RTYPE_LOC_PHY_LNK);
    RadixTree<Resource>* link_tree = RDB.Tree(rcType);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if (link = (Link*)node->Data())
            if (link->lclIfAddr == lclIf.s_addr && link->rmtIfAddr == rmtIf.s_addr)
                return link;
        node = link_tree->NextNode(node);
    }
    return NULL;
}

Link* ResourceDB::LookupNextLinkByLclRmtIf(Link* prev_link)
{
    RadixTree<Resource>* link_tree = RDB.Tree(prev_link->Type());
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    bool found_prev_link = false;

    while (node)
    {
        if (link = (Link*)node->Data())
        {
            if (link->lclIfAddr == prev_link->lclIfAddr && link->rmtIfAddr == prev_link->rmtIfAddr && found_prev_link)
                return link;
            if (link == prev_link)
                found_prev_link = true;
        }
        node = link_tree->NextNode(node);
    }
    return NULL;
}

Link* ResourceDB::LookupLinkBySubnetLocalId(in_addr rtId, u_int32_t localId)
{
    RadixTree<Resource>* link_tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if ((link = (Link*)node->Data()) != NULL && link->advRtId == rtId.s_addr 
         && (localId >> 16) == LOCAL_ID_TYPE_SUBNET_IF_ID && link->Iscds().size() > 0)
        {
            list<ISCD*>::iterator iter_iscd = link->Iscds().begin();
            for ( ; iter_iscd != link->Iscds().end(); iter_iscd++)
            {
                if ( (*iter_iscd)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM 
                 && (((*iter_iscd)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0 
                 && (*iter_iscd)->subnet_uni_info.subnet_uni_id == ((localId >> 8) & 0xff))
                    return link;
            }
        }
        node = link_tree->NextNode(node);
    }
    return NULL;
}

RadixNode<Resource>* ResourceDB::LookupIncompleteLinkNode(ResourceType rcType, Prefix* prefix)
{
    if (rcType == RTYPE_LOC_PHY_LNK)
        rcType = RTYPE_LOC_PHY_LNK_INCOMPLETE;
    else if (rcType == RTYPE_GLO_ABS_LNK)
        rcType = RTYPE_GLO_ABS_LNK_INCOMPLETE;
    else if (rcType != RTYPE_LOC_PHY_LNK_INCOMPLETE && rcType != RTYPE_GLO_ABS_LNK_INCOMPLETE)
        return NULL;

    RadixNode<Resource>* node = r_trees[rcType].Root();
    while (node)
    {
        if ((*prefix == (node->Index())) && (node->Data() != NULL))
            return node;
        node = r_trees[rcType].NextNode(node);
    }
    return NULL;
}

void ResourceDB::BookmarkIncompleteLink(Link* link)
{
    assert(link);
    ResourceType rcIncompleteType;
    if (link->Type() == RTYPE_LOC_PHY_LNK)
        rcIncompleteType = RTYPE_LOC_PHY_LNK_INCOMPLETE;
    else if (link->Type() == RTYPE_GLO_ABS_LNK)
        rcIncompleteType = RTYPE_GLO_ABS_LNK_INCOMPLETE;
    else
        return;
    Prefix prefix_incomplete = link->IncompleteIndex();
    r_trees[rcIncompleteType].InsertNode(&prefix_incomplete, (Resource*)link);
}

void ResourceDB::RemoveIncompleteLinkNode(ResourceType rcIncompleteType, RadixNode<Resource>* node)
{
    if (rcIncompleteType != RTYPE_LOC_PHY_LNK_INCOMPLETE && rcIncompleteType != RTYPE_GLO_ABS_LNK_INCOMPLETE)
        return;
    node->SetData(NULL);
    r_trees[rcIncompleteType].DeleteNode(node);
    //do not remove node data (pointed to by node in a regular RDB tree)
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
            LOG_CERR<<"Unknown RESOURCE TYPE "<<type<<endl;
        }

        tree.UnlockNode(node);
    } while (node = tree.NextNodeHavingData(node)); 
}

