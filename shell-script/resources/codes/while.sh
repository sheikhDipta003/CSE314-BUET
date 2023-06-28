#!/bin/bash

read pass

# space around the condition
while [ $pass != "111" ]
do
 echo "incorrect"
 read pass
done
