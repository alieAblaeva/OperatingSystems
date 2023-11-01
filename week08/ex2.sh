#!/bin/bash

# Compile pager.c
gcc pager.c -o pager
# Compile mmu.c
gcc mmu.c -o mmu
mkdir -p /tmp/ex2
# Test Case 1
echo "Test case 1"
./pager 4 2 &
sleep 1
PAGER_PID=$!
./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $PAGER_PID &



