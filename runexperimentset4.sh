#!/bin/bash
for i in 10 12 14 16 18 20
do
    ./runntimes.sh $1 experiments/CPFAU60minArea${i}x${i}.xml results/CPFAU60minArea${i}x${i}.txt
done
