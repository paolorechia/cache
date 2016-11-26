#/bin/bash


if [ $computer="HOME" ]; then
    visualizer=eom
    else
    visualizer=eog
fi

if [ -n $arg1 ] && [ -a $arg1 ]; then
	echo "./strip.sh $1 > stripped_$1"
	./strip.sh $1 > stripped_$1 
	echo "./build_dat.sh $1 > trab162/plots/assoc-bl.dat"
	./build_dat.sh $1 > trab162/plots/assoc-bl.dat
	echo "( cd trab162/plots ; ./dat-assoc-bl.gp > $1 ; $visualizer $1 )"
	( cd trab162/plots ; ./dat-assoc-bl.gp > $1.png ; $visualizer $1.png )
else
    echo "Passe um argumento, e.g. 'area4k'"
fi
