#!/bin/bash
mpirun -n $1 -machinefile $2 build/cpfa_evolver
