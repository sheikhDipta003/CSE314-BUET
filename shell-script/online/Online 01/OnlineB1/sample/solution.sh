#!/bin/bash
print_space()
{
    for((i=0;i<$1;i++))
    do
        printf '\t'
    done
}

unzip_files()
{
    dn=$(dirname "$1")
    unzip "$1" -d "$dn" > /dev/null 2>&1
    rm -rf "$1"
}

find . -wholename "**/*.zip" | while read -r f; do
    unzip_files "$f"
done

visit()
{
    print_space "$2"
    echo -n "|--"
    name=$(basename "$1")
    echo "$name"
    
    if [ -d "$1" ]
    then
        for i in "$1"/*
        do
            visit "$i" $(($2 + 1))
        done
    fi
}

visit "." 0