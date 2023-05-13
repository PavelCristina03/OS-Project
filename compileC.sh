#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 file.c"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: $1 is not a file"
    exit 1
fi

output=$(gcc -Wall "$1" 2>&1)
error_count=$(echo "$output" | grep -c 'error:')
warning_count=$(echo "$output" | grep -c 'warning:')
echo "$error_count $warning_count"
