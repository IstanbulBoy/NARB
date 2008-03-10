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
# File: sext.pl
#

use lib ("/usr/share/dragon/lib", "./lib");

use Log;
use strict;
use sigtrap;
use POSIX;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Getopt::Long;
use threads;
use threads::shared;
use IO::Socket::INET;

sub dump_config($;$);

sub usage() {
	print("usage: sext [-h] [-d] [-p <port>]\n");
	print("       -d: daemonize\n");
	print("       -p <port>: listen on this port (default 2690)\n");
	print("       -h: prints this message\n");
	print("  Long options:\n");
	print("       --port <port>: listen on this port (default 2690)\n");
	print("       --log: <log file> log file (default: /var/log/sext.log)\n");
	print("       --dbg: <list> of components to debug (run config narb rce net)\n");
	print("              with no agrument, all the components will become very verbose\n");
	print("              and the program will not fork.\n");
	print("       --help: prints this message\n");
	exit;
}

#signal handlers

sub catch_term {
	my $signame = shift;
	Log::log "info", "caught SIG$signame:";
	$::ctrlC = 1;
}

sub print_dbg($$;@) {
	my ($sys, $msg, @args) = @_;
	if($::dbg_sys & (1<<$sys)) {
		$msg = sprintf($msg, @args);
		Log::log "info",  $msg;
	}
}


sub process_opts($$) {
	my ($n, $v) = @_;
	my $k1 = undef;
	my $k2 = undef;
	my $k3 = undef;
	my $is_array = 0;
	if($n eq "d") 				{$k1 = "daemonize";}
	if(($n eq "c") || ($n eq "config")) 	{$k1 = "config";}
	if(($n eq "p") || ($n eq "port")) 	{$k1 = "port";}
	if(($n eq "l") || ($n eq "log")) 	{$k1 = "log";}
	if($n eq "dbg")			 	{$k1 = "dbg";}

	if(defined($k1) && !defined($k2) && !$is_array) {
		if($k1 eq "dbg") {
			$::cfg{$k1}{v} |= ($v eq "run")?(1<<$::RUN_DBG):
						$v eq "config"?(1<<$::CFG_DBG):
						$v eq "narb"?(1<<$::NARB_DBG):
						$v eq "rce"?(1<<$::RCE_DBG):
						$v eq "net"?(1<<$::NET_DBG):
						$v eq "api"?(1<<$::API_DBG):0
						;
			$::cfg{$k1}{s} = 'c';
		}
		else {
			$::cfg{$k1}{v} = $v;
			$::cfg{$k1}{s} = 'c';
		}
	}
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

# transfer the configured values to more sensible variables
# r1: daemonize
sub init_cfg($) {
	my ($r1) = @_;
	if($::cfg{dbg}{v} & (1<<$::CFG_DBG)) {
		print("LEGEND: (d) - default value, (f) - file supplied,\n");
		print("        (c) - cli override, (i) - stdin override\n");
		print("------------------------------------------------------------\n");
		dump_config(\%::cfg);
		print("------------------------------------------------------------\n");
	}
	$$r1 = $::cfg{daemonize}{v};
	$::dbg_sys = $::cfg{dbg}{v};
}

sub get_msg($) {
	my ($s) = @_;
	#get the header
	my $hdr;
	if(defined($s->recv($hdr, 24, 0))) {
		my ($type, $action, $len, $ucid, $sn, $chksum, $tag1, $tag2) = unpack("CCSNNNNN", $hdr);	
		print_dbg($::API_DBG, "----------------------- header -----------------------\n");
		print_dbg($::API_DBG, "type action length:\t0x%02X (%u)  0x%02X (%u)  0x%04X (%u)\n", $type, $type, $action, $action, $len, $len);
		print_dbg($::API_DBG, "ucid:\t\t\t0x%08X (%u)\n", $ucid, $ucid);
		print_dbg($::API_DBG, "seq. number:\t\t0x%08X (%u)\n", $sn, $sn);
		print_dbg($::API_DBG, "checksum:\t\t0x%08X (%u)\n", $chksum, $chksum);
		print_dbg($::API_DBG, "tag1:\t\t\t0x%08X (%u)\n", $tag1, $tag1);
		print_dbg($::API_DBG, "tag2:\t\t\t0x%08X (%u)\n", $tag2, $tag2);
		print_dbg($::API_DBG, "------------------------------------------------------\n");
	}
	else {
		Log::log "err", "recv: $@\n";
	}
}

################################################################################
################################################################################
$SIG{TERM} = \&catch_term;
$SIG{INT} = \&catch_term;
$SIG{HUP} = \&catch_term;
$::ctrlC = 0;
$| = 1;
#$::d = 0; # a global to use in regex embedded command execution

$::RUN_DBG = 0;
$::CFG_DBG = 1;
$::NARB_DBG = 2;
$::RCE_DBG = 3;
$::NET_DBG = 4;
$::API_DBG = 5;

$::RCE_MSG_TERCE_TOPO_SYNC = 0x11;
$::RCE_MSG_TERCE_TOPO_ASYNC = 0x12;
$::ACT_QUERY = 0x01;
$::ACT_INSERT = 0x02;
$::ACT_DELETE = 0x03;
$::ACT_UPDATE = 0x04;
$::ACT_ACK = 0x05;
$::ACT_ACKDATA = 0x06;
$::ACT_ERROR = 0x07;
$::ACT_INIT = 0x0A;
$::ACT_ALIVE = 0x0B;

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
		"v" => "/etc/dcn/sext.conf", 
		"s" => 'd'
	},
	"port" => {
		"v" => "2690", 
		"s" => 'd'
	},
	"log" => 	{
		"v" => "/var/log/sext.log", 
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
		'port=s' =>		\&process_opts,
		'l=s' =>		\&process_opts,
		'log=s' =>		\&process_opts,
		'dbg:s{,}' =>		\&process_opts,
		'h' => 			\&usage,
		'help' => 		\&usage)) {
	usage();
}

my $daemonize = undef;
$::dbg_sys = undef;

init_cfg(\$daemonize);

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
	my $serv_sock = IO::Socket::INET->new(Listen => 5,
		LocalAddr => inet_ntoa(INADDR_ANY),
		LocalPort => $::cfg{port}{v},
		ReuseAddr => 1,
		Blocking => 1,
		Proto     => 'tcp') or die "socket: $@\n";
	print_dbg($::NET_DBG, "listening on port $::cfg{port}{v}");
	while(!$::ctrlC) {
		my @conn = $serv_sock->accept();
		if(!@conn) {
			next;
		}
		my $client_sock = $conn[0];
		my @tmp = sockaddr_in($conn[1]);
		my $peer_ip = inet_ntoa($tmp[1]);
		my $peer_port = $tmp[0];
		Log::log "info", "accepted connection from $peer_ip:$peer_port\n";
		#examine the message and create appropriate thread 
		get_msg($client_sock);
	}

	$serv_sock->shutdown(SHUT_RDWR);
};
if($@) {
	Log::log "err", $@;
	print("ERROR: $@\n");
}

Log::close();
