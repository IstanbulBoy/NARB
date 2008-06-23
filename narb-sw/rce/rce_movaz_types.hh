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

#ifndef __RCE_MOVAZ_TYPES__
#define __RCE_MOVAZ_TYPES__


typedef struct movaz_tlvdata_te_passthrough_wavegrid // 48 Bytes MOVAZ_TE_LGRID
{
 u_int32_t base;
 u_int16_t interval;
 u_int16_t size;
 u_int8_t  in_channels[20];
 u_int8_t  out_channels[20];
} MovazWaveGrid;

typedef struct movaz_tlvdata_te_lambda_info //36 Bytes MOVAZ_TE_LAMBDA
{
 u_int8_t reserved1[4];
 u_int8_t link_id[4];
 u_int32_t channel_id;
 u_int32_t direction;
 u_int8_t reserved2[4];
 u_int8_t priority;
 u_int8_t sw_cap[4];
 u_int8_t reserved3[7];
 u_int32_t data_rate;
} MovazTeLambda;


// 4 Bytes u_int32_t MOVAZ_IF_PHYID

#define MOVAZ_LSC 151

inline void ntoh_wavegrid(MovazWaveGrid& wavegrid)
{
    wavegrid.base = ntohl(wavegrid.base);
    wavegrid.size = ntohs(wavegrid.size);
    wavegrid.interval = ntohs(wavegrid.interval);
}

inline void ntoh_telambda(MovazTeLambda& tel)
{
    tel.channel_id = ntohl(tel.channel_id);
    //? tel.data_rate
}

inline bool valid_channel(u_int32_t freq)
{
    return (freq >= 192000 && freq < 196000 && freq%100 == 0);
}

//////////////////////////////////////////////////////
/// fixed data-rate, encoding, swcap mapping table            ////
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////
/* Below is an example of LSA dump for Movaz's propietary TLVs
Unknown TLV: [type(0x8101), length(0x8)] 8 Bytes
Ring  : Number:1, First: 10.100.40.0, Direction CW
 {0xa, 0x64, 0x3c, 0x0, 0x1, 0x0, 0x0, 0x0}


Unknown TLV: [type(0x8109), length(0x8)] 8 Bytes
Interface HW Info: 0x00000001 0x00000001
 {0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1}


Unknown TLV: [type(0x810b), length(0x4)] 4 Bytes
Interface PHY Id: 20800
 {0x0, 0x0, 0x51, 0x40}


Unknown TLV: [type(0x810f), length(0x4)] 4 Bytes
Transparent channel bandwidth: 11095.70 Mbps
 {0x4e, 0xa5, 0x56, 0xbd}

Unknown TLV: [type(0x810c), length(0x30)] 48 Bytes
TE Pass-trough lambda grid. Base: 192000 Interval 100 Size 40:
Input channels (channel id @ priority) :
192000 @ 7 192100 @ 7 
192200 @ 7 192300 @ 7 
192400 @ 7 192500 @ 7                 
192600 @ 7 192700 @ 7 
192800 @ 7 192900 @ 7 
193000 @ 7 
193200 @ 7 
193400 @ 7 193500 @ 7 
193600 @ 7 193700 @ 7 
193800 @ 7 193900 @ 7 
194000 @ 7 
194200 @ 7 194300 @ 7 
194400 @ 7 194500 @ 7 
194600 @ 7 194700 @ 7 
194800 @ 7 194900 @ 7 
195000 @ 7 195100 @ 7 
195200 @ 7 195300 @ 7 
195400 @ 7 195500 @ 7 
195600 @ 7 195700 @ 7 
195800 @ 7 195900 @ 7 
Output channels (channel id @ priority) :
192000 @ 7 192100 @ 7 
192200 @ 7 192300 @ 7 
192400 @ 7 192500 @ 7 
192600 @ 7 192700 @ 7                 
192800 @ 7 192900 @ 7 
193000 @ 7 
193200 @ 7 
193400 @ 7 193500 @ 7 
193600 @ 7 193700 @ 7 
193800 @ 7 193900 @ 7 
194000 @ 7 
194200 @ 7 194300 @ 7 
194400 @ 7 194500 @ 7 
194600 @ 7 194700 @ 7 
194800 @ 7 194900 @ 7 
195000 @ 7 195100 @ 7 
195200 @ 7 195300 @ 7 
195400 @ 7 195500 @ 7 
195600 @ 7 195700 @ 7 
195800 @ 7 195900 @ 7 
 {
 0x0, 0x2, 0xee, 0x0, base
 0x0, 0x64, interval
 0x0, 0x14, 20*2 = size ?
			below 40-byte mask (each byte for a channel with 
				eight priorities, 7 is the lowest)
 0x77, 0x77, 0x77, 0x77, 
 0x77, 0xf7, 0xf7, 0x77, {input channels 12, 14: 0xf, others 0x7}
 0x77, 0x77, 0xf7, 0x77 <repeats 14 times>, 0xf7, 0xf7, 0x77, 
 0x77, 0x77, 0xf7, 0x77, {output channels 12, 14: 0xf, others 0x7} 
 0x77, 0x77, 0x77, 0x77, 
 0x77, 0x77, 0x77, 0x77
 }

Unknown TLV: [type(0x8107), length(0x24)] 36B TeLambdaInfo
TE Lambda:193100, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0x20000d Encoding: DW-10GbE DataRate: 11095.70 Mbps
 {
 0x0, 0x1, 0x0, 0x9, (fixed?)
 0x0, 0x1, 0x0, 0x0, Link ID
 0x0, 0x2, 0xf2, 0x4c, Channel#
 0x1, 0x0, 0x0, 0x0, (Direction 1: in 2: out)
 0x0, 0x2, 0x0, 0x5, (fixed?)
 0x7, Priority
 0x0, 0x20, 0x0, 0xd, SwCap (Encoding implied by datarate?)
 0x0, 0x0, 0x0, 0x81, 0x8, 0x0, 0x4, (fixed ?)
 0x4e, 0xa5, 0x56, 0xbd, DataRate (in network float -> htonl + ntohf_mbps)
 }
More TLVs: [type(0x8107), length(0x24)] 36B TeLambdaInfo
TE Lambda:193100, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0x20000d Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:194100, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0x20000d Encoding: oc48/stm16 DataRate: 2488.32 Mbps             
TE Lambda:194100, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0x20000d Encoding: oc48/stm16 DataRate: 2488.32 Mbps
TE Lambda:193300, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0x20000d Encoding: oc48/stm16 DataRate: 2488.32 Mbps
TE Lambda:193300, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0x20000d Encoding: oc48/stm16 DataRate: 2488.32 Mbps
TE Lambda:203940, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:203940, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:201200, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:201200, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:198540, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:198540, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:195940, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:195940, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps                  
TE Lambda:193410, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:193410, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:190950, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:190950, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:188550, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:188550, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:186210, Link:0.1.0.0, Dir:In, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps
TE Lambda:186210, Link:0.1.0.0, Dir:Out, Pri:7, SwCap:0xe Encoding: DW-10GbE DataRate: 11095.70 Mbps

*/

#endif
