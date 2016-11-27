#/bin/bash

if [ -z $2 ]; then
    visualizer=eog
else
    visualizer=$2
fi
    
if [ -n $arg1 ] && [ -a $arg1 ]; then
	echo "./strip.sh $1 > stripped_$1"
	./strip.sh $1 > stripped_$1 
	echo "./build_dat.sh $1 > trab162/plots/assoc-bl.dat"
	./build_dat.sh $1 > trab162/plots/assoc-bl.dat
    arquivo=$1-bl.gp
    saida=trab162/plots/$arquivo

cat <<DOC0 > $saida
#!/usr/bin/gnuplot
reset
set terminal png

set xlabel "associativity"
set logscale x 2 
DOC0
printf "set ylabel \"$1\"\n" >> $saida
printf "set title \"$1 x Associatividade\"\n" >> $saida
cat <<DOC1 >> $saida
set key reverse Left outside
set grid

set style data linespoints
plot "assoc-bl.dat" using 1:2 title "bl8", \
"" using 1:3 title "bl16", \
"" using 1:4 title "bl32", \
"" using 1:5 title "bl64", \
"" using 1:6 title "bl128", \
#
DOC1
	echo "( cd trab162/plots ; ./$arquivo > $1 ; $visualizer $1 )"
	( cd trab162/plots ; chmod 700 $arquivo; ./$arquivo > $1.png ; $visualizer $1.png & )
else
    echo "Passe um argumento, e.g. 'area4k'"
fi