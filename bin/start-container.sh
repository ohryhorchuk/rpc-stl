#!/bin/sh

docker run -v $PWD:/rpc-stl -it cpp-build-image scl enable devtoolset-7 bash