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
#include "rce_types.hh"
#include "rce_config.hh"
#include "terce_apiclient.hh"

string SystemConfig::config_file;
string SystemConfig::schema_file;

string SystemConfig::ospfd_inter_host("localhost");
int SystemConfig::ospfd_inter_port = 2607;
int SystemConfig::ospfd_inter_port_local = 4607;

string SystemConfig::ospfd_intra_host("localhost");
int SystemConfig::ospfd_intra_port = 2617;
int SystemConfig::ospfd_intra_port_local = 4617;

string SystemConfig::terce_host;
int SystemConfig::terce_port = 0;
int SystemConfig::terce_port_local = 0;

int SystemConfig::ospf_sync_interval = 30;
int SystemConfig::max_ospf_sync_attempts = 10;

int SystemConfig::delta_expire_query = 10;
int SystemConfig::delta_expire_reserve = 30;

string SystemConfig::narb_host("localhost");
int SystemConfig::narb_port = 2609;

int SystemConfig::api_port = 2678;
int SystemConfig::cli_port = 2688;
int SystemConfig::metric_interdomain_nice_incremental = 1000;
    
string SystemConfig::cli_password("dragon");
string SystemConfig::cli_address("localhost");
int SystemConfig::cli_timeout = 900; //CLI timeout = 15 minutes

u_int32_t SystemConfig::domainMask = 0;
u_int32_t SystemConfig::domainId = 0;
string SystemConfig::sysDescription = "";

string SystemConfig::subnet_file = "";
bool  SystemConfig::should_incorporate_subnet = false;

HomeVlsrRouterPairList SystemConfig::home_vlsr_search_list;

void SystemConfig::AddHomeVlsrRouterIdPair(u_int32_t home_vlsr, u_int32_t router_id)
{
    HomeVlsrRouterPair pair;
    pair.home_vlsr = home_vlsr; pair.router_id = router_id;

    HomeVlsrRouterPairList::iterator it = SystemConfig::home_vlsr_search_list.begin();
    for ( ; it != SystemConfig::home_vlsr_search_list.end(); it++ )
    {
        if ( (*it).home_vlsr == home_vlsr && (*it).router_id == router_id)
            break;
    }

    if (it == SystemConfig::home_vlsr_search_list.end())
        SystemConfig::home_vlsr_search_list.push_back(pair);
}

u_int32_t SystemConfig::FindHomeVlsrByRouterId(u_int32_t router_id)
{
    HomeVlsrRouterPairList::iterator it = SystemConfig::home_vlsr_search_list.begin();
    for ( ; it != SystemConfig::home_vlsr_search_list.end(); it++ )
    {
        if ( (*it).router_id == router_id )
            return (*it).home_vlsr;
    }

    return 0;
}

/////////////////////////////////////////////////////
/////         Borrowed in part from narb_config.cc          //////
////////////////////////////////////////////////////

void SystemConfig::Init(const char* fileName)
{
    ReadConfig(fileName? (char*)fileName : (char*)config_file.c_str(), (char*)CURRENT_CONFIG_FILE, (char*)DEFAULT_CONFIG_FILE);
}

// Read up configuration file from rce config_file. 
void SystemConfig::ReadConfig (char *config_file, char *config_current_dir, char *config_default_dir)
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
void SystemConfig::ConfigFromFile(ifstream& inFile)
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
              else if  (ReadConfigParameter(blk_body, "asn", "%s", domain_id))
              {
                  u_int32_t asn1, asn2;
                  int ret = sscanf(domain_id, "%u.%u", &asn1, &asn2);
                  if (ret == 1)
                      domainId = (asn1&0xffff);
                  else if (ret == 2)
                      domainId = ((asn1 << 16) | (asn2&0xffff));
                  else
                      LOG("DomainID in asn (AS Number) format should be x or x.x, where x is a short integer."<<endl);
              }
              else
              {
                  LOG("ReadConfigParameter failed on domain-id"<<endl);
              }
          }
          break;

        case CONFIG_SUBNET:
          {
              char filename[MAXPATHLEN];
              if (ReadConfigParameter(blk_body, "path", "%s", filename))
              {
                  subnet_file = filename;
              }
          }
          break;

        case CONFIG_SCHEMA:
          {
              char filename[MAXPATHLEN];
              if (ReadConfigParameter(blk_body, "path", "%s", filename))
              {
                  schema_file = filename;
              }
          }
          break;

        case  CONFIG_TERCE:
          {
              char address[MAXADDRLEN];
              int port;
  
              if (ReadConfigParameter(blk_body, "address", "%s", address))
              {
                  SystemConfig::terce_host = address;
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE  : address" << endl);
                  SystemConfig::terce_host = "localhost";
                  LOG("TERCE address has been set to 127.0.0.1 (localhost)" << endl);
              }
  
              if (ReadConfigParameter(blk_body, "localport", "%d", &port))
              {
                  SystemConfig::terce_port_local = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE : localport --> set to default: " << RCE_TERCE_SYNC_PORT << endl);
                  SystemConfig::terce_port_local = RCE_TERCE_SYNC_PORT;
              }
              
              if (ReadConfigParameter(blk_body, "port", "%d", &port))
              {
                  SystemConfig::terce_port = port;
              }
              else
              {
                  LOG("ReadConfigParameter failed on TERCE : port" << endl);
                  SystemConfig::terce_port  = TERCE_API_SERVER_PORT;
                  LOG("TERCE server port has been set to " << TERCE_API_SERVER_PORT << endl);
              }
          }
          break;

      case  CONFIG_UNKNOWN:
      default:
         LOGF("Unknow configration block: %s {%s} for SystemConfig::ConfigFromFile()\n", blk_header, blk_body);
      }
    }

}

int SystemConfig::blk_code (char *buf)
{
    if (strstr(buf, "domain-id"))
        return CONFIG_DOMAIN_ID;
    else if (strstr(buf, "include-subnet-topology"))
        return CONFIG_SUBNET;
    else if (strstr(buf, "include-tedb-schema"))
        return CONFIG_SCHEMA;
    else if (strstr(buf, "terce"))
        return CONFIG_TERCE;
    else if (strstr(buf, "router"))
        return CONFIG_ROUTER;
    else if (strstr(buf, "link"))
        return CONFIG_LINK;
    else
        return CONFIG_UNKNOWN;
}

// reading config block inside {}, note that the block may contain sub-level {}
int SystemConfig::ReadConfigBlock(char *buf, char * header, char * body, char ** next)
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
int SystemConfig::ReadConfigParameter(char * buf, char * id, char * fmt, void * parameter)
{
    char *str;
  
    str = strstr(buf, id);
    if (!str)
        return 0;
  
    // return 1 if successful, otherwise 0
    return sscanf(str + strlen(id) + 1, fmt, parameter);
}


