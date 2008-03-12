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

package Server;

use strict;
use warnings;
use Aux;
use API;

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

my $ctrlC = 0;
my $name = undef;

sub new {
	shift;
	my $s = shift;
	my $self = {};
	bless $self;
	$self->initialize($s);
	return $self;
}

sub initialize($) {
	shift;
	my ($s) = @_;
	my %msg;
	my $sn = API::get_msg($s, \%msg);
	#guess who's calling ....
	if($msg{$sn}{hdr}{tag2} eq 2693) {
		$name = "narb";
	}
	elsif($msg{$sn}{hdr}{tag2} eq 2695) {
		$name = "rce";
	}
	else {
		$name = "unidentified";
	}
	Aux::print_dbg_net("started (%s) server thread\n", $name);
	
#	process_msg($s, \%msg);
}

sub term {
	$ctrlC = 1;
}

sub run() {
	while(!$ctrlC) {
		threads->yield();
	}
	Log::log("info", "exiting $name server thread");
}


1;
