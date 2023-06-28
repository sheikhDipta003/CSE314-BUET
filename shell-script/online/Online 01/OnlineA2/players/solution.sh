#!/bin/bash

find . -wholename "*.txt" | while read -r f; do
    playername=$(sed -n '1p' "$f")
    country=$(sed -n '2p' "$f")
    role=$(sed -n '4p' "$f")

    # move the text files
    mkdir -p "$country/$role"
    mv "$f" "$country/$role/$playername.txt"

    #recursively delete the empty folders
    st=$(ls .)
    if [ -z "$st" ]
    then
        rm -r "$1"
    fi
done