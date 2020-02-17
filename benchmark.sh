#!/bin/sh

COMPILE_CMD="g++ -std=c++17 -Ofast -pthread main.cpp -o prime-sieve -DBENCHMARK"

#-------------------------------------------------------------------------------

echo "Testing single-core performance"

for SIZE in 1024 2048 4096 8192 16384 32768
do
	eval ${COMPILE_CMD} "-DSIZE="${SIZE}" -DSINGLE_THREAD"
	TIME=0
	for i in 1 2 3 4
	do
		TIME=$((TIME+$(./prime-sieve)))
	done
	TIME=$((TIME/4000))
	echo $((SIZE/1024))"K:" $TIME "ms"
done

echo "Testing multi-core performance"

for SIZE in 1024 2048 4096 8192 16384 32768
do
	eval ${COMPILE_CMD} "-DSIZE="${SIZE}
	TIME=0
	for i in 1 2 3 4
	do
		TIME=$((TIME+$(./prime-sieve)))
	done
	TIME=$((TIME/4000))
	echo $((SIZE/1024))"K:" $TIME "ms"
done
