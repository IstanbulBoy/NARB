/*
 * Copyright (c) 2007
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2007
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __RCE_LSA_H__
#define __RCE_LSA_H__

#include "rce_types.hh"
#include "resource.hh"
#include "rce_api.hh"
#include "event.hh"

using namespace std;

// OSPF LSA Types
#define OSPF_ROUTER_LSA                 1
#define OSPF_NETWORK_LSA              2
#define OSPF_SUMMARY_LSA              3
#define OSPF_ASBR_SUMMARY_LSA    4
#define OSPF_AS_EXTERNAL_LSA       5
#define OSPF_AS_NSSA_LSA	           7
#define OSPF_OPAQUE_LINK_LSA	    9
#define OSPF_OPAQUE_AREA_LSA	    10
#define OSPF_OPAQUE_AS_LSA	    11

// TE LSA Types
#define NOT_TE_LSA          	       0
#define ROUTER_ID_TE_LSA		1
#define LINK_TE_LSA			       2
#define LINK_LOCAL_TE_LSA		3

//
// Opaque LSA types will be assigned by IANA.
// <http://www.iana.org/assignments/ospf-opaque-types>
//
#define OPAQUE_TYPE_TE_LINKLOCAL_LSA	1
#define OPAQUE_TYPE_TE_AREA_LSA		1
#define OPAQUE_TYPE_SYCAMORE_OPTICAL_TOPOLOGY_DESC	2
#define OPAQUE_TYPE_GRACE_LSA				3

//
// Opaque TYPE and ID operators
//
#define LSID_OPAQUE_TYPE_MASK	0xff000000
#define LSID_OPAQUE_ID_MASK	0x00ffffff

#define	GET_OPAQUE_TYPE(lsid) (((u_int32_t)(lsid) & LSID_OPAQUE_TYPE_MASK) >> 24)

#define	GET_OPAQUE_ID(lsid) ((u_int32_t)(lsid) & LSID_OPAQUE_ID_MASK)

#define	SET_OPAQUE_LSID(type, id) \
	((((type) << 24) & LSID_OPAQUE_TYPE_MASK) \
	| ((id)          & LSID_OPAQUE_ID_MASK))



struct lsa_header
{
    u_int16_t ls_age;
    u_char options;
    u_char type;
    struct in_addr id;
    struct in_addr adv_router;
    int ls_seqnum;
    u_int16_t checksum;
    u_int16_t length;
};

#define OSPF_LSA_HEADER_SIZE sizeof(struct lsa_header)

//
// Following section defines TLV (tag, length, value) structures,
// used for Traffic Engineering.
//

struct te_tlv_header
{
  u_int16_t	type;	// TE_TLV Type
  u_int16_t	length;
};

#define TLV_HDR_SIZE \
	sizeof (struct te_tlv_header)

#define TLV_BODY_SIZE(tlvh) \
	(ntohs ((tlvh)->length) % 4 > 0 ? (ntohs ((tlvh)->length) / 4 + 1) * 4 : ntohs ((tlvh)->length))

#define TLV_SIZE(tlvh) \
	(TLV_HDR_SIZE + TLV_BODY_SIZE(tlvh))

#define TLV_HDR_TOP(lsah) \
	(struct te_tlv_header *)((char *)(lsah) + OSPF_LSA_HEADER_SIZE)

#define TLV_HDR_NEXT(tlvh) \
	(struct te_tlv_header *)((char *)(tlvh) + TLV_SIZE(tlvh))

#define SUBTLV_HDR_TOP(tlvh) \
	(struct te_tlv_header *)((char *)(tlvh) + TLV_HDR_SIZE)

#define SUBTLV_HDR_NEXT(sub_tlvh) \
	(struct te_tlv_header *)((char *)(sub_tlvh) + TLV_SIZE(sub_tlvh))


//
// Defining TE-LSA TLV's and SUB-TLV's (LSA Type 10, Opaque Type 1)
//

// Router Address TLV, Mandatory!
#define	TE_TLV_ROUTER_ADDR	1
struct te_tlv_router_addr
{
  struct te_tlv_header	header;
  struct in_addr	value;
};

// TE Link TLV
#define	TE_TLV_LINK			2
struct te_tlv_link
{
  struct te_tlv_header	header;

};

// Link Local Identifier TLV
#define	TE_TLV_LINK_LOCAL_ID	1
struct te_tlv_link_local_id
{
  struct te_tlv_header	header;
  u_int32_t	value;
};

// Link Type Sub-TLV, Mandatory!
#define	TE_LINK_SUBTLV_LINK_TYPE	1
struct te_link_subtlv_link_type
{
  struct te_tlv_header	header;
  struct {
#define	LINK_TYPE_SUBTLV_VALUE_PTP	1
#define	LINK_TYPE_SUBTLV_VALUE_MA	2
      u_char	value;
      u_char	padding[3];
  } link_type;
};

// Link Sub-TLV: Link ID, Mandatory
#define	TE_LINK_SUBTLV_LINK_ID		2
struct te_link_subtlv_link_id
{
  struct te_tlv_header	header;
  struct in_addr	value;
};

// Link Sub-TLV: Local Interface IP Address
#define	TE_LINK_SUBTLV_LCLIF_IPADDR	3
struct te_link_subtlv_lclif_ipaddr
{
  struct te_tlv_header	header;
  struct in_addr	value;	//Only one address is allowed
};

// Link Sub-TLV: Remote Interface IP Address
#define	TE_LINK_SUBTLV_RMTIF_IPADDR	4
struct te_link_subtlv_rmtif_ipaddr
{
  struct te_tlv_header	header;
  struct in_addr	value;	// Only one address is allowed
};

// Link Sub-TLV: Traffic Engineering Metric
#define	TE_LINK_SUBTLV_TE_METRIC		5
struct te_link_subtlv_te_metric
{
  struct te_tlv_header	header;
  u_int32_t		value;
};

// Link Sub-TLV: Maximum Bandwidth
#define	TE_LINK_SUBTLV_MAX_BW		6
struct te_link_subtlv_max_bw
{
  struct te_tlv_header	header;
  float			value;
};

// Link Sub-TLV: Maximum Reservable Bandwidth
#define	TE_LINK_SUBTLV_MAX_RSV_BW	7
struct te_link_subtlv_max_rsv_bw
{
  struct te_tlv_header	header;
  float			value;
};

// Link Sub-TLV: Unreserved Bandwidth
#define	TE_LINK_SUBTLV_UNRSV_BW		8
struct te_link_subtlv_unrsv_bw
{
  struct te_tlv_header	header;
  float			value[8];
};

// Link Sub-TLV: Administrative Group, a.k.a. Resource Class/Color
#define	TE_LINK_SUBTLV_RSC_CLSCLR	9
struct te_link_subtlv_rsc_clsclr
{
  struct te_tlv_header	header;	
  u_int32_t		value;
};

// Link Sub-TLV: Link Local/Remote Identifiers
#define	TE_LINK_SUBTLV_LINK_LCRMT_ID	11
struct te_link_subtlv_link_lcrmt_id
{
  struct te_tlv_header	header;
  u_int32_t		link_local_id;		// Local Identifier
  u_int32_t		link_remote_id;		// Remote Identifier
};

// Link Sub-TLV / Switching Capability-specific information: PSC-1, PSC-2, PSC-3, or PSC-4
struct link_ifswcap_specific_psc {
	float		 	min_lsp_bw;
	u_int16_t		mtu;
	u_char		padding[2];
};

// Link Sub-TLV / Switching Capability-specific information: TDM
struct link_ifswcap_specific_tdm {
	float	 		min_lsp_bw;
	u_char		indication;
	u_char		padding[3];
};

// Link Sub-TLV / Switching Capability-specific information: VLAN/Ethernet
#define IFSWCAP_SPECIFIC_VLAN_BASIC 0x0002
#define IFSWCAP_SPECIFIC_VLAN_ALLOC 0x0004
#define IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z 0x8000
struct link_ifswcap_specific_vlan {
	u_int16_t		length;		//up to 512 byes in vlan_bitmask. So 515 will be the default length.
	u_int16_t	 	version;       //version id and options mask
	u_char           bitmask[MAX_VLAN_NUM/8];
	u_char           bitmask_alloc[MAX_VLAN_NUM/8];
};

#define IFSWCAP_SPECIFIC_SUBNET_UNI 0x4000
#define MAX_TIMESLOTS_NUM 192 // 192 STS-1 = 10Gbps 
struct link_ifswcap_specific_subnet_uni {
	u_int16_t		length;
	u_int16_t	 	version;       //version id and options mask | IFSWCAP_SPECIFIC_VLAN_SUBNET_UNI
	u_int8_t		subnet_uni_id;
	u_int8_t		first_timeslot; //not used in ospfd
	u_char		swcap_ext;
	u_char		encoding_ext;
	u_int32_t		tna_ipv4;
	u_int32_t		nid_ipv4;
	u_int32_t		data_ipv4;
	u_int32_t		logical_port_number;
	u_int32_t		egress_label_downstream; //egress label on the UNI interface  //to be removed
	u_int32_t		egress_label_upstream; //egress label on the UNI interface for bidirectional traffic //to be removed
	char			control_channel[12];
	char			node_name[16];
	u_int8_t		timeslot_bitmask[MAX_TIMESLOTS_NUM/8]; //time slots available == 1
};

#define HAS_VLAN(P, VID) ((P[(VID-1)/8] & (0x80 >> (VID-1)%8)) != 0)
#define SET_VLAN(P, VID) P[(VID-1)/8] = (P[(VID-1)/8] | (0x80 >> (VID-1)%8))
#define RESET_VLAN(P, VID) P[(VID-1)/8] = (P[(VID-1)/8] & ~(0x80 >> (VID-1)%8))

#define HAS_TIMESLOT HAS_VLAN
#define SET_TIMESLOT SET_VLAN
#define RESET_TIMESLOT RESET_VLAN

//
//  Switching Capability-specific information for L2SC, LSC and FSC will be added later on
//  Some of those information is still under development or vendor-specific
//

// Link Sub-TLV: Interface Switching Capability Descriptor
#define	TE_LINK_SUBTLV_LINK_IFSWCAP		15
struct te_link_subtlv_link_ifswcap
{
  struct te_tlv_header	header;		/* Value length is variable length octets. */
  struct {
#define LINK_IFSWCAP_SUBTLV_SWCAP_PSC1		1
#define LINK_IFSWCAP_SUBTLV_SWCAP_PSC2		2
#define LINK_IFSWCAP_SUBTLV_SWCAP_PSC3		3 
#define LINK_IFSWCAP_SUBTLV_SWCAP_PSC4		4
#define LINK_IFSWCAP_SUBTLV_SWCAP_L2SC		51
#define LINK_IFSWCAP_SUBTLV_SWCAP_TDM		100
#define LINK_IFSWCAP_SUBTLV_SWCAP_LSC		150
#define LINK_IFSWCAP_SUBTLV_SWCAP_FSC		200
       u_char	switching_cap;

#define LINK_IFSWCAP_SUBTLV_ENC_PKT			1
#define LINK_IFSWCAP_SUBTLV_ENC_ETH			2
#define LINK_IFSWCAP_SUBTLV_ENC_PDH			3
#define LINK_IFSWCAP_SUBTLV_ENC_RESV1		4
#define LINK_IFSWCAP_SUBTLV_ENC_SONETSDH	5
#define LINK_IFSWCAP_SUBTLV_ENC_RESV2		6
#define LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP	7
#define LINK_IFSWCAP_SUBTLV_ENC_LAMBDA		8
#define LINK_IFSWCAP_SUBTLV_ENC_FIBER		9
#define LINK_IFSWCAP_SUBTLV_ENC_RESV3		10
#define LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL		11
	u_char	encoding;

       u_char	reserved[2];
	float max_lsp_bw_at_priority[8];
       union {
		struct link_ifswcap_specific_psc  ifswcap_specific_psc;
		struct link_ifswcap_specific_tdm ifswcap_specific_tdm; 
       	struct link_ifswcap_specific_vlan ifswcap_specific_vlan; 
       	struct link_ifswcap_specific_subnet_uni ifswcap_specific_subnet_uni;
       } ifswcap_specific_info;
  } link_ifswcap_data;
};
#define IFSWCAP_BASE_SIZE 48

#define DRAGON_TLV_TYPE_BASE 0x8800
#define TE_LINK_SUBTLV_RESV_SCHEDULE  DRAGON_TLV_TYPE_BASE + 1
#define TE_LINK_SUBTLV_LINK_IFADCAP DRAGON_TLV_TYPE_BASE + 2
#define TE_LINK_SUBTLV_DOMAIN_ID DRAGON_TLV_TYPE_BASE + 0x10

struct te_link_subtlv_link_ifadcap
{
    struct te_tlv_header	header;
    u_char	switching_cap_lower;
    u_char	encoding_lower;
    u_char	switching_cap_upper;
    u_char	encoding_upper;
    u_int32_t max_lsp_bw_at_priority[8];
};

struct te_link_subtlv_domain_id
{
    struct te_tlv_header	header;	
    u_int32_t value;
};

/////////Testing TLV's/////////
#define MOVAZ_TEST0 33025
#define MOVAZ_TEST1 33031
#define MOVAZ_TEST2 33033
#define MOVAZ_TEST3 33035

/////////////////////////////////////////////////////////////////
//////////////LSAHandler defined as an Priority Event///////////////////
/////////////////////////////////////////////////////////////////
class Resource;

class LSAHandler: public Event
{
private:
    struct lsa_header * lsa;
    api_action action;
    Resource* rc;
    u_int32_t domain_mask;
public:
    LSAHandler(): lsa(NULL), rc(NULL), domain_mask(0), action(ACT_NOOP) {}
    virtual ~LSAHandler();
    virtual void Run();
    void Load(api_msg* msg);
    Resource* Parse();
};

#endif
