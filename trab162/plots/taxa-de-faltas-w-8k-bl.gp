#!/usr/bin/gnuplot
reset
set terminal png

set xlabel "associativity"
set logscale x 2 
set ylabel "taxa-de-faltas-w-8k"
set title "Cache 8K (write-through)"
set key reverse Left outside
set grid

set style data linespoints
plot "assoc-bl.dat" using 1:2 title "bl8", "" using 1:3 title "bl16", "" using 1:4 title "bl32", "" using 1:5 title "bl64", "" using 1:6 title "bl128", #
