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

#ifndef __RCE_API_H__
#define __RCE_API_H__

#include <iostream>
#include "rce_types.hh"
#include "event.hh"
#include "toolbox.hh"
#include "rce_apiserver.hh"

#define API_MAX_MSG_SIZE 6000

enum api_msg_type
{
    MSG_LSP = 0x01,
    MSG_LSA = 0x02,
    MSG_AAA = 0x03,
    MSG_RM = 0x04,
    MSG_CTRL = 0x05
};

enum api_action
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
    //extensions:
    ACT_INIT = 0x0A,
    ACT_ALIVE = 0x0B,
    //0xF0 -- 0xFE reserved for NARB-RCE
    ACT_QUERY_MRN = 0xF0
};

struct api_msg_header
{
    u_char msgtype; // (e.g. LSA queries/ LSP queries)
    u_char action; // (e.g. Request/Insert/Query/Delete/Update
    u_int16_t msglen; 
    u_int32_t ucid; //  unique client id
    u_int32_t msgseq;
    u_int32_t chksum;
    union {
        u_int32_t msgtag[2];
        struct {
            u_int32_t options;
            u_int32_t tag;
        };
    };
};

#define MSG_CHKSUM(X) (((u_int32_t*)&X)[0] + ((u_int32_t*)&X)[1] + ((u_int32_t*)&X)[2])

struct api_msg
{
    api_msg_header hdr;
    char* body;
};

class APIServer;
class APIWriter;
class APIReader: public Reader
{
private:
    APIServer *server;
    APIWriter* api_writer;
public:
    APIReader(int fd, APIServer *server_ptr):Reader(fd), server(server_ptr) 
        { assert(server_ptr); }
    virtual ~APIReader() {}
    virtual void Run ();
    APIWriter* GetWriter() { return api_writer; }    
    void SetWriter(APIWriter* writer) { api_writer = writer; }
    api_msg * ReadMessage();
    void HandleMessage (api_msg *);
};

#define URGENT_POST true

class APIWriter: public Writer
{
private:
    APIServer *server;
    list<api_msg *> msgQueue;
    APIReader* api_reader;
public:
    APIWriter(int fd, APIServer *server_ptr): Writer(fd), server(server_ptr) 
        { assert(server_ptr); }
    virtual ~APIWriter() {}
    virtual void Run ();
    APIReader* GetReader() { return api_reader; }    
    void SetReader(APIReader* reader) { api_reader = reader; }
    int WriteMessage(api_msg *msg);
    void PostMessage(api_msg *msg, bool urgent = (!URGENT_POST));
};

#endif
