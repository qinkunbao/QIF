#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Std;

my $LONG_FORMULA = 8;
my @ALGS = qw( AES DES RC4 MD5 SHA1 SHA256 SHA384 SHA512 );

my %opts;
my $rv = getopts("vfr", \%opts);
# options:
#     v : verbose output
#     f : output matching reference/target filenames
#     r : output matching reference filenames only

my %matched_algs;

# Process all match files in the current directory
my $path = ".";
if(defined($ARGV[0])) {
    print "Using path $ARGV[0]\n";
    $path = $ARGV[0];
    $path =~ s/\/$//;  # remove trailing slash
}
my @match_files = split(/\s+/, `ls $path/match_*.txt`);

foreach my $match_file (@match_files) {
    chomp($match_file);
    my ($header, $formulas) = parse_matchfile("$match_file");
    if(!defined($header)) {
        warn "skipping match file '$match_file'";
        next;
    }

    if($opts{v}) {
        print "Match file '$match_file' has " . scalar(@$formulas) . " matching formulas, for algorithm " . guess_alg($header->{Reference}) . "\n";
    }

    foreach my $match (@$formulas) {
        my $rlen = formula_len($match->{"Reference Formula"});
        my $tlen = formula_len($match->{"Target Formula"});

        if($opts{v}) {
           print "    Matching formula ref len $rlen, target len $tlen\n"; 
        }

        if( $rlen > $LONG_FORMULA ||  $tlen > $LONG_FORMULA) {
#            print "Good match with $header->{Reference}\n";
            my $reference = $header->{Reference};
            my $alg = guess_alg($reference);
            my $target = $header->{Target};
            
            my $files = "";
            if($opts{r}) {
               $files = "$reference\n";
            }
            else{
                $files = "        $reference\t $target\t $match_file\n";
            }

            $matched_algs{$alg} = (defined($matched_algs{$alg}))? $matched_algs{$alg} . $files : $files; 
            last;       # One long formula is considered enough to be a good match, for now (TODO)
        }

    }
}

if(scalar(keys %matched_algs) == 0) {
    if(!$opts{r}) {
        print "Found no matching algorithms\n";
    }
}
else {
    if(!$opts{r}) {
        print "Found matches for the following algorithms :\n";
    }
    foreach my $k (keys %matched_algs) {
        if(!$opts{r}) {
            print $k . " (" . scalar(split(/\n/,$matched_algs{$k})) . " matches)\n";
        }

        if($opts{v} || $opts{f} || $opts{r}) {
                print "$matched_algs{$k}";
        
        }
    }
}

#########################################################

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

sub parse_matchfile
{
    my ($match_file) = @_;

    my $FH;
    my $rv = open($FH, "<", $match_file);
    if(!$rv) {
        warn "Failed to open file '$match_file'; $!";
        return undef;
    }
    my $file = join("", <$FH>);
    close($FH);

    # Split the file on blank lines
    my @blocks = split(/\n\s*\n/, $file);
    if(scalar(@blocks) < 2) {
        warn "Unexpected match file format; expected header then match information separated by blank lines";
        return undef;
    }
    # print "File $match_file has " . scalar(@blocks) . " blocks\n";
    
    # The first block is the header
    my $header_block = shift(@blocks); 
    my $header = to_hash($header_block);

    my @matches = ();
    foreach my $block (@blocks) {
        push @matches, to_hash($block);
    }
        
    return($header, \@matches);
}

# Parse a string of the form:
#   key1 : value1
#   ...
# into a perl hash. 
# Lines that don't have this format are ignored.
sub to_hash
{
    my ($input) = @_;

    my %h;
    my @lines = split(/\n/, $input);;
    foreach my $line (@lines) {
        chomp($line);
        if($line =~ /\s*(.*)\s*:\s*(.*)\s*/) {
            $h{$1} = $2;
        }
    }

    return \%h;
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

