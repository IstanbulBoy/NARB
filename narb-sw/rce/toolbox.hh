#ifndef __TOOLBOX_HH__
#define __TOOLBOX_HH__

#include "rce_types.hh"

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif 

struct api_msg;

extern "C"
{

/* Read nbytes from fd and store into ptr. */
int readn (int fd, char *ptr, int nbytes);

/* Write nbytes from ptr to fd. */
int writen(int fd, char *ptr, int nbytes);

struct api_msg * api_msg_new (u_char msgtype, u_char action, void *msgbody, u_int32_t ucid, u_int32_t seqnum, u_int16_t msglen, u_int32_t tag = 0);

void api_msg_delete (struct api_msg* msg);

void api_msg_dump (struct api_msg* msg);

u_int32_t get_rce_seqnum();

void sighup (int sig);

void sigint (int sig);

RETSIGTYPE * signal_set (int signo, void (*func)(int));

void signal_init ();

int daemon (int nochdir, int noclose);

bool float_equal(float x, float y);

bool float_evenly_dividable(float x, float y);

}

void ntohf_mbps(u_int32_t &x);

void ntohf_mbps(float &x);

void htonf_mbps(u_int32_t &x);

void htonf_mbps(float &x);


#define ZBUFSIZE 1024
void z_compress(u_char* buf, u_int32_t& len);

void z_uncompress(u_char* buf, u_int32_t& len);

#endif

