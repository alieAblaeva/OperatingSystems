#!/bin/bash

n=$1
gcc publisher.c -o publisher
gcc subscriber.c -o subscriber

for ((i=0; i<$n; i++)); do
	gnome-terminal -- ./subscriber "$i" 
done

./publisher $n 

