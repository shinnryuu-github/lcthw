#!/bin/bash

for i in $(seq 1 10000); do
	echo $((RANDOM % 10000 + 1))
done | sort -R | tr '\n' ' '
