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
#include "radixtree.hh"
#include "event.hh"
#include "rce_log.hh"
#include "resource.hh"
#include "rce_api.hh"
#include "rce_apiserver.hh"
#include "zebra_ospfclient.hh"
#include "rce_schema.hh"
#include "rce_subnet.hh"
#include "rce_cli.hh"
#include "rce_clicmd.hh"
#include "toolbox.hh"
#include <string>

 
#ifndef HAVE_DECL_GETOPT
  #define HAVE_DECL_GETOPT 1
#endif
#include <getopt.h>

struct option longopts[] = 
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "schema_file", required_argument, NULL, 's'},
    { "subnet_file", required_argument, NULL, 'c'},
    { "cli_port",        required_argument,       NULL, 'p'},
    { "api_port",        required_argument,       NULL, 'P'},
    { "help",        no_argument,       NULL, 'h'},
    { 0 }
};

void usage()
{
    cout<<"RCE  Usage:"<<endl;
    cout<<"\t rce [-d] (daemon)  [-f config_file] [-s schema_file] [-c subnet_topology_file]  [-p CLI port] [-P API port] [-h] (help)" <<endl;
}

ZebraOspfSync *zebra_client_inter = NULL;
ZebraOspfSync *zebra_client_intra = NULL;

int main( int argc, char* argv[])
{
    bool is_daemon = false;

    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "ds:f:c:p:P:h", longopts, 0);
        if (opt == EOF)
            break;

        switch (opt) 
        {
        case 'f':
            SystemConfig::config_file = optarg;
            break;
        case 's':
            SystemConfig::schema_file = optarg;
            break;
        case 'c':
            SystemConfig::subnet_file = optarg;
            break;
        case 'd':
            is_daemon = true;
            break;
        case 'p':
            SystemConfig::cli_port = atoi(optarg);
            break;
        case 'P':
            SystemConfig::api_port = atoi(optarg);
            break;
        case 'h':
        default:
            usage();
            break;
        }
    }
    
    signal_init ();

    char log_file[20];
    //u_int32_t pid = getpid();
    //sprintf(log_file, "rce-%d.log", pid);    
    sprintf(log_file, "/var/log/rce.log");    
    Log::Init(LOG_ALL, log_file);
    Log::SetDebug(true);
    LOG("DRAGON RCE Started..."<<endl);

    ResourceSchema rsd(0);
    rsd.Init((char*)SystemConfig::schema_file.c_str());

    Subnet_ConfigFile subnetTopoIntra(SystemConfig::subnet_file);
    subnetTopoIntra.Init();

    ZebraOspfSync ospfSyncIntra((char*)SystemConfig::ospfd_intra_host.c_str(), SystemConfig::ospfd_intra_port, 
            DOMAIN_MASK_LOCAL, SystemConfig::ospf_sync_interval);
    zebra_client_intra = &ospfSyncIntra;
    eventMaster.Schedule(&ospfSyncIntra);
    ospfSyncIntra.Run();
    ZebraOspfSync ospfSyncInter((char*)SystemConfig::ospfd_inter_host.c_str(), SystemConfig::ospfd_inter_port, 
            DOMAIN_MASK_GLOBAL, SystemConfig::ospf_sync_interval);
    zebra_client_inter = &ospfSyncInter;
    eventMaster.Schedule(&ospfSyncInter);
    ospfSyncInter.Run();

    APIServer server(SystemConfig::api_port);
    server.Start();

    CLIServer cli_server(SystemConfig::cli_port);
    cli_server.Start();

    if (is_daemon)
        daemon(0, 0);

    eventMaster.Run();
    return 0;
}
