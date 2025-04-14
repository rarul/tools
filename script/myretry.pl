#!/usr/bin/perl

use warnings;
use autodie;
use strict;

sub usage {
    print "./myretry RETRYNUM command [ARGUMENTS]\n";
    exit 1;
}

sub main {
    if (@ARGV < 2) {
        usage;
    }

    my $retrynum = int($ARGV[0]);
    if ($retrynum <= 0) {
        print "retrynum adjusted to 1\n";
    $retrynum = 1;
    } elsif ($retrynum > 10) {
        print "retrynum adjusted to 10\n";
        $retrynum = 10;
    }

    shift @ARGV;
    my $retval = 1;
    my $i=0;
    for($i=0; $i<$retrynum; $i++) {
        $retval = system(@ARGV);
        if ($retval == 0) {
            last;
        }
        print "$i th execute failed $retval \n";
    }
    if ($retval != 0) {
        print "retrynum $retrynum over but failed\n";
    } else {
        print "retry $i th success\n";
    }

    # to get exit status, shift right 8
    return ($retval >> 8);
}

exit main;

