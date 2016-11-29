#/bin/bash

file_time=stripped_tempo-acesso4k
    
times=$(cat $file_time | tr '\n' ' ')

txfs=$(cat $1 | tr '\n' ' ')

i=0
for val in $times; do
    t[$i]=$val
    i=$(($i + 1))
done

i=0
for val in $txfs; do
    txf[$i]=$val
    i=$(($i + 1))
done

i=0
j=0
k=0
blocos=(8 16 32 64 128)
ass=(1 2 4 8)

echo \#\#assc blkSize $1 $2 

while [ $i -lt ${#t[@]} ]; do
	if [ $j -eq 0 ]; then
		printf "${ass[$k]}"
	fi
	
	q[$i]=$(echo "scale=8; 0.25*${t[$i]}+${t[$i]}^2"  | bc -l)

	#printf "bl=${blocos[$j]}; txf=${txf[$i]}; q=0${q[$i]}; t=${t[$i]}"

	tm[$i]=$(echo "scale=8; ${q[$i]}+${txf[$i]}*(60+${blocos[$j]})"  | bc -l)

    numero=${tm[$i]}
    primeiro_digito=${numero:0:1}
    if [ $primeiro_digito = "." ]; then
	    printf " 0${tm[$i]} "
    else
    	printf " ${tm[$i]} "
    fi


	i=$(($i + 1))
	j=$(($j + 1))
	j=$(($j % ${#blocos[@]}))
	if [ $j -eq 0 ]; then
		k=$(($k + 1))
		echo
	fi
done

echo

