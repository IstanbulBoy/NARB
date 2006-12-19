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
#include "types.hh"
#include "resource.hh"
#include "zebra_ospfclient.hh"
#include "narb_config.hh"
#include "dts.hh"

DomainInfo::~DomainInfo()
{
    int i;

    for (i = 0; i <  if_narb_table.size(); i++)
    {
        if_narb_info * item = if_narb_table[i];
        if (item)
            delete item;
    }

    for (i = 0; i <  services.size(); i++)
    {
        service_info * item = services[i];
        if (item)
            delete item;
    }

    for (i = 0; i <  svc_probes.size(); i++)
    {
        svc_probe * item = svc_probes[i];
        if (item)
            delete item;
    }
}
    
service_info * DomainInfo::ServiceLookupById(int id)
{
    service_info *nodedata;
    for (int i = 0; i < this->services.size(); i++)
    {
        nodedata = services[i];
        if (nodedata->service_id == id)
            return nodedata;
    }
  
  return NULL;
}

void DomainInfo::AddRouter(router_id_info * router)
{
    router->type = RTYPE_GLO_RTID;
    RDB.Update(router);
}

void DomainInfo::AddLink(link_info* link)
{
    link->type = RTYPE_GLO_ABS_LNK;
    RDB.Update(link);

}

router_id_info::router_id_info(u_int32_t domain_id, in_addr ip):RouterId(RTYPE_GLO_RTID, domain_id, ip.s_addr)
{
    opaque_id = narb_ospf_opaque_id();
    hide = false;
    rt_type = RT_TYPE_DEFAULT;
}

router_id_info* DomainInfo::LookupRouterById(in_addr id)
{
    Prefix prefix;
    prefix.length  = 64;
    prefix_word(prefix, 0) = id.s_addr;
    prefix_word(prefix, 1) = id.s_addr;
    RadixNode<Resource>* node = RDB.LookupByPrefix(RTYPE_GLO_RTID, &prefix);
    if (node)
        return (router_id_info*)node->Data();
    return NULL;
}

link_info::link_info(u_int32_t domain_id, in_addr advId, in_addr linkId):
    Link(RTYPE_GLO_ABS_LNK, domain_id, advId.s_addr, linkId.s_addr)
{
    opaque_id = narb_ospf_opaque_id();
    hide = false;
    info_flag = 0;
    ISCD * iscd = new ISCD;
    memset(iscd, 0, sizeof(ISCD));
    iscds.push_back(iscd);
}

link_info* DomainInfo::LookupLinkByLclIf(in_addr id)
{
    RadixTree<Resource>* link_tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if (link = (Link*)node->Data())
            if (link->lclIfAddr == id.s_addr)
                return (link_info*)link;
        node = link_tree->NextNode(node);
    }
    return NULL;
}

link_info* DomainInfo::LookupLinkByRmtIf(in_addr id)
{
    RadixTree<Resource>* link_tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
    RadixNode<Resource>* node = link_tree->Root();
    Link* link;
    while (node)
    {
        if (link = (Link*)node->Data())
            if (link->rmtIfAddr == id.s_addr)
                return (link_info*)link;
        node = link_tree->NextNode(node);
    }
    return NULL;
}

link_info* DomainInfo::LookupInterdomainLinkByLclIf(in_addr id)
{
    link_info* link;
    for (int i = 0; i < inter_domain_te_links.size(); i++)
        if (inter_domain_te_links[i]->lclIfAddr == id.s_addr)
            return inter_domain_te_links[i];

    return NULL;
}

link_info* DomainInfo::LookupInterdomainLinkByRmtIf(in_addr id)
{
    link_info* link;
    for (int i = 0; i < inter_domain_te_links.size(); i++)
        if (inter_domain_te_links[i]->rmtIfAddr == id.s_addr)
            return inter_domain_te_links[i];

    return NULL;
}

void DomainInfo::SearchAndProcessInterdomainLink(list<ero_subobj*>&ero)
{
    list<ero_subobj*>::iterator it;
 
    for (it = ero.begin(); it != ero.end(); it++)
    {
        if ((*it) && (*it)->hop_type == ERO_TYPE_LOOSE_HOP)
        {
            if (LookupInterdomainLinkByLclIf((*it)->addr) != NULL)
            {
                (*it)->hop_type = ERO_TYPE_STRICT_HOP;
                it++;
                assert(it != ero.end());
                (*it)->hop_type = ERO_TYPE_STRICT_HOP;
            }
            return;
        }
     }
}

//Useless for Links not locally originated 
bool DomainInfo::FromSameRouter(in_addr& id1, in_addr& id2)
{
    if (id1.s_addr == id2.s_addr)
        return true;
    link_info * link1 = LookupLinkByLclIf(id1);
    if (!link1)
        return false;
    link_info * link2 = LookupLinkByLclIf(id2);
    if (!link2)
        return false;
    if (link1->AdvRtId() == link2->AdvRtId())
        return true;
    return false;
}

//Useless for Links not locally originated 
bool DomainInfo::ToSameRouter(in_addr& id1, in_addr& id2)
{
    if (id1.s_addr == id2.s_addr)
        return true;
    link_info * link1 = LookupLinkByRmtIf(id1);
    if (!link1)
        return false;
    link_info * link2 = LookupLinkByRmtIf(id2);
    if (!link2)
        return false;
    if (link1->Id() == link2->Id())
        return true;
    return false;
}

void DomainInfo::AddPeerNarb (char *addr, int port, in_addr rmt_if)
{
    if_narb_info * nodedata;
    int i;
    for (i = 0; i < if_narb_table.size(); i++)
    {
          nodedata = if_narb_table[i];
          if (strcmp(nodedata->addr, addr) == 0)
          {
              if (port > 0)
                  nodedata->port = port;
              list<in_addr>::iterator it;
              for (it = nodedata->if_addr_list.begin(); it != nodedata->if_addr_list.end(); it++)
              {
                  if ((*it).s_addr ==rmt_if.s_addr)
                      break;
              }
              if (it == nodedata->if_addr_list.end())
                  nodedata->if_addr_list.push_back(rmt_if);
              break;
          }
    }

    if (i == if_narb_table.size())
    {
        nodedata = new if_narb_info;
        strcpy(nodedata->addr, addr);
        nodedata->port = port;
        nodedata->if_addr_list.push_back(rmt_if);
        if_narb_table.push_back(nodedata);
    }
}

if_narb_info * DomainInfo::LookupNarbByAddr(char *addr)
{
    assert(addr);
    if_narb_info * nodedata;
    for (int i = 0; i < if_narb_table.size(); i++)
    {
        nodedata = if_narb_table[i];
        assert (nodedata);
                    
        if (strcmp(nodedata->addr, addr) == 0)
            return NULL;
    }

    return NULL;
}

if_narb_info * DomainInfo::LookupNarbByRmtIf(in_addr rmt_if)
{
    if_narb_info * nodedata;
    for (int i = 0; i < if_narb_table.size(); i++)
    {
        nodedata = if_narb_table[i];
        assert (nodedata);
      
        list<in_addr>::iterator it;
        for (it = nodedata->if_addr_list.begin(); it != nodedata->if_addr_list.end(); it++)
        {
            if ((*it).s_addr ==rmt_if.s_addr)
                return nodedata;
        }
    }

    return NULL;
}

router_id_info * DomainInfo::FirstRouterId()
{
    router_id_info * router = NULL;
    RadixTree<Resource>* tree = RDB.Tree(RTYPE_GLO_RTID);
    RadixNode<Resource>* node = tree->Root();

    if (node)
    {
        router = (router_id_info*)node->Data();
        if (!router)
        {
            node = tree->NextNodeHavingData(node);
            if (node)
                router = (router_id_info*)node->Data();
        }
    }

    current_node = node;
    return router;
}

router_id_info * DomainInfo::NextRouterId()
{
    RadixTree<Resource>* tree = RDB.Tree(RTYPE_GLO_RTID);
    if (current_node)
    {
        current_node = tree->NextNodeHavingData(current_node);
        if (current_node)
            return (router_id_info*)current_node->Data();
    }
    return NULL;
}

link_info * DomainInfo::FirstLink()
{
    link_info * link = NULL;
    RadixTree<Resource>* tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
    RadixNode<Resource>* node = tree->Root();

    if (node)
    {
        link = (link_info*)node->Data();
        if (!link)
        {
            node = tree->NextNodeHavingData(node);
            if (node)
                link = (link_info*)node->Data();            
        }
    }

    current_node = node;
    return link;
}

link_info * DomainInfo::NextLink()
{
    RadixTree<Resource>* tree = RDB.Tree(RTYPE_GLO_ABS_LNK);
    if (current_node)
    {
        current_node = tree->NextNodeHavingData(current_node);
        if (current_node)
            return (link_info*)current_node->Data();
    }
    return NULL;
}

void DomainInfo::ClearRouterIds()
{
    RDB.ClearTree(RTYPE_GLO_RTID);
}

void DomainInfo::ClearLinks()
{
    RDB.ClearTree(RTYPE_GLO_ABS_LNK);
}

//  Timer function to originate domain summary LSA's periodically
void DomainTopologyOriginator::Run()
{
    if (!ospf_client || !ospf_client->Alive())
        return;
    if (!ospf_client->GetReader() || !!ospf_client->GetWriter())
    {
        if (ospf_client->RunWithoutSyncLsdb() < 0)
            return;
    }

    NarbDomainInfo.DeleteTopology(ospf_client->GetWriter());
    NarbDomainInfo.OriginateTopology(ospf_client->GetWriter());
}


// Function originating Router ID TE LSA
int DomainInfo::OriginateRouterId (ZebraOspfWriter* oc_writer, router_id_info* router)
{
  int ret = 0;
  void *opaquedata;
  int opaquelen;
  u_char lsa_type = 10;
  u_char opaque_type = 1;

  opaquedata = (void *)ospf_te_router_addr_tlv_alloc(*(in_addr*)&router->id); 
  opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                + sizeof (struct te_tlv_header);

  router->opaque_id = narb_ospf_opaque_id();
  ret = oc_writer->OriginateLsa(NarbDomainInfo.ospfd_inter.ori_if,
                *(in_addr*)&router->advRtId, //$$$$ hacked
                NarbDomainInfo.ospfd_inter.area, lsa_type, opaque_type, 
                router->opaque_id, opaquedata, opaquelen);

  free(opaquedata);
  
  LOGF("ROUTER_ID (lsa-type[%d] opaque-type[%d] opaque-id[%d]) originated through OSPFd at %s.\n", 
     lsa_type, opaque_type, router->opaque_id, NarbDomainInfo.ospfd_inter.addr);
  LOGF("\t ID = %X, ADV_ROUTER = %X, return code = %d.\n", router->id, router->advRtId, ret);

  return ret;
}

void* DomainInfo::BuildTeLinkOpaqueData(link_info* link)
{
  void* opaquedata = (void *)ospf_te_link_tlv_alloc(link->linkType, *(in_addr*)&link->id); 
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_LOC_IF))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LCLIF_IPADDR, &link->lclIfAddr);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_REM_IF))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RMTIF_IPADDR, &link->rmtIfAddr);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_MAX_BW, &link->maxBandwidth);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
    		TE_LINK_SUBTLV_TE_METRIC, &link->metric);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_RSV_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
    		TE_LINK_SUBTLV_MAX_RSV_BW, &link->maxReservableBandwidth);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_UNRSV_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_UNRSV_BW, link->unreservedBandwidth);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LINK_IFSWCAP, (void*)link->ifswcap);
      if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_VLAN))
          opaquedata = ospf_te_link_subtlv_set_swcap_vlan((te_tlv_header*)opaquedata, link->vtagBitMask, link->vtagBitMaskAlloc);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_RESV))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RESV_SCHEDULE, (void*)(&link->resvTable.reserves));
    }

  opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_DOMAIN_ID, (void*)&NarbDomainInfo.domain_id);

  return opaquedata;
}

// Function originating Link TE LSA
int DomainInfo::OriginateTeLink (ZebraOspfWriter* oc_writer, link_info* link)
{
  int ret = 0;
  u_char lsa_type = 10;
  u_char opaque_type = 1;
  
  void *opaquedata = BuildTeLinkOpaqueData(link);
  int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                    + sizeof (struct te_tlv_header);

  link->opaque_id = narb_ospf_opaque_id();
  ret = oc_writer->OriginateLsa(NarbDomainInfo.ospfd_inter.ori_if,
                *(in_addr*)&link->advRtId, //$$$$ DRAGON
                NarbDomainInfo.ospfd_inter.area, lsa_type, opaque_type, 
                link->opaque_id, opaquedata, opaquelen);
  
  free(opaquedata);
  
  LOGF("TE_LINK (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) originated through OSPFd at %s.\n", 
     lsa_type, opaque_type, link->opaque_id, NarbDomainInfo.ospfd_inter.addr);
  LOGF("\t ID = %X, TYPE = %X, ADV_ROUTER = %X, return code = %d.\n", link->id, link->type, link->advRtId, ret);

  return ret;
}

// Function updating Link TE LSA
int DomainInfo::UpdateTeLink (ZebraOspfWriter* oc_writer, link_info* link)
{
   /*
    oc_writer->DeleteLsa(*(in_addr*)&link->advRtId, //$$$$ DRAGON
                    NarbDomainInfo.ospfd_inter.area, 10, 1, link->opaque_id);    
    return OriginateTeLink(oc_writer, link);
    */
  int ret = 0;
  u_char lsa_type = 10;
  u_char opaque_type = 1;
  
  void *opaquedata = BuildTeLinkOpaqueData(link);
  int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                    + sizeof (struct te_tlv_header);

  ret = oc_writer->UpdateLsa(NarbDomainInfo.ospfd_inter.ori_if,
                *(in_addr*)&link->advRtId, //$$$$ DRAGON
                NarbDomainInfo.ospfd_inter.area, lsa_type, opaque_type, 
                link->opaque_id, opaquedata, opaquelen);
  
  free(opaquedata);
  
  LOGF("TE_LINK (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) updated through OSPFd at %s.\n", 
     lsa_type, opaque_type, link->opaque_id, NarbDomainInfo.ospfd_inter.addr);
  LOGF("\t ID = %X, TYPE = %X, ADV_ROUTER = %X, return code = %d.\n", link->id, link->type, link->advRtId, ret);

  return ret;
}

// Implementation of LSA origination using router_ids and te_links 
// in narb_domain_info. LSA's are originated to OSPFd. Note that 
// the function ospf_apiclient_lsa_originate has been hacked to use 
// different advertising router ids instead of using the id of OSPFd only.
int DomainInfo::OriginateTopology (ZebraOspfWriter* oc_writer)
{
    int ret = 0;
    
    //Automatically probing/refreshing virtual te links using intRA-domain OSPFd CSPF requests
    NarbDomainInfo.CleanupProbedVirtualLinks();
    NarbDomainInfo.AutoProbeVirtualLinks();

    // originate router-id LSA's
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        if (!router_id->hide)
        {
            this->OriginateRouterId(oc_writer, router_id);
         }
        router_id = NarbDomainInfo.NextRouterId();
    }

    // originate  te-link LSA's
    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        if (!link->hide)
        {
            this->OriginateTeLink(oc_writer, link);
        }
        link = NarbDomainInfo.NextLink();
    }

    return ret;
}

// delete all router_id and te_link LSA's originated from narb_domain_info
// Note that the function ospf_apiclient_lsa_delete has been hacked
// to use different advertising router ids.
int DomainInfo::DeleteTopology (ZebraOspfWriter* oc_writer)
{
    int ret = 0;
  
    u_char lsa_type = 10;
    u_char opaque_type = 1;
  
    // delete router-id LSA's
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        if (!router_id->hide)
        {
            ret = oc_writer->DeleteLsa(*(in_addr*)&router_id->advRtId, /* $$$$ hacked */
                            NarbDomainInfo.ospfd_inter.area, lsa_type, opaque_type, router_id->opaque_id);
            
            LOGF("ROUTER_ID (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) deleted through OSPFd at %s.\n", 
               lsa_type, opaque_type, router_id->opaque_id, NarbDomainInfo.ospfd_inter.addr);
            LOGF("\t ID = %X, ADV_ROUTER = %X, return code = %d.\n", router_id->id, router_id->advRtId, ret);
        }
        router_id = NarbDomainInfo.NextRouterId();
    }

    // delete  te-link LSA's
    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        if (!link->hide)
        {
            ret = oc_writer->DeleteLsa(*(in_addr*)&link->advRtId, /* $$$$ hacked */
                            NarbDomainInfo.ospfd_inter.area, lsa_type,  
                            opaque_type, link->opaque_id);
            
            LOGF("LINK_ID (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) deleted through OSPFd at %s.\n", 
               lsa_type, opaque_type, link->opaque_id, NarbDomainInfo.ospfd_inter.addr);
            LOGF("\t ID = %X, ADV_ROUTER = %X, return code = %d.\n", link->id, link->advRtId, ret);
        }
        link = NarbDomainInfo.NextLink();
    }

    return ret;
}

void DomainInfo::HideTopology ()
{
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        router_id->hide = true;
        router_id = NarbDomainInfo.NextRouterId();
    }

    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        link->hide = true;
        link = NarbDomainInfo.NextLink();
    }
}

void DomainInfo::ExposeTopology ()
{
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        router_id->hide = false;
        router_id = NarbDomainInfo.NextRouterId();
    }

    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        link->hide = false;
        link = NarbDomainInfo.NextLink();
    }
 }

link_info* DomainInfo::VirtualTeLinkProbe(RCE_APIClient& rce, svc_probe *svc_probe, router_id_info *router)
{
    u_int32_t seqnum;
    te_tlv_header * tlv;
    link_info *link = NULL;
    int len;
    in_addr *router_ip;
  
    struct msg_narb_cspf_request cspf_req;
    memset(&cspf_req, 0, sizeof(msg_narb_cspf_request));
    
    // messages to OSPFd_intra and OSPFd_inter use different area id
    cspf_req.area_id = NarbDomainInfo.ospfd_intra.area;
    cspf_req.app_req_data.src.s_addr = svc_probe->router->id;
    cspf_req.app_req_data.dest.s_addr = router->id;
    cspf_req.app_req_data.switching_type = svc_probe->service->sw_type;
    cspf_req.app_req_data.encoding_type = svc_probe->service->encoding;
    cspf_req.app_req_data.bandwidth = svc_probe->service->max_bw;
    seqnum = 0xffffffff; //$$$$
  
    api_msg *rce_msg;
    rce_msg = api_msg_new((u_char)MSG_LSP, (u_char)ACT_QUERY, sizeof(cspf_req.app_req_data), &cspf_req.app_req_data, cspf_req.lspb_id, cspf_req.app_seqnum);
    rce_msg->header.options = LSP_TLV_NARB_CSPF_REQ | LSP_OPT_STRICT;
    
    rce.GetWriter()->WriteMessage(rce_msg);
    api_msg_delete(rce_msg);
    rce_msg = rce.ReadMessage();
    
    if (!rce_msg)
        return NULL;
    
    assert (rce_msg->header.type_8== MSG_LSP && ntohl(rce_msg->header.seqnum) == seqnum);

    if (rce_msg->header.action== ACT_ACKDATA)
    {
        list<ero_subobj*> ero;
        LSPQ::GetERO((te_tlv_header*)rce_msg->body, ero);
        if (ero.size() <= 2)
            goto _out;
        in_addr ip; ip.s_addr = 0;
        link = new link_info(NarbDomainInfo.domain_id, ip, ip);
        link->advRtId = svc_probe->router->id;
        link->id = router->id;
        link->linkType = 1;
        link->lclIfAddr = ero.front()->addr.s_addr;
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_LOC_IF);
        link->lclIfAddr = ero.back()->addr.s_addr;
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_REM_IF);
        link->ifswcap->swtype = (u_char)svc_probe->service->sw_type;
        link->ifswcap->encoding = (u_char)svc_probe->service->encoding;
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
        link->maxBandwidth = link->maxReservableBandwidth  = svc_probe->service->max_bw;
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_BW);
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_RSV_BW);
        if (svc_probe->router->rt_type == RT_TYPE_HOST || router->rt_type == RT_TYPE_HOST)
            link->metric = HOST_BORDER_METRIC;
        else if (svc_probe->router->type == RT_TYPE_BORDER && router->type == RT_TYPE_BORDER)
            link->metric = BORDER_BORDER_METRIC;
        else
            link->metric = 1;
        SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC);
    }
  
_out:
    api_msg_delete (rce_msg);
    return link;
}

void DomainInfo::AutoProbeVirtualLinks()
{
    svc_probe *svc_probe;
    router_id_info *router;
    link_info *link;
  
    if(NarbDomainInfo.svc_probes.size() == 0)
        return;
  
    //Start RCE APIClient
    RCE_APIClient rce_client((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
    rce_client.Connect();
 
    for (int i = 0; i < NarbDomainInfo.svc_probes.size(); i++)
    {
        svc_probe = NarbDomainInfo.svc_probes[i];
        router = FirstRouterId();
        while(router)
        {
            if ( (svc_probe->router->type == RT_TYPE_HOST && router->type == RT_TYPE_BORDER)
              || (svc_probe->router->type == RT_TYPE_BORDER && router != svc_probe->router) )
            {
                link = VirtualTeLinkProbe(rce_client, svc_probe, router);
                if (link)
                {
                    AddLink( link);
                    SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_AUTO_PROBED);
                }
            }
            router = NextRouterId();
        }
    }

    //Stop RCE APIClient
    rce_client.Close();
}

void DomainInfo::CleanupProbedVirtualLinks()
{
    link_info * link = NarbDomainInfo.FirstLink();

    while (link)
    {
        if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_AUTO_PROBED))
        {
            RDB.Delete(link);
        }
        link = NarbDomainInfo.NextLink();
    }
}

