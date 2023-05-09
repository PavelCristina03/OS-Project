#!/bin/bash

#check args: expect one to be .c file; checks if is the correct number; checks file

if [ $# -ne 1 ]; then
    echo "Usage: $0 file.c"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: $1 is not a file"
    exit 1
fi

#compile
gcc -o compile -Wall "$1" > compiler_output.txt 2>&1

#check if compilation was successful
if [ $? -eq 0 ]; then
    #Count the number of errors and warnings in the compiler_output.txt file
    errors=$(grep -c 'error' compiler_output.txt)
    warnings=$(grep -c 'warning' compiler_output.txt)
    echo "Compilation successful. Executable file created: myprogram"
    echo "Number of errors: $errors"
    echo "Number of warnings: $warnings"
else
    echo "Compilation failed."
    exit 1
fi

# Remove the compiler_output.txt file
rm compiler_output.txt