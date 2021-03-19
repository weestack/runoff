#!/bin/sh

# Clean and make
make clean && make

tmpdir=$(mktemp -d)

# Loop over each file
for FILE in tests_programs/*
    do
    	file=$(basename $FILE)
        ./runoff $FILE --prettyprint > $tmpdir/$file-1
		./runoff $tmpdir/$file-1 --prettyprint > $tmpdir/$file-2
		diff -Naur $tmpdir/$file-1 $tmpdir/$file-2
    done;
