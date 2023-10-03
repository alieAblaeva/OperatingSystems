#!/bin/bash
gcc ex3.c -o ex3
./ex3 3 &
sleep 12

pstree > ex3n3.txt

./ex3 5 &
sleep 24

pstree >ex3n5.txt
