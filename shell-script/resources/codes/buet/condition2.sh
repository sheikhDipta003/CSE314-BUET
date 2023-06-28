#!/bin/sh

read pass

if [ $pass = "secret" ]
then
	echo "correct"
else
	echo "incorrect"
fi
