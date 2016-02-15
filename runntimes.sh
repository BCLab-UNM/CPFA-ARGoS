#!/bin/bash
for ((i=1;i<$1;i++))
do
    echo "Running $i of $1"
   argos3 -l argos_log.txt -c experiments/CPFA.xml >> results.txt
done
