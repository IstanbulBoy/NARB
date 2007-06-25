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
#ifndef __NARB_APICLIENT_HH__
#define __NARB_APICLIENT_HH__

#include <iostream>
#include "types.hh"
#include "event.hh"
#include "apiserver.hh"
#include "apiclient.hh"

class NARB_APIClient: public APIClient
{
private:
    list<in_addr> associated_addrs;
public:
    NARB_APIClient() {}
    NARB_APIClient(char* host, int port):APIClient(host, port) {}
    virtual int HandleMessage(api_msg* msg);
    bool IsMatched(char* host, int port);
    bool IsAssociated(in_addr ip);
    void AssociateTEAddr(in_addr ip);
    void QueryLspRecursive (msg_narb_recursive_cspf_request &rec_cspf_req, u_int32_t ucid, u_int32_t options, u_int32_t vtag = 0, u_int32_t hop_back = 0, msg_app2narb_vtag_mask* vtag_mask = NULL);
    void RelayMessageToPeer(u_int16_t type, api_msg* msg, list<ero_subobj*>& ero_forward);
};

class NARB_APIClient_Factory
{
private:
    static NARB_APIClient_Factory factory;
    list<NARB_APIClient*> peers;
    NARB_APIClient_Factory() { }
    ~NARB_APIClient_Factory();
public:
    static NARB_APIClient_Factory& Instance() {return factory;}
    NARB_APIClient * Create(char *host, int port);
    NARB_APIClient * GetClient(char *host, int port);
    NARB_APIClient * GetClient(in_addr ip);
    void RemoveClient(NARB_APIClient * client);
    NARB_APIClient * Lookup(in_addr ip);
};

#define NarbFactory NARB_APIClient_Factory::Instance()

#endif

