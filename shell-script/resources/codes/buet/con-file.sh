#!/bin/sh

filepath="/mnt/c/Users/hp/Desktop/Lab_practice_3-2/cse314/shell-command/resources/codes/shell.sh"

if [ -d $filepath ]; then
	echo "file"
else
	echo "directory"
fi
