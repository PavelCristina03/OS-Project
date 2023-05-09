#!/bin/bash

# Check that a filename has been provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <filename.c>"
    exit 1
fi

# Compile the .c file and redirect output to a temporary file
gcc -Wall -Wextra "$1" 2> temp.txt

# Count the number of warnings and errors in the temporary file
num_warnings=$(grep -c warning temp.txt)
num_errors=$(grep -c error temp.txt)

# Print the number of warnings and errors
echo "$num_warnings"
echo "$num_errors"

# Clean up the temporary file
rm temp.txt