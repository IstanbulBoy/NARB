package Aux;

use strict;
use sigtrap;
use English '-no_match_vars';
use IO::Socket;
use Errno qw( EAGAIN EINPROGRESS EPIPE ECONNRESET );
use Fcntl qw(F_GETFL F_SETFL O_NONBLOCK);
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Log;
use XML::Writer;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.34 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw( CTRL_CMD ASYNC_CMD INIT_ASYNC ADDR_TERCE ADDR_GMPLS_CORE ADDR_GMPLS_S ADDR_GMPLS_NARB_S ADDR_GMPLS_RCE_S ADDR_GMPLS_NARB_C ADDR_GMPLS_RCE_C ADDR_WEB_S ADDR_SOAP_S ADDR_SOAP_S_BASE MAX_SOAP_SRVR ADDR_GMPLS_S_BASE MAX_GMPLS_CS %msg_addr_X);
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
use constant INIT_ASYNC => 1; # this will initialize the client queue and open the async socket

use constant MAX_SOAP_SRVR => 10;
use constant MAX_GMPLS_CS => 4; #client/server

use constant ADDR_TERCE => 0;
use constant ADDR_GMPLS_CORE => 1;
use constant ADDR_GMPLS_S => 2;
use constant ADDR_WEB_S => 3;
use constant ADDR_SOAP_S => 4;
# soap server children addresses
use constant ADDR_SOAP_S_BASE => 5;
use constant ADDR_GMPLS_S_BASE => ADDR_SOAP_S_BASE + MAX_SOAP_SRVR;
use constant ADDR_GMPLS_NARB_S => ADDR_GMPLS_S_BASE;
use constant ADDR_GMPLS_NARB_C => ADDR_GMPLS_S_BASE + 1;
use constant ADDR_GMPLS_RCE_S => ADDR_GMPLS_S_BASE + 2;
use constant ADDR_GMPLS_RCE_C => ADDR_GMPLS_S_BASE + 3;

use constant TERCE_MSG_SCAN_L => 64;
use constant TERCE_MSG_CHUNK => 16384;

our %msg_addr_X = 	(
	0 => "TERCE",
	1 => "GMPLS CORE",
	2 => "GMPLS SERVER",
	16 => "GMPLS NARB CLIENT",
	18 => "GMPLS RCE CLIENT",
	3 => "WEB SERVER",
	4 => "SOAP SERVER",
	15 => "GMPLS NARB SERVER",
	17 => "GMPLS RCE SERVER"
);

my $dbg_sys = 0;

sub xfrm_tree($$);

sub catch_quiet_term {
	$::ctrlC = 1;
}


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

######################################################################
############################# IPC ####################################
######################################################################

# $root: root element for this recursion level
# $tr: element tree reference (message element tree from the parser)
############## data structure ####################
#[ msg, [ {"dst"=>2,"src"=>3}, data, [ {"fmt"=>"NN","cmd"=>3,"type"=>5...} item [{"seq"=>0},0,STRING0], item [{"seq"=>1},0,STRING1]]]]
sub xfrm_tree($$) {
	my ($root, $tr) = @_;
	my $attrs = shift(@$tr);
	my $fmt = undef;
	my $cmd = undef;
	my $type = undef;
	my $subtype = undef;
	my $rtr = undef;
	my $data = [];
	my $ret = undef;

	for(my $i=0; $i<@$tr; $i+=2) {
		my $el = $$tr[$i];
		my $chld = $$tr[$i+1];
		if($el eq "0") {
			return {"seq"=>$$attrs{seq}, "item"=>$chld}; 
		}
		else {
			push(@$data, xfrm_tree($el, $chld));
		}
	}

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
		@$data = sort {$$a{seq} <=> $$b{seq}} @$data;
		my $tmp = [];
		for(my $i=0; $i<@$data; $i++) {
			push(@$tmp, ${$$data[$i]}{item});
		}
		$ret = {
			cmd => $cmd,
			type => $type,
			subtype => $subtype,
			rtr => $rtr,
			data => $tmp
		};
		return $ret;
	}
	if(lc($root) eq "msg") {
		return $$data[0];
	}
}

sub receive_msg($) {
}

# $owner: sender process descriptor
# $dst: destination address
# @data: raw data
sub send_msg($$@) {
	my ($owner, $dst, @data) = @_;  
	my $hdr = shift @data;
	# $hdr: internal header describing the encapsulated data: 
	# 	fmt: template for packing and unpacking (required)
	#	cmd: processing instruction (required)
	#	type: type of data (optional)
	#	subtype: usually, tlv subtype such as "uni" (optional)
	#	rtr: advertizing router (optional)
	eval {
		$$owner{writer}->startTag("msg", "dst" => $dst, "src" => $$owner{addr});

		$$owner{writer}->startTag("data", 
			"fmt" => $$hdr{fmt}, 
			"cmd" => $$hdr{cmd}, 
			"type" => defined($$hdr{type})?$$hdr{type}:"undef",
			"subtype" => defined($$hdr{subtype})?$$hdr{subtype}:"undef",
			"rtr" => defined($$hdr{rtr})?$$hdr{rtr}:"undef");
		for(my $i=0; $i<@data; $i++) {
			 $$owner{writer}->dataElement("item", $data[$i], "seq"=>$i);
		}

		$$owner{writer}->endTag("data");
		$$owner{writer}->endTag("msg");
		$$owner{writer}->end();
	};
	if($@) {
		die "send_msg: $@";
	}
}

# this will either forward, consume the IPC message or return the socket handle
# if it does not belong to the IPC address space
sub act_on_msg($$) {
	my ($owner, $queue_ref) = @_;

	my @readable = $$owner{select}->can_read();
	if($!) {
		return undef;
	}

	foreach my $h (@readable) {
		# return if not an IPC socket
		if(!exists($$owner{proc}{$h})) {
			return $h;
		}
		my $src_n = fileno($h);
		if(!exists($$queue_ref{$src_n}{buffer})) {
			# create new stream buffer and start scanning
			Aux::print_dbg_msg("setting up a pipe queue on %s for %s\n", $$owner{name}, $$owner{proc}{$h}{name});
			$$queue_ref{$src_n}{buffer} = "";  # stream buffer
			$$queue_ref{$src_n}{src} = $h;
		}
		my $dst;
		my $addr;

		while(1) {
			my $tmp;
			$addr = $h->recv($tmp, TERCE_MSG_SCAN_L, MSG_DONTWAIT);
			$$queue_ref{$src_n}{buffer} .= $tmp;
			if(!defined($addr) || $!) {
				if(($! != EAGAIN)) {
					Log::log "err",  "recv on $$owner{proc}{$h}{name} failed\n";
				}
				last;
			}
			# lock on the message and discard anything before the message start tag
			if($$queue_ref{$src_n}{buffer} =~ /.*?(<msg(.*?)>.*?<\/msg>)(.*)/) {
				my $attrs = $2;
				$$queue_ref{$src_n}{msg} = $1;
				if(defined($3)) {
					$$queue_ref{$src_n}{buffer} = $3; # shorten the buffer to the unprocessed data
				}
				else {
					$$queue_ref{$src_n}{buffer} = "";
				}
				$attrs =~ /dst.*?=.*?"(\d+?)"(?:\s|$)/;
				$dst = $1;
				$$queue_ref{$src_n}{dst} = $$owner{proc}{$dst}{fh};

				# consume
				if($dst == $$owner{addr}) {
					if(defined($$owner{processor})) {
						$$owner{processor}($owner, $$queue_ref{$src_n}{msg});
					}
					$$queue_ref{$src_n}{msg} = "";
				}
				# forward everything in the queues (only the parent is allowed to forward)
				elsif($$owner{addr} == ADDR_TERCE) {
					foreach my $k (keys %$queue_ref)  {
						if(!length($$queue_ref{$k}{msg})) {
							next;
						}
						my @writeable = $$owner{select}->can_write();
						my $n = 0;
						foreach my $wh (@writeable) {
							if($wh == $$queue_ref{$k}{dst}) {
								$n = $$queue_ref{$k}{dst}->send($$queue_ref{$k}{msg}, MSG_DONTWAIT);
								last;
							}
						}
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
							Aux::print_dbg_msg("forward: %s -> %s\n", $$owner{proc}{$$queue_ref{$k}{src}}{name}, $$owner{proc}{$$queue_ref{$k}{dst}}{name});
							$$queue_ref{$k}{msg} = "";
						}
					}
				}
			}
		}
	}
	return undef;
}

# $child_mapref: a map of all open sockets to their associated process info
# $selref:  IO::Select object - the core of the IPC router
# $coderef: child's entry point
# $proc_name: child's process name
# $proc_addr: child's process IPC address
# $pool_fh: if defined, spawn will use this file handle and will not allocate a socket pair
# @args: all the remaining arguments are passed to &$coderef as its arguments
sub spawn($$$$$$@) {
	my ($child_mapref, $selref, $coderef, $proc_name, $proc_addr, $pool_fh, @args) = @_;
	my $pid;
	my $to_ch;
	my $to_ph;
	my $sp_pool = []; # socketpair pool
	# $to_ch: socket descriptor ... the socket used by parent to talk to child
	# $to_ph: socket descriptor ... the socket used by child to talk to parent
	if(!defined($pool_fh)) {
		($to_ch, $to_ph) = IO::Socket->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpair: $!\n";
		# create a pool of socket pairs for the SOAP server
		if($proc_addr == ADDR_SOAP_S) {
			for(my $i = 0; $i<MAX_SOAP_SRVR; $i++) {
				my ($to_ch_pool, $to_ph_pool) = IO::Socket->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpool: $!\n";
				push(@$sp_pool, [$to_ch_pool, $to_ph_pool]);
			}
		}
		# create a pool of socket pairs for the GMPLS server
		elsif($proc_addr == ADDR_GMPLS_S) {
			for(my $i = 0; $i<MAX_GMPLS_CS; $i++) {
				my ($to_ch_pool, $to_ph_pool) = IO::Socket->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpool: $!\n";
				push(@$sp_pool, [$to_ch_pool, $to_ph_pool]);
			}
		}
	}
	if (!defined($pid = fork)) {
		Log::log "err",  "cannot fork: $!";
		if(!defined($pool_fh)) {
			close $to_ph;
			close $to_ch;
		}
		die "cannot fork $proc_name\n";
	} elsif ($pid) {
		if(!defined($pool_fh)) {
			close $to_ph;
			$$selref->add($to_ch);
			# a doubly-keyed hash  (I don't think that this constitutes a closure ... we shall see)
			my $tmp = {"fh" => $to_ch, "addr" => $proc_addr, "cpid" => $pid, "name" => $proc_name};
			$$child_mapref{$proc_addr} = $tmp;
			$$child_mapref{$to_ch} = $tmp;
			# load the socketpair pool for the forked ws servers
			if($proc_addr == ADDR_SOAP_S) {
				for(my $i = 0; $i<MAX_SOAP_SRVR; $i++) {
					close ${$$sp_pool[$i]}[1];
					$$selref->add(${$$sp_pool[$i]}[0]);
					$tmp = {"fh" => ${$$sp_pool[$i]}[0], "addr" => (ADDR_SOAP_S_BASE+$i), "cpid" => $pid, "name" => $proc_name."($i)"};
					$$child_mapref{ADDR_SOAP_S_BASE+$i} = $tmp;
					$$child_mapref{${$$sp_pool[$i]}[0]} = $tmp;
				}
			}
			elsif($proc_addr == ADDR_GMPLS_S) {
				for(my $i = 0; $i<MAX_GMPLS_CS; $i++) {
					close ${$$sp_pool[$i]}[1];
					$$selref->add(${$$sp_pool[$i]}[0]);
					$tmp = {"fh" => ${$$sp_pool[$i]}[0], "addr" => (ADDR_GMPLS_S_BASE+$i), "cpid" => $pid, "name" => $proc_name."($i)"};
					$$child_mapref{ADDR_GMPLS_S_BASE+$i} = $tmp;
					$$child_mapref{${$$sp_pool[$i]}[0]} = $tmp;
				}
			}
		}
		return $pid;
	}
	my $s_pool = [];
	if(!defined($pool_fh)) {
		close $to_ch;
		if($proc_addr == ADDR_SOAP_S) {
			for(my $i = 0; $i<MAX_SOAP_SRVR; $i++) {
				close ${$$sp_pool[$i]}[0];
				push(@$s_pool, {fh => ${$$sp_pool[$i]}[1], in_use => 0, pid => 0});
			}
		}
		elsif($proc_addr == ADDR_GMPLS_S) {
			for(my $i = 0; $i<MAX_GMPLS_CS; $i++) {
				close ${$$sp_pool[$i]}[0];
				push(@$s_pool, {fh => ${$$sp_pool[$i]}[1], in_use => 0, pid => 0});
			}
		}
	}
	$SIG{TERM} = \&catch_quiet_term;
	$SIG{INT} = \&catch_quiet_term;
	$SIG{HUP} = \&catch_quiet_term;

	my $tmp;
	my %proc;
	# a doubly-keyed hash 
	if(defined($pool_fh)) {
		$tmp = {"fh" => $pool_fh, "addr" => $proc_addr, "cpid" => $$, "name" => $proc_name, "pool" => $s_pool};
		$proc{$proc_addr} = $tmp;
		$proc{$pool_fh} = $tmp;
	}
	else {
		$tmp = {"fh" => $to_ph, "addr" => $proc_addr, "cpid" => $$, "name" => $proc_name, "pool" => $s_pool};
		$proc{$proc_addr} = $tmp;
		$proc{$to_ph} = $tmp;
	}

	exit &$coderef(\%proc, @args); # this is the child's entry point
}


1;
