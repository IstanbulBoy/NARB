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
#include "config.h"
#include "types.hh"
#include "log.hh"
#include "lsp_broker.hh"
#include "narb_apiclient.hh"
#include "narb_config.hh"
#include "lsa.hh"
#include "toolbox.hh"
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef FREEBSD
  #include <sys/sendfile.h>
#endif
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

 
#ifndef HAVE_DECL_GETOPT
  #define HAVE_DECL_GETOPT 1
#endif
#include <getopt.h>

char host[20];
int port = 2609;
int sock = -1;
in_addr source;
in_addr destination;
u_int8_t swtype = 51;
u_int8_t encoding = 2;
float bandwidth = 500; //Mbps
u_int32_t vtag = 0x0000ffff;
u_int32_t lclid_src = 0, lclid_dest = 0;
u_int32_t opt_bidirectional = LSP_OPT_BIDIRECTIONAL;
u_int32_t opt_strict = LSP_OPT_STRICT;
u_int32_t opt_preferred = LSP_OPT_PREFERRED;
u_int32_t opt_mrn = 0;
u_int32_t opt_e2e_vlan = 0;
u_int32_t opt_via_movaz = 0;
u_int32_t opt_excluded_layers = 0;
u_int32_t opt_req_all_vtags = 0;
u_int32_t opt_vtag_mask = 0;
u_int32_t opt_query_hold = 0;
u_int32_t opt_query_with_confirmation = 0;
u_int32_t opt_alt_paths = 0;
u_int32_t opt_query_subnet_ero = 0;
in_addr hop_back;

struct msg_narb_vtag_mask vtag_mask;

struct option longopts[] = 
{
    { "host",        no_argument,       NULL, 'H'},
    { "port",     no_argument,       NULL, 'P'},
    { 0 }
};

void usage()
{
    cout<<"NARB Tester Usage:"<<endl;
    cout<<"\t narb_test [-H host] [-P port] [-S source] [-D dest] [-B] [-b bandwidth] [-x switching type] [-e encoding type] ";
    cout<<"  ( [-S source] and [-D dest] are mandatory [-U]: unidirectional ) [-L]: loose-hop [-O]: only (as gainst preferred)" <<endl;
    cout<<"  [ [-M] multi-region network [-v tag]: E2E VLAN with specified tag [-V]: E2E VLAN with tag picked by RCE ]" <<endl;
    cout<<"  [ [-E mask] Excluding routing layers (umask) [-m] using Movaz/ADVA private info [-a]: Return all available VLAN tags ]" <<endl;
    cout<<"  [ [-l local-id/s]  [-Q] Query and Hold [-C] Query expecting Confirmation ID] " <<endl;
}

// SIGINT handler.
void sigint (int sig)
{
    LOG("Terminating on signal SIGINT"<<endl);
    exit(0);
}

// SIGINT handler.
void sigsegv (int sig)
{
    LOG("Terminating on signal SIGSEGV"<<endl);
    exit(-1);
}


int narbapi_connect ()
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
        cerr <<  "narbapi_connect: no such host " <<  host << endl;
        return (-1);
    }

    fd = socket (AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
	  cerr <<"narbapi_connect: socket():" << strerror (errno) << endl;
	  return (-1);
    }

                                                                                
    ret = setsockopt (fd, SOL_SOCKET,
                      SO_REUSEADDR, (void *) &on, sizeof (on));
    if (ret < 0)
    {
        cerr << "narbapi_connect: SO_REUSEADDR failed" <<endl;
        close (fd);
        return (-1);
    }
  
  #ifdef SO_REUSEPORT
    ret = setsockopt (fd, SOL_SOCKET, SO_REUSEPORT,
                      (void *) &on, sizeof (on));
    if (ret < 0)
    {
        cerr << "narbapi_connect: SO_REUSEPORT failed" <<endl;
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
        cerr << "narbapi_connect: connect(): " << strerror (errno);
        close (fd);
        return (-1);
    }
  
    return fd;
}

int set_vtag_mask_from_cmdline(struct msg_narb_vtag_mask* vtag_mask, char* vtag_range)
{
	int vlan1, vlan2;
	vtag_mask->type = htons(TLV_TYPE_NARB_VTAG_MASK);
	// TLV length = content length (for OSPF only)
	vtag_mask->length = htons(sizeof(struct msg_narb_vtag_mask) - 4);
	if (sscanf(vtag_range, "%d-%d", &vlan1, &vlan2) != 2)
	{
		printf("Wrong -t option argument '%s': must be in the format of [vtag_low-vtag_high]\n", vtag_range);
		return -1;
	}
	if (vlan1 < 1 || vlan1 > 4095)
	{
		printf("Wrong low VLAN tag %d: must be <1-4095>\n", vlan1);
		return -1;
	}
	if (vlan2 < 1 || vlan2 > 4095 || vlan1 > vlan2)
	{
		printf("Wrong high VLAN tag %d: must be <%d-4095>\n", vlan2, vlan1);
		return -1;
	}

	for (int vlan = vlan1; vlan <= vlan2; vlan++)
		vtag_mask->bitmask[(vlan-1)/8] = vtag_mask->bitmask[(vlan-1)/8] |(0x80 >> ((vlan-1)%8));
}

struct api_msg * narbapi_read (int fd)
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
        LOGF("narb APIReader failed to read from %d\n", fd);
        return NULL;
    }
    else if (rlen == 0)
    {
        LOGF("Connection closed for APIReader(%d)\n", fd);
        return NULL;    
    }
    else if (rlen != sizeof (struct api_msg_header))
    {
        LOGF("narb APIReader(%d) cannot read the message header\n", fd);
        return NULL;
    }
  
    if (MSG_CHKSUM(hdr) != hdr.chksum)
    {
        LOGF("narb APIReader(%d) packet corrupt \n", fd);
        return NULL;
    }
  
    /* Determine body length. */
    bodylen = ntohs (hdr.length);
    if (bodylen > API_MAX_MSG_SIZE)
    {
        LOGF("narb APIReader(%d) cannot read oversized packet\n", fd);
        return NULL;
    }
      
    if (bodylen > 0)
    {
        /* Read message body*/
        rlen = readn (fd, buf, bodylen);
        if (rlen < 0)
        {
  	     LOGF("narb APIReader failed to read from %d\n", fd);
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
    msg = api_msg_new (ntohs(hdr.type), ntohs(hdr.length), buf, ntohl (hdr.ucid), ntohl (hdr.seqnum), ntohl(hdr.tag));
    return msg;
}

int narbapi_send (int fd, struct api_msg *msg)
{
  static char buf[API_MAX_MSG_SIZE];
  int len, wlen;
  int ret = 0;

  assert (fd > 0);  
  assert (msg);
  assert (msg->body);

  /* Length of message including header*/
   len = sizeof (struct api_msg_header) + ntohs (msg->header.length);

  /* Make contiguous memory buffer for message */
  memcpy (buf, &msg->header, sizeof (struct api_msg_header));
  memcpy (buf + sizeof (struct api_msg_header), msg->body, ntohs (msg->header.length));

  if (MSG_CHKSUM(msg->header) != msg->header.chksum)
  {
      LOGF("narb APIWriter(%d) packet corrupt\n", fd);
      return -1;
  }

  wlen = writen(fd, buf, len);
  if (wlen < 0)
    {
      LOGF("narb APIWriter failed to write %d\n",fd);
      return -1;
    }
  else if (wlen == 0)
    {
      LOGF("Connection closed for APIWriter(%d)\n", fd);
      return -1;
    }
  else if (wlen != len)
    {
      LOGF("narb APIWriter(%d) cannot write the message\n", fd);
      return -1;
    }

  return ret;
}

msg_app2narb_request * new_app_request()
{
  msg_app2narb_request* app_req = new (struct msg_app2narb_request);
  app_req->type = htons(MSG_APP_REQUEST);
  app_req->length = htons(sizeof(struct msg_app2narb_request) - TLV_HDR_SIZE);
  app_req->src.s_addr = source.s_addr;
  app_req->dest.s_addr = destination.s_addr;
  app_req->bandwidth = bandwidth;
  app_req->encoding_type = encoding;
  app_req->switching_type = swtype;
  app_req->gpid = htons(8);

  return app_req;
}


api_msg* narbapi_query_lsp (u_int32_t options, u_int32_t ucid, u_int32_t seqnum, msg_app2narb_request *app_req)
{
  struct api_msg *narb_msg;
  int bodylen = 0;
  char msgbody[1024];
  memcpy(msgbody, app_req, sizeof(msg_app2narb_request));
  bodylen += sizeof(msg_app2narb_request);

  if (vtag_mask.type != 0)
  {
    memcpy(msgbody + bodylen, &vtag_mask, sizeof(msg_narb_vtag_mask));
    bodylen += sizeof(msg_narb_vtag_mask);
  }

  if (hop_back.s_addr != 0)
  {
    msg_narb_hop_back* hopback_tlv = (msg_narb_hop_back*)(msgbody + bodylen);
    hopback_tlv->type = htons(TLV_TYPE_NARB_HOP_BACK);
    hopback_tlv->length = htons(sizeof(msg_narb_hop_back) - TLV_HDR_SIZE);
    hopback_tlv->ipv4 = hop_back.s_addr;
    bodylen += sizeof (msg_narb_hop_back);
  }

  if (lclid_src != 0 && lclid_dest != 0)
  {
    msg_narb_local_id* lcl_id_tlv = (msg_narb_local_id*)(msgbody+bodylen);
    lcl_id_tlv->type = htons(TLV_TYPE_NARB_LOCAL_ID);
    lcl_id_tlv->length = htons(sizeof(msg_narb_local_id) - TLV_HDR_SIZE);
    lcl_id_tlv->lclid_src = htonl(lclid_src);
    lcl_id_tlv->lclid_dest = htonl(lclid_dest);
    bodylen += sizeof (msg_narb_local_id);
  }

  narb_msg = api_msg_new(NARB_MSG_LSPQ, bodylen, (void*)msgbody, ucid, seqnum, vtag);
  narb_msg->header.msgtag[0] = htonl(options | opt_bidirectional | opt_strict | opt_preferred |opt_mrn |
        opt_e2e_vlan | opt_via_movaz | opt_excluded_layers | opt_req_all_vtags | opt_vtag_mask |
        opt_query_hold | opt_query_with_confirmation | opt_query_subnet_ero | opt_alt_paths);

  if (narbapi_send(sock, narb_msg) < 0)
  {
    LOGF("Send MSG to narb failed\n");
    return NULL;
  }

  narb_msg = narbapi_read(sock);

  if (!narb_msg)
    LOGF("Read MSG from narb failed\n");

  return narb_msg;
}

NARB_APIServer* narb_server = NULL;
ZebraOspfSync * zebra_client = NULL;
TerceApiTopoSync * terce_client = NULL;
DomainTopologyOriginator * dts_originator = NULL;
ConfigFile configMaster;

char xml_buf[4000];

int main(int argc, char* argv[])
{
    char* xml_file = NULL;
    Log::Init(LOG_ALL, "./test_narb.log");
    Log::SetDebug(true);

    strcpy(host, "localhost");
    source.s_addr = destination.s_addr = hop_back.s_addr = 0;
    memset(&vtag_mask, 0, sizeof(struct msg_narb_vtag_mask));
    while (1)
    {
        int opt;

        opt = getopt_long (argc, argv, "E:H:P:S:D:X:b:e:l:k:t:v:x:CLMOQUVZmas", longopts, 0);
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
        case 'e':
            sscanf(optarg, "%d", &encoding);
            break;
        case 'l':
            {
                u_int32_t lclid_src_type = 0, lclid_src_val = 0;
                u_int32_t lclid_dest_type = 0, lclid_dest_val = 0;
                if (sscanf(optarg, "%d:%d", &lclid_src, &lclid_dest) != 2)
                {
                    if (sscanf(optarg, "%d/%d:%d/%d", &lclid_src_type, &lclid_src_val, &lclid_dest_type, &lclid_dest_val) == 4)
                    {
                        lclid_src = ((lclid_src_type << 16) | (lclid_src_val & 0xffff));
                        lclid_dest = ((lclid_dest_type << 16) | (lclid_dest_val & 0xffff));
                    }
                }
                if (lclid_src == 0 || lclid_dest == 0)
                {
                    printf("Wrong local ID format, use either A) lclid_src:lclid_dest, or B) src_type/src_val:dest_type/dest_val\n");
                    exit(-1);
                }
            }
            break;
        case 'v':
            opt_e2e_vlan = LSP_OPT_E2E_VTAG;
            sscanf(optarg, "%d", &vtag);
            break;
        case 'x':
            sscanf(optarg, "%d", &swtype);
            break;
        case 'S':
            inet_aton(optarg, &source);
            break;
        case 'D':
            inet_aton(optarg, &destination);
            break;
        case 'U':
            opt_bidirectional = 0;
            break;
        case 'L':
            opt_strict = 0;
            break;
        case 'O':
            opt_preferred = 0;
            break;
        case 'm':
            opt_via_movaz= LSP_OPT_VIA_MOVAZ;
            break;
        case 'M':
            opt_mrn = LSP_OPT_MRN;
            break;
        case 'Q':
            opt_query_hold = LSP_OPT_QUERY_HOLD;
            break;
        case 'C':
            opt_query_with_confirmation = LSP_OPT_QUERY_CONFIRM;
            break;
        case 'V':
            opt_e2e_vlan= LSP_OPT_E2E_VTAG;
            break;
        case 'X':
            xml_file = optarg;
            break;
        case 'Z':
            opt_alt_paths = LSP_OPT_ALT_PATHS;
            break;
        case 'E':
            sscanf(optarg, "%d", &opt_excluded_layers);
            //1: excluding layer-1; 2: layer-tdm; 4: layer-2; 8: layer-3 ; 3: both layer 1 (optical) and tdm
            opt_excluded_layers <<= 4;
            break;
        case 't':
            if (set_vtag_mask_from_cmdline(&vtag_mask, optarg) < 0)
            {
                exit(1);
            }
            opt_e2e_vlan = LSP_OPT_E2E_VTAG;
            opt_req_all_vtags = LSP_OPT_REQ_ALL_VTAGS;
            opt_vtag_mask = LSP_OPT_VTAG_MASK;
            break;
        case 'a':
            opt_req_all_vtags = LSP_OPT_REQ_ALL_VTAGS;
            break;
        case 'k':
            inet_aton(optarg, &hop_back);
            break;
        case 's':
            opt_query_subnet_ero = LSP_OPT_SUBNET_ERO;
            break;
        default:
            usage();
            exit(1);
            break;
        }
    }

    if ((source.s_addr == 0 || destination.s_addr == 0) && !xml_file)
    {
        usage();
        exit(1);
    }

    if ((sock = narbapi_connect()) < 0)
        exit(2);

    if (xml_file)
    {
	int fd = open(xml_file, O_RDONLY);
#ifndef FREEBSD
	struct stat file_stat;
	if (fstat(fd, &file_stat) == -1) {
            printf("fstat failed\n");
            exit(1);
	}
        int total = sendfile(sock, fd, 0, file_stat.st_size);
#else
        int total = sendfile(fd, sock, 0, 0, NULL, NULL, 0);
#endif
        if (total <= 0) {
            printf("sendfile() returns %d\n", total);
            exit(1);
        }
        total = recv(sock, xml_buf, 4000, 0);
        if (total <= 0) {
            printf("recv() returns %d\n", total);
            exit(1);
        }
        write(1, xml_buf, total);
        sleep(5);
        return 0;
    }

    msg_app2narb_request * app_req = new_app_request();
    srandom(getpid() + getppid() + time(NULL));
    u_int32_t ucid = random();
    u_int32_t seqnum = random();

    api_msg * narb_reply =  narbapi_query_lsp(0, ucid, seqnum, app_req);

    int len, offset;
    ipv4_prefix_subobj* subobj_ipv4;
    unum_if_subobj* subobj_unum;
    char addr[20];
    if (narb_reply)
    {
        te_tlv_header * tlv = (te_tlv_header*)narb_reply->body;
        switch (ntohs(narb_reply->header.type))
        {
        case MSG_REPLY_ERO:
            LOGF("Request successful! ERO returned...\n");
            len = ntohs(tlv->length);
            assert( len > 0 && len % 4 == 0);
            offset = sizeof(struct te_tlv_header);
            subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
            while (len > 0)
            {
                if ((subobj_ipv4->l_and_type & 0x7f) == 4)
                    subobj_unum = (unum_if_subobj *)((char *)tlv + offset);
                else
                    subobj_unum = NULL;

                if (subobj_unum)
                {
                    inet_ntop(AF_INET, &subobj_unum->addr, addr, 20);
                    LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d)]\n", (subobj_unum->l_and_type & (1<<7)) == 0?"strict":"loose", addr, ntohl(subobj_unum->ifid), ntohl(subobj_unum->ifid)>>16, (u_int16_t)ntohl(subobj_unum->ifid));
                    len -= sizeof(unum_if_subobj);
                    offset += sizeof(unum_if_subobj);
                }
                else
                {
                    inet_ntop(AF_INET, (in_addr*)subobj_ipv4->addr, addr, 20);
                    LOGF("HOP-TYPE [%s]: %s\n", (subobj_ipv4->l_and_type & (1<<7)) == 0?"strict":"loose", addr);
                    len -= sizeof(ipv4_prefix_subobj);
                    offset += sizeof(ipv4_prefix_subobj);
                }
                subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
            }
            if (opt_e2e_vlan)
                LOGF("E2E VLAN TAG [ %d ]\n", ntohl(narb_reply->header.tag));
            if (vtag == ANY_VTAG && opt_req_all_vtags != 0 && ntohs(narb_reply->header.length) > TLV_HDR_SIZE + ntohs(tlv->length))
            {
                msg_narb_vtag_mask* vtagmask = (msg_narb_vtag_mask*) ((char*)tlv + TLV_HDR_SIZE + ntohs(tlv->length));
                LOGF("ALL E2E VLAN TAGS:");
                for (int vtag = 1; vtag < MAX_VLAN_NUM; vtag++)
                {
                    if (HAS_VLAN(vtagmask->bitmask, vtag))
                        cout<<" " << vtag;
                }
                cout<<endl;
                offset += sizeof(msg_narb_vtag_mask);
            }
            tlv = (te_tlv_header*)((char*)tlv + TLV_HDR_SIZE + ntohs(tlv->length));
            if (htons(narb_reply->header.length) - offset > sizeof(te_tlv_header) && ntohs(tlv->type) == TLV_TYPE_NARB_SUBNET_ERO)
            {
                LOGF("Subnet ERO hops\n");
                len = ntohs(tlv->length);
                offset = sizeof(struct te_tlv_header);
                subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
                while (len > 0)
                {
                    if ((subobj_ipv4->l_and_type & 0x7f) == 4)
                        subobj_unum = (unum_if_subobj *)((char *)tlv + offset);
                    else
                        subobj_unum = NULL;

                    if (subobj_unum)
                    {   
                        inet_ntop(AF_INET, &subobj_unum->addr, addr, 20);
                        LOGF("HOP-TYPE [%s]: %s [UnumIfId: %d(%d,%d)]\n", (subobj_unum->l_and_type & (1<<7)) == 0?"strict":"loose", addr, ntohl(subobj_unum->ifid), ntohl(subobj_unum->ifid)>>16, (u_int16_t)ntohl(subobj_unum->ifid));
                        len -= sizeof(unum_if_subobj);
                        offset += sizeof(unum_if_subobj);
                    }   
                    else
                    {   
                        inet_ntop(AF_INET, (in_addr*)subobj_ipv4->addr, addr, 20);
                        LOGF("HOP-TYPE [%s]: %s\n", (subobj_ipv4->l_and_type & (1<<7)) == 0?"strict":"loose", addr);
                        len -= sizeof(ipv4_prefix_subobj);
                        offset += sizeof(ipv4_prefix_subobj);
                    }   
                    subobj_ipv4  = (ipv4_prefix_subobj *)((char *)tlv + offset);
                }           
            }

            break;
        case MSG_REPLY_ERROR:
            LOGF("Request failed : %s\n", error_code_to_cstr(ntohl(*(u_int32_t *)((char *)tlv + sizeof(struct te_tlv_header)))));
            break;
        }

        api_msg_delete(narb_reply);
    }
    close(sock);
    return 0;
}

