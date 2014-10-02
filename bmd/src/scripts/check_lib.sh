#!/bin/bash
regexp="(TOTALDEEDBEEFTHATSHOULDNOTHAPPEN"
for i in `nm $1 | grep " T " | cut -f3 -d' '`; do
regexp="$regexp|$i";
done
regexp="$regexp)";
grep -REl $regexp . 
#for j in `find .`; do
#	echo $j
#	grep $regexp $j
#done
