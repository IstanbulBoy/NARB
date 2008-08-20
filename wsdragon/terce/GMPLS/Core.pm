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
# July 8, 2008
#
# File: Core.pm
#

package GMPLS::Core;

use strict;
use warnings;
use Socket;
use GMPLS::Constants;
use XML::Parser;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.10 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;


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


sub tag($$$);

sub new {
	shift;
	my($proc) = @_;
	my ($k, $proc_val) = each %$proc;  # child processes hold only self-descriptors
	my $self;
	eval {
		$self = {
			# process descriptor:
			"proc" => $proc, # process info
			"addr" => $$proc_val{addr}, # process IPC address
			"name" => $$proc_val{name}, # process name
			"fh" => $$proc_val{fh}, # IPC filehandle 
			"pool" => $$proc_val{pool}, # empty
			"select" => new IO::Select($$proc_val{fh}), # corresponding select object
			"writer" => new XML::Writer(OUTPUT => $$proc_val{fh}, ENCODING => "us-ascii"),
			"parser" => new XML::Parser(Style => "tree"), # incomming data parser
			"processor" => \&process_tedb_data, # data processor

			# object descriptor:
			"abstract_tedb" => undef,
			"control_tedb" => undef,
			"data_tedb" => undef
		};
	};
	if($@) {
		die "$$proc_val{name} instantiation failed: $@\n";
	}
	bless $self;
	return $self;
}

####################################################################

sub find_remote($$) {
	my $self = shift;
	my($dr, $ip) = @_;
	foreach my $rtr (keys %$dr) {
		foreach my $link (keys %{$$dr{$rtr}}) {
			# skip the top-level descriptors
			if(ref($$dr{$rtr}{$link}) ne "HASH") {
				next;
			}
			#replace the id with the actual ref to the remote link block
			if($$dr{$rtr}{$link}{local} eq $ip) {
				return $$dr{$rtr}{$link};
			}
		}
	}
	return undef;
}

# this probably creates a closure: we have to break up the graph after we are done with it
# otherwise we'll run out of memory
sub create_graph($) {
	my $self = shift;
	my($dr) = @_;
	foreach my $rtr (keys %$dr) {
		foreach my $link (keys %{$$dr{$rtr}}) {
			# skip the top-level descriptors
			if(ref($$dr{$rtr}{$link}) ne "HASH") {
				next;
			}
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

sub destroy_graph($) {
	my $self = shift;
	my($dr) = @_;
	foreach my $rtr (keys %$dr) {
		foreach my $link (keys %{$$dr{$rtr}}) {
			# skip the top-level descriptors
			if(ref($$dr{$rtr}{$link}) ne "HASH") {
				next;
			}
			# break all the references so the garbage collector can do its job
			if(exists($$dr{$rtr}{$link}{remote})) {
				$$dr{$rtr}{$link}{remote} = {};
			}
		}
	}
}

sub tag($$$) {
	my $self = shift;
	my($dr, $rtr, $t) = @_;
	
	foreach my $link (keys %{$$dr{$rtr}}) {
		# skip the top-level descriptors
		if(ref($$dr{$rtr}{$link}) ne "HASH") {
			next;
		}
		my $l = $$dr{$rtr}{$link};
		if(!defined($$l{tag})) {
			if(defined($$dr{$rtr}{tag}) && ($$dr{$rtr}{tag} ne $t)) {
				die "inconsistent network graph\n";
			}
			$$dr{$rtr}{tag} = $t;
			$$l{tag} = $t;
			# follow the link
			if(ref($$l{remote}) eq "HASH") {
				if(!defined($$l{remote}{tag})) {
					$self->tag($dr, $$l{remote}{rtr_id}, $t);
				}
			}
		}
	}
}

sub traverse($) {
	my $self = shift;
	my($dr) = @_;
	my $t = 0;
	foreach my $rtr (keys %$dr) {
		if(!defined($$dr{$rtr}{tag})) {
			$self->tag($dr, $rtr, $t);
			$t++;
		}
	}
	if($t > 2) {
		die "unexpected number of disconnected network graphs: $t\n";
	}
}

sub split_graph($) {
	my $self = shift;
	my($dr) = @_;
	my $t = undef;
	my @ret = ();
	my $tedb_ref = {};
	foreach my $rtr (sort({$$dr{$a}{tag} <=> $$dr{$b}{tag}} (keys %$dr))) {
		if(!defined($$dr{$rtr}{tag})) {
			next;
		}
		if(defined($t) && ($$dr{$rtr}{tag} != $t)) {
			push(@ret, $tedb_ref);
			$tedb_ref = {};
			$t = $$dr{$rtr}{tag};
		}
		else {
			$t = $$dr{$rtr}{tag};
		}
		$$tedb_ref{$rtr} = $$dr{$rtr};
	}
	push(@ret, $tedb_ref);
	return @ret;
}

sub get_topo_type($) {
	my $self = shift;
	my($dr) = @_;
	foreach my $rtr (keys %$dr) {
		if($$dr{$rtr}{src} eq "narb") {
			#abstract topology
			return "abstract";
		}
		elsif($$dr{$rtr}{src} eq "rce") {
			foreach my $l (keys %{$$dr{$rtr}}) {
				# skip the top-level descriptors
				if(ref($$dr{$rtr}{$l}) ne "HASH") {
					next;
				}
				if(exists($$dr{$rtr}{$l}{sw_cap}{uni})) {
					return "control";
				}
			}
		}
		else {
			return undef;
		}
	}
	return "data";
}


#
# inserts the temporary link block into the temporary tedb (if complete)
#
sub insert_link_blk($$$$) {
	my $self = shift;
	my($dr, $br, $hr, $cn) = @_;
	if(exists($$dr{$$br})) {
		# check if we have a link_id
		if(exists(${$$hr}{status})) {
			if((${$$hr}{status} & STAT_LINK_COMPLETE) == STAT_LINK_COMPLETE) {
				$$dr{$$br}{${$$hr}{link_id}} = $$hr;
				if(defined($$dr{$$br}{src}) && ($$dr{$$br}{src} ne $cn)) {
					Log::log "warning", "incoherent router block\n";
				}
				$$dr{$$br}{src} = $cn;
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
		$$dr{$data[0]} = {"src" => undef, "tag" => undef};
		Aux::print_dbg_tedb("top level rtr insert: 0x%08x\n", ${$$d{data}}[0]);
		return 0;
	}
	#link subtlv delimiter
	elsif($$d{cmd} == TEDB_LINK_MARK) {
		if(exists($$dr{$$d{rtr}})) {
			if(defined($$br)) {
				# insert the old link block to the dr
				my $err = $self->insert_link_blk($dr, $br, $tr, $$d{client}) if defined($$tr);
				Aux::print_dbg_tedb("  }");
				if(!$err) {
					Aux::print_dbg_tedb("  => inserted to %08x (%s) from %s\n", $$br, defined(WS::External::get_rtr_name($$br))?WS::External::get_rtr_name($$br):"unknown", $$d{client});
				}
				# and start a new one
				$$tr = {"status" => 0};
				$$br = $$d{rtr};
				$$sr = $err;
			}
			# first run of the current TEDB batch
			else {
				$$tr = {"status" => 0};
				$$br = $$d{rtr};
			}
			$$br = $$d{rtr};
			Aux::print_dbg_tedb("  LINK BLOCK (%s) {\n", defined(WS::External::get_rtr_name($$br))?WS::External::get_rtr_name($$br):"unknown");
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
						${$$tr}{tag} = undef; # tag used for net-graph traversal
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
						if(!defined($$d{subtype})) {
							${$$tr}{sw_cap}{vtags} = [];
							for(my $i = 14; $i<@data; $i++) {
								$v .= sprintf("\n      (%s)", $data[$i]) if Aux::dbg_tedb();
								push(@{${$$tr}{sw_cap}{vtags}}, $data[$i]);
							}
						}
						elsif($$d{subtype} eq "uni") {
							${$$tr}{sw_cap}{uni}{id} = $data[12];
							${$$tr}{sw_cap}{uni}{ts1} = $data[13];
							${$$tr}{sw_cap}{uni}{swcap_ext} = $data[14];
							${$$tr}{sw_cap}{uni}{enc_ext} = $data[15];
							${$$tr}{sw_cap}{uni}{tna4} = $data[16];
							${$$tr}{sw_cap}{uni}{nid4} = $data[17];
							${$$tr}{sw_cap}{uni}{data4} = $data[18];
							${$$tr}{sw_cap}{uni}{lpn} = $data[19];
							${$$tr}{sw_cap}{uni}{eld} = $data[20];
							${$$tr}{sw_cap}{uni}{elu} = $data[21];
							${$$tr}{sw_cap}{uni}{node_name} = $data[22];
							$v = sprintf("\n      (%s)", $data[22]) if Aux::dbg_tedb();
						}
						${$$tr}{status} |= STAT_LINK_SWCAP; 
					}
					Aux::print_dbg_tedb("    sub-level insert: %s%s%s\n", $sub_tlvs_link_X{$$d{type}}, 
						defined($$d{subtype})?"($$d{subtype})":"",
						$v);
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
		my $err = $self->insert_link_blk($dr, $br, $tr, $$d{client}) if defined($$tr);
		Aux::print_dbg_tedb("  }");
		if(!$err) {
			Aux::print_dbg_tedb("  => inserted to %08x (%s) from %s\n", $$br, defined(WS::External::get_rtr_name($$br))?WS::External::get_rtr_name($$br):"unknown", $$d{client});
		}
		$$tr = {"status" => 0};
		$$br = undef; 
		return 1;
	}
}

sub process_tedb_data($) {
	my $self = shift;
	my ($msg)  = @_;
	my $d;

	my $tedb = {};
	my $lblock = undef;
	my $tmp = undef; #temporary storage for the link block data
	my $stat = 0;
	my @tedbs = ();

	# parse the message
	my $tr;  # XML tree reference
	eval {
		$tr = $$self{parser}->parse($msg);
		$d = Lib::xfrm_tree("msg", $$tr[1]);
		if(!defined($d)) {
			Log::log("warning", "IPC message parsing failed\n");
			return;
		}
	};
	if($@) {
		Log::log("err", "$@\n");
		return;
	}

	my $res = $self->process_q($d, $tedb, \$lblock, \$tmp, \$stat);

	if($res == 1) {
		if($stat == 0) {
			# create net graph
			$self->create_graph($tedb);
			eval {
				$self->traverse($tedb);
				@tedbs = $self->split_graph($tedb);
			};
			if($@) {
				Log::log("err", "$@\n");
				Aux::print_dbg_tedb("TEDB1 not updated\n");
			}
			else {
				# loop through all the disconnected graphs
				for(my $i=0; $i<@tedbs; $i++) {
					# first, look at the source
					my $type = $self->get_topo_type($tedbs[$i]);
					if(defined($type)) {
						# transfer the completed TEDBs
						if($type eq "abstract") {
							my $tmp = $$self{abstract_tedb};
							$$self{abstract_tedb} = $tedbs[$i];
							$self->destroy_graph($tmp);
							Aux::print_dbg_tedb("updating abstract TEDB\n");
						}
						elsif($type eq "control") {
							my $tmp = $$self{control_tedb};
							$$self{control_tedb} = $tedbs[$i];
							$self->destroy_graph($tmp);
							Aux::print_dbg_tedb("updating control TEDB\n");
						}
						elsif($type eq "data") {
							my $tmp = $$self{data_tedb};
							$$self{data_tedb} = $tedbs[$i];
							$self->destroy_graph($tmp);
							Aux::print_dbg_tedb("updating data TEDB\n");
						}
					}
					else {
						Log::log("err", "unknown source\n");
						Aux::print_dbg_tedb("TEDB not updated\n");
					}
				}
			}
		}
		else {
			Aux::print_dbg_tedb("TEDB not updated\n");
		}
		$tedb = {};
		$stat = 0;
	}
}

sub run() {
	my $self = shift;
	my %pipe_queue;

	Log::log "info", "starting $$self{name}\n";
	while(!$::ctrlC) {
		Aux::act_on_msg($self, \%pipe_queue);
	}
	Aux::print_dbg_run("exiting $$self{name}\n");
}

1;
