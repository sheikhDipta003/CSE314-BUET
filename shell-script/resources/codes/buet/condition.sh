#!/bin/sh

read n

if [ $n -gt 100 ]
then
	echo "This is greater than 100"
elif [ $n - gt 50 ]
then 
	echo "This is greater than 50"
else
	echo "Less than or equal to 50"
fi
