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

#ifndef __RCE_APICLIENT_H__
#define __RCE_APICLIENT_H__

#include <iostream>
#include "types.hh"
#include "event.hh"
#include "apiserver.hh"
#include "apiclient.hh"
#include "lsa.hh"

enum rce_api_msg_type
{
    MSG_LSP = 0x01,
    MSG_LSA = 0x02,
    MSG_AAA = 0x03,
    MSG_RM = 0x04,
    MSG_CTRL = 0x05
};

enum rce_api_action
{
    ACT_NOOP = 0x00,
    ACT_QUERY = 0x01,
    ACT_INSERT = 0x02,
    ACT_DELETE = 0x03,
    ACT_UPDATE = 0x04,
    ACT_ACK = 0x05,
    ACT_ACKDATA = 0x06,
    ACT_ERROR = 0x07,
    ACT_CONFIRM = 0x08,
    //extensions
    ACT_INIT = 0x0A,
    ACT_ALIVE = 0x0B,
    ACT_QUERY_MRN = 0x10
};
#define ACT_ORIGINATE ACT_INSERT

struct msg_narb_cspf_request;
struct msg_app2narb_request;
struct msg_narb_vtag_mask;
struct ero_subobj;
struct dtl_hop;
class RCE_APIClient: public APIClient
{

public:
    RCE_APIClient(){}
    RCE_APIClient(char* host, int port): APIClient(host, port) {}
    virtual ~RCE_APIClient();
    virtual int HandleMessage(api_msg * msg);
    bool IsMatched(char* host, int port);
    void QueryLsp (msg_narb_cspf_request & cspf_req, u_int32_t ucid, u_int32_t options, u_int32_t vtag = 0, u_int32_t hop_back = 0, u_int32_t src_lcl_id = 0, u_int32_t dest_lcl_id = 0, msg_narb_vtag_mask* vtag_bitmask = NULL,
        list<ero_subobj*>* p_subnet_ero = NULL, list<ero_subobj*>* p_user_ero = NULL);
    void QueryLsp_MRN (msg_narb_cspf_request & cspf_req, msg_app2narb_request &mrn_spec, u_int32_t ucid, u_int32_t options, u_int32_t vtag = 0, u_int32_t hop_back = 0, msg_narb_vtag_mask* vtag_bitmask = NULL);

    void NotifyResvStateWithERO(u_int8_t type, u_int8_t action, msg_app2narb_request* msg_req, list<ero_subobj*>& ero_forward,  list<ero_subobj*>& subnet_ero, list<dtl_hop>& subnet_dtl,  u_int32_t ucid, u_int32_t seqnum, 
        u_int32_t options, u_int32_t msgtag, u_int32_t src_lcl_id = 0, u_int32_t dest_lcl_id = 0, msg_narb_vtag_mask* vtag_mask_tlv = NULL, u_int32_t holding_time = 0);
    void NotifyResvStateWithERO(u_int8_t type, u_int8_t action, api_msg* msg, list<ero_subobj*>& ero_forward, list<ero_subobj*>& subnet_ero, list<dtl_hop>& subnet_dtl, u_int32_t src_lcl_id, u_int32_t dest_lcl_id)
        {
            NotifyResvStateWithERO(type, action, (msg_app2narb_request*)msg->body, ero_forward, subnet_ero, subnet_dtl, ntohl(msg->header.ucid), ntohl(msg->header.seqnum), ntohl(msg->header.options), ntohl(msg->header.tag), src_lcl_id, dest_lcl_id);
        }
};


class RCE_APIClient_Factory
{
private:
    static RCE_APIClient_Factory factory;
    list<RCE_APIClient*> clients;
    RCE_APIClient_Factory() { }
    ~RCE_APIClient_Factory();
public:
    static RCE_APIClient_Factory& Instance() {return factory;}
    RCE_APIClient * Create(char *host, int port);
    RCE_APIClient * GetClient(char *host, int port);
    void RemoveClient(RCE_APIClient * client);
};

#define RceFactory RCE_APIClient_Factory::Instance()

#endif

