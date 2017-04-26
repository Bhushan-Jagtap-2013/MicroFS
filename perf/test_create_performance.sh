#!/usr/bin/bash
set -x
for((i = 0; i < 60; i++))
do
	echo "creating file : $i"
	touch $1/$i
done
