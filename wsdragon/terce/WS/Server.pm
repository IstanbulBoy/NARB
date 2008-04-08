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
	$VERSION = sprintf "%d.%03d", q$Revision: 1.9 $ =~ /(\d+)/g;
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

use constant STAT_LINK_ID => 		(1<<0);
use constant STAT_LINK_TYPE => 		(1<<1);
use constant STAT_LINK_LOCAL => 	(1<<2);
use constant STAT_LINK_REMOTE =>	(1<<3);
use constant STAT_LINK_METRIC => 	(1<<4);
use constant STAT_LINK_CAPACITY => 	(1<<5);
use constant STAT_LINK_MAX_BW => 	(1<<6);
use constant STAT_LINK_SWCAP => 	(1<<7);

use constant STAT_LINK_COMPLETE => 	(
	STAT_LINK_ID | 
	STAT_LINK_LOCAL | 
	STAT_LINK_METRIC | 
	STAT_LINK_CAPACITY | 
	STAT_LINK_MAX_BW | 
	STAT_LINK_SWCAP);

my $srvr = undef;
my $lport = undef;
my $tq1 = undef;
my $tq2 = undef;

sub new {
	shift;
	($lport, $tq1, $tq2)  = @_;
	my $self = {
		_tedb1 => undef,
		_tedb2 => undef,
		_xml => undef
	};
	bless $self;
	$srvr = new SOAP::Transport::HTTP::Daemon(
		LocalAddr => inet_ntoa(INADDR_ANY),
		LocalPort => $lport,
		ReuseAddr => 1,
		Blocking => 0
		)->dispatch_to($self);
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
	generate_soap_resp();
	return $$self{_xml}; 
}

sub insertNetworkTopology {
	my $self = shift;
	Aux::print_dbg_ws("insertNetworkTopology()\n");
}

####################################################################

sub generate_soap_resp() {
	my $self = shift;
	my $xml = {};

	if(!defined($$self{_tedb})) {
		die SOAP::Fault->faultcode('Receiver') 
		->faultstring('topology not ready')
		->faultdetail(SOAP::Data->name('TerceTedbFault')
			->value(\SOAP::Data->name('msg' => 'TERCE has not received all the necesary information from narb/rce to form the response')));
	}
	else {
		$xml = SOAP::Data->name('topology' => undef);
		$xml->attr({ id => TERCE_TOPO_ID,
				xmlns => TERCE_TOPO_XMLNS });
		my $xml_idcID = SOAP::Data->name('idcId' => TERCE_IDC_ID);
		my $xml_domain = SOAP::Data->name('domain' => undef);
		$xml_domain->attr({ id => TERCE_ID_PREF.":".TERCE_DOMAIN_ID});

		my @topo_a = ($xml_idcID, $xml_domain);

		$xml->name('topology')->value(\@topo_a);
	}
	$$self{_xml} = $xml;
}

#
# inserts the temporary link block into the temporary tedb (if complete)
#
sub insert_link_blk($$$) {
	my($dr, $br, $hr) = @_;
	if(exists($$dr{$$br})) {
		# check if we have a link_id
		if(exists(${$$hr}{status})) {
			if((${$$hr}{status} & STAT_LINK_COMPLETE) == STAT_LINK_COMPLETE) {
				$$dr{$$br}{${$$hr}{link_id}} = $$hr;
			}
			else {
				Log::log "warning", "incomplete link block\n";
				Log::log "warning", "... entire link tlv discarded\n";
				return -1;
			}
		}
		else {
			Log::log "warning", "corrupted link block\n";
			Log::log "warning", "... entire link tlv discarded\n";
			return -1;
		}
		return 0;
	}
}

#
# $d: data received via the queue
# NOTE: Some of the following variables are references to variables holding references. 
#       To access the hash elements they must be dereferenced twice.
# $dr: reference to a reference to a temporary tedb
# $br: reference to a variable which holds a link block ID
# $tr: reference to a variable which holds a reference to a temporary anonymous hash
#      which is used to store the link block while waiting for the complete data set
# $sr: reference to a variable which holds the parser status
#
sub process_q($$$$$) {
	my($d, $dr, $br, $tr, $sr) = @_;
	my @data = @{$$d{data}};
	#add a valid OSPF-TE talker to TEDB
	# (these will serve as validation for the sub-tlv insertions)
	if($$d{cmd} == TEDB_RTR_ON) {
		$$dr{$data[0]}{rtr_id} = $data[0];
		Aux::print_dbg_tedb("top level rtr insert: 0x%08x\n", ${$$d{data}}[0]);
		return 0;
	}
	#link subtlv delimiter
	elsif($$d{cmd} == TEDB_LINK_MARK) {
		if(exists($$dr{$$d{rtr}})) {
			if(defined($$br)) {
				# insert the old link block to the dr
				my $err = insert_link_blk($dr, $br, $tr) if defined($$tr);
				# and start a new one
				$$tr = {"status" => 0};
				$$br = $$d{rtr};
				Aux::print_dbg_tedb("  }\n");
				if(!$err) {
					Aux::print_dbg_tedb("  => inserted to %08x\n", $$br);
				}
				$$sr = $err;
			}
			# first run of the current TEDB batch
			else {
				$$tr = {"status" => 0};
				$$br = $$d{rtr};
			}
			$$br = $$d{rtr};
			Aux::print_dbg_tedb("  LINK BLOCK {\n");
		}
		else {
			# reset link tlv delimiters
			$$tr = undef;
			$$br = undef;
			Log::log "warning", "receiving link subtlvs from unknown advertiser: $$d{rtr}\n";
			Log::log "warning", "... sub-tlv discarded\n";
		}
		return 0;
	}
	elsif($$d{cmd} == TEDB_INSERT) {
		# do something only if we saw the top level link tlv
		if(defined($$br)) {
			if(exists($$dr{$$d{rtr}})) {
				my $v = "";
				if(defined($$tr)) {
					if($$d{type} == TE_LINK_SUBTLV_LINK_ID) {
						${$$tr}{status} |= STAT_LINK_ID; 
						${$$tr}{link_id} = $data[0];
						$v = sprintf(" (%08x)", $data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_LINK_TYPE) {
						${$$tr}{status} |= STAT_LINK_TYPE; 
						${$$tr}{type} = $data[0];
						$v = sprintf(" (%s)", $sub_tlvs_link_type_X{$data[0]}) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_LCLIF_IPADDR) {
						${$$tr}{status} |= STAT_LINK_LOCAL; 
						${$$tr}{local} = $data[0];
						$v = sprintf(" (%08x)", $data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_RMTIF_IPADDR) {
						${$$tr}{status} |= STAT_LINK_REMOTE; 
						${$$tr}{remote} = $data[0];
						$v = sprintf(" (%08x)", $data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_TE_METRIC) {
						${$$tr}{status} |= STAT_LINK_METRIC; 
						${$$tr}{metric} = $data[0];
						$v = sprintf(" (%d)", $data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_MAX_BW) {
						${$$tr}{status} |= STAT_LINK_CAPACITY; 
						${$$tr}{capacity} = $data[0];
						$v = sprintf(" (%f)", 8*$data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_MAX_RSV_BW) {
						${$$tr}{status} |= STAT_LINK_MAX_BW; 
						${$$tr}{max_bw} = $data[0];
						$v = sprintf(" (%f)", 8*$data[0]) if Aux::dbg_tedb();
					}
					elsif($$d{type} == TE_LINK_SUBTLV_LINK_IFSWCAP) {
						${$$tr}{sw_cap}{cap} = $data[0];
						${$$tr}{sw_cap}{enc} = $data[1];
						$v = sprintf("\n      (%s, %s)", 
							$sub_tlvs_link_swcap_cap{$data[0]},
							$sub_tlvs_link_swcap_enc{$data[1]}) if Aux::dbg_tedb();
						${$$tr}{status} |= STAT_LINK_SWCAP; 
					}
					Aux::print_dbg_tedb("    sub-level insert: %s%s\n", $sub_tlvs_link_X{$$d{type}}, $v);
				}
				else {
					$$br = undef;
					Log::log "warning", "malformed link block\n";
					Log::log "warning", "... entire link tlv discarded\n";
				}
			}
			else {
				# reset link tlv delimiters
				$$tr = undef;
				$$br = undef;
				Log::log "warning", "attempting to insert sub-tlv from unknown advertiser: $$d{rtr}\n";
				Log::log "warning", "... sub-tlv discarded\n";
			}
		}
		return 0;
	}
	elsif($$d{cmd} == TEDB_ACTIVATE) {
		# insert the last link block to the tedb
		my $err = insert_link_blk($dr, $br, $tr) if defined($$tr);
		$$tr = {"status" => 0};
		Aux::print_dbg_tedb("  }\n");
		if(!$err) {
			Aux::print_dbg_tedb("  => inserted to %08x\n", $$br);
		}
		$$br = undef; 
		return 1;
	}
}

sub run() {
	my $self = shift;
	my $d = undef;
	my $tedb1 = {};
	my $tedb2 = {};
	my $lblock1 = undef;
	my $lblock2 = undef;
	my $tmp1 = undef; #temporary storage for the link block data
	my $tmp2 = undef; #temporary storage for the link block data
	my $stat1 = 0;
	my $stat2 = 0;
	while(!$::ctrlC) {
		# WS server
		$srvr->handle;

		# rce or narb TEDB queue
		$d = $tq1->dequeue_nb();
		if(defined($d)) {
			my $res = process_q($d, $tedb1, \$lblock1, \$tmp1, \$stat1);
			if($res == 1) {
				if($stat1 == 0) {
					# transfer the completed TEDB
					Aux::print_dbg_tedb("updating TEDB1\n");
					$$self{_tedb1} = $tedb1;
				}
				else {
					Aux::print_dbg_tedb("TEDB1 not updated\n");
				}
				$tedb1 = {};
				$stat1 = 0;
			}
		}
		# rce or narb TEDB queue
		$d = $tq2->dequeue_nb();
		if(defined($d)) {
			my $res = process_q($d, $tedb2, \$lblock2, \$tmp2, \$stat2);
			if($res == 1) {
				if($stat2 == 0) {
					# transfer the completed TEDB
					Aux::print_dbg_tedb("updating TEDB2\n");
					$$self{_tedb2} = $tedb2;
				}
				else {
					Aux::print_dbg_tedb("TEDB2 not updated\n");
				}
				$tedb2 = {};
				$stat2 = 0;
			}
		}
	}
	$srvr->shutdown(SHUT_RDWR);
	Aux::print_dbg_run("exiting web services thread\n");
}

1;
