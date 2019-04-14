#!/bin/bash

# $1: file_name
if (($# < 2)); then
    echo "Usage: $0 input_file [output_file]" # [plot_type]
    exit 1
fi

gnuplot <<EOF
load 'common.plt'
set output "$2"
set xlabel 'Sampling Frequency [Hz]'
set ylabel 'Execution Time [s]'
plot "$1" u (6000/\$1):2 with lp lw 5 pt 2 ps 2 title '\textsc{Overshoot}',  "$1" u (6000/\$1):4 with lp lw 5 pt 7 ps 2 title '\textsc{Ringing}'
EOF
