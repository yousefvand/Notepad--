#!/usr/bin/env bash

file="1GB.txt"

# Define the size in byte
s (1 GB) size=$((1 * 1024 * 1024 * 1024))

# Generate the random file < /dev/urandom 
tr -dc 'a-zA-Z0-9 ' | head -c "$size" > "$file"

echo "Done!"





