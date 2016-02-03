#!/bin/bash
for i in {1..100}
do
 sleep 1
 argos3 -c experiments/DSA.xml
done
