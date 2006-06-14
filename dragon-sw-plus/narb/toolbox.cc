#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include "toolbox.hh"
#include "event.hh"
#include "log.hh"

void ntohf_mbps(u_int32_t &x)
{
    x = ntohl(x);
    float *p = (float*)&x;
    (*p) = (*p)*8.0/1000000.0;
}

void ntohf_mbps(float &x)
{
    u_int32_t *p = (u_int32_t*)&x;
    ntohf_mbps(*p);
}

void htonf_mbps(float &x)
{
    x = x*1000000.0/8.0;
    u_int32_t * p = (u_int32_t*)&x;
    *p = htonl(*p);
}

void htonf_mbps(u_int32_t &x)
{
    float *p = (float*)&x;
    htonf_mbps(*p);
}

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


u_int32_t get_narb_seqnum()
{
    static u_int32_t seqnum = 1;
    return seqnum++;
}

bool is_all_loose_hops(list<ero_subobj*>&ero)
{
    list<ero_subobj*>::iterator it;

    for (it = ero.begin(); it != ero.end(); it++)
        if ((*it) && (*it)->hop_type == ERO_TYPE_STRICT_HOP)
            return false;

    return true;
}

bool is_all_strict_hops(list<ero_subobj*>&ero)
{
    list<ero_subobj*>::iterator it;

    for (it = ero.begin(); it != ero.end(); it++)
        if ((*it) && (*it)->hop_type == ERO_TYPE_LOOSE_HOP)
            return false;

    return true;
}

ero_subobj* last_strict_hop(list<ero_subobj*>&ero)
{
    list<ero_subobj*>::iterator it;

    ero_subobj *ret = NULL;
    for (it = ero.begin(); it != ero.end(); it++)
    {
        if ((*it) && (*it)->hop_type == ERO_TYPE_STRICT_HOP)
            ret = *it;
        else
            break;
    }

    return ret;
}

ero_subobj* first_loose_hop(list<ero_subobj*>&ero)
{
    list<ero_subobj*>::iterator it;
 
    for (it = ero.begin(); it != ero.end(); it++)
    {
        if ((*it) && (*it)->hop_type == ERO_TYPE_LOOSE_HOP)
            return *it;
     }

    return NULL;
}

// each opaque LSA from the same advertising router needs a unique opaque_id
u_int32_t narb_ospf_opaque_id (void)
{
    static u_int32_t opaque_id = 0;
    opaque_id++;
    return opaque_id;
}

// SIGHUP handler
void sighup (int sig)
{
    LOG("SIGHUP received"<<endl);
}

extern void sigint (int sig);

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
    if (x-y < small_enough && y-x <small_enough)
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

struct string_value_conversion str_val_conv_switching = 
{
	8,
	{{ "psc1", 	LINK_IFSWCAP_SUBTLV_SWCAP_PSC1, 		4},
	{ "psc2", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC2, 		4}, 
	{ "psc3", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC3, 		4}, 
	{ "psc4", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC4, 		4},
	{ "l2sc", 		LINK_IFSWCAP_SUBTLV_SWCAP_L2SC, 		2},
	{ "tdm", 		LINK_IFSWCAP_SUBTLV_SWCAP_TDM, 		1}, 
	{ "lsc", 		LINK_IFSWCAP_SUBTLV_SWCAP_LSC, 		2}, 
	{ "fsc", 		LINK_IFSWCAP_SUBTLV_SWCAP_FSC, 		1}}
};

struct string_value_conversion str_val_conv_encoding = 
{
	8,
	{{ "packet", 	LINK_IFSWCAP_SUBTLV_ENC_PKT, 			2}, 
	{ "ethernet", 	LINK_IFSWCAP_SUBTLV_ENC_ETH, 			1}, 
	{ "pdh", 		LINK_IFSWCAP_SUBTLV_ENC_PDH, 			2}, 
	{ "sdh", 		LINK_IFSWCAP_SUBTLV_ENC_SONETSDH, 		1},
	{ "dwrapper", LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP, 		1}, 
	{ "lambda", 	LINK_IFSWCAP_SUBTLV_ENC_LAMBDA, 		1}, 
	{ "fiber", 		LINK_IFSWCAP_SUBTLV_ENC_FIBER, 			2}, 
	{ "fchannel", 	LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL, 		2}}
};


struct string_value_conversion str_val_conv_gpid = 
{
	29,
	{{ "e4async", 		G_Asyn_E4, 			3}, 
	{ "ds3async", 		G_Asyn_DS3, 			4}, 
	{ "e3async", 		G_Asyn_E3, 			3}, 
	{ "e3bitsync", 		G_BitSyn_E3, 			4},
	{ "e3bytesync",   	G_ByteSyn_E3, 		4}, 
	{ "ds2async", 		G_Asyn_DS2, 			4}, 
	{ "ds2bitsync", 	G_BitSyn_DS2, 		4}, 
	{ "e1asyn", 		G_Asyn_E1, 			3},
	{ "e1bytesyn", 	G_ByteSyn_E1, 		3},
	{ "ds031bytesyn",	G_ByteSyn_31DS0, 	3},
	{ "ds1async",		G_Asyn_DS1,			4},
	{ "ds1bitsyn",		G_BitSyn_DS1,		4},
	{ "t1bytesyn",		G_ByteSyn_T1,		1},
	{ "vc11",			G_VC11,				2},
	{ "ds1sfasyn",		G_DS1SFAsyn,		4},
	{ "ds1esfasyn",	G_DS1ESFAsyn,		4},
	{ "ds3m23asyn",	G_DS3M23Asyn,		4},
	{ "ds3cbitasyn",	G_DS3CBitAsyn,		4},
	{ "vtlovc",		G_VT_LOVC,			2},
	{ "stshovc",		G_STS_HOVC,		2},
	{ "posunscr16",	G_POSUnscr16,		9},
	{ "posunscr32",	G_POSUnscr32,		9},
	{ "posscr16",		G_POSScram16,		7},
	{ "posscr32",		G_POSScram32, 		7},
	{ "atm",			G_ATM, 				1},
	{ "ethernet",		G_Eth, 				2},
	{ "sdh",			G_SONET_SDH, 		2},
	{ "wrapper",		G_DigiWrapper, 		1},
	{ "lambda",		G_Lambda, 			1}}
};

const char* err_cstrs[] = {
	"Unrecognized Error Code",
	"Unknown Source Address",
	"Unknown Destination Address",
	"No Routing Path Found",
	"NARB Internal Error",
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

