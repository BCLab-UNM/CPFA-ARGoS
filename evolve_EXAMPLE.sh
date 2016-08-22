#!/bin/bash
# First argument is the number of processes to generate
# Second argumen is the name of the machine file

nohup bash -c "mpirun -n $1 -machinefile $2 ~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 50 -g 100 -c 0.1 -m 0.1 -s 0.2 -e 1 -x experiments/ExampleEvolution.xml seed 1111 && cat example_evolution_output.txt | mail -s \"Example Evolution Description\" example@domain.com" > evolve_clustered.out &

