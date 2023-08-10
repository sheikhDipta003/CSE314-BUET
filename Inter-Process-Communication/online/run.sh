#!/bin/bash
rm -f output.txt
rm -f a2_solve.out
echo 'Generating the object files'
g++ -pthread a2_solve.cpp -o a2_solve.out
echo 'All ready, running...'
./a2_solve.out