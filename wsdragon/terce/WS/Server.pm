#
#	Copyright (c) 2007 Linux Kinetics, LLC
#	All rights reserved.
#
#    Redistribution and use in source and binary forms are permitted
#    provided that the above copyright notice and this paragraph are
#    duplicated in all such forms and that any documentation, advertising
#    materials, and other materials related to such distribution and use
#    acknowledge that the software was developed by the University of
#    Southern California, Information Sciences Institute.  The name of the
#    University may not be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
#    THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
#    WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Author: Jaroslav Flidr
# March 1, 2008
#
# File: Server.pm
#

package WS::Server;

use strict;
use warnings;
use sigtrap;
use POSIX;
use Socket;
use GMPLS::Constants;
use WS::External;
use WS::Handlers;
use WS::Constants;
use SOAP::Lite;
use SOAP::Transport::HTTP;
use Aux;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.52 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter SOAP::Transport::HTTP::Server);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;


use constant STAT_LINK_ID => 		(1<<0);
use constant STAT_LINK_TYPE => 		(1<<1);
use constant STAT_LINK_LOCAL => 	(1<<2);
use constant STAT_LINK_REMOTE =>	(1<<3);
use constant STAT_LINK_METRIC => 	(1<<4);
use constant STAT_LINK_CAPACITY => 	(1<<5);
use constant STAT_LINK_MAX_BW => 	(1<<6);
use constant STAT_LINK_SWCAP => 	(1<<7);

use constant STAT_LINK_COMPLETE => 	(
	STAT_LINK_ID | 
	STAT_LINK_LOCAL | 
	STAT_LINK_METRIC | 
	STAT_LINK_CAPACITY | 
	STAT_LINK_MAX_BW | 
	STAT_LINK_SWCAP);


our $pool_ref;

sub tag($$$);

sub grim {
	my $child;
	while ((my $waitedpid = waitpid(-1,WNOHANG)) > 0) {
		Aux::print_dbg_run("reaped $waitedpid with exit $?\n" ) if $? != 0;
		for(my $i=0; $i<MAX_SOAP_SRVR; $i++) {
			if(${$$pool_ref[$i]}{pid} == $waitedpid) {
				${$$pool_ref[$i]}{in_use} = 0;
				last;
			}
		}
	}
	$SIG{CHLD} = \&grim;
}

sub piper {
	Log::log('warning', 'the ws client closed connection prematurely\n');
	$::ctrlC = 1;
	$SIG{PIPE} = \&piper;
}

sub nice_piper {
	Log::log('warning', 'unexpected: WS daemon caught SIGPIPE\n');
	$SIG{PIPE} = \&nice_piper;
}

sub data_timeout() {
	$::ctrlC = 1;
	die 'data retrieval failed\n';
}

# this is a subclass of SOAP::Transport::HTTP::Server
sub new {
	my $self = shift;
	unless (ref $self) {
		my $class = ref($self) || $self;
		my $proc = shift;
		eval 'require HTTP::Daemon' or die "$@\n" unless UNIVERSAL::can('HTTP::Daemon' => 'new');
		my @params = @_;
		$self = $class->SUPER::new();
		my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors
		eval {
			# process descriptor (prefix with class name so we won't clash with superclasses):
			$self->{proc} = $proc; # process info
			$self->{pid} = $$proc_val{cpid}; # process PID
			$self->{addr} = $$proc_val{addr}; # process IPC address
			$self->{name} = $$proc_val{name}; # process name
			$self->{fh} = $$proc_val{fh}; # IPC filehandle 
			$self->{pool} = $$proc_val{pool}; # socket pool for forked children
			$self->{select} = undef; # will be defined after forking
			$self->{writer} = undef; # will be defined after forking
			$self->{parser} = undef; # will be defined after forking
			$self->{processor} = undef; # will be defined after forking

			# object descriptor:
			$self->{daemon} = new HTTP::Daemon(@params) or die "Can't create daemon: $!\n";
			$self->{xml} = undef;
			};
		if($@) {
			die "$$proc_val{name} instantiation failed: $@\n";
		}
		$pool_ref = $self->{pool};
		$self->dispatch_to(new WS::Handlers($self));
	}
	return $self;
}

sub retrieve_tedb($) {
	my $self = shift;
	my ($scope) = @_;
	alarm ALRM_WS_SELECT_TOPO;
	my $scope_m = 0;
	if(defined($scope)) {
		$scope_m |= (lc($scope) eq 'abstract')?SCOPE_ABS_M:0;
		$scope_m |= (lc($scope) eq 'control')?SCOPE_CRL_M:0;
		$scope_m |= (lc($scope) eq 'data')?SCOPE_DAT_M:0;
		$scope_m |= (lc($scope) eq 'all')?(SCOPE_CRL_M | SCOPE_DAT_M | SCOPE_ABS_M):0;
		# send the request to GMPLS Core
		my @cmd = ({'cmd'=>WS_GET_TEDB, 'type'=>$scope_m});
		Aux::send_msg($self, ADDR_GMPLS_CORE, @cmd);
	}
}

sub retrieve_path($) {
	my $self = shift;
	my ($som) = @_;
	my @args = ();

	if($som->match('//srcEndpoint')) {
		push(@args, $som->valueof('//srcEndpoint'));
	}
	if($som->match('//destEndpoint')) {
		push(@args, $som->valueof('//destEndpoint'));
	}
	if($som->match('//bandwidth')) {
		push(@args, $som->valueof('//bandwidth'));
	}
	if($som->match('//encoding')) {
		push(@args, $som->valueof('//encoding'));
	}
	if($som->match('//swcap')) {
		push(@args, $som->valueof('//swcap'));
	}
	if($som->match('//gpid')) {
		push(@args, $som->valueof('//gpid'));
	}
	if($som->match('//vtag')) {
		push(@args, $som->valueof('//vtag'));
	}

	my $attrs = $som->dataof('//findPath')->attr;

	my $lsp_opt = 0;
	my $lsp_act = ACT_QUERY;
	my @data = ();
	foreach my $a (keys %$attrs) {
		if(lc($a) eq 'strict') {
			$lsp_opt |= LSP_OPT_STRICT_HOP if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'preferred') {
			$lsp_opt |= LSP_OPT_N_FORCE_HOP if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'mrn') {
			$lsp_act = ACT_QUERY_MRN if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'bidirectional') {
			$lsp_opt |= LSP_OPT_BI_DIR if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'e2evtag') {
		}
		if(lc($a) eq 'vtagmask') {
			$lsp_opt |= LSP_OPT_VLAN_BMP if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'queryhold') {
			$lsp_opt |= LSP_OPT_QHOLD if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'subnetero') {
			$lsp_opt |= LSP_OPT_SUB_ERO if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'subnetdtl') {
			$lsp_opt |= LSP_OPT_SUB_DTL if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'altpaths') {
		}
		if(lc($a) eq 'allvtags') {
			$lsp_opt |= LSP_OPT_VTAG_ALL if(lc($$attrs{$a}) eq 'true');
		}
		if(lc($a) eq 'allwaves') {
			$lsp_opt |= LSP_OPT_LAMBDA_ALL if(lc($$attrs{$a}) eq 'true');
		}
	}
	alarm ALRM_WS_FIND_PATH;

	push(@data, $lsp_opt, @args);
	unshift(@data, {'cmd'=>WS_FIND_PATH, 'type'=>RCE_MSG_LSP, 'subtype'=>$lsp_act});
	Aux::send_msg($self, ADDR_GMPLS_RCE_C, @data);
}

sub retrieve_data() {
	my $self = shift;
	if(!$self->request()) {
		die 'undefined request\n';
	}
	# peep into the content: we need to know what data
	# to retrieve before handling the request
	my $req = eval { $self->deserializer->deserialize($self->request()->content()) };
	if($@) {
		die "deserializer failed: $@\n";
	}
	if($req->match('/Envelope/Body/selectNetworkTopology/scope')) {
		$self->retrieve_tedb($req->valueof('//scope'));
	}
	elsif(my $som = $req->match('/Envelope/Body/findPath')) {
		$self->retrieve_path($som);
	}
}

sub process_msg() {
	my $self = shift;
	my ($msg)  = @_;
	my $d;

	# parse the message
	my $tr;  # XML tree reference
	eval {
		$tr = $$self{parser}->parse($msg);
		$d = Aux::xfrm_tree('msg', $$tr[1]);
		if(!defined($d)) {
			Log::log('warning', 'IPC message parsing failed\n');
			return;
		}
	};
	if($@) {
		Log::log('err', "$@\n");
		return;
	}
	if(defined($d)) {
		my $type = $$d{type};
		my @data = @{$$d{data}};
		if($$d{cmd} == WS_SET_TEDB) {
			if($data[0] ne 'undef') {
				if($type & SCOPE_ABS_M) {
					$$self{abstract_tedb} = $data[0];
				}
				if($type & SCOPE_CRL_M) {
					$$self{control_tedb} = $data[0];
				}
				if($type & SCOPE_DAT_M) {
					$$self{data_tedb} = $data[0];
				}
			}
		}
	}
	$::ctrlC = 1;
}

# fork a child handling the request
sub start_ws_server($$$) {
	my ($proc, $self, $sock) = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors

	$SIG{PIPE} = \&piper;

	$$self{proc} = $proc; # process info
	$$self{pid} = $$proc_val{cpid}; # process PID
	$$self{addr} = $$proc_val{addr}; # process IPC address
	$$self{name} = $$proc_val{name}; # process name
	$$self{fh} = $$proc_val{fh}; # IPC filehandle 
	$$self{pool} = undef;
	$$self{select} = new IO::Select($$proc_val{fh}); # select handle
	$$self{writer} = new XML::Writer(OUTPUT => $$proc_val{fh}, ENCODING => 'us-ascii');
	$$self{parser} = new XML::Parser(Style => 'tree'); # incomming data parser
	$$self{processor} = \&process_msg; # msg processor

	$$self{abstract_tedb} = undef;
	$$self{control_tedb} = undef;
	$$self{data_tedb} = undef;

	$$self{select}->add($sock);

	my $ws_fh;
	my %pipe_queue;

	Log::log 'info', "starting $$self{name} (pid: $$self{pid})\n";
	eval {
		local $SIG{ALRM} = \&data_timeout;
		while(!$::ctrlC) {
			$ws_fh = Aux::act_on_msg($self, \%pipe_queue);
			# this is the client's WS request
			if(defined($ws_fh)) {
				my $r = $ws_fh->get_request;
				$self->request($r);
				$self->retrieve_data();
				$$self{select}->remove($ws_fh);
			}
		}
		alarm 0;
	};
	if($@) {
		Log::log 'err', "$@\n";
	}
	if(defined($sock->connected())) {
		$self->handle();
		$sock->send_response($self->response);
	}

	Aux::print_dbg_run("exiting $$self{name} (pid: $$self{pid})\n");
	if($$self{select}->exists($sock)) {
		$$self{select}->remove($sock);
		$sock->shutdown(SHUT_RDWR);
	}
	$$self{select}->remove($$self{fh});
	return 0;
}

sub run() {
	my $self = shift;
	my $pid;
	my $c;
	my $fh = undef;
	my $port = $$self{daemon}->sockport();

	$SIG{CHLD} = \&grim;
	$SIG{PIPE} = \&nice_piper;

	Log::log 'info', "starting $$self{name} (pid: $$self{pid}) on port $port\n";
	while(!$::ctrlC) {
		# WS server
		$c = $$self{daemon}->accept();
		if(!$c) {
			next;
		}
		# find an unused socket from the pool
		my $i;
		for($i=0; $i<MAX_SOAP_SRVR; $i++) {
			if(!${${$$self{pool}}[$i]}{in_use}) {
				$fh = ${${$$self{pool}}[$i]}{fh};
				${${$$self{pool}}[$i]}{in_use} = 1;
				last;
			}
		}
		$pid = Aux::spawn(undef, undef, \&start_ws_server, $$self{name}."($i)", ADDR_SOAP_S_BASE+$i, $fh, $self, $c);
		${${$$self{pool}}[$i]}{pid} = $pid;
		$c->close();
	}
	$$self{daemon}->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting $$self{name} (pid: $$self{pid})\n");
}

1;
