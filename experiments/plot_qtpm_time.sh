#!/bin/bash

# $1: file_name
if (($# < 2)); then
    echo "Usage: $0 input_file [output_file]" # [plot_type]
    exit 1
fi

gnuplot <<EOF
load 'common.plt'
set output "$2"
set xlabel 'Number of entries of the signal [$\times 10,000$]'
set ylabel 'Execution time [s]'
plot "$1" u (\$1/100000):2 with lp lw 5 pt 2 ps 2 title '\textsc{Overshoot}',  "$1" u (\$1/100000):4 with lp lw 5 pt 7 ps 2 title '\textsc{Ringing}'
EOF
