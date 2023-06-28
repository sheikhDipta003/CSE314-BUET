#!/bin/bash

# return 
take_sum()
{
 total=0
for i in $*
do
 total=$(($total + $i))
done

return $total
}

# just the name is enough to call the func
# args passed to take_sum
take_sum 1 2 3 4
a=$?
# returned value is saved in $?
take_sum 1 2 3 4 5
b=$?

echo $a
echo $b

