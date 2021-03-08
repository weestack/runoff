#!/bin/sh

make clean && make
for FILE in tests_programs/*
do
  echo "Testing File " $FILE;
  cat $FILE | ./runoff
done;
