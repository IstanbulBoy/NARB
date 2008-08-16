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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.17 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub new {
	shift;
	my ($proc, $sock)  = @_;
	my $proc_val = each %$proc;  # child processes hold only self-descriptors
	my $self;
	eval {
		$self = {
			# process descriptor:
			"proc" => $proc,
			"addr" => $$proc_val{addr}, # process IPC address
			"fh" => $$proc_val{fh},
			"pool" => $$proc_val{pool}, # empty
			"select" => new IO::Select($$proc_val{fh}), # select handle
			"parser" => new XML::Parser(Style => "tree"), # incomming data parser

			# object descriptor:
			"sock" => $sock,	# server socket
			"gmpls_select" => new IO::Select($sock) # select handle
		};
	};
	if($@) {
		die "$$proc_val{name} instantiation failed: $@\n";
	}
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
			$sn = GMPLS::API::get_msg($$self{gmpls_select}, \%msg);
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
				Log::log "info", "starting $$self{name}\n";
				# init the control channel
				my @data = ($$self{sock}->peerhost(), $msg{$sn}{hdr}{tag2});
				unshift(@data, {"cmd"=>CTRL_CMD, "type"=>INIT_Q_T});
				Aux::send_msg($$self{proc}, ADDR_GMPLS_CORE, $$self{addr}, @data);
				GMPLS::API::ack_msg($$self{sock}, $msg{$sn});
			}
			elsif(GMPLS::API::is_sync_insert($msg{$sn})) {
				if(GMPLS::API::parse_msg($msg{$sn}{data}, $$self{proc}) <0) {
					$err = 1;
				}
				GMPLS::API::ack_msg($$self{sock}, $msg{$sn}, $err);
			}
			elsif(GMPLS::API::is_delim($msg{$sn})) {
				$self->activate_tedb();
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
	close($$self{sock});
}

sub activate_tedb() {
	my $self = shift;
	my @cmd = ({"cmd"=>TEDB_ACTIVATE});
	Aux::send_msg($$self{proc}, ADDR_GMPLS_CORE, $$self{addr}, @cmd);
}

1;
