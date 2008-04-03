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
# File: TEDB.pm
#

package GMPLS::TEDB;

use threads;
use threads::shared;
use strict;
use warnings;
use Aux;
use GMPLS::Constants;
use IO::Select;
use SOAP::Lite;

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

my $tqin;
my $tqout;

sub new {
	shift;
	($tqin, $tqout)  = @_;
	my $self = {
	      _tedb => {},
	};
	bless $self;
	return $self;
}

sub run() {
	my $d;
	my $self = shift;
	while(!$::ctrlC) {
		$d = $tqin->dequeue_nb();
		if(defined($d)) {
			#add a valid OSPF-TE talker to TEDB
			# (these will serve as validation for the sub-tlv insertions)
			if($$d{cmd} == TEDB_RTR_ON) {
				$$self{_tedb}{${$$d{data}}[0]}{id} = ${$$d{data}}[0];
				Aux::print_dbg_tedb("top level rtr insert: 0x%08x\n", ${$$d{data}}[0]);
			}
			elsif($$d{cmd} == TEDB_INSERT) {
				Aux::print_dbg_tedb("    sub-level insert: %s\n", $sub_tlvs_link_X{$$d{type}});
			}
			elsif($$d{cmd} == TEDB_GET) {
				Aux::print_dbg_tedb("TEDB_GET request\n");
				$tqout->enqueue();
			}
		}
	}
	Aux::print_dbg_run("exiting TEDB thread\n");
}

1;
