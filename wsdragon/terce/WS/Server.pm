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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.34 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter SOAP::Transport::HTTP::Daemon);
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

sub new {
	shift;
	my ($lp)  = @_;
	my $self = {
		server => new SOAP::Transport::HTTP::Daemon(
			LocalAddr => inet_ntoa(INADDR_ANY),
			LocalPort => $lp,
			ReuseAddr => 1,
			Blocking => 1
		),
		lport => $lp,
		xml => undef
	};
	bless $self;
	$$self{server}->dispatch_to(new WS::Handlers($self));
	return $self;
}

# this is an override so we can fork the process and handle multiple requests 
sub handle {
	my $self = shift->new;
	while (my $c = $self->accept) {
		while (my $r = $c->get_request) {
			$self->request($r);
			$self->SUPER::handle;
			$c->send_response($self->response)
		}
		UNIVERSAL::isa($c, 'shutdown') ? $c->shutdown(2) : $c->close(); 
		$c->close;
	}
}

sub run() {
	my $self = shift;
	while(!$::ctrlC) {
		# WS server
		$$self{server}->handle;

	}
	$$self{server}->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting web services server\n");
}

1;
