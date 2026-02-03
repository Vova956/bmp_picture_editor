#!/bin/bash
echo "Compiling and running Image Utility tests on Linux..."

mkdir -p bin/linux

gcc -Wall -Wextra -std=c11 -Iinclude src/image.c src/test.c -o bin/linux/test_all -lm

if [ $? -eq 0 ]; then
    echo "Compilation successful."
    ./bin/linux/test_all
else
    echo "Compilation failed."
    exit 1
fi