#!/bin/bash
#if [ "$1" = "clean" ]; then
    echo "Deleting and recreating the build directory "
    rm -rf build
    mkdir build
#fi

cd build

echo "Configuring Makefiles with CMAKE..."
cmake ..

echo "Making code..."
make
cd ..

echo "Finished. Call run.sh to run the DDSA..."

