#ifndef __TOOLBOX_H__
#define __TOOLBOX_H__

extern "C"
{

/* Read nbytes from fd and store into ptr. */
int readn (int fd, char *ptr, int nbytes);

/* Write nbytes from ptr to fd. */
int writen(int fd, char *ptr, int nbytes);

u_int32_t get_narb_seqnum();

}
#endif

