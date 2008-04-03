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

package WS::API;

use strict;
use warnings;
use Aux;
use Log;
use Socket;
use GMPLS::Constants;
use subs qw(findPath selectNetworkTopology insertNetworkTopology);

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.2 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;
my ($tqin, $tqout) = (undef, undef);

sub findPath {
	shift;
	Aux::print_dbg_ws("findPath()\n");
}

sub selectNetworkTopology {
	shift;
	my($scope) = @_;
	if(!defined($scope)) {
	}
	Aux::print_dbg_ws("selectNetworkTopology($scope)\n");
	my @cmd = ({"cmd"=>TEDB_GET});
	Aux::send_to_tedb($tqin, @cmd);
	my $res = Aux::recv_from_tedb($tqout);
	if($res eq "timeout") {
		Log::log "warning", "selectNetworkTopology($scope) request timed out\n";
	}
	else {
		return $res;
	}
}

sub insertNetworkTopology {
	shift;
	Aux::print_dbg_ws("insertNetworkTopology()\n");
}

sub set_queues($$) {
	($tqin, $tqout) = @_;
}

1;
