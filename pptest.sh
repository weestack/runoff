#!/bin/sh

tmpdir=$(mktemp -d)

# Loop over each file
for FILE in tests_programs/*
	do
		file=$(basename $FILE)
		./runoff -p $FILE > $tmpdir/$file-1
		./runoff -p $tmpdir/$file-1 > $tmpdir/$file-2
		diff -Naur $tmpdir/$file-1 $tmpdir/$file-2
	done;
