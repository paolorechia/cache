#/bin/bash

for each in $(ls | grep .sh); do
    nome=$(echo $each | tr '-' '_')
    mv $each $nome
done
