#!/usr/bin/perl -w

use strict;
use Getopt::Long;

$SIG{INT} = "printstats"; # traps keyboard interrupt

my $PACE = 5; # simulation time unit in seconds

my @END_SYSTEMS = (
                   '164.57.24.22',
                   '164.57.24.160',
                   '164.57.24.32',
                   '164.57.24.64',
                   '164.57.24.96',
                   '164.57.24.128',
                   '164.57.25.22',
                   '164.57.26.13',
                   '164.57.25.34',
                   '164.57.27.229',
                   '164.57.27.12',
                   '249.165.129.17',
                   '164.57.27.101',
                   '164.57.27.140',
                   '164.57.27.197',
                   '164.57.25.66',
                   '164.57.27.98',
                   '164.57.25.141',
                   '164.57.26.133',
                   '164.57.26.101',
                   '164.57.26.76',
                   '164.57.27.165',
                   );

my @BANDWIDTHS = (
		  '1000',
		  '2000',
		  '3000',
		  '4000',
		  '5000',
		  );

my @INTERVAL = (
		  '5',
		  '10',
		  '15',
		  '20',
		  '25',
		  );

my @STARTTIME = (
		  '5',
		  '10',
		  '15',
		  '20',
		  '25',
		  );

#Scheduling: STARTTIME = $INVTERVALS[rand($#INTERVAL)]
#Scheduling: DURATION = $INVTERVALS[rand($#INTERVAL)]*${opts{'arrivalrate'}}

my %opts = ('pce'   => 'mcbase',
	    'mode'   => 'iterative',
	    'arrivalrate'   => '1',
	    'alpha'   => '1.0',
	    'maxlsps'   => '100', 
            );

GetOptions (\%opts, "pce=s", # mcbase or mcsched
	    "mode=s",    # mode = random or iterative
            "arrivalrate=s",
            "alpha=s",
            "maxlsps=s",
            "noreverse",
            );

my %LSPs; # indexed by GRI in format 'ucid:seqnum'
my $num_lsp = 0;

# if random, pick 2 end systems and try to setup an LSP...
# if iterative mode, try all combinations/permutations...

my $total_narb_requests = 0;
my $failed_narb_requests = 0;
my $successful_narb_requests = 0;

my $tot_lsps = 0;
my $num_lsps = 0;

my $ucid = 1234;
my $seqnum = 100;
sub new_gri {
   $seqnum++;
   return "$ucid:$seqnum"; 
}
sub get_gri {
   return "$ucid:$seqnum"; 
}

my $lsp_src = 0;
my $lsp_dst = 0;

sub get_srcdst_pair {
    if ($opts{'mode'} =~ /^random$/i) {
         $lsp_src = int(rand($#END_SYSTEMS));
         while(1) {
             $lsp_dst = int(rand($#END_SYSTEMS));
             last if ($lsp_dst != $lsp_src);
         }
    } elsif ($opts{'mode'} =~ /^iterative$/i) {
        $lsp_dst++; 
        if ($lsp_dst == $#END_SYSTEMS) {
            $lsp_src++;
            if ($lsp_src == $#END_SYSTEMS-1) {
                return (undef, undef);
            }
            if ($opts{'noreverse'}) {
                $lsp_dst = $lsp_src + 1;
            }
            else {
                $lsp_dst = 0;
            }
        }
    }
    return ($END_SYSTEMS[$lsp_src], $END_SYSTEMS[$lsp_dst]);
}

sub prepare_lsp {
    my $gri = new_gri();
    my ($a, $z) = get_srcdst_pair();
    return undef unless ($a);
    $LSPs{$gri}{'gri'} = $gri; 
    $LSPs{$gri}{'src'} = $a; 
    $LSPs{$gri}{'dst'} = $z;
    $LSPs{$gri}{'bw'} = $BANDWIDTHS[int(rand($#BANDWIDTHS))];
    if ($opts{'pce'} eq 'mcsched') {
        $LSPs{$gri}{'start_time'} = 0;
        if (rand() < $opts{'alpha'}) {
            $LSPs{$gri}{'start_time'} = $STARTTIME[int(rand($#STARTTIME))];
        }
    }
    $LSPs{$gri}{'duration'} = $INTERVAL[int(rand($#INTERVAL))]*$opts{'arrivalrate'};
    return $LSPs{$gri};
}

sub setup_lsp_mcbase {
    for ($num_lsp = 0; $num_lsp < $opts{'maxlsps'}; $num_lsp++)
    {
        my $lsp = prepare_lsp();
        return unless($lsp);
        my $interval = $INTERVAL[int(rand($#INTERVAL))];
        print "\nCreating LSP: $lsp->{'gri'} (for $lsp->{'duration'} seconds; next LSP: $interval seconds) \n  A: $lsp->{'src'} \n  Z: $lsp->{'dst'} \n\n";
        $total_narb_requests++;
        print "** /usr/local/dragon/sbin/narb_test -Q -S $lsp->{'src'} -D $lsp->{'dst'} -x4 -e1 -m -b $lsp->{'bw'} -g $lsp->{'gri'}\n";
        open I, "time -p /usr/local/dragon/sbin/narb_test -Q -S $lsp->{'src'} -D $lsp->{'dst'} -x4 -e1 -m -b $lsp->{'bw'} -g $lsp->{'gri'}|";
        my $narb_fail = 1;
        while(<I>) {
            print;
            $narb_fail = 0 if /request successful/i;
	    if (/NumLinks/) {
                #collect sumbandwidth and bandwidth
            }
        }
        close I;
        if ($narb_fail) {
	    print "*** NARB could not calculate a path...\n";
	    $failed_narb_requests++;
            delete($LSPs{$lsp->{'gri'}});
        }
        else {
            $successful_narb_requests++;
        }
        while ($interval > 0) {
            $interval -= $PACE;
            sleep($PACE);
            check_lsps_for_deletion();
        } 
    }
    while (keys %LSPs) {
        sleep($PACE);
        check_lsps_for_deletion();
    }
}

sub check_lsps_for_deletion {
    foreach my $gri (keys %LSPs) {
        $LSPs{$gri}{'duration'} -= $PACE;
        if ($LSPs{$gri}{'duration'} <= 0) {
            print "\nDeleting LSP: $gri\n  A: $LSPs{$gri}{'src'} \n  Z: $LSPs{$gri}{'dst'} \n\n";
            print "** /usr/local/dragon/sbin/narb_test -S $LSPs{$gri}{'src'} -D $LSPs{$gri}{'dst'} -x4 -e1 -m -b $LSPs{$gri}{'bw'} -d -g $gri\n";
            open I, "time -p /usr/local/dragon/sbin/narb_test -S $LSPs{$gri}{'src'} -D $LSPs{$gri}{'dst'} -x4 -e1 -m -b $LSPs{$gri}{'bw'} -d -g $gri|";
            my $narb_fail = 1;
            while(<I>) {
                print;
            }
            close I;
            delete($LSPs{$gri});
        }
    }
}

sub setup_lsp_mcsched {
    for ($num_lsp = 0; $num_lsp < $opts{'maxlsps'}; $num_lsp++)
    {
        my $lsp = prepare_lsp();
        return unless($lsp);
        my $interval = $INTERVAL[int(rand($#INTERVAL))];
        print "\nCreating LSP: $lsp->{'gri'} (starting in $lsp->{'start_time'} seconds; last for $lsp->{'duration'} seconds; next LSP: $interval seconds) \n  A: $lsp->{'src'} \n  Z: $lsp->{'dst'} \n\n";
        $total_narb_requests++;
        print "** /usr/local/dragon/sbin/narb_test -R $lsp->{'start_time'}:$lsp->{'duration'} -S $lsp->{'src'} -D $lsp->{'dst'} -x4 -e1 -m -b $lsp->{'bw'} -g $lsp->{'gri'}\n";
        open I, "time -p /usr/local/dragon/sbin/narb_test -R $lsp->{'start_time'}:$lsp->{'duration'} -S $lsp->{'src'} -D $lsp->{'dst'} -x4 -e1 -m -b $lsp->{'bw'} -g $lsp->{'gri'}|";
        my $narb_fail = 1;
        while(<I>) {
            print;
            $narb_fail = 0 if /request successful/i;
	    if (/NumLinks/) {
                #collect sumbandwidth and bandwidth
            }
        }
        close I;
        if ($narb_fail) {
	    print "*** NARB could not schedule a path...\n";
	    $failed_narb_requests++;
            delete($LSPs{$lsp->{'gri'}});
        }
        else {
            $successful_narb_requests++;
        }
        while ($interval > 0) {
            $interval -= $PACE;
            sleep($PACE);
            #scheduled lsps will be automatically removed by PCE. no need to call check_lsps_for_deletion  
        } 
    }
}

if ($opts{'pce'} eq 'mcbase') {
    setup_lsp_mcbase();
}
elsif ($opts{'pce'} eq 'mcsched') {
    setup_lsp_mcsched();
}


&printstats;

exit(0);


sub printstats {

    print "\n\n\n";

    print "narb requests (good/bad/total): $successful_narb_requests / $failed_narb_requests / $total_narb_requests\n";

    exit(0);
}


