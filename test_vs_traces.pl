#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Std;
use Term::ANSIColor 2.01 qw(colorstrip colored);


my @ALGS = qw( AES DES RC4 MD5 SHA1 SHA256 SHA384 SHA512 );
my $PROG_START = time();

my %opts;
my $rv = getopts("so:", \%opts);

if( !$rv || scalar(@ARGV) < 3) {
    print "Usage: $0 [opts] <alg> <refernce dir> <test vector top-level dir>\n";
    print "    <alg>    is the reference algorithm, one of AES, DES, RC4, MD5, SHA1, SHA256, SHA384, SHA512\n";
    print "    -s       the test vector dir is a single directory, rather than directory containting multiple directories\n";
    print "    -o <dir> the name of the directory that will be created to hold output.\n";
    exit(-1);
}

if(! -e '../start.py') {
    print "This script must be run from the build directory\n";
    exit(-1);
}

my $alg = guess_alg($ARGV[0]);
if($alg eq 'UNKNOWN') {
    print "Invalid algorithm parameter '$ARGV[0]'\n";
    exit(-1);
}

my $refdir = $ARGV[1];
my $testdir = $ARGV[2];

if(!-d $refdir) {
    print "Invalid reference directory '$refdir'\n";
    exit(-1);
}
if(!-d $testdir) {
    print "Invalid test directory '$testdir'\n";
    exit(-1);
}

my @match_files = split(/\s+/, `ls match_*.txt 2>/dev/null`);
if(scalar(@match_files) > 0) {
    print "Match files found in current directory; move or delete them first\n";
    exit(-1);
}

#remove trailing slashes
$refdir =~ s/\/$//;
$testdir =~ s/\/$//;

my $comparisons = 0;
my $pass = 0;

# Create a driectory and logfile to hold results
my $ref_name = $refdir;
$ref_name =~ s/\.\.\/references\///;
$ref_name =~ s/\//_/g;
my $matchdir = "test-$ref_name-$PROG_START";
if(defined($opts{o})) {
    $matchdir = $opts{o};
}
if(!mkdir($matchdir)) {
    die "Failed to create directory for test output. (tried to create: '$matchdir', mkdir error: $!)";
}
my $logfile;
open($logfile, ">", "$matchdir/results.log") || die "Failed to open logfile for writing; $!\n";

if($opts{s}) {
    my ($found_expected, @false_positives) = do_comparison($refdir, "$testdir", $alg, $matchdir);
    $comparisons++;
    $pass += print_results($refdir, $testdir, $logfile, $alg, $found_expected, @false_positives);
}
else {
    # For every test sub directory, compare the reference to the test
    my $dh;
    opendir($dh, $testdir) || die "Failed to open '$testdir'; $!";
    while(my $f = readdir($dh)) {
        
        # Only use nontrivial directories
        if(!-d "$testdir/$f") {
            next;
        }
        if($f eq '.' || $f eq '..') {
            next;   
        }

        my ($found_expected, @false_positives) = do_comparison($refdir, "$testdir/$f", $alg, $matchdir);
        $comparisons++;
        $pass += print_results($refdir, "$testdir/$f", $logfile, $alg, $found_expected, @false_positives);
    }
}
my $donemsg = "Done, $pass of $comparisons tests passed. Results in $matchdir\n";
print $donemsg;
print $logfile $donemsg;
close($logfile);


# Interpret, print and log the results. 
sub print_results 
{
    my ($refdir, $testdir, $logfile, $alg, $found_expected, @false_positives) = @_;

    my $ispass = 1;
    my $output = "";

    # Note, the @false_positives will always be empty; since we currently only compare
    # against one reference algorithm. the only type of FP is finding $alg where we
    # know we shouldn't

    $output .= "Test result: $refdir vs. $testdir. Reference algorithm is $alg\n";

    if($testdir =~ /$alg/i) {
        if($found_expected) {
            $output .= green("PASS: Found $alg as expected.\n");
        }
        else {
            $output .= red("FAIL: Did not find $alg, but should have\n");
            $ispass = 0;
        }
    }
    else {
        if($found_expected) {
            $output .= red("FAIL: Found $alg where we shouldn't have. (false positive)\n");
            $ispass = 0;
        }
        else {
            $output .= green("PASS: Did not find $alg, as expected\n");
        }
    }

    $output .= "\n";


#    if(scalar(@false_positives)) {
#        $output .= red("FAIL: False positives: " . join(" ", @false_positives) . "\n");
#        $ispass = 0;
#    }
#    else {
#        $output .= green("PASS: no false positives\n");
#    }

    print $output;
    print $logfile colorstrip($output);

    return $ispass;
}

sub red
{
    return colored(['red'], @_);
}

sub green
{
    return colored(['green'], @_);
}


sub do_comparison
{
    my ($ref, $target, $alg, $matchdir) = @_;

    print "Going to compare '$ref' and '$target'\n";

    my $rv = system("../start.py -t  $ref $target > /dev/null");
    if($rv != 0) {
        warn "Failed to run start.py on '$ref' and '$target'\n";
        return 0;
    }

    my @match_files = split(/\s+/, `ls match_*.txt 2>/dev/null`);
    my $found_expected = 0;
    my @false_positives = ();
    if(scalar(@match_files) > 0) {
        my $match_info = `../match_info.pl`;
        if($match_info =~ /Found matches/) {
            my @matches = extract_matches($match_info);
            foreach my $match (@matches) {
                if($match eq $alg) {
                    $found_expected = 1;
                }
                else {
                    push(@false_positives, $match);
                }
            }
        }

        # keep the match files
        $target =~ /.*\/(.*)/;
        my $test_output = "$matchdir/vs-$1";
        mkdir($test_output);
        my $rv = system("mv match_*.txt $test_output/");
        if($rv != 0) {
            warn "Failed to move match files to '$test_output'";
            # remove the match files, so that the next test runs properly
            if(system("rm match_*.txt") != 0) {
                
            }
        }
        system("mv *.log $test_output");
    }


    return($found_expected, @false_positives); 

}


sub guess_alg
{
    my ($s) = @_;
    foreach my $alg (@ALGS) {
        if($s =~ /$alg/i) {
            return $alg;
        }
    }

    return "UNKNOWN";
}

sub extract_matches
{
    my ($s) = @_;

    my @found = ();

    foreach my $alg (@ALGS) {
        if($s =~ /$alg/) {
            push(@found, $alg);
        }
    }

    return @found;
}
