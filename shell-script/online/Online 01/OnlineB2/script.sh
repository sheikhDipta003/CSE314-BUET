#!/bin/bash
read -r n      # n -> number of operations
for((i=0;i<n;i++))
do
    read -r cmd filename
    if [ "$cmd" = "open" ]
    then
        if [ ! -f "$filename" ]   # if file doesn't exist
        then
            touch "$filename"
        fi
    elif [ "$cmd" = "append" ]
    then
    elif [ "$cmd" = "close" ]
    then
        
    fi
done

for i in "${descriptor[@]}"; do echo "$i"; done  
