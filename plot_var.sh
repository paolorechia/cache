#/bin/bash

ROOT_DIR=~/cache

if [ -z $2 ]; then
    visualizer=eog
else
    visualizer=$2
fi


len=${#1}
index=$(($index - 4))
politica=${1:$index:1}
index=$(($index + 2))
if [ $politica = w ]; then
    titulo="Cache ${1:$index:1}K (write-through)"
else
    titulo="Cache ${1:$index:1}K (copy-back)"
fi    
echo $politica
echo $titulo
    
if [ -n $arg1 ] && [ -a $arg1 ]; then
	echo "$ROOT_DIR/strip.sh $1 > stripped_$1"
	$ROOT_DIR/strip.sh $1 > stripped_$1 
	echo "$ROOT_DIR/build_dat.sh $1 > trab162/plots/assoc-bl.dat"
	$ROOT_DIR/build_dat.sh $1 > $ROOT_DIR/trab162/plots/assoc-bl.dat
#	rm stripped_$1
    arquivo=$1-bl.gp
    saida=$ROOT_DIR/trab162/plots/$arquivo

cat <<DOC0 > $saida
#!/usr/bin/gnuplot
reset
set terminal png

set xlabel "associativity"
set logscale x 2 
DOC0
ylabel="Taxa de Faltas"
printf "set ylabel \"$ylabel\"\n" >> $saida
printf "set title \"$titulo\"\n" >> $saida
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
	( cd $ROOT_DIR/trab162/plots ; chmod 700 $arquivo; ./$arquivo > $1.png ; $visualizer $1.png & )
else
    echo "Passe um argumento, e.g. 'area4k'"
fi
