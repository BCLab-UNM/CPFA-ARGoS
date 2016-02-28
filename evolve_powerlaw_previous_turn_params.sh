#!/bin/bash
nohup bash -c "mpirun -n $1 -machinefile $2 ~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 50 -g 100 -c 0.1 -m 0.1 -s 0.2 -e 1 -x experiments/CPFAPreviousTurning.xml seed 1111 && cat evolution_powerlaw_previous.txt | mail -s \"CPFA Evolution for Power Law Report\" matthew@fricke.co.uk" > evolution_powerlaw_previous.txt &
#nohup bash -c "~/GitHub/CPFA-ARGoS/build/cpfa_evolver -t 10 -p 30 -g 30 -c 0.1 -m 0.1 -s 0.5 -e 1 && cat evolution.txt | mail -s \"CPFA Evolution Report\" matthew@fricke.co.uk" &
