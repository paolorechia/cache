#/bin/bash

if [ -n $arg1 ] && [ -a $arg1 ]; then
	echo "./strip.sh $1 > stripped_$1"
	./strip.sh $1 > stripped_$1 
	echo "./build_dat.sh $1 > trab162/plots/assoc-bl.dat"
	./build_dat.sh $1 > trab162/plots/assoc-bl.dat
	echo "( cd trab162/plots ; ./dat-assoc-bl.gp > $1 ; eog $1 )"
	( cd trab162/plots ; ./dat-assoc-bl.gp > $1.png ; eog $1.png )
else
    echo "Passe um argumento, e.g. 'area4k'"
fi
