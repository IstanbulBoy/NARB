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
use SOAP::Lite;
use SOAP::Transport::HTTP;
use Aux;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.42 $ =~ /(\d+)/g;
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


sub tag($$$);

sub grim {
	my $child;
	while ((my $waitedpid = waitpid(-1,WNOHANG)) > 0) {
		Aux::print_dbg_run("reaped $waitedpid with exit $?\n" ) if $? != 0;
	}
	$SIG{CHLD} = \&grim;
}

# this is a subclass of SOAP::Transport::HTTP::Server
sub new {
	my $self = shift;
	unless (ref $self) {
		my $class = ref($self) || $self;
		my $proc = shift;
		eval "require HTTP::Daemon" or die "$@\n" unless UNIVERSAL::can('HTTP::Daemon' => 'new');
		my @params = @_;
		$self = $class->SUPER::new();
		my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors
		eval {
			# process descriptor (prefix with class name so we won't clash with superclasses):
			$self->{_ws_srvr_proc} = $proc; # process info
			$self->{_ws_srvr_addr} = $$proc_val{addr}; # process IPC address
			$self->{_ws_srvr_name} = $$proc_val{name}; # process name
			$self->{_ws_srvr_fh} = $$proc_val{fh}; # IPC filehandle 
			$self->{_ws_srvr_pool} = $$proc_val{pool}; # socket pool for forked children
			$self->{_ws_srvr_select} = new IO::Select($$proc_val{fh}); # corresponding select object
			$self->{writer} = undef;
			$self->{_ws_srvr_parser} = undef;

			# object descriptor:
			$self->{_ws_daemon} = new HTTP::Daemon(@params) or die "Can't create daemon: $!\n";
			$self->{_ws_srvr_xml} = undef;
			};
		if($@) {
			die "$$proc_val{name} instantiation failed: $@\n";
		}
		$self->dispatch_to(new WS::Handlers($self));
	}
	return $self;
}

# fork a child handling the request
sub start_ws_server($) {
	my $self = shift;
	my ($c) = @_;
	while (my $r = $c->get_request) {
		$self->request($r);
		$self->handle();
		$c->send_response($self->response)
	}
	UNIVERSAL::isa($c, 'shutdown') ? $c->shutdown(2) : $c->close(); 
	$c->close;
}

sub run() {
	my $self = shift;
	my $pid;
	my $c;
	my $fh = undef;
	my $port = $$self{_ws_daemon}->sockport();

	$SIG{CHLD} = \&grim;

	Log::log "info", "starting $$self{_ws_srvr_name} on port $port\n";
	while(!$::ctrlC) {
		# WS server
		$c = $$self{_ws_daemon}->accept();
		if(!$c) {
			next;
		}
		# find an unused socket form the pool
		my $i;
		for($i=0; $i<MAX_SOAP_SRVR; $i++) {
			if(!${${$$self{_ws_srvr_pool}}[$i]}{in_use}) {
				$fh = ${${$$self{_ws_srvr_pool}}[$i]}{fh};
				${${$$self{_ws_srvr_pool}}[$i]}{in_use} = 1;
				last;
			}
		}
		Aux::print_dbg_run("WS request: forking $$self{_ws_srvr_name}\n");
		Aux::spawn(undef, undef, $self->start_ws_server, $$self{_ws_srvr_name}."($i)", $$self{_ws_srvr_addr}+$i, $fh);
	}
	$$self{_ws_daemon}->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting $$self{_ws_srvr_name}\n");
}

1;
