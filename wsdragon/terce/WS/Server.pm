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
use GMPLS::Constants;
use IO::Socket::INET;
use SOAP::Lite;
use SOAP::Transport::HTTP;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.5 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

use constant TERCE_TOPO_ID => "dcn.internet2.edu-1202827532";
use constant TERCE_TOPO_XMLNS => "http://ogf.ogf/schema/network/topology/ctrlPlane/20071023";
use constant TERCE_IDC_ID => "https://ndb3-blmt.abilene.ucaid.edu:8443/axis2/services/OSCARS";
use constant TERCE_ID_PREF => "urn:ogf:network";
use constant TERCE_DOMAIN_ID => "domain=dcn.internet2.edu";
use constant TERCE_NODE_ID => "node=%s";
use constant TERCE_PORT_ID => "port=%s";
use constant TERCE_LINK_ID => "link=%s";

my $srvr = undef;
my $lport = undef;
my $tqin = undef;
my $tqout = undef;

sub new {
	shift;
	($lport, $tqin, $tqout)  = @_;
	my $self = {
	      _tedb => {},
	      _xml => SOAP::Data->name('topology' => undef)
	};
	bless $self;
	$srvr = new SOAP::Transport::HTTP::Daemon(
		LocalAddr => inet_ntoa(INADDR_ANY),
		LocalPort => $lport,
		ReuseAddr => 1,
		Blocking => 0
		)->dispatch_to($self);
	$$self{_xml}->attr({ id => TERCE_TOPO_ID,
			xmlns => TERCE_TOPO_XMLNS });
	my $xml_idcID = SOAP::Data->name('idcId' => TERCE_IDC_ID);
	my $xml_domain = SOAP::Data->name('domain' => undef);
	$xml_domain->attr({ id => TERCE_ID_PREF.":".TERCE_DOMAIN_ID});

	my @topo_a = ($xml_idcID, $xml_domain);

	$$self{_xml}->name('topology')->value(\@topo_a);
	return $self;
}

############################## WS API ##############################
sub findPath {
	my $self = shift;
	Aux::print_dbg_ws("findPath()\n");
}

sub selectNetworkTopology {
	my $self = shift;
	my($scope) = @_;
	if(!defined($scope)) {
	}
	Aux::print_dbg_ws("selectNetworkTopology($scope)\n");
	return $$self{_xml}; 
}

sub insertNetworkTopology {
	my $self = shift;
	Aux::print_dbg_ws("insertNetworkTopology()\n");
}

####################################################################

sub run() {
	my $self = shift;
	my $d = undef;
	while(!$::ctrlC) {
		# WS server
		$srvr->handle;

		# TEDB queue
		$d = $tqin->dequeue_nb();
		if(defined($d)) {
			#add a valid OSPF-TE talker to TEDB
			# (these will serve as validation for the sub-tlv insertions)
			if($$d{cmd} == TEDB_RTR_ON) {
				$$self{_tedb}{${$$d{data}}[0]}{id} = ${$$d{data}}[0];
				Aux::print_dbg_tedb("top level rtr insert: 0x%08x\n", ${$$d{data}}[0]);
			}
			elsif($$d{cmd} == TEDB_INSERT) {
				if(exists($$self{_tedb}{$$d{rtr}})) {
					Aux::print_dbg_tedb("    sub-level insert: %s\n", $sub_tlvs_link_X{$$d{type}});
				}
				else {
					Log::log "warning", "attempting to insert sub-tlv from unknown advertiser: $$d{rtr}\n";
					Log::log "warning", "... sub-tlv discarded\n";
				}
			}
		}
	}
	$srvr->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting web services thread\n");
}

1;
