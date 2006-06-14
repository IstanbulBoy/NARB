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


#ifndef __NARB_XMLSERVER_H__
#define __NARB_XMLSERVER_H__

#include "types.hh"
#include "event.hh"
#include "apiserver.hh"
#include "narb_config.hh"
#include "lsp_broker.hh"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;
#include <list>

class XML_LSP_Broker;
class NARB_XMLServer: public APIServer
{
public:
    NARB_XMLServer(u_int32_t port_val):APIServer(port_val){}
    virtual ~NARB_XMLServer() { }
    virtual void Run();

private:
    //list<XML_LSP_Broker*> broker_list;    // XML parser (one per XML interface connection) list

};

#define XML_BUF_BLOCK_SIZE 8192 //8 KB

class XML_LSP_Broker: public LSP_Broker
{
public:
    XML_LSP_Broker(int sock, NARB_XMLServer* server);
    virtual ~XML_LSP_Broker();
    virtual void Run();
    virtual int HandleReplyMessage (api_msg* msg);

    int ParseAll();
    LSPQ* ParseLSPQuery(xmlNodePtr cur);
    int ParseTopoQuery(xmlNodePtr cur);
    void WaitForAllQueries();
    int ReadXML();
    int WriteXML();
    void PrintXML_ERO (te_tlv_header* tlv_ero);
    void PrintXML_ErrCode(te_tlv_header* tlv_ero);

private:
    //list<LSPQ*> lspq_list;
    char* xml_ibuffer;
    int xml_ibufsize;
    char* xml_obuffer;
    int xml_obufsize;
};

inline char* skip_xml_space(xmlChar* key)
{
    char *ptr = (char*)key;
    while(ptr && isspace(*ptr)) 
        ptr++;
    return ptr;
}

#endif

