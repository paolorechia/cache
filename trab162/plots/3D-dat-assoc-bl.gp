#!/usr/bin/gnuplot
reset
set terminal png

set xlabel "associativity"
set logscale x 2 

set ylabel "cap"
set zlabel "area"

set pm3d

#set title "Area x Associatividade" 
#set key reverse Left outside
set grid

#set style data linespoints

splot "assoc-bl.dat" \
   u 1:2:3 with pm3d title "bl8", \
"" u 1:2:3 with pm3d title "bl16", \
"" u 1:2:4 with pm3d title "bl32", \
"" u 1:2:5 with pm3d title "bl64", \
"" u 1:2:6 with pm3d title "bl128", \
#
