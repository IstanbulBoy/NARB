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
#include "rce_log.hh"
#include "rce_api.hh"
#include "rce_lsa.hh"
#include "event.hh"
#include "rce_apiserver.hh"
#include "zebra_ospfclient.hh"

zebra_msg * zebra_msg_new (u_char msgtype, void *msgbody, u_int32_t seqnum, u_int16_t msglen)
{
  zebra_msg* msg;

  msg = new (zebra_msg);
  memset (msg, 0, sizeof (zebra_msg));

  msg->hdr.version = ZEBRA_OSPF_API_VERSION;
  msg->hdr.msgtype = msgtype;
  msg->hdr.msglen = htons (msglen);
  msg->hdr.msgseq = htonl (seqnum);

 int len = (int)msglen;
  msg->body= new char[len];
  memcpy (msg->body, msgbody, msglen);

  return msg;
}

void zebra_msg_delete (zebra_msg *msg)
{
    assert (msg && msg->body);
    delete []msg->body;
    delete msg;
}

int ZebraOspfSync::Connect (char *host, int syncport, int remote_port)
{
    struct sockaddr_in myaddr_sync, myaddr_async, peeraddr;
    int size = 0, on = 1, sock_accept, ret;
    socklen_t peeraddrlen;
    struct hostent *hp;

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
    memset (&myaddr_async, 0, sizeof (struct sockaddr_in));
    myaddr_async.sin_family = AF_INET;
    myaddr_async.sin_addr.s_addr = htonl (INADDR_ANY);
    myaddr_async.sin_port = htons (syncport+1);
    size = sizeof (struct sockaddr_in);
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
    ret = bind (sock_accept, (struct sockaddr *) &myaddr_async, size);
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
    memset (&myaddr_sync, 0, sizeof (struct sockaddr_in));
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons (syncport);
    //myaddr_sync.sin_len = sizeof (struct sockaddr_in);

    ret = bind (sync_fd, (struct sockaddr *) &myaddr_sync, size);
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
    //myaddr_sync.sin_len = sizeof (struct sockaddr_in);

    /* Now establish synchronous channel with OSPF daemon */
    ret = connect (sync_fd, (struct sockaddr *) &myaddr_sync, sizeof (struct sockaddr_in));
    if (ret < 0)
    {
      LOG_CERR<<"ZebraOspfSync::Connect: sync connect failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    /* Accept reverse connection */
    peeraddrlen = sizeof (struct sockaddr_in);
    memset (&peeraddr, 0, peeraddrlen);
    async_fd = accept (sock_accept, (struct sockaddr *) &peeraddr, &peeraddrlen);
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


void ZebraOspfSync::Run ()
{
    if (sync_fd < 0)
    {
        if (!Alive())
            return;
        if (attempt++ >= SystemConfig::max_ospf_sync_attempts-1)
        {
            SetObsolete(true);
            SetRepeats(0);
        }
	int local_port = (ospfd_port == SystemConfig::ospfd_intra_port)? SystemConfig::ospfd_intra_port_local : SystemConfig::ospfd_inter_port_local;
        if (Connect(ospfd_host, local_port, ospfd_port) < 0)
        {
            LOGF("Attempt %d/%d ### Check OSPFd %s:%d ###\n",  attempt, SystemConfig::max_ospf_sync_attempts, ospfd_host, ospfd_port);
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
  
  zebra_msg* zmsg = zebra_msg_new(MSG_ZEBRA_SYNC_LSDB, &lsa_filter, get_rce_seqnum(), len);
  writer->PostMessage(zmsg);
}

ZebraOspfSync::~ZebraOspfSync ()
{  
    Exit();
}

void ZebraOspfSync::Exit()
{
    if(reader) reader->Close();
    if(writer) writer->Close();
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

zebra_msg * ZebraOspfReader::ReadMessage ()
{
    zebra_msg* msg;
    zebra_msg_header hdr;
    static char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    /* Read message header */
    rlen = readn (fd, (char *) &hdr, sizeof (zebra_msg_header));

    if (rlen < 0)
    {
        LOG("ZebraOspfReader failed to read from"<<fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        close(fd);
        LOG("Connection closed for ZebraOspfReader(" << fd <<')' << endl);
        return NULL;
    }
    else if (rlen != sizeof (struct zebra_msg_header))
    {
        LOG("ZebraOspfReader(" << fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("ZebraOspfReader(" << fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("ZebraOspfReader failed to read from" << fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
             LOG("Connection closed for ZebraOspfReader(" << fd <<')' << endl);
            return NULL;
        }
        else if (rlen != bodylen)
        {
             LOG("ZebraOspfReader(" << fd << ") cannot read the message body" <<endl);
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
    api_msg *rce_msg;
    u_int32_t ack_id = 0x0f;
    zebra_lsa_change_notify* msg_notify;

    switch (msg->hdr.msgtype)
    {
    case MSG_ZEBRA_LSA_UPDATE_NOTIFY:
        //convert the zebra_msg into rce_api_msg
        msg_notify = (zebra_lsa_change_notify*)msg->body;
        rce_msg = api_msg_new(MSG_LSA, ACT_UPDATE, &msg_notify->data, 0, ntohl(msg->hdr.msgseq), 
            ntohs(msg->hdr.msglen) - sizeof(zebra_lsa_change_notify) + sizeof(lsa_header));
        rce_msg->hdr.msgtag[0] = server->DomainMask();
        zebra_msg_delete(msg);
        lsaEvent = new LSAHandler;
        lsaEvent->Load(rce_msg);
        if (lsaEvent->Parse())
        {
            lsaEvent->SetAutoDelete(true);
            eventMaster.Schedule(lsaEvent);
        }
        else
            delete lsaEvent;
        break;
    case MSG_ZEBRA_LSA_DELETE_NOTIFY:
        //convert the zebra_msg into rce_api_msg
        msg_notify = (zebra_lsa_change_notify*)msg->body;
        rce_msg = api_msg_new(MSG_LSA, ACT_DELETE, &msg_notify->data, 0, ntohl(msg->hdr.msgseq), 
            ntohs(msg->hdr.msglen) - sizeof(zebra_lsa_change_notify) + sizeof(lsa_header));
        rce_msg->hdr.msgtag[0] = server->DomainMask();
        zebra_msg_delete(msg);
        lsaEvent = new LSAHandler;
        lsaEvent->Load(rce_msg);
        if (lsaEvent->Parse())
        {
            lsaEvent->SetAutoDelete(true);
            eventMaster.Schedule(lsaEvent);
        }
        else
            delete lsaEvent;        
        break;
    default:
        zebra_msg_delete(msg);
        LOG("Unknown/Unprocessed zebra_msg type " << (int)msg->hdr.msgtype << endl);
    }    
}

void ZebraOspfWriter::Run()
{
    Writer::Run();

    //write an API message
    assert (fd > 0);

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

