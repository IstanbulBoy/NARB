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

#ifndef __RCE_SUBNET_HH__
#define __RCE_SUBNET_HH__

using namespace std;
#include <string>
#include "resource.hh"
#include "rce_config.hh"

// data structure of an opaque router id generated in domain summary 
#define RT_TYPE_DEFAULT 0x0
#define RT_TYPE_HOST 0x01
#define RT_TYPE_BORDER 0x02
class router_id_info: public RouterId
{
public:
    bool hide;
    int rt_type; 
    router_id_info(ResourceType router_type, u_int32_t domain_id, in_addr ip);
    virtual ~router_id_info() {}
};

// data structure of an opaque TE-link generated in domain summary 
class link_info: public Link
{
public:
    bool hide;
    u_int16_t info_flag;

#define _ifswcap iscds.front()
    ISCD* GetISCD () { return iscds.front(); }
    link_info(ResourceType link_type, u_int32_t domain_id, in_addr advId, in_addr linkId);
    virtual ~link_info() {}
};

#define LINK_PARA_FLAG_MAX_BW 0x1
#define LINK_PARA_FLAG_MAX_RSV_BW 0x2
#define LINK_PARA_FLAG_UNRSV_BW 0x4
#define LINK_PARA_FLAG_IFSW_CAP 0x10
#define LINK_PARA_FLAG_METRIC 0x20
#define LINK_PARA_FLAG_LOC_IF 0x40
#define LINK_PARA_FLAG_REM_IF 0x80
// DRAGON proprietary Speficit TLV's
#define LINK_PARA_FLAG_RESV 0x100
#define LINK_PARA_FLAG_IACD 0x200
#define LINK_PARA_FLAG_VLAN 0x400
//Flag indicating whether the link is originated by automatic probing 
//This is not indicating a TE parameter
#define LINK_PARA_FLAG_AUTO_PROBED 0x1000 
//macros to access link_info flag 
#define SET_LINK_PARA_FLAG(X, F) ((X) = (X) | (F))
#define LINK_PARA_FLAG(X, F) ((X) & (F))


class Subnet
{
public:
    Subnet(string& name):subnetDescription(name) {
        InternalInit();
    }
    Subnet(const char* name) {
        subnetDescription = name;
        InternalInit();
    }
    virtual ~Subnet() { }

    void AddRouter(router_id_info * router);
    void AddLink(link_info* link);

    void HideTopology();
    void ExposeTopology();
    void ClearTopology();

protected:
    void InternalInit() {
        routerType = RTYPE_LOC_RTID;
        linkType = RTYPE_LOC_PHY_LNK;
        domainId = 0;
        SystemConfig::should_incorporate_subnet = true; 
    }
    string subnetDescription;

    ResourceType routerType;
    ResourceType linkType;
    u_int32_t domainId;
    vector<router_id_info*> routerVector;
    vector<link_info*> linkVector;
};


////////////////////////////////////////////////////////
//////////      Config Subnet Topology from File     ////////////
///////////////////////////////////////////////////////
#define SUBNET_CURRENT_CONFIG_FILE "subnet_ciena.conf"
#define SUBNET_DEFAULT_CONFIG_FILE "/usr/local/dragon/etc/subnet_ciena.conf"
#define SUBNET_BUFSIZ 0x10000
#define SUBNET_BLKSIZ 0x1000
#define SUBNET_LINESIZ 0x800
#define MAXADDRLEN 60
#define MAXPATHLEN 256


// definitions of codes for configuration blocks
enum config_code {
  CONFIG_END = 0,
  CONFIG_ROUTER,
  CONFIG_LINK,
  CONFIG_UNKNOWN
};


class Subnet_ConfigFile: public Subnet
{
public:
    Subnet_ConfigFile(string& fileName):configFile(fileName), Subnet("Subnet-PreconfigedFromFile") {}
    virtual ~Subnet_ConfigFile() { }

    void Init(const char* fileName= NULL);
    int ReadConfig(char *config_file, char *config_current_dir,	char *config_default_dir);
    void ConfigFromFile(ifstream &ifs);
    int ReadConfigBlock(char *buf, char * header, char * body, char ** next);
    int ReadConfigParameter(char * buf, char * id, char * fmt, void * parameter);
    //int ReadConfigVlanTagSet(char * buf, char * id, u_char* vtagMask);

private:
    string configFile;
};

#endif

