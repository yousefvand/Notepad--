#!/usr/bin/env bash

# Function to convert human-readable sizes to bytes
convert_to_bytes() {
    local size=$1
    local multiplier=1

    case "${size: -2}" in
        KB|kb) multiplier=1024; size=${size%??} ;;
        MB|mb) multiplier=$((1024 * 1024)); size=${size%??} ;;
        GB|gb) multiplier=$((1024 * 1024 * 1024)); size=${size%??} ;;
        B|b) multiplier=1; size=${size%?} ;;
        *) echo "Invalid size format: $size"; exit 1 ;;
    esac

    echo $((size * multiplier))
}

# Check if file size is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <file size> (e.g., 2MB, 30KB, 1GB)"
    exit 1
fi

# Convert human-readable size to bytes
FILE_SIZE_BYTES=$(convert_to_bytes "$1")
OUTPUT_FILE="random_file.txt"

# Generate random characters
LC_ALL=C tr -dc 'a-zA-Z0-9 \n' </dev/urandom | head -c "$FILE_SIZE_BYTES" > "$OUTPUT_FILE"

echo "Generated random file: $OUTPUT_FILE with size: $1 ($FILE_SIZE_BYTES bytes)"
