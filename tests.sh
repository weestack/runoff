#!/bin/sh

# Clean and make
make clean && make

# Loop over each file
for FILE in tests_programs/*
    do
        # echo out file name
        echo "Testing File " $FILE;
        # pass file into ./runoff
        cat $FILE | ./runoff
    done;
