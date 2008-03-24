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

package WS::Server;

use strict;
use warnings;
use Aux;
use SOAP::Transport::HTTP;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.1 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

my $srvr = undef;
my $lport = undef;

sub new {
	shift;
	($lport)  = @_;
	my $self = {};
	bless $self;
	$srvr = new SOAP::Transport::HTTP::Daemon(LocalAddr => 'localhost', LocalPort => 80)->dispatch_to('/Your/Path/To/Deployed/Modules', 'Module::Name', 'Module::method');
	return $self;
}

sub run() {
	while(!$::ctrlC) {
		threads->yield();
	}
	Aux::print_dbg_run("exiting web services thread\n");
}

1;
