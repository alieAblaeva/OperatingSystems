#!/bin/bash

gcc ex4.c -pthread -o ex4

m_values=(1 2 4 10 100)

output_file="ex4_res.txt"

rm -f $output_file

for m in "${m_values[@]}"
do
    echo "m=$m"
    (time ./ex4 10000000 $m) 2>> $output_file
done

