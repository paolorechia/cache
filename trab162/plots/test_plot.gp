#!/usr/bin/gnuplot
reset
set terminal png

set xdata time
set timefmt "%d/%m/%Y %H:%M:%S"
set format x "%H:%M"
set xlabel "associativity"

set ylabel "time[ns]"
set yrange [0.5:130.4]

set title "Tempo De Acesso X"
set key reverse Left outside
set grid

set style data linespoints

plot "test_plot.dat" using 1:3 title "slot 1", \
"" using 1:4 title "slot 2", \
"" using 1:5 title "slot 3", \
"" using 1:6 title "slot 4", \
"" using 1:7 title "slot 5", \
#
