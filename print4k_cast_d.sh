#/bin/bash

cap=$1

echo Printing ${cap}k cache

cd trab161/TRC

for assoc in 1 2 4 8; do
        for blk in 4 8 16 32 64 128; do
            for wp in c w; do
                if [ ${wp} = "w" ] ; then ap=n; else ap=w ; fi
                echo cast_d_${cap}kA${assoc}B${blk}W${wp}A${ap}
                cat cast_d_${cap}kA${assoc}B${blk}W${wp}A${ap}
            done
        done
done

cd -
