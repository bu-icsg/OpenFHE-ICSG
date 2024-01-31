#!/bin/bash

if [ "$1" == "remove" ]; then
	rm -f output.txt output_*.txt
else
	./build/bin/examples/pke/simple-real-numbers-evalout
	while [ ! -f output.txt ]; do

	  sleep 1

	done
	python3 numtofile.py
fi
