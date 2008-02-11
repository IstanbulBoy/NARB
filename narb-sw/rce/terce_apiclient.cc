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
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_INSERT, lsah, ucid, get_rce_seqnum(), server->DomainId(), htons(lsah->length));

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
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_DELETE, lsah, ucid, get_rce_seqnum(), server->DomainId(), htons(lsah->length));

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
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_UPDATE, lsah, ucid, get_rce_seqnum(), server->DomainId(), htons(lsah->length));

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

//@@@@
int TerceApiTopoOriginator::OriginateTopology ()
{
    int ret = 0;

    if (!terce_client || !terce_client->GetWriter())
        return -1;

    // originate router-id LSA's
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        if (!router_id->hide)
        {
            this->OriginateRouterId(tc_writer, router_id);
         }
        router_id = NarbDomainInfo.NextRouterId();
    }

    // originate  te-link LSA's
    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        if (!link->hide)
        {
            this->OriginateTeLink(tc_writer, link);
        }
        link = NarbDomainInfo.NextLink();
    }

    return ret;
}

//@@@@
int TerceApiTopoOriginator::DeleteTopology ()
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;
  
    u_char lsa_type = 10;
    u_char opaque_type = 1;
  
    // delete router-id LSA's
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        if (!router_id->hide)
        {
            ret = tc_writer->DeleteLsa(*(in_addr*)&router_id->advRtId, lsa_type, opaque_type, router_id->opaque_id);
            
            LOGF("ROUTER_ID (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) deleted through NARB-TERCE API at %s.\n", 
               lsa_type, opaque_type, router_id->opaque_id, NarbDomainInfo.terce.addr);
            LOGF("\t ID = %X, ADV_ROUTER = %X, return code = %d.\n", router_id->id, router_id->advRtId, ret);
        }
        router_id = NarbDomainInfo.NextRouterId();
    }

    // delete  te-link LSA's
    link_info * link = NarbDomainInfo.FirstLink();
    while (link)
    {
        if (!link->hide)
        {
            ret = tc_writer->DeleteLsa(*(in_addr*)&(link->advRtId), lsa_type, opaque_type, link->opaque_id);
            
            LOGF("LINK_ID (lsa-type[%d] opaque-type[%d]  opaque-id[%d]) deleted through NARB-TERCE API at %s.\n", 
               lsa_type, opaque_type, link->opaque_id, NarbDomainInfo.terce.addr);
            LOGF("\t ID = %X, ADV_ROUTER = %X, return code = %d.\n", link->id, link->advRtId, ret);
        }
        link = NarbDomainInfo.NextLink();
    }

    return ret;
}


int TerceApiTopoOriginator::OriginateRouterId (TerceApiTopoWriter* tc_writer, RouterId* rtid)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;
  
    void *opaquedata;
    int opaquelen;
    u_char lsa_type = 10;
    u_char opaque_type = 1;
  
    opaquedata = (void *)BuildRouterIdOpaqueData(rtid); 
    opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                  + sizeof (struct te_tlv_header);

    //using Router ID as the opaque ID, which TERCE should not care
    ret = tc_writer->OriginateLsa(*(in_addr*)&(rtid->advRtId), lsa_type, opaque_type, rtid->Id, opaquedata, opaquelen);
    free(opaquedata);
    return ret;
}

int TerceApiTopoOriginator::OriginateTeLink (TerceApiTopoWriter* tc_writer, Link* link)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;
  
    u_char lsa_type = 10;
    u_char opaque_type = 1;
    
    void *opaquedata = BuildTeLinkOpaqueData(link);
    int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length)
                      + sizeof (struct te_tlv_header);

    //using Link Local IfAddr as the opaque ID, which TERCE should not care
    ret = tc_writer->OriginateLsa(*(in_addr*)&(link->advRtId), lsa_type, opaque_type, link->LclIfAddr(), opaquedata, opaquelen);
    free(opaquedata);
    return ret;
}

int TerceApiTopoOriginator::UpdateTeLink (TerceApiTopoWriter* tc_writer, link_info* link)
{
    int ret = 0;
    if (!terce_client || !terce_client->GetWriter())
        return -1;

    u_char lsa_type = 10;
    u_char opaque_type = 1;
    
    void *opaquedata = BuildTeLinkOpaqueData(link);
    int opaquelen = ntohs(((struct te_tlv_header *)opaquedata)->length) + sizeof (struct te_tlv_header);
  
    //using Link Local IfAddr as the opaque ID, which TERCE should not care
    ret = tc_writer->UpdateLsa(*(in_addr*)&(link->advRtId), lsa_type, opaque_type, link->LclIfAddr(), opaquedata, opaquelen);   
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

// @@@@
void* TerceApiTopoOriginator::BuildTeLinkOpaqueData(Link* link)
{
  void* opaquedata = (void *)ospf_te_link_tlv_alloc(link->linkType, *(in_addr*)&link->id); 
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_LOC_IF))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LCLIF_IPADDR, &link->lclIfAddr);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_REM_IF))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RMTIF_IPADDR, &link->rmtIfAddr);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_MAX_BW, &link->maxBandwidth);
    }
  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_METRIC))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
    		TE_LINK_SUBTLV_TE_METRIC, &link->metric);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_MAX_RSV_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata, 
    		TE_LINK_SUBTLV_MAX_RSV_BW, &link->maxReservableBandwidth);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_UNRSV_BW))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_UNRSV_BW, link->unreservedBandwidth);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_IFSW_CAP))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_LINK_IFSWCAP, (void*)link->ifswcap);
      if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_VLAN))
          opaquedata = ospf_te_link_subtlv_set_swcap_vlan((te_tlv_header*)opaquedata, link->vtagBitMask, link->vtagBitMaskAlloc);
    }

  if (LINK_PARA_FLAG(link->info_flag, LINK_PARA_FLAG_RESV))
    {
      opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_RESV_SCHEDULE, (void*)(&link->resvTable.reserves));
    }

  opaquedata = ospf_te_link_subtlv_append((te_tlv_header*)opaquedata,
    		TE_LINK_SUBTLV_DOMAIN_ID, (void*)&NarbDomainInfo.domain_id);

  return opaquedata;

}

