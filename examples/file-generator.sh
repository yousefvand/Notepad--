#!/usr/bin/env bash

file="100MB.txt"

# Define the size in bytes (10 MB)
size=$((100 * 1024 * 1024))

# Pre-generate random data and then split it into lines
< /dev/urandom tr -dc 'a-zA-Z0-9 ' | head -c "$size" | awk -v min=40 -v max=400 '
{
    while (length($0) > 0) {
        len = int(min + rand() * (max - min + 1))
        print substr($0, 1, len)
        $0 = substr($0, len + 1)
    }
}
' > "$file"

echo "Done! The file $file has been created."
