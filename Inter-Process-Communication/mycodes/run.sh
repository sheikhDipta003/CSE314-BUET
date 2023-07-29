#!/bin/bash
rm -f output.txt
rm -f asgn_solve.out
echo 'Generating the object files'
g++ -pthread asgn_solve.cpp -o asgn_solve.out
echo 'All ready, running...'
./asgn_solve.out
echo 'Completed successfully'