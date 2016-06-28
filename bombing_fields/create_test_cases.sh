#!/bin/bash
# Shell Script used to create test cases using the file testcase_builder.py

for n in 5 7 10
do
	for t in 50 100 1000 
	do
		for b in 1 5 10
		do
			b1000=`expr $b \* 1000`
			t1000=`expr $t \* 1000`
			n1000=`expr $n \* 1000`
			python testcase_builder.py $n1000 $t1000 $b1000 > tests/n${n}k_t${t}k_b${b}k.txt
  		done
	done
done