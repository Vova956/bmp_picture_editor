#!/bin/bash

echo "Compiling and running Image Utility (interactive CLI) on Linux..."

mkdir -p bin/linux

gcc -Wall -Wextra -std=c11 -Iinclude src/image.c src/main.c -o bin/linux/imagetool_script_main -lm

if [ $? -eq 0 ]; then
    echo "Compilation successful."
    ./bin/linux/imagetool
else
    echo "Compilation failed."
    exit 1
fi

#Usage
#chmod +x run_linux.sh
#./run_linux.sh