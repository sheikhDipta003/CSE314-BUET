#!/bin/bash
rm -f output.txt
rm -f sample.out
echo 'Compiling the cpp file and generating object files'
g++ -pthread sample.cpp -o sample.out
echo 'All ready, running...'
./sample.out
echo 'Completed successfully'