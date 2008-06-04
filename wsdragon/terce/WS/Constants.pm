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
# File: Constants.pm
#

package WS::Constants;

use strict;
use warnings;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.1 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw( TERCE_TOPO_ID TERCE_TOPO_XMLNS TERCE_IDC_ID TERCE_ID_PREF TERCE_DOMAIN_ID TERCE_NODE_ID TERCE_PORT_ID TERCE_LINK_ID CTRLP_NODE_T CTRLP_PORT_T CTRLP_LINK_T CTRLP_SWCAP_T SCOPE_ALL SCOPE_ABS SCOPE_CRL SCOPE_DAT SCOPE_ABS_M SCOPE_CRL_M SCOPE_DAT_M SCOPE_MAX  MIN_RES_BW );
	%EXPORT_TAGS = ( );
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
use constant SCOPE_ABS => "abstract";
use constant SCOPE_CRL => "control";
use constant SCOPE_DAT => "data";

use constant SCOPE_ABS_M => (1<<0);
use constant SCOPE_CRL_M => (1<<1);
use constant SCOPE_DAT_M => (1<<2);
use constant SCOPE_MAX => 3;

use constant MIN_RES_BW => 50000000;

1;
