#/bin/bash

# lista todos os arquivos que nao terminam em .sh
for each in $(ls | grep -v .sh); do
    nome=$(echo $each | tr '_' '*')
    mv $each $nome
done
