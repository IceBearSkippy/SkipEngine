#!/bin/bash

source source-env.sh;
cmake -DCMAKE_C_COMPILER_ID=gcc -DCMAKE_CXX_COMPILER_ID=g++ -B./build;

cd build;
make;
