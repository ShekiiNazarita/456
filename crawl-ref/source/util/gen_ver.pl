#!/usr/bin/env perl

use strict;
use warnings;

use File::Basename;

my $scriptpath = dirname($0);
my $outfile = $ARGV[0];
my $mergebase = $ARGV[1];

$mergebase or $mergebase = "";

mkdir dirname($outfile);

$_ = `git describe --tags $mergebase 2> /dev/null`
    || (open(IN, "<", "$scriptpath/release_ver") ? <IN>
        : die "No Git, and $scriptpath/release_ver doesn't exist.\n")
    or die "couldn't get the version information\n";

chomp;

/v?(?'tag'(?'major'[0-9]+\.[0-9]+)(?:\.[0-9]+)?(?:-(?'pretyp'[a-zA-Z]+[0-9]+))?)(?:-[0-9]+-g[a-fA-F0-9]+)?/
    or die "Version string '$_' is malformed.\n";

my ($major, $tag, $pretyp) = ($+{major}, $+{tag}, $+{pretyp});

my $rel = defined($pretyp) ? $pretyp le "b" ? "ALPHA" : "BETA" : "FINAL";

my $prefix = "CRAWL";

open OUT, ">", "$outfile" or die $!;
print OUT <<__eof__;
#define ${prefix}_VERSION_MAJOR "$major"
#define ${prefix}_VERSION_RELEASE VER_$rel
#define ${prefix}_VERSION_SHORT "$tag"
#define ${prefix}_VERSION_LONG "$_"
__eof__
close OUT or die $!;
