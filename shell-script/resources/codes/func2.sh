#!/bin/bash

# return by echo

take_sum()
{
 total=0
for i in $*
do
 total=$(($total + $i))
done
echo "result "
echo $total
}

# just the name is enough to call the func
# args passed to take_sum
result=$(take_sum 1 2 3 4)

echo $result

res=$(expr 1 + 1)
echo $res
