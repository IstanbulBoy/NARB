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
#include "log.hh"
#include "resource.hh"
#include "rce_apiclient.hh"
#include "zebra_ospfclient.hh"
#include "narb_apiserver.hh"
#include "narb_apiclient.hh"
#include "narb_config.hh"
#include "dts.hh"
#include "cli.hh"
#include "clicmd.hh"
#include <string>
#ifndef HAVE_DECL_GETOPT
  #define HAVE_DECL_GETOPT 1
#endif
#include <getopt.h>

struct option longopts[] = 
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "port",        no_argument,       NULL, 'p'},
    { "help",        no_argument,       NULL, 'h'},
    { "version",     no_argument,       NULL, 'v'},
    { 0 }
};

#if 0
class Terminator: public Timer
{
public:
    Terminator(): Timer(1, 0, 9) {}
    virtual void Run()
        {
            cout<<" * "<<flush;
            if (repeats == 0)
            {
                cout<< " bye ..." <<endl;
                exit(0);
            }
        }
};
Terminator terminator;
#endif 

ZebraOspfSync * zebra_client = NULL;
DomainTopologyOriginator* dts_originator = NULL;

// SIGINT handler.
void sigint (int sig)
{
    LOG("Terminating on signal"<<endl);
    if (zebra_client)
	delete zebra_client;
    exit(0);
#if 0
    LOG("NARB is cleaning up \n \t Pls. wait a few secnods ");
    // server cleanup
    // cli cleanup
    eventMaster.Schedule(&terminator);
    //close connection to Zebra;
    if (zebra_client)
       zebra_client->Stop();
#endif
}

ConfigFile configMaster;

    
int main( int argc, char* argv[])
{
    int ret;
    //Command line processing
    bool is_daemon = false;
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "df:p:P:vhH:", longopts, 0);
        if (opt == EOF)
            break;

        switch (opt) 
        {
        case 'f':
            SystemConfig::config_file = optarg;
            break;
        case 'd':
            is_daemon = true;
            break;
        default:
            break;
        }
    }

    //System log initilization
    char log_file[20];
    u_int32_t pid = getpid();
    sprintf(log_file, "narb-%d.log", pid);
    Log::Init(LOG_ALL, log_file);
    Log::SetDebug(true);
    LOG("DRAGON NARB Started..."<<endl);

    //Install signal handlers
    signal_init ();

    //Config file processing
    configMaster.ReadConfig((char*)SystemConfig::config_file.c_str(), (char*)NARB_CURRENT_CONFIG_FILE, (char*)NARB_DEFAULT_CONFIG_FILE, NarbDomainInfo);

    //Start NARB API Server
    NARB_APIServer server(NARB_API_PORT);
    server.Start();

    //Start NARB CLI
    CLIServer cli_server(NARB_CLI_PORT);
    cli_server.Start();

    if (NarbDomainInfo.ospfd_inter.addr[0] == 0 || NarbDomainInfo.ospfd_inter.port == 0)
    {
        LOG("Running without connection to OSPFd......" <<endl);
        LOG("No abstract topology generated......"<<endl);
    }
    else
    {
        //Start Zebra OSPF APIClient
        zebra_client = new ZebraOspfSync(NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port, DOMAIN_MASK_GLOBAL, 0);
        if (zebra_client->RunWithoutSyncLsdb() < 0)
        {
            LOGF("communication error with OSPFd %s:%d....\n", NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
            exit(1);
        }
        ZebraOspfWriter* ospf_apiwriter = zebra_client->GetWriter();
        if(!ospf_apiwriter)
        {
            LOGF("cannot create ZebraOspfWriter for OSPFd %s:%d....\n", NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
            exit(2);
        }

        // Register type (10, 1) Opaque-TE LSA.
        ret = ospf_apiwriter->RegisterOpqaueType(10, 1);
        if (ret < 0)
        {
            LOG("RegisterOpqaueType[10, 1] failed !"<<endl);
        }
        //Start abstract domain topology origination    
        NarbDomainInfo.OriginateTopology(ospf_apiwriter);
        dts_originator= new DomainTopologyOriginator(SystemConfig::topology_refresh_interval);
        dts_originator->SetRepeats(FOREVER);
        dts_originator->SetAutoDelete(true);
        dts_originator->SetOspfClient(zebra_client);
        eventMaster.Schedule(dts_originator);
    }

    //Run program as daemon
    if (is_daemon)
        daemon(0, 0);

    //Start main event loop
    eventMaster.Run();
    return 0;
}
