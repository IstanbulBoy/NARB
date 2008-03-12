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

use Aux;
use Log;
use Server;
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

my $ctrlC = 0;
my @servers:shared = ();

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
	Log::log("info", "caught SIG$signame:");
	kill_servers($signame);
	$ctrlC = 1;
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
			$::cfg{$k1}{v} |= Aux::get_dbg_sys($v);
			$::cfg{$k1}{s} = 'c';
		}
		else {
			$::cfg{$k1}{v} = $v;
			$::cfg{$k1}{s} = 'c';
		}
	}
}

# transfer the configured values to more sensible variables
# r1: daemonize
sub init_cfg($) {
	my ($r1) = @_;
	Aux::init_dbg($::cfg{dbg}{v});
	if(Aux::dbg_cfg()) {
		print("LEGEND: (d) - default value, (f) - file supplied,\n");
		print("        (c) - cli override, (i) - stdin override\n");
		print("------------------------------------------------------------\n");
		Aux::dump_config(\%::cfg);
		print("------------------------------------------------------------\n");
	}
	$$r1 = $::cfg{daemonize}{v};
}

# NOTE: running in a different thread here ...
sub spawn_server($) {
	my ($s) = @_;
	my $srvr = new Server($s);
	my $thr = threads->self();

	#NOTE1 The only values that can be assigned to a shared scalar 
	#      are other scalar values, or shared refs.
	#NOTE2 (in case I wonder later): When share is used on arrays, 
	#      hashes, array refs or hash refs, 
	#      any data they contain will be lost.
	my $tmp:shared = &share({});
	$$tmp{server} = share($srvr);
	$$tmp{thread} = share($thr);
	{
		lock @servers;
		push(@servers, $tmp);
	}
	$srvr->run();
}

sub kill_servers($) {
	my $sig = shift;
	lock @servers;
	foreach my $s (@servers) {
		$$s{server}->term();
		$$s{thread}->join();
	}
	@servers = ();
}

################################################################################
################################################################################
$SIG{TERM} = \&catch_term;
$SIG{INT} = \&catch_term;
$SIG{HUP} = \&catch_term;
$| = 1;

#$::d = 0; # a global to use in regex embedded command execution


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
	Aux::print_dbg_net("listening on port $::cfg{port}{v}");
	while(!$ctrlC) {
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
		threads->create(\&spawn_server, $client_sock);
	}

	$serv_sock->shutdown(SHUT_RDWR);
};
if($@) {
	Log::log("err", $@);
}

Log::close();
