#!/bin/bash
for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    ./runntimes.sh 100 experiments/CPFAPL${i}Searchers.xml final_results/CPFAPL${i}Searchers.txt
done

for i in 0 1 2 3 4 5 6 7 8 9 10
do
    ./runntimes.sh 100 experiments/CPFAPLError${i}.xml final_results/CPFAPLError${i}.txt
done

for i in 0 1 2 3 4 5 6 7 8 9 10
do
    ./runntimes.sh 100 experiments/CPFACError${i}.xml final_results/CPFACError${i}.txt
done

for i in 0 1 2 3 4 5 6 7 8 9 10
do
    ./runntimes.sh 100 experiments/CPFAUError${i}.xml final_results/CPFAUError${i}.txt
done

