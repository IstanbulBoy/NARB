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

#ifndef __ZEBRA_OSPFCLIENT_H__
#define __ZEBRA_OSPFCLIENT_H__

#include "types.hh"
#include "event.hh"
#include "toolbox.hh"
#include "rce_apiclient.hh" 

#define ZEBRA_OSPF_API_VERSION 1
#define OSPF_MAX_LSA_SIZE 1500

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

#define NARB_ZEBRA_LOCAL_PORT    4100

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

struct zebra_register_opaque_type
{
  u_char lsatype;
  u_char opaquetype;
  u_char pad[2];
};

struct zebra_unregister_opaque_type
{
  u_char lsatype;
  u_char opaquetype;
  u_char pad[2];
};

struct zebra_lsa_originate_request
{
  // Used for LSA type 9 otherwise ignored 
  in_addr ifaddr;
  // Used for LSA type 10 otherwise ignored 
  in_addr area_id;
  // LSA header and LSA-specific part
  lsa_header data;
};

struct zebra_lsa_delete_request
{
  in_addr area_id;	// "0.0.0.0" for AS-external opaque LSAs 
  in_addr adv_router; // $$$$ hacked 
  u_char lsa_type;
  u_char opaque_type;
  u_char pad[2];		// padding
  u_int32_t opaque_id;
};

struct zebra_reply
{
  char errcode;
#define OSPF_API_OK                         0
#define OSPF_API_NOSUCHINTERFACE          (-1)
#define OSPF_API_NOSUCHAREA               (-2)
#define OSPF_API_NOSUCHLSA                (-3)
#define OSPF_API_ILLEGALLSATYPE           (-4)
#define OSPF_API_OPAQUETYPEINUSE          (-5)
#define OSPF_API_OPAQUETYPENOTREGISTERED  (-6)
#define OSPF_API_NOTREADY                 (-7)
#define OSPF_API_NOMEMORY                 (-8)
#define OSPF_API_ERROR                    (-9)
#define OSPF_API_UNDEF                   (-10)
  u_char pad[3];
};

struct zebra_lsa_change_notify
{
  // Used for LSA type 9 otherwise ignored 
  in_addr ifaddr;
  // Area ID. Not valid for AS-External and Opaque11 LSAs
  in_addr area_id;
  u_char is_self_originated;	// 1 if self originated. 
  u_char pad[3];
  lsa_header data;
};

struct zebra_neighbor_count
{
  u_int32_t count;	// number of neighbors
  u_int32_t status;		// neighbor status code
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
    u_int32_t neighbors;
    int attempt;
public:
    ZebraOspfSync(char* host, int port, u_int32_t dmask, int sync_interval): Timer(sync_interval, 0, FOREVER), 
            sync_fd(-1), async_fd(-1), reader(NULL), writer(NULL), ospfd_host(host), ospfd_port(port), domain_mask(dmask),
            neighbors(0), attempt(0) { }
    virtual ~ZebraOspfSync();

    int Connect (char *host, int syncport, int remote_port);
    int Connect() { return Connect(ospfd_host, NARB_ZEBRA_LOCAL_PORT, ospfd_port); }
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
    void Stop()
        {
            if (sync_fd > 0)
                close(sync_fd);
            if (async_fd > 0)
                close(async_fd);
        }
    int RunWithoutSyncLsdb ();

    u_int32_t NeighborCount() {return neighbors; }
    void SetNeighborCount(u_int32_t count) { neighbors = count; }

    u_int32_t DomainMask()  { return domain_mask; }
};

class ZebraOspfReader: public Reader
{
private:
    ZebraOspfSync *server;
    int sync_fd;

public:
    ZebraOspfReader(int fd, ZebraOspfSync *server_ptr): Reader(fd), server(server_ptr), sync_fd(-1)
        {  assert(server_ptr); server_ptr->SetReader(this); }
    virtual ~ZebraOspfReader() {}
    void SetSyncSocket (int sock);
    virtual void Run ();
    zebra_msg * ReadMessage( );
    zebra_msg * ReadSyncMessage ();
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

    int OriginateLsa(in_addr ori_if, in_addr adv_id, in_addr area, u_char lsa_type, 
        u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen);
    int DeleteLsa(in_addr adv_id, in_addr area, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id);
    int RegisterOpqaueType(u_char ltype, u_char otype);
};

/////// Global OSPF TE TLV functions ///////
te_tlv_header * ospf_te_router_addr_tlv_alloc(in_addr addr);
te_tlv_header *ospf_te_link_tlv_alloc(u_char type, in_addr addr);
te_tlv_header * ospf_te_link_subtlv_append(te_tlv_header * tlv_header, u_int16_t type, void *value);
te_tlv_header * ospf_te_link_subtlv_set_swcap_vlan(te_tlv_header * tlv_header, u_char* vtagMaskAvail, u_char* vtagMaskAlloc=NULL);

#endif
