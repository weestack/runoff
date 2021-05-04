#!/bin/sh

basedir=$(pwd)
export RUNOFF=$basedir/runoff

tests=$(find tests/ -maxdepth 1 -path "tests/test*" -type d | sed 's,tests/test,,' | sort -g)

testcount=$(echo $tests | wc -w)
passcount=0
failcount=0

for testnum in $tests
do
	echo -n "Running test $testnum... "

	cd tests/test$testnum
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

passrate=$((echo "scale=2" ; echo "100 * $passcount / $testcount") | bc)
echo "Test results:"
echo "	total:  $testcount"
echo "	passed: $passcount"
echo "	failed: $failcount"
echo "	pass rate: $passrate%"
