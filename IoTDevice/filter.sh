#! /bin/bash

soft_ln=$(find ./ -type l)

for sln in $soft_ln
do
	if [ -e $sln ]; then
		echo "$sln exist."
	else
		rm -f $sln
		echo "$sln not exist."
	fi
	
done