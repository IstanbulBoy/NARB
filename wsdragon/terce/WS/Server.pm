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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.45 $ =~ /(\d+)/g;
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
			$self->{proc} = $proc; # process info
			$self->{pid} = $$proc_val{cpid}; # process PID
			$self->{addr} = $$proc_val{addr}; # process IPC address
			$self->{name} = $$proc_val{name}; # process name
			$self->{fh} = $$proc_val{fh}; # IPC filehandle 
			$self->{pool} = $$proc_val{pool}; # socket pool for forked children
			$self->{select} = undef;
			$self->{writer} = undef;
			$self->{parser} = undef;
			$self->{processor} = undef;

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

sub process_msg() {
	my $self = shift;
	my ($msg)  = @_;

}

# fork a child handling the request
sub start_ws_server($$$) {
	my ($proc, $self, $sock) = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors

	$$self{proc} = $proc; # process info
	$$self{pid} = $$proc_val{cpid}; # process PID
	$$self{addr} = $$proc_val{addr}; # process IPC address
	$$self{name} = $$proc_val{name}; # process name
	$$self{fh} = $$proc_val{fh}; # IPC filehandle 
	$$self{pool} = undef;
	$$self{select} = new IO::Select($$proc_val{fh}); # select handle
	$$self{writer} = new XML::Writer(OUTPUT => $$proc_val{fh}, ENCODING => "us-ascii");
	$$self{parser} = new XML::Parser(Style => "tree"); # incomming data parser
	$$self{processor} = \&process_msg; # msg processor

	$$self{select}->add($sock);

	my $ws_fh;
	my %pipe_queue;

	Log::log "info", "starting $$self{name} (pid: $$self{pid})\n";
	while(!$::ctrlC) {
		$ws_fh = Aux::act_on_msg($self, \%pipe_queue);
		if(defined($ws_fh)) {
			eval {
				while (my $r = $ws_fh->get_request) {
					$self->request($r);
					$self->handle();
					$ws_fh->send_response($self->response)
				}
				UNIVERSAL::isa($ws_fh, 'shutdown') ? $ws_fh->shutdown(2) : $ws_fh->close(); 
				$$self{select}->remove($ws_fh);
				$::ctrlC = 1;
			};
			if($@) {
				Log::log "err", "$@\n";
				last;
			}
		}
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

	Log::log "info", "starting $$self{name} (pid: $$self{pid}) on port $port\n";
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
		$pid = Aux::spawn(undef, undef, \&start_ws_server, $$self{name}."($i)", $$self{addr}+$i, $fh, $self, $c);
		${${$$self{pool}}[$i]}{pid} = $pid;
	}
	$$self{daemon}->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting $$self{name} (pid: $$self{pid})\n");
}

1;
