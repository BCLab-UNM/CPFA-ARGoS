#!/bin/bash
#if [ "$1" = "clean" ]; then
    echo "Deleting and recreating the build directory "
    rm -rf build
    mkdir build
#fi

cd build

echo "Configuring Makefiles with CMAKE..."
cmake -DBUILD_EVOLVER=NO .. 

echo "Making..."
make

echo "Finished."

