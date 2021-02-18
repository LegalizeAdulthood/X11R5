#!/bin/sh
for i in `cat filelist`
do
	IFS=">"
	set - $i
	echo "mv $2 $1"
	mv $2 $1
done
