#!/usr/bin/env perl
use strict;
use warnings;

my @deps = qw(
    gdb
);

if ($ENV{BUILD_ALL}) {
    system "git submodule update --init --recursive";

    push @deps, qw(
        libasound2-dev
        libesd0-dev
        xorg-dev
    );
}
else {
    push @deps, qw(
        liblua5.1-0-dev
    );

    push @deps, qw(
        libsdl1.2-dev
        libsdl-image1.2-dev
    ) if $ENV{TILES} || $ENV{WEBTILES};
}

exec "sudo apt-get install @deps";
