#!/usr/bin/perl

# Automatically create references from traces.
# create_refs.pl can be pointed to a path containing subdirs of traces for
# a given algorithm.  For example
# [..]/references/referenceMaker/reference_crypto_traces$ tree -d all
# all
# ├── aes
# ├── des
# ├── md4
# ├── md5
# ├── rc4
# ├── sha1
# ├── sha256
# ├── sha384
# └── sha512
# In one algorithm directory there can be multiple traces, e.g., 
# [...]references/referenceMaker/reference_crypto_traces$ tree all/aes
# all/aes
# ├── aes_decrypt
# ├── AesDecrypt
# ├── AES_decrypt
# └── private_AES_set_encrypt_key

# Then "create_refs.pl -p all" will go through each algorithm directory, 
# extract loops from traces with LoopDetection, then deduplicate and 
# remove loops with only short formulas.
#
# Then if -p is specified, the references will be "pruned", i.e., 
# they will be compared against the evaluation traces, known to (not) contain
# specific algorithm.  If a reference file causes a false positive, it is deleted. 

use strict;
use warnings;
use Getopt::Std;
use Cwd qw(cwd);
use File::Path qw(rmtree);

my $ROOT = '/home/gregz/work/malware/malwarecrypto';
my $LOOP_DETECT = "$ROOT/build/LoopDetection";
my $DEDUP = "$ROOT/references/referenceMaker/dedup.pl";
my @ALGS = qw( AES DES RC4 MD5 SHA1 SHA256 SHA384 SHA512 );

my %opts;
my $rv = getopts("p", \%opts);
# -p  Prune the references by running unit tests (evalutation), and removing
#     refs that cause false positives.

my $path = ".";
if(defined($ARGV[0])) {
    $path = $ARGV[0];
    if($path =~ /\/$/) {
        chop($path);
    }
}

if(!-e $LOOP_DETECT) {
    print "$LOOP_DETECT not found. Make sure it's built, and check root path in this script.\n";
    exit;
}

my $DH;
opendir($DH, $path) || die "Failed to open directory '$path'; $!";

my @dirs;
foreach my $dir (readdir($DH)) {
    if($dir eq '.' || $dir eq '..') {
        next;
    }
    push(@dirs, cwd() . "/$path/$dir");   
}

#TODO: this loop should be parallelized with multiple processors. See testVectors/traces/process_traces.pl for an example
foreach my $dir (@dirs) {

    my $save_dir = cwd();
    chdir($dir);
    run_loop_detection();
    my $rv = run_dedup();
    if(!$rv) {
        warn "Deduplicating failed in $dir";
    }
    chdir($save_dir);
}
close($DH);

if($opts{p}) {
    #This doesn't need to be parallelized, since running the test vectors is
    #done in parallel already (via start.py)
    foreach my $dir (@dirs) {
        prune_refs($dir);   
    }
}

##########################################

sub prune_refs
{
    my ($dir) = @_;

    $dir =~ /.*\/(.*)$/;
    my $alg = guess_alg($1);
    if($alg eq 'UNKNOWN') {
        warn "guess_alg failed";
        return 0;
    }

    print "Pruning results for dir: '$dir', algorithm: '$alg'\n";

    my $cwd_save = cwd();
    chdir("$ROOT/build");
    my $tmp_dir = "prune_script_tmp";
    if(-e $tmp_dir) {
        rmtree($tmp_dir);
    }
    my $test_command = "../test_vs_traces.pl -o $tmp_dir $alg $dir $ROOT/testVectors/traces";
    my $rv = system($test_command);
    if($rv != 0) {
        warn "Failed to run tests while pruning, command: $test_command";
    }

    print "Completed running tests while pruning, checking for false positives\n";
    # foreach directory in tmp_dir
    #  Skip if the directory name contains $alg (it's a match that was supposed to happen)
    #  Otherwise it's a false positive, run match_info to get the names of the
    #  matching files that caused the match and delete them.
    my $DH;
    $rv = opendir($DH, $tmp_dir);
    if(!$rv) {
        warn "Failed to opendir $tmp_dir; $!";
        return;
    }

    foreach my $resultdir (readdir($DH)) {
        if($resultdir eq '.' || $resultdir eq '..' || ! -d "$tmp_dir/$resultdir") {
            print "Skipping $resultdir\n";
            next;
        }

        print "Checking dir $tmp_dir/$resultdir\n";

        if($resultdir =~ /$alg/) {
            print "Skipping $resultdir (it's the algorithm were creating references for)\n";
            next;
        }

        my $match_command = "../match_info.pl -r $tmp_dir/$resultdir";
        my $matching_ref_files = `$match_command`;
        if($? != 0) {
            warn "Failed to run match_info with command '$match_command'";
            return;
        }
        print "Found matching files: \n $matching_ref_files\n";
        foreach my $file (split(/\n/, $matching_ref_files)) {
            # The matches are output with absolute paths
            print "Deleting refrence file $file";
            $rv = unlink("$file");
            if(!$rv) {
                print "   FAILED: $!\n";
            }
            else {
                print "  done\n";
            }
            
        }
    }
    close($DH);
    chdir($cwd_save);

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


sub run_dedup
{
    my $rv = system("$DEDUP -s -o uniques");
    if($rv != 0) {
        return 0;
    }
    if(-e "uniques") {
        system("rm *.ref");
        system("mv uniques/* .");
        system("rmdir uniques");
    }

    return 1;
}

sub run_loop_detection
{
    # Run LoopDetection on all files in the current dir. 
    my $files = `ls`;

    foreach my $file (split /[\s\n]+/, $files) {
        print "Running $LOOP_DETECT $file\n";
        my $rv = system("$LOOP_DETECT $file >/dev/null");
        if($rv != 0) {
            warn "Loop detection failed on $file\n";
            return 0;
        }
    }

    return 1;
}
