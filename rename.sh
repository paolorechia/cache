#/bin/bash

for each in $(ls); do
    nome=$(echo $each | tr '_' '-')
    mv $each $nome
done
