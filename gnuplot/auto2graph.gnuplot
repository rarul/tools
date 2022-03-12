#!/usr/bin/gnuplot -c

# This is a script to create quick and simple graph.
# Usage
### Prepare a file which has one value per one line.
###   ex) seq 1 10 > seq.txt
### Make graph by the file
### $ ./auto2graph.gnuplot seq.txt

# output to PNG 640x640
#set terminal png size 1280,800
set terminal wxt size 1280,800

# output filename
#set output "vmstat_sample.png"

# creating one data line
#set size 1.0,0.5
#set title "CPU usage"
set xlabel "seq"
set ylabel "value"
#set yrange [0:100]
#set xdata time
#set timefmt "%H:%M:%S"
#set format x "%H:%M:%S"
#set xtics rotate
#set style fill transparent solid 0.5 noborder
plot ARG1 using 0:1 title "data" with lines

pause -1


