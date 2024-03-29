#/bin/bash


function extractFetches {
    grep Fetches | grep -v \( | tr -s ' ' | cut -f3 | cut -f2 -d' '
}

function extractMisses {
    grep Misses | tr -s ' ' | cut -f3 | cut -f2 -d' '
}

function taxaFaltas {
    taxa_faltas=$(echo "scale=8; $2/$1"  | bc -l)
    echo $taxa_faltas
}

cap=$1
tabela_taxa_faltas=tabela_taxa_faltas.txt

if [ -a $tabela_taxa_faltas ]; then
    rm $tabela_taxa_faltas
fi

echo Printing ${cap}k cache 

dir=trab161/TRC

for wp in c w; do
    echo Write policy... ${wp} 
    for assoc in 1 2 4 8; do
            for blk in 4 8 16 32 64 128; do
                if [ ${wp} = "w" ] ; then ap=n; else ap=w ; fi
                current=cast_d_${cap}kA${assoc}B${blk}W${wp}A${ap}
                file=$dir/$current
                fetches=$(cat $file | extractFetches) 
                misses=$(cat $file | extractMisses)
                taxa=$(taxaFaltas $fetches $misses)
                echo $current
                echo $taxa
            done
    done
done

