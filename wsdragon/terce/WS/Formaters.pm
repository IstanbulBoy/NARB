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
# June 4, 2008
#
# File: Formaters.pm
#

package WS::Formaters;

use GMPLS::Constants;
use WS::Constants;
use IO::Socket::INET;

use strict;
use warnings;


####################################################################
##########################  XML Formaters ##########################
####################################################################

sub get_swcap_xml($) {
	my ($lr) = @_;
	my $enc_xml = SOAP::Data->name('encodingType' => $link_swcap_enc_xml{$$lr{sw_cap}{enc}});
	my $swtype_xml = SOAP::Data->name('switchingcapType' => $link_swcap_cap_xml{$$lr{sw_cap}{cap}});
	my $info_xml = undef;
	my @swcap = ();
	if($$lr{sw_cap}{enc} == LINK_IFSWCAP_SUBTLV_ENC_ETH) {
		if(exists($$lr{sw_cap}{vtags})) {
			my $vtags = "";
			my @vtags_xml = ();
			for(my $i=0; $i<@{$$lr{sw_cap}{vtags}}; $i++) {
				$vtags .= $$lr{sw_cap}{vtags}[$i];
				if($i<(@{$$lr{sw_cap}{vtags}}-1)) {
					$vtags .= ",";
				}
			}
			push(@vtags_xml, SOAP::Data->name('vlanRangeAvailability' => $vtags));
			push(@vtags_xml, SOAP::Data->name('interfaceMTU' => "9000")->type("xsd:string"));
			$info_xml = SOAP::Data->name('switchingCapabilitySpecficInfo' => \SOAP::Data->value(@vtags_xml));
		}
	}
	else {
		$info_xml = SOAP::Data->name('switchingCapabilitySpecficInfo' => 
			\SOAP::Data->value(SOAP::Data->name('capability' => 'unimplemented')));
	}
	push(@swcap, $enc_xml, $swtype_xml);
	push(@swcap, $info_xml) if defined($info_xml);
	return SOAP::Data->name('SwitchingCapabilityDescriptors' => \SOAP::Data->value(@swcap));
}

sub get_nodes_xml($) {
	my ($db) = @_;
	my @nodes_xml = ();
	foreach my $rtr (keys %$db) {
		my $rn = WS::External::get_rtr_name($rtr);
		my $rtr_id = sprintf(TERCE_NODE_ID, defined($rn)?$rn:sprintf("%08x", $rtr));
		my @ports_xml = ();
		foreach my $link (keys %{$$db{$rtr}}) {
			# skip the top-level descriptors
			if(ref($$db{$rtr}{$link}) ne "HASH") {
				next;
			}
			#port
			my $pn = WS::External::get_port_name($rtr, $link);
			$pn = exists($$db{$rtr}{$link}{remote})?
			("DTL".(defined($pn)?$pn:sprintf("%08x", $link))):
			("S".(defined($pn)?$pn:sprintf("%08x", $link)));
			#check for uni info
			if(exists($$db{$rtr}{$link}{sw_cap}{uni})) {
				if(defined($$db{$rtr}{$link}{sw_cap}{uni}{id})) {
					$pn = "S".sprintf("%d", ($$db{$rtr}{$link}{sw_cap}{uni}{id}<<8) | 255);
				}
			}
			my $p_id = sprintf(TERCE_PORT_ID, $pn);
			my $fqp_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rtr_id.":".$p_id;
			#link
			my $l_id = sprintf(TERCE_LINK_ID, inet_ntoa(pack("N", $$db{$rtr}{$link}{local})));
			my $fql_id = TERCE_ID_PREF.":".TERCE_DOMAIN_ID.":".$rtr_id.":".$p_id.":".$l_id;
			#swcap
			my $swcap_xml = get_swcap_xml($$db{$rtr}{$link});

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
	die SOAP::Fault->faultcode($_[0])
	->faultstring($_[1])
	->faultdetail(SOAP::Data->name($_[2])
		->value(\SOAP::Data->name('msg' => $_[3])));
}

sub generate_soap_resp($$) {
	my ($ws, $scope_m) = @_;
	my $xml = {};
	my $done = 0;

	if(($scope_m & SCOPE_ABS_M) && !defined($$ws{abstract_tedb})) {
		generate_soap_fault('Receiver', 
			'abstract topology not ready', 
			'TerceTedbFault', 
			'TERCE has not received all the necessary information from narb/rce to form the response');
	}
	elsif(($scope_m & SCOPE_CRL_M) && !defined($$ws{control_tedb})) {
		generate_soap_fault('Receiver', 
			'control topology not ready', 
			'TerceTedbFault', 
			'TERCE has not received all the necessary information from narb/rce to form the response');
	}
	elsif(($scope_m & SCOPE_DAT_M) && !defined($$ws{data_tedb})) {
		generate_soap_fault('Receiver', 
			'data topology not ready', 
			'TerceTedbFault', 
			'TERCE has not received all the necessary information from narb/rce to form the response');
	}
	else {
		my @dbs = ($$ws{abstract_tedb}, $$ws{control_tedb}, $$ws{data_tedb});
		for(my $i=0; $i<SCOPE_MAX; $i++) {
			if(defined($dbs[$i]) && ($scope_m & (1<<$i))) {
				my @nodes = ();
				# <node> array of
				push(@nodes, get_nodes_xml($dbs[$i]));
				# <idcID>
				my $idcID_xml = SOAP::Data->name('idcId' => TERCE_IDC_ID);
				# <domain>
				my $domain_xml = SOAP::Data->name('domain' => \SOAP::Data->value(@nodes));
				$domain_xml->attr({ ' id' => TERCE_ID_PREF.":".TERCE_DOMAIN_ID});

				# construct the document
				$xml = SOAP::Data->name('topology' => \SOAP::Data->value($idcID_xml, $domain_xml));
				$xml->attr({ ' id' => TERCE_TOPO_ID, xmlns => TERCE_TOPO_XMLNS });
				$$ws{xml} = $xml;
			}
		}
	}
}
1;
