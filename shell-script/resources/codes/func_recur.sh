#!/bin/bash

# recursively print all files and folders
visit()
{
 # if the argument is a directory, recursively visit within it
 if [ -d "$1" ]
 then
  for i in "$1"/*
  do
    visit "$i"	# enclose in double quote since there maybe space in path
  done
 elif [ -f $2 ]		# else just print the filename
 then
  echo "$i"
 fi
}

visit "$1"

