#/bin/bash

arg1=stripped_$1
arg2=stripped_$2
arg3=stripped_$3

if [ -n $arg1 ] && [ -a $arg1 ]; then
    if [ -n $arg2 ] && [ -a $arg2 ]; then
        all_lines=$(cat $arg1 | tr '\n' ' ')
        i=0
        for line in $all_lines; do
            lines[$i]=$line
            i=$(($i + 1))
        done
        all_rows=$(cat $arg2 | tr '\n' ' ')
        i=0
        for row in $all_rows; do
            rows[$i]=$row
            i=$(($i + 1))
        done

        i=0
        echo \#\#BlkSize $1 $2 
        for x in 8 16 32 64 128; do
            echo "$x " | tr '\n ' ' '
            echo " ${lines[$i]}" | tr '\n ' ' '
            echo " ${rows[$i]}" | tr '\n ' ' '
            i=$(($i + 1))
                echo 
        done
    else
        echo "Passe dois argumentos!"
    fi
else
    echo "Passe um argumento pelo menos!"
fi
