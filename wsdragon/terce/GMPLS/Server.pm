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
use POSIX;
use IO::Socket::INET;
use Aux;
use GMPLS::API;
use GMPLS::Constants;
use GMPLS::Client;
use IO::Select;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.25 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub grim {
	my $child;
	while ((my $waitedpid = waitpid(-1,WNOHANG)) > 0) {
		if($?) {
			$::ctrlC = 1;
		}
		Aux::print_dbg_run("reaped $waitedpid with exit $?\n" ) if $? != 0;
	}
	$SIG{CHLD} = \&grim;
}

sub new {
	shift;
	my ($proc)  = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors
	my $self;
	eval {
		$self = {
			# process descriptor:
			"proc" => $proc,
			"addr" => $$proc_val{addr}, # process IPC address
			"name" => $$proc_val{name}, # process name
			"fh" => $$proc_val{fh},
			"pool" => $$proc_val{pool},
			"select" => undef,
			"writer" => undef,
			"parser" => undef,
			"processor" => undef,

			# object descriptor:
			"daemon" => IO::Socket::INET->new(Listen => 5,
				LocalAddr => inet_ntoa(INADDR_ANY),
				LocalPort => $::cfg{gmpls}{port}{v},
				ReuseAddr => 1,
				Blocking => 1,
				Proto     => 'tcp')
		};
	};
	if($@) {
		die "$$proc_val{name} instantiation failed: $@\n";
	}
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
			my @data = ($fh->peerhost(), $msg{$sn}{hdr}{tag2});
			my $dst = ($$self{addr} == ADDR_GMPLS_NARB_S)?ADDR_GMPLS_NARB_C:ADDR_GMPLS_RCE_C;
			unshift(@data, {"fmt"=>"C/a*N", "cmd"=>CTRL_CMD, "type"=>INIT_ASYNC});
			Aux::send_msg($self, $dst, @data);
			GMPLS::API::ack_msg($fh, $msg{$sn});
		}
		elsif(GMPLS::API::is_sync_insert($msg{$sn})) {
			if(GMPLS::API::parse_msg($msg{$sn}{data}, $self) <0) {
				$err = 1;
			}
			GMPLS::API::ack_msg($fh, $msg{$sn}, $err);
		}
		elsif(GMPLS::API::is_delim($msg{$sn})) {
			$self->activate_tedb();
			$msg{$sn} = {};
		}
		else {
			GMPLS::API::ack_msg($fh, $msg{$sn});
		}
	};
	if($@) {
		die "$@\n";
	}
}

# GMPLS Client provides the async interface to narb and rce
sub start_gmpls_client($) {
	my ($proc) = @_;
	my $client;
	eval {
		$client = new GMPLS::Client($proc);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$client->run();
	}
}


sub start_gmpls_server($$$) {
	my ($proc, $self, $sock) = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors

	$$self{proc} = $proc;
	$$self{addr} = $$proc_val{addr}; # process IPC address
	$$self{name} = $$proc_val{name}; # process name
	$$self{fh} = $$proc_val{fh};
	$$self{pool} = $$proc_val{pool};
	$$self{select} = new IO::Select($$proc_val{fh}); # select handle
	$$self{writer} = new XML::Writer(OUTPUT => $$proc_val{fh}, ENCODING => "us-ascii");
	$$self{parser} = new XML::Parser(Style => "tree"); # incomming data parser
	$$self{processor} = \&process_msg; # msg processor

	$$self{select}->add($sock);
	my $gmpls_fh;
	my %pipe_queue;
	while(!$::ctrlC) {
		if(!$sock->connected()) {
			Log::log "err", "client disconnect\n";
			$$self{select}->remove($sock);
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
	if($$self{select}->exists($sock)) {
		$$self{select}->remove($sock);
	}
	$$self{select}->remove($$self{fh});
	Aux::print_dbg_run("exiting $$self{name}\n");
	close($sock);
}

sub run() {
	my $self = shift;
	my @conn;

	$SIG{CHLD} = \&grim;

	Log::log "info", "starting $$self{name}\n";
	while(!$::ctrlC) {
		# WS server
		@conn = $$self{daemon}->accept();
		if(!@conn) {
			next;
		}
		my $client_sock = $conn[0];
		my @tmp = sockaddr_in($conn[1]);
		my $peer_ip = inet_ntoa($tmp[1]);
		my $peer_port = $tmp[0];

		Aux::print_dbg_net("accepted connection from $peer_ip:$peer_port\n");
		my $n = "";
		my $addr = -1;
		my $fh = undef;
		my $addr_c = -1;
		my $fh_c = undef;
		if($peer_port eq $::cfg{gmpls}{narb_sport}{v}) {
			$n = "narb";
			$addr = ADDR_GMPLS_NARB_S;
			$addr_c = ADDR_GMPLS_NARB_C;
			$fh = ${${$$self{pool}}[0]}{fh};
			$fh_c = ${${$$self{pool}}[1]}{fh};
		}
		elsif($peer_port eq $::cfg{gmpls}{rce_sport}{v}) {
			$n = "rce";
			$addr = ADDR_GMPLS_RCE_S;
			$addr_c = ADDR_GMPLS_RCE_C;
			$fh = ${${$$self{pool}}[2]}{fh};
			$fh_c = ${${$$self{pool}}[3]}{fh};
		}
		else {
			Log::log "err", "narb/rce client is not using a known source port ($peer_port)\n";
			Log::log "err", "shutting down the client connection\n";
			$client_sock->shutdown(SHUT_RDWR);
			next;
		}
		Aux::print_dbg_run("GMPLS connection: forking $$self{name}\n");

		# start client queue
		eval {
			Aux::spawn(undef, undef, \&start_gmpls_client, "Client Queue ($n)", $addr_c, $fh_c);
		};
		if($@) {
			last;
		}
		Aux::spawn(undef, undef, \&start_gmpls_server, "GMPLS Server ($n)", $addr, $fh, $self, $client_sock);

	}
	Aux::print_dbg_run("exiting $$self{name}\n");
	$$self{daemon}->shutdown(SHUT_RDWR);
}

1;
