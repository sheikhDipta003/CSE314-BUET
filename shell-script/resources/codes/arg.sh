#!/bin/sh

# args are $1, $2,$3,...

# show all args
echo $*

# show number of args
echo $#

# show a specific arg
echo ${10}

# retrieve list of all files by ls command
for file in `ls`
do
	echo $file
done

total=0
for i in $*
do
 total=$(($total + $i))
done

echo $total

