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

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.4 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter SOAP::Server::Parameters);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}

our @EXPORT_OK;

sub new {
	shift;
	my $ws = shift;
	my $self = {
		ws => $ws  #back ref to the actual server object
	};
	bless $self;
	return $self;
}

############################## WS API ##############################
sub findPath {
	my $self = shift;
	my @args = SOAP::Server::Parameters::byName ([qw(
		srcEndpoint 
		destEndpoint
		bandwidth
		encoding
		swcap
		gpid
		vtag
		)], @_);
	my $som = pop @_;
	my $attrs = $som->dataof('//findPath')->attr;
	my $attr = 0;

	Aux::print_dbg_ws("findPath()\n");
}

sub selectNetworkTopology {
	my $self = shift;
	my($scope) = @_;
	if(!defined($scope)) {
		WS::Formaters::generate_soap_fault('Sender', 
			'no topology set specified', 
			'TerceTedbFault', 
			'<scope> must be specified');
	}
	elsif(
		lc($scope) ne SCOPE_ALL && 
		lc($scope) ne SCOPE_ABS && 
		lc($scope) ne SCOPE_CRL && 
		lc($scope) ne SCOPE_DAT ) {
		WS::Formaters::generate_soap_fault('Sender', 
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
		WS::Formaters::generate_soap_resp($$self{ws}, $scope_m);
	}
	return $$self{ws}{xml}; 
}

sub insertNetworkTopology {
	my $self = shift;
	Aux::print_dbg_ws("insertNetworkTopology()\n");
	WS::Formaters::generate_soap_fault('Receiver', 
		'method not implemented', 
		'TerceTedbFault', 
		'insertNetworkTopology is not implemented');
}


1;
