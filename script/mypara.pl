#!/usr/bin/perl

use warnings;
use autodie;
use strict;

use threads;
use Thread::Queue;
use POSIX;

my $PRECMD = '';
my $POSTCMD = '';
my $PARAJOB = 4;
my $qqq = new Thread::Queue;

sub usage {
    print "./mypara [-c PRECMD] [-C POSTCMD] [-j NUM] [-f FILENAME] [ARG1 [ARG2 [...]]]\n";
    print "\t-c PRECMD --- All jobs are execute like \"PRECMD job\".\n";
    print "\t-c POSTCMD --- All jobs are execute like \"job POSTCMD\".\n";
    print "\t-j NUM --- Max execute in parallel.\n";
    print "\t-f FILENAME --- Filename where command list written. One line one command.\n";
    print "\tARG1 ARG2 --- One ARG one command. In case multiple argument, use quote like \"cmd arg1 arg2\".\n";
    print "\tWhen STDIN is not tty (like PIPE), then read one line by one and execute it as command.\n";
    exit 1;
}

sub worker_func {
    while(1) {
        ### block until getting message
        my $job = $qqq->dequeue;
        if ($job eq "null") {
            last;
        }
        $job = $PRECMD . ' ' . $job . ' ' . $POSTCMD;
        my $retval = system($job);
    }
}

sub check_and_push {
    my $job = shift;
    $job =~ s/(\r|\n)$//;
    if ($job ne '') {
        $qqq->enqueue($job);
    }
}

sub main {
    my $i=0;
    my $argnum = @ARGV;
    my $filename = '';

    # parse argument
    for($i=0; $i<$argnum; $i++) {
        if ($ARGV[$i] eq '-j') {
            if ($i+1 < $argnum) {
                $PARAJOB = int($ARGV[$i+1]);
                if ("$PARAJOB" ne $ARGV[$i+1]) {
                    print "Perhaps argument is invalid: $ARGV[$i+1] \n";
                    usage();
                }
                $i++;
            } else {
                print "need additional argument for $ARGV[$i] \n";
                usage();
            }
        } elsif ($ARGV[$i] eq '-f') {
            if ($i+1 < $argnum) {
                $filename = $ARGV[$i+1];
                $i++;
            } else {
                print "need additional argument for $ARGV[$i] \n";
                usage();
            }
        } elsif ($ARGV[$i] eq '-c') {
            if ($i+1 < $argnum) {
                $PRECMD = $ARGV[$i+1];
                $i++;
            } else {
                print "need additional argument for $ARGV[$i] \n";
                usage();
            }
        } elsif ($ARGV[$i] eq '-C') {
            if ($i+1 < $argnum) {
                $POSTCMD = $ARGV[$i+1];
                $i++;
            } else {
                print "need additional argument for $ARGV[$i] \n";
                usage();
            }
        } elsif ($ARGV[$i] eq '-h') {
            usage();
        } elsif (substr($ARGV[$i], 0, 1) eq '-') {
            print "unknown argument for $ARGV[$i] \n";
            usage();
        } else {
            last;
        }
    }

    # check argument valid
    if ($PARAJOB <= 0 || $PARAJOB > 20) {
        print "WARNINGS: invalid PARAJOB $PARAJOB is modified to 4\n";
        $PARAJOB = 4;
    }
    if ($filename ne '') {
        if ( ! -f $filename) {
            print "file not found: $filename \n";
            usage();
        }
    }

    # prepare worker
    my @mythreads;
    for(my $j=0; $j<$PARAJOB; $j++) {
        my $thread = threads->create('worker_func');
        push(@mythreads, $thread);
    }

    # enqueue job by argument
    for( ; $i<$argnum; $i++) {
        check_and_push($ARGV[$i]);
    }

    # enqueue job by filename
    if ($filename ne '') {
        open my $fp, '<', $filename;
        foreach my $line (<$fp>) {
            check_and_push($line);
        }
        close $fp;
    }

    # enqueue job by stdin
    if (!POSIX::isatty(0)) {
        while(my $line = <STDIN>) {
            check_and_push($line);
        }
    }

    # enqueue "null" to stop threads
    for(my $j=0; $j<$PARAJOB; $j++) {
        $qqq->enqueue('null');
    }

    # wait for threads
    foreach my $thread (@mythreads) {
        $thread->join;
    }

    # end successfully ?
    # ToDo: do we need to set exit status ?
    return 0;
}

exit main;

