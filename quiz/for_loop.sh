#!/bin/bash
echo "Number of commandline args : $#"
i=1
for arg in $*
do
    echo "Argument No. $i is $arg"
    ((i++))
done

for file in "function.sh" "func_ret.sh"
do
    printf "\nContent: " >> "$file"
done
