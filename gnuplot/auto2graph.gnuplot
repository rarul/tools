#!/usr/bin/gnuplot -c

# This is a script to create quick and simple graph.
# Usage
### Prepare a file which has one value per one line.
###   ex) seq 1 10 > seq.txt
### Make graph by the file
### $ ./auto2graph.gnuplot seq.txt

# output to PNG 640x640
set terminal png size 400,300
#set terminal wxt size 400,300

# output filename
set output "hoge.png"

# creating one data line
#set size 1.0,0.5
#set title "CPU usage"
set xlabel "sec" offset 15,3
set ylabel "percent" offset 2.7,4
set yrange [0:100]
#set xdata time
#set timefmt "%H:%M:%S"
#set format x "%H:%M:%S"
#set xtics rotate
#set style fill transparent solid 0.5 noborder
set key left top
set lmargin at screen 0.12
plot ARG1 using 1:2 title "avg5" with lines, ARG1 using 1:3 title "avg60" with lines
#pause -1


