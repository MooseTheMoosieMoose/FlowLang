#!/bin/bash

cd build
cmake -G Ninja ..
cmake --build .
echo "Finished!"
bash ./bin/FlowLang