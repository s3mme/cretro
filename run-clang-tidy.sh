#!/bin/sh

# Clean up before regenerating compilation files
make clean

# Check whether bear (https://github.com/rizsotto/Bear) is installed
# This is required to generate compile_commands.json that clang-tidy requires
if ! [ -x "$(command -v bear)" ]; then
    echo "bear could not be found. Please install it."
    exit
fi

bear -- make

find src/ -iname *.h -o -iname *.c | xargs clang-tidy --fix --fix-errors --config-file=.clang-tidy