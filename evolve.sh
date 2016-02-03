#!/bin/bash
mpirun -n $1 -machinefile $2 ~/GitHub/CPFA-ARGoS/build/cpfa_evolver
