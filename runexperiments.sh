%#!/bin/bash
for i in 1 2 4 6 8 10 15 20 25 30
do
    ./runntimes.sh $1 experiments/DDSAPL${i}Searcher.xml results/DDSAPL${i}Searchers.txt
done

for i in 0 2 4 6 8 10 15 20 25 30
do
    ./runntimes.sh $1 experiments/DDSAPLError${i}.xml results/DDSAPLError${i}.txt
done

for i in 20 40 60 80 100 150 200 250
do
    ./runntimes.sh $1 experiments/DDSAU60minTargets${i}.xml results/DDSAU60minTargets${i}.txt
done

for i in 10 12 14 16 18 20
do
    ./runntimes.sh $1 experiments/DDSAU60minSize${i}x${i}.xml results/DDSAU60minSize${i}x${i}.txt
done

