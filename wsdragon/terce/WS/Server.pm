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
use Socket;
use GMPLS::Constants;
use WS::External;
use WS::Handlers;
use SOAP::Lite;
use SOAP::Transport::HTTP;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.38 $ =~ /(\d+)/g;
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

# this is a subclass of SOAP::Transport::HTTP::Server
sub new {
	my $self = shift;
	unless (ref $self) {
		my $class = ref($self) || $self;
		$self = $class->SUPER::new(@_);
		my $proc = shift;
		my $s_pool = shift;
		my $proc_val = each %$proc;  # child processes hold only self-descriptors
		eval {
			# process descriptor:
			$self->{proc} = $proc; # process info
			$self->{addr} = $$proc_val{addr}; # process IPC address
			$self->{fh} = $$proc_val{fh}; # IPC filehandle 
			$self->{select} = new IO::Select($$proc_val{fh}); # corresponding select object
			$self->{parser} = new XML::Parser(Style => "tree"); # incomming data parser

			# object descriptor:
			$self->{pool} = $s_pool;
			$self->{xml} = undef;
			};
		if($@) {
			die "$$proc_val{name} instantiation failed: $@\n";
		}
		dispatch_to(new WS::Handlers($self));
	}
	return $self;
}

# fork a child handling the request
sub start_ws_server() {
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
	Log::log "info", "starting $$self{proc}{name} on port $$self{port}\n";
	while(!$::ctrlC) {
		# WS server
		$c = $self->accept();
		if(!$c) {
			next;
		}
		Aux::print_dbg_run("WS request: forking $$self{name}\n");
	}
	$$self{server}->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting $$self{name}\n");
}

1;
