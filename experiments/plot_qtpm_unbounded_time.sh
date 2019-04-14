#!/bin/bash

# $1: file_name
if (($# < 2)); then
    echo "Usage: $0 input_file [output_file]" # [plot_type]
    exit 1
fi

gnuplot <<EOF
load 'common.plt'
set output "$2"
set xlabel 'Duration of the signal [1,000 s]'
set ylabel 'Execution time [s]'
plot "$1" u (\$1/1000):2 with lp lw 5 pt 3 ps 2 lc 3 title '\textsc{Overshoot (Unbounded)}'
EOF
