#ifndef __TOOLBOX_HH__
#define __TOOLBOX_HH__

#include "types.hh"
#include "lsp_broker.hh"

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif 

void ntohf_mbps(u_int32_t &x);

void ntohf_mbps(float &x);

void htonf_mbps(u_int32_t &x);

void htonf_mbps(float &x);


/* Structure for string<-->value conversion */
struct string_value_conversion {
	u_char 	   number;
	struct {
		const char *string;
		u_int32_t	   value;
		u_char 	   len;
	} sv[255];
};

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

