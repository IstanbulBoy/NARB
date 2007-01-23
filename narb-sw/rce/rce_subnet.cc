/*
 * Copyright (c) 2005
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

router_id_info::router_id_info(ResourceType router_type, u_int32_t domain_id, in_addr ip):RouterId(router_type, domain_id, ip.s_addr)
{
    hide = false;
    rt_type = RT_TYPE_DEFAULT;
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
    ReadConfig(fileName? (char*)fileName : (char*)configFile.c_str(), (char*)SUBNET_CURRENT_CONFIG_FILE, (char*)SUBNET_DEFAULT_CONFIG_FILE);
}

// Read up configuration file from subnet_file. 
void Subnet_ConfigFile::ReadConfig (char *config_file, char *config_current_dir, char *config_default_dir)
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
            return;
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
                return;
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
  
    ConfigFromFile(inFile);
 
    inFile.close();
    return;
}

// Subnet topology is created from here.
void Subnet_ConfigFile::ConfigFromFile(ifstream& inFile)
{
    char buf[SUBNET_BUFSIZ];
    string line;
    char blk_header[SUBNET_LINESIZ];
    char blk_body[SUBNET_BLKSIZ];
  
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
        case CONFIG_DOMAIN_ID:
          {
              char domain_id[MAXADDRLEN];
              if (ReadConfigParameter(blk_body, "ip", "%s", domain_id))
              {
                  inet_aton(domain_id, (struct in_addr*)(&domainId));
              }
              else if  (ReadConfigParameter(blk_body, "id", "%s", domain_id))
              {
                  domainId = strtoul(domain_id, NULL, 10);
              }
              else
              {
                  LOG("ReadConfigParameter failed on domain-id"<<endl);
              }
          }
          break;

        case  CONFIG_ROUTER:
          {
              char link_header[SUBNET_LINESIZ], link_body[SUBNET_BLKSIZ];
              char * link_blk, *p_str;
              int ret;
              char router_id[MAXADDRLEN];
              int service_id;
              in_addr ip; ip.s_addr = 0;
              router_id_info * router = new router_id_info(routerType, domainId, ip);
  
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
                      memcpy(link->_ifswcap->max_lsp_bw, link->unreservedBandwidth, 8*sizeof(float));
                  }
  
                  if (ReadConfigParameter(link_body, "enc_type", "%d", &encoding))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->_ifswcap->encoding = (u_char)encoding;
                  }
  
                  if (ReadConfigParameter(link_body, "sw_type", "%d", &sw_type))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP);
                      link->_ifswcap->swtype = (u_char)sw_type;
                  }
  
                  if (ReadConfigParameter(link_body, "metric", "%d", &link->metric))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC);
                  }

/*
                  if (ReadConfigVlanTagSet(link_body, "vlan_tags", link->vtagBitMask))
                  {
                      SET_LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_VLAN);
                  }
*/
                  AddLink(link);
                }
          }
          break;

      case  CONFIG_UNKNOWN:
      default:
         LOGF("Unknow configration block: %s {%s}\n", blk_header, blk_body);
      }
    }

}

// recognizing a configuration block
static int blk_code (char *buf)
{
    if (strstr(buf, "router"))
        return CONFIG_ROUTER;
    else if (strstr(buf, "link"))
        return CONFIG_LINK;
    else if (strstr(buf, "domain-id"))
        return CONFIG_DOMAIN_ID;
    else
        return CONFIG_UNKNOWN;
}

// reading config block inside {}, note that the block may contain sub-level {}
int Subnet_ConfigFile::ReadConfigBlock(char *buf, char * header, char * body, char ** next)
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
int Subnet_ConfigFile::ReadConfigParameter(char * buf, char * id, char * fmt, void * parameter)
{
    char *str;
  
    str = strstr(buf, id);
    if (!str)
        return 0;
  
    // return 1 if successful, otherwise 0
    return sscanf(str + strlen(id) + 1, fmt, parameter);
}

// reading a parameter of list<u_int32_t> type from a config block (char *buf).
/*
int Subnet_ConfigFile::ReadConfigVlanTagSet(char * buf, char * id, u_char* vtagMask)
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
		vtagMask[vlan/8] = vtagMask[vlan/8] | (0x80 >> ((vlan-1)%8));
        } 
        else if (n == 2) {
		range += vlan;
		if (range > MAX_VLAN_NUM)
			return 0;
		for (; vlan < range; vlan++) {
			vtagMask[vlan/8] = vtagMask[vlan/8] |(0x80 >> ((vlan-1)%8));
		}
	 }
        else
            return 0;
        str = strtok(NULL, " \t,;[]()|");
    }

    return 1;
}
*/
