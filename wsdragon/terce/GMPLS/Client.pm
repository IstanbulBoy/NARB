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
# June 1, 2008
#
# File: Client.pm
#

package GMPLS::Client;

use strict;
use warnings;
use Socket;
use GMPLS::Constants;
use Aux;

use constant CQ_INIT_S => (1<<0);

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.2 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub new {
	shift;
	my ($n, $q)  = @_;
	my $self = {
		name => $n,
		ctrl_sock => undef,
		queue => $q,
		status => 0
	};
	bless $self;
	return $self;
}

sub open_ctrl_channel($$) {
	my ($peer, $ctrl_p) = @_;
	my $ctrl_sock = IO::Socket::INET->new(
		PeerAddr => $peer,
		PeerPort => $ctrl_p,
		Proto     => 'tcp') or die "control socket ".$peer.":".$ctrl_p.": $@\n";
	if($ctrl_sock) {
		if($ctrl_sock->connected()) {
			Aux::print_dbg_net("connected to %s:%d\n", $peer, $ctrl_p);
		}
	}
	return $ctrl_sock;
}


sub run() {
	my $self = shift;
	my $d;
	Aux::print_dbg_run("starting (%s) client queue\n", $$self{name});
	while(!$::ctrlC) {
		# this blocking queue is being controlled from WS server
		$d = $$self{queue}->dequeue();
		if(defined($d)) {
			my @data = @{$$d{data}};
			if($$d{cmd} == CTRL_CMD) {
				if($$d{type} == TERM_T_T) {
					last;
				}
				elsif($$d{type} == RUN_Q_T) {
				}
				elsif($$d{type} == INIT_Q_T) {
					if(defined($data[0]) && defined($data[1])) {
						$$self{ctrl_socket} = open_ctrl_channel($data[0], $data[1]);
						if(defined($$self{ctrl_socket})) {
							$$self{status} |= CQ_INIT_S;
						}
					}
				}
			}
		}
	}
	Aux::print_dbg_run("exiting %s client queue\n", $$self{name});
	if(defined($$self{ctrl_sock})) {
		close($$self{ctrl_sock});
	}
}

1;
