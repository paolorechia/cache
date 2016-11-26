#!/usr/bin/gnuplot
reset
set terminal png

set xlabel "associativity"
set logscale x 2

set ylabel "block size"
set logscale y 2

set zlabel "area"

set pm3d
set grid

splot "3D-target.dat" \
   u 1:2:3 title "Associativity X Block Size X Area" 
#
