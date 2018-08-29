#!/bin/bash
#if [ "$1" = "clean" ]; then
    echo "Deleting and recreating the build directory "
    rm -rf build
    mkdir build
#fi

cd build
export PKG_CONFIG_PATH=/opt/local/argos3/2.8.12.2/gcc/5.4.0/lib/pkgconfig
echo "Configuring Makefiles with CMAKE..."
cmake -DBUILD_EVOLVER=NO -DCMAKE_BUILD_TYPE=Release ..

echo "Making..."
make

cd ..

echo "Finished."

