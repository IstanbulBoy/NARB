package Aux;

use strict;
use sigtrap;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Log;
use XML::Writer;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.19 $ =~ /(\d+)/g;
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

use constant ADDR_TERCE => (1<<0);
use constant ADDR_GMPLS_CORE => (1<<1);
use constant ADDR_GMPLS_NARB_S => (1<<2);
use constant ADDR_GMPLS_NARB_C => (1<<3);
use constant ADDR_GMPLS_RCE_S => (1<<4);
use constant ADDR_GMPLS_RCE_C => (1<<5);
use constant ADDR_WEB_S => (1<<6);
use constant ADDR_SOAP_S => (1<<7);

use constant ADDR_SPACE => (ADDR_TERCE | ADDR_GMPLS_CORE| ADDR_GMPLS_NARB_S | ADDR_GMPLS_NARB_C | ADDR_GMPLS_RCE_S | ADDR_GMPLS_RCE_C | ADDR_WEB_S | ADDR_SOAP_S);

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

sub xfrm_tree($$);

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

# $tr: element tree reference (message element tree from the parser)
sub xfrm_tree($$) {
	my ($root, $tr) = @_;
	my $attrs = shift(@$tr);
	my $fmt = undef;
	my $cmd = undef;
	my $type = undef;
	my $subtype = undef;
	my $rtr = undef;
	my $client = undef;
	my $data = [];
	my $ret = undef;
	if(lc($root) eq "data") {
		if(exists($$attrs{fmt}) && defined($$attrs{fmt})) {
			$fmt = $$attrs{fmt};
		}
		else {
			die "missing \"fmt\" attribute\n";
		}
		if(exists($$attrs{cmd}) && defined($$attrs{cmd})) {
			$cmd = $$attrs{cmd};
		}
		else {
			die "missing \"cmd\" attribute\n";
		}
		if(exists($$attrs{type})) {
			$type = $$attrs{type};
		}
		if(exists($$attrs{subtype})) {
			$subtype = $$attrs{subtype};
		}
		if(exists($$attrs{rtr})) {
			$rtr = $$attrs{rtr};
		}
		if(exists($$attrs{client})) {
			$client = $$attrs{client};
		}
		if($$tr[0] == 0) {
			if(length($fmt)>0) {
				@$data = unpack($fmt, $$tr[1]);
			}
		}
		else {
			die "data element\n";
		}
		$ret = {
			cmd => $cmd,
			type => $type,
			subtype => $subtype,
			rtr => $rtr,
			client => $client,
			data => $data
		};
		return $ret;
	}
	foreach my $el (keys %$tr) {
		$ret = xfrm_tree($el, $$tr{$el});
	}
	return $ret;
}

# $proc: sender process descriptor
# $dst: destination address
# $src: source address
# @data: raw data
sub send_msg($$$@) {
	my ($proc, $dst, $src, @data) = @_;  
	my $hdr = shift @data;
	# $hdr: internal header describing the encapsulated data: 
	# 	fmt: template for packing and unpacking (required)
	#	cmd: processing instruction (required)
	#	type: type of data (optional)
	#	subtype: usually, tlv subtype such as "uni" (optional)
	#	rtr: advertizing router (optional)
	#	client: e.g. rce ... helps disambiguation of data source (optional)
	my $writer = new XML::Writer(OUTPUT => $$proc{fh}, ENCODING => "us-ascii");
	if(!defined($writer)) {
		Log::log "warning", "XML writer failure\n";
		return;
	}
	$writer->startTag("msg", "dst" => $dst, "src" => $src);
	$writer->startTag("data", 
		"fmt" => $$hdr{fmt}, 
		"cmd" => $$hdr{cmd}, 
		"type" => $$hdr{type}, 
		"subtype" => $$hdr{subtype}, 
		"rtr" => $$hdr{rtr}, 
		"client" => $$hdr{client});
	$writer->characters(pack($$hdr{fmt}, @data));
	$writer->endTag("data");
	$writer->endTag("msg");
	$writer->end();
}

# this will either forward or consume the IPC message
sub act_on_msg($$$$$) {
	my ($owner, $processor, $sel, $map_ref, $queue_ref) = @_;
	my @readable = $sel->can_read();

	foreach my $h (@readable) {
		my $src_n = fileno($h);
		if(!exists($$queue_ref{$src_n}{buffer})) {
			# create new stream buffer and start scanning
			Aux::print_dbg_msg("setting up a pipe queue for %s\n", $$map_ref{$h}{name});
			$$queue_ref{$src_n}{buffer} = "";  # stream buffer
		}
		my $o = length($$queue_ref{$src_n}{buffer});
		my $dst;
		my $n;
		my $c_cnt = 0;
		while(1) {
			$n = sysread($h, $$queue_ref{$src_n}{buffer}, TERCE_MSG_SCAN_L, $o);
			$c_cnt += $n;
			if(!$n) {
				last;
			}
			$o += $n;
			# lock on the message and discard anything before the message start tag
			if($$queue_ref{$src_n}{buffer} =~ /<msg(.*?)>.*?<\/msg>/) {
				my $attrs = $1;
				$$queue_ref{$src_n}{msg} = $&;
				$$queue_ref{$src_n}{buffer} = $'; # shorten the buffer to the unprocessed data
				$attrs =~ /dst.*?=.*?(\d+)(?:\s|$)/;
				$dst = $1;
				$$queue_ref{$src_n}{dst} = $$map_ref{$dst}{fh};

				$$queue_ref{$src_n}{buffer} = "";

				if(!($dst & ADDR_SPACE)) {
					Log::log "warning", "unknown/unspecified destination address ($dst)\n";
					last;
				}
				# consume
				if($dst == $owner) {
					&$processor($$queue_ref{$src_n}{msg});
					$$queue_ref{$src_n}{msg} = "";
				}
				# forward everything in the queues (only the parent is allowed to forward)
				elsif($owner == ADDR_TERCE) {
					foreach my $k (keys %$queue_ref)  {
						if(!length($$queue_ref{$k}{msg})) {
							next;
						}
						$n = syswrite($$queue_ref{$k}{dst}, $$queue_ref{$k}{msg});
						if(!defined($n)) {
							Log::log "warning", "message forwarding failed\n";
							$$queue_ref{$k}{msg} = "";
							last;
						}
						if($n < length($$queue_ref{$k}{msg})) {
							Aux::print_dbg_msg("incomplete forwarding (%d of %d)", $n, length($$queue_ref{$k}{msg}));
							# store to the queue buffer
							$$queue_ref{$k}{msg} = substr($$queue_ref{$k}{msg}, $n);
						}
						else {
							$$queue_ref{$k}{msg} = "";
						}
					}
				}
			}
			Aux::print_dbg_msg("received message from %s to %s\n", $$map_ref{$h}{name}, $$map_ref{$dst}{name});
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
