#!/bin/bash

cd build
if [ "$#" -eq 1 ]; then
    echo "Building release"
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
else
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
fi
cmake --build .
echo "Finished!"

../bin/FlowLang