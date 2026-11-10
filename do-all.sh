#!/bin/bash

LINES=${1:-1000}

echo "Generating $LINES lines..."
python3 ./Tools/message-generator.py $LINES

echo "Building the C++..."
cmake --build build

echo "Running the C++..."
build/app

echo "Comparing the outputs"
python3 ./Tools/check.py
