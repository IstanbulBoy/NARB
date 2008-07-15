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
# July 15, 2008
#
# File: Server.pm
#

package HTTP::Server;

use strict;
use warnings;
use HTTP::Daemon;
use HTTP::Status;

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.1 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter SOAP::Transport::HTTP::Daemon);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

sub new {
	shift;
	my ($fh, $proc, $p)  = @_;
	my $self;
	eval {
		$self = {
			"port" => $p,
			"server"=> new HTTP::Daemon(
				LocalAddr => inet_ntoa(INADDR_ANY),
				LocalPort => $p,
				ReuseAddr => 1,
				Timeout => 5,
				Blocking => 1
			),
			"proc" => $proc,
			"fh" => $fh, 		# IPC pipe
			"select" => new IO::Select($sock) # select handle
		};
	};
	if($@) {
		die "$n: $@\n";
	}
	bless $self;
	return $self;
}

sub run() {
	my $self = shift;
	my $c;
	Log::log "info", "starting $$self{name}\n";
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
	Aux::print_dbg_run("exiting $$self{name}\n");
}

1;
