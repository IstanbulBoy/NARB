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
 
#include "apiserver.hh"
#include "toolbox.hh"
#include "log.hh"
#include <errno.h>

int APIServer::Start()
{
    int sock;
    int ret = -1;
    struct sockaddr_in sa_in;
    memset (&sa_in, 0, sizeof (struct sockaddr_in));

    //Get a stream socket
    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        LOG("can't make socket sockunion_stream_socket");
        return ret;
    }
  
    // reuse address and port on the server
    int on = 1;
    ret = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
        LOGF("can't set sockopt SO_REUSEADDR to socket %d", sock);
        return ret;
    }
#ifdef SO_REUSEPORT
    ret = setsockopt (sock, SOL_SOCKET, SO_REUSEPORT, (void *) &on, sizeof (on));
    if (ret < 0)
    {
        LOGF("can't set sockopt SO_REUSEPORT to socket %d", sock);
        return ret;
    }
#endif

    // bind socket
    assert (port > 0);

    int size = sizeof(struct sockaddr_in);
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    ret = bind (sock, (sockaddr *)&sa_in, size);
    if (ret < 0)
   {
      LOGF("can't bind socket : %s", strerror (errno));
      close (sock);
      return ret;
    }

    // Listen under queue length 4
    ret = listen (sock, 4);
    if (ret < 0)
    {
      LOGF ("APIServer::Init()::listen: %s",  strerror (errno));
      close (sock);
      return ret;
    }

    //Sechedule itself
    this->fd = sock;
    this->SetRepeats(FOREVER);
    eventMaster.Schedule(this);

    return ret;
}

void APIServer::Exit()
{
    if (!this->Obsolete())
        eventMaster.Remove(this);
}

APIReader* APIServer::CreateAPIReader(int sock)
{
    return (new APIReader(sock, this));
}

APIWriter* APIServer::CreateAPIWriter(int sock)
{
    return (new APIWriter(sock, this));
}

void APIServer::Run()
{
    struct sockaddr_in sa_in;
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    socklen_t len = sizeof(struct sockaddr_in);
    int new_sock = accept (fd, (struct sockaddr *)&sa_in, &len);

    APIWriter* api_writer = CreateAPIWriter(new_sock);
    api_writer->SetAutoDelete(false);
    api_writer->SetRepeats(0);

   APIReader* api_reader = CreateAPIReader(new_sock);
    api_reader->SetAutoDelete(true);
    api_reader->SetRepeats(FOREVER);
    api_reader->SetWriter(api_writer);
    api_writer->SetReader(api_reader);
    
    //Schedule read and write events for each accepted connection
    //eventMaster.Schedule(api_writer);
    eventMaster.Schedule(api_reader);

    LOG_DEBUG("Accepted a connection on socket(" <<new_sock <<")" << endl);
}

/////////////////////////////////////////////////////////////////

void APIReader::Run()
{
    Reader::Run();

    //read an API message
    assert (fd > 0);
    api_msg * msg = ReadMessage();

    //something is wrong with socket read
    if (!msg)
    {
        assert(api_writer);
        api_writer->Close();
        Close();
        return;
    }

    //api_msg_dump(msg);

    //Message handler    
    HandleMessage(msg); 
}

api_msg * APIReader::ReadMessage ()
{
    api_msg* msg;
    api_msg_header header;
    char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    /* Read message header */
    rlen = readn (fd, (char *) &header, sizeof (api_msg_header));

    if (rlen < 0)
    {
        LOG("APIReader failed to read from "<<fd<<endl);
        return NULL;
    }
    else if (rlen == 0)
    {
        close(fd);
        LOG("Connection closed for APIReader(" << fd <<')' << endl);
        return NULL;
    }
    else if (rlen != sizeof (struct api_msg_header))
    {
        LOG("APIReader(" << fd << ") cannot read the message header" <<endl);
        return NULL;
    }

    if (MSG_CHKSUM(header) != header.chksum)
    {
        LOG("APIReader(" << fd << ") packet corrupt" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (header.length);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOG("APIReader(" << fd << ") cannot read oversized packet" <<endl);
        return NULL;
    }

    if (bodylen > 0)
    {
        // Read message body
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
             LOG("APIReader failed to read from" << fd << endl);
            return NULL;
        }
        else if (rlen == 0)
        {
             LOG("Connection closed for APIReader(" << fd <<')' << endl);
            return NULL;
        }
        else if (rlen != bodylen)
        {
             LOG("APIReader(" << fd << ") cannot read the message body" <<endl);
            return NULL;
        }
    }

    // Allocate new message
    msg = api_msg_new (ntohs(header.type), ntohs(header.length), buf, ntohl(header.ucid), ntohl (header.seqnum));
    msg->header.msgtag[0] = header.msgtag[0];
    msg->header.msgtag[1] = header.msgtag[1];
    return msg;
}

int APIReader::HandleMessage (api_msg *msg)
{
    LOG_DEBUG("Virtual function APIReader::HandleMessage()"<<endl);
    return 0;
}

/////////////////////////////////////////////////////////////////

void APIWriter::Run()
{
    Writer::Run();

    //write an API message
    assert (fd > 0);

    if (msgQueue.empty())
        return;

    api_msg * msg = msgQueue.front();
    assert (msg);
    msgQueue.pop_front();
    int ret = WriteMessage(msg); //msg is deleted in WriteMessage

    //something is wrong with socket write
    if (ret < 0)
    {
        assert(api_reader);
        api_reader->Close();
        Close();
        return;
    }

    if (msgQueue.size() > 0)
    {
        this->SetObsolete(false);
        this->SetRepeats(1);
    }
}

int APIWriter::WriteMessage (api_msg *msg)
{
    char buf[API_MAX_MSG_SIZE];

    assert (msg);

    // Length of message including header
    int len = sizeof (api_msg_header) + ntohs (msg->header.length);

    // Make contiguous memory buffer for message
    memcpy (buf, &msg->header, sizeof (api_msg_header));
    if (msg->body && ntohs(msg->header.length) > 0)
        memcpy (buf + sizeof (api_msg_header), msg->body, ntohs (msg->header.length));

    if (MSG_CHKSUM(msg->header) != msg->header.chksum)
    {
        LOG("APIWriter(" << fd << ") packet corrupt" <<endl);
        api_msg_delete (msg);
        return -1;
    }

    api_msg_delete (msg);

    int wlen = writen(fd, buf, len);
    if (wlen < 0)
    {
        LOG("APIWriter failed to write " << fd << endl);
        return -1;
    }
    else if (wlen == 0)
    {
        close(fd);
        LOG("Connection closed for APIWriter(" << fd <<')' << endl);
        return -1;
    }
    else if (wlen != len)
    {
        LOG("APIWriter(" << fd << ") cannot write the message" <<endl);
        return -1;
    }
    return 0;
}

void APIWriter::PostMessage (api_msg *msg, bool urgent)
{
    assert (msg);

    if (urgent == URGENT_POST)
        this->msgQueue.push_front(msg);
    else
        this->msgQueue.push_back(msg);

    this->SetRepeats(0);
    this->SetObsolete(false);
    if (msgQueue.size() == 1)
        eventMaster.Schedule(this);
}

////////////////////////////// Global C functions ///////////////////////////
api_msg * api_msg_new (u_int16_t type, u_int16_t length, void *msgbody, u_int32_t ucid, u_int32_t seqnum, u_int32_t tag)
{
    api_msg *msg;

    msg = new (api_msg);
    memset (msg, 0, sizeof(api_msg));

    msg->header.type = htons(type);
    msg->header.length = htons (length);
    msg->header.seqnum = htonl (seqnum);
    msg->header.ucid = htonl(ucid);
    msg->header.tag = htonl(tag);

    if (length > 0 && msgbody != NULL)
    {
        msg->body = new char[length];
        memcpy(msg->body, msgbody, length);
    }
    else 
        msg->body = NULL;

    msg->header.chksum = MSG_CHKSUM(msg->header);
    return msg;
}

api_msg * api_msg_new (u_char msgtype, u_char action, u_int16_t msglen, void *msgbody, u_int32_t ucid, u_int32_t seqnum, u_int32_t tag)
{
    api_msg *msg;

    msg = new (api_msg);
    memset (msg, 0, sizeof(api_msg));

    msg->header.type_8 = msgtype;
    msg->header.action = action;
    msg->header.length = htons (msglen);
    msg->header.seqnum= htonl (seqnum);
    msg->header.ucid = htonl(ucid);
    msg->header.tag = htonl(tag);

    if (msglen > 0 && msgbody != NULL)
    {
        msg->body = new char[msglen];
        memcpy(msg->body, msgbody, msglen);
    }
    else 
        msg->body = NULL;

    msg->header.chksum = MSG_CHKSUM(msg->header);
    return msg;
}

void api_msg_delete (struct api_msg* msg)
{
    assert(msg);

    if (msg->body != NULL && ntohs(msg->header.length) > 0)
        delete []msg->body;
    delete msg;
}

void api_msg_dump (struct api_msg* msg)
{
    LOG_DEBUG("api_msg_dump::TYPE(" <<msg->header.type<<" [type_8=" <<msg->header.type_8 <<": action=("<<msg->header.action
        <<"]) LENGTH("<<ntohs(msg->header.length) <<") SEQ#"<<ntohl(msg->header.seqnum)<<endl);
}

