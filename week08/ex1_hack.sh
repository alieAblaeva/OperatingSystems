#!/bin/bash


PID=$(cat /tmp/ex1.pid)

sudo gdb -pid=$PID -ex "info proc map" -ex "detach" -ex "exit" > info.txt
HEAP=$(cat info.txt | grep "heap")
BEGIN=$(echo $HEAP | awk '{print $1}')
SIZE=$(echo $HEAP | awk '{print $3}')

sudo gdb -pid=$PID -ex "find $BEGIN, +$SIZE, {char[5]}\"pass:\"" -ex "detach" -ex "exit" > addrs.txt
ADDRS=`cat addrs.txt |grep -ho "0x[0-9a-f]*"`
AD=`echo $ADDRS | awk '{print $2}'`
sudo gdb -pid=$PID -ex "x/13c $AD" -ex "detach" -ex "exit" > pass.txt
accumulated_sumbols=""
while IFS= read -r line; do
    extracted_symbols=$(echo "$line" | grep -oE "'[^']+'" | sed "s/'//g" | tr -d '\n')
    accumulated_symbols+=${extracted_symbols}
    #echo "$accumulated_symbols"
done < "pass.txt"
echo "$accumulated_symbols"
kill -9 $PID
