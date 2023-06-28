#!/bin/bash
take_sum()
{
 echo "func called"
 # args passed to this func
 echo $*
 echo $#
}

# just the name is enough to call the func
# args passed to take_sum
take_sum 1 2 3 4 5

# command line arg
echo $*
