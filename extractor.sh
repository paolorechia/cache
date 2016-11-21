#/bin/bash


function extractFetches {
    grep Fetches | grep -v \( | tr -s ' ' | cut -f3 | cut -f2 -d' '
}

function extractMisses {
    grep Misses | tr -s ' ' | cut -f3 | cut -f2 -d' '
}

fetches=$(cat $1 | extractFetches)
misses=$(cat $1 | extractMisses)
echo Fetches = $fetches
echo Misses = $misses
taxa_faltas=$(echo "scale=2; $fetches/$misses"  | bc -l)
echo $taxa_faltas

echo Taxa de faltas = $taxa_faltas%
