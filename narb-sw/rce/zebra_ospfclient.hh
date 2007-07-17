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

#ifndef __RCE_ZEBRA_OSPFCLIENT_H__
#define __RCE_ZEBRA_OSPFCLIENT_H__

#include "rce_types.hh"
#include "event.hh"
#include "toolbox.hh"
#include "rce_api.hh"
#include "rce_lsa.hh"
#include "rce_apiserver.hh"

#define ZEBRA_OSPF_API_VERSION 1

// Message tyes to OSPF daemon.
#define MSG_ZEBRA_REGISTER_OPAQUETYPE   1
#define MSG_ZEBRA_UNREGISTER_OPAQUETYPE 2
#define MSG_ZEBRA_REGISTER_EVENT        3
#define MSG_ZEBRA_SYNC_LSDB             4
#define MSG_ZEBRA_ORIGINATE_REQUEST     5
#define MSG_ZEBRA_DELETE_REQUEST        6
#define MSG_ZEBRA_NEIGHBOR_COUNT_REQUEST     7

// Message types from OSPF daemon.
#define MSG_ZEBRA_REPLY                10
#define MSG_ZEBRA_READY_NOTIFY         11
#define MSG_ZEBRA_LSA_UPDATE_NOTIFY    12
#define MSG_ZEBRA_LSA_DELETE_NOTIFY    13
#define MSG_ZEBRA_NEW_IF               14
#define MSG_ZEBRA_DEL_IF               15
#define MSG_ZEBRA_ISM_CHANGE           16
#define MSG_ZEBRA_NSM_CHANGE           17
#define MSG_ZEBRA_NEIGHBOR_COUNT   18

#define RCE_ZEBRA_LOCAL_PORT    4000

struct zebra_msg_header
{
  u_char version;
  u_char msgtype;
  u_int16_t msglen;
  u_int32_t msgseq;
};

struct zebra_msg
{
    zebra_msg_header hdr;
    char* body;
};

struct zebra_lsa_filter
{
  u_int16_t typemask;		// bitmask for selecting LSA types (1..16)
  u_char origin;		// selects according to origin.
#define NON_SELF_ORIGINATED	0
#define	SELF_ORIGINATED  (OSPF_LSA_SELF)
#define	ANY_ORIGIN 2
  u_char num_areas;		// number of areas in the filter. 
};

struct zebra_lsa_change_notify
{
  // Used for LSA type 9 otherwise ignored 
  struct in_addr ifaddr;
  // Area ID. Not valid for AS-External and Opaque11 LSAs
  struct in_addr area_id;
  u_char is_self_originated;	// 1 if self originated. 
  u_char pad[3];
  lsa_header data;
};


class ZebraOspfReader;
class ZebraOspfWriter;
class ZebraOspfSync: public Timer
{
private:
    ZebraOspfReader* reader;
    ZebraOspfWriter* writer;
    int sync_fd;
    int async_fd;
    char* ospfd_host;
    int ospfd_port;
    u_int32_t domain_mask;
    int attempt;
public:
    ZebraOspfSync(char* host, int port, u_int32_t dmask, int sync_interval): Timer(sync_interval, 0, FOREVER),
            sync_fd(-1), async_fd(-1), reader(NULL), writer(NULL), ospfd_host(host), ospfd_port(port), domain_mask(dmask),
            attempt(0) { }
    virtual ~ZebraOspfSync();

    int Connect (char *host, int syncport, int remote_port);
    bool Alive() 
        { extern int module_connectable(char * host, int port); 
            return module_connectable (ospfd_host, ospfd_port); }
    ZebraOspfReader * GetReader() {  return reader;}
    ZebraOspfWriter * GetWriter() {  return writer;}
    void SetReader(ZebraOspfReader *reader_ptr) {  reader = reader_ptr;}
    void SetWriter(ZebraOspfWriter *writer_ptr) {  writer = writer_ptr;}
    void SetSyncFd(int x) { sync_fd = x; }
    void SetAsyncFd(int x) { async_fd = x; }
    void SetAttemptNum(int x) { attempt = x; }
    
    virtual void Run();
    void Exit();
    u_int32_t DomainMask()  { return domain_mask; }
};

class ZebraOspfReader: public Reader
{
private:
    ZebraOspfSync *server;
public:
    ZebraOspfReader(int fd, ZebraOspfSync *server_ptr): Reader(fd), server(server_ptr) 
        {  assert(server_ptr); server_ptr->SetReader(this); }
    virtual ~ZebraOspfReader() {}
    virtual void Run ();
    zebra_msg * ReadMessage( );
    void HandleMessage (zebra_msg *msg);
};

class ZebraOspfWriter: public Writer
{
private:
    list<zebra_msg *> msgQueue;
    ZebraOspfSync *server;
public:
    ZebraOspfWriter(int fd, ZebraOspfSync *server_ptr): Writer(fd), server(server_ptr) 
            {  assert(server_ptr); server_ptr->SetWriter(this); }
    virtual ~ZebraOspfWriter() {}
    virtual void Run ();
    int WriteMessage(zebra_msg *msg);
    void PostMessage(zebra_msg *msg);
};

#endif
