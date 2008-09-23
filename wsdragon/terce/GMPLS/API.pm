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
# File: API.pm
#

package GMPLS::API;

use strict;
use warnings;
use Aux;
use Log;
use GMPLS::Constants;
use Socket;
use Compress::Zlib;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.31 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ( );
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub parse_tlv($$$;$);

sub dump_hdr($) {
	my ($mr) = @_;
	Aux::print_dbg_data("----------------------- header -----------------------\n");
	Aux::print_dbg_data("(%s, %s)\n", $msg_type_X{$$mr{hdr}{type}}, $msg_action_X{$$mr{hdr}{action}});
	Aux::print_dbg_data("type action length:\t0x%02X (%u)  0x%02X (%u)  0x%04X (%u)\n", $$mr{hdr}{type}, $$mr{hdr}{type}, $$mr{hdr}{action}, $$mr{hdr}{action}, $$mr{hdr}{length}, $$mr{hdr}{length});
	Aux::print_dbg_data("ucid:\t\t\t0x%08X (%u)\n", $$mr{hdr}{ucid}, $$mr{hdr}{ucid});
	Aux::print_dbg_data("seq. number:\t\t0x%08X (%u)\n", $$mr{hdr}{seqn}, $$mr{hdr}{seqn});
	Aux::print_dbg_data("checksum:\t\t0x%08X (%u)\n", $$mr{hdr}{chksum}, $$mr{hdr}{chksum});
	Aux::print_dbg_data("tag1:\t\t\t0x%08X (%u)\n", $$mr{hdr}{tag1}, $$mr{hdr}{tag1});
	Aux::print_dbg_data("tag2:\t\t\t0x%08X (%u)\n", $$mr{hdr}{tag2}, $$mr{hdr}{tag2});
}

sub dump_data($) {
	my ($mr) = @_;
	if(Aux::dbg_data()) {
		dump_hdr($mr);
		Aux::print_dbg_data("-------------------- raw data ------------------------\n");
		if(defined($$mr{data})) {
			my @tmp = unpack("C*", $$mr{data});
			my $len = $$mr{hdr}{length};
			if($len != @tmp) {
				Log::log "warning", "corrupted message: ".$len." <-> ".(@tmp+0)."\n";
				return;
			}
			for(my $i=0; $i<$len; $i++) {
				Aux::print_dbg_data("%02X ", $tmp[$i]);
				if(!(($i+1)%16) || (($i+1)==$len)) {
					Aux::print_dbg_data("\n");
				}
			}
		}
		Aux::print_dbg_data("------------------------------------------------------\n");
	}
}

sub queue_bin_msg($$$$;$) {
	my ($owner, $act, $type, $err, $data) = @_;
	my ($len, $ucid, $sn);

	if(($act == ACT_ACK) && (defined($$owner{bin_queue}{in}))) {
		$type = $$owner{bin_queue}{in}{hdr}{type};
		$ucid = $$owner{bin_queue}{in}{hdr}{ucid};
		$sn = $$owner{bin_queue}{in}{hdr}{seqn};
	}
	else {
		$ucid = $$owner{bin_queue}{ucid};
		$sn = $$owner{bin_queue}{out}{seqn}++;
	}

	# process data (TLVs) if any
	if(defined($data)) {
	}
	else {
		$len = 0;
	}

	# form a header
	if($err != 0) {
		$act = ACT_ERROR;
		$len = 0;
	}

	my $chksum = Aux::chksum("CCnNN", "%32N3", $type, $act, $len, $ucid, $sn);
	$$owner{bin_queue}{out}{hdr} = {
		"type" => $type,
		"action" => ($err==0)?$act:ACT_ERROR,
		"length" => $len,
		"ucid" => $ucid,
		"seqn" => $sn,
		"chksum" => $chksum,
		"tag1" => 0,
		"tag2" => $err
	};
	$$owner{bin_queue}{out}{queue} .= pack("CCnNNNNN", 
		$type,
		($err==0)?$act:ACT_ERROR,
		$len,
		$ucid,
		$sn,
		$chksum,
		0,
		$err
	);
}

sub send_bin_msg($) {
	my ($owner) = @_;
	my $n = 0;
	
	my @witeable = $$owner{select}->can_write();
	$n = $$owner{bin_queue}{fh}->syswrite($$owner{bin_queue}{out}{queue});
	if(defined($n)) {
		$$owner{bin_queue}{out}{queue} = substr($$owner{bin_queue}{out}{queue}, $n);
		Aux::print_dbg_api("sent %s for %s to %s:%s\n", 
			$msg_action_X{$$owner{bin_queue}{out}{hdr}{action}}, 
			$msg_type_X{$$owner{bin_queue}{out}{hdr}{type}},
			$$owner{bin_queue}{fh}->peerhost(), 
			$$owner{bin_queue}{fh}->peerport()
		);
		dump_data($$owner{bin_queue}{out});
	}
	else {
		$$owner{bin_queue}{out}{queue} = '';
		Aux::print_dbg_api("failed to send %s for %s to %s:%s\n", 
			$msg_action_X{$$owner{bin_queue}{out}{hdr}{action}}, 
			$msg_type_X{$$owner{bin_queue}{out}{hdr}{type}},
			$$owner{bin_queue}{fh}->peerhost(), 
			$$owner{bin_queue}{fh}->peerport()
		);
	}
}

sub get_bin_msg($) {
	my ($owner) = @_;
	my $hdr;
	my $data;
	my ($type, $action, $len, $ucid, $sn, $chksum, $tag1, $tag2);

	if(!defined($$owner{bin_queue}{fh}->connected())) {
		die "client not connected\n";
	}
	my $n = $$owner{bin_queue}{fh}->sysread($hdr, 24);
	if(!defined($n)) {
		if($! != Errno::EINTR) {
			die "socket error\n";
		}
		die "connection terminated\n";
	}
	if(!$n) {
		die "client disconnect\n";
	}
	if($n != 24) {
		die "GMPLS: malformed msg. header (wrong length)\n";
	}
	($type, $action, $len, $ucid, $sn, $chksum, $tag1, $tag2) = unpack("CCnNNNNN", $hdr);	
	if(!(
			defined($type) && 
			defined($action) && 
			defined($len) && 
			defined($ucid) && 
			defined($sn) && 
			defined($chksum) && 
			defined($tag1) && 
			defined($tag2))) {
		die "GMPLS: malformed msg. header\n";
	}
	$$owner{bin_queue}{in}{hdr} = {
		"type" => $type,
		"action" => $action,
		"length" => $len,
		"ucid" => $ucid,
		"seqn" => $sn,
		"chksum" => $chksum,
		"tag1" => $tag1,
		"tag2" => $tag2
	};
	$$owner{bin_queue}{in}{data} = undef;

	# and the body
	if($len > 0) {
		$n = $$owner{bin_queue}{fh}->sysread($data, $len);
		if(!defined($n)) {
			die "socket error\n";
		}
		$$owner{bin_queue}{in}{data} = $data;
	}
	Aux::print_dbg_api("received %s from %s:%s\n", $msg_action_X{$$owner{bin_queue}{in}{hdr}{action}},
		$$owner{bin_queue}{fh}->peerhost(), $$owner{bin_queue}{fh}->peerport()
	);
	dump_data($$owner{bin_queue}{in});
}

sub clean_bin_msg($) {
	my ($owner) = @_;
	$$owner{bin_queue}{in} = undef;
}

sub parse_tlv_data($$$$) {
	my ($md, $o, $tmpl, $ar) = @_;
	#expand the template first
	while($tmpl =~ /(\D)(\d+)/) {
		$tmpl = $`;
		for(my $i=0; $i<$2; $i++) {
			$tmpl.=$1;
		}
		$tmpl .=$';
	}
	my $n = length($tmpl);
	$tmpl = "x[$o]".$tmpl;
	@$ar = unpack($tmpl, $md);
	if(@$ar != $n) {
		return (-1);
	}
	for(my $i=0; $i<@$ar; $i++) {
		if(!defined($$ar[$i])) {
			return (-1);
		}
	}
	return (0);
}

# NOTE: wildly recursive
sub parse_tlv($$$;$) {
	my ($md, $o, $owner, $p) = @_;
	my $ret = 0;
	my $fmt = "";
	my ($adv_rtr, $lsa_len) = unpack("x[8]Nx[10]n", $md);	
	# TLV header
	my ($tlv_type, $tlv_len) = unpack("x[$o]nn", $md);	
	my $tlv_stype = undef;
	if(!(defined($tlv_type) && defined($tlv_len))) {
		Log::log "err", "corrupted LSA (TLV Header)\n";
		return (-1);
	}
	$o += TLV_HDR_SIZE;
	# top level TLV
	if(!defined($p)) {
		if($tlv_type == TE_TLV_ROUTER_ADDR) {
			Aux::print_dbg_lsa("TLV: %s(%d)\n", $tlvs_X{$tlv_type}, $tlv_len);
			my @res;
			$fmt = "N";
			parse_tlv_data($md, $o, $fmt, \@res);
			Aux::print_dbg_lsa("   ROUTER ADDRESS: 0x%08x\n", $res[0]);
			unshift(@res, {"cmd"=>TEDB_RTR_ON, "type"=>$tlv_type, "rtr"=>$adv_rtr});
			Aux::send_msg($owner, ADDR_GMPLS_CORE, @res);
			return (0);
		}
		elsif($tlv_type == TE_TLV_LINK) {
			Aux::print_dbg_lsa("TLV: %s(%d)\n", $tlvs_X{$tlv_type}, $tlv_len);
			my @cmd = ({"cmd"=>TEDB_LINK_MARK, "type"=>$tlv_type, "rtr"=>$adv_rtr});
			Aux::send_msg($owner, ADDR_GMPLS_CORE, @cmd);
			return(parse_tlv($md, $o, $owner, TE_TLV_LINK));
		}
		else {
			Log::log "warning", "unknown TLV type ($tlv_type)\n";
			return (0);
		}
	}
	# sub level TLVs
	else {
		# traverse to the end of the sub-TLVs first to see
		# if all the data are consistent (allign to 4-byte boundary)
		my $o1 = $o + (($tlv_len+TLV_ALIGN-1)&(~(TLV_ALIGN-1)));
		if($o1<$lsa_len) {
			if(parse_tlv($md, $o1, $owner, $p)<0) {
				# this will invalidate the entire chain
				# before anything gets parsed if error occurs
				return -1;
			}
		}
		if(exists($sub_tlvs_link_X{$tlv_type})) {
			Aux::print_dbg_lsa("    sub-TLV: %s(%d)\n", $sub_tlvs_link_X{$tlv_type}, $tlv_len);
		}
		my @res;
		my @info = ();
		my $tmp_fmt = "";
		if($tlv_type == TE_LINK_SUBTLV_LINK_TYPE) {
			$fmt = "C";
			$tmp_fmt = "C";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %s\n", $sub_tlvs_link_type_X{$res[0]});
		}
		elsif($tlv_type == TE_LINK_SUBTLV_LINK_ID) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       0x%08x\n", $res[0]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_LCLIF_IPADDR) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %s\n", inet_ntoa(pack("N", $res[0])));
		}
		elsif($tlv_type == TE_LINK_SUBTLV_RMTIF_IPADDR) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %s\n", inet_ntoa(pack("N", $res[0])));
		}
		elsif($tlv_type == TE_LINK_SUBTLV_TE_METRIC) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %d\n", $res[0]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_MAX_BW) {
			$fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			$tmp_fmt = "f";
			$res[0] = unpack("f", pack("V", $res[0]));
			Aux::print_dbg_lsa("       %s\n", $res[0]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_MAX_RSV_BW) {
			$fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			$tmp_fmt = "f";
			$res[0] = unpack("f", pack("V", $res[0]));
			Aux::print_dbg_lsa("       %s\n", $res[0]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_UNRSV_BW) {
			$fmt = "N8";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			$tmp_fmt = "f8";
			for(my $i=0; $i<@res; $i++) {
				$res[$i] = unpack("f", pack("V", $res[$i]));
				Aux::print_dbg_lsa("       %s\n", $res[$i]);
			}
		}
		elsif($tlv_type == TE_LINK_SUBTLV_RSC_CLSCLR) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %d\n", $res[0]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_LINK_LCRMT_ID) {
			$fmt = "NN";
			$tmp_fmt = "NN";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       0x%08x\n", $res[0]);
			Aux::print_dbg_lsa("       0x%08x\n", $res[1]);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_LINK_IFSWCAP) {
			# common part
			$fmt = "C4N8";
			$tmp_fmt = "C4f8";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       %s\n", $sub_tlvs_link_swcap_cap{$res[0]});
			Aux::print_dbg_lsa("       %s\n", $sub_tlvs_link_swcap_enc{$res[1]});
			for(my $i=0; $i<8; $i++) {
				$res[$i+4] = unpack("f", pack("V", $res[$i+4]));
				Aux::print_dbg_lsa("       max. bw at pr. %d: %s\n", $i, $res[$i+4]);
			}
			$o += 36;
			if($res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 ||
				$res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_PSC2 ||
				$res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_PSC3 ||
				$res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_PSC4) {
				$fmt = "Nn";
				if(parse_tlv_data($md, $o, $fmt, \@info)<0) {
					return (-1);
				}
				$tmp_fmt .= "fn";
				$info[0] = unpack("f", pack("V", $info[0]));
				Aux::print_dbg_lsa("       min. bw: %s\n", $info[0]);
				Aux::print_dbg_lsa("       mtu: %d\n", $info[1]);
			}
			elsif($res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC) {
				$fmt = "nn";
				$tmp_fmt .= "nn";
				if(parse_tlv_data($md, $o, $fmt, \@info)<0) {
					return (-1);
				}
				my ($len, $ver) = @info;
				my $vlan_bitmaps = "";
				my $tmp_len = 0;
				if($ver & IFSWCAP_SPECIFIC_SUBNET_UNI) {
					$fmt = "CCCCNNNNNN";
					if(parse_tlv_data($md, $o+4, $fmt, \@info)<0) {
						return (-1);
					}
					$tlv_stype = "uni";
					Aux::print_dbg_lsa("       subnet uni id: %d\n", $info[0]);
					Aux::print_dbg_lsa("       first timeslot: %d\n", $info[1]);
					Aux::print_dbg_lsa("       swcap ext: %d\n", $info[2]);
					Aux::print_dbg_lsa("       encoding ext: %d\n", $info[3]);
					Aux::print_dbg_lsa("       tna ipv4: %08x\n", $info[4]);
					Aux::print_dbg_lsa("       nid ipv4: %08x\n", $info[5]);
					Aux::print_dbg_lsa("       data ipv4: %08x\n", $info[6]);
					Aux::print_dbg_lsa("       logical port number: %d\n", $info[7]);
					Aux::print_dbg_lsa("       egress label downstream: %d\n", $info[8]);
					Aux::print_dbg_lsa("       egress label upstream: %d\n", $info[9]);
					Aux::print_dbg_lsa("       control channel: %s\n", substr($md, $o + 4 + 28, 12));
					Aux::print_dbg_lsa("       node name: %s\n", substr($md, $o + 4 + 40, 16));
					my $l = index(substr($md, $o + 4 + 40, 16), chr(0));
					if($l < 0) {
						$l = 16;
					}
					push(@info, substr($md, $o + 4 + 40, $l));
					$tmp_fmt .= "CCCCNNNNNNC/a*";   # NOTE: C/a* template is packing a string with its length
					my $ts = substr($md, $o + 4 + 56, 192/8);
					my $tcnt = 0;
					Aux::print_dbg_lsa("       time slots: ");
					for(my $i=0; $i<192; $i++) {
						if(vec($ts, $i, 1)) {
							Aux::print_dbg_lsa("%03d ", $i);
							$tcnt++;
							if(!($tcnt%8)) {
								Aux::print_dbg_lsa("\n                   ");
							}
						}
					}
					if(($tcnt%8) || !$tcnt) {
						Aux::print_dbg_lsa("\n");
					}
				}
				else {
					# uncompress first if needed
					if($ver & IFSWCAP_SPECIFIC_VLAN_BASIC) {
						$tmp_len += 512;
					}
					if($ver & IFSWCAP_SPECIFIC_VLAN_ALLOC) {
						$tmp_len += 512;
					}
					if($ver & IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z) {
						$vlan_bitmaps = uncompress(substr($md, $o+4, $len));
						if(!defined($vlan_bitmaps)) {
							Log::log "err", "vlan compression error\n";
							return (-1);
						}
					}
					else {
						$vlan_bitmaps = substr($md, $o+4, $len);
					}
					if(length($vlan_bitmaps) != $tmp_len) {
						Log::log "err", "vlan bitmask error (length)\n";
						return (-1);
					}
					my $vlans = "";
					my $alloc_vlans = "";
					if($tmp_len > 512) {
						$vlans = substr($vlan_bitmaps, 0, 512);
						$alloc_vlans = substr($vlan_bitmaps, 512, 512);
					}
					else {
						$vlans = $vlan_bitmaps;
					}
					if(length($vlans)>0) {
						Aux::print_dbg_lsa("       vlans:");
						my $bin_s = join("", unpack("B*", $vlans));
						my $p = 0;
						while($bin_s =~ s/(1+)//) {
							push(@info, sprintf("%d-%d", $-[0]+$p+1, $+[0]+$p));
							my $l = length($info[$#info]);
							$tmp_fmt .= "C/a*";
							Aux::print_dbg_lsa(" %d-%d", $-[0]+$p+1, $+[0]+$p);
							$p = $+[0] - $-[0];
						}
						Aux::print_dbg_lsa("\n");
					}
					if(length($alloc_vlans)>0) {
						Aux::print_dbg_lsa("       alloc. vlans:");
						my $bin_s = join("", unpack("B*", $alloc_vlans));
						my $p = 0;
						while($bin_s =~ s/(1+)//) {
							Aux::print_dbg_lsa(" %d-%d", $-[0]+$p+1, $+[0]+$p);
							$p = $+[0] - $-[0];
						}
						Aux::print_dbg_lsa("\n");
					}
				}
			}
			elsif($res[0] == LINK_IFSWCAP_SUBTLV_SWCAP_TDM) {
				$fmt = "NC";
				$tmp_fmt .= "fC";
				if(parse_tlv_data($md, $o, $fmt, \@info)<0) {
					return (-1);
				}
				$info[0] = unpack("f", pack("V", $info[0]));
				Aux::print_dbg_lsa("       min. bw: %s\n", $info[0]);
				Aux::print_dbg_lsa("       indication: %d\n", $info[1]);
			}
			push(@res, @info);
		}
		elsif($tlv_type == TE_LINK_SUBTLV_RESV_SCHEDULE) {
			Log::log "warning", "Resv. schedule TLV. Not parsed.";
		}
		elsif($tlv_type == TE_LINK_SUBTLV_LINK_IFADCAP) {
			$fmt = "C4N8";
			$tmp_fmt = "C4f8";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       swcap lower: %d\n", $res[0]);
			Aux::print_dbg_lsa("       enc lower: %d\n", $res[1]);
			Aux::print_dbg_lsa("       swcap upper: %d\n", $res[2]);
			Aux::print_dbg_lsa("       enc upper: %d\n", $res[3]);
			for(my $i=0; $i<8; $i++) {
				$res[4+$i] = unpack("f", pack("V", $res[0+$i]));
				Aux::print_dbg_lsa("       max. bw at pr. %d: %s\n", $i, $res[4+$i]);
			}
		}
		elsif($tlv_type == TE_LINK_SUBTLV_DOMAIN_ID) {
			$fmt = "N";
			$tmp_fmt = "N";
			if(parse_tlv_data($md, $o, $fmt, \@res)<0) {
				return (-1);
			}
			Aux::print_dbg_lsa("       domain ID: 0x%08x\n", $res[0]);
		}
		else{
			Log::log "warning", "unknown sub-TLV type ($tlv_type)\n";
			return (-1);
		}
		unshift(@res, {"cmd"=>TEDB_INSERT, "type"=>$tlv_type, "subtype"=>$tlv_stype, "rtr"=>$adv_rtr});
		Aux::send_msg($owner, ADDR_GMPLS_CORE, @res);
	}
	return(0);
}

sub parse_msg($) {
	my ($owner) = @_;
	my $ret = 0;
	# LSA header
	my ($age, $opts, $type, $id, $rtr, $seqn, $chksum, $len) = unpack("nCCNNNnn", $$owner{bin_queue}{in}{data});	
	if(!(defined($age) && defined($age) && defined($age) && defined($age) && 
			defined($age) && defined($age) && defined($age) && defined($age))) {
		Log::log "err", "corrupted LSA (LSA Header)\n";
		return (-1);
	}
	if($type != OSPF_OPAQUE_AREA_LSA) {
		Log::log "warning", "unexpected LSA type ($lsa_type_X{$type})\n";
		return (0);
	}
	Aux::print_dbg_lsa("----------------- parsed data ------------------------\n");
	Aux::print_dbg_lsa("%s from %s (%d)\n", $lsa_type_X{$type}, inet_ntoa(pack("N", $rtr)), $len);
	$ret = parse_tlv($$owner{bin_queue}{in}{data}, LSA_HDR_SIZE, $owner);
	Aux::print_dbg_lsa("------------------------------------------------------\n");
	return($ret);
}

sub is_sync_init($) {
	my ($owner) = @_;
	return (($$owner{bin_queue}{in}{hdr}{type} == TERCE_TOPO_SYNC) && ($$owner{bin_queue}{in}{hdr}{action} == ACT_INIT));
}

sub is_delim($) {
	my ($owner) = @_;
	return (($$owner{bin_queue}{in}{hdr}{type} == TERCE_TOPO_SYNC) && ($$owner{bin_queue}{in}{hdr}{action} == ACT_NOP));
}

sub is_sync_insert($) {
	my ($owner) = @_;
	return (($$owner{bin_queue}{in}{hdr}{type} == TERCE_TOPO_SYNC) && ($$owner{bin_queue}{in}{hdr}{action} == ACT_INSERT));
}

1;
