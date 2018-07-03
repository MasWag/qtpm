#!/bin/bash

readonly RUN_TIMES=20

readonly date_str=$(date +%m%d%H%M%S)
readonly DATA_DIR=data/10.0
mkdir -p log/$date_str

if [[ $(uname) == Darwin ]]; then
    GTIME=/usr/local/bin/gtime
else
    GTIME=/usr/bin/time
fi

cd $(dirname $0)

cd ../build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j3 qtpm && cd -

## Settling
for input in $DATA_DIR/*.tsv; do
    for t in $(seq $RUN_TIMES); do
        $GTIME -v -o log/$date_str/qtpm-${input##*/}-$t.log ../build/qtpm -i $input -f ./saturation.dot --abs > /dev/null
    done
done
