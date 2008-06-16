#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include "toolbox.hh"
#include "rce_log.hh"
#include "rce_api.hh"
#include "zebra_ospfclient.hh"

int readn (int fd, char *ptr, int nbytes)
{
  int nleft;
  int nread;

  nleft = nbytes;

  while (nleft > 0) 
    {
      nread = read (fd, ptr, nleft);

      if (nread < 0) 
	return (nread);
      else
	if (nread == 0) 
	  break;

      nleft -= nread;
      ptr += nread;
    }

  return nbytes - nleft;
}  


int writen(int fd, char *ptr, int nbytes)
{
	int nleft;
	int nwritten;

	nleft = nbytes;

	while (nleft > 0) 
	{
	  nwritten = write(fd, ptr, nleft);
	  
	  if (nwritten <= 0) 
         return (nwritten);

	  nleft -= nwritten;
	  ptr += nwritten;
	}
	return nbytes - nleft;
}


struct api_msg * api_msg_new (u_char msgtype, u_char action, void *msgbody, u_int32_t ucid, u_int32_t seqnum, u_int16_t msglen, u_int32_t tag)
{
    api_msg *msg;

    msg = new (api_msg);
    memset (msg, 0, sizeof(api_msg));

    msg->hdr.msgtype = msgtype;
    msg->hdr.action = action;
    msg->hdr.msglen = htons (msglen);
    msg->hdr.ucid = htonl (ucid);
    msg->hdr.msgseq = htonl (seqnum);
    msg->hdr.tag = htonl (tag);
    
    int len = msglen;
    msg->body = new char[len];
    memcpy(msg->body, msgbody, msglen);

    msg->hdr.chksum = MSG_CHKSUM(msg->hdr);

    return msg;
}

struct api_msg * api_msg_append_tlv (struct api_msg * msg, struct te_tlv_header* tlv)
{
    api_msg* rmsg = new (api_msg);
    memcpy(rmsg, msg, sizeof(api_msg));
    rmsg->hdr.msglen = htons(ntohs(msg->hdr.msglen) + sizeof(te_tlv_header) + ntohs(tlv->length));
    rmsg->hdr.chksum = MSG_CHKSUM(rmsg->hdr);
    rmsg->body = new char[ntohs(msg->hdr.msglen) + sizeof(te_tlv_header) + ntohs(tlv->length)];
    memcpy(rmsg->body, msg->body, ntohs(msg->hdr.msglen));
    memcpy(rmsg->body + ntohs(msg->hdr.msglen), tlv, sizeof(te_tlv_header) + ntohs(tlv->length));
    api_msg_delete(msg);
    return rmsg;
}

void api_msg_delete (struct api_msg* msg)
{
    assert(msg);
    assert(msg->body);    	

    delete []msg->body;
    delete msg;
}

void api_msg_dump (struct api_msg* msg)
{
    LOG_DEBUG("api_msg_dump::TYPE(" <<msg->hdr.msgtype<<") ACTION("<<msg->hdr.action
                    <<") LENGTH("<<ntohs(msg->hdr.msglen) <<") UCID#" << ntohl(msg->hdr.ucid) 
                    << ") SEQ#"<<ntohl(msg->hdr.msgseq)<<endl);
}

u_int32_t get_rce_seqnum()
{
    static u_int32_t seqnum = 1;
    return seqnum++;
}

int count_fragments(string &s)
{
    static char buf[256];
    assert(s.size() < 255);
    strcpy (buf, s.c_str());

    if (strtok(buf, " \t\r\n") == NULL)
        return 0;
    int ret = 1;
    while (strtok(0, " \t\r\n") != NULL)
        ret ++;
    return ret;
}

string get_fragments(string &s, int start, int n)
{
    static char buf[256];
    if(count_fragments(s) <= n)
        return s;

    if (s.size() <= start)
        return string("");

    strcpy (buf, s.c_str() + start);
    char *pstr = strtok(buf, " \t\r\n");
    assert(pstr);
    string rs(pstr);
    n--;
    while ((pstr = strtok(NULL, " \t\r\n")) != NULL && n-- > 0)
    {
        rs += ' ';
        rs += pstr;
    }
    return rs;
}

// SIGHUP handler
void sighup (int sig)
{
    LOG("SIGHUP received"<<endl);
}

extern ZebraOspfSync *zebra_client_inter;
extern ZebraOspfSync *zebra_client_intra;

// SIGINT handler.
void sigint (int sig)
{
    LOG("Terminating on signal"<<endl);

    // server cleanup
    if (zebra_client_inter)
        zebra_client_inter->Exit();

    if (zebra_client_intra)
        zebra_client_intra->Exit();

    // cli cleanup
  
    exit (0);
}

// Signal wrapper.
RETSIGTYPE * signal_set (int signo, void (*func)(int))
{
  int ret;
  struct sigaction sig;
  struct sigaction osig;

  sig.sa_handler = func;
  sigemptyset (&sig.sa_mask);
  sig.sa_flags = 0;
#ifdef SA_RESTART
  sig.sa_flags |= SA_RESTART;
#endif

  ret = sigaction (signo, &sig, &osig);

  if (ret < 0) 
    return ((RETSIGTYPE*)SIG_ERR);
  else
    return ((RETSIGTYPE*)osig.sa_handler);
}

// Initialization of signal handles.
void signal_init ()
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
#ifdef SIGTSTP
  signal_set (SIGTSTP, SIG_IGN);
#endif
#ifdef SIGTTIN
  signal_set (SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTTOU
  signal_set (SIGTTOU, SIG_IGN);
#endif
}

// Daemonize myself
int daemon (int nochdir, int noclose)
{
  pid_t pid;

  pid = fork ();

  /* In case of fork is error. */
  if (pid < 0)
    {
      perror ("fork");
      return -1;
    }

  /* In case of this is parent process. */
  if (pid != 0)
    exit (0);

  /* Become session leader and get pid. */
  pid = setsid();

  if (pid < -1)
    {
      perror ("setsid");
      return -1;
    }

  /* Change directory to root. */
  if (! nochdir)
    chdir ("/");

  /* File descriptor close. */
  if (! noclose)
    {
      int fd;

      fd = open ("/dev/null", O_RDWR, 0);
      if (fd != -1)
	{
	  dup2 (fd, STDIN_FILENO);
	  dup2 (fd, STDOUT_FILENO);
	  dup2 (fd, STDERR_FILENO);
	  if (fd > 2)
	    close (fd);
	}
    }

  umask (0027);

  return 0;
}

#define small_enough 0.001
bool float_equal(float x, float y)
{
    if (x-y < small_enough && y-x < small_enough)
        return true;

    return false;

}

bool float_evenly_dividable(float x, float y)
{
    if (y < 0 || x < y)
        return false;

    if (fmodf(x, y) < small_enough)
        return true;

    return false;
}

void ntohf_mbps(u_int32_t &x)
{
    x = ntohl(x);
    float *p = (float*)&x;
    (*p) = (*p)*8/1000000;
}

void ntohf_mbps(float &x)
{
    u_int32_t *p = (u_int32_t*)&x;
    ntohf_mbps(*p);
}

void htonf_mbps(float &x)
{
    x = x*1000000/8;
    u_int32_t * p = (u_int32_t*)&x;
    *p = htonl(*p);
}

void htonf_mbps(u_int32_t &x)
{
    float *p = (float*)&x;
    htonf_mbps(*p);
}

bool is_slash30_ipv4 (u_int32_t ip)
{
    switch ((ip >> 24) & 0x03) 
    {
    case 0x01:
        return true;
        break;
    case 0x02:
        return true;
        break;
    }
    return false;
}

u_int32_t get_slash30_peer (u_int32_t ip)
{
    switch ((ip >> 24) & 0x03) 
    {
    case 0x01:
        return ((ip & (~0x01000000)) | 0x02000000);
        break;
    case 0x02:
        return ((ip & (~0x02000000)) | 0x01000000);
        break;
    }

    return ip;
}

u_int32_t string_to_value(struct string_value_conversion *db, const char *str)
{
	int i;
	
	for (i=0; i<db->number; i++)
	{
		if (strncmp(db->sv[i].string, str, db->sv[i].len)==0)
			return db->sv[i].value;
	}
	return 0;
}

const char * value_to_string(struct string_value_conversion *db, u_int32_t value)
{
	int i;
	static const char* def_string = "Unknown";
	
	for (i=0; i<db->number; i++)
	{
		if (db->sv[i].value == value)
			return db->sv[i].string;
	}
	return def_string;
}

