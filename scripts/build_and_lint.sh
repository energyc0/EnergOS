#!/bin/bash

set -uo pipefail

cd "$(dirname "$0")/.."

status=0

if cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE=toolchain/cortex-m4.cmake;
then
    echo "Configured CMake project successfully."
    if cmake --build build; then
        echo "Built project successfully."
    else
        echo "Failed to build project!"
        status=1
    fi
else 
    echo "Failed to configure CMake project!"
    status=1
fi

FILES=$(find kernel/ portable/ user/ util/ -type f \( -name '*.c' -o -name '*.h' \) \
            ! -path '*/third_party/*')

echo "Checking files: $FILES"

if run-clang-tidy -p=build -quiet $FILES; then
    echo "run-clang-tidy - success."
else
    echo "run-clang-tidy - fail!"
    status=1
fi

if echo $FILES | xargs clang-format-22 --dry-run --Werror --Wno-error=unknown; then
    echo "clang-format-22 - success."
else
    echo "clang-format-22 - fail."
    status=1
fi

exit $status