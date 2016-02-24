#!/bin/bash
for i in 10 12 14 16 18 20
do
    ./runntimes.sh $1 experiments/CPFAU60minSize${i}x${i}.xml results/CPFAU60minSize${i}x${i}.txt
done
