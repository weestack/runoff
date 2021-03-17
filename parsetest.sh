#!/bin/sh

# Clean and make
make clean && make
ERRORS=0
PARSED=0
# Loop over each file
for FILE in tests_programs/*
    do
        # echo out file name
        # echo "Testing File " $FILE;
        # pass file into ./runoff
        OUTPUT=$(cat $FILE | ./runoff >&1)

        if [ $? != 0 ]
        then
            ERRORS=$(echo $ERRORS + 1 | bc )
            printf 'File: %s Failed with error: \n%s\n' "$FILE" "$OUTPUT"
        else
            PARSED=$(echo $PARSED + 1 | bc )
        fi
    done;


TOTAL=$(echo $PARSED + $ERRORS | bc )
printf 'Parsed %s files where %s files were ok and %s files had errors.\n ' "$TOTAL" "$PARSED" "$ERRORS"
