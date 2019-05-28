#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Std;
use Term::ANSIColor 2.01 qw(colorstrip colored);

my $SYMEX_BIN = "../../build/SymbolicExecution";

# Run inputs that should cause a crash or assertion
my @testdirs = qw( assertrepro  assertrepro2  assertrepro3  crashrepro );

foreach my $testdir (@testdirs){

    my ($f1, $f2) = input_files($testdir);

    if(!defined($f1)) {
        print red("Test $testdir failed! Invalid test files/directory\n");
        next;
    }

    my $rv = system("$SYMEX_BIN $testdir/$f1 $testdir/$f2 >/dev/null 2>&1"); 
    if($rv != 0) {
        print red("Test $testdir failed.\n");
    }
    else {
        print green("Test $testdir passed.\n");
    }

}


sub red
{
    return colored(['red'], @_);
}

sub green
{
    return colored(['green'], @_);
}

sub input_files
{
    my ($dir) = @_;

    my @files = ();
    my $DH;
    opendir($DH, $dir) || die "Failed to open test dir '$dir'; $!";
    while(my $f = readdir($DH)) {
        if($f =~ m/.*ref$/) {
            push(@files, $f);
        }
    }
    close($DH);

    my $nfiles = scalar(@files);
    if($nfiles != 2) {
        warn "Error: Found $nfiles files in $dir, expected 2";
        return undef;
    }

    return @files;
}
