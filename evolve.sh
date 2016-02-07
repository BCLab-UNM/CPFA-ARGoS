#!/bin/bash
nohup mpirun -n $1 -machinefile $2 ~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 28 -g 100 -c 0.1 -m 0.1 -s 0.1 &
