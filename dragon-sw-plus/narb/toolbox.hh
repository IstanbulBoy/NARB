#ifndef __TOOLBOX_HH__
#define __TOOLBOX_HH__

#include "types.hh"
#include "lsp_broker.hh"

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif 

void ntohf_mbps(u_int32_t &x);
void ntohf_mbps(float &x);
void htonf_mbps(float &x);
void htonf_mbps(u_int32_t &x);


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


#define G_Illegal 0
#define G_Reserved1 1
#define G_Reserved2 2
#define G_Reserved3 3
#define G_Reserved4 4
#define G_Asyn_E4 5
#define G_Asyn_DS3 6
#define G_Asyn_E3 7
#define G_BitSyn_E3 8
#define G_ByteSyn_E3 9
#define G_Asyn_DS2 10
#define G_BitSyn_DS2 11
#define G_Reserved5 12
#define G_Asyn_E1 13
#define G_ByteSyn_E1 14
#define G_ByteSyn_31DS0 15
#define G_Asyn_DS1 16
#define G_BitSyn_DS1 17
#define G_ByteSyn_T1 18
#define G_VC11 19
#define G_Reserved6 20
#define G_Reserved7 21
#define G_DS1SFAsyn 22
#define G_DS1ESFAsyn 23
#define G_DS3M23Asyn 24
#define G_DS3CBitAsyn 25
#define G_VT_LOVC 26
#define G_STS_HOVC 27
#define G_POSUnscr16 28
#define G_POSUnscr32 29
#define G_POSScram16 30
#define G_POSScram32 31
#define G_ATM 32
#define G_Eth 33
#define G_SONET_SDH 34
#define G_Reserved 35
#define G_DigiWrapper 36
#define G_Lambda 37

#endif

