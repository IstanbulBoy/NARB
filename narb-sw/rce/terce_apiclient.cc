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

#include "rce_log.hh"
#include "rce_lsa.hh"
#include "terce_apiclient.hh"
#include <errno.h>
#include <vector>

void TerceApiTopoSync::InitRceTerceComm()
{
    if (sync_fd < 0)
    {
        LOGF("Sync-comm socket to TERCE %s:%d does not exist\n", terce_host, terce_port);
        return;
    }

    //$$$$ TBD
    u_int32_t ucid = domain_id;
    api_msg* amsg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_INIT, NULL, ucid, get_rce_seqnum(), 0);
    amsg->hdr.tag = RCE_TERCE_SYNC_PORT+1; //a.k.a. amsg->hdr.msgtag[1]; amsg->hdr.msgtag[0] is for domain_mask
    writer->WriteMessage(amsg);

    api_msg* rmsg = reader->ReadSyncMessage();
    if (rmsg->hdr.action == ACT_ACK)
        api_ready = true;
    else // error code?
        api_ready = false;
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
      LOG_CERR<< "TerceApiTopoSync::Connect: creating async socket failed\n" <<endl;
      return -1;
    }

    //Prepare socket for asynchronous messages
    //Initialize async address structure
    memset (&myaddr_async, 0, sizeof (sockaddr_in));
    myaddr_async.sin_family = AF_INET;
    myaddr_async.sin_addr.s_addr = htonl (INADDR_ANY);
    myaddr_async.sin_port = htons (syncport+1);
    size = sizeof (sockaddr_in);
    //myaddr_async.sin_len = size;

    //This is a server socket, reuse addr and port
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: SO_REUSEADDR failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sock_accept, SOL_SOCKET,
    	    SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: SO_REUSEPORT failed\n"<<endl;
      close (sock_accept);
      return -1;
    }
#endif

    //Bind socket to address structure
    ret = bind (sock_accept, (sockaddr *) &myaddr_async, size);
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: bind async socket failed\n"<<endl;
      close (sock_accept);
      return -1;
    }

    //Wait for reverse channel connection establishment from server
    ret = listen (sock_accept, 4);
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: listen :" << strerror (errno);
      close (sock_accept);
      return -1;
    }

    // Make connection for synchronous requests and connect to server
    // Resolve address of server
    hp = gethostbyname (host);
    if (!hp)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: no such host " << host <<endl;
      close (sock_accept);
      return -1;
    }

    sync_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sync_fd < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: creating sync socket failed" <<endl;
      close (sock_accept);
      return -1;
    }

    //Reuse addr and port
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: SO_REUSEADDR failed" <<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sync_fd, SOL_SOCKET, SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
      LOG_CERR<< "TerceApiTopoSync::Connect: SO_REUSEPORT failed" <<endl;
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

    ret = bind (sync_fd, (sockaddr *) &myaddr_sync, size);
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: bind sync socket failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    //Prepare address structure for connect
    memcpy (&myaddr_sync.sin_addr, hp->h_addr, hp->h_length);
    myaddr_sync.sin_family = AF_INET;
    myaddr_sync.sin_port = htons(remote_port);

    // Now establish synchronous channel with OSPF daemon
    ret = connect (sync_fd, (sockaddr *) &myaddr_sync, sizeof (sockaddr_in));
    if (ret < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: sync connect failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    //Accept reverse connection
    peeraddrlen = sizeof (sockaddr_in);
    memset (&peeraddr, 0, peeraddrlen);
    async_fd = accept (sock_accept, (sockaddr *) &peeraddr, &peeraddrlen);
    if (async_fd < 0)
    {
      LOG_CERR<<"TerceApiTopoSync::Connect: accept async failed"<<endl;
      close (sock_accept);
      close (sync_fd);
      sync_fd = -1;
      return -1;
    }

    //Server socket is not needed anymore since we are not accepting more connections
    close (sock_accept);

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
            LOGF("Attempt %d/3 ### Check TERCE status %s:%d ###\n", attempt, terce_host, terce_port);
            return;
        }
    }
    if (reader == NULL)
    {
        reader = new TerceApiTopoReader(async_fd, this);
        reader->SetRepeats(FOREVER);
        reader->SetAutoDelete(true);
        eventMaster.Schedule(reader);
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
    writer->PostMessage(amsg);
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
    //read an RCE-TERCE API async message
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

    case MSG_TERCE_TOPO_ASYNC:
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
    assert (msg->body);

    // Length of message including header
    int len = sizeof (api_msg_header) + ntohs (msg->hdr.msglen);

    // Make contiguous memory buffer for message
    memcpy (buf, &msg->hdr, sizeof (api_msg_header));
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
    api_msg * msg = api_msg_new(MSG_TERCE_TOPO_SYNC, ACT_QUERY, lsah, ucid, get_rce_seqnum(), server->DomainId(), htons(lsah->length));

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
    u_char buf[API_MAX_MSG_SIZE];
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

