#!/bin/bash
for ((i=1;i<=$1;i++))
do
    echo "Running $i of $1"
    time argos3 -l argos_log.txt -c $2 >> $3
done
