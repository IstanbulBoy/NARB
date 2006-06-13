#ifndef __TOOLBOX_HH__
#define __TOOLBOX_HH__

#include "types.hh"
#include "lsp_broker.hh"

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif 

inline void ntohf_mbps(u_int32_t &x)
{
    x = ntohl(x);
    float *p = (float*)&x;
    (*p) = (*p)*8/1000000;
}

inline void ntohf_mbps(float &x)
{
    u_int32_t *p = (u_int32_t*)&x;
    *p = ntohl(*p);
    x = x*8/1000000;
}

inline void htonf_mbps(float &x)
{
    x = x*1000000/8;
    u_int32_t * p = (u_int32_t*)&x;
    *p = htonl(*p);
}

inline void htonf_mbps(u_int32_t &x)
{
    float *p = (float*)&x;
    *p = (*p)*1000000/8;
    x = htonl(x);
}


/* Structure for string<-->value conversion */
struct string_value_conversion {
	u_char 	   number;
	struct {
		const char *string;
		u_int32_t	   value;
		u_char 	   len;
	} sv[255];
};

extern struct string_value_conversion str_val_conv_switching;

extern struct string_value_conversion str_val_conv_encoding;

extern struct string_value_conversion str_val_conv_gpid;

extern "C"
{

/* Read nbytes from fd and store into ptr. */
int readn (int fd, char *ptr, int nbytes);

/* Write nbytes from ptr to fd. */
int writen(int fd, char *ptr, int nbytes);

u_int32_t get_narb_seqnum();

bool is_all_loose_hops(list<ero_subobj*>&ero);

bool is_all_strict_hops(list<ero_subobj*>&ero);

ero_subobj* last_strict_hop(list<ero_subobj*>&ero);

ero_subobj* first_loose_hop(list<ero_subobj*>&ero);

//void interdomain_link_into_strict_hops(list<ero_subobj*>&ero);

u_int32_t narb_ospf_opaque_id (void);

void sighup (int sig);

void sigint (int sig);

RETSIGTYPE * signal_set (int signo, void (*func)(int));

void signal_init ();

int daemon (int nochdir, int noclose);

bool float_equal(float x, float y);

bool float_evenly_dividable(float x, float y);

u_int32_t string_to_value(struct string_value_conversion *db, const char *str);

const char *value_to_string(struct string_value_conversion *db, u_int32_t value);

const char* error_code_to_cstr(u_int32_t errcode);

}

#endif

