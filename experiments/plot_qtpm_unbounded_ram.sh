#!/bin/bash

# $1: file_name
if (($# < 2)); then
    echo "Usage: $0 input_file [output_file]" # [plot_type]
    exit 1
fi

gnuplot <<EOF
load 'common.plt'
set output "$2"
set xlabel 'Duration of the Signal [s]'
set ylabel 'Memory Usage [KB]'
plot "$1" u 1:3 w lp title '\textsc{Overshoot (Unbounded)}'
EOF
