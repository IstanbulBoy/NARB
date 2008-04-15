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
use Socket;
use GMPLS::Constants;
use WS::External;
use IO::Socket::INET;
use SOAP::Lite;
use SOAP::Transport::HTTP;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.14 $ =~ /(\d+)/g;
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


use constant CTRLP_NODE_T => "CtrlPlane:CtrlPlaneNodeContent";
use constant CTRLP_PORT_T => "CtrlPlane:CtrlPlanePortContent";
use constant CTRLP_LINK_T => "CtrlPlane:CtrlPlaneLinkContent";
use constant CTRLP_SWCAP_T => "CtrlPlane:CtrlPlaneSwcapContent";

use constant SCOPE_ALL => "all";
use constant SCOPE_ADJ => "adjacentDomains";
use constant SCOPE_DLT => "delta";
use constant SCOPE_NDS => "nodes";
use constant SCOPE_LNK => "internetworkLinks";


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

use constant MIN_RES_BW => 50000000;

my $srvr = undef;
my $lport = undef;
my $tq1 = undef;
my $tq2 = undef;

sub new {
	shift;
	($lport, $tq1, $tq2)  = @_;
	my $self = {
		tedb1 => undef,
		tedb2 => undef,
		xml => undef
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
	$self->generate_soap_fault('Receiver', 
		'method not implemented', 
		'TerceTedbFault', 
		'findPath is not implemented');
}

sub selectNetworkTopology {
	my $self = shift;
	my($scope) = @_;
	if(!defined($scope)) {
		$self->generate_soap_fault('Sender', 
			'no topology set specified', 
			'TerceTedbFault', 
			'<from> must be specified');
	}
	elsif(
		lc($scope) ne SCOPE_ALL && 
		lc($scope) ne SCOPE_ADJ && 
		lc($scope) ne SCOPE_DLT && 
		lc($scope) ne SCOPE_NDS && 
		lc($scope) ne SCOPE_LNK ) {
		$self->generate_soap_fault('Sender', 
			'unknown topology set', 
			'TerceTedbFault', 
			'$scope not defined in the service description');
	}
	elsif(
		lc($scope) eq SCOPE_ADJ ||
		lc($scope) eq SCOPE_DLT ||
		lc($scope) eq SCOPE_NDS ) {
		$self->generate_soap_fault('Receiver', 
			'set not implemented', 
			'TerceTedbFault', 
			'$scope is not implemented');
	}
	else {
		Aux::print_dbg_ws("selectNetworkTopology($scope)\n");
		$self->generate_soap_resp();
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

####################################################################
##########################  XML Formaters ##########################
####################################################################

sub get_swcap_xml($) {
	my $self = shift;
	my ($lr) = @_;
	my $enc_xml = SOAP::Data->name('encodingType' => $link_swcap_enc_xml{$$lr{sw_cap}{enc}});
	my $swtype_xml = SOAP::Data->name('switchingcapType' => $link_swcap_cap_xml{$$lr{sw_cap}{cap}});
	my $info_xml;
	if($$lr{sw_cap}{enc} == LINK_IFSWCAP_SUBTLV_ENC_ETH) {
		my @vtags_xml = ();
		for(my $i=0; $i<@{$$lr{sw_cap}{vtags}}; $i++) {
			push(@vtags_xml, SOAP::Data->name('vlanRangeAvailability' => ${$$lr{sw_cap}{vtags}}[$i]));
		}
		push(@vtags_xml, SOAP::Data->name('interfaceMTU' => "9000")->type("xsd:string"));
		$info_xml = SOAP::Data->name('switchingCapabilitySpecficInfo' => \SOAP::Data->value(@vtags_xml));
	}
	else {
		$info_xml = SOAP::Data->name('switchingCapabilitySpecficInfo' => 
			\SOAP::Data->value(SOAP::Data->name('capability' => 'unimplemented')));
	}
	return SOAP::Data->name('SwitchingCapabilityDescriptors' => \SOAP::Data->value($enc_xml, $swtype_xml, $info_xml));
}

sub get_nodes_xml($) {
	my $self = shift;
	my ($db) = @_;
	my @nodes_xml = ();
	foreach my $rtr (keys %$db) {
		my $rn = WS::External::get_rtr_name($rtr);
		my $rtr_id = sprintf(TERCE_NODE_ID, defined($rn)?$rn:sprintf("%08x", $rtr));
		my @ports_xml = ();
		foreach my $link (keys %{$$db{$rtr}}) {
			#port
			my $pn = WS::External::get_port_name($rtr, $link);
			$pn = exists($$db{$rtr}{$link}{remote})?
			("DTL".(defined($pn)?$pn:sprintf("%08x", $link))):
			("S".(defined($pn)?$pn:sprintf("%08x", $link)));
			my $p_id = sprintf(TERCE_PORT_ID, $pn);
			my $fqp_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rtr_id.":".$p_id;
			#link
			my $l_id = sprintf(TERCE_LINK_ID, inet_ntoa(pack("N", $$db{$rtr}{$link}{local})));
			my $fql_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rtr_id.":".$p_id.":".$l_id;
			#swcap
			my $swcap_xml = $self->get_swcap_xml($$db{$rtr}{$link});

			#the rest
			my $cap_xml = SOAP::Data->name('capacity' => $$db{$rtr}{$link}{capacity})->type("xsd:string");
			my $maxrb_xml = SOAP::Data->name('maximumReservableCapacity' => $$db{$rtr}{$link}{max_bw})->type("xsd:string");
			my $minrb_xml = SOAP::Data->name('minimumReservableCapacity' => MIN_RES_BW)->type("xsd:string");
			my $gran_xml = SOAP::Data->name('granularity' => MIN_RES_BW)->type("xsd:string");
			my $met_xml = SOAP::Data->name('trafficEngineeringMetric' => $$db{$rtr}{$link}{metric})->type("xsd:string");
			my $rmt_xml;
			if(exists($$db{$rtr}{$link}{remote})) {
				my $rlink = $$db{$rtr}{$link}{remote};
				my $rrn = WS::External::get_rtr_name($$rlink{rtr_id});
				my $rrtr_id = sprintf(TERCE_NODE_ID, defined($rrn)?$rrn:sprintf("%08x", $$rlink{rtr_id}));
				my $rpn = WS::External::get_port_name($$rlink{rtr_id}, $$rlink{link_id});
				$rpn = "DTL".(defined($rpn)?$rpn:sprintf("%08x", $$rlink{link_id}));
				my $rp_id = sprintf(TERCE_PORT_ID, $rpn);
				my $fqp_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rrtr_id.":".$rp_id;
				#link
				my $rl_id = sprintf(TERCE_LINK_ID, inet_ntoa(pack("N", $$rlink{local})));
				my $rfql_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rrtr_id.":".$rp_id.":".$rl_id;
				$rmt_xml = SOAP::Data->name('remoteLinkId' => $rfql_id)->type("xsd:string");
			}
			else {
				$rmt_xml = SOAP::Data->name('remoteLinkId' => 'urn:ogf:network:domain=*:node=*:port=*:link=*')->type("xsd:string");
			}

			my $link_xml = SOAP::Data->name('link' => \SOAP::Data->value($swcap_xml, $cap_xml, $maxrb_xml, $minrb_xml, $gran_xml, $met_xml, $rmt_xml))->type(CTRLP_LINK_T);
			$link_xml->attr({ ' id' => $fql_id});
			$cap_xml = SOAP::Data->name('capacity' => $$db{$rtr}{$link}{capacity})->type("xsd:string");
			$maxrb_xml = SOAP::Data->name('maximumReservableCapacity' => $$db{$rtr}{$link}{max_bw})->type("xsd:string");
			$minrb_xml = SOAP::Data->name('minimumReservableCapacity' => MIN_RES_BW)->type("xsd:string");
			$gran_xml = SOAP::Data->name('granularity' => MIN_RES_BW)->type("xsd:string");
			my $port_xml = SOAP::Data->name('port' => \SOAP::Data->value($link_xml, $cap_xml, $maxrb_xml, $minrb_xml, $gran_xml))->type(CTRLP_PORT_T);
			$port_xml->attr({ ' id' => $fqp_id});
			push(@ports_xml, $port_xml);
		}
		my $id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rtr_id;
		my $addr_xml = SOAP::Data->name('address' => inet_ntoa(pack("N", $rtr)));
		my $node_xml = SOAP::Data->name('node' => \SOAP::Data->value($addr_xml, @ports_xml))->type(CTRLP_NODE_T);
		$node_xml->attr({ ' id' => $id});
		push(@nodes_xml, $node_xml);
	}
	return @nodes_xml;
}

sub generate_soap_fault($$$$) {
	my $self = shift;
	die SOAP::Fault->faultcode($_[0])
	->faultstring($_[1])
	->faultdetail(SOAP::Data->name($_[2])
		->value(\SOAP::Data->name('msg' => $_[3])));
}

sub generate_soap_resp() {
	my $self = shift;
	my $xml = {};
	my $done = 0;

	if(!defined($$self{tedb1}) && !defined($$self{tedb2})) {
		$self->generate_soap_fault('Receiver', 
			'topology not ready', 
			'TerceTedbFault', 
			'TERCE has not received all the necessary information from narb/rce to form the response');
	}
	else {
		my @nodes = ();
		foreach my $db ($$self{tedb1}, $$self{tedb2}) {
			if(defined($db)) {
				# <node> array of
				push(@nodes, $self->get_nodes_xml($db));
			}
		}
		# <idcID>
		my $idcID_xml = SOAP::Data->name('idcId' => TERCE_IDC_ID);
		# <domain>
		my $domain_xml = SOAP::Data->name('domain' => \SOAP::Data->value($idcID_xml, @nodes));
		$domain_xml->attr({ ' id' => TERCE_ID_PREF.":".TERCE_DOMAIN_ID});

		# construct the document
		$xml = SOAP::Data->name('topology' => \SOAP::Data->value($domain_xml));
		$xml->attr({ ' id' => TERCE_TOPO_ID, xmlns => TERCE_TOPO_XMLNS });
		$$self{xml} = $xml;
	}
}

####################################################################

sub find_remote($$) {
	my $self = shift;
	my($dr, $ip) = @_;
	foreach my $rtr (keys %$dr) {
		foreach my $link (keys %{$$dr{$rtr}}) {
			#replace the id with the actual ref to the remote link block
			if($$dr{$rtr}{$link}{local} eq $ip) {
				return $$dr{$rtr}{$link};
			}
		}
	}
	return undef;
}

sub create_graph($) {
	my $self = shift;
	my($dr) = @_;
	foreach my $rtr (keys %$dr) {
		foreach my $link (keys %{$$dr{$rtr}}) {
			#replace the id with the actual ref to the remote link block
			if(exists($$dr{$rtr}{$link}{remote})) {
				$$dr{$rtr}{$link}{remote} = $self->find_remote($dr, $$dr{$rtr}{$link}{remote});
				if(!defined($$dr{$rtr}{$link}{remote})) {
					delete($$dr{$rtr}{$link}{remote});
				}
			}
		}
	}
}


#
# inserts the temporary link block into the temporary tedb (if complete)
#
sub insert_link_blk($$$) {
	my $self = shift;
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
	my $self = shift;
	my($d, $dr, $br, $tr, $sr) = @_;
	my @data = @{$$d{data}};
	#add a valid OSPF-TE talker to TEDB
	# (these will serve as validation for the sub-tlv insertions)
	if($$d{cmd} == TEDB_RTR_ON) {
		$$dr{$data[0]} = undef;
		Aux::print_dbg_tedb("top level rtr insert: 0x%08x\n", ${$$d{data}}[0]);
		return 0;
	}
	#link subtlv delimiter
	elsif($$d{cmd} == TEDB_LINK_MARK) {
		if(exists($$dr{$$d{rtr}})) {
			if(defined($$br)) {
				# insert the old link block to the dr
				my $err = $self->insert_link_blk($dr, $br, $tr) if defined($$tr);
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
						${$$tr}{rtr_id} = $$br; # parent router ID
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
						${$$tr}{sw_cap}{vtags} = [];
						for(my $i = 14; $i<@data; $i++) {
							$v .= sprintf("\n      (%s)", $data[$i]) if Aux::dbg_tedb();
							push(@{${$$tr}{sw_cap}{vtags}}, $data[$i]);
						}
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
		my $err = $self->insert_link_blk($dr, $br, $tr) if defined($$tr);
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
			my $res = $self->process_q($d, $tedb1, \$lblock1, \$tmp1, \$stat1);
			if($res == 1) {
				if($stat1 == 0) {
					# create net graph
					$self->create_graph($tedb1);
					# transfer the completed TEDB
					Aux::print_dbg_tedb("updating TEDB1\n");
					$$self{tedb1} = $tedb1;
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
			my $res = $self->process_q($d, $tedb2, \$lblock2, \$tmp2, \$stat2);
			if($res == 1) {
				if($stat2 == 0) {
					# create net graph
					$self->create_graph($tedb2);
					# transfer the completed TEDB
					Aux::print_dbg_tedb("updating TEDB2\n");
					$$self{tedb2} = $tedb2;
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
