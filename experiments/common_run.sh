#!/bin/bash

mkdir -p $LOG_DIR

if [[ $(uname) == Darwin ]]; then
    GTIME=/usr/local/bin/gtime
else
    GTIME=/usr/bin/time
fi

cd ../build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j3 qtpm && cd -

## Settling
# for input in $DATA_DIR/*.tsv; do
#     for t in $(seq $RUN_TIMES); do
#         $GTIME -v -o "$LOG_DIR/qtpm-settling-${input##*/}-$t.log" ../build/qtpm -i "$input" -f ./settling.dot --abs $@ > /dev/null
#     done
# done

## Overshoot
for input in $DATA_DIR/*.tsv; do
    for t in $(seq $RUN_TIMES); do
        $GTIME -v -o "$LOG_DIR/qtpm-overshoot-${input##*/}-$t.log" ../build/qtpm -i "$input" -f ./overshoot.dot --abs $@ > /dev/null
    done
done

## Ringing
for input in $DATA_DIR/*.tsv; do
    ./appendDiff.sh $input
    for t in $(seq $RUN_TIMES); do
        $GTIME -v -o "$LOG_DIR/qtpm-ringing-${input##*/}-$t.log" ../build/qtpm -i "$input" -f ./ringing.dot --abs $@ > /dev/null
    done
done
