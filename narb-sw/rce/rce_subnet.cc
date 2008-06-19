/*
 * Copyright (c) 2007
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang Jan 23, 2007
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

#include "rce_subnet.hh"
#include "rce_movaz_types.hh"

router_id_info::router_id_info(ResourceType router_type, u_int32_t domain_id, in_addr ip):RouterId(router_type, domain_id, ip.s_addr)
{
    hide = false;
    rt_type = RT_TYPE_DEFAULT;
    home_vlsr = 0;
}

link_info::link_info(ResourceType link_type, u_int32_t domain_id, in_addr advId, in_addr linkId):
    Link(link_type, domain_id, advId.s_addr, linkId.s_addr)
{
    hide = false;
    info_flag = 0;
    ISCD * iscd = new ISCD;
    memset(iscd, 0, sizeof(ISCD));
    iscds.push_back(iscd);
}

void Subnet::AddRouter(router_id_info * router)
{
    assert (router);
    router->type = routerType;
    //local search for duplicate
    routerVector.push_back(router);
    RDB.Update(router);

    if (router->home_vlsr != 0)
        SystemConfig::AddHomeVlsrRouterIdPair(router->home_vlsr, router->id);
}

void Subnet::AddLink(link_info* link)
{
    assert (link);
    link->type = linkType;
    //local search for duplicate
    linkVector.push_back(link);
    RDB.Update(link);
}

void Subnet::HideTopology()
{
    int i, nR = routerVector.size(), nL = linkVector.size();
    
    for (i = 0; i < nR; i++)
    {
        RDB.Remove(routerVector[i]);
	routerVector[i]->hide = true;
    }

    for (i = 0; i < nL; i++)
    {
        RDB.Remove(linkVector[i]);
	linkVector[i]->hide = true;
    }
}

void Subnet::ExposeTopology()
{
    int i, nR = routerVector.size(), nL = linkVector.size();
    
    for (i = 0; i < nR; i++)
    {
        RDB.Update(routerVector[i]);
	routerVector[i]->hide = false;
    }

    for (i = 0; i < nL; i++)
    {
        RDB.Update(linkVector[i]);
	linkVector[i]->hide = false;
    }
}

void Subnet::ClearTopology()
{
    int i, nR = routerVector.size(), nL = linkVector.size();
    
    for (i = 0; i < nR; i++)
    {
        RDB.Delete(routerVector[i]);
    }
    routerVector.clear();
    
    for (i = 0; i < nL; i++)
    {
        RDB.Delete(linkVector[i]);
    }
    linkVector.clear();
}


/////////////////////////////////////////////////////
/////         Borrowed in part from narb_config.cc          //////
////////////////////////////////////////////////////

void Subnet_ConfigFile::Init(const char* fileName)
{
    ReadConfig(fileName? (char*)fileName : (char*)SystemConfig::subnet_file.c_str(), (char*)SUBNET_CURRENT_CONFIG_FILE, (char*)SUBNET_DEFAULT_CONFIG_FILE);
}

// Subnet topology is created from here.
void Subnet_ConfigFile::ConfigFromFile(ifstream& inFile)
{
    char buf[CONFIG_BUFSIZ];
    string line;
    char blk_header[CONFIG_LINESIZ];
    char blk_body[CONFIG_BLKSIZ];
  
    int len;
    int config_code;
    char * next_blk = buf;
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
        case  CONFIG_ROUTER:
          {
              char link_header[CONFIG_LINESIZ], link_body[CONFIG_BLKSIZ];
              char * link_blk, *p_str;
              int ret;
              char router_id[MAXADDRLEN];
              char home_vlsr[MAXADDRLEN];
              int service_id;
              in_addr ip; ip.s_addr = 0;
              router_id_info * router = new router_id_info(routerType, domainId, ip);
  
              if (ReadConfigParameter(blk_body, (char*)"id", (char*)"%s", router_id))
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

              if (ReadConfigParameter(blk_body, (char*)"home_vlsr", (char*)"%s", home_vlsr))
              {
                  inet_aton(home_vlsr, (in_addr*)&router->home_vlsr);
              }

              ReadConfigParameter(blk_body, (char*)"dtl_name", (char*)"%s", router->dtl_name);

              AddRouter(router);

              link_blk = strstr(blk_body, "link");
              while (link_blk && strstr(link_blk, "link"))
              {
                  char link_id[MAXADDRLEN];
                  char loc_if[MAXADDRLEN];
                  char rem_if[MAXADDRLEN];
                  int link_type;
                  int encoding, sw_type;
                  link_info *link;
                    
                  ret = ReadConfigBlock(link_blk, link_header, link_body, &link_blk);
                  assert (ret == CONFIG_LINK);
                  in_addr ip; ip.s_addr = 0;
                  link = new link_info(linkType, domainId, ip, ip);
  
                  // reading madatory parameter
                  if (ReadConfigParameter(link_body, (char*)"id", (char*)"%s", link_id))
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
                  if (ReadConfigParameter(link_body, (char*)"type", (char*)"%d", &link_type))
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
                  if (ReadConfigParameter(link_body, (char*)"local_if", (char*)"%s", loc_if))
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
                  if (ReadConfigParameter(link_body, (char*)"remote_if", (char*)"%s", rem_if))
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
                  if (ReadConfigParameter(link_body, (char*)"max_bw", (char*)"%f", &link->maxBandwidth))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_BW);
                  }
  
                  if (ReadConfigParameter(link_body, (char*)"max_rsv_bw", (char*)"%f", &link->maxReservableBandwidth))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag,  LINK_PARA_FLAG_MAX_RSV_BW);
                  }
  
                  if (ReadConfigParameter(link_body, (char*)"unrsv_bw0", (char*)"%f", &link->unreservedBandwidth[0]))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag,  LINK_PARA_FLAG_UNRSV_BW);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw1", (char*)"%f", &link->unreservedBandwidth[1]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw2", (char*)"%f", &link->unreservedBandwidth[2]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw3", (char*)"%f", &link->unreservedBandwidth[3]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw4", (char*)"%f", &link->unreservedBandwidth[4]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw5", (char*)"%f", &link->unreservedBandwidth[5]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw6", (char*)"%f", &link->unreservedBandwidth[6]);
                      ReadConfigParameter(link_body, (char*)"unrsv_bw7", (char*)"%f", &link->unreservedBandwidth[7]);
                      memcpy(link->_ifswcap->max_lsp_bw, link->unreservedBandwidth, 8*sizeof(float));
                  }
  
                  if (ReadConfigParameter(link_body, (char*)"enc_type", (char*)"%d", &encoding))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->_ifswcap->encoding = (u_char)encoding;
                  }
  
                  if (ReadConfigParameter(link_body, (char*)"sw_type", (char*)"%d", &sw_type))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->_ifswcap->swtype = (u_char)sw_type;
                  }
  
                  if (ReadConfigParameter(link_body, (char*)"metric", (char*)"%d", &link->metric))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC);
                  }

                  if (ReadConfigDragonLambda(link_body, (char*)"dragon_lambda", link))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_DRAGON_LAMBDA);
                  }

                  if (ReadConfigWdmTeGrid(link_body, (char*)"wavelength_grid", link))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_WDM_TE_GRID);
                  }

                  ReadConfigParameter(link_body, (char*)"dtl_id", (char*)"%d", &link->dtl_id);

                  AddLink(link);
                }
          }
          break;

      case  CONFIG_UNKNOWN:
      default:
         LOGF("Unknow configration block: %s {%s} for Subnet_ConfigFile::ConfigFromFile()\n", blk_header, blk_body);
      }
    }

}


int Subnet_ConfigFile::ReadConfigDragonLambda(char* buf, char* id, link_info* link)
{
    char *str;
    str = strstr(buf, id);
    if (!str)
        return 0;

#ifdef HAVE_EXT_ATTR
    u_int32_t lambda = 0;
    sscanf(str, "%d", &lambda);
    if (lambda != 0)
    {
        ResourceIndexingElement *pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA");
        int a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/DRAGON_LAMBDA");
        if (pe == NULL || a_index == 0)
            return 0;

        if (link->attrTable.size() < a_index +1)
        {
            link->attrTable.resize(a_index+1);
        }
        link->SetAttribute(a_index, pe->dataType, 4, (char*)&lambda, pe);
        return 1;
    }
#endif

    return 0;
}


int Subnet_ConfigFile::ReadConfigWdmTeGrid(char* buf, char* id, link_info* link)
{
    static char buf_para[256];

    char *str;
    str = strstr(buf, id);
    if (!str)
        return 0;

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

    ResourceIndexingElement *pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA");
    int a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LAMBDA");
    if (pe == NULL || a_index == 0)
        return 0;
    if (link->attrTable.size() < a_index +1)
    {
        link->attrTable.resize(a_index+1);
    }

    while (str)
    {
        u_int32_t lambda;
	 if(sscanf(str, "%d", &lambda) != 1)
            return 0;
        int l = (lambda-192000)/100;
        if (l < 0 || l >= 40)
            return 0;

        lambda_info.channel_id = lambda;
        link->SetAttribute(a_index, pe->dataType, sizeof(movaz_tlvdata_te_lambda_info), (char*)&lambda_info, pe);
        
        wavegrid.in_channels[l/2] |= ((0xf0) >> ((l%2)*4)); // l even: 0xf0; l odd: ox0f;
        wavegrid.out_channels[l/2] |= ((0xf0) >> ((l%2)*4)); // l even: 0xf0; l odd: ox0f;
        str = strtok(NULL, " \t,;[]()|");
    }

    pe = GET_ATTR_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID");
    a_index = ATTR_INDEX_BY_TAG("LSA/OPAQUE/TE/LINK/MOVAZ_TE_LGRID");
    if (pe == NULL || a_index == 0)
        return 0;

    if (link->attrTable.size() < a_index +1)
    {
        link->attrTable.resize(a_index+1);
    }
    link->SetAttribute(a_index, pe->dataType, sizeof(movaz_tlvdata_te_passthrough_wavegrid), (char*)&wavegrid, pe);
    return 1;
#endif

    return 0;
}
