#!/bin/bash

readonly RUN_TIMES=20

readonly date_str=$(date +%m%d%H%M%S)
readonly DATA_DIR=data/10.0_short
readonly LOG_DIR="log/$date_str"

cd $(dirname $0)

## Make short data
mkdir -p $DATA_DIR
for length in $(seq 100 100 1000); do
     head -n $length ./data/10.0/BrCCPulse-60000.tsv > $DATA_DIR/BrCCPulse-${length}0.tsv
done

mkdir -p $LOG_DIR

if [[ $(uname) == Darwin ]]; then
    GTIME=/usr/local/bin/gtime
else
    GTIME=/usr/bin/time
fi

cd ../build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j3 qtpm && cd -

## Overshoot
for input in $DATA_DIR/*.tsv; do
    for t in $(seq $RUN_TIMES); do
        $GTIME -v -o "$LOG_DIR/qtpm-overshoot_unbounded-${input##*/}-$t.log" ../build/qtpm -i "$input" -f ./overshoot_unbounded.dot --abs --minplus > /dev/null
    done
done
