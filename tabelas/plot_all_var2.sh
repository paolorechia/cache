#/bin/bash

for each in $(ls | grep txf); do
    ../plot_var.sh "$each" $1
done
