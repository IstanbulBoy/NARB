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
#include "rce_types.hh"
#include "rce_log.hh"
#include "rce_api.hh"
#include "rce_lsp.hh"
#include "rce_lsa.hh"
#include <netinet/in.h>
#include "toolbox.hh"
#include "zebra_ospfclient.hh"
 
#ifndef HAVE_DECL_GETOPT
  #define HAVE_DECL_GETOPT 1
#endif
#include <getopt.h>


char host[20];
int port = 2678;
int sock = -1;
in_addr source;
in_addr destination;
u_int8_t swtype = 51;
u_int8_t encoding = 2;
float bandwidth = 500; //Mbps
u_int32_t vtag = 0x0000ffff;
u_int32_t opt_bidirectional = LSP_OPT_BIDIRECTIONAL;
u_int32_t opt_strict = LSP_OPT_STRICT;
u_int32_t opt_preferred = LSP_OPT_PREFERRED;
u_int32_t opt_mrn = 0;
u_int32_t opt_e2e_vlan = 0;
u_int32_t opt_req_all_vtags = 0;
u_int32_t opt_query_hold = 0;

struct option longopts[] = 
{
    { "host",        no_argument,       NULL, 'H'},
    { "port",     no_argument,       NULL, 'P'},
    { 0 }
};

void usage()
{
    cout<<"RCE Tester Usage:"<<endl;
    cout<<"\t rce_test [-H host] [-P port] [-S source] [-D dest] [-B] [-M] [-b bandwidth] [-x switching type] [-e encoding type] ";
    cout<<"  ( [-S source] and [-D dest] are mandatory [-U]: unidirectional [-M]: multi-region-network routing" <<endl;
    cout<<"  [-v]: E2E VLAN with speicified tag [-V]: E2E VLAN with tag picked by RCE [-Q]: query and hold" <<endl;
}

int rceapi_connect ()
{
    struct sockaddr_in addr;
    struct hostent *hp;
    int fd;
    int ret;
    int size;
    int on = 1;
	
    hp = gethostbyname (host);
    if (!hp)
    {
        cerr <<  "rceapi_connect: no such host " <<  host << endl;
        return (-1);
    }

    fd = socket (AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
	  cerr <<"rceapi_connect: socket():" << strerror (errno) << endl;
	  return (-1);
    }

                                                                                
    ret = setsockopt (fd, SOL_SOCKET,
                      SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
        cerr << "rceapi_connect: SO_REUSEADDR failed" <<endl;
        close (fd);
        return (-1);
    }
  
  #ifdef SO_REUSEPORT
    ret = setsockopt (fd, SOL_SOCKET, SO_REUSEPORT,
                      (void *) &on, sizeof (on));
    if (ret < 0)
    {
        cerr << "rceapi_connect: SO_REUSEPORT failed" <<endl;
        close (fd);
        return (-1);
    }
  #endif /* SO_REUSEPORT */
  
    /* Prepare address structure for connect */
    memset (&addr, 0, sizeof (struct sockaddr_in));
    memcpy (&addr.sin_addr, hp->h_addr, hp->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (port);
  
    ret = connect (fd, (struct sockaddr *) &addr,
                   sizeof (struct sockaddr_in));
    if (ret < 0)
    {
        cerr << "rceapi_connect: connect(): " << strerror (errno);
        close (fd);
        return (-1);
    }
  
    return fd;
}


struct api_msg * rceapi_read (int fd)
{
    static char buf[API_MAX_MSG_SIZE];
    int ret;
    struct api_msg_header hdr;
    int bodylen;
    int rlen;
    struct api_msg *msg;
    
    /* Read message header */
    rlen = readn (fd, (char *) &hdr, sizeof (struct api_msg_header));
  
    if (rlen < 0)
    {
        LOGF("RCE APIReader failed to read from %d\n", fd);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOGF("Connection closed for APIReader(%d)\n", fd);
        return NULL;    
    }
    else if (rlen != sizeof (struct api_msg_header))
    {
        LOGF("RCE APIReader(%d) cannot read the message header\n", fd);
        return NULL;
    }
  
    if (MSG_CHKSUM(hdr) != hdr.chksum)
    {
        LOGF("RCE APIReader(%d) packet corrupt \n", fd);
        return NULL;
    }
  
    /* Determine body length. */
    bodylen = ntohs (hdr.msglen);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOGF("RCE APIReader(%d) cannot read oversized packet\n", fd);
        return NULL;
    }
      
    if (bodylen > 0)
    {
        /* Read message body*/
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
  	     LOGF("RCE APIReader failed to read from %d\n", fd);
            return NULL;
        }
        else if (rlen == 0)
        {
  	     LOGF("Connection closed for APIReader(%d)\n", fd);
            return NULL;
        }
        else if (rlen != bodylen)
        {
            LOGF("APIReader(%d) cannot read the message body. \n", fd);
            return NULL;
        }
    }
  
    /* Allocate new message*/
    msg = api_msg_new (hdr.msgtype, hdr.action, buf, ntohl (hdr.ucid), ntohl (hdr.msgseq), ntohs (hdr.msglen), ntohl (hdr.tag));
    return msg;
}

int rceapi_send (int fd, struct api_msg *msg)
{
  static char buf[API_MAX_MSG_SIZE];
  int len, wlen;
  int ret = 0;

  assert (fd > 0);  
  assert (msg);
  assert (msg->body);

  /* Length of message including header*/
   len = sizeof (struct api_msg_header) + ntohs (msg->hdr.msglen);

  /* Make contiguous memory buffer for message */
  memcpy (buf, &msg->hdr, sizeof (struct api_msg_header));
  memcpy (buf + sizeof (struct api_msg_header), msg->body, ntohs (msg->hdr.msglen));

  if (MSG_CHKSUM(msg->hdr) != msg->hdr.chksum)
  {
      LOGF("RCE APIWriter(%d) packet corrupt\n", fd);
      return -1;
  }

  wlen = writen(fd, buf, len);
  if (wlen < 0)
    {
      LOGF("RCE APIWriter failed to write %d\n",fd);
      return -1;
    }
  else if (wlen == 0)
    {
      LOGF("Connection closed for APIWriter(%d)\n", fd);
      return -1;
    }
  else if (wlen != len)
    {
      LOGF("RCE APIWriter(%d) cannot write the message\n", fd);
      return -1;
    }

  return ret;
}

/* data structure of APP->NARB request message */
struct msg_app2narb_request
{
  u_int16_t type;
  u_int16_t length;
  struct in_addr src;
  struct in_addr dest;
  u_int8_t  encoding_type;
  u_int8_t  switching_type;
  u_int16_t gpid;
  float bandwidth;
};

/* structure of NARB CSPF request message (msg body only)*/
struct msg_narb_cspf_request 
{
  u_int32_t narb_apiserv_id;
  u_int32_t app_seqnum;
  struct in_addr area_id;
  struct msg_app2narb_request app_req_data;
}; 

msg_narb_cspf_request * new_cspf_request()
{
  msg_narb_cspf_request * cspf_req = new (struct msg_narb_cspf_request);
  
  static int seqno = 1;
  cspf_req->app_seqnum = htonl(seqno++);
  cspf_req->area_id.s_addr = 0;
  cspf_req->narb_apiserv_id = 0;
  cspf_req->app_req_data.type = htons(0x02);
  cspf_req->app_req_data.length = htons(sizeof(struct msg_app2narb_request) - TLV_HDR_SIZE);
  cspf_req->app_req_data.src.s_addr = source.s_addr;
  cspf_req->app_req_data.dest.s_addr = destination.s_addr;
  cspf_req->app_req_data.bandwidth = bandwidth;
  cspf_req->app_req_data.encoding_type = encoding;
  cspf_req->app_req_data.switching_type = swtype;
  cspf_req->app_req_data.gpid = htons(8);

  return cspf_req;
}


api_msg* rceapi_query_lsp (u_int32_t options, u_int32_t ucid, u_int32_t seqnum, msg_narb_cspf_request *cspf_req)
{
  struct api_msg *rce_msg;

  rce_msg = api_msg_new(MSG_LSP, ACT_QUERY,  (void*)&(cspf_req->app_req_data), ucid, seqnum, sizeof(cspf_req->app_req_data), vtag);
  rce_msg->hdr.msgtag[0] = htonl(options | LSP_TLV_NARB_REQ |  opt_bidirectional | opt_strict | opt_preferred | opt_mrn | opt_e2e_vlan
    |opt_req_all_vtags|opt_query_hold);
  
  if (rceapi_send(sock, rce_msg) < 0)
  {
    LOGF("Send MSG to RCE failed\n");
    return NULL;
  }

  rce_msg = rceapi_read(sock);

  if (!rce_msg)
    LOGF("Read MSG from RCE failed\n");

  return rce_msg;
}

#define  TLV_TYPE_NARB_ERO  0x03
#define  TLV_TYPE_NARB_ERROR_CODE  0x04

struct ero_subobj
{
    struct in_addr addr;
    u_char hop_type;
    u_char prefix_len;
    u_char pad[2];
    u_int32_t if_id;
    //added parameters in the private, composite ERO sub-object
    u_char sw_type;
    u_char encoding;
    union {
        u_int16_t lsc_lambda;
        u_char tdm_indication;
        u_int16_t l2sc_vlantag;
        u_int16_t psc_mtu;
    };
    float bandwidth;
};

static const char* err_cstrs[] = {
	"Unrecognized Error Code",
	"Unknown Source Address",
	"Unknown Destination Address",
	"No Routing Path Found",
	"Corrupt Req. Message Packet",
	"Invalid Path Request",
	"System Warming Up",
	"Max. Retransmission of Request Exceeded",
};

const char* error_code_to_cstr(u_int32_t errcode)
{
	if (0 < errcode  && 8 > errcode)
		return err_cstrs[errcode];
	return err_cstrs[0];
}
ZebraOspfSync *zebra_client_inter = NULL;
ZebraOspfSync *zebra_client_intra = NULL;

int main(int argc, char* argv[])
{
    char log_file[20];
    u_int32_t pid = getpid();
    sprintf(log_file, "test_rce-%d.log", pid);    
    Log::Init(LOG_ALL, log_file);
    Log::SetDebug(true);

    strcpy(host, "localhost");
    source.s_addr = destination.s_addr = 0;
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "H:P:S:D:b:x:e:v:LOMQUVa", longopts, 0);
        if (opt == EOF)
            break;

        switch (opt) 
        {
        case 'H':
            strcpy(host, optarg);
            break;
        case 'P':
            sscanf(optarg, "%d", &port);
            break;
        case 'b':
            sscanf(optarg, "%f", &bandwidth);
            break;            
        case 'x':
            sscanf(optarg, "%d", &swtype);
            break;
        case 'e':
            sscanf(optarg, "%d", &encoding);
            break;
        case 'v':
            opt_e2e_vlan|= LSP_OPT_E2E_VTAG;
            sscanf(optarg, "%d", &vtag);
            break;
        case 'S':
            inet_aton(optarg, &source);
            break;
        case 'U':
            opt_bidirectional = 0;
            break;
        case 'D':
            inet_aton(optarg, &destination);
            break;
        case 'L':
            opt_strict = 0;
            break;
        case 'O':
            opt_preferred = 0;
            break;
        case 'M':
            opt_mrn |= LSP_OPT_MRN;
            break;
        case 'Q':
            opt_query_hold = LSP_OPT_QUERY_HOLD;
            break;
        case 'V':
            opt_e2e_vlan|= LSP_OPT_E2E_VTAG;
            break;
        case 'a':
            opt_req_all_vtags |= LSP_OPT_REQ_ALL_VTAGS;
            break;
        default:
            usage();
            exit(1);
            break;
        }
    }

    if (source.s_addr == 0 || destination.s_addr == 0)
    {
        usage();
        exit(1);
    }

    if ((sock = rceapi_connect()) < 0)
        exit(2);


    msg_narb_cspf_request * cspf_req = new_cspf_request();
    api_msg * rce_reply =  rceapi_query_lsp(0, 0, 0, cspf_req);

    int len;
    ero_subobj* subobj;
    char addr[20];
    if (rce_reply)
    {
        assert(rce_reply->hdr.msgtype == MSG_LSP);
        te_tlv_header * tlv = (te_tlv_header*)rce_reply->body;
        switch (ntohs(tlv->type))
        {
        case TLV_TYPE_NARB_ERO:
            assert (rce_reply->hdr.action = ACT_ACKDATA);
            LOGF("Request successful! ERO returned...\n");
            len = ntohs(tlv->length);
            assert( len > 0 && len% sizeof(ero_subobj) == 0);

            subobj  = (ero_subobj *)((char *)tlv + sizeof(struct te_tlv_header));
            for (; len > 0 ;subobj++, len -= sizeof(ero_subobj))
            {
                inet_ntop(AF_INET, &subobj->addr, addr, 20);
                LOGF("HOP-TYPE [%s]: %s\n", subobj->hop_type?"loose":"strict", addr);
            }
            if (opt_e2e_vlan)
                LOGF("E2E VLAN TAG [ %d ]\n", ntohl(rce_reply->hdr.tag));
            if (vtag == ANY_VTAG && opt_req_all_vtags != 0 && ntohs(rce_reply->hdr.msglen) > TLV_HDR_SIZE + ntohs(tlv->length))
            {
                narb_lsp_vtagmask_tlv* vtagmask = (narb_lsp_vtagmask_tlv*) ((char*)tlv + sizeof(struct te_tlv_header) + ntohs(tlv->length));
                LOGF("ALL E2E VLAN TAGS:");
                for (int vtag = 1; vtag < MAX_VLAN_NUM; vtag++)
                {
                    if (HAS_VLAN(vtagmask->bitmask, vtag))
                        cout<<" " << vtag;
                }
                cout<<endl;
            }
            break;
        case TLV_TYPE_NARB_ERROR_CODE:
            assert (rce_reply->hdr.action = ACT_ERROR);
            LOGF("Request failed : %s\n", error_code_to_cstr(ntohl(*(u_int32_t *)((char *)tlv + sizeof(struct te_tlv_header)))));
            break;
        }
        api_msg_delete(rce_reply);
    }

    close(sock);
    return 0;
}

