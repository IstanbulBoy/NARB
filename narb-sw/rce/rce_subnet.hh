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

#ifndef __RCE_SUBNET_HH__
#define __RCE_SUBNET_HH__

using namespace std;
#include <string>
#include "resource.hh"
#include "rce_config.hh"
#include "rce_lsp.hh"

// data structure of an opaque router id generated in domain summary 
#define RT_TYPE_DEFAULT 0x0
#define RT_TYPE_HOST 0x01
#define RT_TYPE_BORDER 0x02
class router_id_info: public RouterId
{
public:
    u_int32_t home_vlsr;
    bool hide;
    int rt_type; 
    char dtl_name[MAX_DTL_NODENAME_LEN+1];

    router_id_info(ResourceType router_type, u_int32_t domain_id, in_addr ip);
    virtual ~router_id_info() {}
};

// data structure of an opaque TE-link generated in domain summary 
class link_info: public Link
{
public:
    bool hide;
    u_int16_t info_flag;
    u_int32_t dtl_id;

    list<ISCD*>& Iscds () { return iscds; }
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
#define LINK_PARA_FLAG_DRAGON_LAMBDA 0x2000 
#define LINK_PARA_FLAG_WDM_TE_GRID 0x4000 
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
        if (SystemConfig::wdm_subnet_on)
            SystemConfig::should_incorporate_ciena_subnet = false;
        else
            SystemConfig::should_incorporate_ciena_subnet = true;
    }
    string subnetDescription;
    ResourceType routerType;
    ResourceType linkType;
    vector<router_id_info*> routerVector;
    vector<link_info*> linkVector;
};


////////////////////////////////////////////////////////
//////////      Config Subnet Topology from File     ////////////
///////////////////////////////////////////////////////
#define SUBNET_CURRENT_CONFIG_FILE "subnet_ciena.conf"
#define SUBNET_DEFAULT_CONFIG_FILE "/usr/local/dragon/etc/subnet_ciena.conf"

class Subnet_ConfigFile: public SystemConfig, Subnet
{
public:
    Subnet_ConfigFile(string& fileName):SystemConfig(fileName), Subnet("Subnet-PreconfigedFromFile") {}
    virtual ~Subnet_ConfigFile() { }

    virtual void Init(const char* fileName= NULL);
    virtual void ConfigFromFile(ifstream &ifs);
    int ReadConfigDragonLambda(char* buf, char* id, link_info* link);
    int ReadConfigWdmTeGrid(char* buf, char* id, link_info* link);
};

#endif

