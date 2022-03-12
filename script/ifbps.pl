#!/usr/bin/perl

# This is a Perl script to show current ethernet throughput statistics.
# Showing 8 values in each interval: 4 for receive and 4 for transmit.
# Default interval is 1 sec. Change it by the argument.
# 4 values are the average of:
# - last 1 interval
# - last 5 interval
# - last 15 interval
# - from the beginning to current
# unit for the value is Kbps (== 1000 * bps)
# if the value is bigger than 10000, then the unit is Mbps with showing 'M' character.
### ex) $ ./ifbps.pl 1
### Receive kbps (last 1, avg last 5, avg last 15, avg start-end), Trans kbps (same)
###   (---snip---)
###  23M(1) 6813 (5) 2272 (15) 2272 (t) R | T 1553 (1)  448 (5)  150 (15)  150 (t)
###  12M(1) 9218 (5) 3073 (15) 2881 (t) R | T  703 (1)  588 (5)  197 (15)  185 (t)



use strict;
use warnings;
use autodie;
use Time::HiRes 'sleep';
use Time::HiRes 'time';

my $target_eth_name = 'enp';

sub get_current_index_value {
	my $gettingindex = shift;
	my $recv_bytes = 0;
	open (my $fp, '<', '/proc/net/dev') or die 'cannot open /proc/net/dev';
	while (my $line = <$fp>) {
		if (index($line, $target_eth_name) < 0) { next; }
		$line =~ s/[\s]+/\t/g;
		my @splited = split(/\t/, $line);
		$recv_bytes = $splited[$gettingindex];
		last;
	}
	close ($fp);
	if ($recv_bytes <= 0) {
		print 'cannot parse /proc/net/dev with ' . $target_eth_name . "\n";
		exit 1;
	}
    return $recv_bytes;
}
sub get_current_tran_bytes {
    return get_current_index_value(9);
}
sub get_current_recv_bytes {
    return get_current_index_value(1);
}

sub get_format_bytes {
	my $in_val = shift;
	$in_val = 8 * $in_val;
	my $in_str = ' ';
	if ($in_val >= 10 * 1000 * 1000) {
		### Mbps
		$in_val /= (1000*1000);
		$in_str = 'M';
	} else {
		### kbps
		$in_val /= (1000);
	}
	return int($in_val) . $in_str;
}
sub my_main {
	my $sleep_time = shift;
	my $full_interval = 15;
	my $half_interval = int($full_interval / 3);
	my @last_recv_bytes;
	my @last_tran_bytes;
	my @last_times;

	# initialize
	my $start_recv_byte = get_current_recv_bytes();
	my $start_tran_byte = get_current_tran_bytes();
	my $start_time = time();
	for (my $i=0; $i<$full_interval; $i++) {
		push (@last_recv_bytes, $start_recv_byte);
		push (@last_tran_bytes, $start_tran_byte);
		push (@last_times, $start_time);
	}

	printf "Receive kbps (last 1, avg last %d, avg last %d, avg start-end), Trans kbps (same)\n",
		$half_interval, $full_interval ;
	# loop
	while (1) {
		# sleep
		sleep($sleep_time);

		# get current info
		my $current_recv_byte = get_current_recv_bytes();
		my $current_tran_byte = get_current_tran_bytes();
		my $current_time = time();

		# calc recv statistics
		# for last 1
		my $last_recv_val = get_format_bytes( ($current_recv_byte - $last_recv_bytes[0]) / ($current_time - $last_times[0]));
		# for half of stored array
		my $half_recv_val = get_format_bytes( ($current_recv_byte - $last_recv_bytes[$half_interval-1]) / ($current_time - $last_times[$half_interval-1]));
		# for full of stored array
		my $full_recv_val = get_format_bytes( ($current_recv_byte - $last_recv_bytes[$full_interval-1]) / ($current_time - $last_times[$full_interval-1]));
		# for total
		my $total_recv_val = get_format_bytes( ($current_recv_byte - $start_recv_byte) / ($current_time - $start_time));

		# calc tran statistics
		# for last 1
		my $last_tran_val = get_format_bytes( ($current_tran_byte - $last_tran_bytes[0]) / ($current_time - $last_times[0]));
		# for half of stored array
		my $half_tran_val = get_format_bytes( ($current_tran_byte - $last_tran_bytes[$half_interval-1]) / ($current_time - $last_times[$half_interval-1]));
		# for full of stored array
		my $full_tran_val = get_format_bytes( ($current_tran_byte - $last_tran_bytes[$full_interval-1]) / ($current_time - $last_times[$full_interval-1]));
		# for total
		my $total_tran_val = get_format_bytes( ($current_tran_byte - $start_tran_byte) / ($current_time - $start_time));

		# show statistics
		printf "%5s(1) %5s(%d) %5s(%d) %5s(t) R | T %5s(1) %5s(%d) %5s(%d) %5s(t)\n",
			$last_recv_val,
			$half_recv_val, $half_interval,
			$full_recv_val, $full_interval,
			$total_recv_val,
			$last_tran_val,
			$half_tran_val, $half_interval,
			$full_tran_val, $full_interval,
			$total_tran_val ;
		# Remove the tail value and Add to the head.
		pop (@last_recv_bytes); unshift (@last_recv_bytes, $current_recv_byte);
		pop (@last_tran_bytes); unshift (@last_tran_bytes, $current_tran_byte);
		pop (@last_times); unshift (@last_times, $current_time);
	}
	return 0;
}

sub main {
	my $interval_sec = 1;
	for my $arg (@ARGV) {
		my $value = int($arg);
		if ($value > 0) {
			$interval_sec = $value;
			last;
		}
	}
	return my_main $interval_sec;
}

exit main;
