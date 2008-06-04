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
# June 3, 2008
#
# File: Handlers.pm
#

package WS::Handlers;

use strict;
use warnings;
use WS::Formaters;
use WS::Constants;

############################## WS API ##############################
sub findPath {
	print(join(" ", @_));
	Aux::print_dbg_ws("findPath()\n");
}

sub selectNetworkTopology {
	my $self = shift;
	my($scope) = @_;
	if(!defined($scope)) {
		$self->generate_soap_fault('Sender', 
			'no topology set specified', 
			'TerceTedbFault', 
			'<scope> must be specified');
	}
	elsif(
		lc($scope) ne SCOPE_ALL && 
		lc($scope) ne SCOPE_ABS && 
		lc($scope) ne SCOPE_CRL && 
		lc($scope) ne SCOPE_DAT ) {
		$self->generate_soap_fault('Sender', 
			'unknown topology type', 
			'TerceTedbFault', 
			'$scope not defined in the service description');
	}
	else {
		my $scope_m = 0;
		$scope_m |= (lc($scope) eq "abstract")?SCOPE_ABS_M:0;
		$scope_m |= (lc($scope) eq "control")?SCOPE_CRL_M:0;
		$scope_m |= (lc($scope) eq "data")?SCOPE_DAT_M:0;
		$scope_m |= (lc($scope) eq "all")?(SCOPE_CRL_M | SCOPE_DAT_M | SCOPE_ABS_M):0;
		Aux::print_dbg_ws("selectNetworkTopology($scope)\n");
		$self->generate_soap_resp($scope_m);
	}
	return $$self{xml}; 
}

sub insertNetworkTopology {
	my $self = shift;
	Aux::print_dbg_ws("insertNetworkTopology()\n");
	$self->generate_soap_fault('Receiver', 
		'method not implemented', 
		'TerceTedbFault', 
		'insertNetworkTopology is not implemented');
}


1;
