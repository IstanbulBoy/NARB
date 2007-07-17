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

string SystemConfig::config_file("rce.conf");
string SystemConfig::schema_file("schema_combo.rsd");

string SystemConfig::ospfd_inter_host("localhost");
int SystemConfig::ospfd_inter_port = 2607;
int SystemConfig::ospfd_inter_port_local = 4607;

string SystemConfig::ospfd_intra_host("localhost");
int SystemConfig::ospfd_intra_port = 2617;
int SystemConfig::ospfd_intra_port_local = 4617;

//To be configured from CLI ?
string SystemConfig::terce_host("localhost");
int SystemConfig::terce_port = TERCE_API_SERVER_PORT;
int SystemConfig::terce_port_local = RCE_TERCE_SYNC_PORT;

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

string SystemConfig::subnet_file("subnet_ciena.conf");
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
