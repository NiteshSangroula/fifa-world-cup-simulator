#!/bin/bash

gcc \
    -Wall \
    -Wextra \
    -Wpedantic \
    -std=c17 \
    -Iinclude \
    src/display.c \
    src/validation.c \
    src/simulation.c \
    src/test.c \
    tests/test_utils.c \
    tests/test_display.c \
    -o test

if [ $? -eq 0 ]; then
    ./test
fi
