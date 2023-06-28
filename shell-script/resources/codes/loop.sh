#!/bin/bash

# bash has more functionality than sh
# this loop cannot be done in sh
# print sum of numbers 1 - 10
total=0
for((i=1;i<10;i++))
do
 total=$(($total + $i))
done

echo $total
