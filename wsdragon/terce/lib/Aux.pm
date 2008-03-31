package Aux;

use strict;
use sigtrap;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Log;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.4 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub dump_config($;$);

use constant RUN_DBG => 0;
use constant CFG_DBG => 1;
use constant NARB_DBG => 2;
use constant RCE_DBG => 3;
use constant NET_DBG => 4;
use constant API_DBG => 5;
use constant DATA_DBG => 6;
use constant LSA_DBG => 7;
use constant RAW_DBG => 8;

my $dbg_sys = undef;

sub set_dbg_sys($) {
	my ($v) = @_;
	$dbg_sys = $v;
}

sub add_dbg_sys($) {
	my ($v) = @_;
	$dbg_sys |= (1 << $v);
}

sub get_dbg_sys($) {
	my ($v) = @_;
	return(	$v eq "run")?	(1 << RUN_DBG):
		$v eq "config"?	(1 << CFG_DBG):
		$v eq "narb"?	(1 << NARB_DBG):
		$v eq "rce"?	(1 << RCE_DBG):
		$v eq "net"?	(1 << NET_DBG):
		$v eq "api"?	(1 << API_DBG):
		$v eq "data"?	(1 << DATA_DBG):
		$v eq "lsa"?	(1 << LSA_DBG):0
		;
}

sub dbg_cfg() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << CFG_DBG));
	}
	return 0;
}

sub dbg_api() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << API_DBG));
	}
	return 0;
}

sub dbg_data() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << DATA_DBG));
	}
	return 0;
}

sub dbg_lsa() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << LSA_DBG));
	}
	return 0;
}

sub dbg_raw() {
	if(defined($dbg_sys)) {
		return ($dbg_sys &(1 << RAW_DBG));
	}
	return 0;
}

sub print_dbg($;@) {
	my ($sys, $msg, @args) = @_;
	if(!defined($dbg_sys)) {
		return;
	}
	if($dbg_sys & (1<<$sys)) {
		$msg = sprintf($msg, @args);
		Log::log("info",  $msg);
	}
}

sub print_dbg_run($;@) {
	 print_dbg(RUN_DBG, @_);
}

sub print_dbg_net($;@) {
	 print_dbg(NET_DBG, @_);
}

sub print_dbg_api($;@) {
	print_dbg(API_DBG, @_);
}

sub print_dbg_data($;@) {
	if(!dbg_lsa()) {
		print_dbg(DATA_DBG, @_);
	}
}

sub print_dbg_lsa($;@) {
	print_dbg(LSA_DBG, @_);
}

sub dump_config($;$) {
	my ($hr,$u) = @_;
	foreach my $k (sort(keys %$hr)) {
		if((ref($$hr{$k}) eq "HASH") && exists($$hr{$k}{v}) && $$hr{$k}{s}) {
			if(defined($$hr{$k}{v})) {
				printf("%s$k($$hr{$k}{s}):\t\t$$hr{$k}{v}\n", defined($u)?"$u:":"");
			}
			else {
				printf("%s$k($$hr{$k}{s}):\t\tundefined\n", defined($u)?"$u:":"");
			}
		}
		elsif(ref($$hr{$k}) eq "ARRAY") {
			for(my $i=0; $i<@{$$hr{$k}}; $i++) {
				dump_config(${$$hr{$k}}[$i], "$i: $k");
			}
		}
		else {
			dump_config($$hr{$k}, defined($u)?"$u:$k":$k);
		}
	}
}

sub chksum($$@) {
	my ($ppat, $upat, @d) = @_;
	my $block = pack($ppat, @d);
	my $chksum = unpack($upat, $block);
	return $chksum;
}

1;
