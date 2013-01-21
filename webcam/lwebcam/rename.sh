#!/bin/bash
c=0
for x in $(ls *.png); do
	c=$(($c+1))	
	mv $x file$c.png;
done
