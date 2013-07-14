#!/bin/bash

for f in $(find . -name "*.h" -or -name "*.cpp")
do 
	wc -l $f
done | awk '{sum += $1; } END { print sum }'
