#!/bin/bash

gcc \
    -Wall \
    -Wextra \
    -Wpedantic \
    -std=c17 \
    -Iinclude \
    src/file_io.c \
    src/utility.c \
    src/validation.c \
    src/test.c \
    tests/test_utils.c \
    tests/test_file_io.c \
    -o test

if [ $? -eq 0 ]; then
    ./test
fi
