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

package GMPLS::Server;

use strict;
use warnings;
use Aux;
use GMPLS::API;
use GMPLS::Constants;
use IO::Select;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.9 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub activate_tedb($$) {
	my ($sq, $cn) = @_;
	my @cmd = ({"cmd"=>TEDB_ACTIVATE, "client"=>$cn});
	Aux::send_via_queue($sq, @cmd);
}

sub new {
	shift;
	my ($sock, $n, $sq, $cq)  = @_;
	my $select = new IO::Select($sock);
	my $self = {
		"name" => $n, 		# server name  
		"sock" => $sock,	# server socket
		"select" => $select,	# select handle
		"ctrl_sock" => undef,	# control channel socket
		"sq" => $sq,		# server queue (to ws server)
		"cq" => $cq		# cleint queue (from ws server)
	};
	bless $self;
	return $self;
}

sub run() {
	my $self = shift;
	my %msg;
	my $sn;
	my $err;
	while(!$::ctrlC) {
		if(!$$self{sock}->connected()) {
			Log::log "err", "client disconnect\n";
			last;
		}

		eval {
			$sn = GMPLS::API::get_msg($$self{select}, \%msg);
		};
		if($@) {
			Log::log "err", "$@\n";
			last;
		}
		if(!defined($sn)) {
			# timeout on select
			next;
		}
		eval {
			$err = 0;
			if(GMPLS::API::is_sync_init($msg{$sn})) {
				Aux::print_dbg_run("starting (%s) server thread\n", $$self{name});
				# open the control channel
				$$self{ctrl_sock} = GMPLS::API::open_ctrl_channel($$self{sock}, $msg{$sn}{hdr}{tag2});
				GMPLS::API::ack_msg($$self{sock}, $msg{$sn});
			}
			elsif(GMPLS::API::is_sync_insert($msg{$sn})) {
				if(GMPLS::API::parse_msg($msg{$sn}{data}, $$self{sq}, $$self{name}) <0) {
					$err = 1;
				}
				GMPLS::API::ack_msg($$self{sock}, $msg{$sn}, $err);
			}
			elsif(GMPLS::API::is_delim($msg{$sn})) {
				activate_tedb($$self{sq}, $$self{name});
				$msg{$sn} = {};
			}
			else {
				GMPLS::API::ack_msg($$self{sock}, $msg{$sn});
			}
		};
		if($@) {
			die "$@\n";
		}
		threads->yield();
	}
	Aux::print_dbg_run("exiting %s server thread\n", $$self{name});
	if(defined($$self{ctrl_sock})) {
		close($$self{ctrl_sock});
	}
	close($$self{sock});
}

1;
