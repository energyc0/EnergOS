#!/bin/bash

set -euo pipefail

cd "$(dirname "$0")/.."

cmake -B build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_TOOLCHAIN_FILE=toolchain/cortex-m4.cmake \
  -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build/tests --output-on-failure