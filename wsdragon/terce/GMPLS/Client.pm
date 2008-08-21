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

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.13 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

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
			"fh" => $$proc_val{fh},
			"name" => $$proc_val{name}, # process name
			"pool" => $$proc_val{pool}, # empty
			"select" => new IO::Select($$proc_val{fh}), # select handle
			"writer" => new XML::Writer(OUTPUT => $$proc_val{fh}, ENCODING => "us-ascii"),
			"parser" => new XML::Parser(Style => "tree"), # incomming data parser
			"processor" => \&process_msg, # msg processor

			# object descriptor:
			"ctrl_sock" => undef,
		};
	};
	if($@) {
		die "$$proc_val{name} instantiation failed: $@\n";
	}
	bless $self;
	return $self;
}

sub open_ctrl_channel($$) {
	my $self = shift;
	my ($peer, $ctrl_p) = @_;
	my $ctrl_sock = IO::Socket::INET->new(
		PeerAddr => $peer,
		PeerPort => $ctrl_p,
		Proto     => 'tcp') or die "control socket ".$peer.":".$ctrl_p.": $@\n";
	if($ctrl_sock) {
		if($ctrl_sock->connected()) {
			$$self{select}->add($ctrl_sock);
			$$self{ctrl_sock} = $ctrl_sock;
			Aux::print_dbg_net("connected to %s:%d\n", $peer, $ctrl_p);
		}
	}
	return $ctrl_sock;
}

sub process_msg($) {
	my $self = shift;
	my ($msg)  = @_;
	my $d;

	# parse the message
	my $tr;  # XML tree reference
	eval {
		$tr = $$self{parser}->parse($msg);
		$d = Aux::xfrm_tree("msg", $$tr[1]);
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
		my @data = @{$$d{data}};
		if($$d{cmd} == CTRL_CMD) {
			if($$d{type} == INIT_ASYNC) {
				if(defined($data[0]) && defined($data[1])) {
					$self->open_ctrl_channel($data[0], $data[1]);
				}
			}
		}
		# findPath
		if($$d{cmd} == ASYNC_CMD) {
			if($$d{type} == RCE_MSG_LSP) {
				if(($$d{subtype} == ACT_QUERY) || ($$d{subtype} == ACT_QUERY_MRN)) {
				}
			}
		}
	}
}

sub run() {
	my $self = shift;
	my %pipe_queue;

	Log::log "info", "starting $$self{name}\n";
	while(!$::ctrlC) {
		Aux::act_on_msg($self, \%pipe_queue);
	}
	Aux::print_dbg_run("exiting $$self{name}\n");
	if(defined($$self{ctrl_sock})) {
		close($$self{ctrl_sock});
	}
}

1;
