#!/usr/bin/env bash

output_file="1GB.txt"

base64 /dev/urandom | head -c 1073741824 > "$output_file"

echo "Done!"
