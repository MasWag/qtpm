#!/bin/bash

# $1: file_name
if (($# < 2)); then
    echo "Usage: $0 input_file [output_file]" # [plot_type]
    exit 1
fi

gnuplot <<EOF
load 'common.plt'
set output "$2"
set xlabel 'Number of entries of the signal [$\times 100$]'
set ylabel 'Memory Usage [MiB]'
plot "$1" u (\$1/1000):(\$3/1024) with lp lw 5 pt 3 ps 2 lc 3 title '\textsc{Overshoot (Unbounded)}'
EOF
