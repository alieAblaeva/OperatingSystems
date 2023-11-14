#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <directory_path>"
    exit 1
fi

directory_path=$1

gcc monitor.c -o monitor
gcc ex1.c -o ex1

./monitor "$directory_path" &

sleep 1

./ex1 "$directory_path"

