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

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <arpa/inet.h>
#include "narb_xmlserver.hh"
#include "lsa.hh"
#include "toolbox.hh"


// C function from ast_master
xmlNodePtr findxmlnode(xmlNodePtr cur, char* keyToFound)
{
  if (strcasecmp((char*)cur->name, keyToFound) == 0)
    return cur;

  cur = cur->xmlChildrenNode;
  while (cur) {
    if (cur->xmlChildrenNode)  {
      if (strcasecmp((char*)cur->name, keyToFound) == 0) 
	return cur;
      return findxmlnode(cur, keyToFound);
    } 
    cur = cur->next;
  }

  return NULL;
}

//////////////////////////////////
//  NARB_XMLServer Implementation //
//////////////////////////////////

void NARB_XMLServer::Run()
{
    struct sockaddr_in sa_in;
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    socklen_t len = sizeof(struct sockaddr_in);
    int new_sock = accept (fd, (struct sockaddr *)&sa_in, &len);

    XML_LSP_Broker* broker = new XML_LSP_Broker(new_sock, this);
    broker->SetAutoDelete(true);
    broker->SetRepeats(FOREVER);

    broker_list.push_back(broker);
    eventMaster.Schedule(broker);

    LOG_DEBUG("Accepted an XML API connection on socket(" <<new_sock <<")" << endl);
}

//////////////////////////////////
//  NARB_XMLParser Implementation ///
//////////////////////////////////

XML_LSP_Broker::XML_LSP_Broker(int sock, NARB_XMLServer * server): LSP_Broker(sock, (NARB_APIServer*)server)
{
    api_writer = NULL;
    xml_ibuffer = xml_obuffer = NULL;
    xml_ibufsize = xml_obufsize = 0;
}


XML_LSP_Broker::~XML_LSP_Broker()
{
    if (xml_ibuffer)
        delete[] xml_ibuffer;

    if (xml_obuffer)
        delete[] xml_obuffer;

    // cleanning XML_LSP_Broker's
}


void XML_LSP_Broker::Run()
{
    if (GetWriter() == NULL)
    {
        APIWriter* api_writer = new APIWriter(fd, server);
        api_writer->SetAutoDelete(false);
        api_writer->SetObsolete(false);
        api_writer->SetRepeats(0);
        api_writer->SetReader(this);
        this->SetWriter(api_writer);
    }

    if (xml_ibuffer == NULL)
    {
        xml_ibufsize = ReadXML();

        if (!xml_ibuffer)
        {
            Close();
            api_writer->Close();
            return;
        }

        int ret = ParseAll();
        // return;
    }

    if (lspq_list.size() > 0)
    {
        //find an idle one and start LSPQ request handling
        list<LSPQ*>::iterator iter = lspq_list.begin();
        for ( ; iter != lspq_list.end(); iter++)
        {
            LSPQ *lspq = *iter;
            if (lspq->State() == STATE_IDLE) 
            {
                lspq->HandleLSPQRequest();
                return; //Hanlde one LSPQ each time to be fair to other lsp_brokers
            }
        }

        WaitForAllQueries();
    }
}

int XML_LSP_Broker::ParseAll()
{
    int ret;
    xmlChar *key;
    xmlDocPtr doc;
    xmlNodePtr cur, level1Node;
    char keyToFound[100];
  
    if (xml_ibuffer == NULL || xml_ibuffer <= 0)
        return 0;
  
    doc = xmlParseMemory(xml_ibuffer, xml_ibufsize);
  
    if (doc == NULL) {
        LOGF("NARB_XMLParser::ParseAll(): XML contents in memory were not parsed successfully.\n");
        ret = -1; goto _out;
    }
  
    cur = xmlDocGetRootElement(doc);
  
  
    if (cur == NULL) {
        LOGF("NARB_XMLParser::ParseAll(): No XML contents in memory.\n");
        ret = -1; goto _out;
    }
  
    // Locate "narb" 
  
    strcpy(keyToFound, "narb");
    cur = findxmlnode(cur, keyToFound);
  
    if (!cur) {
        LOGF("NARB_XMLParser::ParseAll(): XML contents in memory do not have <%d>.\n", keyToFound);
        ret = -1; goto _out;
    }

    //break down into sublevel parse
    for (level1Node = cur->xmlChildrenNode; level1Node; level1Node = level1Node->next) 
    {
        key = xmlNodeGetContent(level1Node->xmlChildrenNode);
        if (!key)
            continue;

        if (strcasecmp((char*)key, "lsp") == 0)
        {
            LSPQ* lspq = ParseLSPQuery(level1Node);
            if (lspq != NULL) {
                ret = -1; goto _out;
            }
            // add the broker into a list ...
            lspq_list.push_back(lspq);
        }
        else if (strcasecmp((char*)key, "topology") == 0)
        {
            ret = ParseTopoQuery(level1Node);
            goto _out;
        }
    }

_out:
      xmlFreeDoc(doc);
      xml_ibuffer = 0;
      return ret;
}

extern struct string_value_conversion str_val_conv_switching;
extern struct string_value_conversion str_val_conv_encoding;

LSPQ* XML_LSP_Broker::ParseLSPQuery(xmlNodePtr cur)
{
    LSPQ*lspq = NULL;
    if (!cur) return NULL;
    int ucid = 0;

    xmlNodePtr level1Node, level2Node;
    
    xmlChar* xml_ucid= xmlGetProp(cur, (xmlChar*)"ucid");
    if (xml_ucid)
        sscanf((char*)xml_ucid, "%d", &ucid);

    xmlChar* xml_action= xmlGetProp(cur, (xmlChar*)"action");
    if (xml_action)
    {
        if (strcasecmp((char*)xml_action, "query")==0)
        {
            msg_app2narb_request app_req;
            memset(&app_req, 0, sizeof(struct msg_app2narb_request));
            u_int32_t vtag = 0;
            xmlChar* key;

            app_req.type = htons(MSG_APP_REQUEST);
            app_req.length = htons(sizeof(struct msg_app2narb_request) - 4);        

            //source
            level1Node = findxmlnode(cur, "source");
            if (!level1Node)
                return NULL;
            for (level2Node = level1Node->xmlChildrenNode; level2Node; level2Node = level2Node->next)
            {
                if (strcasecmp((char*)level2Node->name, "router_id") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    inet_aton((char*)key, &app_req.src);
                    xmlFree(key);
                }
            }
            
            //destination
            level1Node = findxmlnode(cur, "destination");
            if (!level1Node)
                return NULL;
            for (level2Node = level1Node->xmlChildrenNode; level2Node; level2Node = level2Node->next)
            {
                if (strcasecmp((char*)level2Node->name, "router_id") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    inet_aton((char*)key, &app_req.dest);  
                    xmlFree(key);
                }
            }

            //te parameters
            level1Node = findxmlnode(cur, "te_params");
            if (!level1Node)
                return NULL;
            for (level2Node = level1Node->xmlChildrenNode; level2Node; level2Node = level2Node->next)
            {
                key = NULL;
                if (strcasecmp((char*)level2Node->name, "bandwidth") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    sscanf((char*)key, "%f", &app_req.bandwidth);
                }
                else if (strcasecmp((char*)level2Node->name, "switching") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    app_req.switching_type = htons(string_to_value(&str_val_conv_switching, (char*)key));
                }
                else if (strcasecmp((char*)level2Node->name, "encoding") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    app_req.encoding_type = htons(string_to_value(&str_val_conv_encoding, (char*)key));
                }
                else if (strcasecmp((char*)level2Node->name, "gpid") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    app_req.gpid = htons(33); //G_Ether by default
                }
                else if (strcasecmp((char*)level2Node->name, "vtag") == 0)
                {
                    key = xmlNodeGetContent(level2Node);
                    sscanf((char*)key, "%d", &vtag);
                }
                if (key)
                    xmlFree(key);
            }

            // ignore local_ids

            // create LSPQ ...
            u_int32_t lsp_opt = LSP_OPT_STRICT | LSP_OPT_PREFERRED| LSP_OPT_BIDIRECTIONAL |LSP_OPT_MRN;
            if (vtag != 0)
                lsp_opt |= LSP_OPT_E2E_VTAG;
            lspq = new LSPQ(this, app_req, ucid, lsp_opt);
            lspq_list.push_back(lspq);
            lspq->SetReqUcid(ucid);
            lspq->SetReqVtag(vtag);
        }
    }

    return lspq;
}

int XML_LSP_Broker::ParseTopoQuery(xmlNodePtr cur)
{
    // Return topology and WriteXML
    return 0;
}

void XML_LSP_Broker::WaitForAllQueries()
{
    bool complete = true;
    list<LSPQ*>::iterator iter;
    for (iter = lspq_list.begin(); iter != lspq_list.end(); iter++)
    {
        LSPQ* lspq = *iter;
        if (lspq->State() != STATE_ERO_COMPLETE || lspq->State() != STATE_ERROR)
        {
            complete = false;
        }
    }
    
    if (complete)
    {
        xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "</narb>");
        xml_obuffer[xml_obufsize++] = '\0';
        int ret = WriteXML();

        Close();
        api_writer->Close();
    }
    //timer ???
}

int XML_LSP_Broker::ReadXML()
{
    int offset = 0, rlen;
    char* swap_buffer = NULL;
    int bufsize = XML_BUF_BLOCK_SIZE;

    if (xml_ibuffer == NULL) {
        xml_ibuffer = new char[bufsize];
        xml_ibufsize = 0;
    }
    if (xml_ibuffer == NULL)
        return -1;

    while ((rlen = readn (fd, xml_ibuffer+offset, XML_BUF_BLOCK_SIZE-1)) > 0 )
    {
        offset += rlen;
        if (offset >= (bufsize -XML_BUF_BLOCK_SIZE/2))
        {
            swap_buffer = xml_ibuffer;
            bufsize += XML_BUF_BLOCK_SIZE/2;
            xml_ibuffer = new char[bufsize];
            if (xml_ibuffer == NULL)
                return -1;          
            memcpy(xml_ibuffer, swap_buffer, offset);
            delete[] swap_buffer;
        }
    }
    xml_ibuffer[offset] = '\0';
    return offset+1;
}

int XML_LSP_Broker::WriteXML()
{
    int ret  = 0;
    if (xml_obuffer == NULL || xml_obufsize == 0)
        return ret;

    ret = writen(fd, xml_obuffer, xml_obufsize);
    if (ret == xml_obufsize)
        xml_obufsize = 0;

    return ret;
}

int XML_LSP_Broker::HandleReplyMessage (api_msg* rmsg)
{
    int ret = -1;
    int bufsize = XML_BUF_BLOCK_SIZE;
    char* swap_buffer = NULL;

    assert(rmsg);

    if (xml_obuffer == NULL) {
        xml_obuffer = new char[bufsize];
        xml_obufsize = 0;
        if (xml_obuffer == NULL)
            return ret;
        xml_obufsize += sprintf(xml_obuffer, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<narb version=\"1.0\">\n");
    }
    

    if (xml_obufsize >= (bufsize -XML_BUF_BLOCK_SIZE/2))
    {
        swap_buffer = xml_ibuffer;
        bufsize += XML_BUF_BLOCK_SIZE/2;
        xml_ibuffer = new char[bufsize];
        if (xml_ibuffer == NULL)
            return ret;
        memcpy(xml_ibuffer, swap_buffer, xml_obufsize);
        delete[] swap_buffer;
    }

    //XML output to xml_obuffer and update xml_obufsize
    u_int32_t errcode = 0;
    if (ntohs(rmsg->header.type) == MSG_REPLY_ERROR)
    {
        errcode = ntohl(*(u_int32_t*)(rmsg->body + sizeof(TLV_HDR_SIZE)));
    }
    xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "<lsp ucid=\"%d\" action=\"query-reply\">\n<return_code>%d<return_code>\n", 
        ntohl(rmsg->header.ucid), errcode);

    if (errcode ==0)
    {
        PrintXML_ERO((te_tlv_header*)rmsg->body);
    }
    else
    {
        PrintXML_ErrCode((te_tlv_header*)rmsg->body);
    }

    xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "</lsp>\n");
}

void XML_LSP_Broker::PrintXML_ERO (te_tlv_header* tlv_ero)
{
    list<ero_subobj*> ero;
    LSPQ::GetERO_RFCStandard(tlv_ero, ero);

    xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "<ero>\n");

    ero_subobj* subobj;
    char ipbuf[20];
    list<ero_subobj*>::iterator iter;
    for (iter = ero.begin(); iter != ero.end(); iter++)
    {
        subobj = *iter;

        bool loose = ((subobj->hop_type >> 7) == ERO_TYPE_LOOSE_HOP);
        strcpy(ipbuf, inet_ntoa(subobj->addr));
        switch (subobj->hop_type & 0x7f)
        {
        case 1: //IPv4
            xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "<ipv4 loose=\"%s\" ip=\"%s\" />\n",
                loose? "false":"true", ipbuf);
            break;
        case 4:
            xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "<unmum loose=\"%s\" ip=\"%s\" ifid=\"%d\" />\n",
                loose? "false":"true", ipbuf, subobj->l2sc_vlantag ==0?0:(0x0400&subobj->l2sc_vlantag));
            break; //Unnum
        default: // Unrecognized
            break;
        }

        xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "</ero>\n");
    }
}

void XML_LSP_Broker::PrintXML_ErrCode (te_tlv_header* tlv_ero)
{
    //const char* errmsg =  error_code_to_cstr(ntohl(*(u_int32_t*)(tlv_ero + sizeof(TLV_HDR_SIZE))));
    //xml_obufsize += sprintf(xml_obuffer+xml_obufsize, "<error_message>%s</error_message>\n", errmsg); 
}

