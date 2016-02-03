#!/bin/bash
#if [ "$1" = "clean" ]; then
    echo "Deleting and recreating the build directory "
    rm -rf build
    mkdir build
#fi

echo "Compiling ga-mpi library..."
cd source
make clean
make lib
cd ..

cd build
echo "Configuring Makefiles with CMAKE..."
cmake ..

echo "Making code..."
make
cd ..

echo "Finished. Call run.sh to run the CPFA 
or evolve.sh to run the openmpi genetic algorithm 
to optimize CPFA paramters."

