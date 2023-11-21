#!/bin/bash

input="input.txt"
disk=$(head -n 1 "$input")

gcc ex2.c -o ex2
gcc create_fs.c -o create_fs

./create_fs $disk
sleep 1
./ex2 $input
