#/bin/bash

for each in $(ls | grep taxa); do
    ./plot_var.sh "$each" $1
done
