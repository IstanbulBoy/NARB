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
#ifndef __NARB_CONFIG_HH__
#define  __NARB_CONFIG_HH__
using namespace std;
#include "dts.hh"
#include <string>
#include <iostream>
#include <list>

////////////////////////////////////////////////////////
//////////////    GLOBAL CONFIG OPTIONS /////////////////
////////////////////////////////////////////////////////


#define NARB_API_PORT 2609
#define NARB_CLI_PORT 2626
#define NARB_XML_PORT 2627

//Default auto-generated virtual link metrics, forcing through-traffic to use border routers
#define HOST_BORDER_METRIC 5
#define BORDER_BORDER_METRIC 3

// define the narb routing modes
enum narb_routing_mode {
  RT_MODE_ALL_STRICT_ONLY = 0x01,
  RT_MODE_MIXED_ALLOWED,
  RT_MODE_MIXED_PREFERRED,
  RT_MODE_ALL_LOOSE_ALLOWED
};

enum narb_working_mode {
  WORKING_MODE_SINGLE_DOMAIN = 0x01,
  WORKING_MODE_MULTI_DOMAIN
};

// The types of wrapped RCE-Request TLV 
#define LSP_TLV_NARB_CSPF_REQ    0x0001
#define LSP_OPT_REQ_ALL_VTAGS    0x0002 //Ask RCE to return a list of available vlan tags instead of just one.
#define LSP_OPT_REQ_ALL_WAVES   0x0004  //Ask RCE to return a list of available wavelengths instead of just one.

// Indicating layers excluded from path computation
#define LSP_OPT_EXCLUD_L1      0x0010
#define LSP_OPT_EXCLUD_TDM   0x0020
#define LSP_OPT_EXCLUD_L2      0x0040
#define LSP_OPT_EXCLUD_L3      0x0080

// LSP computation options
#define LSP_OPT_STRICT  ((u_int32_t)(0x0001 << 16)) //otherwise LSP_OPT_LOOSE
#define LSP_OPT_PREFERRED ((u_int32_t)(0x0002 << 16)) //otherwise LSP_OPT_ONLY
#define LSP_OPT_MRN ((u_int32_t)0x0004 << 16)  //otherwise SINGLE_REGION
//#define LSP_OPT_MRN_RELAY ((u_int32_t)0x08 << 16)  //relay path computation for multiregion multidomain networks
#define LSP_OPT_BIDIRECTIONAL ((u_int32_t)0x0010 << 16)  // otherwise UNIDIRECTIONAL
#define LSP_OPT_E2E_VTAG  ((u_int32_t)(0x0020 << 16)) //otherwise Untgged VLAN for E2E Ethernet
#define LSP_OPT_VIA_MOVAZ  ((u_int32_t)(0x0040 << 16)) //using MOVAZ proprietary TLVs for optical layer routing
#define LSP_OPT_VTAG_MASK  ((u_int32_t)(0x0080 << 16)) //Privodes a 512 bytes vtag mask to constrain the available taggs

class SystemConfig
{
public:
    static string config_file;
    static narb_routing_mode routing_mode;
    static narb_working_mode working_mode;
    static u_int32_t rce_options;
    static bool forced_merge;
    static bool zcompress;

    static string rce_pri_host;
    static int rce_pri_port;

    static string rce_sec_host;
    static int rce_sec_port;

    static int narb_ospfd_local_port_inter;
    static int narb_ospfd_local_port_intra;

    static DomainInfo narb_domain_info;
 
    static int topology_refresh_interval;

    static string cli_password;
    static string cli_address;
    static int cli_timeout;

    static bool use_manual_ero;
    static list<ero_subobj*> manual_ero;
};

#define NarbDomainInfo (SystemConfig::narb_domain_info)

////////////////////////////////////////////////////////
//////////////     CONFIG FILE PROCESSING ////////////////
////////////////////////////////////////////////////////
#define NARB_CURRENT_CONFIG_FILE "narb.conf"
#define NARB_DEFAULT_CONFIG_FILE "/usr/local/etc/narb.conf"
#define NARB_BUFSIZ 0x10000
#define NARB_BLKSIZ 0x1000
#define NARB_LINESIZ 0x800
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

#define DEFAULT_OSPFD_INTER_PORT 2607
#define DEFAULT_OSPFD_INTRA_PORT 2617

// definitions of codes for configuration blocks
enum config_code {
  CONFIG_END = 0,
  CONFIG_INTER_DOMAIN_ODPFD = 1,
  CONFIG_INTRA_DOMAIN_ODPFD,
  CONFIG_ROUTER,
  CONFIG_LINK,
  CONFIG_TE_PROFILE,
  CONFIG_AUTO_LINK,
  CONFIG_INTER_DOMAIN_TE_LINK,
  CONFIG_CLI,
  CONFIG_DOMAIN_ID,
  CONFIG_UNKNOWN
};

class ConfigFile
{
public:
    static string fileName;

    int ReadConfig(char *config_file, char *config_current_dir,	char *config_default_dir, DomainInfo & p_domain_info);
    void ConfigFromFile(ifstream &ifs , DomainInfo & domain_info);
    int ReadConfigBlock(char *buf, char * header, char * body, char ** next);
    int ReadConfigParameter(char * buf, char * id, char * fmt, void * parameter);
    int ReadConfigVlanTagSet(char * buf, char * id, u_char* vtagMask);

};


#endif
 
