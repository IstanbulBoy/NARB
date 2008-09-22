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
# File: External.pm
#

package WS::External;

use strict;
use warnings;
use Socket;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.6 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

my $ip_to_name = {};
my $ip_to_urn = {};

sub add($$$$) {
	my($rtr_id, $rtr_name, $link_id, $port_name) = @_;
	return if !(defined($rtr_id) && defined($rtr_name)); 
	if(!exists($$ip_to_name{$rtr_id})) {
		$$ip_to_name{$rtr_id} = {};
	}
	$$ip_to_name{$rtr_id}{$link_id} = {};
	$$ip_to_name{$rtr_id}{name} = uc($rtr_name);
	$$ip_to_name{$rtr_id}{$link_id}{name} = uc($port_name);
}

sub flush () {
	$ip_to_name = {};
}

sub get_rtr_name($) {
	my($rtr_id) = @_;
	my $idIP = inet_ntoa(pack("N", $rtr_id));
	return undef if(!exists($$ip_to_name{$idIP}));
	return $$ip_to_name{$idIP}{name};
}

sub get_port_name($$) {
	my($rtr_id, $link_id) = @_;
	my $idIP = inet_ntoa(pack("N", $rtr_id));
	my $lidIP = inet_ntoa(pack("N", $link_id));
	return undef if(!exists($$ip_to_name{$idIP}));
	return undef if(!exists($$ip_to_name{$idIP}{$lidIP}));
	return $$ip_to_name{$idIP}{$lidIP}{name};
}

sub get_urn($;$) {
}

1;
