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
        blocos=(8 16 32 64 128)
        ass=(1 2 4 8)
        j=0
        k=0
        echo \#\#assc blkSize $1 $2 
            while [ $i -lt ${#lines[@]} ];
            do
                if [ $j -eq 0 ]; then
                    printf "${ass[$k]} "
                fi
#                echo "${ass[$k]} " | tr '\n ' ' '
#                echo "${blocos[$j]} " | tr '\n ' ' '
                echo "${lines[$i]}" | tr '\n ' ' '
#                echo "${rows[$i]}" | tr '\n ' ' '

                i=$(($i + 1))
                j=$(($j + 1))
                j=$(($j % ${#blocos[@]}))
                if [ $j -eq 0 ]; then
                    k=$(($k + 1))
                    echo
                fi
            done
    else
        echo "Passe dois argumentos!"
    fi
else
    echo "Passe um argumento pelo menos!"
fi
