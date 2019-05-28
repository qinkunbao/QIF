#!/usr/bin/perl

use strict;
use warnings;
use Parallel::ForkManager;      # Ubuntu: libparallel-forkmanager-perl

# Script to take foo.trace, and output the loops found by LoopDetection to a directory called foo. 
# TODO: not cross platform. too much use of backticks :(

# Use 7 parallel processes
my $pm = Parallel::ForkManager->new(7);

my $traces = `ls *.trace`;

DATA_LOOP:
foreach my $trace (split(/\s+/, $traces)) {

   # Forks and returns the pid for the child:
    my $pid = $pm->start and next DATA_LOOP;

    # Begin per-process work ---------
    chomp($trace);

    if($trace !~ m/([0-9a-zA-Z\-]+)\.trace/) {
       warn "file '$trace' doesn't end with .trace";
       next;
    }
    my $name = $1;

    if(-e $name) {
        system("rm -fr $name");
    }
    mkdir($name);
    
    system("cp $trace $name");
    chdir($name);

    my $ret = system("../../../build/LoopDetection $trace");
    if($ret != 0) {
        warn "LoopDetection failed on $trace";
        next;
    }

    system("../../../references/referenceMaker/dedup.pl -s");
    system("rm *.ref");     # remove un-dup'd references
    system("mv uniques-*/*.ref .");
    system("rmdir uniques-*");
    unlink($trace);
    chdir("..");
    # End per-process work --------- 

    $pm->finish; # Terminates the child process
}
