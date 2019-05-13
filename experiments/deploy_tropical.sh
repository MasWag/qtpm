#!/bin/bash

if (($# < 2)); then
    echo "Usage: $0 directory type"
    exit 1
fi

if [[ "$2" == length ]]; then
    make -j $1/qtpm-all-BrCCPulse.tsv &&
        cp $1/qtpm-all-BrCCPulse.tsv ~/Papers/qtpm/dat/qtpm-all-BrCCPulse-tropical.tsv
elif [[ "$2" == density ]]; then
    make -j $1/qtpm-all-BrCCPulseDensity.tsv &&
        cp $1/qtpm-all-BrCCPulseDensity.tsv ~/Papers/qtpm/dat/qtpm-all-BrCCPulseDensity-tropical.tsv
elif [[ "$2" == unbounded ]]; then
    make -j $1/qtpm-overshoot_unbounded-BrCCPulse.tsv &&
        cp $1/qtpm-overshoot_unbounded-BrCCPulse.tsv ~/Papers/qtpm/dat/qtpm-all-BrCCPulseUnbounded-tropical.tsv
else
    echo 'type should be length, density, or unbounded'
    exit 1
fi
