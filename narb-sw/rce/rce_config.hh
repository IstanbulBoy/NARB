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

#ifndef __RCE_CONFIG_HH__
#define __RCE_CONFIG_HH__

using namespace std;
#include <string>
#include <list>

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

typedef struct HomeVlsrRouterPair_type
{
    u_int32_t home_vlsr;
    u_int32_t router_id;
} HomeVlsrRouterPair;

typedef list<HomeVlsrRouterPair> HomeVlsrRouterPairList;

class SystemConfig
{
public:
    static string config_file;
    static string schema_file;

    static string ospfd_inter_host;
    static int ospfd_inter_port;
    static int ospfd_inter_port_local;

    static string ospfd_intra_host;
    static int ospfd_intra_port;
    static int ospfd_intra_port_local;

    static string terce_host;
    static int terce_port;
    static int terce_port_local;

    static int ospf_sync_interval;
    static int max_ospf_sync_attempts;

    static int delta_expire_query;
    static int delta_expire_reserve;

    static string narb_host;
    static int narb_port;

    static int api_port;
    static int cli_port;
    static string cli_password;
    static string cli_address;
    static int cli_timeout;
    
    static int metric_interdomain_nice_incremental;

    static u_int32_t domainMask;
    static u_int32_t domainId;
    static string sysDescription;

    static string subnet_file;
    static bool  should_incorporate_subnet;

    static HomeVlsrRouterPairList home_vlsr_search_list;

    static void AddHomeVlsrRouterIdPair(u_int32_t home_vlsr, u_int32_t router_id);
    static u_int32_t FindHomeVlsrByRouterId(u_int32_t router_id);
};

#endif
