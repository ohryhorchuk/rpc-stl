#!/bin/sh

rm -f -R build/
mkdir build
cd build

cmake ../  -Wdev  -DCMAKE_BUILD_TYPE=Debug
make