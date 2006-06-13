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

enum G_PID {
	G_Illegal = 0,
	G_Reserved1 = 1,
	G_Reserved2 = 2,
	G_Reserved3 = 3,
	G_Reserved4 = 4,
	G_Asyn_E4 = 5,
	G_Asyn_DS3 = 6,
	G_Asyn_E3 = 7,
	G_BitSyn_E3 = 8,
	G_ByteSyn_E3 = 9,
	G_Asyn_DS2 = 10,
	G_BitSyn_DS2 = 11,
	G_Reserved5 = 12,
	G_Asyn_E1 = 13,
	G_ByteSyn_E1 = 14,
	G_ByteSyn_31DS0 = 15,
	G_Asyn_DS1 = 16,
	G_BitSyn_DS1 = 17,
	G_ByteSyn_T1 = 18,
	G_VC11 = 19,
	G_Reserved6 = 20,
	G_Reserved7 = 21,
	G_DS1SFAsyn = 22,
	G_DS1ESFAsyn = 23,
	G_DS3M23Asyn = 24,
	G_DS3CBitAsyn = 25,
	G_VT_LOVC = 26,
	G_STS_HOVC = 27,
	G_POSUnscr16 = 28,
	G_POSUnscr32 = 29,
	G_POSScram16 = 30,
	G_POSScram32 = 31,
	G_ATM = 32,
	G_Eth = 33,
	G_SONET_SDH = 34,
	G_Reserved = 35,
	G_DigiWrapper = 36,
	G_Lamda = 37
};

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

