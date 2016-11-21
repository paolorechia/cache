#/bin/bash

cap=$1
dump=dump_print.txt
rm $dump

echo Printing ${cap}k cache >> $dump

dir=trab161/TRC

for wp in c w; do
    echo Write policy... ${wp} >> $dump
    for assoc in 1 2 4 8; do
            for blk in 4 8 16 32 64 128; do
                if [ ${wp} = "w" ] ; then ap=n; else ap=w ; fi
                current=cast_d_${cap}kA${assoc}B${blk}W${wp}A${ap}
                echo $current >> $dump
                file=$dir/$current
                cat $file >> $dump
                ./extractor.sh $file
            done
    done
done

