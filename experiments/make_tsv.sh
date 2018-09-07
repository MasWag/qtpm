#!/bin/bash

cd $(dirname $0)
readonly dir=$(dirname $1)

# $ ./make_tsv.sh ./log/0704051325/qtpm-BrCCPulse.tsv

if [[ $(uname) == Darwin ]]; then
    GGREP=/usr/local/bin/ggrep
else
    GGREP=/usr/bin/grep
fi

for file in $dir/$2*.log; do
    basename $file | $GGREP -o '[0-9.e+]*[0-9]' | xargs | awk '{print $1}'
    ./extract_execution_time.sh $file
    ./extract_max_res_set.sh $file
done | xargs -n 3 | awk 'BEGIN{OFS="\t"}{c[$1]+=1;time[$1]+=$2;mem[$1]+=$3}END{{for (key in c){print key,time[key]/c[key],mem[key]/c[key]}}}' | sort -g > $1

# plot "./log/0704093418/qtpm-BrCCPulseDensity.tsv" using (6000.0/$1):2 with lp
# plot "./log/0704051325/qtpm-BrCCPulse.tsv" u 1:3 w lp
