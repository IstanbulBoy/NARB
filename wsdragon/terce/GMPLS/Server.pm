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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.21 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub new {
	shift;
	my ($proc, $sock)  = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors
	my $self;
	eval {
		$self = {
			# process descriptor:
			"proc" => $proc,
			"addr" => $$proc_val{addr}, # process IPC address
			"name" => $$proc_val{name}, # process name
			"fh" => $$proc_val{fh},
			"pool" => $$proc_val{pool}, # empty
			"select" => new IO::Select($$proc_val{fh}), # select handle
			"parser" => new XML::Parser(Style => "tree"), # incomming data parser
			"processor" => \&process_msg, # msg processor

			# object descriptor:
			"sock" => $sock,	# server socket
		};
	};
	if($@) {
		die "$$proc_val{name} instantiation failed: $@\n";
	}
	$$self{select}->add($sock);
	bless $self;
	return $self;
}

sub activate_tedb() {
	my $self = shift;
	my @cmd = ({"fmt"=>"", "cmd"=>TEDB_ACTIVATE});
	Aux::send_msg($self, ADDR_GMPLS_CORE, @cmd);
}

sub process_msg() {
	my $self = shift;
	my ($msg)  = @_;
	my $d;

	# parse the message
	my $tr;  # XML tree reference
	eval {
		$tr = $$self{parser}->parse($msg);
		$d = Lib::xfrm_tree("msg", $$tr[1]);
		if(!defined($d)) {
			Log::log("warning", "IPC message parsing failed\n");
			return;
		}
	};
	if($@) {
		Log::log("err", "$@\n");
		return;
	}
	if(defined($d)) {
	}
}

sub process_bin_msg($) {
	my $self = shift;
	my ($fh) = @_;
	my %msg;
	my $sn;
	my $err;
	eval {
		$sn = GMPLS::API::get_msg($fh, \%msg);
	};
	if($@) {
		die "$@\n";
	}
	eval {
		$err = 0;
		if(GMPLS::API::is_sync_init($msg{$sn})) {
			# init the control channel
			my @data = ($$self{sock}->peerhost(), $msg{$sn}{hdr}{tag2});
			unshift(@data, {"fmt"=>"", "cmd"=>CTRL_CMD, "type"=>INIT_Q_T});
			Aux::send_msg($self, ADDR_GMPLS_CORE, @data);
			GMPLS::API::ack_msg($$self{sock}, $msg{$sn});
		}
		elsif(GMPLS::API::is_sync_insert($msg{$sn})) {
			if(GMPLS::API::parse_msg($msg{$sn}{data}, $self) <0) {
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
}

sub run() {
	my $self = shift;
	my %pipe_queue;
	my $gmpls_fh;

	Log::log "info", "starting $$self{name}\n";
	while(!$::ctrlC) {
		if(!$$self{sock}->connected()) {
			Log::log "err", "client disconnect\n";
			$$self{select}->remove($$self{sock});
			last;
		}
		$gmpls_fh = Aux::act_on_msg($self, \%pipe_queue);
		if(defined($gmpls_fh)) {
			eval {
				$self->process_bin_msg($gmpls_fh);
			};
			if($@) {
				Log::log "err", "$@\n";
				last;
			}
		}
	}
	if($$self{select}->exists($$self{sock})) {
		$$self{select}->remove($$self{sock});
	}
	$$self{select}->remove($$self{fh});
	Aux::print_dbg_run("exiting $$self{name}\n");
	close($$self{sock});
}

1;
