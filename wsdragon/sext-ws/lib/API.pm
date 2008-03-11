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

%::msg_type = 	(	0x11 => "TERCE_TOPO_SYNC",
			0x12 => "TERCE_TOPO_ASYNC");

%::msg_action =	(	0x01 => "ACT_QUERY",
			0x02 => "ACT_INSERT", 
			0x03 => "ACT_DELETE",
			0x04 => "ACT_UPDATE",
			0x05 => "ACT_ACK",
			0x06 => "ACT_ACKDATA",
			0x07 => "ACT_ERROR",
			0x0A => "ACT_INIT",
			0x0B => "ACT_ALIVE");

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
				"chksum" => $chksum,
				"tag1" => $tag1,
				"tag2" => $tag2
			}
		};

	}
	else {
		Log::log("err", "recv hdr: $@\n");
	}
	# and the body
	if($len > 0) {
		if(defined($s->recv($data, $len, 0))) {
			$$mr{$sn} = {
				"data" => $data 
			};
		}
		else {
			Log::log("err", "recv body: $@\n");
		}
	}
	if(Aux::dbg_api()) {
		Aux::print_dbg_api("----------------------- header -----------------------\n");
		Aux::print_dbg_api("type action length:\t0x%02X (%u)  0x%02X (%u)  0x%04X (%u)\n", $$mr{$sn}{hdr}{type}, $$mr{$sn}{hdr}{type}, $$mr{$sn}{hdr}{action}, $$mr{$sn}{hdr}{action}, $$mr{$sn}{hdr}{length}, $$mr{$sn}{hdr}{length});
		Aux::print_dbg_api("ucid:\t\t\t0x%08X (%u)\n", $$mr{$sn}{hdr}{ucid}, $$mr{$sn}{hdr}{ucid});
		Aux::print_dbg_api("seq. number:\t\t0x%08X (%u)\n", $sn, $sn);
		Aux::print_dbg_api("checksum:\t\t0x%08X (%u)\n", $$mr{$sn}{hdr}{chksum}, $$mr{$sn}{hdr}{chksum});
		Aux::print_dbg_api("tag1:\t\t\t0x%08X (%u)\n", $$mr{$sn}{hdr}{tag1}, $$mr{$sn}{hdr}{tag1});
		Aux::print_dbg_api("tag2:\t\t\t0x%08X (%u)\n", $$mr{$sn}{hdr}{tag2}, $$mr{$sn}{hdr}{tag2});
		Aux::print_dbg_api("------------------------ data ------------------------\n");
		if(defined($data)) {
			my @tmp = unpack("C*", $$mr{$sn}{data});
			for(my $i=0; $i<$len; $i++) {
				Aux::print_dbg_api("%02X ", $tmp[$i]);
				if(($i+1)%16 && (($i+1)<$len)) {
					Aux::print_dbg_api("\n");
				}
			}
		}
		Aux::print_dbg_api("------------------------------------------------------\n");
	}
}

1;
