#!/bin/bash

for f in n*.txt
do 
  for n in 2 4 8 16 32
  do 
  (time mpirun -disable-hostname-propagation -n $n -f ../maquinas.txt ../bombing_fields $f) > ../output/${f}_${n}nodes.txt 2>&1
  done
done