#!/bin/bash
# This script runs the XML file/ argos file x100 with a new random seed each time by using
# the random_argos.py by Jeffrey Nichol.


#Generate Data for Random Distribution Stimulation
for _ in {1..20}; do python ga.py -f $1; done #command to run file n times
for _ in {1..20}; do python ga.py -f $2; done #command to run file n times
for _ in {1..20}; do python ga.py -f $3; done #command to run file n times
echo "DONE"

 




