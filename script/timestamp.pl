#!/usr/bin/perl

use warnings;
use strict;
use autodie;

use POSIX qw(strftime);

sub get_my_time() {
    return POSIX::strftime("%y%m%d-%H%M%S ", localtime());
}

sub do_pipe_mode() {
    while (defined(my $line = <STDIN>)) {
        print get_my_time() . ' ' . ${line};
    }
    return 0;
}

sub do_exec_mode() {
    open (my $fp, '-|', "stdbuf -oL @ARGV 2>&1");
    while (defined(my $line = <$fp>)) {
        print get_my_time() . ' ' . ${line};
    }
    close($fp);
    return 0;
}

sub main {
    $ENV{'TERM'} = 'dumb';
    if ( -p "/dev/stdin" ) {
        #printf("do_pipe_mode\n");
        return do_pipe_mode();
    } else {
        #printf("do_exec_mode\n");
        if ( @ARGV < 1) {
            printf("ARGUMENT required\n");
            return 1;
        }
        return do_exec_mode();
    }
    return 1;
}


exit main;
