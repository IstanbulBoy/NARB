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
# File: Constants.pm
#

package GMPLS::Constants;

use strict;
use warnings;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.5 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw( TEDB_RTR_ON TEDB_INSERT TEDB_UPDATE TEDB_DELETE TEDB_ACTIVATE TEDB_LINK_MARK TERCE_TOPO_SYNC TERCE_TOPO_ASYNC ACT_NOP ACT_QUERY ACT_INSERT ACT_DELETE ACT_UPDATE ACT_ACK ACT_ACKDATA ACT_ERROR ACT_INIT ACT_ALIVE OSPF_ROUTER_LSA OSPF_NETWORK_LSA OSPF_SUMMARY_LSA OSPF_ASBR_SUMMARY_LSA OSPF_AS_EXTERNAL_LSA OSPF_AS_NSSA_LSA OSPF_OPAQUE_LINK_LSA OSPF_OPAQUE_AREA_LSA OSPF_OPAQUE_AS_LSA NOT_TE_LSA ROUTER_ID_TE_LSA LINK_TE_LSA LINK_LOCAL_TE_LSA OPAQUE_TYPE_TE_AREA_LSA OPAQUE_TYPE_SYCAMORE_OPTICAL_TOPOLOGY_DESC OPAQUE_TYPE_GRACE_LSA OPAQUE_TYPE_RTR_INFO_LSA TE_TLV_ROUTER_ADDR TE_TLV_LINK TE_LINK_SUBTLV_LINK_TYPE TE_LINK_SUBTLV_LINK_ID TE_LINK_SUBTLV_LCLIF_IPADDR TE_LINK_SUBTLV_RMTIF_IPADDR TE_LINK_SUBTLV_TE_METRIC TE_LINK_SUBTLV_MAX_BW TE_LINK_SUBTLV_MAX_RSV_BW TE_LINK_SUBTLV_UNRSV_BW TE_LINK_SUBTLV_RSC_CLSCLR TE_LINK_SUBTLV_LINK_LCRMT_ID TE_LINK_SUBTLV_LINK_IFSWCAP DRAGON_TLV_TYPE_BASE TE_LINK_SUBTLV_RESV_SCHEDULE TE_LINK_SUBTLV_LINK_IFADCAP TE_LINK_SUBTLV_DOMAIN_ID LINK_TYPE_SUBTLV_VALUE_PTP LINK_TYPE_SUBTLV_VALUE_MA LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 LINK_IFSWCAP_SUBTLV_SWCAP_PSC2 LINK_IFSWCAP_SUBTLV_SWCAP_PSC3 LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 LINK_IFSWCAP_SUBTLV_SWCAP_L2SC LINK_IFSWCAP_SUBTLV_SWCAP_TDM LINK_IFSWCAP_SUBTLV_SWCAP_LSC LINK_IFSWCAP_SUBTLV_SWCAP_FSC IFSWCAP_SPECIFIC_VLAN_BASIC IFSWCAP_SPECIFIC_VLAN_ALLOC IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z IFSWCAP_SPECIFIC_SUBNET_UNI LSA_HDR_SIZE TLV_HDR_SIZE TLV_ALIGN
%msg_type_X %msg_action_X %lsa_type_X %te_lsa_type_X %opaque_lsa_type_X %tlvs_X %sub_tlvs_link_X %sub_tlvs_link_type_X %sub_tlvs_link_swcap_cap %sub_tlvs_link_swcap_enc
);
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

use constant TEDB_RTR_ON => 1;
use constant TEDB_INSERT => 2;
use constant TEDB_UPDATE => 3;
use constant TEDB_DELETE => 4;
use constant TEDB_LINK_MARK => 5;
use constant TEDB_ACTIVATE => 6;

use constant TERCE_TOPO_SYNC => 0x11;
use constant TERCE_TOPO_ASYNC => 0x12;

use constant ACT_NOP => 0x00;
use constant ACT_QUERY => 0x01;
use constant ACT_INSERT => 0x02; 
use constant ACT_DELETE => 0x03;
use constant ACT_UPDATE => 0x04;
use constant ACT_ACK => 0x05;
use constant ACT_ACKDATA => 0x06;
use constant ACT_ERROR => 0x07;
use constant ACT_INIT => 0x0A;
use constant ACT_ALIVE => 0x0B;

# OSPF LSA Types
use constant OSPF_ROUTER_LSA => 1;
use constant OSPF_NETWORK_LSA => 2;
use constant OSPF_SUMMARY_LSA => 3;
use constant OSPF_ASBR_SUMMARY_LSA => 4;
use constant OSPF_AS_EXTERNAL_LSA => 5;
use constant OSPF_AS_NSSA_LSA => 7;
use constant OSPF_OPAQUE_LINK_LSA => 9;
use constant OSPF_OPAQUE_AREA_LSA => 10;
use constant OSPF_OPAQUE_AS_LSA => 11;

# TE LSA Types
use constant NOT_TE_LSA => 0;
use constant ROUTER_ID_TE_LSA => 1;
use constant LINK_TE_LSA => 2;
use constant LINK_LOCAL_TE_LSA => 3;

# Opaque LSA types
use constant OPAQUE_TYPE_TE_AREA_LSA => 1;
use constant OPAQUE_TYPE_SYCAMORE_OPTICAL_TOPOLOGY_DESC => 2;
use constant OPAQUE_TYPE_GRACE_LSA => 3;
use constant OPAQUE_TYPE_RTR_INFO_LSA => 4;

# TLVs
use constant TE_TLV_ROUTER_ADDR => 1;
use constant TE_TLV_LINK => 2;

# sub-TLVs
use constant TE_LINK_SUBTLV_LINK_TYPE => 1;
use constant TE_LINK_SUBTLV_LINK_ID => 2;
use constant TE_LINK_SUBTLV_LCLIF_IPADDR => 3; # Local Interface IP Address
use constant TE_LINK_SUBTLV_RMTIF_IPADDR => 4; # Remote Interface IP Address
use constant TE_LINK_SUBTLV_TE_METRIC => 5; # Traffic Engineering Metric
use constant TE_LINK_SUBTLV_MAX_BW => 6; # Maximum Bandwidth
use constant TE_LINK_SUBTLV_MAX_RSV_BW => 7; # Maximum Reservable Bandwidth
use constant TE_LINK_SUBTLV_UNRSV_BW => 8; # Unreserved Bandwidth
use constant TE_LINK_SUBTLV_RSC_CLSCLR => 9; # Administrative Group, a.k.a. Resource Class/Color
use constant TE_LINK_SUBTLV_LINK_LCRMT_ID => 11; # Link Local/Remote Identifiers
use constant TE_LINK_SUBTLV_LINK_IFSWCAP => 15;
use constant DRAGON_TLV_TYPE_BASE => 0x8800;
use constant TE_LINK_SUBTLV_RESV_SCHEDULE => DRAGON_TLV_TYPE_BASE + 1;
use constant TE_LINK_SUBTLV_LINK_IFADCAP => DRAGON_TLV_TYPE_BASE + 2;
use constant TE_LINK_SUBTLV_DOMAIN_ID => DRAGON_TLV_TYPE_BASE + 0x10;

# sub-TLV fields
use constant LINK_TYPE_SUBTLV_VALUE_PTP => 1;
use constant LINK_TYPE_SUBTLV_VALUE_MA => 2;

use constant LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 => 1;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_PSC2 => 2;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_PSC3 => 3;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 => 4;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_L2SC => 51;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_TDM => 100;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_LSC => 150;
use constant LINK_IFSWCAP_SUBTLV_SWCAP_FSC => 200;

use constant IFSWCAP_SPECIFIC_VLAN_BASIC => 0x0002;
use constant IFSWCAP_SPECIFIC_VLAN_ALLOC => 0x0004;
use constant IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z => 0x8000;
use constant IFSWCAP_SPECIFIC_SUBNET_UNI => 0x4000;

use constant LSA_HDR_SIZE => 20;
use constant TLV_HDR_SIZE => 4;
use constant TLV_ALIGN => 4;


# _X translators
our %msg_type_X = 	(
			0x11 => "TERCE_TOPO_SYNC",
			0x12 => "TERCE_TOPO_ASYNC");

our %msg_action_X =	(	
			0x00 => "ACT_NOP",
			0x01 => "ACT_QUERY",
			0x02 => "ACT_INSERT", 
			0x03 => "ACT_DELETE",
			0x04 => "ACT_UPDATE",
			0x05 => "ACT_ACK",
			0x06 => "ACT_ACKDATA",
			0x07 => "ACT_ERROR",
			0x0A => "ACT_INIT",
			0x0B => "ACT_ALIVE");

our %lsa_type_X = 	(
			1 => "OSPF_ROUTER_LSA",
			2 => "OSPF_NETWORK_LSA",
			3 => "OSPF_SUMMARY_LSA",
			4 => "OSPF_ASBR_SUMMARY_LSA",
			5 => "OSPF_AS_EXTERNAL_LSA",
			7 => "OSPF_AS_NSSA_LSA",
			9 => "OSPF_OPAQUE_LINK_LSA",
			10 => "OSPF_OPAQUE_AREA_LSA",
			11 => "OSPF_OPAQUE_AS_LSA");

our %te_lsa_type_X = 	(
			0 => "NOT_TE_LSA",
			1 => "ROUTER_ID_TE_LSA",
			2 => "LINK_TE_LSA",
			3 => "LINK_LOCAL_TE_LSA");
		
our %opaque_lsa_type_X =	(
			1 => "OPAQUE_TYPE_TE_AREA_LSA",
			2 => "OPAQUE_TYPE_SYCAMORE_OPTICAL_TOPOLOGY_DESC",
			3 => "OPAQUE_TYPE_GRACE_LSA",
			4 => "OPAQUE_TYPE_RTR_INFO_LSA");

our %tlvs_X = 		(
			1 => "TE_TLV_ROUTER_ADDR",
			2 => "TE_TLV_LINK");

our %sub_tlvs_link_X = 	(
			1 => "TE_LINK_SUBTLV_LINK_TYPE",
			2 => "TE_LINK_SUBTLV_LINK_ID",
			3 => "TE_LINK_SUBTLV_LCLIF_IPADDR", # Local Interface IP Address
			4 => "TE_LINK_SUBTLV_RMTIF_IPADDR", # Remote Interface IP Address
			5 => "TE_LINK_SUBTLV_TE_METRIC", # Traffic Engineering Metric
			6 => "TE_LINK_SUBTLV_MAX_BW", # Maximum Bandwidth
			7 => "TE_LINK_SUBTLV_MAX_RSV_BW", # Maximum Reservable Bandwidth
			8 => "TE_LINK_SUBTLV_UNRSV_BW", # Unreserved Bandwidth
			9 => "TE_LINK_SUBTLV_RSC_CLSCLR", # Administrative Group, a.k.a. Resource Class/Color
			11 => "TE_LINK_SUBTLV_LINK_LCRMT_ID", # Link Local/Remote Identifiers
			15 => "TE_LINK_SUBTLV_LINK_IFSWCAP",
			0x8801 => "TE_LINK_SUBTLV_RESV_SCHEDULE",
			0x8802 => "TE_LINK_SUBTLV_LINK_IFADCAP",
			0x8810 => "TE_LINK_SUBTLV_DOMAIN_ID");

our %sub_tlvs_link_type_X = 	(
			1 => "PTP",
			2 => "MA");

our %sub_tlvs_link_swcap_cap = 	(
			1 => "LINK_IFSWCAP_SUBTLV_SWCAP_PSC1",
			2 => "LINK_IFSWCAP_SUBTLV_SWCAP_PSC2",
			3 => "LINK_IFSWCAP_SUBTLV_SWCAP_PSC3",
			4 => "LINK_IFSWCAP_SUBTLV_SWCAP_PSC4",
			51 => "LINK_IFSWCAP_SUBTLV_SWCAP_L2SC",
			100 => "LINK_IFSWCAP_SUBTLV_SWCAP_TDM",
			150 => "LINK_IFSWCAP_SUBTLV_SWCAP_LSC",
			200 => "LINK_IFSWCAP_SUBTLV_SWCAP_FSC");

our %sub_tlvs_link_swcap_enc = 	(
			1 => "LINK_IFSWCAP_SUBTLV_ENC_PKT",
			2 => "LINK_IFSWCAP_SUBTLV_ENC_ETH",
			3 => "LINK_IFSWCAP_SUBTLV_ENC_PDH",
			4 => "LINK_IFSWCAP_SUBTLV_ENC_RESV1",
			5 => "LINK_IFSWCAP_SUBTLV_ENC_SONETSDH",
			6 => "LINK_IFSWCAP_SUBTLV_ENC_RESV2",
			7 => "LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP",
			8 => "LINK_IFSWCAP_SUBTLV_ENC_LAMBDA",
			9 => "LINK_IFSWCAP_SUBTLV_ENC_FIBER",
			10 => "LINK_IFSWCAP_SUBTLV_ENC_RESV3",
			11 => "LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL");


1;
