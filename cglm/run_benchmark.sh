#!/bin/sh

DEFAULT_COUNT=20000000

COUNT=$1
if [ -z "$COUNT" ]; then
  COUNT=$DEFAULT_COUNT
fi

echo "Simulating $COUNT GL view updates"
T=$(/usr/bin/time -f "%S %U" build/cglm_bench $COUNT 2>&1)

T_USER=$(echo $T | awk '{ print $1 }')
T_SYS=$(echo $T | awk '{ print $2 }')
T_TOTAL=$(echo $T_USER + $T_SYS | bc)
AVERAGE_SECONDS=$(echo $COUNT / $T_TOTAL | bc)

echo
echo "Benchmark have simulated $COUNT GL view updates in $T_TOTAL CPU seconds time"
echo "CGLM can compute a maximum average of $AVERAGE_SECONDS frames per seconds"
echo
