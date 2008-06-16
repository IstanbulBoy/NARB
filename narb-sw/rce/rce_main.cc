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
#include "radixtree.hh"
#include "event.hh"
#include "rce_log.hh"
#include "resource.hh"
#include "rce_api.hh"
#include "rce_apiserver.hh"
#include "zebra_ospfclient.hh"
#include "terce_apiclient.hh"
#include "rce_schema.hh"
#include "rce_subnet.hh"
#include "rce_cli.hh"
#include "rce_clicmd.hh"
#include "toolbox.hh"
#include <stdlib.h>
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
    cout<<"\t rce [-d] (daemon)  [-f config_file] [-p CLI port] [-P API port] [-h] (help)" <<endl;
}

ZebraOspfSync *zebra_client_inter = NULL;
ZebraOspfSync *zebra_client_intra = NULL;
TerceApiTopoSync *terce_client = NULL;
TerceApiTopoOriginator* topo_originator = NULL;

int main( int argc, char* argv[])
{
    bool is_daemon = false;
    bool has_config_file = false;

    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "ds:f:p:P:h", longopts, 0);
        if (opt == EOF)
            break;

        switch (opt) 
        {
        case 'f':
            SystemConfig::config_file = optarg;
            has_config_file = true;
            break;
        case 's':
            SystemConfig::schema_file = optarg;
            has_config_file = true;
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
    LOG(endl<<endl<<"#####################"<<endl
        <<"DRAGON RCE Started..."<<endl
        <<"#####################"<<endl<<endl);

    if (has_config_file)
    {
        SystemConfig systemConfigFileHandler(SystemConfig::config_file);
        systemConfigFileHandler.Init();
    }

    if (SystemConfig::schema_file.size() == 0)
        LOG(endl<<"#### TEDB schema file must be present (configured with -s cmdline option or in 'rce.conf')...  "<<endl);
    ResourceSchema rsd(0);
    rsd.Init((char*)SystemConfig::schema_file.c_str());

    if (SystemConfig::subnet_file.size() > 0)
    {
        Subnet_ConfigFile subnetTopoIntra(SystemConfig::subnet_file);
        subnetTopoIntra.Init();
    }

    ZebraOspfSync ospfSyncIntra((char*)SystemConfig::ospfd_intra_host.c_str(), SystemConfig::ospfd_intra_port, 
            DOMAIN_MASK_LOCAL, SystemConfig::ospf_sync_interval);
    zebra_client_intra = &ospfSyncIntra;
    eventMaster.Schedule(zebra_client_intra);
    zebra_client_intra->Run();

    ZebraOspfSync ospfSyncInter((char*)SystemConfig::ospfd_inter_host.c_str(), SystemConfig::ospfd_inter_port, 
            DOMAIN_MASK_GLOBAL, SystemConfig::ospf_sync_interval);
    zebra_client_inter = &ospfSyncInter;
    eventMaster.Schedule(zebra_client_inter);
    zebra_client_inter->Run();

    TerceApiTopoSync terceClient((char*)SystemConfig::terce_host.c_str(), SystemConfig::terce_port, DOMAIN_MASK_LOCAL, 0);
    if (SystemConfig::terce_host.size() > 0 && SystemConfig::terce_port > 0)
    {
        LOG("Initiating RCE-TERCE API communication......" <<endl);
        terce_client = &terceClient;
        if (terce_client->RunWithoutSyncTopology() < 0)
        {
            LOGF("TerceApiTopoSync failed to start: API server not ready (%s:%d)....\n", SystemConfig::terce_host.c_str(), SystemConfig::terce_port);
        }
        while (!terce_client->RceTerceApiReady())
        {
            LOGF("RCE-TERCE API server (%s:%d) is not ready\n\t... wait 10 seconds...\n", SystemConfig::terce_host.c_str(), SystemConfig::terce_port);
            sleep(10);
            terce_client->RunWithoutSyncTopology();
        }

        //Start abstract domain topology origination via TERCE
        if (topo_originator == NULL)
        {
            topo_originator= new TerceApiTopoOriginator(SystemConfig::terce_topo_sync_interval);
            topo_originator->SetRepeats(FOREVER);
            topo_originator->SetAutoDelete(true);
        }
        topo_originator->SetTerceClient(terce_client);
        topo_originator->OriginateTopology();
        eventMaster.Schedule(topo_originator);
    }

    APIServer server(SystemConfig::api_port);
    server.Start();

    CLIServer cli_server(SystemConfig::cli_port);
    cli_server.Start();

    if (is_daemon)
        daemon(0, 0);

    eventMaster.Run();
    return 0;
}

