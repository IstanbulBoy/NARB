package Aux;

use strict;
use sigtrap;
use English '-no_match_vars';
use IO::Socket::INET;
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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.49 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw( 	TEDB_RTR_ON TEDB_INSERT TEDB_UPDATE TEDB_DELETE TEDB_ACTIVATE TEDB_LINK_MARK 
				CLIENT_Q_INIT CLIENT_Q_INIT_PORT
				WS_GET_TEDB WS_SET_TEDB WS_FIND_PATH
				ALRM_WS_SELECT_TOPO
			   	ADDR_TERCE ADDR_GMPLS_CORE ADDR_GMPLS_S ADDR_GMPLS_NARB_S ADDR_GMPLS_RCE_S ADDR_GMPLS_NARB_C ADDR_GMPLS_RCE_C ADDR_WEB_S ADDR_SOAP_S ADDR_SOAP_S_BASE MAX_SOAP_SRVR ADDR_GMPLS_S_BASE MAX_GMPLS_CS %msg_addr_X);
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub dump_config($;$);
sub dump_data_structure($;$$);
sub serialize($$);

# messaging commands
#    management commands
use constant CLIENT_Q_INIT => 1;

#    GMPLS commands
use constant TEDB_RTR_ON => 2;
use constant TEDB_INSERT => 3;
use constant TEDB_UPDATE => 4;
use constant TEDB_DELETE => 5;
use constant TEDB_LINK_MARK => 6;
use constant TEDB_ACTIVATE => 7;

#    WS commands
use constant WS_GET_TEDB => 8;
use constant WS_SET_TEDB => 9;
use constant WS_FIND_PATH => 10;

# debugging
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

# messaging types
#    management command types
use constant CLIENT_Q_INIT_PORT => 1;

# system constants
use constant MAX_SOAP_SRVR => 10;
use constant MAX_GMPLS_CS => 4; #client/server

use constant ALRM_WS_SELECT_TOPO => 10; # [sec]

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

use constant TERCE_MSG_S_IN_BUFF => (1<<0); # msg start in buffer
use constant TERCE_MSG_E_IN_BUFF => (1<<1); # msg end in buffer
use constant TERCE_MSG_IN_PROGRESS => (1<<2); # in the middle of a valid message
use constant TERCE_MSG_RDY => (1<<3); # ready for further processing

use constant TERCE_MSG_TERM => "</msg>";
use constant TMTL => length(TERCE_MSG_TERM); # Terce Msg Terminator Length

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
sub reconstruct_payload($$$$$);

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

sub dump_data_structure($;$$) {
	my ($data, $ind, $nl) = @_;
	if(!defined($ind)) {
		$ind = 0;
	}

	if(ref($data) eq "") {
			printf("%*s%s\n", (defined($nl)?$ind:0), "", $data);
	}
	elsif(ref($data) eq "SCALAR") {
			printf("%*s%s\n", (defined($nl)?$ind:0), "", $$data);
	}
	elsif(ref($data) eq "HASH") {
		printf("{\n");
		$ind += 2;
		foreach my $k (keys %$data) {
			printf("%*s%s => ", $ind, "", $k);
			dump_data_structure($$data{$k}, $ind);
		}
		printf("%*s}\n", $ind-2, "");
	}
	elsif(ref($data) eq "ARRAY") {
		printf("[\n");
		$ind += 2;
		for(my $i=0; $i<@$data; $i++) {
			dump_data_structure($$data[$i], $ind, 1);
		}
		printf("%*s]\n", $ind-2, "");
	}
}

sub chksum($$@) {
	my ($ppat, $upat, @d) = @_;
	my $block = pack($ppat, @d);
	my $chksum = unpack($upat, $block);
	return $chksum;
}

sub reconnect_graph($$) {
	my ($rr, $lr) = @_;
	foreach my $lid (keys %$rr) {
		${$$rr{$lid}}{remote} = $$lr{$lid};
	}
}

######################################################################
############################# IPC ####################################
######################################################################

# NOTE: the <item> element ALWAYS corresponds to just ONE data variable
# on the top of the tree structure.
sub reconstruct_payload($$$$$) {
	my ($root, $tr, $plr, $rmtr, $lnkr) = @_;

	# simple scalar 
	if($root eq "0") {
		$$plr = $tr;
		return;
	}

	my $attrs = shift(@$tr); # of the root (the parser creates a really confusing data structure)
	my $element;
	my $cont;
	my $pl;

	# reconstruct the data structures expressed by 
	# the XML message
	for(my $i=0; $i<@$tr; $i+=2) {
		$element = $$tr[$i];
		$cont = $$tr[$i+1];
		if($root eq "SCALAR") {
			reconstruct_payload($element, $cont, \$pl, $rmtr, $lnkr);
			$$plr = \$pl;
		}
		elsif($root eq "ARRAY") {
			$pl = [] if !defined($pl);
			reconstruct_payload($element, $cont, \$pl, $rmtr, $lnkr);
			$$plr = $pl;
		}
		elsif($root eq "HASH") {
			$pl = {} if !defined($pl);
			reconstruct_payload($element, $cont, \$pl, $rmtr, $lnkr);
			$$plr = $pl;
		}
		elsif($root eq "value") {
			reconstruct_payload($element, $cont, \$pl, $rmtr, $lnkr);
			# array
			if(exists($$attrs{idx})) {
				push(@{$$plr},$pl);
			}
			# hash
			elsif(exists($$attrs{key})) {
				${$$plr}{$$attrs{key}} = $pl;
				# for later reconstruction of broken circular references
				if($$attrs{key} eq "link_id") {
					$$lnkr{$pl} = $$plr;
				}
				if($$attrs{key} eq "remote") {
					if(exists($$attrs{ref})) {
						$$rmtr{$$attrs{ref}} = $$plr;
					}
				}
			}
		}
	}
}

# $root: root element for this recursion level
# $tr: element tree reference (message element tree from the parser)
############## data structure ####################
#[ msg, [ {"dst"=>2,"src"=>3}, data, [ {"cmd"=>3,"type"=>5...} item [{"idx"=>0},0,STRING0], item [{"idx"=>1},"HASH",[....etc...]]]]]
# this function can process only a structure resulting from the IPC msg XML schema:
# e.g.:
sub xfrm_tree($$) {
	my ($root, $tr) = @_;
	my $attrs = shift(@$tr);
	my $cmd = undef;
	my $type = undef;
	my $subtype = undef;
	my $rtr = undef;
	my $ret = undef;


	my $element;
	my $cont;
	if(lc($root) eq "msg") {
		$element = $$tr[0];
		$cont = $$tr[1];
		# examine <data>
		return xfrm_tree($element, $cont);
	}
	elsif(lc($root) eq "data") {
		# iterate over all the <item> elements
		my $data = [];
		for(my $i=0; $i<@$tr; $i+=2) {
			$element = $$tr[$i];
			$cont = $$tr[$i+1];
			push(@$data, xfrm_tree($element, $cont));
		}
		# NOTE: all the functions a recursive. All the data is
		# more or less processed at this point.
		@$data = sort {$$a{idx} <=> $$b{idx}} @$data;

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
	elsif(lc($root) eq "item") {
		my $pl;
		my %rmt_w_links = ();
		my %links_w_rmt = ();
		$element = $$tr[0];
		$cont = $$tr[1];
		reconstruct_payload($element, $cont, \$pl, \%rmt_w_links, \%links_w_rmt);
		reconnect_graph(\%rmt_w_links, \%links_w_rmt);
		return {idx=>$$attrs{idx}, item=>$pl};
	}
}

sub in_addr_space($) {
	my ($addr) = @_;
	if(!defined($addr)) {
		return 0 
	}
	return(($addr >= ADDR_TERCE) && ($addr <= ADDR_GMPLS_RCE_C));
}

sub receive_msg($) {
}

# this recursive function will serialize even data structures containing 
# circular references such as TEDBs (connected graphs)
# Only SCALARs and refs to SCALAR, ARRAY, HASH are serialized
# NOTE: it can break up the TEDB data structures only (for now ... it
# makes a faster code)
# $w: XML writer
# $d: data chunk
sub serialize($$) {
	my ($w, $d) = @_;
	if(!defined($d)) {
		$w->characters("undef");
		return;
	}
	my $type = ref($d);
	# no function, io, lvalue ref serialization maybe in the furure
	if(($type eq "CODE") || ($type eq "GLOB") || ($type eq "REF") || ($type eq "LVALUE")) {
		$w->characters("undef");
	}
	# not a reference -> data
	elsif($type eq "") {
		$w->characters($d);
	}
	# scalar ref
	elsif($type eq "SCALAR") {
		$w->startTag($type);
		serialize($w, $$d);
		$w->endTag($type);
	}
	# array ref
	elsif($type eq "ARRAY") {
		$w->startTag($type);
		for(my $i=0; $i<@$d; $i++) {
			# we have to restore array order on deserialize
			$w->startTag("value", "idx"=>$i);
			serialize($w, $$d[$i]);
			$w->endTag("value");
		}
		$w->endTag($type);
	}
	# hash ref
	elsif($type eq "HASH") {
		$w->startTag($type);
		foreach my $k (keys %$d) {
			if(($k eq "remote") && (ref($$d{$k}) eq "HASH")) {
				# break up recursive link references
				$w->startTag("value", "key"=>$k, "ref"=>${$$d{$k}}{link_id});
				serialize($w, undef);
				$w->endTag("value");
			}
			else {
				$w->startTag("value", "key"=>$k);
				serialize($w, $$d{$k});
				$w->endTag("value");
			}
		}
		$w->endTag($type);
	}
	# all what remains is an object ref -> a bit tricky:
	# would have to be re-blessed, reconstructed, etc. ignore for now
	else {
		$w->characters("undef");
	}
}

# $owner: sender process descriptor
# $dst: destination address
# @data: raw data (it could be another xml doc)
sub send_msg($$@) {
	my ($owner, $dst, @data) = @_;  
	my $hdr = shift @data;
	# $hdr: internal header describing the encapsulated data: 
	#	cmd: processing instruction (required)
	#	type: type of data (optional)
	#	subtype: usually, tlv subtype such as "uni" (optional)
	#	rtr: advertizing router (optional)
	eval {
		$$owner{writer}->startTag("msg", "dst" => $dst, "src" => $$owner{addr});

		$$owner{writer}->startTag("data", 
			"cmd" => $$hdr{cmd}, 
			"type" => defined($$hdr{type})?$$hdr{type}:"undef",
			"subtype" => defined($$hdr{subtype})?$$hdr{subtype}:"undef",
			"rtr" => defined($$hdr{rtr})?$$hdr{rtr}:"undef");
		for(my $i=0; $i<@data; $i++) {
			 $$owner{writer}->startTag("item", "idx"=>$i);
			 serialize($$owner{writer}, $data[$i]);
			 $$owner{writer}->endTag("item");
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
	if(!scalar(@readable)) {
		return undef;
	}

	foreach my $h (@readable) {
		# return if not an IPC socket
		if(!exists($$owner{proc}{$h})) {
			return $h;
		}
		if(!defined($h->connected())) {
			$$owner{select}->remove($h);
			next;
		}
		my $src_n = fileno($h);
		if(!exists($$queue_ref{$src_n}{buffer})) {
			# create new stream buffer and start scanning
			Aux::print_dbg_msg("setting up a pipe queue on %s for %s\n", $$owner{name}, $$owner{proc}{$h}{name});
			$$queue_ref{$src_n}{buffer} = "";  # stream buffer
			$$queue_ref{$src_n}{status} = 0;  # queue status
			$$queue_ref{$src_n}{dst} = undef;  # current destination
			$$queue_ref{$src_n}{msg} = "";
			$$queue_ref{$src_n}{src} = $h;
		}
		my $dst;
		my $n;

		while(1) {
			$n = $h->sysread($$queue_ref{$src_n}{buffer}, TERCE_MSG_SCAN_L, length($$queue_ref{$src_n}{buffer}));
			if(!defined($n)) {
				if(($! != EAGAIN)) {
					Log::log "err",  "recv on $$owner{proc}{$h}{name} failed with $!\n";
					$$owner{select}->remove($h);
					return undef;
				}
				last;
			}
			# this will terminate all the "TERCE Core" connections after SIGTERM or SIGINT
			if(!$n) {
				$$owner{select}->remove($h);
				last;
			}
			# scan the buffer and lock on the message
			if($$queue_ref{$src_n}{buffer} =~ /<msg.*?>/) {
				# don'l look for a new msg while processing the old one
				if(!($$queue_ref{$src_n}{status} & TERCE_MSG_IN_PROGRESS)) {
					$$queue_ref{$src_n}{status} |= TERCE_MSG_S_IN_BUFF;
				}
			}
			if($$queue_ref{$src_n}{buffer} =~ /<\/msg>/) {
				$$queue_ref{$src_n}{status} |= TERCE_MSG_E_IN_BUFF;
			}

			# process the header
			if($$queue_ref{$src_n}{status} & TERCE_MSG_S_IN_BUFF) {
				# get the dst info and process the header
				$$queue_ref{$src_n}{buffer} =~ /^.*?(<msg(.*?)>)(.*)$/s;
				my $msg_hdr = $1;
				my $attrs = $2;
				$$queue_ref{$src_n}{buffer} = $3; # discard garbage before the header (e.g. \n) 
				$attrs =~ /dst.*?=.*?"(\d+?)"(?:\s|$)/;
				$dst = $1;
				if(in_addr_space($dst)) {
					$$queue_ref{$src_n}{msg} = $msg_hdr;
					$$queue_ref{$src_n}{dst}{addr} = $dst;
					$$queue_ref{$src_n}{dst}{fh} = $$owner{proc}{$dst}{fh};
					$$queue_ref{$src_n}{status} |= TERCE_MSG_IN_PROGRESS;
				}
				else {
					Log::log "err", "IPC message header corrupted\n";
					$$queue_ref{$src_n}{msg} = "";
					$$queue_ref{$src_n}{dst} = undef;
					$$queue_ref{$src_n}{status} &= ~TERCE_MSG_E_IN_BUFF;
				}
				$$queue_ref{$src_n}{status} &= ~TERCE_MSG_S_IN_BUFF;
			}
			# process the end
			if($$queue_ref{$src_n}{status} & TERCE_MSG_E_IN_BUFF) {
				$$queue_ref{$src_n}{buffer} =~ /^(.*?<\/msg>)/s;
				$$queue_ref{$src_n}{msg} .= $1;
				$$queue_ref{$src_n}{buffer} = substr($$queue_ref{$src_n}{buffer}, length($1));
				$$queue_ref{$src_n}{status} &= ~TERCE_MSG_E_IN_BUFF;
				$$queue_ref{$src_n}{status} &= ~TERCE_MSG_IN_PROGRESS;
				$$queue_ref{$src_n}{status} |= TERCE_MSG_RDY;
			}
			if($$queue_ref{$src_n}{status} & TERCE_MSG_IN_PROGRESS) {
				my $l = length($$queue_ref{$src_n}{buffer});
				if($l<TMTL) {
					$l = TMTL;
				}
				$$queue_ref{$src_n}{msg} .= substr($$queue_ref{$src_n}{buffer}, 0, $l - TMTL);
				# make sure we don't split the terminator: </msg>
				$$queue_ref{$src_n}{buffer} = substr($$queue_ref{$src_n}{buffer}, $l - TMTL);
			}

			# consume if ready
			if(($$queue_ref{$src_n}{status} & TERCE_MSG_RDY) && ($$queue_ref{$src_n}{dst}{addr} == $$owner{addr})) {
					$$queue_ref{$src_n}{dst} = undef;
					if(defined($$owner{processor})) {
						$$owner{processor}($owner, $$queue_ref{$src_n}{msg});
					}
					$$queue_ref{$src_n}{msg} = "";
					$$queue_ref{$src_n}{status} &= ~TERCE_MSG_RDY;
			}
			# forward as soon as we know where
			elsif($$owner{addr} == ADDR_TERCE) {
				foreach my $k (keys %$queue_ref)  {
					if(!length($$queue_ref{$k}{msg})) {
						next;
					}
					my @writeable = $$owner{select}->can_write();
					my $n = 0;
					# make sure that the dst is writeable
					foreach my $wh (@writeable) {
						if(!defined($$queue_ref{$k}{dst})) {
							next;
						}
						if($wh == $$queue_ref{$k}{dst}{fh}) {
							$n = $wh->syswrite($$queue_ref{$k}{msg});
							last;
						}
					}
					if(!defined($n)) {
						Log::log "warning", "message forwarding failed\n";
						$$queue_ref{$k}{msg} = "";
						last;
					}
					$$queue_ref{$k}{msg} = substr($$queue_ref{$k}{msg}, $n);
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
	my $flags;
	my $to_ch;
	my $to_ph;
	my $sp_pool = []; # socketpair pool
	# $to_ch: socket descriptor ... the socket used by parent to talk to child
	# $to_ph: socket descriptor ... the socket used by child to talk to parent
	if(!defined($pool_fh)) {
		($to_ch, $to_ph) = IO::Socket::INET->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpair: $!\n";
		$flags = fcntl($to_ch, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
		$flags = fcntl($to_ch, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
		$flags = fcntl($to_ph, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
		$flags = fcntl($to_ph, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
		# create a pool of socket pairs for the SOAP server
		if($proc_addr == ADDR_SOAP_S) {
			for(my $i = 0; $i<MAX_SOAP_SRVR; $i++) {
				my ($to_ch_pool, $to_ph_pool) = IO::Socket::INET->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpool: $!\n";
				$flags = fcntl($to_ch_pool, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
				$flags = fcntl($to_ch_pool, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
				$flags = fcntl($to_ph_pool, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
				$flags = fcntl($to_ph_pool, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
				push(@$sp_pool, [$to_ch_pool, $to_ph_pool]);
			}
		}
		# create a pool of socket pairs for the GMPLS server
		elsif($proc_addr == ADDR_GMPLS_S) {
			for(my $i = 0; $i<MAX_GMPLS_CS; $i++) {
				my ($to_ch_pool, $to_ph_pool) = IO::Socket::INET->socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC) or  die "socketpool: $!\n";
				$flags = fcntl($to_ch_pool, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
				$flags = fcntl($to_ch_pool, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
				$flags = fcntl($to_ph_pool, F_GETFL, 0) or die "Can't get flags for the socket: $!\n";
				$flags = fcntl($to_ph_pool, F_SETFL, $flags | O_NONBLOCK) or die "Can't set flags for the socket: $!\n";
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
