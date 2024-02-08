#!/bin/bash
if [ "$1" == "remove" ]; then
    rm -rf temp
else
    mkdir -p temp
    ./build/bin/examples/pke/simple-real-numbers-evalout
    mv input_c1.txt input_c2.txt output_*.txt temp/

    for file in temp/*; do
        python3 numtofile.py "$file"
    done
fi
