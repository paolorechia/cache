#/bin/bash


function extractFetches {
        grep Fetches | grep -v \(
    }

function extractMisses {
    grep Misses 
}

cat $1 | extractFetches ; cat $1 | extractMisses
