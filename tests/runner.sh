#!/bin/sh

basedir=$(pwd)
export RUNOFF=$basedir/runoff

tests=$(find tests/ -maxdepth 1 -path "tests/test*" -type d)

testcount=$(echo $tests | wc -w)
passcount=0
failcount=0

for tdir in $tests
do
	number=$(echo $tdir | sed 's,tests/test,,')
	echo -n "Running test $number... "

	cd $tdir
	outfile=$(mktemp)
	sh run > $outfile
	if [ -n "$(diff $outfile output)" ]
	then
		failcount=$(echo $failcount + 1 | bc)
		echo FAIL
		echo "	Test description: $(cat desc)"
		echo "	Expected output file: $(realpath output)"
		echo "	Actual output file:   $outfile"
	else
		passcount=$(echo $passcount + 1 | bc)
		echo PASS
		rm $outfile
	fi
	cd $basedir
done

echo "Test results: total=$testcount pass=$passcount fail=$failcount"