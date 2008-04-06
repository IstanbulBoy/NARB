#!/usr/bin/perl -w
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
# File: terce.pl
#

use lib ("/usr/share/dragon/lib", "./lib");

use threads;
use threads::shared;
use Thread::Queue;

use Aux;
use Log;
use GMPLS::Server;
use WS::Server;
use WS::External;
use strict;
use sigtrap;
use POSIX;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Getopt::Long;
use IO::Socket::INET;

my @servers = ();
$::ctrlC = 0;
$::d = 0; # a global to use in regex embedded command execution

sub usage() {
	print <<USG;
usage: terce [-h] [-d] [-c <config file>] [-l <log file>] [-p <port>]
       -d: daemonize
       -c <config file>: (defaults to /etc/terce/terce.conf)
       -l <log file>: log file (default: /var/log/terce.log)
       -p <port>: listen on this port for narb and rce data
              (default 2690)
       -h: prints this message
  Long options:
       --config <config file>: (defaults to /etc/terce/terce.conf)
       --gmpls_port <port>: listen on this port for narb and rce data
              (default 2690)
       --ws_port <port>: listen on this web services port (default 8080)
       --wsdl <wsdl file>: serve TERCE wsdl file (default undefined)
       --subnet_cfg <subnet config file>: load some additional info 
              from the file 
       --www_port <port>: serve the wsdl file on this port (default 80)
              (only if te wsdl file is specified)
       --www_root <dir>: root of the web server
       --log: <log file> log file (default: /var/log/terce.log)
       --dbg: <list> of components to debug:
              (run config narb rce net api data lsa ws)
       --help: prints this message
USG
	exit;
}

#signal handlers

sub catch_term {
	my $signame = shift;
	Log::log("info", "terminating threads... (SIG$signame)\n");
	$::ctrlC = 1;
}

sub process_opts($$) {
	my ($n, $v) = @_;
	my $k1 = undef;
	my $k2 = undef;
	my $k3 = undef;
	my $is_array = 0;
	if($n eq "d") 				{$k1 = "daemonize";}
	if(($n eq "c") || ($n eq "config")) 	{$k1 = "config";}
	if(($n eq "l") || ($n eq "log")) 	{$k1 = "log";}
	if($n eq "dbg")			 	{$k1 = "dbg";}
	if(($n eq "p") || ($n eq "gmpls_port"))	{$k1 = "gmpls"; $k2 = "port";}
	if($n eq "www_port")			{$k1 = "http"; $k2 = "port"}
	if($n eq "wsdl")			{$k1 = "http"; $k2 = "wsdl"}
	if($n eq "ws_port")			{$k1 = "ws"; $k2 = "port"}
	if($n eq "wsdl")			{$k1 = "ws"; $k2 = "wsdl"}
	if($n eq "subnet_cfg")			{$k1 = "ws"; $k2 = "subnet_cfg"}

	if(defined($k1) && !defined($k2) && !$is_array) {
		if($k1 eq "dbg") {
			$::cfg{$k1}{v} |= Aux::get_dbg_sys($v);
			$::cfg{$k1}{s} = 'c';
		}
		else {
			$::cfg{$k1}{v} = $v;
			$::cfg{$k1}{s} = 'c';
		}
	}
	elsif(defined($k1) && defined($k2) && !$is_array) {
		$::cfg{$k1}{$k2}{v} = $v;
		$::cfg{$k1}{$k2}{s} = 'c';
	}
}

# transfer the configured values to more sensible variables
# r1: daemonize
sub init_cfg($) {
	my ($r1) = @_;
	Aux::set_dbg_sys($::cfg{dbg}{v});
	if(Aux::dbg_cfg()) {
		print("LEGEND: (d) - default value, (f) - file supplied,\n");
		print("        (c) - cli override, (i) - stdin override\n");
		print("------------------------------------------------------------\n");
		Aux::dump_config(\%::cfg);
		print("------------------------------------------------------------\n");
	}
	if(Aux::get_dbg_sys() > 0) {
		Log::log "warning", "running in foreground\n";
		$$r1 = 0;
	}
}

# NOTE: running in a different thread here ...
sub spawn_server($$$) {
	my ($ss, $cs, $tq) = @_;
	my $srvr;
	$ss->close();
	eval {
		$srvr = new GMPLS::Server($cs, $tq);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

sub start_ws_server($) {
	my ($p, $tqin, $tqout) = @_;
	my $srvr;
	eval {
		$srvr = new WS::Server($p, $tqin, $tqout);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

sub cleanup_servers() {
	foreach my $s (@servers) {
		$s->join();
	}
}

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
	$cfg_s =~ /^(?{$::d=0})((?:(?(?{$::d<0})$|{(?{$::d++}))|(?(?{$::d<0})$|}(?{$::d--}))|(?(?{$::d<0})$|[^{}]*))*)$/;
	die "syntax error in $f (unbalanced curly)\n" unless $::d==0;

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
	my @gmpls = $cfg_s =~ /gmpls\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
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
	my @http = $cfg_s =~ /http\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
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
	my @ws = $cfg_s =~ /ws\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
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
	$cfg_s =~ /^(?{$::d=0})((?:(?(?{$::d<0})$|{(?{$::d++}))|(?(?{$::d<0})$|}(?{$::d--}))|(?(?{$::d<0})$|[^{}]*))*)$/;
	die "syntax error in $fn (unbalanced curly)\n" unless $::d==0;

	#get the domain id
	my @did = $cfg_s =~ /domain-id\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};

	#get the router blocks and balance the brackets
	my @r = $cfg_s =~ /router\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
	if(defined(pos)) { pos = 0;};
	#get the link blocks and parse them
	for(my $i=0; $i<@r; $i++) {
		my @l = $r[$i] =~ /link\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)/g;
		if(defined(pos)) { pos = 0;};
		$r[$i] =~ s/link\s*({(?{$::d=1})(?:{(?{$::d++})|(?(?{$::d>0})}(?{$::d--})|$)|(?(?{$::d>0})[^{}]*|$))*)//g;
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

################################################################################
################################################################################
$SIG{TERM} = \&catch_term;
$SIG{INT} = \&catch_term;
$SIG{HUP} = \&catch_term;
$| = 1;
share($::ctrlC);
share($::d);

# configuration parameters hash
# each parameter holds it's value "v" and it's status "s".
#     status ... d: default, 
#     		 f: from config file, 
#     		 c: overriden from the cli
#		 i: from stdin
%::cfg = (
	"daemonize" => 	{
		"v" => 0, 
		"s" => 'd'
	},
	"config" => {
		"v" => "/etc/terce/terce.conf", 
		"s" => 'd'
	},
	"gmpls" => {
		"port" => {
			"v" => "2690", 
			"s" => 'd'
		}
	},
	"http" => {
		"port" => {
			"v" => "80", 
			"s" => 'd'
		},
		"root" => {
			"v" => "./www", 
			"s" => 'd'
		}
	},
	"ws" => {
		"port" => {
			"v" => "8080", 
			"s" => 'd'
		},
		"wsdl" => {
			"v" => undef, 
			"s" => 'd'
		},
		"subnet_cfg" => {
			"v" => undef, 
			"s" => 'd'
		}
	},
	"log" => 	{
		"v" => "/var/log/terce.log", 
		"s" => 'd'
	},
	"dbg" => 	{
		"v" => 0, 
		"s" => 'd'
	}
);

if(!GetOptions ('d' =>			\&process_opts,
		'c=s' =>		\&process_opts,
		'config=s' =>		\&process_opts,
		'p=s' =>		\&process_opts,
		'gmpls_port=s' =>	\&process_opts,
		'www_port=s' =>		\&process_opts,
		'www_root=s' =>		\&process_opts,
		'ws_port=s' =>		\&process_opts,
		'wsdl=s' =>		\&process_opts,
		'subnet_cfg=s' =>	\&process_opts,
		'l=s' =>		\&process_opts,
		'log=s' =>		\&process_opts,
		'dbg:s{,}' =>		\&process_opts,
		'h' => 			\&usage,
		'help' => 		\&usage)) {
	usage();
}

# process the main config file
eval {
	process_cfg();
};	
if($@) {
	Log::log "err", $@;
	Log::log "info", "process terminating";
	print("ERROR: $@\n");
	print("process terminating\n");
	exit;
}

my $daemonize = undef;

init_cfg(\$daemonize);

eval {
	#load_configuration($::cfg{ws}{subnet_cfg}{v});
};
if($@) {
	$::cfg{ws}{subnet_cfg}{v} = undef;	
}

if(!defined($::cfg{ws}{subnet_cfg}{v})) {
	Log::log "warning", "WARNING: subnet config file was not loaded\n";
	Log::log "warning", "WARNING: some data such as router names will be generated\n";
}


if($daemonize) {
	Log::open(Log::FILE, "info warning err", $::cfg{log}{v});
}
else {
	Log::open(Log::FILE | Log::OUT, "info warning err", $::cfg{log}{v});
}

if($daemonize) {
	my $pid = fork;
	exit if $pid;
	die "Couldn't fork: $!" unless defined($pid);
	for my $handle (*STDIN, *STDOUT, *STDERR) {
		open($handle, "+<", "/dev/null")
		|| die "can't reopen $handle to /dev/null: $!";
	}

	use POSIX;

	POSIX::setsid( )
		or die "Can't start a new session: $!";
}
eval {
	# start TEDB thread
	my $tqin = new Thread::Queue;
	my $tqout = new Thread::Queue;

	# start the HTTP server
	#if(0) {
#		my $sw_server = threads->create(\&start_ws_server, $::cfg{ws}{port}{v}, $tqin, $tqout);
#		if($sw_server) {
#			push(@servers, $sw_server);
#		}
#	}

	# start the SOAP/HTTP server
	my $ws_server = threads->create(\&start_ws_server, $::cfg{ws}{port}{v}, $tqin, $tqout);
	if($ws_server) {
		push(@servers, $ws_server);
	}

	my $serv_sock = IO::Socket::INET->new(Listen => 5,
		LocalAddr => inet_ntoa(INADDR_ANY),
		LocalPort => $::cfg{gmpls}{port}{v},
		ReuseAddr => 1,
		Blocking => 1,
		Proto     => 'tcp') or die "socket: $@\n";
	Aux::print_dbg_net("listening on port $::cfg{gmpls}{port}{v}\n");
	while(!$::ctrlC) {
		my @conn = $serv_sock->accept();
		if(!@conn) {
			next;
		}
		my $client_sock = $conn[0];
		my @tmp = sockaddr_in($conn[1]);
		my $peer_ip = inet_ntoa($tmp[1]);
		my $peer_port = $tmp[0];
		Log::log("info", "accepted connection from $peer_ip:$peer_port\n");
		#spawn a new server thread
		my $thr = threads->create(\&spawn_server, $serv_sock, $client_sock, $tqin);
		$client_sock->close();
		if($thr) {
			push(@servers, $thr);
		}
	}
	cleanup_servers();

	$serv_sock->shutdown(SHUT_RDWR);
};
if($@) {
	Log::log("err", "$@\n");
}

Log::close();
