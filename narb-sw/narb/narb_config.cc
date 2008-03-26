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
#include "types.hh"
#include "narb_config.hh"
#include "cli.hh"
#include <arpa/inet.h>
#include "terce_apiclient.hh"

string SystemConfig::config_file = NARB_DEFAULT_CONFIG_FILE;
narb_routing_mode SystemConfig::routing_mode  = RT_MODE_MIXED_ALLOWED;
narb_working_mode SystemConfig::working_mode  = WORKING_MODE_DYNAMIC_INTERDOMAIN_TOPOLOGY;
u_int32_t SystemConfig::rce_options = LSP_TLV_NARB_CSPF_REQ | LSP_OPT_STRICT;
bool SystemConfig::forced_merge = true;
bool SystemConfig::zcompress = true;

string SystemConfig::rce_pri_host("localhost");
int SystemConfig::rce_pri_port = 2678;

string SystemConfig::rce_sec_host;
int SystemConfig::rce_sec_port = 0;

u_int32_t SystemConfig::auto_topo_rce_options = 0;

int SystemConfig::narb_ospfd_local_port_inter = 5000;
int SystemConfig::narb_ospfd_local_port_intra = 5010;


DomainInfo SystemConfig::narb_domain_info;
 
int SystemConfig::topology_refresh_interval = 3500;

string SystemConfig::cli_password("dragon");
string SystemConfig::cli_address("localhost");
int SystemConfig::cli_timeout = 900; //CLI time out = 15 minutes

IndexedEROList SystemConfig::indexed_static_ero_list;

int SystemConfig::confirmed_ero_expire_secs = 10;
int SystemConfig::confirmed_ero_trash_secs = 30;

void SystemConfig::AddStaticERO(indexed_ero* p_ero)
{
    assert(p_ero != NULL);
    indexed_static_ero_list.push_back(p_ero);
}

indexed_ero* SystemConfig::LookupStaticERO(u_int32_t src_ip, u_int32_t dest_ip)
{
    IndexedEROList::iterator it = indexed_static_ero_list.begin();
    for (; it != indexed_static_ero_list.end(); it++)
    {
        if ((*it)->src_ip == src_ip && (*it)->dest_ip == dest_ip)
            return (*it);
    }
    return NULL;
}

indexed_ero* SystemConfig::RemoveStaticERO(u_int32_t src_ip, u_int32_t dest_ip)
{
    indexed_ero* p_ero = NULL;
    IndexedEROList::iterator it = indexed_static_ero_list.begin();
    for (; it != indexed_static_ero_list.end(); it++)
    {
        if ((*it)->src_ip == src_ip && (*it)->dest_ip == dest_ip)
        {
            p_ero = (*it);
            indexed_static_ero_list.erase(it);
            break;
        }
    }
    return p_ero;
}

/////////////////// Config File Processing //////////////////////

string ConfigFile::fileName(NARB_DEFAULT_CONFIG_FILE);

// Read up configuration file from config_file. 
int ConfigFile::ReadConfig (char *config_file, char *config_current_dir, char *config_default_dir, DomainInfo& domain_info)
{
    char *cwd;
    ifstream inFile;
    string fullpath ;
  
    if (config_file != NULL)
    {
        if (config_file[0] != '/')
        {
            cwd = getcwd (NULL, MAXPATHLEN);
            fullpath = cwd;;
            fullpath += '/';
            fullpath +=  config_file;
        }
        else
            fullpath = config_file;

        inFile.open (fullpath.c_str(), ifstream::in);

        if (!inFile  || inFile.bad()) 
        {
            LOG_CERR << "Failed to open the config file: " << fullpath << endl;
            return -1;
        }
    }
    else // config_file not specified in comand line
    {
        // Relative path configuration file open. 
        if (config_current_dir)
        {
            inFile.open (config_current_dir, ifstream::in);
          // If there is no relative path exists, open system default file.
        }

        if (!inFile  || inFile.bad()) 
        {
            inFile.open (config_default_dir, ifstream::in);
            if (!inFile  || inFile.bad()) 
            {
                LOG_CERR << "Failed to open the config file: " << config_default_dir << endl;
                return -1;
            }
            else
            {
                fullpath = config_default_dir;
            }
        }
        else
        {
            // Rleative path configuration file. 
            cwd = getcwd (NULL, MAXPATHLEN);
            fullpath = cwd;
            fullpath += '/';
            fullpath += config_current_dir;
        }  
    }
  
    ConfigFromFile(inFile, domain_info);
 
    inFile.close();
    return 0;
}

// recognizing a configuration block
static int blk_code (char *buf)
{
    if (strstr(buf, "domain-id"))
        return CONFIG_DOMAIN_ID;
    else if (strstr(buf, "inter-domain-ospfd"))
        return CONFIG_INTER_DOMAIN_OSPFD;
    else if (strstr(buf, "intra-domain-ospfd"))
        return CONFIG_INTRA_DOMAIN_OSPFD;
    else if (strstr(buf, "terce"))
        return CONFIG_TERCE;
    else if (strstr(buf, "router"))
        return CONFIG_ROUTER;
    else if (strstr(buf, "inter-domain-te-link"))
        return CONFIG_INTER_DOMAIN_TE_LINK;
    else if (strstr(buf, "link"))
        return CONFIG_LINK;
    else if (strstr(buf, "te-profile"))
        return CONFIG_TE_PROFILE;
    else if (strstr(buf, "auto-link"))
        return CONFIG_AUTO_LINK;
    else if (strstr(buf, "auto-topo-rce"))
        return CONFIG_AUTO_TOPO_RCE;
    else if (strstr(buf, "cli"))
        return CONFIG_CLI;
    else
        return CONFIG_UNKNOWN;
}

// parsing the config file and create data structures to store the
// configuration. In particular the abstract topology (domain summary)
// is created from here.
void ConfigFile::ConfigFromFile(ifstream& inFile, DomainInfo& domain_info)
{
    char buf[NARB_BUFSIZ];
    string line;
    char blk_header[NARB_LINESIZ];
    char blk_body[NARB_BLKSIZ];
  
    int len;
    int config_code;
    char * next_blk = buf;
    char area[MAX_ADDR_LEN];
  
    char te_link_id_inter[MAX_ADDR_LEN];
    char narb_addr[MAX_ADDR_LEN];
    int narb_port;
    struct link_info* link;
  
    // reading everything into a big buffer
    buf[0] = 0;
    while (getline(inFile, line))
    {
        if (line[0] == '!')
            continue;
        
        line += ' ';
        strcat (buf, line.c_str());
    }

    // parsing the big buffer into configuration blocks
    while ((config_code = ReadConfigBlock(next_blk, blk_header,  blk_body, & next_blk)) !=CONFIG_END)
    {
        // each block is identified by the config_code
        switch(config_code) 
        {
        case CONFIG_DOMAIN_ID:
          {
              char domain_id[MAX_ADDR_LEN];
              if (ReadConfigParameter(blk_body, "ip", "%s", domain_id))
              {
                  inet_aton(domain_id, (struct in_addr*)(&domain_info.domain_id));
              }
              else if  (ReadConfigParameter(blk_body, "id", "%s", domain_id))
              {
                  domain_info.domain_id = strtoul(domain_id, NULL, 10);
              }
              else if  (ReadConfigParameter(blk_body, "asn", "%s", domain_id))
              {
                  u_int32_t asn1, asn2;
                  int ret = sscanf(domain_id, "%u.%u", &asn1, &asn2);
                  if (ret == 1)
                      domain_info.domain_id = (asn1&0xffff);
                  else if (ret == 2)
                      domain_info.domain_id = ((asn1 << 16) | (asn2&0xffff));
                  else
                      LOG("DomainID in asn (AS Number) format should be x or x.x, where x is a short integer."<<endl);
              }
              else
              {
                  LOG("ReadConfigParameter failed on domain-id"<<endl);
              }
          }
          break;
        case  CONFIG_INTER_DOMAIN_OSPFD:
          {
              char address[MAX_ADDR_LEN];
              int port;
              char ori_if[MAX_ADDR_LEN];
  
              if (ReadConfigParameter(blk_body, "address", "%s", address))
              {
                  strcpy(domain_info.ospfd_inter.addr, address);
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-ospfd  : address" << endl);
                  strcpy(domain_info.ospfd_inter.addr, "127.0.0.1");
                  LOG("inter-domain-ospfd address has been set to 127.0.0.1 (localhost)" << endl);
              }
  
              if (ReadConfigParameter(blk_body, "localport", "%d", &port))
              {
                  domain_info.ospfd_inter.localport = port;
              }
              else
              {
                  domain_info.ospfd_inter.localport =  SystemConfig::narb_ospfd_local_port_inter;
              }
              
              if (ReadConfigParameter(blk_body, "port", "%d", &port))
              {
                  domain_info.ospfd_inter.port = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-ospfd : port" << endl);
                  domain_info.ospfd_inter.port = DEFAULT_OSPFD_INTER_PORT;
                  LOGF("inter-domain-ospfd port has been set to %d \n",  DEFAULT_OSPFD_INTER_PORT);
              }
  
              if (ReadConfigParameter(blk_body, "originate-interface", "%s",   ori_if))
              {
                  inet_aton(ori_if, &domain_info.ospfd_inter.ori_if);
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-ospfd : ori_if" << endl);
              }
  
              if (ReadConfigParameter(blk_body, "area", "%s", area))
              {
                  inet_aton(area, &domain_info.ospfd_inter.area);
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-ospfd : area" <<endl);
              }
          }
          break;
        case  CONFIG_INTRA_DOMAIN_OSPFD:
          {
              char address[MAX_ADDR_LEN];
              int port;
              char ori_if[MAX_ADDR_LEN];
  
              if (ReadConfigParameter(blk_body, "address", "%s", address))
              {
                  strcpy(domain_info.ospfd_intra.addr, address);
              }
              else
              {
                  LOG("ReadConfigParameter failed on intra-domain-ospfd address" <<endl);
                  strcpy(domain_info.ospfd_intra.addr, "127.0.0.1");
                  LOG("intra-domain-ospfd address has been set to 127.0.0.1 (localhost)" << endl);
              }
  
              if (ReadConfigParameter(blk_body, "localport", "%d", &port))
              {
                  domain_info.ospfd_intra.localport = port;
              }
              else
              {
                  domain_info.ospfd_intra.localport = SystemConfig::narb_ospfd_local_port_intra;
              }
              
              if (ReadConfigParameter(blk_body, "port", "%d", &port))
              {
                  domain_info.ospfd_intra.port = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on intra-domain-ospfd : port" << endl);
                  domain_info.ospfd_inter.port = DEFAULT_OSPFD_INTRA_PORT;
                  LOGF("inter-domain-ospfd port has been set to %d\n", DEFAULT_OSPFD_INTRA_PORT);
              }
  
              if (ReadConfigParameter(blk_body, "originate-interface", "%s", ori_if))
              {
                  inet_aton(ori_if, &domain_info.ospfd_intra.ori_if);
              }
              else
              {
                  LOG("ReadConfigParameter failed on intra-domain-ospfd : ori_if" <<endl);
              }
  
              if (ReadConfigParameter(blk_body, "area", "%s", area))
              {
                  inet_aton(area, &domain_info.ospfd_intra.area);
              }
              else
              {
                  LOG("ReadConfigParameter failed on intra-domain-ospfd : area" <<endl);
              }
          }
          break;
        case  CONFIG_TERCE:
          {
              char address[MAX_ADDR_LEN];
              int port;
  
              if (ReadConfigParameter(blk_body, "address", "%s", address))
              {
                  strcpy(domain_info.terce.addr, address);
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE  : address" << endl);
                  strcpy(domain_info.terce.addr, "127.0.0.1");
                  LOG("TERCE address has been set to 127.0.0.1 (localhost)" << endl);
              }
  
              if (ReadConfigParameter(blk_body, "localport", "%d", &port))
              {
                  domain_info.terce.localport = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE : localport --> set to default: " << NARB_TERCE_SYNC_PORT << endl);
                  domain_info.terce.localport =  NARB_TERCE_SYNC_PORT;
              }
              
              if (ReadConfigParameter(blk_body, "port", "%d", &port))
              {
                  domain_info.terce.port = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE : port" << endl);
                  domain_info.terce.port = TERCE_API_SERVER_PORT;
                  LOGF("TERCE server port has been set to %d \n",  TERCE_API_SERVER_PORT);
              }
          }
          break;
        case  CONFIG_ROUTER:
          {
              char link_header[NARB_LINESIZ], link_body[NARB_BLKSIZ];
              char * link_blk, *p_str;
              int ret;
              char router_id[MAX_ADDR_LEN];
              char auto_link_type[10];
              int te_profile_id;
              auto_link *p_auto_link;
              in_addr ip; ip.s_addr = 0;
              router_id_info * router = new router_id_info(NarbDomainInfo.domain_id, ip);
  
              if (ReadConfigParameter(blk_body, "id", "%s", router_id))
              {
                  inet_aton(router_id, (in_addr*)&router->id);
                  router->advRtId = router->id;
              }
              else
              {
                  delete router;
                  LOG("ReadConfigParameter failed on router : id" << endl);
                  continue;
              }
              domain_info.AddRouter(router);
  
              link_blk = blk_body;
              
              if (ReadConfigParameter(blk_body, "auto-link", "%s", auto_link_type) > 0)
              {
                  if (strcasecmp(auto_link_type, "border") == 0)
                      router->rt_type = RT_TYPE_BORDER;
                  else if (strcasecmp(auto_link_type, "stub") == 0 || strcasecmp(auto_link_type, "host") == 0)
                      router->rt_type = RT_TYPE_HOST;
                  else
                  {
                      delete router;
                      LOG("ReadConfigParameter failed on router : wrong auto_link type " << auto_link_type << endl);
                      continue;
                  }
                     
                  link_blk = strstr(blk_body, "te-profile");
                  if (link_blk)
                  {
                      ret = ReadConfigBlock(link_blk, link_header, link_body, &link_blk);
                      assert (ret == CONFIG_TE_PROFILE);
  
                      p_str = strtok(link_body, " \t,");
                      assert(p_str);
                      do {
                          p_auto_link = new auto_link;
                          p_auto_link->router = router;
                          te_profile_id = atoi(p_str);

                          // search for the te_profile block in the te_profiles list
                          p_auto_link->te_profile = domain_info.TeProfileLookupById(te_profile_id);
                          if (!p_auto_link->te_profile)
                          {
                              LOGF("domain_info::TeProfileLookupById failed on te_profile_id (%d)", te_profile_id);
                              delete p_auto_link;
                          }
                          else
                          {
                              domain_info.auto_links.push_back( p_auto_link);
                          }
                        } while ((p_str = strtok(NULL, " \t,")) != NULL);
                  }
              }
  
              // one or more te links advertised by the router 
              // are contained in the router block
              link_blk = strstr(blk_body, "link");
              while (link_blk && strstr(link_blk, "link")) 
              {
                  if (*(link_blk-1) == '-') //excluding auto-links
                  {
                      ReadConfigBlock(link_blk, link_header, link_body, &link_blk);
                      continue;
                  }

                  char link_id[MAX_ADDR_LEN];
                  char loc_if[MAX_ADDR_LEN];
                  char rem_if[MAX_ADDR_LEN];
                  int link_type;
                  int encoding, sw_type;
                  link_info *link;
                    
                  ret = ReadConfigBlock(link_blk, link_header, link_body, &link_blk);
                  assert (ret == CONFIG_LINK);
                  in_addr ip; ip.s_addr = 0;
                  link = new link_info(NarbDomainInfo.domain_id, ip, ip);
  
                  // reading madatory parameter
                  if (ReadConfigParameter(link_body, "id", "%s", link_id))
                  {
                      inet_aton(link_id, (in_addr*) &link->id);
                      inet_aton(router_id, (in_addr*)&link->advRtId);
                  }
                  else
                  {
                      delete link;
                      LOG("ReadConfigParameter failed on link : id" << endl);
                      continue;
                  }
                  // reading madatory parameter
                  if (ReadConfigParameter(link_body, "type", "%d", &link_type))
                  {
                      link->linkType = link_type;
                  }
                  else
                  {
                      delete link;
                      LOG("ReadConfigParameter failed on link : type" << endl);
                      continue;
                  }
                  // reading madatory parameter
                  if (ReadConfigParameter(link_body, "local_if", "%s", loc_if))
                  {
                      inet_aton(loc_if, (in_addr*)&link->lclIfAddr);
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_LOC_IF);
                  }
                  else
                  {
                      delete link;
                      LOG("ReadConfigParameter failed on link : LocIf" << endl);
                      continue;
                  }
                  // reading madatory parameter
                  if (ReadConfigParameter(link_body, "remote_if", "%s", rem_if))
                  {
                      inet_aton(rem_if, (in_addr*)&link->rmtIfAddr);
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_REM_IF);
                  }
                  else
                  {
                      delete link;
                      LOG("ReadConfigParameter failed on link : RemIf" << endl);
                      continue;
                  }
  
                  // reading optional parameter
                  if (ReadConfigParameter(link_body, "max_bw", "%f", &link->maxBandwidth))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_BW);
                  }
  
                  if (ReadConfigParameter(link_body, "max_rsv_bw", "%f", &link->maxReservableBandwidth))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag,  LINK_PARA_FLAG_MAX_RSV_BW);
                  }
  
                  if (ReadConfigParameter(link_body, "unrsv_bw0", "%f", &link->unreservedBandwidth[0]))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag,  LINK_PARA_FLAG_UNRSV_BW);
                      ReadConfigParameter(link_body, "unrsv_bw1", "%f", &link->unreservedBandwidth[1]);
                      ReadConfigParameter(link_body, "unrsv_bw2", "%f", &link->unreservedBandwidth[2]);
                      ReadConfigParameter(link_body, "unrsv_bw3", "%f", &link->unreservedBandwidth[3]);
                      ReadConfigParameter(link_body, "unrsv_bw4", "%f", &link->unreservedBandwidth[4]);
                      ReadConfigParameter(link_body, "unrsv_bw5", "%f", &link->unreservedBandwidth[5]);
                      ReadConfigParameter(link_body, "unrsv_bw6", "%f", &link->unreservedBandwidth[6]);
                      ReadConfigParameter(link_body, "unrsv_bw7", "%f", &link->unreservedBandwidth[7]);
                      memcpy(link->ifswcap->max_lsp_bw, link->unreservedBandwidth, 8*sizeof(float));
                  }
  
                  if (ReadConfigParameter(link_body, "enc_type", "%d", &encoding))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->ifswcap->encoding = (u_char)encoding;
                  }
  
                  if (ReadConfigParameter(link_body, "sw_type", "%d", &sw_type))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->ifswcap->swtype = (u_char)sw_type;
                  }
  
                  if (ReadConfigParameter(link_body, "metric", "%d", &link->metric))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC);
                  }

                  if (ReadConfigVlanTagSet(link_body, "vlan_tags", link->vtagBitMask))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_VLAN);
                  }

                  // more optional TE parameters will be supported in the future
                  domain_info.AddLink(link);
                }
          }
          break;
        case  CONFIG_INTER_DOMAIN_TE_LINK:
          {
              in_addr link_inter_rmt_if;
  
              if (ReadConfigParameter(blk_body, "id", "%s", te_link_id_inter))
              {
                  inet_aton(te_link_id_inter, &link_inter_rmt_if);
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-te-link : id");
                  continue;
              }
  
              // read peering narb information that is associated with this inter-domain link
              if (ReadConfigParameter(blk_body, "narb-peer", "%s", narb_addr))
              {
                  if (!ReadConfigParameter(blk_body, "port", "%d", &narb_port))
                  {
                      narb_port = NARB_API_PORT;
                  }

                  // DRAGON assumes the remote interface address is unique for inter-domain links
                  link = domain_info.LookupLinkByRmtIf(link_inter_rmt_if);                                  
                  if (link)
                  {
                      domain_info.inter_domain_te_links.push_back(link);
                      domain_info.AddPeerNarb(narb_addr, narb_port, link_inter_rmt_if);
                  }
                  else if (domain_info.auto_links.size() == 0)
                  {
                      LOGF("Warning: Unknown inter-domain-te-link ID: %s\n", te_link_id_inter);
                  }
              }
              else
              {
                  LOG("ReadConfigParameter failed on inter-domain-te-link : narb_addr");
              }
        }
        break;
      case  CONFIG_TE_PROFILE:
        {
            te_profile_info *p_te_profile = new te_profile_info;
                      
            if (ReadConfigParameter(blk_body, "id", "%d", &p_te_profile->te_profile_id))
            {
                if (!ReadConfigParameter(blk_body, "sw_type", "%d", &p_te_profile->sw_type))
                    LOG("ReadConfigParameter failed on te_profile : sw_type"<<endl);
                if (!ReadConfigParameter(blk_body, "enc_type", "%d", &p_te_profile->encoding))
                    LOG("ReadConfigParameter failed on te_profile : encoding" <<endl);
                if (!ReadConfigParameter(blk_body, "bandwidth", "%f", &p_te_profile->bandwidth))
                    LOG("ReadConfigParameter failed on te_profile : bandwidth"<<endl);
                if (strstr(blk_body, "has_vtags") ||strstr(blk_body, "HAS_VTAGS"))
                    p_te_profile->has_vtags = true;

                domain_info.te_profiles.push_back(p_te_profile);
            }
            else
            {
                delete p_te_profile;
                LOG("ReadConfigParameter failed on te_profile : id" <<endl);
            }
        }
        break;
      case  CONFIG_AUTO_TOPO_RCE:
        {
            if (strstr(blk_body, "abs") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_ABS;
            if (strstr(blk_body, "phy") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_PHY;
            if (strstr(blk_body, "any-domain") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_ANY_DOMAIN;
            if (strstr(blk_body, "fsc") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_FSC;
            if (strstr(blk_body, "lsc") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_LSC;
            if (strstr(blk_body, "tdm") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_TDM;
            if (strstr(blk_body, "l2sc") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_L2SC;
            if (strstr(blk_body, "psc1") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_PSC1;
            if (strstr(blk_body, "psc2") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_PSC2;
            if (strstr(blk_body, "psc3") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_PSC3;
            if (strstr(blk_body, "psc4") == 0)
                SystemConfig::auto_topo_rce_options |= LSA_QUERY_PSC4;
        }
        break;
      case CONFIG_CLI:
        {
            char pass[20];
            char host[20];

            if (ReadConfigParameter(blk_body, "password", "%s", pass))
            {
                SystemConfig::cli_password = pass;
            }
            if (ReadConfigParameter(blk_body, "host", "%s", host))
            {
                SystemConfig::cli_address = host;
            }

            // Executing the CLI commands embedded in narb.conf
            list<string> cli_cmd_lines;
            CLIReader* cli_reader = NULL;
            CLIWriter* cli_writer = NULL;
            if ( ReadCliCommands(blk_body,"'`\"\t()", cli_cmd_lines) > 0)
            {
                cli_reader = new CLIReader(0, NULL);
                cli_writer = new CLIWriter(0, NULL);
                cli_writer->SetReader(cli_reader);
                cli_writer->SetIsPasswd(false);
                cli_reader->SetWriter(cli_writer);
                cli_reader->InitSession();
                while (!cli_cmd_lines.empty())
                {
                    
                    cli_reader->Execute(cli_cmd_lines.front(), true);
                    cli_cmd_lines.pop_front();
                }
                delete cli_reader;
                delete cli_writer;
            }
        }
        break;
      case  CONFIG_UNKNOWN:
      default:
         LOGF("Unknow configration block: %s {%s}\n", blk_header, blk_body);
      }
    }

}


// reading config block inside {}, note that the block may contain sub-level {}
int ConfigFile::ReadConfigBlock(char *buf, char * header, char * body, char ** next)
{
  int i = 0, j = 0, n = 0;
  int ret = CONFIG_UNKNOWN;

  while (buf[j++] !='{') 
    {
      if (!buf[j-1])
         return CONFIG_END;
    }
  
  if (j)
    {
      n++;
      i = j;
      strncpy(header, buf, i-1);
      header[i] = 0;
      ret = blk_code(header);

      while (n > 0)
        {
          if (!buf[j])
            return CONFIG_END;
   
          if (buf[j] == '{')
            n++;
          else if (buf[j] == '}')
            n--;

          j++;
        }
      
      strncpy(body, buf+i, j-i-1);
      body[j-i-1] = 0;
      *next = buf + j;
    }

  return ret;
}


// reading a parameter from a config block (char *buf). Each parameter is 
// specified in 'id = value' format. fmt describes it data type. The value is 
// returned in the buffer (void * parameter)
int ConfigFile::ReadConfigParameter(char * buf, char * id, char * fmt, void * parameter)
{
    char *str;
  
    str = strstr(buf, id);
    if (!str)
        return 0;

    if (!fmt)
        return 1;

    // return 1 if successful, otherwise 0
    return sscanf(str + strlen(id) + 1, fmt, parameter);
}

// reading a parameter of list<u_int32_t> type from a config block (char *buf).
int ConfigFile::ReadConfigVlanTagSet(char * buf, char * id, u_char* vtagMask)
{
    char *str;
    static char buf_para[256];
    u_int32_t vlan;
    int n, range;

    str = strstr(buf, id);
    if (!str)
        return 0;

    str += (strlen(id)+1);

    int i=0;
    while (str[i] != ']' && str[i] != ')')
    {
        buf_para[i] = *(str+i);
        i++;
        if (i >= 256 || i > strlen(buf) -strlen(id))
            return 0;
    }
    buf_para[i] = 0;
    str = strtok(buf_para, " \t,;[]()|");
    if (!str)
        return 0;

    while (str)
    {
	 n = sscanf(str, "%d:%d", &vlan, &range);

        if (n == 1) {
		vtagMask[(vlan-1)/8] = vtagMask[(vlan-1)/8] | (0x80 >> ((vlan-1)%8));
        } 
        else if (n == 2) {
		range += vlan;
		if (range > MAX_VLAN_NUM)
			return 0;
		for (; vlan < range; vlan++) {
			vtagMask[(vlan-1)/8] = vtagMask[(vlan-1)/8] |(0x80 >> ((vlan-1)%8));
		}
	 }
        else
            return 0;
        str = strtok(NULL, " \t,;[]()|");
    }

    return 1;
}

// reading a CLI command demarked by characters in 'quote', store the command into cmd_str 
// return the pointer to the end of the found command
int ConfigFile::ReadCliCommands(char * buf, const char* quote, list<string>& cmd_lines)
{
    int ret = 0;
    string cmd_str;
    char *str = strstr(buf, "execute-commands");
    if (!str)
        return 0;
    str += 17;
    str = strtok(str, quote);

    while (str != NULL)
    {
        if (strlen(str) > 2)
        {
            cmd_str = str;
            cmd_lines.push_back(cmd_str);
            ret++;
        }
        str = strtok(NULL, quote);
    }

    return ret;
}


