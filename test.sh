#/bin/bash

len=${#1}
len=$(($len - 2))
echo $len
echo ${1:$len:1}
