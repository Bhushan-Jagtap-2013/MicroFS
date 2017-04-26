#!/usr/bin/bash
for((i = 0; i<30000; i++))
do
	echo "creating file : $i"
	touch $1/$i
	echo "deleting file : $i"
	rm -rf $1/$i
done
