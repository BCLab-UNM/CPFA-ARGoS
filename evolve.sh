#!/bin/bash
rm nohup.out
nohup bash -c "mpirun -n $1 -machinefile $2 ~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 50 -g 100 -c 0.1 -m 0.1 -s 0.2 -e 1 -x experiments/CPFACEvolution.xml seed 1111 && cat evolution_clustered.txt | mail -s \"CPFA Evolution for CLusered Report\" matthew@fricke.co.uk" &
#nohup bash -c "~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 30 -g 30 -c 0.1 -m 0.1 -s 0.5 -e 1 && cat evolution.txt | mail -s \"CPFA Evolution Report\" matthew@fricke.co.uk" &
