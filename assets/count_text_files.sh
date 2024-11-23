#!/bin/bash

# Check if a path is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <directory-path>"
  exit 1
fi

# Assign the provided path to a variable
DIR="$1"

# Check if the provided path is a valid directory
if [ ! -d "$DIR" ]; then
  echo "Error: $DIR is not a valid directory."
  exit 1
fi

# Find files, check MIME type, and count text files
total_text_files=$(find "$DIR" -type f -exec file --mime-type {} + | grep -c "text/")

# Output the result
echo "Total text files in '$DIR' and its subdirectories: $total_text_files"
