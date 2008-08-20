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

use Socket;
use IO::Handle;
use IO::Select;
use IO::Socket::INET;

use Aux;
use Log;
use Parser;
use GMPLS::Core;
use GMPLS::Server;
use GMPLS::Client;
use WS::Server;
use WS::External;
use HTTP::Server;
use strict;
use sigtrap;
use POSIX;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';
use Getopt::Long;

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
       --narb_sport <port>: expect connection from this narb source port
              (default 2692)
       --rce_sport <port>: expect connection from this rce source port
              (default 2694)
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
              (run config narb rce net api data lsa ws msg)
       --help: prints this message
USG
	exit;
}

my %child_map;

#signal handlers
sub catch_term {
	my $signame = shift;
	Log::log("info", "terminating ... (SIG$signame)\n");
	$::ctrlC = 1;
}

sub grim {
	my $child;
	while ((my $waitedpid = waitpid(-1,WNOHANG)) > 0) {
		# error on exit usually means error while instantiated => exit process
		if($?) {
			$::ctrlC = 1;
		}
		# let's check what died
		else {
			foreach my $k (keys %child_map) {
				if((ref($k) eq "SCALAR") && ($child_map{$k}{cpid} == $waitedpid)) {
					if(
						($k == ADDR_GMPLS_CORE) || 
						($k == ADDR_SOAP_S) || 
						($k == ADDR_WEB_S)) { # fatal
						$::ctrlC = 1;
					}
					# probably narb/rce crashed
					elsif($k == ADDR_GMPLS_NARB_S) {
						# kill the client too
						if(exists($child_map{ADDR_GMPLS_NARB_C})) {
							my $fh = $child_map{ADDR_GMPLS_NARB_C}{fh};
							my $pid = $child_map{ADDR_GMPLS_NARB_C}{cpid}; 
							delete $child_map{ADDR_GMPLS_NARB_C};
							delete $child_map{$fh};
							kill 15, $pid;
						}
					}
					elsif($k == ADDR_GMPLS_NARB_C) {
						# kill the server too
						if(exists($child_map{ADDR_GMPLS_NARB_S})) {
							my $fh = $child_map{ADDR_GMPLS_NARB_S}{fh};
							my $pid = $child_map{ADDR_GMPLS_NARB_S}{cpid}; 
							delete $child_map{ADDR_GMPLS_NARB_S};
							delete $child_map{$fh};
							kill 15, $pid;
						}
					}
					elsif($k == ADDR_GMPLS_RCE_S) {
						# kill the client too
						if(exists($child_map{ADDR_GMPLS_RCE_C})) {
							my $fh = $child_map{ADDR_GMPLS_RCE_C}{fh};
							my $pid = $child_map{ADDR_GMPLS_RCE_C}{cpid}; 
							delete $child_map{ADDR_GMPLS_RCE_C};
							delete $child_map{$fh};
							kill 15, $pid;
						}
					}
					elsif($k == ADDR_GMPLS_RCE_C) {
						# kill the server too
						if(exists($child_map{ADDR_GMPLS_RCE_S})) {
							my $fh = $child_map{ADDR_GMPLS_RCE_S}{fh};
							my $pid = $child_map{ADDR_GMPLS_RCE_S}{cpid}; 
							delete $child_map{ADDR_GMPLS_RCE_S};
							delete $child_map{$fh};
							kill 15, $pid;
						}
					}
				}
			}
		}
		Aux::print_dbg_run("reaped $waitedpid with exit $?\n" ) if $? != 0;
	}
	$SIG{CHLD} = \&grim;
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
	if($n eq "narb_sport")			{$k1 = "gmpls"; $k2 = "narb_sport";}
	if($n eq "rce_sport")			{$k1 = "gmpls"; $k2 = "rce_sport";}
	if($n eq "www_port")			{$k1 = "http"; $k2 = "port";}
	if($n eq "wsdl")			{$k1 = "http"; $k2 = "wsdl";}
	if($n eq "ws_port")			{$k1 = "ws"; $k2 = "port";}
	if($n eq "wsdl")			{$k1 = "ws"; $k2 = "wsdl";}
	if($n eq "subnet_cfg")			{$k1 = "ws"; $k2 = "subnet_cfg";}

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

######################################################################
###################### child process entry points ####################
######################################################################

# GMPLS Server listens to either narb or rce 
sub start_gmpls_server($$) {
	my ($proc, $cs) = @_;
	my $srvr;
	eval {
		$srvr = new GMPLS::Server($proc, $cs);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

# GMPLS Client provides the async interface to narb and rce
sub start_gmpls_client($) {
	my ($proc) = @_;
	my $client;
	eval {
		$client = new GMPLS::Client($proc);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$client->run();
	}
}

# GMPLS Core provides all the TEDB-related processing
sub start_gmpls_core($) {
	my ($proc) = @_;
	my $core;
	eval {
		$core = new GMPLS::Core($proc);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$core->run();
	}
}

# WS Server handles external web services requests
sub start_ws_server($$) {
	my ($proc, $p) = @_;
	my $srvr;
	eval {
		$srvr = new WS::Server($proc, 
					LocalAddr => inet_ntoa(INADDR_ANY),
					LocalPort => $p,
					ReuseAddr => 1,
					Blocking => 1
				);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

# HTTP Server is a simple web server (wsdl, description, etc.)
sub start_http_server($$) {
	my ($proc, $p) = @_;
	my $srvr;
	eval {
		$srvr = new HTTP::Server($proc, $p);
	};
	if($@) {
		Log::log "err",  "$@\n";
	}
	else {
		$srvr->run();
	}
}

################################################################################
#############################  main loop #######################################
################################################################################
$SIG{TERM} = \&catch_term;
$SIG{INT} = \&catch_term;
$SIG{HUP} = \&catch_term;
$SIG{CHLD} = \&grim;
$| = 1;
$::ctrlC = 0;
my $status = 0;

use constant TERCE_STAT_NARB_CONN => (1<<0);
use constant TERCE_STAT_RCE_CONN => (1<<1);
use constant TERCE_STAT_INIT_DONE => (TERCE_STAT_NARB_CONN | TERCE_STAT_NARB_CONN);

# configuration parameters hash
# each parameter holds it's value "v" and it's status "s".
#     status ... d: default, 
#     		 f: from config file, 
#     		 c: overriden from the cli
#		 i: from stdin
%::cfg = (
	"daemonize" => {
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
		},
		"narb_sport" => {
			"v" => "2692",
			"s" => 'd'
		},
		"rce_sport" => {
			"v" => "2694",
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
	"log" => {
		"v" => "/var/log/terce.log",
		"s" => 'd'
	},
	"dbg" => {
		"v" => 0,
		"s" => 'd'
	}
);

if(!GetOptions ('d' =>			\&process_opts,
		'c=s' =>		\&process_opts,
		'config=s' =>		\&process_opts,
		'p=s' =>		\&process_opts,
		'gmpls_port=s' =>	\&process_opts,
		'narb_sport=s' =>	\&process_opts,
		'rce_sport=s' =>	\&process_opts,
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

# process the main config file
eval {
	Parser::process_cfg();
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
		Parser::load_configuration($::cfg{ws}{subnet_cfg}{v});
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

########################### initial setup ############################
eval {
	my %proc_queue;
	my $sel = new IO::Select();


	# start the GMPLS Processor Core
	Aux::spawn(\%child_map, \$sel, \&start_gmpls_core, "GMPLS Core", ADDR_GMPLS_CORE, undef);

	# start the HTTP server
	if(defined($::cfg{http}{root}{v}) && defined($::cfg{ws}{wsdl}{v})) {
		Aux::spawn(\%child_map, \$sel, \&start_http_server, "Web Server", ADDR_WEB_S, undef);
	}

	# start the SOAP/HTTP server
	Aux::spawn(\%child_map, \$sel, \&start_ws_server, "SOAP Server", ADDR_SOAP_S, undef, $::cfg{ws}{port}{v});

	# wait for GMPLS connections 
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

		my $n = "";
		if($peer_port eq $::cfg{gmpls}{narb_sport}{v}) {
			$n = "narb";
			$status |= TERCE_STAT_NARB_CONN;
		}
		elsif($peer_port eq $::cfg{gmpls}{rce_sport}{v}) {
			$n = "rce";
			$status |= TERCE_STAT_RCE_CONN;
		}
		else {
			Log::log "err", "narb/rce client is not using a known source port ($peer_port)\n";
			Log::log "err", "shutting down the client connection\n";
			$client_sock->shutdown(SHUT_RDWR);
			next;
		}
		# start uninitialized client queue
		Aux::spawn(\%child_map, \$sel, \&start_gmpls_client, "Client Queue ($n)", ($n eq "narb")?ADDR_GMPLS_NARB_C:ADDR_GMPLS_RCE_C, undef);

		# start GMPLS server
		Aux::spawn(\%child_map, \$sel, \&start_gmpls_server, "GMPLS Server ($n)", ($n eq "narb")?ADDR_GMPLS_NARB_S:ADDR_GMPLS_RCE_S, undef, $client_sock);
		if(($status & TERCE_STAT_INIT_DONE) == TERCE_STAT_INIT_DONE) {
			last;
		}
	}
	
	# start the msg relay loop 
	# ASSUMPTIONS (so we can be reentrant):
	#     1. a source will send only complete messages to a single destination
	#         (no multiplexing of partial messages)
	#     2. the relay will forward only full messages
	#     => if we get an incomplete message from a src we can come back
	#        and continue to build the same queue when more data is available
	Aux::print_dbg_run("entering message relay loop\n") if !$::ctrlC;

	# terce self descriptor NOTE: this one is not an object
	my $self;
	eval {
		$self = {
			# process descriptor:
			"proc" => \%child_map, # process info
			"addr" => ADDR_TERCE, # process IPC address
			"name" => "TERCE", # process name
			"fh" => undef, # many handles available 
			"pool" => [], # empty
			"select" => $sel, # corresponding select object
			"parser" => new XML::Parser(Style => "tree"), # incomming data parser
			"processor" => \&Aux::receive_msg, # data processor
		};
	};
	if($@) {
		die "TERCE instantiation failed: $@\n";
	}

	# msg router
	while(!$::ctrlC) {
		Aux::act_on_msg($self, \%proc_queue);
	}
	$serv_sock->shutdown(SHUT_RDWR);
};
if($@) {
	Log::log("err", "$@\n");
}

Log::close();
