#!/bin/bash

mkdir -p build
cd       build
conan install .. --build missing -s build_type=Debug
cmake ..
cd ..
