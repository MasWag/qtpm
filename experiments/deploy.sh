#!/bin/bash

if (($# < 2)); then
    echo "Usage: $0 directory type"
    exit 1
fi

if [[ "$2" == length ]]; then
    make -j $1/qtpm-BrCCPulseTime.tikz.tex $1/qtpm-BrCCPulseRAM.tikz.tex $1/qtpm-all-BrCCPulse.tsv &&
        cp $1/qtpm-BrCCPulseTime.tikz.tex $1/qtpm-BrCCPulseRAM.tikz.tex ~/Papers/qtpm/figs/ &&
        cp $1/qtpm-all-BrCCPulse.tsv ~/Papers/qtpm/dat/
elif [[ "$2" == density ]]; then
    make -j $1/qtpm-BrCCPulseDensityTime.tikz.tex $1/qtpm-BrCCPulseDensityRAM.tikz.tex $1/qtpm-all-BrCCPulseDensity.tsv &&
        cp $1/qtpm-BrCCPulseDensityTime.tikz.tex $1/qtpm-BrCCPulseDensityRAM.tikz.tex ~/Papers/qtpm/figs/ &&
        cp $1/qtpm-all-BrCCPulseDensity.tsv ~/Papers/qtpm/dat/
else
    echo 'type should be length or density'
    exit 1
fi
