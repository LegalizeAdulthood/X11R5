#!/bin/sh
for i in `cat filelist`
do
	IFS=">"
	set - $i
	echo "mv $1 $2"
	mv $1 $2
done
