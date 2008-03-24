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
#include "rce_lsp.hh"
#include "rce_api.hh"
#include "rce_apiserver.hh"

void APIReader::Run()
{
    Reader::Run();

    //read an API message
    if (fd < 0)
        return;

    api_msg * msg = ReadMessage();

    //something is wrong with socket read
    if (!msg)
    {
        Close();
        assert(api_writer);
        api_writer->Close();
        return;
    }

    //api_msg_dump(msg);

    //Message handler    
    HandleMessage(msg); 
}

api_msg * APIReader::ReadMessage ()
{
    api_msg* msg;
    api_msg_header hdr;
    char buf[API_MAX_MSG_SIZE];
    int bodylen;
    int rlen;

    /* Read message header */
    rlen = readn (fd, (char *) &hdr, sizeof (api_msg_header));

    if (rlen < 0)
    {
        LOG("APIReader failed to read from"<<fd<<endl);
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

    if (MSG_CHKSUM(hdr) != hdr.chksum)
    {
        LOG("APIReader(" << fd << ") packet corrupt" <<endl);
        return NULL;
    }

    // Determine body length. 
    bodylen = ntohs (hdr.msglen);
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
    msg = api_msg_new (hdr.msgtype, hdr.action, buf, ntohl(hdr.ucid), ntohl (hdr.msgseq), ntohs (hdr.msglen));
    msg->hdr.msgtag[0] = hdr.msgtag[0];
    msg->hdr.msgtag[1] = hdr.msgtag[1];
    return msg;
}

void APIReader::HandleMessage (api_msg *msg)
{
    LSAHandler * lsaEvent;
    LSARetriever * lsarEvent;
    LSPHandler * lspEvent;
    api_msg *rmsg;
    u_int32_t ack_id;
            
    switch (msg->hdr.msgtype)
    {
    case MSG_LSA:

        switch(msg->hdr.action)
        {
        case ACT_UPDATE:
            assert(api_writer);
            ack_id = 0x0f;
            rmsg = api_msg_new(MSG_LSA, ACT_ACK, &ack_id, ntohl(msg->hdr.ucid), ntohl(msg->hdr.msgseq), 4);
            api_writer->PostMessage(rmsg); //temp: always return a MSG_ACK
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
        case ACT_QUERY:
            assert(api_writer);
            lsarEvent = new LSARetriever;
            if (lsarEvent->Parse(msg))
            {
                ack_id = 0;
                rmsg = api_msg_new(MSG_LSA, ACT_ACK, &ack_id, ntohl(msg->hdr.ucid), ntohl(msg->hdr.msgseq), 4);
                api_writer->PostMessage(rmsg); //temp: always return a MSG_ACK
                lsarEvent->SetApiReader(this);
                lsarEvent->SetApiWriter(api_writer);
                lsarEvent->SetAutoDelete(true);
                eventMaster.Schedule(lsarEvent);
            }
            else
            {
                ack_id = 0;
                rmsg = api_msg_new(MSG_LSA, ACT_ERROR, &ack_id, ntohl(msg->hdr.ucid), ntohl(msg->hdr.msgseq), 4);
                api_writer->PostMessage(rmsg); //temp: always return a MSG_ACK
                delete lsarEvent;
            }
            break;
        } // end of inner switch
        break;

     case MSG_LSP:
        switch(msg->hdr.action)
        {
        case ACT_QUERY:
        case ACT_QUERY_MRN:
            lspEvent = new LSPHandler(fd);
            lspEvent->SetAutoDelete(true);
            assert(api_writer);
            lspEvent->AssociateWriter(api_writer);
            lspEvent->Load(msg);
            eventMaster.Schedule(lspEvent);
            break;
        case ACT_CONFIRM:
        case ACT_DELETE:
        case ACT_UPDATE:
            LSPHandler::HandleResvNotification(msg);
            break;
        }
        break;
     case MSG_AAA:
        break;
     case MSG_RM:
        break;
    default:
        LOG_CERR<<"Unkonwn msg type "<<msg->hdr.msgtype<<endl;
    }
    
}

void APIWriter::Run()
{
    Writer::Run();

    //write an API message
    if (fd < 0)
        return;

    if (msgQueue.empty())
        return;

    api_msg * msg = msgQueue.front();
    assert (msg);
    msgQueue.pop_front();
    int ret = WriteMessage(msg); //msg is deleted in WriteMessage

    //something is wrong with socket write
    if (ret < 0)
    {
        Close();
        assert(api_reader);
        api_reader->Close();
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
    assert (msg->body);

    // Length of message including header
    int len = sizeof (api_msg_header) + ntohs (msg->hdr.msglen);

    // Make contiguous memory buffer for message
    memcpy (buf, &msg->hdr, sizeof (api_msg_header));
    memcpy (buf + sizeof (api_msg_header), msg->body, ntohs (msg->hdr.msglen));

    if (MSG_CHKSUM(msg->hdr) != msg->hdr.chksum)
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

