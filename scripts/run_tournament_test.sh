#!/bin/bash

gcc \
    -Wall \
    -Wextra \
    -Wpedantic \
    -std=c17 \
    -Iinclude \
    src/simulation.c \
    src/tournament.c \
    src/file_io.c \
    src/utility.c \
    src/validation.c \
    src/test.c \
    tests/test_utils.c \
    tests/test_tournament.c \
    -o test

if [ $? -eq 0 ]; then
    ./test
fi
