#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Std;

# If a reference loop contains no formulas is longer than this; we don't keep
# it. We only want formulas long enough to be considered a true match by
# match_info.pl (KEEP this value in sync with match_info.pl)
my $LONG_FORMULA = 8; 

# If two files have sizes within SIZE_THRESHOLD bytes of each other, we'll
# compare them
my $SIZE_THRESHOLD = 2000;

# The SymbolicExecution step takes a very long time with large loops, so don't
# try to run it on files larger than 1MB.  This is also useful when
# deduplicating the number of loops in a trace, working only with the small
# files will save time (but will of course miss large dups).
my $MAX_FILE_SIZE = 30000;
my $SYMEX_BIN = "/home/gregz/work/malware/malwarecrypto/build/SymbolicExecution";
my $COMPARISONS = 0;  # global count of the number of times we ran SYMEX_BIN

my %opts;
my $rv = getopts("so:", \%opts);
# The -s option will remove short formulas
# The -o option specifies the name of the output directory

# find all .ref files in the current directory
my @refs = split(/\s+/m, `ls *.ref`);
my $starting_loops = scalar(@refs);

if($starting_loops == 0) {
    print "No reference files found in current directory\n";
    exit(-1);
}
else{
    print "Found $starting_loops reference loop files\n";
}

if($opts{s}) {  
    print "Going to remove loops without long formulas\n";
    my @long_formula_refs = ();
    foreach my $ref (@refs) {
       my $loop_info = `$SYMEX_BIN $ref`;
       if(longest_formula($loop_info) >= $LONG_FORMULA) {
           push(@long_formula_refs, $ref); 
       }
    }

    print "Done. Kept " . scalar(@long_formula_refs) . " of $starting_loops loops.";
    @refs = @long_formula_refs;
}


# Get the size of each file
my %sizes; 
foreach my $file (@refs) {
    my $size = (stat($file))[7];  # size in bytes
    $sizes{$file} = $size;
}

# Make a temp dir to work in, since SymbolicExecution writes match files to the
# current working directory
my $tmpdir = "tmp-" . time(); 
if(!mkdir($tmpdir)) {
    print "Failed to create directory '$tmpdir'; '$!'";
    exit(-1);
}
chdir($tmpdir);
if(! -e $SYMEX_BIN) {
    print "Can't find binary $SYMEX_BIN, check the path in this script\n";
    exit(-1);
}
if(! -x $SYMEX_BIN) {
    print "Binary $SYMEX_BIN isn't executable\n";
    exit(-1);
}


# Create a list of unique loops, and before adding a new loop to it, make sure
# we haven't already added it.  If two loop file sizes are not within
# SIZE_THRESHOLD bytes of each other, we assume they are different. 
# If a loop is larger than MAX_FILE_SIZE, consider it unique. 
my @uniques = ();
foreach my $file (@refs) {
   if(!contains_equivalent(\@uniques, $file, \%sizes)) {
        push(@uniques, $file);
   }
}


# cleanup tmpdir
chdir("..");
system("rm -fr $tmpdir");

# create output dir
if(scalar(@uniques) < $starting_loops) {
    my $outdir = "uniques-" . time();
    if(defined($opts{o})) {
        $outdir = $opts{o};
    }

    mkdir($outdir) || warn "Failed to create dir '$outdir'; $!";
    foreach my $uniq (@uniques) {
        my $ret = system("cp $uniq $outdir/");
        if($ret != 0) {
            warn "Failed to copy file '$uniq' to '$outdir'";
        }
    }

    print "Copied " . scalar(@uniques) . " unique loop files to $outdir (of a total of $starting_loops input loops)\n";
}
else {
    print "All loops are unique, no duplicates found\n";
}

print "Made $COMPARISONS comparisons, instead of the worst case ". $starting_loops*($starting_loops -1)/2 . "\n";

############################

sub contains_equivalent
{
    my ($uniques, $file, $sizes) = @_;

    foreach my $uniq (@$uniques) {
        # We don't want to call are_equivalent on large files. Consider them distinct. 
        if($sizes->{$uniq} > $MAX_FILE_SIZE || $sizes->{$file} > $MAX_FILE_SIZE) {
            next;
        }

        if(abs($sizes->{$uniq} - $sizes->{$file}) < $SIZE_THRESHOLD) {
            print "Comparing $file ($sizes->{$file} bytes) and $uniq ($sizes->{$uniq} bytes)\n";
            $COMPARISONS++;
            if(are_equivalent($file, $uniq) ) {
                return 1;
            }
        }
    }
    return 0;
}

sub are_equivalent
{
    my ($f1, $f2) = @_;

    my $ret = system("$SYMEX_BIN ../$f1 ../$f2 1>/dev/null 2>&1");
    if($ret != 0) {
        warn "'$SYMEX_BIN ../$f1 ../$f2' failed";
        return 0;
    }

    # If there was a match; SymbolicExecution will create a match file
    my $matchfile = `ls match_*.txt 2> /dev/null`;
    chomp($matchfile);

    if(!$matchfile) {
        return 0;
    }

    my %mf = parse_matchfile($matchfile);
    unlink($matchfile);
    if(!%mf) {
        warn "Failed to parse match file '$matchfile'";
        return 0;
    }

    # We require an exact match; the number of formulas must be the same, and
    # they must all match.
    my $nref = $mf{"The number of formulas in the reference"};
    my $ntarget = $mf{"The number of formulas in the target"};
    if($nref != $ntarget) {
        return 0;
    }
    if($mf{"Matching formulas"} != $nref) {
        return 0;
    }

    print "Files $f1 and $f2 are equivalent\n";
    return 1;
}

sub parse_matchfile
{
    # File format:
    # se1 output values: 38
    # se2 output values: 51
    # matching values: 20
    # ...
    # key : value

    my ($matchfile) = @_;

    my $FH;
    my $rv = open($FH, "<", $matchfile); 
    if(!$rv) {
        warn "Failed to open matchfile '$matchfile'; $!"; 
        return undef;
    }

    my %mf;
    while(my $line = <$FH>) {
        chomp($line);
        if($line =~ /\s*(.*)\s*:\s*(.*)\s*/) {
            $mf{$1} = $2;
        }
    }

    close($FH);

    return %mf;
}

sub longest_formula
{
    my ($formulas) = @_;

    if($formulas =~ /size: 0/) {
        return 0; 
    }

    my @lines = split(/\n/, $formulas);

    my $largest = -1;
    foreach my $line (@lines) {
        if($line =~ /Formula index.*Formula:(.*)$/) {
            my $len = formula_len($1);
#            print "formula len: $len\n";
            if($len > $largest) {
                $largest = $len;
            }
        }
    }

    if($largest == -1) {
        # We're not sure; return a large value, so this file is kept
       return 9001; 
    }
    return $largest;
}

# TODO: copied from match_info.pl
sub formula_len
{
    my ($formula) = @_;

#    print "\nFormula: $formula\n";
    $formula =~ s/Con\(.*?\)/---/g;
    my @left_parens = $formula =~ /\(/g;

#    print "Formul*: $formula\n";
#    print "Length : " . scalar(@left_parens) . "\n";

    return scalar(@left_parens);
}

