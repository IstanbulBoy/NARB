package Log;

use strict;
use sigtrap;
use FileHandle;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Fcntl ':flock';

BEGIN {
	use Exporter   ();
	our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);
	$VERSION = sprintf "%d.%03d", q$Revision: 1.3 $ =~ /(\d+)/g;
	@ISA         = qw(Exporter);
	@EXPORT      = qw();
	%EXPORT_TAGS = ();
	@EXPORT_OK   = qw();
}
our @EXPORT_OK;

# Flags to Log::open specyfing where the output should go.
use constant SYSLOG => (2**0);
use constant ERR    => (2**1);
use constant OUT    => (2**2);
use constant FILE   => (2**3);

# levels of debug messages allowed, log all by default
use constant N => 32;

# mask used when logging (init from syslog)
my $syslog_mask = setlogmask(0);

# submask used only for debug messages (default to log all)
my $debug_mask = "";
vec($debug_mask, 0, N) = (2**N-1);

# log destination (default to STDERR)
my $log_dest = ERR;

# log file handle
my $log_fh;


# Description: 
#       Initialize logging, set log mask and log destinations.
# Arguments:
#	$dest	a bitmask describing where log output will go to, must be one
#		or more out of SYSLOG, ERR, OUT and FILE, or-ed together
#	$mask	which messages to print to the log later, must be a string
#		consisting of one or more of emerg, alert, crit, err, warning,
#		notice, info and debug0 ... debug128, separated by non-word
#		delimiters (if undef, log all messages)
#	$file	if $dest & FILE, this is the file name output will be appended
#		to, need not be passes otherwise
# Returns:
#	1 on success
# Exceptions:
#       "Log::open" on failure
sub open($;$$) {
  my ($mask, $file);
  ($log_dest, $mask, $file) = @_;
  # set mask value(s)
  $syslog_mask = setlogmask(0);
  if(defined $mask) {
    # if a mask was passed, clear mask
    vec($debug_mask, 0, N) = 0;
    foreach my $i (split /\W+/, $mask) {
      if($i =~ /(\D+)(\d+)?/) { 
	# for each level in the mask, set the corresponding bit
	my $sublevel = ($2 ? $2 : 0);
	$syslog_mask |= Sys::Syslog::LOG_MASK(Sys::Syslog::xlate($1));
	# if we have a debug sublevel, add it to the mask
	if($sublevel) { vec($debug_mask, $sublevel, 1) = 1; }
      } else { die "Log::open"; }
    }
  } else { vec($debug_mask, 0, N) = (2**N-1); }
  # if we're logging to syslog, initialize it
  if($log_dest & SYSLOG) { 
    setlogsock('unix');
    openlog "sext", "pid,cons", "user";
    setlogmask $syslog_mask; 
  }
  # if we are (also) logging to as file, open it
  if($log_dest & FILE) { 
    if($file) { 
      $log_fh = new FileHandle;
      open $log_fh, ">>$file" or die "Log::open"; 
      $log_fh->autoflush;
    } else { die "Log::open"; }
  }
  return 1;
}


# Description: 
#       Close log(s).
# Arguments:
#	None.
# Returns:
#	1 on success
# Exceptions:
#       "Log::close" on failure
sub close () {
  # close syslog and log file, if we were using them
  if($log_dest & SYSLOG) { closelog; }
  if($log_dest & FILE) { close $log_fh or die "Log::close"; }
  return 1;
}


# Description: 
#       Write a printf style message to the logs, if $level matches the mask
#	passed to Log::open before.
# Arguments:
#	$level	level of the message, must be a string consisting of one out 
#		of emerg, alert, crit, err, warning, notice, info and 
#		debug0 ... debug128
#	@args   a printf-style array containing the message to be logged
# Returns:
#	1 on success
# Exceptions:
#       "Log::log" on failure
sub log ($@) {
  my ($level, @args) = @_;
  my $sublevel = 0;
  my $at = $@;
  unless(defined $level and defined $args[0]) { die "Log::log"; }
  # if we have one of our homegrown debug levels to log, grab the sublevel
  if($level =~ /debug(\d+)/) { ($level, $sublevel) = ("debug", $1); }
  # are we suppposed to print this?
  if(not defined $syslog_mask or 
     $syslog_mask & Sys::Syslog::LOG_MASK Sys::Syslog::xlate $level and
     not $sublevel or vec($debug_mask, $sublevel, 1)) {
    # ok, print it to log(s)
    if($log_dest & SYSLOG) { syslog $level, @args; }
    # now that we have syslogged, feel free to clobber @args
    my $msg = ($#args ? sprintf shift @args, @args : $args[0]);
    chomp $msg;
    if($log_dest & ERR) { print STDERR "$msg\n"; }
    if($log_dest & OUT) { print STDOUT "$msg\n"; }
    # if we print to a file, add syslog-like timestamp
    $msg = scalar(localtime) . " $0\[$$\]: $msg\n";
    if($log_dest & FILE) { 
      flock $log_fh, LOCK_EX or die "Log::log";
      print $log_fh $msg; 
      flock $log_fh, LOCK_UN or die "Log::log";
    }
  }
  # restore global variables
  $@ = $at;
  return 1;
}

1;
