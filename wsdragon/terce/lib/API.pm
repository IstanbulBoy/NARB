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
# File: API.pm
#

package API;

use strict;
use warnings;
use Aux;
use Log;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.1 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

use constant TERCE_TOPO_SYNC => 0x11;
use constant TERCE_TOPO_ASYNC => 0x12;

use constant ACT_QUERY => 0x01;
use constant ACT_INSERT => 0x02; 
use constant ACT_DELETE => 0x03;
use constant ACT_UPDATE => 0x04;
use constant ACT_ACK => 0x05;
use constant ACT_ACKDATA => 0x06;
use constant ACT_ERROR => 0x07;
use constant ACT_INIT => 0x0A;
use constant ACT_ALIVE => 0x0B;

my %msg_type = 	(	0x11 => "TERCE_TOPO_SYNC",
			0x12 => "TERCE_TOPO_ASYNC");

my %msg_action =	(	
			0x01 => "ACT_QUERY",
			0x02 => "ACT_INSERT", 
			0x03 => "ACT_DELETE",
			0x04 => "ACT_UPDATE",
			0x05 => "ACT_ACK",
			0x06 => "ACT_ACKDATA",
			0x07 => "ACT_ERROR",
			0x0A => "ACT_INIT",
			0x0B => "ACT_ALIVE");
sub dump_data($) {
	my ($mr) = @_;
	if(Aux::dbg_data()) {
		Aux::print_dbg_data("----------------------- header -----------------------\n");
		Aux::print_dbg_data("(%s, %s)\n", $msg_type{$$mr{hdr}{type}}, $msg_action{$$mr{hdr}{action}});
		Aux::print_dbg_data("type action length:\t0x%02X (%u)  0x%02X (%u)  0x%04X (%u)\n", $$mr{hdr}{type}, $$mr{hdr}{type}, $$mr{hdr}{action}, $$mr{hdr}{action}, $$mr{hdr}{length}, $$mr{hdr}{length});
		Aux::print_dbg_data("ucid:\t\t\t0x%08X (%u)\n", $$mr{hdr}{ucid}, $$mr{hdr}{ucid});
		Aux::print_dbg_data("seq. number:\t\t0x%08X (%u)\n", $$mr{hdr}{seqn}, $$mr{hdr}{seqn});
		Aux::print_dbg_data("checksum:\t\t0x%08X (%u)\n", $$mr{hdr}{chksum}, $$mr{hdr}{chksum});
		Aux::print_dbg_data("tag1:\t\t\t0x%08X (%u)\n", $$mr{hdr}{tag1}, $$mr{hdr}{tag1});
		Aux::print_dbg_data("tag2:\t\t\t0x%08X (%u)\n", $$mr{hdr}{tag2}, $$mr{hdr}{tag2});
		Aux::print_dbg_data("------------------------ data ------------------------\n");
		if(defined($$mr{data})) {
			my @tmp = unpack("C*", $$mr{data});
			my $len = $$mr{hdr}{length};
			for(my $i=0; $i<$len; $i++) {
				Aux::print_dbg_data("%02X ", $tmp[$i]);
				if(($i+1)%16 && (($i+1)<$len)) {
					Aux::print_dbg_data("\n");
				}
			}
		}
		Aux::print_dbg_data("------------------------------------------------------\n");
	}
}

sub open_ctrl_channel($$) {
	my ($s, $ctrl_p) = @_;
	my $ctrl_sock = IO::Socket::INET->new(
		PeerAddr => $s->peerhost(),
		PeerPort => $ctrl_p,
		Proto     => 'tcp') or die "control socket ".$s->peerhost().":".$ctrl_p.": $@\n";
	if($ctrl_sock) {
		if($ctrl_sock->connected()) {
			Aux::print_dbg_net("connected to %s:%d\n", $s->peerhost(), $ctrl_p);
		}
	}
	return $ctrl_sock;
}

sub ack_msg($$) {
	my ($s, $mr) = @_;
	my $block = pack("CCSNN", TERCE_TOPO_SYNC, ACT_ACK, 0, $$mr{hdr}{ucid}, $$mr{hdr}{seqn});
	my $chksum = unpack("%32N3", $block);
	my $ack_msg = {
		"hdr" => {
			"type" => TERCE_TOPO_SYNC,
			"action" => ACT_ACK,
			"length" => 0,
			"ucid" => $$mr{hdr}{ucid},
			"seqn" => $$mr{hdr}{seqn},
			"chksum" => $chksum,
			"tag1" => 0,
			"tag2" => 0
		},
		"data" => undef
	};
	my $ack_msg_bin = pack("CCSNNNNN", 
		$$ack_msg{hdr}{type}, 
		$$ack_msg{hdr}{action}, 
		$$ack_msg{hdr}{length}, 
		$$ack_msg{hdr}{ucid},
		$$ack_msg{hdr}{seqn},
		$$ack_msg{hdr}{chksum},, 
		$$ack_msg{hdr}{tag1},
		$$ack_msg{hdr}{tag2}
	);
	if(defined($s->send($ack_msg_bin, 0))) {
		Aux::print_dbg_api("sent ACK for %s to %s:%s\n", $msg_action{$$mr{hdr}{action}},
			$s->peerhost(), $s->peerport()
		);
		dump_data($ack_msg);
	}
}

sub get_msg($$) {
	my ($s, $mr) = @_;
	#get the header
	my $hdr;
	my $data;
	my ($type, $action, $len, $ucid, $sn, $chksum, $tag1, $tag2);
	if(defined($s->recv($hdr, 24, 0))) {
		($type, $action, $len, $ucid, $sn, $chksum, $tag1, $tag2) = unpack("CCSNNNNN", $hdr);	
		$$mr{$sn} = {
			"hdr" => {
				"type" => $type,
				"action" => $action,
				"length" => $len,
				"ucid" => $ucid,
				"seqn" => $sn,
				"chksum" => $chksum,
				"tag1" => $tag1,
				"tag2" => $tag2
			},
			"data" => undef
		};

	}
	else {
		Log::log("err", "recv hdr: $@\n");
	}
	# and the body
	if($len > 0) {
		if(defined($s->recv($data, $len, 0))) {
			$$mr{$sn}{data} =  $data;
		}
		else {
			Log::log("err", "recv body: $@\n");
		}
	}
	Aux::print_dbg_api("received %s from %s:%s\n", $msg_action{$$mr{$sn}{hdr}{action}},
		$s->peerhost(), $s->peerport()
	);
	dump_data($$mr{$sn});
	return $sn;
}

sub process_msg($$$) {
	my ($s, $mr, $csr) = @_;
	if($$mr{hdr}{type} == TERCE_TOPO_SYNC) {
		if($$mr{hdr}{action} == ACT_INIT) {
			# open the control channel
			eval{
				$$csr = open_ctrl_channel($s, $$mr{hdr}{tag2});
			};
			if($@) {
				die "ACT_INIT: $@\n";
			}
			# ack the message
			eval{
				ack_msg($s, $mr);
			};
			if($@) {
				die "ACK ACT_INIT: $@\n";
			}
		}
	}
	#nuke the message
	$mr = {};
}

1;
