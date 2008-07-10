package Aux;

use threads;
use threads::shared;
use strict;
use sigtrap;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Log;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.14 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw( CTRL_CMD ASYNC_CMD RUN_Q_T TERM_T_T INIT_Q_T ADDR_TERCE ADDR_GMPLS_CORE ADDR_GMPLS_NARB_S ADDR_GMPLS_NARB_C ADDR_GMPLS_RCE_S ADDR_GMPLS_RCE_C ADDR_WEB_S ADDR_SOAP_S);
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub dump_config($;$);

use constant RUN_DBG => 0;
use constant CFG_DBG => 1;
use constant NARB_DBG => 2;
use constant RCE_DBG => 3;
use constant NET_DBG => 4;
use constant API_DBG => 5;
use constant DATA_DBG => 6;
use constant LSA_DBG => 7;
use constant TEDB_DBG => 8;
use constant WS_DBG => 9;
use constant RAW_DBG => 10;

# commands/types for controlling the interthread client queues
# NOTE: the "type" key in this construct translates directly to "action" of 
# the API message format
use constant ASYNC_CMD => 0x0001;

use constant CTRL_CMD => 0xffff;
use constant RUN_Q_T => 1; # this dislodges a blocking queue  (so a condition can be checked)
use constant TERM_T_T => 2; # this will force the termination of a thread run loop
use constant INIT_Q_T => 3; # this will initialize a client queue and open the async socket

use constant ADDR_TERCE => 1;
use constant ADDR_GMPLS_CORE => 2;
use constant ADDR_GMPLS_NARB_S => 3;
use constant ADDR_GMPLS_NARB_C => 4;
use constant ADDR_GMPLS_RCE_S => 5;
use constant ADDR_GMPLS_RCE_C => 6;
use constant ADDR_WEB_S => 7;
use constant ADDR_SOAP_S => 8;

my $dbg_sys = 0;

sub set_dbg_sys($) {
	my ($v) = @_;
	$dbg_sys = $v;
}

sub add_dbg_sys($) {
	my ($v) = @_;
	$dbg_sys |= (1 << $v);
}

sub get_dbg_sys(;$) {
	my ($v) = @_;
	return $dbg_sys if(!defined($v));
	return(	$v eq "run")?	(1 << RUN_DBG):
		$v eq "config"?	(1 << CFG_DBG):
		$v eq "narb"?	(1 << NARB_DBG):
		$v eq "rce"?	(1 << RCE_DBG):
		$v eq "net"?	(1 << NET_DBG):
		$v eq "api"?	(1 << API_DBG):
		$v eq "data"?	(1 << DATA_DBG):
		$v eq "lsa"?	(1 << LSA_DBG):
		$v eq "tedb"?	(1 << TEDB_DBG):
		$v eq "ws"?	(1 << WS_DBG):0
		;
}

sub dbg_cfg() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << CFG_DBG));
	}
	return 0;
}

sub dbg_api() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << API_DBG));
	}
	return 0;
}

sub dbg_data() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << DATA_DBG));
	}
	return 0;
}

sub dbg_lsa() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << LSA_DBG));
	}
	return 0;
}

sub dbg_tedb() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << TEDB_DBG));
	}
	return 0;
}

sub dbg_ws() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << WS_DBG));
	}
	return 0;
}

sub dbg_raw() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << RAW_DBG));
	}
	return 0;
}

sub print_dbg($;@) {
	my ($sys, $msg, @args) = @_;
	if(!defined($dbg_sys)) {
		return;
	}
	if($dbg_sys & (1<<$sys)) {
		$msg = sprintf($msg, @args);
		Log::log("info",  $msg);
	}
}

sub print_dbg_run($;@) {
	 print_dbg(RUN_DBG, @_);
}

sub print_dbg_net($;@) {
	 print_dbg(NET_DBG, @_);
}

sub print_dbg_api($;@) {
	print_dbg(API_DBG, @_);
}

sub print_dbg_data($;@) {
	if(!dbg_lsa()) {
		print_dbg(DATA_DBG, @_);
	}
}

sub print_dbg_lsa($;@) {
	print_dbg(LSA_DBG, @_);
}

sub print_dbg_tedb($;@) {
	print_dbg(TEDB_DBG, @_);
}

sub print_dbg_ws($;@) {
	print_dbg(WS_DBG, @_);
}

sub dump_config($;$) {
	my ($hr,$u) = @_;
	foreach my $k (sort(keys %$hr)) {
		if((ref($$hr{$k}) eq "HASH") && exists($$hr{$k}{v}) && $$hr{$k}{s}) {
			if(defined($$hr{$k}{v})) {
				printf("%s$k($$hr{$k}{s}):\t\t$$hr{$k}{v}\n", defined($u)?"$u:":"");
			}
			else {
				printf("%s$k($$hr{$k}{s}):\t\tundefined\n", defined($u)?"$u:":"");
			}
		}
		elsif(ref($$hr{$k}) eq "ARRAY") {
			for(my $i=0; $i<@{$$hr{$k}}; $i++) {
				dump_config(${$$hr{$k}}[$i], "$i: $k");
			}
		}
		else {
			dump_config($$hr{$k}, defined($u)?"$u:$k":$k);
		}
	}
}

sub chksum($$@) {
	my ($ppat, $upat, @d) = @_;
	my $block = pack($ppat, @d);
	my $chksum = unpack($upat, $block);
	return $chksum;
}

sub send_via_queue($@) {
	my $q = shift;
	my $t  = shift;
	my(@d) = @_;
	my $ref = &share({});
	$$ref{cmd} = $$t{cmd};
	$$ref{type} = $$t{type} if exists($$t{type}); 
	$$ref{subtype} = $$t{subtype} if exists($$t{subtype}); 
	$$ref{rtr} = $$t{rtr} if exists($$t{rtr}); 
	$$ref{client} = $$t{client} if exists($$t{client}); 
	$$ref{data} = &share([]);
	for(my $i=0; $i<@d; $i++) {
		${$$ref{data}}[$i] = $d[$i]; 
	}
	$q->enqueue($ref);
}

sub recv_from_tedb($;$) {
	my ($q, $to) = @_;
	my $d;
	$to = 5 if !defined($to);

	eval {
		local $SIG{ALRM} = sub { die "timeout"};
		alarm $to;
		$::timeout = 0;

		while(!defined($d)) {
			$d = $q->dequeue();
		}
		alarm 0;
	};
	if($@) {
		return "timeout";
	}
	return $d;
}

1;
