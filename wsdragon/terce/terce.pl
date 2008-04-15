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

use lib ("$ENV{HOME}/dragon/lib", "./lib");

use threads;
use threads::shared;
use Thread::Queue;

use Aux;
use Log;
use Parser;
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
use HTTP::Daemon;
use HTTP::Status;

my @servers = ();
%::cfg = ();
$::ctrlC = 0;

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
              NOTE: its location is relative to www_root
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
	Log::log("info", "   (http server can take up to 10s to exit)\n");
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
	$$r1 = $::cfg{daemonize}{v};
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
	if(defined($::cfg{http}{root}{v})) {
		eval {
			if(!(-e $::cfg{http}{root}{v} && -d $::cfg{http}{root}{v} && -r $::cfg{http}{root}{v})) {
				die "http root problem\n";
			}
		};
		if($@) {
			$::cfg{http}{root}{v} = undef;
			$::cfg{ws}{wsdl}{v} = undef;
			Log::log "warning", "$@\n",
			Log::log "warning", "HTTP server will not be started\n",
			return;
		}
	}
	if(defined($::cfg{ws}{wsdl}{v})) {
		my $wsdl = join("/", $::cfg{http}{root}{v}, $::cfg{ws}{wsdl}{v});
		eval {
			open(SUB_H, "<", $wsdl) or die "Can't open $wsdl: $!\n";
			close(SUB_H) or die "Can't close $wsdl: $!\n";
		};
		if($@) {
			$::cfg{http}{root}{v} = undef;
			$::cfg{ws}{wsdl}{v} = undef;
			Log::log "warning", "$@\n",
			Log::log "warning", "HTTP server will not be started\n",
		}
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

sub start_ws_server($$$) {
	my ($p, $tq1, $tq2) = @_;
	my $srvr;
	Log::log "info",  "starting ws server on port $p\n";
	eval {
		$srvr = new WS::Server($p, $tq1, $tq2);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

sub start_http_server() {
	my $http = new HTTP::Daemon(
		LocalAddr => inet_ntoa(INADDR_ANY),
		LocalPort => $::cfg{http}{port}{v},
		ReuseAddr => 1,
		Timeout => 5,
		Blocking => 1
	);
	Log::log "info",  "starting HTTP server on port $::cfg{http}{port}{v}\n";
	my $c;
	while(!$::ctrlC) {
		while ($c = $http->accept) {
			while (my $r = $c->get_request) {
				if ($r->method eq 'GET') {
					my $f;
					if($r->uri eq "/") {
						$f = $::cfg{http}{root}{v}."/index.html";
					}
					else {
						$f = $::cfg{http}{root}{v}.$r->uri;
					}
					if(-d $f) {
						$c->send_error(RC_FORBIDDEN);
					}
					elsif(-e $f && -r $f) {
						if($f =~ /(\.[Ww][Ss][Dd][Ll]\s*$|[Xx][Ss][Dd]\s*$)/) {
							my $code = RC_OK;
							my $msg = "";
							my $content = "";
							eval {
								open(FH, "<", $f) or die "Can't open $f: $!\n";
								while(<FH>) {
									$content .= $_;
								}
								close(FH) or die "Can't close $f: $!\n";
							};
							if($@) {
								Log::log "warning", "wsdl file reading error: $@\n";
								$c->send_error(RC_BAD_REQUEST);
							}
							else {
								my $h = new HTTP::Headers();
								$h->header('Content-Type' => 'text/xml');
								my $res = new HTTP::Response( $code, $msg, $h, $content );
								$c->send_response($res);
							}
						}
						else {
							$c->send_file_response($f);
						}
					}
					else {
						$c->send_error(RC_NOT_FOUND);
					}
				}
				else {
					$c->send_error(RC_METHOD_NOT_ALLOWED);
				}
			}
		}
	}
	$c->close if defined;
	undef($c);
	Aux::print_dbg_run("exiting http server thread\n");
}

sub cleanup_servers() {
	foreach my $s (@servers) {
		$s->join();
	}
}


################################################################################
################################################################################
$SIG{TERM} = \&catch_term;
$SIG{INT} = \&catch_term;
$SIG{HUP} = \&catch_term;
$| = 1;
share($::ctrlC);
share(%::cfg);

# configuration parameters hash
# each parameter holds it's value "v" and it's status "s".
#     status ... d: default, 
#     		 f: from config file, 
#     		 c: overriden from the cli
#		 i: from stdin
$::cfg{daemonize} = &share({});
$::cfg{daemonize}{v} = 0;
$::cfg{daemonize}{s} = 'd';

$::cfg{config} = &share({});
$::cfg{config}{v} = "/etc/terce/terce.conf";
$::cfg{config}{s} = 'd';

$::cfg{gmpls} = &share({});
$::cfg{gmpls}{port} = &share({});
$::cfg{gmpls}{port}{v} = "2690";
$::cfg{gmpls}{port}{s} = 'd';

$::cfg{http} = &share({});
$::cfg{http}{port} = &share({});
$::cfg{http}{port}{v} = "80";
$::cfg{http}{port}{s} = 'd';
$::cfg{http}{root} = &share({});
$::cfg{http}{root}{v} = "./www";
$::cfg{http}{root}{s} = 'd';

$::cfg{ws} = &share({});
$::cfg{ws}{port} = &share({});
$::cfg{ws}{port}{v} = "8080";
$::cfg{ws}{port}{s} = 'd';
$::cfg{ws}{wsdl} = &share({});
$::cfg{ws}{wsdl}{v} = undef;
$::cfg{ws}{wsdl}{s} = 'd';
$::cfg{ws}{subnet_cfg} = &share({});
$::cfg{ws}{subnet_cfg}{v} = undef;
$::cfg{ws}{subnet_cfg}{s} = 'd';

$::cfg{log} = &share({});
$::cfg{log}{v} = "/var/log/terce.log";
$::cfg{log}{s} = 'd';

$::cfg{dbg} = &share({});
$::cfg{dbg}{v} = 0;
$::cfg{dbg}{s} = 'd';

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

# tmp logging facility
Log::open(Log::OUT, "info warning err");

# NOTE: the following code is a workaround for perl/libc bug which causes
# a complex rexeg engine state incorrect freeing of the allocated memory
# cloned across multiple threads. To prevent the crash, all complex regex
# expression are run in a separate thread (async block)so they don't get cloned to other
# threads. As a result the main $::cfg structure had to be made shared.

# process the main config file
eval {
	my $th = async{Parser::process_cfg_wrapper();};
	die "cfg" if($th->join());
};	
if($@) {
	Log::log "err", $@;
	Log::log "info", "process terminating\n";
	exit;
}


my $daemonize = undef;
init_cfg(\$daemonize);

if(defined($::cfg{ws}{subnet_cfg}{v})) {
	eval {
		my $th = async{Parser::load_config_wrapper($::cfg{ws}{subnet_cfg}{v});};
		die "cfg" if($th->join());
	};
	if($@) {
		$::cfg{ws}{subnet_cfg}{v} = undef;	
	}
}

if(!defined($::cfg{ws}{subnet_cfg}{v})) {
	Log::log "warning", "WARNING: subnet config file was not loaded\n";
	Log::log "warning", "WARNING: some data such as router names will be generated\n";
}
Log::close();

# reopen logging 
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
	my $tq1 = new Thread::Queue;
	my $tq2 = new Thread::Queue;
	my @tqs = ($tq1, $tq2);

	# start the HTTP server
	if(defined($::cfg{http}{root}{v}) && defined($::cfg{ws}{wsdl}{v})) {
		my $http_server = threads->create(\&start_http_server);
		if($http_server) {
			push(@servers, $http_server);
		}
	}

	# start the SOAP/HTTP server
	my $ws_server = threads->create(\&start_ws_server, $::cfg{ws}{port}{v}, $tq1, $tq2);
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
		my $thr = threads->create(\&spawn_server, $serv_sock, $client_sock, shift(@tqs));
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
