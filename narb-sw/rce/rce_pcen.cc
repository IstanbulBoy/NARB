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

#include "radixtree.hh"
#include "resource.hh"
#include "rce_filter.hh"
#include "rce_pcen.hh"
#include "rce_lsa.hh"
#include "rce_movaz_types.hh"
#include "rce_subnet.hh"
#include <netinet/in.h>
#include <arpa/inet.h>

u_int32_t PCENNode::DomainId ()
{
    if (router == NULL)
        return DOMAIN_ID_UNKNOWN;

    list<RouterId2DomainId*>::iterator iter;
    RouterId2DomainId* r2d_conv;
    for (iter = RDB.routerToDomainDirectory.begin(); iter != RDB.routerToDomainDirectory.end(); iter++)
    {
        r2d_conv = *iter;
        if (router->advRtId == r2d_conv->router_id)
        {
            router->domainId = r2d_conv->domain_id;
            return router->domainId;
        }
    }
    
    router->domainId = DOMAIN_ID_UNKNOWN;
    return router->domainId;
}

PCENLink * PCENNode::GetOutLinkByIp(in_addr * ip)
{
    list<PCENLink*>::iterator iter;
    PCENLink * pcen_link;

    for (iter = out_links.begin(); iter != out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->link == NULL)
            continue;
        if (pcen_link->link->id == ip->s_addr || pcen_link->link->rmtIfAddr== ip->s_addr)
            return pcen_link;
    }
    return NULL;
}

PCENLink * PCENNode::GetInLinkByIp(in_addr * ip)
{
    list<PCENLink*>::iterator iter;
    PCENLink * pcen_link;

    for (iter = out_links.begin(); iter != out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->link == NULL)
            continue;
        if (pcen_link->link->advRtId == ip->s_addr || pcen_link->link->lclIfAddr== ip->s_addr)
            return pcen_link;
    }
    return NULL;    
}

u_int32_t PCENLink::HeadDomainId ()
{
    assert(lcl_end);
    return lcl_end->DomainId();
}

u_int32_t PCENLink::TailDomainId ()
{
    assert(rmt_end);
    return rmt_end->DomainId();
}

void PCENNode::DisplayInfo()
{
    if (!router)
    {
        cout << "NODE[empty] "<<endl;
        return;
    }

    in_addr ip;
    ip.s_addr = router->id;
    char addr[20];
    inet_ntop(AF_INET, &ip, addr, 20);
    cout <<"NODE[" << addr << "]" <<endl;
}

void PCENNode::ShowPath()
{
    if (path.size() == 0)
    {
        cout<<"Unreachable... "<<endl;
        return;
    }

    cout <<"Via Path: "<<endl;
    
    in_addr ip1, ip2;
    char addr1[20], addr2[20];
    Link* link;    
    list<PCENLink*>::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        link = (*it)->link;

        ip1.s_addr = link->lclIfAddr;
        ip2.s_addr = link->rmtIfAddr;

        inet_ntop(AF_INET, &ip1, addr1, 20);
        inet_ntop(AF_INET, &ip2, addr2, 20);
        cout<<"["<<addr1 <<"-" <<addr2 <<"] ";
    }
    
    cout << " at cost [" <<minCost<< "]" <<endl;
}

void  PCENLink::DisplayInfo()
{
    if (!link)
    {
        cout <<"LINK[void] "<<endl;
        return;
    }

    in_addr ip, ip1, ip2;
    ip.s_addr = link->advRtId;
    ip1.s_addr = link->lclIfAddr;
    ip2.s_addr = link->rmtIfAddr;
    char addr[20], addr1[20], addr2[20];
    inet_ntop(AF_INET, &ip, addr, 20);
    inet_ntop(AF_INET, &ip1, addr1, 20);
    inet_ntop(AF_INET, &ip2, addr2, 20);
    cout << "LINK@" << addr << "[" << addr1 << "-" <<addr2 << "] ";
}

#ifdef HAVE_EXT_ATTR
void*  PCENLink::AttributeById (u_int16_t tlv_type)
{
    int index = ATTR_INDEX(link->type, tlv_type);
    if (index < 0 || index > link->attrTable.size() - 1)
        return NULL;

    return link->attrTable[index].p;
}

void*  PCENLink::AttributeByTag (const char * tag)
{
    int index = ATTR_INDEX_BY_TAG(tag);
    if (index < 0 || index > link->attrTable.size() - 1)
        return NULL;

    return link->attrTable[index].p;
}
#endif


PCENNode::PCENNode(int id): waveset(MAX_WAVE_NUM, 190000, 100), timeslotset(MAX_SUBWAVE_CHANNELS), vtagset(MAX_VLAN_NUM)
{
    ref_num=id;
    Init();
}

void PCENNode::FilterNode() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=1;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=1;
        itLink++;
    }
    this->nflg.nfg.filteroff=1;
}

void PCENNode::RestoreNodeFilter() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=0;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.filteroff=0;
        itLink++;
    }
    this->nflg.nfg.filteroff=0;
}

void PCENNode::MaskNode() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=1;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=1;
        itLink++;
    }
    this->nflg.nfg.maskoff=1;
}

void PCENNode::RestoreNodeMask() 
{
    list<PCENLink*>::iterator itLink;
    itLink=out_links.begin();
    while (itLink!=out_links.end()) 
    {
        (*itLink)->lflg.lfg.maskoff=0;
        itLink++;
    }
    itLink=in_links.begin();
    while (itLink!=in_links.end()) 
    {
    	(*itLink)->lflg.lfg.maskoff=0;
    	itLink++;
    }
    this->nflg.nfg.maskoff=0;
}

bool PCENLink::IsAvailableForTspec(TSpec& tspec)
{
    TSpec tspec_link;
    ISCD * iscd;
    list<ISCD*>::iterator it;
    for (it = link->iscds.begin(); it != link->iscds.end(); it++)
    {
        iscd = *it;
        assert(iscd);
        tspec_link.Update(iscd->swtype, iscd->encoding, iscd->max_lsp_bw[7]);//?

        if (tspec == tspec_link)
            return true;

        //@@@@ A temporary matching (available) condition for LSC and FSC links
        //@@@@ A testing code for now. We need to further consider meaning of bandwidth parameters in LSC and FSC.
        if ( (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC)
            && tspec_link.SWtype == tspec.SWtype && tspec_link.ENCtype == tspec.ENCtype )
                return true;

        //@@@@ Movaz special handling
        if ( tspec.SWtype == MOVAZ_LSC && tspec_link.SWtype == MOVAZ_LSC 
            ||tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && tspec_link.SWtype == MOVAZ_LSC 
            ||tspec_link.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC && tspec.SWtype == MOVAZ_LSC )
                return true;

        // $$$$ No bandwidth constraints for now. OK for MOVAZ special handling. 
        // $$$$ Wavelength (lambda) constraints will be taken care after this function call.
        //@@@@ End

        if (tspec <= tspec_link)
        {
            if (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM)
            {
               if (tspec.ENCtype == LINK_IFSWCAP_SUBTLV_ENC_G709ODUK && !this->IsCienaOTNXInterface())
                    return true; // no need for timeslots checking as that will be done by ::ProceedByUpdatingTimeslots
               else if (float_equal(iscd->min_lsp_bw, 0) ||float_evenly_dividable(tspec.Bandwidth, iscd->min_lsp_bw))
                    return true;
            }
            else if (tspec.SWtype >= LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 && 
                    tspec.SWtype <=  LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 ||
                    tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
                return true;
            else //tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || LINK_IFSWCAP_SUBTLV_SWCAP_FSC
                continue;
        }
    }

    return false;
}

bool PCENLink::CanBeEgressLink(TSpec& tspec)
{
    TSpec tspec_link;
    ISCD * iscd;
    list<ISCD*>::iterator it;

    //@@@@ link ==> reverse_link
    if (reverse_link == NULL || reverse_link->link == NULL)
        return false;
    for (it = reverse_link->link->iscds.begin(); it != reverse_link->link->iscds.end(); it++)
    {
        iscd = *it;
        assert(iscd);
        tspec_link.Update(iscd->swtype, iscd->encoding, iscd->max_lsp_bw[7]);//?
        if (tspec_link.ENCtype != tspec.ENCtype)
            continue;

        if (tspec == tspec_link)
            return true;

        if (tspec <= tspec_link)
        {
            if (tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM )
            {
                if (float_equal(iscd->min_lsp_bw, 0) ||float_evenly_dividable(tspec.Bandwidth, iscd->min_lsp_bw))
                    return true;
            }
            else if (tspec.SWtype >= LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 && 
                    tspec.SWtype <=  LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 ||
                    tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
                return true;
            else //tspec.SWtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC || LINK_IFSWCAP_SUBTLV_SWCAP_FSC
                continue;
        }
    }

    return false;
}


//$$$$ Ciena OTNx-interface
bool PCENLink::IsCienaOTNXInterface()
{
    if (!this->link)
        return false;
    list<ISCD*>::iterator it_iscd = link->Iscds().begin();
    for (; it_iscd != link->Iscds().end(); it_iscd++)
    {
        ISCD* iscd = *it_iscd;
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && iscd->encoding == LINK_IFSWCAP_SUBTLV_ENC_G709ODUK
            && (iscd->ciena_opvcx_info.version & IFSWCAP_SPECIFIC_CIENA_OPVCX) != 0)
            return true;
    }
    return false;
}

//$$$$ only ADVA/Movaz specific handling for now, other WDM foramt has to be converted into the ADVA/Movaz format
//$$$$ only constrain the forward direction (not checking the reverse)
void PCENLink::ProceedByUpdatingWaves(ConstraintTagSet &head_waveset, ConstraintTagSet &next_waveset)
{
    next_waveset.Clear();
    bool any_wave_ok = head_waveset.HasAnyTag();

    //$$$$ ADVA/Movaz specific TE info
    // Retieve available wavelength information based on TE Wavelength Grid (present in LSAs originated from ROADMs)
    MovazWaveGrid* wavegrid = (MovazWaveGrid*)(this->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID"));
    if (wavegrid != NULL)
    {
        int l;
        u_int32_t base = ntohl(wavegrid->base);
        u_int16_t interval = ntohs(wavegrid->interval);
        u_int16_t size = ntohs(wavegrid->size);
        for (l = 0; l < (int)size/2; l++)
        {
            if ((wavegrid->out_channels[l] & 0xf0) == 0x70) // f == channel unavailable; 0~7 == priority level
                next_waveset.AddTag(base+l*2*interval);
            if ((wavegrid->out_channels[l] & 0x0f) == 0x07)
                next_waveset.AddTag(base+(l*2+1)*interval);
        }
    }

    //$$$$ ADVA/Movaz specific TE info
    // Retieve available wavelength information based on TE Lambda list (present in LSAs originated from REs)
    list<void*> *p_list = (list<void*>*)(this->AttributeByTag("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA"));
    MovazTeLambda tel;
    bool has_wave = false;
    if (p_list != NULL)
    {
        list<void*>::iterator it;
        for (it = p_list->begin(); it!= p_list->end(); it++)
        {
            tel = *(MovazTeLambda*)(*it);
            ntoh_telambda(tel);
            if (tel.priority == 0x07)
            {
                if (valid_channel(tel.channel_id))
                {
                    next_waveset.AddTag(tel.channel_id);
                    has_wave = true;
                }
                else 
                    next_waveset.DeleteTag(tel.channel_id);
            }
        }
    }

    if (this->reverse_link) //$$$$ Movaz-RE--->VLSR speical handling
    {
        u_int32_t* p_freq = (u_int32_t*)(reverse_link->AttributeByTag("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA"));
        if (p_freq && !has_wave)
        {
            next_waveset.AddTag(ntohl(*p_freq));
        }
        else if (p_freq)
        {
            if(!next_waveset.HasTag(ntohl(*p_freq)))
            {
                next_waveset.Clear();
            }
        }
    }
    
    if (!any_wave_ok)
        next_waveset.Intersect(head_waveset);
}

//$$$$ Ciena OTNx-interface
void PCENLink::ProceedByUpdatingOTNXTimeslots(ConstraintTagSet &head_timeslotset, ConstraintTagSet &next_timeslotset)
{
    if (!IsCienaOTNXInterface()) //currentlywe only constrain timeslots for Ciena-OTNX
        return;

    next_timeslotset.Clear();
    bool any_timeslot_ok = head_timeslotset.HasAnyTag();

    // Constrain forward link only as we assume bidirectional, symetric provisioning
    // Retieve available timeslots
    list<ISCD*>::iterator it = link->Iscds().begin();
    for (; it != link->Iscds().end(); it++)
    {
        ISCD* iscd = *it;
        if (!iscd)
            continue;
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && iscd->encoding == LINK_IFSWCAP_SUBTLV_ENC_G709ODUK
            && (iscd->ciena_opvcx_info.version & IFSWCAP_SPECIFIC_CIENA_OPVCX) != 0)
        {
            next_timeslotset.AddTags(iscd->ciena_opvcx_info.wave_opvc_map[0].opvc_bitmask, iscd->ciena_opvcx_info.num_chans);
        }
    }

    if (!any_timeslot_ok)
        next_timeslotset.Intersect(head_timeslotset);

    PCEN::TrimOTNXTimeslotsByBandwidth(next_timeslotset, this->lcl_end->tspec.Bandwidth);
}

//$$$$ Only constraining the forward direction (not checking the reverse; assuming symetric L2SC link configurations)
void PCENLink::ProceedByUpdatingVtags(ConstraintTagSet &head_vtagset, ConstraintTagSet &next_vtagset)
{
    next_vtagset.Clear();
    list<ISCD*>::iterator it;
    ISCD * iscd;
    bool any_vlan_ok = head_vtagset.HasAnyTag();
    bool non_vlan_link = true;

    // Add VLAN tags available for this link.
    for (it = link->iscds.begin(); it != link->iscds.end(); it++)
    {
        iscd = *it;
        if (!iscd)
            continue;
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_LSC
            || iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_FSC) // The non-L2SC layers are temoperaty here and yet to remove.
        {
            if (ntohs(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC)
            {
                non_vlan_link = false;
                next_vtagset.AddTags(iscd->vlan_info.bitmask, MAX_VLAN_NUM);
            }
        }
    }

    // Add VLAN tags used by any links on the local- or remote-end nodes of the link.
    list<PCENLink*>::iterator it_link;
    if (lcl_end)
    {
        for (it_link = lcl_end->in_links.begin(); it_link != lcl_end->in_links.end(); it_link++)
        {
            (*it_link)->ExcludeAllocatedVtags(next_vtagset);
        }
        for (it_link = lcl_end->out_links.begin(); it_link != lcl_end->out_links.end(); it_link++)
        {
            if (*it_link != this)
                (*it_link)->ExcludeAllocatedVtags(next_vtagset);
        }
    }
    if (rmt_end)
    {
        for (it_link = rmt_end->in_links.begin(); it_link != rmt_end->in_links.end(); it_link++)
        {
            if (*it_link != this)
                (*it_link)->ExcludeAllocatedVtags(next_vtagset);
        }
        for (it_link = rmt_end->out_links.begin(); it_link != rmt_end->out_links.end(); it_link++)
        {
            (*it_link)->ExcludeAllocatedVtags(next_vtagset);
        }
    }
        
    if (non_vlan_link)
        next_vtagset = head_vtagset;
    else if (!any_vlan_ok)
        next_vtagset.Intersect(head_vtagset);
}

void PCENLink::ExcludeAllocatedVtags(ConstraintTagSet &vtagset)
{
    list<ISCD*>::iterator it;
    ISCD * iscd;

    for (it = link->iscds.begin(); it != link->iscds.end(); it++)
    {
        iscd = *it;
        if (!iscd)
            continue;
        if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC)
        {
            if (ntohs(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_ALLOC)
            {
                vtagset.DeleteTags(iscd->vlan_info.bitmask_alloc, MAX_VLAN_NUM);
            }
        }
    }
}

bool PCENLink::CrossingRegionBoundary(TSpec& tspec)
{
    assert(this->link);

    // Check adaptation defined by IACD(s)
    list<IACD*>::iterator it_iacd;
    for (it_iacd = this->link->iacds.begin(); it_iacd != this->link->iacds.end(); it_iacd++)
    {
        //crossing from lower layer to upper layer
        if ((*it_iacd)->swtype_lower == tspec.SWtype && (*it_iacd)->encoding_lower == tspec.ENCtype)
            return true;
        //crossing from upper layer to lower layer
        if ((*it_iacd)->swtype_upper == tspec.SWtype && (*it_iacd)->encoding_upper == tspec.ENCtype)
            return true;

         // @@@@ bandwidth criteria to be considered in the future.
    }

    // Check implicit adaptation
    if (this->reverse_link && this->reverse_link->link)
    {
        if (this->link->iscds.size()* this->reverse_link->link->iscds.size() > 1) 
            return true; //at least one direction of the link supports multiple ISCDs
        if (this->link->iscds.size() == 1 && this->reverse_link->link->iscds.size() == 1
            && this->link->iscds.front()->swtype != this->reverse_link->link->iscds.front()->swtype)
            return true; //implicit adaptation
    }

    return false;
}

int PCENLink::GetNextRegionTspec(TSpec& tspec)
{
    assert(this->link);

    // Check adaptation defined by IACD(s)
    list<IACD*>::iterator it_iacd;
    for (it_iacd = this->link->iacds.begin(); it_iacd != this->link->iacds.end(); it_iacd++)
    {
        //crossing from lower layer to upper layer
        if ((*it_iacd)->swtype_lower == tspec.SWtype && (*it_iacd)->encoding_lower == tspec.ENCtype)
        {
            tspec.SWtype = (*it_iacd)->swtype_upper;
            tspec.ENCtype = (*it_iacd)->encoding_upper;
            // Bandwidth adaptation for lower->upper layer
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            }
            return 0;
        }
        
        //crossing from upper layer to lower layer
        if ((*it_iacd)->swtype_upper == tspec.SWtype && (*it_iacd)->encoding_upper == tspec.ENCtype)
        {
            tspec.SWtype = (*it_iacd)->swtype_lower;
            tspec.ENCtype = (*it_iacd)->encoding_lower;
            // Bandwidth adaptation for upper->lower layer
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_FSC:
                //tspec.Bandwidth = (*it_iacd)->max_lsp_bw[7]; //?
                break;
            }
            return 0;
        }
    }


    // Check implicit adaptation
    if (this->reverse_link && this->reverse_link->link)
    {
        ISCD* iscd_adapted = NULL;         
        if (this->link->iscds.size() == 1 && this->reverse_link->link->iscds.size() == 1&& this->link->iscds.front()->swtype != this->reverse_link->link->iscds.front()->swtype)
            iscd_adapted = this->reverse_link->link->iscds.front();
        else if (this->link->iscds.size() *this->reverse_link->link->iscds.size() > 1)
        {
            list<ISCD*>::iterator iter_iscd = this->reverse_link->link->iscds.begin();
            for (; iter_iscd != this->reverse_link->link->iscds.end(); iter_iscd++)
                if ((*iter_iscd)->swtype != tspec.SWtype)
                {
                    iscd_adapted = *iter_iscd;
                    break;
                }
        }
        
        if (iscd_adapted)
        {
            tspec.SWtype = iscd_adapted->swtype;
            tspec.ENCtype = iscd_adapted->encoding;

            //Bandwidth adaptation
            // @@@@ To be enhanced in the future
            switch (tspec.SWtype)
            {
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC1:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC2:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC3:
            case LINK_IFSWCAP_SUBTLV_SWCAP_PSC4:
            case LINK_IFSWCAP_SUBTLV_SWCAP_L2SC:
                //bandwidth constraint unchanged
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_TDM:
                //@@@@ bandwidth constraint unchanged
                /*
                if (!float_equal(pcen_link->reverse_link->link->iscds.front()->min_lsp_bw, 0))
                    tspec.Bandwidth = ceilf(tspec.Bandwidth /pcen_link->reverse_link->link->iscds.front()->min_lsp_bw)
                    * pcen_link->reverse_link->link->iscds.front()->min_lsp_bw;
                */
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_LSC:
                //@@@@ bandwidth constraint unchanged
                //tspec.Bandwidth = pcen_link->reverse_link->link->iscds.front()->max_lsp_bw[7];//?
                break;
            case LINK_IFSWCAP_SUBTLV_SWCAP_FSC:
                //@@@@ bandwidth constraint unchanged
                //tspec.Bandwidth = pcen_link->reverse_link->link->iscds.front()->max_lsp_bw[7];//?
                break;
            }
            return 0;
        }
    }

    return -1;
}

PCENNode* PCENLink::search_PCENNode(vector<PCENNode*>& routers, int NodeId) 
{
    for (unsigned int i=0; i<routers.size(); i++) 
    {
        if (routers[i]->ref_num==NodeId)
            return routers[i];
    }
    return NULL;
}


PCENLink::PCENLink(int id)
{
	linkID=id;
	lcl_end=NULL;
	rmt_end=NULL;
	link = NULL;
}

PCENLink::PCENLink(int id, int localNodeId, int remoteNodeId, vector<PCENNode*>& routers) 
{
    linkID=id;
    lcl_end=search_PCENNode(routers, localNodeId);
    lcl_end->out_links.push_back(this);
    rmt_end=search_PCENNode(routers, remoteNodeId);
    rmt_end->in_links.push_back(this);
    Init();
}

PCENLink::~PCENLink()
{
    if (link_self_allocated)
        delete link;
}

void PCENLink::FilterLink() 
{
    this->lflg.lfg.filteroff=1;
}

void PCENLink::RestoreLinkFilter() 
{
    this->lflg.lfg.filteroff=0;
}

void PCENLink::MaskLink() 
{
    this->lflg.lfg.maskoff=1;
}
void PCENLink::RestoreLinkMask() 
{
    this->lflg.lfg.maskoff=0;
}

double PCENLink::PCENmetric ()
{
    if (!link)
        return PCEN_INFINITE_COST;

    return (double)link->metric;
}

void PCENLink::SetPCENmetric (u_int32_t x)
{
    if (!link)
        return;

    
    link->metric = x;
}

void PCENLink::SetPCENmetric (double d)
{
    if (!link)
        return;

    link->metric = (u_int32_t)d;
}

void PCEN::AllocateGraph(int size)
{
    if (gGraph && size > gSize)
    {
        FreeGraph();        
    }

    if (!gGraph)
    {
        if (size > gSize)
            gSize = size;
        gGraph = new float[gSize*gSize];
    }

    for (int i = 0; i < gSize; i++)
        for (int j = 0; j < gSize; j++)
            gGraph[i*gSize + j] = 1000000;
}

void PCEN::FreeGraph()
{
    if (!gGraph)
        return;

    delete []gGraph;

    gGraph = NULL;    
    gSize = 0;
}

void PCEN::ClearFilters()
{ 
    while (filters.size() > 0)
    {
        delete (Filter*)filters.front();
        filters.pop_front();
    }
}

PCENLink* PCEN::search_PCENLink(int lcl_end,int rmt_end)
{
	for(unsigned int i=0; i<links.size(); i++)
	{
		if(links[i]->lcl_end->ref_num == lcl_end && links[i]->rmt_end->ref_num == rmt_end)
			return links[i];
	}
	return NULL;
}

// Apply initial constraints, e.g. Routing Mode (loose/strict-only or partial..) and Domain Restriction...
int PCEN::VerifyRequest()
{
    //Check source and destination
    Prefix index1, index2;
    index1.length = index2.length = 64;
    prefix_word(index1, 0) = source.s_addr;
    prefix_word(index1, 1) = source.s_addr;
    prefix_word(index2, 0) = destination.s_addr;
    prefix_word(index2, 1) = destination.s_addr;

    //options  = options & (~LSP_OPT_STRICT_PREFERRED);
    //options  = options | LSP_OPT_LOOSE_ONLY;
    
    if (!(options & LSP_OPT_STRICT)) // && !(options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_GLO_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_GLO_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else if ((options & LSP_OPT_STRICT) && (options & LSP_OPT_PREFERRED))
    {
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index1) == NULL)
        {
            LOGF("Unknown Source RouterID [%X]\n", source.s_addr);
            return ERR_PCEN_NO_SRC;
        }
        if (RDB.LookupByPrefix(RTYPE_LOC_RTID, &index2) == NULL && RDB.LookupByPrefix(RTYPE_GLO_RTID, &index2) == NULL)
        {
            LOGF("Unknown Destination RouterID [%X]\n", destination.s_addr);
            return ERR_PCEN_NO_DEST;
        }
    }
    else
    {
        LOGF("Incorrection Option Setting [%x]\n", options);
        return ERR_PCEN_INVALID_REQ;
    }

    if (is_e2e_tagged_vlan && ((vtag != ANY_VTAG && vtag >= MAX_VLAN_NUM) || vtag == 0))
    {
        LOGF("PCEN_MRN::PerformComputation() failed for source[%X]-destination[%X] due to invalid VTAG %d\n", source.s_addr, destination.s_addr, vtag);
        return ERR_PCEN_INVALID_REQ;
    }

    // We want all routers
    Prefix *pPrefix = new (Prefix);
    memset(pPrefix, 0 , sizeof(Prefix));
    prefix_list.push_back(pPrefix);

    // Add the switching capability filter
    if (!(options & LSP_OPT_MRN))
    {
        SwitchingTypeFilter * swTypeFilter = new SwitchingTypeFilter(switching_type_egress);
        filters.push_back(swTypeFilter);
    }
    
    return 0;
}

void PCEN::BuildNodeVector(ResourceType type, Prefix *prefix)
{
    RadixTree<Resource>* tree = RDB.Tree(type);
    RadixNode<Resource>* node = tree->LookupNode(prefix);
    
    if (!node)
        return;

    Prefix index = node->Index();

    PCENNode * pcen_node;
    do 
    {
        if (node->Data())
        {
            int i;
            for (i = 0; i < routers.size(); i++)
            {
                if (routers[i]->router &&  routers[i]->router->id == (node->Data())->id)
                    break;
            }
            if (i < routers.size())
                continue;
            pcen_node = new PCENNode((RouterId*)node->Data());
            routers.push_back(pcen_node);
        }
    } while (node = tree->LookupNextMatchedNode(node, &index));
}

void PCEN::BuildLinkVector(ResourceType type)
{
    RadixTree<Resource>* tree = RDB.Tree(type);
    RadixNode<Resource>* node;
    PCENNode* pcen_node_lcl, *pcen_node_rmt;
    PCENLink* pcen_link;
    Prefix index;
    int ref_num = 0;

    vector<PCENNode*>::iterator iter_outer, iter_inner;
    for (iter_outer = routers.begin(); iter_outer != routers.end(); iter_outer++)
    {
        pcen_node_lcl = *iter_outer;
        index = pcen_node_lcl->router->Index();
        index.length = 32;
        node = tree->LookupMatchedNode(&index);
        if (!node)
            continue;
        do 
        {
            if (node->Data())
            {
                pcen_link = NULL; 
                // removing stub links
                for (iter_inner = routers.begin(); iter_inner != routers.end(); iter_inner++)
                {
                    pcen_node_rmt = *iter_inner;
                    if (pcen_node_rmt->router->id == ((Link*)node->Data())->id)
                    {
                        pcen_link = new PCENLink((Link *)node->Data());
                        break;
                    }
                }
                if (pcen_link)
                {
                    assert (pcen_node_lcl);
                    assert (pcen_node_rmt);
                    pcen_link->lcl_end = pcen_node_lcl;
                    pcen_link->rmt_end = pcen_node_rmt;
                    pcen_node_lcl->out_links.push_back(pcen_link);
                    pcen_node_rmt->in_links.push_back(pcen_link);

                    assert(pcen_link->link);
                    // handling quried/existing link state deltas for the same ucid/seqnum ...
                    pcen_link->link->removeDeltaByOwner(ucid, seqnum);
                    // handling expired link state deltas. (refer to class Link and class LSPHandler.)
                    pcen_link->link->deleteExpiredDeltas();

                    pcen_link->linkID = ref_num++;
                    links.push_back(pcen_link);
                 }
            }
        } while (node = tree->LookupNextMatchedNode(node, &index));
    }
}


////////////////Handling Overlapping Links//////////////////

//Add a transit node into the routers vector and return its pointer.
//ref_num has been assigned a valid value. router == NULL, which indicates
//the transit status.
PCENNode* PCEN::NewTransitNode (vector<PCENNode*>& routers)
{
    PCENNode * transitNode = new PCENNode;
    assert (routers.size() > 0);
    PCENNode *tailNode = routers[routers.size() -1];
    assert(tailNode);
    transitNode->ref_num = tailNode->ref_num + 1;
    routers.push_back(transitNode);
    return transitNode;
}

//Add a transit link into the links vector and return its pointer.
// link == NULL, which indicates the transit status.
PCENLink* PCEN::NewTransitLink (vector<PCENLink*>& links)
{
    PCENLink * transitLink = new PCENLink;
    PCENLink *tailLink = links[links.size() -1];
    assert(tailLink);
    transitLink->linkID = tailLink->linkID + 1;
    links.push_back(transitLink);
    return transitLink;
}

void PCEN::SplitLink (vector<PCENNode*>& routers, vector<PCENLink*>& links, PCENLink * teLink)
{
    assert(teLink); 
    assert(teLink->lcl_end);
    assert(teLink->rmt_end);

    // disconnect the original link from the remote node
    PCENLink* inLink;
    list<PCENLink*>::iterator iter;
    for (iter = teLink->rmt_end->in_links.begin(); iter != teLink->rmt_end->in_links.end(); iter++)
    {
        inLink = *iter;
        if (inLink->link == teLink->link)
        {
            teLink->rmt_end->out_links.erase(iter);
            break;
        }
     }
    assert (iter != teLink->rmt_end->in_links.end());

    PCENNode* transitNode = NewTransitNode(routers);
    PCENLink* transitLink = NewTransitLink(links);
    //let the transit node split the original link
    transitNode->in_links.push_back(teLink);
    transitNode->out_links.push_back(transitLink);
    //let the transit link bridge the transit node to the remote node
    transitLink->lcl_end = transitNode;
    transitLink->rmt_end = teLink->rmt_end;
    //let the orginal link point to the transit node
    teLink->rmt_end = transitNode;
}

int PCEN::HandleOverlappingLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links)
{
    int count_overlaps = 0;
    vector<PCENNode*> foundRouters;
    
    PCENNode* teNode;
    for (int i = 0; i < routers.size(); i++)
    {
        teNode = routers[i];

        foundRouters.clear();
        PCENLink* teLink;
        list<PCENLink*>::iterator iter;
        for (iter = teNode->out_links.begin(); iter != teNode->out_links.end(); iter++)
        {
            teLink = *iter;
            bool found = false;
            //check if there has been another link with the same remote end (node)
            for (int j = 0; j < foundRouters.size(); j++)
            {
                // if found such link has existed, split the current link
                if (foundRouters[j]->router == teLink->rmt_end->router)
                {
                    SplitLink(routers, links, teLink);
                    found = true;
                    count_overlaps++;
                    break;
                }
            }

            if (!found)
                foundRouters.push_back(teLink->rmt_end);
        }
    }

    return count_overlaps;
}

////////////////////////////////////////////////////////

void PCEN::BuildGraph(vector<PCENNode*>& routers, vector<PCENLink*>& links)
{
    originalGraphSize = routers.size();
    int numOverlapingLinks = HandleOverlappingLinks(routers, links);
    int numRouters = routers.size();
    int numLinks = links.size();

    //LOGF("Handled/splited a total of %d overlapping links\n", numOverlapingLinks);

    AllocateGraph(numRouters);

    int  i;
    PCENLink *pcen_link;
    for (i =0 ; i < numRouters; i++)
    {
        routers[i]->ref_num = i; 
    }
    float verySmallCost =  0.000001;
    for (i = 0; i < numLinks; i++)
    {
        pcen_link = links[i];
        gGraph[pcen_link->lcl_end->ref_num * numRouters + pcen_link->rmt_end->ref_num] = 
           ( pcen_link->link == NULL ? verySmallCost : ( pcen_link->link->metric == 0 ? 1 : pcen_link->link->metric ) );
    }
}


int PCEN::BuildTopology()
{
    // Buidling the initial router vector
    list<Prefix*>::iterator iter;
    for (iter = prefix_list.begin(); iter != prefix_list.end(); iter++)
    {
        if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
            BuildNodeVector(RTYPE_LOC_RTID, *iter);
        else
            BuildNodeVector(RTYPE_GLO_RTID, *iter);
    }
    if (routers.size() < 2 && (options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        return ERR_PCEN_DATA_CORRUPT;

    // Buidling the initial link vector
    if ((options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        BuildLinkVector(RTYPE_LOC_PHY_LNK);
    else
        BuildLinkVector(RTYPE_GLO_ABS_LNK);
    if (links.size() < 1 && (options & LSP_OPT_STRICT) && !(options & LSP_OPT_PREFERRED))
        return ERR_PCEN_DATA_CORRUPT;

    if (((options & LSP_OPT_STRICT) && (options & LSP_OPT_PREFERRED)))
    {
        for (iter = prefix_list.begin(); iter != prefix_list.end(); iter++)
            BuildNodeVector(RTYPE_LOC_RTID, *iter);
        BuildLinkVector(RTYPE_LOC_PHY_LNK);        
    }

    // Applying filters in list<filters>   
    //      Modifying the router vector   
    //      Modifying the link vector
    list<Filter*>::iterator filter_iter;
    Filter* aFilter;
    for (filter_iter = filters.begin(); filter_iter != filters.end(); filter_iter++)
    {
        aFilter = *filter_iter;
        assert(aFilter);
        aFilter->Process(routers, links);
    }

    // Build graph matrix
    //      Obosolete!!
    if (!(options & LSP_OPT_MRN))
    {
        BuildGraph(routers, links);
    }
    return 0;
}
    
void PCEN::DJhop(float *graphhop, int node_number, int source, int* path, int* hop, float* cost)
{
	int *node_inset = new int[node_number]; 	
	int i,j,k,m;
	float infcost = 1000000;
	float shortcost;

	for (i=0; i<node_number; i++)
		for (j=0; j<node_number; j++)
              {      
			path[i* node_number+j]=-1;
                     if (graphhop[i* node_number+j] == -1)
                        graphhop[i* node_number+j] = 1000000;
              }
	
	path[source*node_number+0]=source;
	

	for (i=0; i<node_number; i++) {
		node_inset[i]=0;
		hop[i]=node_number+1;
		cost[i]=infcost;
		if (graphhop[source*node_number+i]!=infcost) {
			hop[i]=2;
			cost[i]=graphhop[source*node_number+i];
		} 
		if (hop[i]<node_number+1) {
			path[i*node_number+0]=source;
			path[i*node_number+1]=i;
		} else {
			path[i*node_number+0]=-1;
		}
	}
	
	node_inset[source]=1;
	
	for (i=0; i<node_number; i++) {
		shortcost=infcost;
		k=-1;
		for (j=0; j<node_number; j++) {
			if ((!node_inset[j]) && (cost[j]<shortcost)) {
				shortcost=cost[j];
				k=j;
			}
		}
		if (k!=-1) {
			node_inset[k]=1;
			for (j=0; j<node_number; j++) {
				if ((!node_inset[j]) && (graphhop[k*node_number+j]!=infcost)) {
					if (cost[j]>cost[k]+graphhop[k*node_number+j]) {
						cost[j]=cost[k]+graphhop[k*node_number+j];
						for (m=0; m<node_number; m++)
							path[j*node_number+m]=path[k*node_number+m];
						hop[j]=hop[k]+1;
						path[j*node_number+hop[j]-1]=j;
					}
						
				}
			}
		}
	}

    delete []node_inset;
}

int PCEN::PerformComputation()
{
    // SPF computation
    //      Currently we do not have the time scheduling function.
    int node_number = routers.size();
    int src_index = -1;
    int ret = 0;
    int i, j, k;

    for (i = 0; i < node_number; i++)
    {
        if (routers[i]->router == NULL)
            continue;
        if (routers[i]->router->id == source.s_addr)
        {
            src_index = routers[i]->ref_num;
            break;
        }
    }
    if (src_index == -1) 
        return ERR_PCEN_NO_SRC;
    
    int dest_index = -1;
    for (int i = 0; i < node_number; i++)
    {
        if (routers[i]->router == NULL)
            continue;
        if (routers[i]->router->id == destination.s_addr)
        {
            dest_index = routers[i]->ref_num;
            break;
        }
    }
    if (dest_index == -1) 
        return ERR_PCEN_NO_DEST;

    
    int *hop = new int[node_number];
    float *cost = new float[node_number];
    int *path = new int[node_number*node_number];

    DJhop(gGraph, node_number, src_index, path, hop, cost);

    if (path[dest_index* node_number + 0]==-1) 
    {
        ret = ERR_PCEN_NO_ROUTE;
        goto _out;
    }

    assert(ero.size() == 0);

    for (i=0; i<hop[dest_index]-1; )  {
        j=path[dest_index * node_number + i];
        i++;
        k=path[dest_index * node_number + i];
        while (!routers[k]->router)
        {
            i++;
            k=path[dest_index * node_number + i];
        }
        ero_subobj local_if_ip, remote_if_ip;
        memset(&local_if_ip, 0, sizeof(ero_subobj));
        memset(&remote_if_ip, 0, sizeof(ero_subobj));
        if (ret = GetLinkEndsByIndex(routers, links, j, k, &local_if_ip.addr, &remote_if_ip.addr))
            goto _out;

        Prefix prefix; 
        prefix.length = 128;
        prefix_word(prefix, 0) = routers[j]->router->id;
        prefix_word(prefix, 1) = routers[k]->router->id;
        prefix_word(prefix, 2) = local_if_ip.addr.s_addr;
        prefix_word(prefix, 3) = remote_if_ip.addr.s_addr;
        if (RDB.LookupByPrefix(RTYPE_LOC_PHY_LNK, &prefix) != NULL)
            local_if_ip.hop_type = remote_if_ip.hop_type = ERO_TYPE_STRICT_HOP;
        else 
            local_if_ip.hop_type = remote_if_ip.hop_type = ERO_TYPE_LOOSE_HOP;
        
        local_if_ip.prefix_len = 32;
        if (local_if_ip.addr.s_addr != 0)
            ero.push_back(local_if_ip);
        remote_if_ip.prefix_len = 32;
        if (remote_if_ip.addr.s_addr != 0)
            ero.push_back(remote_if_ip);
    }    

_out:
    delete []hop;
    delete []cost;
    delete []path;
    return ret;
}

void PCEN::Run()
{
    int ret;
    Event::Run();

    if (is_e2e_tagged_vlan)
    {
        ReplyErrorCode(ERR_PCEN_INVALID_REQ);
        LOG("PCEN::Run() : should have called PCEN_MRN::Run() to handle E2E Tagged VLAN request!" << endl);
        return;
    }

    if ((ret = VerifyRequest()) != 0)
    {
        LOGF("PCEN::VerifyRequest() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = BuildTopology()) != 0)
    {
        LOGF("PCEN::BuildTopology() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    if ((ret = PerformComputation()) != 0)
    {
        LOGF("PCEN::PerformComputation() failed for source[%X]-destination[%X]\n", source.s_addr, destination.s_addr);
        ReplyErrorCode(ret);
        return;
    }

    ReplyERO();
}

void PCEN::ReplyErrorCode (u_int32_t code)
{
    assert (api_writer);

    LOGF("Replying ErrorCode %d for LSP request (ucid=0x%x, seqnum=0x%x): Src 0x%x Dest 0x%x Bandwidth %g\n",  code,
        ucid, seqnum, source, destination, bandwidth_ingress);

    te_tlv_header * tlv = (te_tlv_header *)(new char[8 + sizeof(narb_lsp_lspb_id_tlv)]);
    tlv->length = htons(8);
    tlv->type = htons(TLV_TYPE_NARB_ERROR_CODE);
    *(u_int32_t*)((char*)tlv + TLV_HDR_SIZE) = htonl(code);

    if (lspb_id != 0)
    {
        narb_lsp_lspb_id_tlv* lspb_id_tlv = (narb_lsp_lspb_id_tlv*)((char*)tlv + 8);
        lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspb_id_tlv->length = htons(sizeof(narb_lsp_lspb_id_tlv));
        lspb_id_tlv->lspb_id = lspb_id;
    }

    api_msg* msg = api_msg_new(MSG_LSP, ACT_ERROR, tlv, ucid, seqnum, 8 + sizeof(narb_lsp_lspb_id_tlv), vtag);
    delete [] (char*)tlv;
    api_writer->PostMessage(msg);
    return;
}

api_msg* PCEN::NewEROReplyMessage ()
{
    char body[2048];

    list<ero_subobj>::iterator iter; 

    //$$$$ Special handling for HOP BACK ...
    if (hop_back != 0)
    {
        for (iter = ero.begin(); iter != ero.end(); iter++)
        {
            if ((*iter).addr.s_addr == hop_back && iter != ero.begin())
            {
                iter = ero.erase(ero.begin(), iter);
                break;
            }
        }
        if (iter == ero.end()) // add a back_hop to the beginning
        {
            ero_subobj subobj_hopback = *(ero.begin());
            if (subobj_hopback.hop_type == ERO_TYPE_STRICT_HOP &&
                (subobj_hopback.if_id >> 16) != LOCAL_ID_TYPE_SUBNET_UNI_SRC && 
                (subobj_hopback.if_id >> 16) != LOCAL_ID_TYPE_SUBNET_UNI_DEST )
            {
                subobj_hopback.addr.s_addr = hop_back;
                ero.push_front(subobj_hopback);
            }
        }
    }

    // coping ero to message buffer
    int bodylen = TLV_HDR_SIZE + sizeof(ero_subobj)*ero.size();
    te_tlv_header * tlv = (te_tlv_header*)body;
    tlv->length = htons(sizeof(ero_subobj)*ero.size());
    tlv->type = htons(TLV_TYPE_NARB_ERO);

    bool is_ero_all_strict = true;
    int i =0;
    ero_subobj* ero_hop = (ero_subobj*)((char*)tlv + TLV_HDR_SIZE);
    for (iter = ero.begin(); iter != ero.end(); iter++, i++)
    {
        *(ero_hop+i) = (*iter);
        if ((*iter).hop_type == ERO_TYPE_LOOSE_HOP)
            is_ero_all_strict = false;
    }

    if (vtag_mask && (options & LSP_OPT_REQ_ALL_VTAGS)) // no vtag_mask holding for last domain (all_strict)
    {
        tlv = (te_tlv_header*)(body + bodylen);
        memcpy(tlv, vtag_mask, sizeof(narb_lsp_vtagmask_tlv));
        bodylen += sizeof(narb_lsp_vtagmask_tlv);
    }

    if (lspb_id != 0)
    {
        narb_lsp_lspb_id_tlv* lspb_id_tlv = (narb_lsp_lspb_id_tlv*)(body + bodylen);
        lspb_id_tlv->type = htons(TLV_TYPE_NARB_LSPB_ID);
        lspb_id_tlv->length = htons(sizeof(narb_lsp_lspb_id_tlv) - TLV_HDR_SIZE);
        lspb_id_tlv->lspb_id = lspb_id;
        bodylen += sizeof(narb_lsp_lspb_id_tlv);
    }
    // placeholder for wave_mask ...

    if (subnet_ero.size() > 0 && (options & LSP_OPT_SUBNET_DTL))
    {
        list<dtl_hop> subnet_dtl;
        TranslateSubnetEROIntoDTL(subnet_ero, subnet_dtl);
        if (subnet_dtl.size() > 0)
        {
            narb_lsp_subnet_dtl_tlv* subnet_dtl_tlv = (narb_lsp_subnet_dtl_tlv*)(body + bodylen);
            subnet_dtl_tlv->type = htons(TLV_TYPE_NARB_SUBNET_DTL);
            subnet_dtl_tlv->length = htons(sizeof(dtl_hop)*subnet_dtl.size());
            i = 0;
            while (subnet_dtl.size() > 0)
            {
                subnet_dtl_tlv->hops[i] = subnet_dtl.front();
                subnet_dtl.pop_front();
                i++;
            }
            bodylen += ntohs(subnet_dtl_tlv->length)+TLV_HDR_SIZE;
        }
        else
        {
            LOG("#### Warning #### User supplied subnet ERO cannot be translated to DTL!"<<endl);
        }
    }

    if (subnet_ero.size() > 0 && (options & LSP_OPT_SUBNET_ERO))
    {
        narb_lsp_subnet_ero_tlv* subnet_ero_tlv = (narb_lsp_subnet_ero_tlv*)(body + bodylen);
        subnet_ero_tlv->type = htons(TLV_TYPE_NARB_SUBNET_ERO);
        subnet_ero_tlv->length = htons(sizeof(ero_subobj)*subnet_ero.size());
        i = 0;
        while (subnet_ero.size() > 0)
        {
            subnet_ero_tlv->subobjects[i] = subnet_ero.front();
            subnet_ero.pop_front();
            i++;
        }
        bodylen += ntohs(subnet_ero_tlv->length)+TLV_HDR_SIZE;
    }

    api_msg* msg = api_msg_new(MSG_LSP, ACT_ACKDATA, body, ucid, seqnum, bodylen, vtag);
    return msg;
}

void PCEN::AddLinkToEROTrack(list<ero_subobj>& ero_track,  PCENLink* pcen_link)
{
    ero_subobj subobj1, subobj2;
    memset(&subobj1, 0, sizeof(ero_subobj));
    subobj1.prefix_len = 32;
    subobj1.addr.s_addr = pcen_link->link->lclIfAddr;
    if (subobj1.addr.s_addr == 0)
    {
        subobj1.addr.s_addr = pcen_link->link->advRtId;
        subobj1.if_id = pcen_link->link->lclRmtId[0];
    }
    memset(&subobj2, 0, sizeof(ero_subobj));
    subobj2.prefix_len = 32;
    subobj2.addr.s_addr = pcen_link->link->rmtIfAddr;
    if (subobj2.addr.s_addr == 0)
    {
        subobj2.addr.s_addr = pcen_link->link->id;
        subobj2.if_id = pcen_link->link->lclRmtId[1];
    }

    if (pcen_link->link->type == RTYPE_LOC_PHY_LNK)
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_STRICT_HOP;
    else 
        subobj1.hop_type = subobj2.hop_type = ERO_TYPE_LOOSE_HOP;

    subobj1.sw_type = subobj2.sw_type = pcen_link->lcl_end->tspec.SWtype;
    subobj1.encoding = subobj2.encoding = pcen_link->lcl_end->tspec.ENCtype;
    subobj1.bandwidth= subobj2.bandwidth = pcen_link->lcl_end->tspec.Bandwidth;

    //The second subobj in a pair takes remote-end TE parameters.
    subobj2.sw_type = pcen_link->rmt_end->tspec.SWtype;
    subobj2.encoding = pcen_link->rmt_end->tspec.ENCtype;
    subobj2.bandwidth = pcen_link->rmt_end->tspec.Bandwidth;
    
    //E2E VLAN related
    if (is_e2e_tagged_vlan && subobj1.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->link)
    {
        list<ISCD*>::iterator iter_iscd = pcen_link->link->iscds.begin();
        for (; iter_iscd != pcen_link->link->iscds.end(); iter_iscd++)
        {
            if ((*iter_iscd)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && (ntohs((*iter_iscd)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) != 0)
            {
                subobj1.l2sc_vlantag = vtag; //*(u_int16_t *)subobj1.pad
                break;
            }
        }
    } 
    if (is_e2e_tagged_vlan && subobj2.sw_type == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && pcen_link->reverse_link && pcen_link->reverse_link->link)
    {
        list<ISCD*>::iterator iter_iscd = pcen_link->reverse_link->link->iscds.begin();
        for (; iter_iscd != pcen_link->reverse_link->link->iscds.end(); iter_iscd++)
        {
            if ((*iter_iscd)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && (ntohs((*iter_iscd)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) != 0)
            {
                subobj2.l2sc_vlantag = vtag; //*(u_int16_t *)subobj2.pad
                break;
            }
        }
    } 

    //Subnet UNI related
    if (SystemConfig::should_incorporate_ciena_subnet && pcen_link->link)
    {
        u_int8_t ts = 0;  //one-based
        list<ISCD*>::iterator iter_iscd = pcen_link->link->iscds.begin();
        for (; iter_iscd != pcen_link->link->iscds.end(); iter_iscd++)
        {
            if ((*iter_iscd)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && (ntohs((*iter_iscd)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0)
                break;
        }
        if (iter_iscd != pcen_link->link->iscds.end())
        {
            u_int8_t ts_start = 1, ts_num = 0;
            if ((dest_lcl_id & 0xff) > 0 &&  (dest_lcl_id & 0xff) <= MAX_TIMESLOTS_NUM && ((dest_lcl_id >> 8) & 0xff) == (*iter_iscd)->subnet_uni_info.subnet_uni_id)
                ts_start = (dest_lcl_id & 0xff);
            for (ts = ts_start; ts <= MAX_TIMESLOTS_NUM; ts++)
            {
                if (HAS_TIMESLOT((*iter_iscd)->subnet_uni_info.timeslot_bitmask, ts))
                {
                    ts_num++;
                    //default case: timeslots handling in non-contiguous mode --> get the first available timeslot
                    if (((*iter_iscd)->subnet_uni_info.version & IFSWCAP_SPECIFIC_SUBNET_CONTIGUOUS) == 0)
                        break;
                }
                else
                {
                    ts_num = 0;
                }
                if (ts_num >= SystemConfig::MapBandwidthToNumberOfTimeslots(pcen_link->rmt_end->tspec.Bandwidth))
                {
                    break;
                }
            }
            if (ts > MAX_TIMESLOTS_NUM) 
            {
                ts = 0;
            }
            else 
            {
                ts = ts-ts_num+1;
            }
            subobj1.if_id = htonl( (LOCAL_ID_TYPE_SUBNET_UNI_DEST << 16) |((*iter_iscd)->subnet_uni_info.subnet_uni_id << 8) | ts );
            subobj1.l2sc_vlantag = 0;
        }
    }

    if ( SystemConfig::should_incorporate_ciena_subnet && pcen_link->reverse_link && pcen_link->reverse_link->link)
    {
        list<ISCD*>::iterator iter_iscd = pcen_link->reverse_link->link->iscds.begin();
        for (; iter_iscd != pcen_link->reverse_link->link->iscds.end(); iter_iscd++)
        {
            if ((*iter_iscd)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM && (ntohs((*iter_iscd)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0)
                break;
        }
        if (iter_iscd != pcen_link->reverse_link->link->iscds.end())
        {
            u_int8_t ts = 0, ts_start = 1, ts_num = 0;
            if ((src_lcl_id & 0xff) > 0 &&  (src_lcl_id & 0xff) <= MAX_TIMESLOTS_NUM && ((src_lcl_id >> 8) & 0xff) == (*iter_iscd)->subnet_uni_info.subnet_uni_id)
                ts_start = (src_lcl_id & 0xff);
            for (ts = ts_start; ts <= MAX_TIMESLOTS_NUM; ts++)
            {
                if (HAS_TIMESLOT((*iter_iscd)->subnet_uni_info.timeslot_bitmask, ts))
                {
                    ts_num++;
					//default case: timeslots handling in non-contiguous mode --> get the first available timeslot
					if (((*iter_iscd)->subnet_uni_info.version & IFSWCAP_SPECIFIC_SUBNET_CONTIGUOUS) == 0)
						break;
                }
                else
                {
                    ts_num = 0;
                }
                if (ts_num >= SystemConfig::MapBandwidthToNumberOfTimeslots(pcen_link->lcl_end->tspec.Bandwidth))
                {
                    break;
                }
            }
            if (ts > MAX_TIMESLOTS_NUM) 
            {
                ts = 0;
            }
            else 
            {
                ts = ts-ts_num+1;
            }
            subobj2.if_id = htonl( (LOCAL_ID_TYPE_SUBNET_UNI_SRC << 16) |((*iter_iscd)->subnet_uni_info.subnet_uni_id <<8) | ts );
            subobj2.l2sc_vlantag = 0;
        }
    }

    ero_track.push_back(subobj1);
    ero_track.push_back(subobj2);
}

void PCEN::GetPathERO(list<PCENLink*>& path)
{
    ero.clear();
    PCENLink * pcen_link;

    list<PCENLink*>::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        pcen_link = *it;
        AddLinkToEROTrack(ero, pcen_link);
    }
}

void PCEN::ReplyERO ()
{
    assert (api_writer);
    LOGF("Replying ERO for LSP request (ucid=0x%x, seqnum=0x%x): Src 0x%x Dest 0x%x Bandwidth %g\n",  
        ucid, seqnum, source, destination, bandwidth_ingress);

    api_msg* msg = NewEROReplyMessage();
    if (msg != NULL)
        api_writer->PostMessage(msg);

    //holding the resources enqueried for a short period of time to avoid contention...
    if (options & LSP_OPT_QUERY_HOLD)
    {
        // $$$$ option 1 --> Holding all vtag_mask, which may block colliding requests
        //HoldLinkStatesUponQuery(is_ero_all_strict ? NULL : vtag_mask);
        // $$$$ option 2 --> Do not hold vtag_mask in Q-Conf process, reducing chance of collision blocking
        // $$$$ this will still hold the single vtag of pick, which will in turn be sent to next domain as suggested vtag
        HoldLinkStatesUponQuery(NULL);
    }
}

void PCEN::HoldLinkStatesUponQuery(narb_lsp_vtagmask_tlv* vtag_mask)
{
    narb_lsp_request_tlv lsp_req;
    lsp_req.type = ((MSG_LSP << 8) | ACT_QUERY);
    lsp_req.length = sizeof(narb_lsp_request_tlv) - TLV_HDR_SIZE;
    lsp_req.src.s_addr = this->source.s_addr;
    lsp_req.dest.s_addr = this->destination.s_addr;
    lsp_req.bandwidth = this->bandwidth_ingress;
    lsp_req.switching_type = this->switching_type_ingress;
    lsp_req.encoding_type = this->encoding_type_ingress;
    lsp_req.gpid = 0;

    bool is_bidir = ((this->options & LSP_OPT_BIDIRECTIONAL) != 0);
    if (ero.size() > 0)
        LSPHandler::UpdateLinkStatesByERO(lsp_req, this->ero, this->ucid, this->seqnum, is_bidir, this->vtag, this->src_lcl_id, this->dest_lcl_id, vtag_mask);

    // Holding links states for Subnet ERO (using the same lsp_req structure, where only req-type, source, destination and bandwidth matter)
    // Also note that query holding on subnet links will not go away with OSPF updates. The deltas will be wiped out by next path computation.
    if (subnet_ero.size() > 0)
        LSPHandler::UpdateLinkStatesByERO(lsp_req, this->subnet_ero, this->ucid, this->seqnum, is_bidir);
}
    
PCEN::~PCEN()
{
    //clean Router list
    vector<PCENNode*>::iterator node_iter;
    for (node_iter = routers.begin();  node_iter != routers.end(); node_iter++)
        if (*node_iter)
            delete *node_iter;

    //clean Link list
    vector<PCENLink*>::iterator link_iter;
    for (link_iter = links.begin();  link_iter != links.end(); link_iter++)
        if (*link_iter)
            delete *link_iter;

    //clean Filter list
    list<Filter*>::iterator filter_iter;
    for (filter_iter = filters.begin();  filter_iter != filters.end(); filter_iter++)
        if (*filter_iter)
            delete *filter_iter;

    //clean prefix list
    list<Prefix*>::iterator prefix_iter;
    for (prefix_iter = prefix_list.begin();  prefix_iter != prefix_list.end(); prefix_iter++)
        if (*prefix_iter)
            delete *prefix_iter;
    
    if (vtag_mask)
        delete vtag_mask;

    FreeGraph();
}

int PCEN::GetLinkEndsByIndex(vector<PCENNode*>& routers, vector<PCENLink*>& links, int j, int k, in_addr * head_ip, in_addr * tail_ip)
{
    assert(head_ip);
    assert(tail_ip);
    
    PCENNode *head = routers[j];
    PCENNode *tail = routers[k];
    PCENLink * pcen_link;
    list<PCENLink*>::iterator iter;
    for (iter = head->out_links.begin(); iter != head->out_links.end(); iter++)
    {
        pcen_link = *iter;
        if (pcen_link->rmt_end->ref_num != tail->ref_num)
            continue;
        if (pcen_link->link == NULL) //transit link
        {
            assert(tail->router);
            head_ip->s_addr = 0;
            list<PCENLink*>::iterator iter_inner = pcen_link->lcl_end->in_links.begin();
            pcen_link = *iter_inner;
            assert(pcen_link->link);
            tail_ip->s_addr = pcen_link->link->rmtIfAddr;
        }
        else if (tail->router == NULL) // the link before transit node/link 
        {
            assert(pcen_link->link);
            head_ip->s_addr = pcen_link->link->lclIfAddr;    
            tail_ip->s_addr = 0;
        }
        else // regular link
        {
            assert(pcen_link->link && tail->router);
            assert(pcen_link->link->advRtId == routers[j]->router->id && pcen_link->link->id == routers[k]->router->id);
            head_ip->s_addr = pcen_link->link->lclIfAddr;    
            tail_ip->s_addr = pcen_link->link->rmtIfAddr;
        }
        return 0;
    }
    return ERR_PCEN_DATA_CORRUPT;
}

PCENLink* PCEN::GetLinkByIp(vector<PCENLink*>& links, in_addr* head_ip, in_addr* tail_ip)
{
    assert(head_ip);
    assert(tail_ip);
    PCENLink* pcen_link;
    for (int i = 0; i < links.size(); i++)
    {
        pcen_link = links[i];
        if (!pcen_link->lcl_end || !pcen_link->rmt_end)
            continue;
        if (pcen_link->lcl_end->router == NULL || pcen_link->rmt_end->router == NULL)
            continue;
        if (pcen_link->lcl_end->router->id == head_ip->s_addr && pcen_link->rmt_end->router->id == tail_ip->s_addr)
            return pcen_link;
    }

    return NULL;
}

PCENNode* PCEN::GetNodeByIp(vector<PCENNode*>& routers, in_addr* ip)
{
    assert(ip);
    PCENNode* pcen_node;
    for (int i = 0; i < routers.size(); i++)
    {
        pcen_node = routers[i];
        if (pcen_node->router == NULL)
            continue;
        if (pcen_node->router->id == ip->s_addr)
        {
            return pcen_node;
        }
    }    
    return NULL;
}

u_int32_t PCEN::GetRouterDomainId(vector<PCENNode*>& routers, in_addr* ip)
{
    assert(ip);
    PCENNode* pcen_node = GetNodeByIp(routers, ip);
    if (!pcen_node)
        return DOMAIN_MASK_GLOBAL;

    return pcen_node->DomainId();
}

int PCEN::GetDomainBorderRouters(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, list<PCENNode*>&borderRouters)
{
    assert (borderRouters.size() == 0);

    PCENNode * pcen_node;
    for (int i = 0; i < routers.size(); i++)
    {
        pcen_node = routers[i];
        if (pcen_node->DomainId() == domainId)
        {
            PCENLink * pcen_link;
            list<PCENLink*>::iterator iter;
            for (iter = pcen_node->out_links.begin(); iter != pcen_node->out_links.end(); iter++)
            {
                pcen_link = *iter;
                assert(pcen_link->HeadDomainId() == domainId);
                if (pcen_link->TailDomainId() != domainId)
                {
                    borderRouters.push_back(pcen_node);
                    break;
                }
            }
        }
    }

    return borderRouters.size();
}

int PCEN::GetInterDomainLinks(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t headDomainId, u_int32_t tailDomainId, list<PCENLink*>&interDomainLinks)
{
    list<PCENNode*> headBorderNodes;

    if (GetDomainBorderRouters(routers, links, headDomainId, headBorderNodes) == 0)
        return 0;

    PCENNode * pcen_node;
    list<PCENNode*>::iterator iter_outer;
    for (iter_outer = headBorderNodes.begin(); iter_outer != headBorderNodes.end(); iter_outer++)
    {
        pcen_node = *iter_outer;
        PCENLink * pcen_link;
        list<PCENLink*>::iterator iter_inner;
        for (iter_inner = pcen_node->out_links.begin(); iter_inner != pcen_node->out_links.end(); iter_inner++)
        {
            pcen_link = *iter_inner;
            if (pcen_link->TailDomainId() != tailDomainId)
            {
                interDomainLinks.push_back(pcen_link);
                break;
            }
        }
    }

    return interDomainLinks.size();
}

bool PCEN::IsDomainBorderRouter(vector<PCENNode*>& routers, vector<PCENLink*>& links, u_int32_t domainId, u_int32_t routerId)
{
    list<PCENNode*> borderRouters;
    list<PCENNode*>::iterator iter;
    PCENNode * pcen_node;
    if (GetDomainBorderRouters(routers, links, domainId, borderRouters) > 0)
    {
        for (iter = borderRouters.begin(); iter != borderRouters.end(); iter++)
        {
            pcen_node = *iter;
            assert(pcen_node);
            if (pcen_node->router && pcen_node->router->id == routerId)
                return true;
        }
    }
    return false;
}

bool PCEN::IsLinkInDomain(PCENLink* pcenLink, u_int32_t domainId)
{
    assert(pcenLink);
    if (pcenLink->lcl_end == NULL || pcenLink->rmt_end == NULL)
        return false;

    if (pcenLink->lcl_end->DomainId() == domainId || pcenLink->rmt_end->DomainId() == domainId)
        return true;

    return false;
}

void PCEN::TranslateSubnetEROIntoDTL(list<ero_subobj>&ero_hops, list<dtl_hop>& dtl_hops)
{
    dtl_hops.clear();
    if (ero_hops.size() < 2 || ero_hops.size()%2 != 0)
    {
        //@@@@ LOG Error: subnet ero hops must be paired
        return;
    }
    
    RadixTree<Resource> *tree_rt = RDB.Tree(RTYPE_LOC_RTID);
    RadixTree<Resource> *tree_lk = RDB.Tree(RTYPE_LOC_PHY_LNK);
    list<ero_subobj>::iterator it;
    ero_subobj *subobj1, *subobj2;
    dtl_hop dhop;
    for (it = ero_hops.begin(); it != ero_hops.end(); it++)
    {
        subobj1 = &(*it);
        it++;
        subobj2 = &(*it);
        RadixNode<Resource> * node_lk = tree_lk->Root();
        if (node_lk && !node_lk->Data())
            node_lk = tree_rt->NextNodeHavingData(node_lk);
        while (node_lk)
        {
            link_info * link = (link_info *)node_lk->Data();
            if (link->LclIfAddr() == subobj1->addr.s_addr && link->RmtIfAddr() == subobj2->addr.s_addr)
            {
                link->AdvRtId();
                RadixNode<Resource> * node_rt = tree_rt->Root();
                if (node_rt && !node_rt->Data())
                    node_rt = tree_rt->NextNodeHavingData(node_rt);
                while (node_rt)
                {
                    router_id_info * rtid = (router_id_info *)node_rt->Data();
                    if (rtid->Id() == link->AdvRtId())
                    {
                        //Adding DTL hop
                        strncpy((char*)dhop.nodename, rtid->dtl_name, MAX_DTL_NODENAME_LEN);
                        dhop.linkid = link->dtl_id;
                        dtl_hops.push_back(dhop);
                        break;
                    }
                    node_rt = tree_rt->NextNodeHavingData(node_rt);
                }
                // if (node_rt != NULL)                     
                //@@@@ LOG Error: No DTL node found
                break;
            }        
            node_lk = tree_rt->NextNodeHavingData(node_lk);
        }        
        // if (node_lk != NULL)
        //@@@@ LOG Error: No DTL node found
    }
}

void PCEN::TranslateSubnetDTLIntoERO(list<dtl_hop>& dtl_hops, list<ero_subobj>& ero_hops)
{
    ero_hops.clear();
    if (dtl_hops.size() == 0)
    {
        return;
    }

    // form a subnet list
    list<RouterId*> subnet_rtids;
    list<Link*> subnet_links;
    RadixTree<Resource> *tree_rt = RDB.Tree(RTYPE_LOC_RTID);
    RadixNode<Resource> *node_rt = tree_rt->Root();
    if (node_rt && !node_rt->Data())
        node_rt = tree_rt->NextNodeHavingData(node_rt);
    while (node_rt)
    {
        RouterId * rtid = (RouterId *)node_rt->Data();
        if (SystemConfig::FindHomeVlsrByRouterId(rtid->Id()) != 0) // subnet node
        {
            subnet_rtids.push_back(rtid);
        }
        node_rt = tree_rt->NextNodeHavingData(node_rt);
    }
    RadixTree<Resource> *tree_lk = RDB.Tree(RTYPE_LOC_PHY_LNK);
    RadixNode<Resource> *node_lk = tree_lk->Root();
    if (node_lk && !node_lk->Data())
        node_lk = tree_lk->NextNodeHavingData(node_lk);
    while (node_lk)
    {
        Link * link = (Link *)node_lk->Data();
        if (SystemConfig::FindHomeVlsrByRouterId(link->AdvRtId()) != 0) // subnet node
        {
            subnet_links.push_back(link);
        }
        node_lk = tree_lk->NextNodeHavingData(node_lk);
    }
    
    list<dtl_hop>::iterator it;
    dtl_hop *dhop;
    for (it = dtl_hops.begin(); it != dtl_hops.end(); it++)
    {
        dhop = &(*it);

        //search the subbet rtid tree to match the dtl-node-name and get the RouterID
        u_int32_t link_lcl_rtid = 0;
        list<RouterId*>::iterator it_rtid = subnet_rtids.begin();
        for ( ; it_rtid!= subnet_rtids.end(); it_rtid++)
        {
            router_id_info * rtid = (router_id_info *)(*it_rtid);
            if (strncmp((char*)dhop->nodename, rtid->dtl_name, MAX_DTL_NODENAME_LEN) == 0)
            {
                link_lcl_rtid = rtid->Id();
                break;
            }
        }

        if (link_lcl_rtid == 0)
            continue;

        //search the subnet link tree to match both the AdvRtID and dtl-link-id
        list<Link*>::iterator it_link = subnet_links.begin();
        for ( ; it_link!= subnet_links.end(); it_link++)
        {
            link_info * link = (link_info *)(*it_link);
            if (link->AdvRtId() == link_lcl_rtid && dhop->linkid == link->dtl_id)
            {
                //create two subnet ERO hops using the link, add into ero_hops
                ero_subobj subobj1, subobj2;
                memset(&subobj1, 0, sizeof(ero_subobj));
                subobj1.prefix_len = 32;
                subobj1.addr.s_addr = link->lclIfAddr;
                if (subobj1.addr.s_addr == 0)
                {
                    subobj1.addr.s_addr = link->advRtId;
                    subobj1.if_id = link->lclRmtId[0];
                }
                memset(&subobj2, 0, sizeof(ero_subobj));
                subobj2.prefix_len = 32;
                subobj2.addr.s_addr = link->rmtIfAddr;
                if (subobj2.addr.s_addr == 0)
                {
                    subobj2.addr.s_addr = link->id;
                    subobj2.if_id = link->lclRmtId[1];
                }
                subobj1.hop_type = subobj2.hop_type = ERO_TYPE_STRICT_HOP;
                assert (link->iscds.size() > 0);

                //$$$$ Assumption: subnet link has single ISCD
                ISCD * iscd = link->iscds.front();
                subobj1.sw_type = subobj2.sw_type = iscd->swtype;
                subobj1.encoding = subobj2.encoding = iscd->encoding;
                subobj1.bandwidth= subobj2.bandwidth = iscd->max_lsp_bw[7];
                ero_hops.push_back(subobj1);
                ero_hops.push_back(subobj2);
            }
        }

    }
}

//$$$$ Ciena OTNX (only handling 10G backplanes (64 timeslots) for now)
void PCEN::TrimOTNXTimeslotsByBandwidth(ConstraintTagSet &timeslotset, float bandwidth)
{
    u_int32_t num_sts1s = SystemConfig::MapBandwidthToNumberOfTimeslots(bandwidth);
    u_int32_t num_opvcx = (num_sts1s+2)/3;
    u_int32_t i, j, ts, ts_start;

    if (num_opvcx > 16)  //requiring OTU2
    {
        for (ts = 1; ts <= 64; ts++)
            if (!timeslotset.HasTag(ts))
            {
                timeslotset.Clear();
                return;
            }
    }
    else //requiring single OTU1
    {
            for (i = 0; i < 4; i++) // 4 OTU1s
            {
                for (ts = i*16+1, ts_start = i*16+1; ts < (i+1)*16; ts++)
                {
                    if (timeslotset.HasTag(ts_start) && !timeslotset.HasTag(ts))
                    {
                        if (ts - ts_start < num_opvcx) //remove unsatisfying blocks
                        {
                            for (j = ts_start; j < ts; j++)
                                timeslotset.DeleteTag(j);
                        }
                        ts_start = ts+1;
                    }
                }
            }
    }
}

