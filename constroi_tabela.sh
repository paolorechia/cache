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

dir=trab161/TRC

for wp in c w; do
    saida=taxa_de_faltas_${wp}_$1k
    if [ -a $saida ]; then
        rm $saida
    fi
    echo **Taxa de faltas \(Write policy... ${wp}\) >> $saida
    for assoc in 1 2 4 8; do
            echo *Associatividade $assoc >> $saida
            for blk in 8 16 32 64 128; do
                if [ ${wp} = "w" ] ; then ap=n; else ap=w ; fi
                current=cast_d_${cap}kA${assoc}B${blk}W${wp}A${ap}
                file=$dir/$current
                fetches=$(cat $file | extractFetches) 
                misses=$(cat $file | extractMisses)
                taxa=$(taxaFaltas $fetches $misses)
                echo Bl${blk}     -      0$taxa >> $saida
            done
    done
done

