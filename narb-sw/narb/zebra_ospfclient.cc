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

#include "types.hh"
#include "log.hh"
#include "rce_apiclient.hh"
#include "lsa.hh"
#include "event.hh"
#include "toolbox.hh"
#include "zebra_ospfclient.hh"
#include <errno.h>
#include <vector>

zebra_msg * zebra_msg_new (u_char msgtype, void *msgbody, u_int32_t seqnum, u_int16_t msglen)
{
  zebra_msg* msg;

  msg = (zebra_msg*)malloc(sizeof(zebra_msg));
  memset (msg, 0, sizeof (zebra_msg));

  msg->hdr.version = ZEBRA_OSPF_API_VERSION;
  msg->hdr.msgtype = msgtype;
  msg->hdr.msglen = htons (msglen);
  msg->hdr.msgseq = htonl (seqnum);

 int len = (int)msglen;
  msg->body= (char*)malloc(len);
  memcpy (msg->body, msgbody, msglen);

  return msg;
}

void zebra_msg_delete (zebra_msg *msg)
{
    assert (msg && msg->body);
    free(msg->body);
    free(msg);
}

int ZebraOspfSync::Connect (char *host, int syncport, int remote_port)
{
    sockaddr_in myaddr_sync, myaddr_async, peeraddr;
    int size = 0, on = 1, sock_accept, ret;
    socklen_t peeraddrlen;
    hostent *hp;

    /* There are two connections between the client and the server.
     First the client opens a connection for synchronous requests/replies 
     to the server. The server will accept this connection and  as a reaction
     open a reverse connection channel for asynchronous messages. */
    sock_accept = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_accept < 0)
    {
      LOG_CERR<< "ZebraOspfSync::Connect: creating async socket failed\n" <<endl;
      return -1;
    }

    /* Prepare socket for asynchronous messages */
    /* Initialize async address structure */
    memset (&myaddr_async, 0, sizeof (sockaddr_in));
    myaddr_async.sin_family = AF_INET;
    myaddr_async.sin_addr.s_addr = htonl (INADDR_ANY);
    myaddr_async.sin_port = htons (syncport+1);
    size = sizeof (sockaddr_in);
    //myaddr_async.sin_len = size;

    /* This is a server socket, reuse addr and port */
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: SO_REUSEADDR failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: SO_REUSEPORT failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#endif

    /* Bind socket to address structure */
    ret = bind (sock_accept, (sockaddr *) &myaddr_async, size);
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: bind async socket failed\n"<<endl;
      close (sock_accept);
      return -1;
    }

    /* Wait for reverse channel connection establishment from server */
    ret = listen (sock_accept, 4);
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: listen :" << strerror (errno);
      close (sock_accept);
      return -1;
    }

    /* Make connection for synchronous requests and connect to server */
    /* Resolve address of server */
    hp = gethostbyname (host);
    if (!hp)
    {
      LOG_CERR<< "ZebraOspfSync::Connect: no such host " << host <<endl;
      close (sock_accept);
      return -1;
    }

    sync_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sync_fd < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: creating sync socket failed" <<endl;
      close (sock_accept);
      return -1;
    }

    /* Reuse addr and port */
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "ZebraOspfSync::Connect: SO_REUSEADDR failed" <<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "ZebraOspfSync::Connect: SO_REUSEPORT failed" <<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }
#endif

    /* Bind sync socket to address structure. This is needed since we
     want the sync port number on a fixed port number. The reverse
     async channel will be at this port+1 */
    memset (&myaddr_sync, 0, sizeof (sockaddr_in));
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons (syncport);
    //myaddr_sync.sin_len = sizeof (sockaddr_in);

    ret = bind (sync_fd, (sockaddr *) &myaddr_sync, size);
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: bind sync socket failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    /* Prepare address structure for connect */
    memcpy (&myaddr_sync.sin_addr, hp->h_addr, hp->h_length);
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons(remote_port);
    //myaddr_sync.sin_len = sizeof (sockaddr_in);

    /* Now establish synchronous channel with OSPF daemon */
    ret = connect (sync_fd, (sockaddr *) &myaddr_sync, sizeof (sockaddr_in));
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: sync connect failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    /* Accept reverse connection */
    peeraddrlen = sizeof (sockaddr_in);
    memset (&peeraddr, 0, peeraddrlen);
    async_fd = accept (sock_accept, (sockaddr *) &peeraddr, &peeraddrlen);
    if (async_fd < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: accept async failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    /* Server socket is not needed anymore since we are not accepting more 
     connections */
    close (sock_accept);

    return 0;
}

int ZebraOspfSync::RunWithoutSyncLsdb ()
{
    if (sync_fd < 0)
    {
        if (Connect(ospfd_host, NARB_ZEBRA_LOCAL_PORT, ospfd_port) < 0)
        {
            LOGF("Check OSPFd %s:%d\n", ospfd_host, ospfd_port);
            SetObsolete(true);
            SetRepeats(0);
            return -1;
        }
        assert(sync_fd > 0 && async_fd > 0);
    }
    if (reader == NULL)
    {
        reader = new ZebraOspfReader(async_fd, this);
        reader->SetRepeats(FOREVER);
        reader->SetAutoDelete(true);
        reader->SetSyncSocket(sync_fd);
        eventMaster.Schedule(reader);
    }
    if (writer == NULL)
    {
        writer = new ZebraOspfWriter(sync_fd, this);
        writer->SetRepeats(0);
        writer->SetAutoDelete(false);
        //eventMaster.Schedule(writer);
    }

    return 0;
}

void ZebraOspfSync::Run ()
{
    if (sync_fd < 0)
    {
        if (attempt++ >= 2)
        {
            SetObsolete(true);
            SetRepeats(0);
        }
        if (Connect(ospfd_host, NARB_ZEBRA_LOCAL_PORT, ospfd_port) < 0)
        {
            LOGF("Attempt %d/3 ### Check OSPFd %s:%d ###\n", attempt, ospfd_host, ospfd_port);
            return;
        }
        assert(sync_fd > 0 && async_fd > 0);
    }
    if (reader == NULL)
    {
        reader = new ZebraOspfReader(async_fd, this);
        reader->SetRepeats(FOREVER);
        reader->SetAutoDelete(true);
        eventMaster.Schedule(reader);
    }
    if (writer == NULL)
    {
        writer = new ZebraOspfWriter(sync_fd, this);
        writer->SetRepeats(0);
        writer->SetAutoDelete(false);
        //scheduled by postMessage()
    }

    //Post an Sync Msg ospfd
  zebra_lsa_filter lsa_filter;


  lsa_filter.typemask = htons(0x200);	// Type-10 Opauqe LSAs only
  lsa_filter.origin = ANY_ORIGIN;
  lsa_filter.num_areas = 0;		// all Areas.
  int len = sizeof (lsa_filter) + lsa_filter.num_areas * sizeof (in_addr);
  
  zebra_msg* zmsg = zebra_msg_new(MSG_ZEBRA_SYNC_LSDB, &lsa_filter, get_narb_seqnum(), len);
  writer->PostMessage(zmsg);
}

ZebraOspfSync::~ZebraOspfSync ()
{  
    if(reader) 
    {
        reader->Close();
        delete reader;
    }
    if(writer)
    {
        writer->Close();
        delete writer;
    }

    if (sync_fd > 0) close(sync_fd);
    if (async_fd > 0) close(async_fd); 
}

  
void ZebraOspfReader::Run()
{
    //read an Zebra OSPF API message
    assert (fd > 0);
    zebra_msg * msg = ReadMessage();

    //something is wrong with socket read
    if (!msg)
    {
        //closing connection
        LOG("Connection closed"<<endl);
        Close();
        server->GetWriter()->Close();
        server->SetReader(NULL);
        server->SetWriter(NULL);
        server->SetSyncFd(-1);
        server->SetAttemptNum(0);
        return;
    }

    //zebra_msg_dump(msg);

    //Message handler    
    HandleMessage(msg);
}

void ZebraOspfReader::SetSyncSocket (int sock)
{
    assert(sock > 0);
    sync_fd = sock;
}

zebra_msg * ZebraOspfReader::ReadMessage ()
{
    zebra_msg* msg = NULL;
    zebra_msg_header hdr;
    static char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    /* Read message header */
    rlen = readn (fd, (char *) &hdr, sizeof (zebra_msg_header));

    if (rlen < 0)
    {
        LOG("ZebraOspfReader / Async failed to read from"<<fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOG("Connection closed for ZebraOspfReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

        close(fd);
        fd = -1;
        if (sync_fd > 0)
        {
            close(sync_fd);
            sync_fd = -1;
        }
        return NULL;
    }
    else if (rlen != sizeof (zebra_msg_header))
    {
        LOG("ZebraOspfReader / Async (" << fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("ZebraOspfReader / Async (" << fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("ZebraOspfReader / Async failed to read from" << fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
            LOG("Connection closed for ZebraOspfReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

            close(fd);
            fd = -1;
            if (sync_fd > 0)
            {
                close(sync_fd);
                sync_fd = -1;
            }
            return NULL;
        }
        else if (rlen != bodylen)
        {
             LOG("ZebraOspfReader / Async (" << fd << ") cannot read the message body" <<endl);
            return NULL;
        }
    }

    // Allocate new message
    msg = zebra_msg_new (hdr.msgtype, buf, ntohl (hdr.msgseq), ntohs (hdr.msglen));
    return msg;
}

zebra_msg * ZebraOspfReader::ReadSyncMessage ()
{
    zebra_msg* msg = NULL;
    zebra_msg_header hdr;
    static char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    /* Read message header */
    rlen = readn (sync_fd, (char *) &hdr, sizeof (zebra_msg_header));

    if (rlen < 0)
    {
        LOG("ZebraOspfReader / Sync failed to read from"<<sync_fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOG("Connection closed for ZebraOspfReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

        close(sync_fd);
        sync_fd = -1;
        if (fd > 0)
        {
            close(fd);
            fd = -1;
        }
        return NULL;
    }
    else if (rlen != sizeof (zebra_msg_header))
    {
        LOG("ZebraOspfReader /Sync(" << sync_fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("ZebraOspfReader / Sync(" << sync_fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (sync_fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("ZebraOspfReader failed to read from / Sync" << sync_fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
            LOG("Connection closed for ZebraOspfReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

            close(sync_fd);
            sync_fd = -1;
            if (fd > 0)
            {
                close(fd);
                fd = -1;
            }
        }
        else if (rlen != bodylen)
        {
             LOG("ZebraOspfReader / Sync(" << sync_fd << ") cannot read the message body" <<endl);
            return NULL;
        }
    }

    // Allocate new message
    msg = zebra_msg_new (hdr.msgtype, buf, ntohl (hdr.msgseq), ntohs (hdr.msglen));
    return msg;
}

void ZebraOspfReader::HandleMessage (zebra_msg *msg)
{
    LSAHandler * lsaEvent;
    u_int32_t ack_id = 0x0f;
    zebra_lsa_change_notify* msg_notify;

    switch (msg->hdr.msgtype)
    {
    case MSG_ZEBRA_LSA_UPDATE_NOTIFY:
    case MSG_ZEBRA_LSA_DELETE_NOTIFY:
        // Ignoring the OSPF LSA updates/deletes from other domains
        // NARB only cares about the LSAs it originates and has been keeping status of those LSAs.
        zebra_msg_delete(msg);
        break;
    case MSG_ZEBRA_NEIGHBOR_COUNT:
        {
            zebra_neighbor_count * neighbor_count = (zebra_neighbor_count *)msg->body;
            assert(server);
            server->SetNeighborCount(neighbor_count->count);
            zebra_msg_delete(msg);
        }
        break;
    case MSG_ZEBRA_REPLY:
        {
            zebra_reply * reply = (zebra_reply*)msg->body;
            LOG("Zebra OSPFd returned code: " << reply->errcode << endl);
            zebra_msg_delete(msg);
        }
        break;
    case MSG_ZEBRA_READY_NOTIFY:
        {
            zebra_reply * reply = (zebra_reply*)msg->body;
            //LOG("Zebra OSPFd is ready for LSA origination. " << reply->errcode << endl);
            zebra_msg_delete(msg);
        }
        break;
    default:
        LOG("Unkonwn/Unprocessed zebra_msg type " << (int)msg->hdr.msgtype << endl);
        zebra_msg_delete(msg);
    }    
}

void ZebraOspfWriter::Run()
{
    Writer::Run();

    //write an API message
    if (fd <= 0)
    {
        Close();
        return;
    }
    
    if (msgQueue.empty())
        return;

    zebra_msg * msg = (zebra_msg*)msgQueue.front();
    assert (msg);
    msgQueue.pop_front();
    int ret = WriteMessage(msg); //msg is deleted in WriteMessage

    //something is wrong with socket write
    if (ret < 0)
    {
        //closing connection.
        LOG("Connection closed"<<endl);
        Close();
        server->GetReader()->Close();
        server->SetReader(NULL);
        server->SetWriter(NULL);
        server->SetSyncFd(-1);
        server->SetAttemptNum(0);
        return;
    }        
    if (msgQueue.size() > 0)
    {
        this->SetObsolete(false);
        this->SetRepeats(1);
    }
}

int ZebraOspfWriter::WriteMessage (zebra_msg *msg)
{
    char buf[API_MAX_MSG_SIZE];

    assert (msg);
    assert (msg->body);

    // Length of message including header
    int len = sizeof (zebra_msg_header) + ntohs (msg->hdr.msglen);

    // Make contiguous memory buffer for message
    memcpy (buf, &msg->hdr, sizeof (zebra_msg_header));
    memcpy (buf + sizeof (zebra_msg_header), msg->body, ntohs (msg->hdr.msglen));

    zebra_msg_delete (msg);

    int wlen = writen(fd, buf, len);
    if (wlen < 0)
    {
        LOG("ZebraOspfWriter failed to write " << fd << endl);
        return -1;
    }
    else if (wlen == 0)
    {
        LOG("Connection closed for ZebraOspfWriter(" << fd <<')' << endl);
        return -1;
    }
    else if (wlen != len)
    {
        LOG("ZebraOspfWriter(" << fd << ") cannot write the message" <<endl);
        return -1;
    }
    return 0;
}

void ZebraOspfWriter::PostMessage (zebra_msg *msg)
{
    assert (msg);
    msgQueue.push_back(msg);

    this->SetRepeats(0);
    this->SetObsolete(false);
    if (msgQueue.size() == 1)
        eventMaster.Schedule(this);
}

static zebra_msg * new_zebra_originate_request (u_int32_t seqnum, in_addr ifaddr, in_addr area_id,  lsa_header *data)
{
    zebra_lsa_originate_request*omsg;
    int omsglen;
    char buf[API_MAX_MSG_SIZE];
  
    omsglen = sizeof (zebra_lsa_originate_request) - sizeof (lsa_header) + ntohs (data->length);
    omsg = (zebra_lsa_originate_request *) buf;
    omsg->ifaddr = ifaddr;
    omsg->area_id = area_id;
    memcpy (&omsg->data, data, ntohs (data->length));
  
    return zebra_msg_new (MSG_ZEBRA_ORIGINATE_REQUEST, omsg, seqnum, omsglen);
}

int ZebraOspfWriter::OriginateLsa(in_addr ori_if, in_addr adv_id, in_addr area, u_char lsa_type, 
        u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen)
{
    u_char buf[OSPF_MAX_LSA_SIZE];
    struct lsa_header *lsah;
    u_int32_t tmp;


    // Make a new LSA from parameters 
    lsah = (lsa_header *) buf;
    lsah->ls_age = 0;
    lsah->options = 0;
    lsah->type = lsa_type;
  
    tmp = SET_OPAQUE_LSID (opaque_type, opaque_id);
    lsah->id.s_addr = htonl (tmp);
    lsah->adv_router = adv_id;
    lsah->ls_age = 0;
    lsah->ls_seqnum = 0;
    lsah->checksum = 0;
    lsah->length = htons (sizeof (lsa_header) + opaquelen);
  
    memcpy (((u_char *) lsah) + sizeof (lsa_header), opaquedata, opaquelen);

    zebra_msg * msg = new_zebra_originate_request(get_narb_seqnum(), ori_if, area, lsah);

    if (!msg)
    {
        LOG("new_zebra_originate_request failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("OriginateLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("OriginateLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    zebra_reply * reply = (zebra_reply*)msg->body;
    int ret = reply->errcode;
    zebra_msg_delete(msg);
    return ret;
}

static zebra_msg* new_zebra_delete_request (u_int32_t seqnum, in_addr adv_router, 
        in_addr area_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id)
{
    zebra_lsa_delete_request dmsg;
  
    dmsg.adv_router = adv_router;
    dmsg.area_id = area_id;
    dmsg.lsa_type = lsa_type;
    dmsg.opaque_type = opaque_type;
    dmsg.opaque_id = htonl (opaque_id);
    memset (&dmsg.pad, 0, sizeof (dmsg.pad));
  
    return zebra_msg_new (MSG_ZEBRA_DELETE_REQUEST, &dmsg, seqnum, sizeof (zebra_lsa_delete_request));
}

int ZebraOspfWriter::DeleteLsa(in_addr adv_id, in_addr area, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id)
{
    zebra_msg* msg = new_zebra_delete_request(get_narb_seqnum (), adv_id, area, lsa_type, opaque_type, opaque_id);

    if (!msg)
    {
        LOG("new_zebra_delete_request failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("DeleteLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("DeleteLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    zebra_reply * reply = (zebra_reply*)msg->body;
    int ret = reply->errcode;
    zebra_msg_delete(msg);
    return ret;
}

int ZebraOspfWriter::GetOrigianteInterfaceStatus(in_addr ifaddr)
{
    zebra_msg* msg = zebra_msg_new (MSG_ZEBRA_ORIGINATE_READY_QUERY, &ifaddr, 0, sizeof (in_addr));

    if (!msg)
    {
        LOG("zebra_msg_new(MSG_ZEBRA_ORIGINATE_READY_QUERY...) failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("GetOrigianteInterfaceStatus / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg || msg->hdr.msgtype != MSG_ZEBRA_ORIGINATE_READY)
    {
        LOG ("GetOrigianteInterfaceStatus / ReadMessage failed\n" << endl);
        return -1;
    }

    zebra_originate_ready* originate_ready = (zebra_originate_ready*)msg->body;
    return originate_ready->status;
}

    
static zebra_msg * new_zebra_update_request (u_int32_t seqnum, in_addr ifaddr, in_addr area_id,  lsa_header *data)
{
    zebra_lsa_originate_request*omsg;
    int omsglen;
    char buf[API_MAX_MSG_SIZE];
  
    omsglen = sizeof (zebra_lsa_originate_request) - sizeof (lsa_header) + ntohs (data->length);
    omsg = (zebra_lsa_originate_request *) buf;
    omsg->ifaddr = ifaddr;
    omsg->area_id = area_id;
    memcpy (&omsg->data, data, ntohs (data->length));
  
    return zebra_msg_new (MSG_ZEBRA_UPDATE_REQUEST, omsg, seqnum, omsglen);
}

int ZebraOspfWriter::UpdateLsa(in_addr ori_if, in_addr adv_id, in_addr area, u_char lsa_type, 
        u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen)
{
    u_char buf[OSPF_MAX_LSA_SIZE];
    struct lsa_header *lsah;
    u_int32_t tmp;

    // Make a new LSA from parameters 
    lsah = (lsa_header *) buf;
    lsah->ls_age = 0;
    lsah->options = 0;
    lsah->type = lsa_type;
  
    tmp = SET_OPAQUE_LSID (opaque_type, opaque_id);
    lsah->id.s_addr = htonl (tmp);
    lsah->adv_router = adv_id;
    lsah->ls_age = 0;
    lsah->ls_seqnum = 0;
    lsah->checksum = 0;
    lsah->length = htons (sizeof (lsa_header) + opaquelen);
  
    memcpy (((u_char *) lsah) + sizeof (lsa_header), opaquedata, opaquelen);

    zebra_msg * msg = new_zebra_update_request(get_narb_seqnum(), ori_if, area, lsah);

    if (!msg)
    {
        LOG("new_zebra_update_request failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("UpdateLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("UpdateLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    zebra_reply * reply = (zebra_reply*)msg->body;
    int ret = reply->errcode;
    zebra_msg_delete(msg);
    return ret;
}

static zebra_msg* new_zebra_register_opaque_type (u_int32_t seqnum, u_char ltype, u_char otype)
{
    struct zebra_register_opaque_type rmsg;
  
    rmsg.lsatype = ltype;
    rmsg.opaquetype = otype;
    memset (&rmsg.pad, 0, sizeof (rmsg.pad));
  
    return zebra_msg_new (MSG_ZEBRA_REGISTER_OPAQUETYPE, &rmsg, seqnum, sizeof (zebra_register_opaque_type));
}

int ZebraOspfWriter::RegisterOpqaueType(u_char ltype, u_char otype)
{
    zebra_msg* msg = new_zebra_register_opaque_type(get_narb_seqnum (), ltype, otype);

    int ret = 0;

    LOGF("RegisterOpqaueType [%d,%d]\n", ltype, otype);

    if (!msg)
    {
        LOG ("new_zebra_register_opaque_type failed" << endl);
        return -1;
    }

    if (WriteMessage(msg) < 0)
    {
        LOG ("RegisterOpqaueType / WriteMessage failed" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg || msg->hdr.msgtype != MSG_ZEBRA_REPLY)
    {
        LOG ("RegisterOpqaueType / ReadMessage failed\n" << endl);
        return -1;
    }
    zebra_reply * reply = (zebra_reply*)msg->body;
    int errcode = reply->errcode;
    zebra_msg_delete(msg);
    return errcode;
}

////////////////////////////////////////////////////////////////////////
///////// Constructing Router_ID TLV
///////// Memory for tlv data allocated within function
te_tlv_header * ospf_te_router_addr_tlv_alloc(in_addr addr)
{
    te_tlv_header * tlv_header = (te_tlv_header*)malloc(sizeof(te_tlv_header)+ sizeof(in_addr));
    tlv_header->type = htons(TE_TLV_ROUTER_ADDR);
    tlv_header->length = htons(sizeof(in_addr));
    memcpy((char *)tlv_header + sizeof(te_tlv_header), (char*)&addr, sizeof(in_addr));
    return tlv_header;
}

//////// Constructing TE Link TLV
//////// Memory for tlv data allocated within function
te_tlv_header *ospf_te_link_tlv_alloc(u_char type, in_addr addr)
{
    te_tlv_header * tlv_header;
    te_link_subtlv_link_type link_type;
    te_link_subtlv_link_id link_id;
    int length = sizeof (te_link_subtlv_link_type) + sizeof(te_link_subtlv_link_id);
    
    tlv_header = (te_tlv_header*)malloc(sizeof(te_tlv_header) + length);
    tlv_header->type = TE_TLV_LINK;
    tlv_header->type = htons(tlv_header->type);
    tlv_header->length = htons(length);
  
    memset(&link_type, 0, sizeof(te_link_subtlv_link_type));
    link_type.header.type = htons(TE_LINK_SUBTLV_LINK_TYPE);
    link_type.header.length = htons(1);
    link_type.link_type.value = type;
    memcpy((char *)tlv_header + sizeof(te_tlv_header), 
                (char *)&link_type, sizeof(te_link_subtlv_link_type));
  
    link_id.header.type = TE_LINK_SUBTLV_LINK_ID;
    link_id.header.type = htons(link_id.header.type);
    link_id.header.length = sizeof(in_addr);
    link_id.header.length = htons(link_id.header.length);
    link_id.value = addr;
    memcpy((char *)tlv_header + sizeof(te_tlv_header) + sizeof(te_link_subtlv_link_type), 
                (char*)&link_id, sizeof(te_link_subtlv_link_id));
  
    return tlv_header;
}

//////// Appending optional TE parameter (sub-TLV) to TE Link TLV.
//////// Memory of the tlv data re-allocated within function
te_tlv_header * ospf_te_link_subtlv_append(te_tlv_header * tlv_header, u_int16_t type, void *value)
{
    te_tlv_header *tlv_header_appended;
    char buf[OSPF_MAX_LSA_SIZE];
    int tlv_size = sizeof(te_tlv_header) + ntohs(tlv_header->length);
    int sub_tlv_size;
  
    switch (type)
    {
    case TE_LINK_SUBTLV_LCLIF_IPADDR:
        {
            te_link_subtlv_lclif_ipaddr *lclif_ipaddr
              = (te_link_subtlv_lclif_ipaddr *)(buf + tlv_size);
            
            lclif_ipaddr->header.type = htons(TE_LINK_SUBTLV_LCLIF_IPADDR);
            sub_tlv_size = sizeof (te_link_subtlv_lclif_ipaddr);
            lclif_ipaddr->header.length = htons(sizeof(in_addr));
            lclif_ipaddr->value = *((in_addr*)value);
        }
        break;
    case TE_LINK_SUBTLV_RMTIF_IPADDR:
        {
            te_link_subtlv_rmtif_ipaddr *rmtif_ipaddr 
              = (te_link_subtlv_rmtif_ipaddr *)(buf + tlv_size);
            
            rmtif_ipaddr->header.type = htons(TE_LINK_SUBTLV_RMTIF_IPADDR);
            sub_tlv_size = sizeof (te_link_subtlv_rmtif_ipaddr);
            rmtif_ipaddr->header.length = htons(sizeof(in_addr));
            rmtif_ipaddr->value = *((in_addr*)value);
         }
         break;
    case TE_LINK_SUBTLV_TE_METRIC:
        {
            te_link_subtlv_te_metric *metric 
              = (te_link_subtlv_te_metric *)(buf + tlv_size);
            
            metric->header.type = htons(TE_LINK_SUBTLV_TE_METRIC);
            sub_tlv_size = sizeof (te_link_subtlv_te_metric);
            metric->header.length = htons(sizeof(u_int32_t));
            metric->value = htonl(*((u_int32_t*)value));	     
        }
        break;
    case TE_LINK_SUBTLV_MAX_BW:
        {
            te_link_subtlv_max_bw *max_bw 
              = (te_link_subtlv_max_bw *)(buf + tlv_size);
            
            max_bw->header.type = htons(TE_LINK_SUBTLV_MAX_BW);
            sub_tlv_size = sizeof (te_link_subtlv_max_bw);
            max_bw->header.length = htons(sizeof(float));
            max_bw->value = *(float*)value;
            htonf_mbps(max_bw->value);
        }
        break;
    case TE_LINK_SUBTLV_MAX_RSV_BW:
        {
            te_link_subtlv_max_rsv_bw *max_rsv_bw 
              = (te_link_subtlv_max_rsv_bw *)(buf + tlv_size);
            
            max_rsv_bw->header.type = htons(TE_LINK_SUBTLV_MAX_RSV_BW);
            sub_tlv_size = sizeof (te_link_subtlv_max_rsv_bw);
            max_rsv_bw->header.length = htons(sizeof(float));
            max_rsv_bw->value = *(float*)value;
            htonf_mbps(max_rsv_bw->value);
        }
        break;
    case TE_LINK_SUBTLV_UNRSV_BW:
        {
            te_link_subtlv_unrsv_bw *max_unrsv_bw 
              = (te_link_subtlv_unrsv_bw *)(buf + tlv_size);
            int i;
            
            max_unrsv_bw->header.type = htons(TE_LINK_SUBTLV_UNRSV_BW);
            sub_tlv_size = sizeof (te_link_subtlv_unrsv_bw);
            max_unrsv_bw->header.length = htons(sizeof(float) * 8);
            for (i = 0; i < 8; i++)
            {
                *((float *)(&(max_unrsv_bw->value)) + i) = *(float*)value;
                htonf_mbps(*((float *)(&(max_unrsv_bw->value)) + i));
            }
        }
        break;
    case TE_LINK_SUBTLV_RSC_CLSCLR:
        {
  	     te_link_subtlv_rsc_clsclr *rsc_clsclr 
              = (te_link_subtlv_rsc_clsclr *)(buf + tlv_size);
            
            rsc_clsclr->header.type = htons(TE_LINK_SUBTLV_RSC_CLSCLR);
            sub_tlv_size = sizeof (te_link_subtlv_rsc_clsclr);
            rsc_clsclr->header.length = htons(sizeof(u_int32_t));
            rsc_clsclr->value = htonl(*((u_int32_t *)value));
        }  
        break;
    case TE_LINK_SUBTLV_LINK_LCRMT_ID:
        {
            te_link_subtlv_link_lcrmt_id *lcrmt_id 
              = (te_link_subtlv_link_lcrmt_id *)(buf + tlv_size);
  
            lcrmt_id->header.type = htons(TE_LINK_SUBTLV_LINK_LCRMT_ID);
            sub_tlv_size = sizeof (te_link_subtlv_link_lcrmt_id);
            lcrmt_id->header.length = htons(sizeof(u_int32_t) * 2);
            lcrmt_id->link_local_id = htonl(*((u_int32_t *)value));
            lcrmt_id->link_remote_id = htonl(*((u_int32_t *)value + 1));
        }
        break;
   #if 0
        case TE_LINK_SUBTLV_LINK_PROTYPE: 
          {
            te_link_subtlv_link_protype *protype 
              = (te_link_subtlv_link_protype *)(buf + tlv_size);
  
            protype->header.type = htons(TE_LINK_SUBTLV_LINK_PROTYPE);
            sub_tlv_size = sizeof (te_link_subtlv_link_protype);
            protype->header.length = htons(1);
            memset(&(protype->value4), 0, 4);
            protype->value4.value = *((u_char *)value);
            break;
          }
  #endif
    case TE_LINK_SUBTLV_LINK_IFSWCAP:
        {
            int i; float x;

            te_link_subtlv_link_ifswcap *ifswcap = (te_link_subtlv_link_ifswcap*)(buf+tlv_size);
            ifswcap->header.type = htons(TE_LINK_SUBTLV_LINK_IFSWCAP);
            sub_tlv_size = sizeof (te_link_subtlv_link_ifswcap);
            ifswcap->header.length = htons(sizeof(te_link_subtlv_link_ifswcap) - sizeof(te_tlv_header));
            memcpy((char *)&ifswcap->link_ifswcap_data, value, 
                      sizeof(te_link_subtlv_link_ifswcap) - sizeof(te_tlv_header));
            for (i = 0; i < 8; i++)
            {
                htonf_mbps(*(ifswcap->link_ifswcap_data.max_lsp_bw_at_priority + i));
            }
        }
        break;
  #if 0
        case TE_LINK_SUBTLV_LINK_SRLG:
          {
            listnode node;
            list srlg_list = (list)value;
            int sub_tlv_length = 0;
            te_tlv_header *header
              = (te_tlv_header *)(buf + tlv_size);
  
            header->type = htons(TE_LINK_SUBTLV_LINK_SRLG);
            for (node = listhead(srlg_list); node; nextnode(node))
              {
                u_int32_t* data = getdata(node);
                *data = htonl(*data);
                memcpy(buf + tlv_size + sizeof(te_tlv_header) 
                          + sub_tlv_length, data, 4);
                sub_tlv_length += 4;
              }
            sub_tlv_size = sizeof(te_tlv_header) + sub_tlv_length;
            header->length = htons(sub_tlv_length);
            memcpy((char *)(buf + tlv_size), (void *)header,
                      sizeof(te_tlv_header));
            break;
          }
  #endif
    case TE_LINK_SUBTLV_RESV_SCHEDULE:
        {
            te_tlv_header *resv_header = (te_tlv_header*)(buf + tlv_size);
            Reservation *resv = (Reservation *)(buf + tlv_size+TLV_HDR_SIZE);
            vector<Reservation*>* resvs_v = (vector<Reservation*>*)value;
            resv_header->type = htons(TE_LINK_SUBTLV_RESV_SCHEDULE);
            resv_header->length = htons(resvs_v->size()*RESV_SIZE);
            sub_tlv_size = TLV_HDR_SIZE + resvs_v->size()*RESV_SIZE;
            for (int i = 0; i < resvs_v->size(); i++)
            {
                memcpy((char *)(resv++), (char*)((*resvs_v)[i]), sizeof(Reservation));
            }
        }
        break;
    case TE_LINK_SUBTLV_DOMAIN_ID:
        {
            te_link_subtlv_domain_id *domain_id 
              = (te_link_subtlv_domain_id *)(buf + tlv_size);
            sub_tlv_size = sizeof (te_link_subtlv_domain_id);
            domain_id->header.type = htons(TE_LINK_SUBTLV_DOMAIN_ID);
            domain_id->header.length = htons(sizeof(u_int32_t));
            domain_id->value = *(u_int32_t*)value;
        }       
        break;
    default:
  	  LOGF("ospf_te_link_subtlv_append: Unrecognized subtlv type-%d\n", type);
         return tlv_header;
    }
  
    tlv_header->length = htons(ntohs(tlv_header->length) + sub_tlv_size);
    memcpy(buf, (char*)tlv_header, tlv_size);
    free(tlv_header);
  
    tlv_header_appended = (te_tlv_header*)malloc(tlv_size + sub_tlv_size);
    memcpy((char *)tlv_header_appended, (char *)buf, tlv_size + sub_tlv_size);
  
    return tlv_header_appended;
}

/*
te_tlv_header * ospf_te_link_subtlv_set_swcap_vlan(te_tlv_header * tlv_header, u_char* vtagMask, u_char* vtagMaskAlloc)
{
    te_tlv_header *tlv_header_appended = tlv_header;

    int tlv_size = sizeof(te_tlv_header) + ntohs(tlv_header->length);
    int sub_tlv_size = sizeof(te_link_subtlv_link_ifswcap);
    te_link_subtlv_link_ifswcap * ifswcap_tlv;
    int vlan_data_len = sizeof(link_ifswcap_specific_vlan_full) - sizeof (link_ifswcap_specific_vlan);

    tlv_header_appended = (te_tlv_header*)malloc(tlv_size + vlan_data_len);
    memcpy((char*)tlv_header_appended, (char*)tlv_header, tlv_size);
    tlv_header_appended->length = htons(ntohs(tlv_header->length) + vlan_data_len);
    free(tlv_header);

    ifswcap_tlv = (te_link_subtlv_link_ifswcap*)((char*)tlv_header_appended + tlv_size - sub_tlv_size);
    ifswcap_tlv->header.length = htons(sub_tlv_size - TLV_HDR_SIZE + vlan_data_len);
    link_ifswcap_specific_vlan_full* vlan_info = (link_ifswcap_specific_vlan_full*)&ifswcap_tlv->link_ifswcap_data.ifswcap_specific_info.ifswcap_specific_vlan;
    vlan_info->version = htons(IFSWCAP_SPECIFIC_VLAN_BASIC | IFSWCAP_SPECIFIC_VLAN_ALLOC);
    vlan_info->length= htons(sizeof(link_ifswcap_specific_vlan_full));
    memcpy(vlan_info->bitmask, vtagMask, MAX_VLAN_NUM/8);
    memset(vlan_info->bitmask_alloc, 0, MAX_VLAN_NUM/8);
    if (vtagMaskAlloc)
        memcpy(vlan_info->bitmask_alloc, vtagMaskAlloc, MAX_VLAN_NUM/8);

    return tlv_header_appended;
}
*/

te_tlv_header * ospf_te_link_subtlv_set_swcap_vlan(te_tlv_header * tlv_header, u_char* vtagMaskAvail, u_char* vtagMaskAlloc)
{
    te_tlv_header *tlv_header_appended = tlv_header;

    int tlv_size = sizeof(te_tlv_header) + ntohs(tlv_header->length);
    int sub_tlv_size = sizeof(te_link_subtlv_link_ifswcap);
    te_link_subtlv_link_ifswcap * ifswcap_tlv;

    link_ifswcap_specific_vlan_full vlan_info;
    vlan_info.version = htons(IFSWCAP_SPECIFIC_VLAN_BASIC | IFSWCAP_SPECIFIC_VLAN_ALLOC);
    u_int32_t vlan_data_len = MAX_VLAN_NUM/8*2;
    memcpy(vlan_info.bitmask, vtagMaskAvail, MAX_VLAN_NUM/8);
    memcpy(vlan_info.bitmask_alloc, vtagMaskAlloc, MAX_VLAN_NUM/8);
//  if (SystemConfig::zcompress)
//  {
      z_compress(vlan_info.bitmask, vlan_data_len);
      vlan_info.version |= htons(IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z);
      vlan_info.length = htons(vlan_data_len + 4); // add length and version = 4 bytes
      u_int32_t atual_len = vlan_data_len%4 == 0 ? vlan_data_len : (vlan_data_len/4 + 1)*4;
//  }
//  else
//  {
//      vlan_info.length = htons(sizeof(link_ifswcap_specific_vlan_full));
//  }
    atual_len -= 4; // the extra bytes after 8 bytes of layer specific info
    tlv_header_appended = (te_tlv_header*)malloc(tlv_size + atual_len); 
    memcpy((char*)tlv_header_appended, (char*)tlv_header, tlv_size);
    tlv_header_appended->length = htons(ntohs(tlv_header->length) + atual_len);
    free(tlv_header);
    ifswcap_tlv = (te_link_subtlv_link_ifswcap*)((char*)tlv_header_appended + tlv_size - sub_tlv_size);
    ifswcap_tlv->header.length = htons(sub_tlv_size - TLV_HDR_SIZE + atual_len);
    memcpy (&ifswcap_tlv->link_ifswcap_data.ifswcap_specific_info.ifswcap_specific_vlan, &vlan_info, atual_len + 8);

    return tlv_header_appended;
}


