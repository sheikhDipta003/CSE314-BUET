#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 old_extension new_extension"
    exit 1
fi

old_extension="$1"
new_extension="$2"

# Loop through each file with the old extension
for file in *"$old_extension"; do
    if [ -f "$file" ]; then
        new_name="${file%"$old_extension"}$new_extension"
        mv "$file" "$new_name"
        echo "Renamed: $file -> $new_name"
    fi
done

echo "File extension change complete."
