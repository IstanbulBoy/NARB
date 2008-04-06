package Parser;

use strict;
use sigtrap;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Log;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.1 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

my $d = 0; # a global to use in regex embedded command execution

sub process_cfg() {
	my $cfg_s = "";
	my $f = $::cfg{config}{v};
	if(!(-e $f)) {
		if($::cfg{config}{s} eq 'd') {
			return;
		}
		else {
			die("could not find $f\n");
		}
	}
	my $l = 1;
	open(FH, "<", $f) or die "Can't open $f: $!\n";
	while(<FH>) {
		s/#.*$//;
		chomp($_);	
		$cfg_s .= $_;
		$l++;
	}
	close(FH) or die "Can't close $f: $!\n";

	# check for unbalanced brackets
	$cfg_s =~ /^(?{$d=0})((?:(?(?{$d<0})$|{(?{$d++}))|(?(?{$d<0})$|}(?{$d--}))|(?(?{$d<0})$|[^{}]*))*)$/;
	die "syntax error in $f (unbalanced curly)\n" unless $d==0;

	if($cfg_s =~ /daemonize\s*;/) {
		if($::cfg{daemonize}{s} ne 'c') {
			$::cfg{daemonize}{v} = 1;
			$::cfg{daemonize}{s} = 'f';
		}
	}
	if($cfg_s =~ /log\s+(\S+)\s*;/) {
		if($::cfg{log}{s} ne 'c') {
			$::cfg{log}{v} = $1;
			$::cfg{log}{s} = 'f';
		}
	}
	if($cfg_s =~ /dbg\s+(.*?)\s*;/) {
		my @tmp = split(/\s+/, $1);
		if($::cfg{dbg}{s} ne 'c') {
			for(my $j=0; $j<@tmp; $j++) {
				$::cfg{dbg}{v} |= Aux::get_dbg_sys($tmp[$j]);
				$::cfg{dbg}{s} = 'f';
			}
		}
	}

	# get the gmpls configuration
	my @gmpls = $cfg_s =~ /gmpls\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};
	if(@gmpls > 1) {
		die "syntax error in $f (gmpls block can be defined only once)\n";
	}
	elsif(@gmpls == 1) {
		if($gmpls[0] =~ /port\s+(\S+)\s*;/) { 
			if($::cfg{gmpls}{port}{s} ne 'c') {
				$::cfg{gmpls}{port}{v} = $1;
				$::cfg{gmpls}{port}{s} = 'f';
			}
		}
	}

	# get the http configuration
	my @http = $cfg_s =~ /http\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};
	if(@http > 1) {
		die "syntax error in $f (http block can be defined only once)\n";
	}
	elsif(@http == 1) {
		if($http[0] =~ /port\s+(\S+)\s*;/) { 
			if($::cfg{http}{port}{s} ne 'c') {
				$::cfg{http}{port}{v} = $1;
				$::cfg{http}{port}{s} = 'f';
			}
		}
		if($http[0] =~ /root\s+(\S+)\s*;/) { 
			if($::cfg{http}{root}{s} ne 'c') {
				$::cfg{http}{root}{v} = $1;
				$::cfg{http}{root}{s} = 'f';
			}
		}
	}
	# get the ws configuration
	my @ws = $cfg_s =~ /ws\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};
	if(@ws > 1) {
		die "syntax error in $f (ws block can be defined only once)\n";
	}
	elsif(@ws == 1) {
		if($ws[0] =~ /port\s+(\S+)\s*;/) { 
			if($::cfg{ws}{port}{s} ne 'c') {
				$::cfg{ws}{port}{v} = $1;
				$::cfg{ws}{port}{s} = 'f';
			}
		}
		if($ws[0] =~ /wsdl\s+(\S+)\s*;/) { 
			if($::cfg{ws}{wsdl}{s} ne 'c') {
				$::cfg{ws}{wsdl}{v} = $1;
				$::cfg{ws}{wsdl}{s} = 'f';
			}
		}
		if($ws[0] =~ /subnet_config\s+(\S+)\s*;/) { 
			if($::cfg{ws}{subnet_cfg}{s} ne 'c') {
				$::cfg{ws}{subnet_cfg}{v} = $1;
				$::cfg{ws}{subnet_cfg}{s} = 'f';
			}
		}
	}
}

sub parse_cfg($) {
	my ($fn) = @_;
	my $cfg_s = "";
	#remove comments and newlines
	open(FH, "<", $fn) or die "Can't open $fn: $!";
	while(<FH>) {
		if(/^!/) {
			if(/(<\s*ext.*?>)/) {
				$cfg_s .= " $1 "; #the spaces are necessary
			}
			next;
		}
		chomp($_);	
		$cfg_s .= " $_ ";
	}
	close(FH) or die "Can't close $fn: $!";
	# check for unbalanced brackets
	$cfg_s =~ /^(?{$d=0})((?:(?(?{$d<0})$|{(?{$d++}))|(?(?{$d<0})$|}(?{$d--}))|(?(?{$d<0})$|[^{}]*))*)$/;
	die "syntax error in $fn (unbalanced curly)\n" unless $d==0;

	#get the domain id
	my @did = $cfg_s =~ /domain-id\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};

	#get the router blocks and balance the brackets
	my @r = $cfg_s =~ /router\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};
	#get the link blocks and parse them
	for(my $i=0; $i<@r; $i++) {
		my @l = $r[$i] =~ /link\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)/g;
		if(defined(pos)) { pos = 0;};
		$r[$i] =~ s/link\s*({(?{$d=1})(?:{(?{$d++})|(?(?{$d>0})}(?{$d--})|$)|(?(?{$d>0})[^{}]*|$))*)//g;
		if(defined(pos)) { pos = 0;};
		die "syntax error in $fn (missing router id)" unless $r[$i] =~ /id\s+(.*?)\s/;
		my $rtr_id = $1;
		my $rtr_ctrlr = "none";
		my $rtr_ctrlip = "none";
		if($r[$i] =~ /[\s{]home_vlsr\s+(.*?)[\s}]/) {
			$rtr_ctrlr = $1;
		} 
		else {
			die "syntax error in $fn (missing home_vlsr)";
		}
		my $rtr_name = ($r[$i]=~/[\s{]dtl_name\s+(.*?)[\s}]/)?$1:undef;
		my $link_id = "";
		my $port_name = "";
		for(my $j=0; $j<@l; $j++) {
			if($l[$j] =~ /[\s{]id\s+(.*?)[\s}]/) {
				$link_id = $1;
				$port_name = undef;
				if($l[$j] =~ /[\s{]dtl_id\s+(.*?)[\s}]/) {
					$port_name = $1;
				} 
				WS::External::add($rtr_id, $rtr_name, $link_id, $port_name);
			} 
			else {
				die "syntax error in $fn (missing link id)";
			}
		}
	}
}

sub load_configuration($) {
	my ($fn) = @_;
	# test the file
	open(SUB_H, "<", $fn) or die "Can't open $fn: $!";
	close(SUB_H) or die "Can't close $fn: $!";
	Log::log "info", "loading $fn\n",
	parse_cfg($fn);
}

1;
