##Trabalho arquitetura

# extract\_cast\_d.sh
Acessa todos os arquivos \_cast\_d relevantes e extrai de cada um deles a taxa de faltas. Sua execucao depende de um argumento que representa a capacidade da cache que se deseja saber as taxas de falta

Exemplo:
$ extract\_cast\_d.sh 4

Alem disso deve estar no diretorio raiz do repositorio.

Seg Nov 21 18:40:14 BRST 2016

# constroi\_tabela.sh

Identico ao extract\_cast\_d.sh, exceto que a saida vem na forma de uma tabela (ainda pouco organizada)

Seg Nov 21 18:48:46 BRST 2016

# strip.sh

Retira todos os headers, letras e simbolas nao numericos de um arquivo no formato tabela construido pelos scripts anteriores, com o objetivo de facilitar a construcao do grafico


Ter Nov 22 14:21:40 BRST 2016

# build\_dat.sh

A partir de dois arquivos stripped, constroi uma tabela .dat. Posteriormente vou construir a tabela inteira.
Ter Nov 22 15:14:57 BRST 2016
