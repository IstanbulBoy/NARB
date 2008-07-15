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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.15 $ =~ /(\d+)/g;
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
use constant MSG_DBG => 11;

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

use constant TERCE_MSG_SCAN => 1;
use constant TERCE_MSG_STARTED => 2;
use constant TERCE_MSG_PEND => 3;

use constant TERCE_MSG_SCAN_L => 64;
use constant TERCE_MSG_CHUNK => 16384;

our %msg_addr_X = 	(
	ADDR_TERCE => "TERCE",
	ADDR_GMPLS_CORE => "GMPLS CORE",
	ADDR_GMPLS_NARB_S => "GMPLS NARB SERVER",
	ADDR_GMPLS_NARB_C => "GMPLS NARB CLIENT",
	ADDR_GMPLS_RCE_S => "GMPLS RCE SERVER",
	ADDR_GMPLS_RCE_C => "GMPLS RCE CLIENT",
	ADDR_WEB_S => "WEB SERVER",
	ADDR_SOAP_S => "SOAP SERVER");

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
		$v eq "ws"?	(1 << WS_DBG):
		$v eq "msg"?	(1 << MSG_DBG):0
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

sub dbg_msg() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << MSG_DBG));
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

sub print_dbg_msg($;@) {
	print_dbg(MSG_DBG, @_);
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

sub send_msg($@) {
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

sub process_msg($$$;$) {
	my ($sel, $map_ref, $queue_ref, $forward) = @_;
	my @readable = $sel->can_read();

	foreach my $h (@readable) {

		my $src_n = fileno($h);
		if(!exists($$queue_ref{$src_n}{buffer})) {
			# create new stream buffer and start scanning
			Aux::print_dbg_msg("setting up a pipe queue for %s\n", $$map_ref{$h}{name});
			$$queue_ref{$src_n}{buffer} = "";  # stream buffer
			$$queue_ref{$src_n}{status} = TERCE_MSG_SCAN; # queue status
			$$queue_ref{$src_n}{length} = TERCE_MSG_SCAN_L; # initial (could be anything) amount of data to read
		}
		# <msg dst=4 src=2 len=47 fmt="NN">........</msg><msg dst=4 src=2 len=47 fmt="NN">........</msg>
		# <msg dst=4 src=2 len=47 fmt="NN">........</msg><msg dst=4 src=2 
		my $o = length($$queue_ref{$src_n}{buffer});
		my $dst;
		my $n;
		my $c_cnt = 0;
		while(1) {
			# handle, message, length, offset
			$n = sysread($h, $$queue_ref{$src_n}{buffer}, $$queue_ref{$src_n}{length}, $o);
			$c_cnt += $n;
			if(!$n) {
				last;
			}
			$o += $n;
			if($$queue_ref{$src_n}{status} == TERCE_MSG_STARTED) {
				# at this point the message length is known
				$$queue_ref{$src_n}{length} -= $n;
			}
			# lock on the message preamble and discard anything before the message start
			# and reinitialize the queue
			if($$queue_ref{$src_n}{buffer} =~ /<msg dst=(\d+) src=(\d+) len=(\d+) fmt="(.*)">/) {
				$$queue_ref{$src_n}{status} = TERCE_MSG_STARTED;
				$dst = $1;
				$$queue_ref{$src_n}{dst} = $$map_ref{$dst}{fh};
				$$queue_ref{$src_n}{msg} = $&;
				$$queue_ref{$src_n}{buffer} = $'; # shorten the buffer
				$$queue_ref{$src_n}{length} = $3 - length($&) - length($'); # remainder of the message
				if($$queue_ref{$src_n}{length} <= 0) {
					$$queue_ref{$src_n}{length} = TERCE_MSG_SCAN_L;
				}
				Aux::print_dbg_msg("receiving message from %s to %s (l: %d)\n", $$map_ref{$h}{name}, $$map_ref{$dst}{name}, $3);
			}
			# forward the message
			if(($$queue_ref{$src_n}{status} == TERCE_MSG_STARTED) && ($$queue_ref{$src_n}{buffer} =~ /<\/msg>/)) {
				$$queue_ref{$src_n}{msg} .= $`.$&;
				$$queue_ref{$src_n}{buffer} = $';
				$$queue_ref{$src_n}{length} = TERCE_MSG_SCAN_L;
				$$queue_ref{$src_n}{status} = TERCE_MSG_SCAN;
				if(defined($forward)) {
					Aux::print_dbg_msg("forwarding message from %s to %s (l: %d)\n", $$map_ref{$h}{name}, $$map_ref{$dst}{name}, length($$queue_ref{$src_n}{msg}));
					$n = syswrite($$queue_ref{$src_n}{dst}, $$queue_ref{$src_n}{msg});
					if(!defined($n)) {
						Log::log "warning", "relay: message delivery failed\n";
					}
					if($n < length($$queue_ref{$src_n}{msg})) {
						$$queue_ref{$src_n}{status} = TERCE_MSG_PEND;
					}
				}
			}
			# give another pipe a chance
			if((@readable > 1) && ($c_cnt > TERCE_MSG_CHUNK)) {
				Aux::print_dbg_msg("interrupting message\n");
				$c_cnt = 0;
				last;
			}
		}
	}
}

1;
