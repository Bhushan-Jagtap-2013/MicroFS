#!/usr/bin/bash
set -x
for((i = 0; i < 60; i++))
do
	echo "removing file : $i"
	rm -f  $1/$i
done
