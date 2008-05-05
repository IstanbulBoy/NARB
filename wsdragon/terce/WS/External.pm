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

use threads;
use threads::shared;
use strict;
use warnings;
use Socket;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.5 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

my $defs = &share({});

sub add($$$$) {
	my($rtr_id, $rtr_name, $link_id, $port_name) = @_;
	return if !(defined($rtr_id) && defined($rtr_name)); 
	if(!exists($$defs{$rtr_id})) {
		$$defs{$rtr_id} = &share({});
	}
	$$defs{$rtr_id}{$link_id} = &share({});
	$$defs{$rtr_id}{name} = uc($rtr_name);
	$$defs{$rtr_id}{$link_id}{name} = uc($port_name);
}

sub flush () {
	$defs = {};
}

sub get_rtr_name($) {
	my($rtr_id) = @_;
	my $idIP = inet_ntoa(pack("N", $rtr_id));
	return undef if(!exists($$defs{$idIP}));
	return $$defs{$idIP}{name};
}

sub get_port_name($$) {
	my($rtr_id, $link_id) = @_;
	my $idIP = inet_ntoa(pack("N", $rtr_id));
	my $lidIP = inet_ntoa(pack("N", $link_id));
	return undef if(!exists($$defs{$idIP}));
	return undef if(!exists($$defs{$idIP}{$lidIP}));
	return $$defs{$idIP}{$lidIP}{name};
}

1;
