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

#ifndef __TERCE_APICLIENT_H__
#define __TERCE_APICLIENT_H__

#include "rce_types.hh"
#include "event.hh"
#include "toolbox.hh"
#include "zebra_ospfclient.hh" 

#define TERCE_API_SERVER_PORT 2690
#define RCE_TERCE_SYNC_PORT    (TERCE_API_SERVER_PORT+4) //2094
#define RCE_TERCE_ASYNC_PORT  (RCE_TERCE_SYNC_PORT+1) //2095

// Message types in addition to existing NARB/RCE API
#define MSG_TERCE_TOPO_SYNC ((u_int8_t)0x11)
#define MSG_TERCE_TOPO_ASYNC ((u_int8_t)0x12)

struct terce_topo_update
{
    u_int16_t type;
    u_int16_t length;
    struct lsa_header lsa_hdr;
    char lsa_body[1];
};

class TerceApiTopoReader;
class TerceApiTopoWriter;
class TerceApiTopoSync: public Timer
{
private:
    TerceApiTopoReader* reader;
    TerceApiTopoWriter* writer;
    int sync_fd;
    int async_fd;
    char* terce_host;
    int terce_port;
    int attempt;
    u_int32_t domain_id;
    u_int32_t domain_mask;
    bool api_ready;

public:
    TerceApiTopoSync(char* host, int port, u_int32_t dmid, int sync_interval): Timer(sync_interval, 0, FOREVER), 
        sync_fd(-1), async_fd(-1), reader(NULL), writer(NULL), terce_host(host), terce_port(port), attempt(0), 
        domain_id(dmid), domain_mask(DOMAIN_MASK_GLOBAL), api_ready(false) { }
    virtual ~TerceApiTopoSync();

    int Connect (char *host, int syncport, int remote_port);
    int Connect() { return Connect(terce_host, RCE_TERCE_SYNC_PORT, terce_port); }
    int Accept (int asyncport);
    int Accept () { return Accept(RCE_TERCE_ASYNC_PORT); }
    bool Alive() { extern int module_connectable(char * host, int port); 
            return module_connectable (terce_host, terce_port); }
    TerceApiTopoReader * GetReader() {  return reader;}
    TerceApiTopoWriter * GetWriter() {  return writer;}
    void SetReader(TerceApiTopoReader *reader_ptr) {  reader = reader_ptr;}
    void SetWriter(TerceApiTopoWriter *writer_ptr) {  writer = writer_ptr;}
    void SetSyncFd(int x) { sync_fd = x; }
    void SetAsyncFd(int x) { async_fd = x; }
    void SetAttemptNum(int x) { attempt = x; }
    u_int32_t DomainId()  { return domain_id; }
    void Stop()
        {
            if (sync_fd > 0)
                close(sync_fd);
            if (async_fd > 0)
                close(async_fd);
        }

    virtual int RunWithoutSyncTopology();
    virtual void Run();
    void KeepAlive();
    void InitRceTerceComm();
    bool RceTerceApiReady() { return api_ready; }
};

class TerceApiTopoReader: public Reader
{
private:
    TerceApiTopoSync *server;
    int sync_fd;

public:
    TerceApiTopoReader(int fd, TerceApiTopoSync *server_ptr): Reader(fd), server(server_ptr), sync_fd(-1)
        {  assert(server_ptr); server_ptr->SetReader(this); }
    virtual ~TerceApiTopoReader() {}
    void SetSyncSocket (int sock);
    virtual void Run ();
    api_msg * ReadMessage( ); // <==> HandleAsyncMessage
    void HandleMessage (api_msg *msg); // <==> HandleAsyncMessage
    api_msg * ReadSyncMessage ();
};

class TerceApiTopoWriter: public Writer
{
private:
    list<api_msg *> msgQueue;
    TerceApiTopoSync *server;
public:
    TerceApiTopoWriter(int fd, TerceApiTopoSync *server_ptr): Writer(fd), server(server_ptr) 
            {  assert(server_ptr); server_ptr->SetWriter(this); }
    virtual ~TerceApiTopoWriter() {}
    virtual void Run ();
    int WriteMessage(api_msg *msg);
    void PostMessage(api_msg *msg);
    int OriginateLsa(in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen);
    int DeleteLsa(in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id);
    int UpdateLsa( in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen);
};

class TerceApiTopoOriginator: public Timer
{
private:
    TerceApiTopoSync * terce_client;

public:
    TerceApiTopoOriginator(int interval): Timer(interval, 0), terce_client(NULL) { }
    virtual ~TerceApiTopoOriginator() { }
    void SetTerceClient(TerceApiTopoSync* tc) { terce_client = tc; }
    virtual void Run();

    int OriginateTopology ();
    int DeleteTopology ();

    int OriginateRouterId (RouterId* rtid);
    int OriginateTeLink (Link* link);
    int UpdateTeLink (Link* link);

    void* BuildRouterIdOpaqueData(RouterId* rtid);
    void* BuildTeLinkOpaqueData(Link* link);
};

#endif

