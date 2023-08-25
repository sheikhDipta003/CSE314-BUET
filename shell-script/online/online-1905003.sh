#!/bin/bash

# copy all files that match
rm -rf "$2" || rmdir "$2"
mkdir -p "$2"

find "$1" -type f | while read -r myfile;do
    k=$(file "$myfile" | grep "ASCII text" | wc --lines)
    if [ $k -gt 0 ];then
        cp "$myfile" "$2"
    fi
done

# rename files
cd "$2"
sorted_files=($(ls -1 | xargs -I {} wc -l {} | sort -n -k 1 | awk '{print $2}'))
k=0
for myfile in ${sorted_files[@]}; do
    new_filename="$k__${myfile##*/}.txt"
    mv "$myfile" "$new_filename"
    ((k++))
done

cd -

