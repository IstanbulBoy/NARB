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

#include "log.hh"
#include "toolbox.hh"
#include "apiclient.hh"
#include "lsp_broker.hh"
#include <errno.h>

APIClient::APIClient():APIReader(-1, NULL), _port(0) 
{
    api_writer = new APIWriter(-1, NULL);
    api_writer->SetReader(this);
}

APIClient::APIClient(char * host, int port):APIReader(-1, NULL)
{
    _host = host; _port = port;
    api_writer = new APIWriter(-1, NULL);
    api_writer->SetReader(this);
}

APIClient::~APIClient()
{
    if (fd > 0)
        Close();

    if (api_writer);
    {
	api_writer->Close();
        delete api_writer;
    }
}

void APIClient::SetHostPort(char *host, int port)
{
    _host = host;
    _port = port;
}

int APIClient::Connect(char *host, int port)
{
      struct sockaddr_in addr;
      struct hostent *hp;
      int ret;
      int size;
      int on = 1;

    _host = host;
    _port = port;
  
      assert (strlen(host) > 0 || port > 0);
  	
      hp = gethostbyname (host);
      if (!hp)
      {
          fprintf (stderr, "APIClient::Connect: no such host %s\n", host);
          return (-1);
      }
  
      fd = socket (AF_INET, SOCK_STREAM, 0);
      if (fd < 0)
      {
  	  LOGF("APIClient::Connect: socket(): %s", strerror (errno));
  	  return (-1);
      }
                                                                                 
      /* Reuse addr and port */
      ret = setsockopt (fd, SOL_SOCKET,  SO_REUSEADDR, (void *) &on, sizeof (on));
      if (ret < 0)
      {
          fprintf (stderr, "APIClient::Connect: SO_REUSEADDR failed\n");
          close (fd);
          return (-1);
      }
  
  #ifdef SO_REUSEPORT
    ret = setsockopt (fd, SOL_SOCKET, SO_REUSEPORT,
                      (void *) &on, sizeof (on));
    if (ret < 0)
    {
        fprintf (stderr, "APIClient::Connect: SO_REUSEPORT failed\n");
        close (fd);
        return (-1);
    }
  #endif /* SO_REUSEPORT */
  
    /* Prepare address structure for connect */
    memset (&addr, 0, sizeof (struct sockaddr_in));
    memcpy (&addr.sin_addr, hp->h_addr, hp->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (port);
    //addr.sin_len = sizeof (struct sockaddr_in);
   
    /* Now establish synchronous channel with OSPF daemon */
    ret = connect (fd, (struct sockaddr *) &addr,
                   sizeof (struct sockaddr_in));
    if (ret < 0)
    {
        LOGF("APIClient::Connect: connect(): %s", strerror (errno));
        close (fd);
        return (-1);
    }

    // Schedule reader
    assert(api_writer);
    api_writer->SetSocket(fd);
    this->SetRepeats(FOREVER);
    eventMaster.Schedule(this);
    api_writer->SetObsolete(false);
    api_writer->SetRepeats(0);
    //api_writer is scheduled by PostMessage

    return fd;
}

int APIClient::Connect()
{
    LOG("API Client connecting ... \n");

    if (_host.size() == 0 || _port == 0)
        return -1;

    if (fd > 0)
    {
        if (api_writer)
            api_writer->Close();
        Close();
    }
    if (Connect((char*)(_host.c_str()), _port) < 0)
    {
        return -1;
    }

    return 0;
}

bool APIClient::IsAlive()
{
    return (fd > 0 && !Obsolete());
}

void APIClient::SendMessage(api_msg *msg)
{
    assert (api_writer);
    api_writer->PostMessage(msg);
}

LSPQ* APIClient::FindOwnerLSPQ(api_msg *msg)
{
    assert(msg);
    return NARB_APIServer::LspqLookup(ntohl(msg->header.ucid), ntohl(msg->header.seqnum));
}

