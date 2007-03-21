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
#ifndef __NARB_APISERVER_H__
#define __NARB_APISERVER_H__
#include "types.hh"
#include "event.hh"
#include "apiserver.hh"
#include "lsa.hh"

class LSP_Broker;
struct ero_subobj;
class NARB_APIServer: public APIServer
{
public:
    static list<LSP_Broker*> lsp_brokers;
    
public:
   NARB_APIServer(int port): APIServer(port) {}
   virtual ~NARB_APIServer();
   virtual void Run();

   static LSP_Broker * LspBrokerLookup(u_int32_t id);
   static LSP_Broker * LspBrokerLookupBySocket(int sock);

   static LSP_Broker* NARB_APIServer::LspqLookup (u_int32_t ucid, u_int32_t seqnum);
};

struct msg_app2narb_vtag_mask;

extern "C" {
    api_msg * narb_new_msg_reply_error (u_int32_t ucid, u_int32_t seqnr, u_int32_t errorcode);
    api_msg * narb_new_msg_reply_ero (u_int32_t ucid, u_int32_t seqnr, list<ero_subobj*>& ero, msg_app2narb_vtag_mask* vtagmask);
    api_msg * narb_new_msg_reply_release_confirm (u_int32_t ucid, u_int32_t seqnr);
    void narb_extract_ero_tlv (te_tlv_header& ero_tlv, list<ero_subobj*>& ero);
}


#endif
