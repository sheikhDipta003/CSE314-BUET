#!/bin/bash
for f in *.txt
do
 [[ -e "$f" ]] || break
 result=$(tail -n 2 "$f" | head -n 1)
 mkdir -p "$result"
 mv "$f" "$result"
done
