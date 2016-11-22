#/bin/bash

tabela=$1


if [ -n $1 ] ; then
    if [ -a $tabela ]; then
        if [ -a stub ]; then
            rm stub
        fi

        if [ -a stripped_$tabela ]; then
            rm stripped_$tabela
        fi

        cat $tabela | grep Bl | tr -s ' ' | cut -f3 -d' ' > stub 

        for line in $(cat stub); do
            
            length=${#line}
            if [ $length -ne 1 ]; then
                echo $line >> stripped_$tabela
            fi
        done
        if [ -a stub ]; then
            rm stub
        fi
    fi
else
    echo $1 nao existe!
fi
