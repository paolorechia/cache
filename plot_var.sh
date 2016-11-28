#/bin/bash

echo $1

ROOT_DIR=~/cache

if [ -z $2 ]; then
    visualizer=eog
else
    visualizer=$2
fi


# codigo xunxo, poderia ser substituido por um while + array
sub=$1
separador=-

indice=$(awk -v a="$sub" -v b="$separador" 'BEGIN{print index(a,b)}')
sub=${sub:$indice}
trace_name=$sub
indice=$(awk -v a="$trace_name" -v b="$separador" 'BEGIN{print index(a,b)}')
indice=$(($indice - 1))
trace_name=${trace_name:0:$indice}
echo $trace_name

indice=$(awk -v a="$sub" -v b="$separador" 'BEGIN{print index(a,b)}')
sub=${sub:$indice}
politica=$sub
indice=$(awk -v a="$politica" -v b="$separador" 'BEGIN{print index(a,b)}')
indice=$(($indice - 1))
politica=${politica:0:$indice}
echo $politica

indice=$(awk -v a="$sub" -v b="$separador" 'BEGIN{print index(a,b)}')
sub=${sub:$indice}
capacidade=$sub
echo $capacidade

if [ $politica = w ]; then
    titulo="$trace_name ${capacidade} (write-through)"
else
    titulo="$trace_name ${capacidade} (copy-back)"
fi    
echo $politica
echo $titulo
    
if [ -n $arg1 ] && [ -a $arg1 ]; then
	echo "$ROOT_DIR/strip.sh $1 > stripped_$1"
	$ROOT_DIR/strip.sh $1 > stripped_$1 
	echo "$ROOT_DIR/build_dat.sh $1 > trab162/plots/assoc-bl.dat"
	$ROOT_DIR/build_dat.sh $1 > $ROOT_DIR/trab162/plots/assoc-bl.dat
	rm stripped_$1
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
