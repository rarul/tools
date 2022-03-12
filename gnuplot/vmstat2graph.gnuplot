#!/usr/bin/gnuplot -c

# This is a script to create a graph by reading vmstat's output.
# Usage
### $ vmstat -n -S m -w -t -a 1 > vmstat.txt
### $ ./vmstat2graph.gnuplot vmstat.txt


# output to PNG 640x640
#set terminal png size 1280,800
set terminal wxt size 1280,800

# output filename
#set output "vmstat_sample.png"

# Plot graph on 2x1 table
set multiplot layout 2, 1
 
# creating CPU graph
set size 1.0,0.5
set title "CPU usage"
set ylabel "percent"
set yrange [0:100]
set xdata time
set timefmt "%H:%M:%S"
set format x "%H:%M:%S"
set xtics rotate
#set style fill transparent solid 0.5 noborder
plot ARG1 using 19:(100-$15-$16) title "used" with lines, \
        ARG1 using 19:16 title "iow" with lines

# creating memory graph
set size 1.0,0.5
set title "memory usage"
set ylabel "size(M Bytes)"
set autoscale y
set yrange [0:]
set xdata time
set timefmt "%H:%M:%S"
set format x "%H:%M:%S"
set xtics rotate
#set style fill transparent solid 0.5 noborder
plot ARG1 using 19:($4+$5) title "avl/buff+free" with lines, \
        ARG1 using 19:($6) title "act/cache" with lines

pause -1
