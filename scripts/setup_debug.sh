#!/bin/bash

# Move to the project root.
cd "$(dirname "$0")/.." || exit

echo "Working in project root: $(pwd)"

if [ ! -d "build" ]; then
  mkdir build
  echo "Created build directory at root."
fi

cd build || exit
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
# This uses all available CPU cores for speed
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

echo "--- Build Complete ---"
