/*
 * Copyright (c) 2008
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2008
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

#include "rce_log.hh"
#include "rce_lsa.hh"
#include "terce_apiclient.hh"
#include <errno.h>
#include <vector>

void TerceApiTopoSync::InitRceTerceComm()
{
    if (sync_fd < 0)
    {
        LOGF("Sync-comm socket to TERCE (server %s:%d) does not exist\n", terce_host, terce_port);
        return;
    }

    //$$$$ TBD
    u_int32_t ucid = domain_id;
    api_msg* amsg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_INIT, NULL, ucid, get_rce_seqnum(), 0, RCE_TERCE_ASYNC_PORT);

    if (writer->WriteMessage(amsg) < 0)
    {
        LOGF("Sync-comm socket to TERCE (server %s:%d) failed to send InitComm message\n", terce_host, terce_port);
        return;
    }

    /* kick start the server */
    if (Accept(RCE_TERCE_ASYNC_PORT) < 0)
    {
	LOGF("RCE-TerceApiTopoSync async server failed.");
	SetObsolete(true);
	SetRepeats(0);
	return;
    }
    assert(async_fd > 0);
    if (reader == NULL)
    {
	reader = new TerceApiTopoReader(async_fd, this);
	reader->SetRepeats(FOREVER);
	reader->SetAutoDelete(true);
	reader->SetSyncSocket(sync_fd);
	eventMaster.Schedule(reader);
    }

    assert(reader != NULL);
    api_msg* rmsg = reader->ReadSyncMessage();
    if (!rmsg)
    {
        LOGF("Sync-comm socket to TERCE (server %s:%d) failed to receive Init response message\n", terce_host, terce_port);        
        return;
    }    
    if (rmsg->hdr.action == ACT_ACK)
        api_ready = true;
    else // error code?
        api_ready = false;
    api_msg_delete(rmsg);
    
} 

void TerceApiTopoSync::KeepAlive()
{
    if (sync_fd < 0)
    {
        LOGF("Sync-comm socket to TERCE %s:%d does not exist\n", terce_host, terce_port);
        return;
    }

    u_int32_t ucid = domain_id;
    api_msg* amsg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_ALIVE, NULL, ucid, get_rce_seqnum(), 0);
    //amsg->hdr.tag = RCE_TERCE_SYNC_PORT+1;
    writer->PostMessage(amsg);
} 

int TerceApiTopoSync::Connect (char *host, int syncport, int remote_port)
{
    sockaddr_in myaddr_sync;
    int on = 1, ret;
    hostent *hp;

    // Make connection for synchronous requests and connect to server
    // Resolve address of server
    hp = gethostbyname (host);
    if (!hp)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: no such host " << host <<endl;
      return -1;
    }

    sync_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sync_fd < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: creating sync socket failed" <<endl;
      return -1;
    }

    //Reuse addr and port
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: SO_REUSEADDR failed" <<endl;
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: SO_REUSEPORT failed" <<endl;
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }
#endif

    /* Bind sync socket to address structure. This is needed since we
     * want the sync port number on a fixed port number. The reverse
     * async channel will be at this port+1 */
    memset (&myaddr_sync, 0, sizeof (sockaddr_in));
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons (syncport);

    ret = bind (sync_fd, (sockaddr *) &myaddr_sync, sizeof (sockaddr_in));
    if (ret < 0)
    { 
	LOG_CERR<<"TerceApiTopoSync::Connect: bind sync socket failed"<<endl;
	close (sync_fd);
	sync_fd = -1;
	return -1;
    }
    //Prepare address structure for connect
    memcpy (&myaddr_sync.sin_addr, hp->h_addr, hp->h_length);
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons(remote_port);

    ret = connect (sync_fd, (sockaddr *) &myaddr_sync, sizeof (sockaddr_in));
    if (ret < 0)
    {
	LOG_CERR<<"TerceApiTopoSync::Connect: sync connect failed"<<endl;
	close (sync_fd);
	sync_fd = -1;
	return -1;
    } 

    return 0;
}

int TerceApiTopoSync::Accept (int asyncport)
{
    sockaddr_in myaddr_async, peeraddr;
    int size = 0, on = 1, sock_accept, ret;
    socklen_t peeraddrlen;


    /* There are two connections between the client and the server.
     First the client opens a connection for synchronous requests/replies 
     to the server. The server will accept this connection and  as a reaction
     open a reverse connection channel for asynchronous messages. */
    sock_accept = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_accept < 0)
    {
      LOG_CERR<< "TerceApiTopoSync::Accept: creating async socket failed\n" <<endl;
      return -1;
    }

    //Prepare socket for asynchronous messages
    //Initialize async address structure
    memset (&myaddr_async, 0, sizeof (sockaddr_in));
    myaddr_async.sin_family = AF_INET;
    myaddr_async.sin_addr.s_addr = htonl (INADDR_ANY);
    myaddr_async.sin_port = htons (asyncport);
    size = sizeof (sockaddr_in);
    //myaddr_async.sin_len = size;

    //This is a server socket, reuse addr and port
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Accept: SO_REUSEADDR failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Accept: SO_REUSEPORT failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#endif

    //Bind socket to address structure
    ret = bind (sock_accept, (sockaddr *) &myaddr_async, size);
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Accept: bind async socket failed\n"<<endl;
      close (sock_accept);
      return -1;
    }

    //Wait for reverse channel connection establishment from server
    ret = listen (sock_accept, 4);
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Accept: listen :" << strerror (errno);
      close (sock_accept);
      return -1;
    }

    //Accept reverse connection
    peeraddrlen = sizeof (sockaddr_in);
    memset (&peeraddr, 0, peeraddrlen);
    async_fd = accept (sock_accept, (sockaddr *) &peeraddr, &peeraddrlen);
    if (async_fd < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Accept: accept async failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    //Server socket is not needed anymore since we are not accepting more connections
    close (sock_accept);

    return 0;
}

int TerceApiTopoSync::RunWithoutSyncTopology ()
{
    if (sync_fd < 0)
    {
        if (Connect(terce_host, RCE_TERCE_SYNC_PORT, terce_port) < 0)
        {
            LOGF("RCE-TerceApiTopoSync connection failed : Check TERCE server %s:%d\n", terce_host, terce_port);
            SetObsolete(true);
            SetRepeats(0);
            return -1;
        }
        assert(sync_fd > 0);
    }
    if (writer == NULL)
    {
        writer = new TerceApiTopoWriter(sync_fd, this);
        writer->SetRepeats(0);
        writer->SetAutoDelete(false);
    }

    //Check if the API is ready
    if (!api_ready)
    {
        InitRceTerceComm();
	if (!api_ready)
	    return -2;
    }

    return 0;
}

void TerceApiTopoSync::Run ()
{
    if (sync_fd < 0)
    {
        if (attempt++ >= 2)
        {
            SetObsolete(true);
            SetRepeats(0);
        }
        if (Connect(terce_host, RCE_TERCE_SYNC_PORT, terce_port) < 0)
        {
            LOGF("RCE-TerceApiTopoSync connection attempt %d/3 ### Check TERCE server status %s:%d ###\n", attempt, terce_host, terce_port);
            return;
        }
    }
    if (writer == NULL)
    {
        writer = new TerceApiTopoWriter(sync_fd, this);
        writer->SetRepeats(0);
        writer->SetAutoDelete(false);
    }

    //Check if the API is ready
    if (!api_ready)
    {
        InitRceTerceComm();
        if (!api_ready)
            return;
    }

    //Start SyncTopology

    //@@@@ RCE IP ?
    u_int32_t ucid = domain_id;
    api_msg* amsg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_QUERY, NULL, ucid, get_rce_seqnum(), 0);
    amsg->hdr.msgtag[1] = htonl(DOMAIN_MASK_GLOBAL);
    if (writer->WriteMessage(amsg) < 0)
    {
        LOGF("Sync-comm socket to TERCE (server %s:%d) failed to send SyncQuery message\n", terce_host, terce_port);
        return;
    }
    assert(reader != NULL);
    api_msg* rmsg = reader->ReadSyncMessage();
    if (!rmsg)
    {
        LOGF("Sync-comm socket to TERCE (server %s:%d) failed to receive SyncQuery response message\n", terce_host, terce_port);
        return;
    }
    if (rmsg->hdr.action == ACT_ERROR)
    {
        LOGF("RCE-TerceApiTopoSync MSG_TERCE_TOPO_SYNC Query failed, ErrorCode=%d...\n", htonl(rmsg->hdr.tag));
    }
    api_msg_delete(rmsg);
    return;
}

TerceApiTopoSync::~TerceApiTopoSync ()
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

void TerceApiTopoReader::Run()
{
    //read an NARB-TERCE API async message
    assert (fd > 0);
    api_msg * msg = ReadMessage();

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

    //Message handler    
    HandleMessage(msg);
}

void TerceApiTopoReader::SetSyncSocket (int sock)
{
    assert(sock > 0);
    sync_fd = sock;
}

api_msg * TerceApiTopoReader::ReadMessage ()
{
    api_msg* msg = NULL;
    api_msg_header hdr;
    static char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    // Read message header
    rlen = readn (fd, (char *) &hdr, sizeof (api_msg_header));

    if (rlen < 0)
    {
        LOG("TerceApiTopoReader / Async failed to read from"<<fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOG("Connection closed for TerceApiTopoReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

        close(fd);
        fd = -1;
        if (sync_fd > 0)
        {
            close(sync_fd);
            sync_fd = -1;
        }
        return NULL;
    }
    else if (rlen != sizeof (api_msg_header))
    {
        LOG("TerceApiTopoReader / Async (" << fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("TerceApiTopoReader / Async (" << fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("TerceApiTopoReader / Async failed to read from" << fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
            LOG("Connection closed for TerceApiTopoReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

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
             LOG("TerceApiTopoReader / Async (" << fd << ") cannot read the message body" <<endl);
            return NULL;
        }
    }

    // Allocate new message
    msg = new api_msg;
    msg->hdr = hdr;
    msg->body = new char[bodylen];
    memcpy(msg->body, buf, bodylen);
    return msg;
}

api_msg * TerceApiTopoReader::ReadSyncMessage ()
{
    api_msg* msg = NULL;
    api_msg_header hdr;
    static char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    // Read message header
    rlen = readn (sync_fd, (char *) &hdr, sizeof (api_msg_header));

    if (rlen < 0)
    {
        LOG("TerceApiTopoReader / Sync failed to read from"<<sync_fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOG("Connection closed for TerceApiTopoReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

        close(sync_fd);
        sync_fd = -1;
        if (fd > 0)
        {
            close(fd);
            fd = -1;
        }
        return NULL;
    }
    else if (rlen != sizeof (api_msg_header))
    {
        LOG("TerceApiTopoReader /Sync(" << sync_fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("TerceApiTopoReader / Sync(" << sync_fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (sync_fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("TerceApiTopoReader failed to read from / Sync" << sync_fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
            LOG("Connection closed for TerceApiTopoReader Sync(" << sync_fd <<") and Async(" << fd <<')'  << endl);

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
             LOG("TerceApiTopoReader / Sync(" << sync_fd << ") cannot read the message body" <<endl);
            return NULL;
        }
    }

    // Allocate new message
    msg = new api_msg;
    msg->hdr = hdr;
    msg->body = new char[bodylen];
    memcpy(msg->body, buf, bodylen);
    return msg;
}

void TerceApiTopoReader::HandleMessage (api_msg *msg)
{
    LSAHandler * lsaEvent;

    switch (msg->hdr.msgtype)
    {
    case MSG_TERCE_TOPO_SYNC: 
        LOG("Unexpected MSG_TERCE_TOPO_SYNC terce messsage: action=" << (int)msg->hdr.action << endl
            << "TerceApiTopoReader::HandleMessage only supposes to receive asychronous messages..." << endl);
        api_msg_delete(msg);
        break;

    case MSG_TERCE_TOPO_ASYNC: //Rce only
        switch(msg->hdr.action)
        {
         case ACT_INSERT:
         case ACT_UPDATE:
         case ACT_DELETE:
            msg->hdr.msgtag[0] = DOMAIN_MASK_GLOBAL;
            lsaEvent = new LSAHandler;
            lsaEvent->Load(msg);
            if (lsaEvent->Parse())
            {
                lsaEvent->SetAutoDelete(true);
                eventMaster.Schedule(lsaEvent);
            }
            else
                delete lsaEvent;
            break;

         case ACT_ALIVE:
            //$$$$TODO
            //update keep-alive timer...
            break;
        }
        break;

    default:
        LOG("Unkonwn/Unprocessed terce messsage (type=" << (int)(msg->hdr.msgtype)
            << ", action=" << (int)msg->hdr.action << ")." << endl);
        api_msg_delete(msg);
    }    
}

void TerceApiTopoWriter::Run()
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

    api_msg * msg = (api_msg*)msgQueue.front();
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

int TerceApiTopoWriter::WriteMessage (api_msg *msg)
{
    char buf[API_MAX_MSG_SIZE];

    assert (msg);

    // Length of message including header
    int len = sizeof (api_msg_header) + ntohs (msg->hdr.msglen);

    // Make contiguous memory buffer for message
    memcpy (buf, &msg->hdr, sizeof (api_msg_header));
    if(msg->body != NULL)
	memcpy (buf + sizeof (api_msg_header), msg->body, ntohs (msg->hdr.msglen));

    api_msg_delete (msg);

    int wlen = writen(fd, buf, len);
    if (wlen < 0)
    {
        LOG("TerceApiTopoWriter failed to write " << fd << endl);
        return -1;
    }
    else if (wlen == 0)
    {
        LOG("Connection closed for TerceApiTopoWriter(" << fd <<')' << endl);
        return -1;
    }
    else if (wlen != len)
    {
        LOG("TerceApiTopoWriter(" << fd << ") cannot write the message" <<endl);
        return -1;
    }
    return 0;
}

void TerceApiTopoWriter::PostMessage (api_msg *msg)
{
    assert (msg);
    msgQueue.push_back(msg);

    this->SetRepeats(0);
    this->SetObsolete(false);
    if (msgQueue.size() == 1)
        eventMaster.Schedule(this);
}

int TerceApiTopoWriter::OriginateLsa(in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen)
{
    //$$$$ no originating interface and area number required in RCE-TERCE API
    
    u_char buf[API_MAX_MSG_SIZE];
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

    u_int32_t ucid = server->DomainId();
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_INSERT, lsah, ucid, get_rce_seqnum(), htons(lsah->length), server->DomainId());

    if (!msg)
    {
        LOG("api_msg_new failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("TerceApiTopoWriter::OriginateLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("TerceApiTopoWriter::OriginateLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    int ret = 0;
    if (msg->hdr.action == ACT_ERROR) 
    {
        ret = ntohl(msg->hdr.tag);
    }
    api_msg_delete(msg);
    return ret;
}

int TerceApiTopoWriter::DeleteLsa(in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id)
{
    u_char buf[OSPF_MAX_LSA_SIZE];
    struct lsa_header *lsah;

    // Make a new LSA from parameters 
    lsah = (lsa_header *) buf;
    lsah->ls_age = 0;
    lsah->options = 0;
    lsah->type = lsa_type; 
    lsah->adv_router = adv_id;
    lsah->id.s_addr = opaque_id;
    lsah->ls_age = 0;
    lsah->ls_seqnum = 0;
    lsah->checksum = 0;
    lsah->length = htons (sizeof (lsa_header));
    //empty lsa body

    u_int32_t ucid = server->DomainId();
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_DELETE, lsah, ucid, get_rce_seqnum(), htons(lsah->length), server->DomainId());

    if (!msg)
    {
        LOG("api_msg_new failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("TerceApiTopoWriter::DeleteLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("TerceApiTopoWriter::DeleteLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    int ret = 0;
    if (msg->hdr.action == ACT_ERROR) 
    {
        ret = ntohl(msg->hdr.tag);
    }
    api_msg_delete(msg);
    return ret;
}
    
int TerceApiTopoWriter::UpdateLsa(in_addr adv_id, u_char lsa_type, u_char opaque_type, u_int32_t opaque_id, void * opaquedata, int opaquelen)
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

    u_int32_t ucid = server->DomainId();
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_UPDATE, lsah, ucid, get_rce_seqnum(), htons(lsah->length), server->DomainId());

    if (!msg)
    {
        LOG("api_msg_new failed"<<endl);
        return -1;
    }
  
    if (WriteMessage(msg) < 0)
    {
        LOG ("TerceApiTopoWriter::UpdateLsa / WriteMessage failed\n" << endl);
        return -1;
    }

    assert (server && server->GetReader());
    msg = server->GetReader()->ReadSyncMessage();
    if (!msg)
    {
        LOG ("TerceApiTopoWriter::UpdateLsa / ReadMessage failed\n" << endl);
        return -1;
    }

    int ret = 0;
    if (msg->hdr.action == ACT_ERROR) 
    {
        ret = ntohl(msg->hdr.tag);
    }
    api_msg_delete(msg);
    return ret;
}




///////////TerceApiTopoOriginator//////////

void TerceApiTopoOriginator::Run()
{
    if (terce_client && terce_client->RceTerceApiReady())
    {
        DeleteTopology();
        OriginateTopology();
    }
}

//Originating intra-domain/physical topology only
//TODO: will be made configurable to select the target topology
int TerceApiTopoOriginator::OriginateTopology ()
{
    int ret = 0;

    if (!terce_client || !terce_client->GetWriter())
        return -1;

    RadixTree<Resource>* tree;
    RadixNode<Resource> *node;

    // originate router-id LSA's
    tree = RDB.Tree(RTYPE_LOC_RTID);
    node = tree->Root();
    while (node)
    {
        RouterId* router_id = (RouterId*)node->Data();
        if (router_id != NULL)
        {
            this->OriginateRouterId(router_id);
        }

        node = tree->NextNode(node);
    }

    // originate  te-link LSA's
    tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
    node = tree->Root();
    while (node)
    {
        Link* link = (Link*)node->Data();
        if (link != NULL)
        {
            this->OriginateTeLink(link);
        }

        node = tree->NextNode(node);
    }

    return ret;
}

int TerceApiTopoOriginator::DeleteTopology ()
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;
  
    u_char lsa_type = 10;
    u_char opaque_type = 1;
  
    RadixTree<Resource>* tree;
    RadixNode<Resource> *node;

    // originate router-id LSA's
    tree = RDB.Tree(RTYPE_LOC_RTID);
    node = tree->Root();
    while (node)
    {
        RouterId* router_id = (RouterId*)node->Data();
        if (router_id != NULL)
        {
			u_int32_t adv_rt_id = router_id->AdvRtId();
        	u_int32_t opaque_id = router_id->Id();
            terce_client->GetWriter()->DeleteLsa(*(in_addr*)&adv_rt_id, lsa_type, opaque_type, opaque_id);
        }

        node = tree->NextNode(node);
    }

    // originate  te-link LSA's
    tree = RDB.Tree(RTYPE_LOC_PHY_LNK);
    node = tree->Root();
    while (node)
    {
        Link* link = (Link*)node->Data();
        if (link != NULL)
        {
			u_int32_t adv_rt_id = link->AdvRtId();
        	u_int32_t opaque_id = link->LclIfAddr();
            terce_client->GetWriter()->DeleteLsa(*(in_addr*)&adv_rt_id, lsa_type, opaque_type, opaque_id);
        }

        node = tree->NextNode(node);
    }

    return ret;
}


int TerceApiTopoOriginator::OriginateRouterId (RouterId* rtid)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;

    void *opaquedata;
    int opaquelen;
    u_char lsa_type = 10;
    u_char opaque_type = 1;

	TerceApiTopoWriter* tc_writer = terce_client->GetWriter();    
    opaquedata = (void *)BuildRouterIdOpaqueData(rtid); 
    opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                  + sizeof (struct te_tlv_header);

    //using Router ID as the opaque ID, which TERCE should not care
	u_int32_t adv_rt_id = rtid->AdvRtId();
    ret = tc_writer->OriginateLsa(*(in_addr*)&adv_rt_id, lsa_type, opaque_type, rtid->Id(), opaquedata, opaquelen);
    free(opaquedata);
    return ret;
}

int TerceApiTopoOriginator::OriginateTeLink (Link* link)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;
  
    u_char lsa_type = 10;
    u_char opaque_type = 1;
    
	TerceApiTopoWriter* tc_writer = terce_client->GetWriter();    
    void *opaquedata = BuildTeLinkOpaqueData(link);
    int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                      + sizeof (struct te_tlv_header);

    //using Link Local IfAddr as the opaque ID, which TERCE should not care
	u_int32_t adv_rt_id = link->AdvRtId();
    ret = tc_writer->OriginateLsa(*(in_addr*)&adv_rt_id, lsa_type, opaque_type, link->LclIfAddr(), opaquedata, opaquelen);
    free(opaquedata);
    return ret;
}

int TerceApiTopoOriginator::UpdateTeLink (Link* link)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;

    u_char lsa_type = 10;
    u_char opaque_type = 1;
    
	TerceApiTopoWriter* tc_writer = terce_client->GetWriter();    
    void *opaquedata = BuildTeLinkOpaqueData(link);
    int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length) + sizeof (struct te_tlv_header);
  
    //using Link Local IfAddr as the opaque ID, which TERCE should not care
	u_int32_t adv_rt_id = link->AdvRtId();
    ret = tc_writer->UpdateLsa(*(in_addr*)&adv_rt_id, lsa_type, opaque_type, link->LclIfAddr(), opaquedata, opaquelen);   
    free(opaquedata);
    return ret;
}

void* TerceApiTopoOriginator::BuildRouterIdOpaqueData(RouterId* rtid)
{
    te_tlv_header * tlv_header = (te_tlv_header*)malloc(sizeof(te_tlv_header)+ sizeof(in_addr));
    tlv_header->type = htons(TE_TLV_ROUTER_ADDR);
    tlv_header->length = htons(sizeof(in_addr));
    *(u_int32_t*)((char *)tlv_header + sizeof(te_tlv_header)) = rtid->Id();
    return (void*)tlv_header;
}


//////// Defining static C functions
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
    case TE_LINK_SUBTLV_LINK_IFSWCAP:
        {
            int i; float x;
            
            ISCD* iscd = (ISCD*)value;
            sub_tlv_size = IFSWCAP_BASE_SIZE;
            if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
            && (htons(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) != 0)
            {
              sub_tlv_size += (sizeof(link_ifswcap_specific_vlan) - 8);
            }
            else if (iscd->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC
            && (htons(iscd->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0)
            {
              sub_tlv_size += (sizeof(link_ifswcap_specific_subnet_uni) - 8);
            }

            te_link_subtlv_link_ifswcap *ifswcap = (te_link_subtlv_link_ifswcap*)(buf+tlv_size);
            ifswcap->header.type = htons(TE_LINK_SUBTLV_LINK_IFSWCAP);
            ifswcap->header.length = htons(sub_tlv_size - sizeof(te_tlv_header));
            memcpy((char *)&ifswcap->link_ifswcap_data, value, sub_tlv_size - sizeof(te_tlv_header));
            for (i = 0; i < 8; i++)
            {
                htonf_mbps(*(ifswcap->link_ifswcap_data.max_lsp_bw_at_priority + i));
            }
        }
        break;
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

//////// End of defining static C functions


void* TerceApiTopoOriginator::BuildTeLinkOpaqueData(Link* link)
{
  void* opaquedata = (void *)ospf_te_link_tlv_alloc(link->linkType, *(in_addr*)&link->id); 
  if (link->lclIfAddr != 0)
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LCLIF_IPADDR, &link->lclIfAddr);
    }
  if (link->rmtIfAddr != 0)
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RMTIF_IPADDR, &link->rmtIfAddr);
    }

  opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_MAX_BW, &link->maxBandwidth);
  
  opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
		TE_LINK_SUBTLV_TE_METRIC, &link->metric);

  if (link->maxReservableBandwidth != 0)
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
    		TE_LINK_SUBTLV_MAX_RSV_BW, &link->maxReservableBandwidth);
    }

  if (link->unreservedBandwidth != 0)
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_UNRSV_BW, link->unreservedBandwidth);
    }

  if (link->iscds.size() > 0)
    {
      //Only sending the first ISCD
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LINK_IFSWCAP, (void*)link->iscds.front());
    }

  if (link->resvTable.reserves.size() > 0)
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RESV_SCHEDULE, (void*)(&link->resvTable.reserves));
    }

  if (SystemConfig::domainId != 0)
  	{
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_DOMAIN_ID, (void*)&SystemConfig::domainId);
  	}

  return opaquedata;
}

