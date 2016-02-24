#!/bin/bash
for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    ./runntimes.sh 25 experiments/CPFAPL${i}Searchers.xml final_results/CPFAPL${i}Searchers.txt
done
