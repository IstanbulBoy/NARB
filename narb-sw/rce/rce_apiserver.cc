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

#include "rce_api.hh"
#include "rce_apiserver.hh"
#include "rce_log.hh"

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

    // Listen under queue length 10
    ret = listen (sock, 9);
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

void APIServer::Run()
{
    struct sockaddr_in sa_in;
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    socklen_t len = sizeof(struct sockaddr_in);
    int new_sock = accept (fd, (struct sockaddr *)&sa_in, &len);
    if (new_sock < 0)
    {
        LOGF("APIServer::Run: cannot accept socket on %d\n", fd);
        return;
    }

    APIWriter* api_writer = new APIWriter(new_sock, this);
    api_writer->SetAutoDelete(false); //$$$$
    api_writer->SetRepeats(0); //$$$$

   APIReader* api_reader = new APIReader(new_sock, this);
    api_reader->SetAutoDelete(true);
    api_reader->SetRepeats(FOREVER);
    api_reader->SetWriter(api_writer);
    api_writer->SetReader(api_reader);
    
    //eventMaster.Schedule(api_writer);
    //Schedule a reader event for each accepted connection
    eventMaster.Schedule(api_reader);

    LOG_DEBUG("Accepted a connection on socket(" <<new_sock <<")" << endl);
}

