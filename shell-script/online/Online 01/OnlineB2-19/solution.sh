#!/bin/bash

# make an array of all virusnames
virus=()
while IFS= read -r virusname; do
    virus+=("$virusname")
done < "b2/virus.txt"
virus+=($(tail -n 1 "b2/virus.txt"))

# copy all files that match virusnames
mkdir -p b2/solve
find b2/in_dir -type f -wholename "*.*" | while read -r f;do
    f2="${f%.*}"
    base=$(basename "$f2")
    for v in "${virus[@]}";do
        if [ "$v" = "$base" ]
        then
            cp "$f" b2/solve
        fi
    done
done

cd b2/solve || exit

# rename files
k=0
for f in $(ls -tr);do
    base=$(basename "$f")
    dn=$(dirname "$f")
    newname="$k"
    newname+="__"
    newname+="$base"
    mv "$f" "$dn/$newname"
    ((k++))
done


cd - || exit
