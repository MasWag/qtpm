#!/bin/bash

cd $(dirname $0)

head -n 20 ../experiments/data/10.0/BrCCPulse-60000.tsv | ../build/qtpm -af ../experiments/ringing.dot $@ | ./toGnuplot.sh 0:120 | cat ./output_pdf_experiment.plt <(echo 'set output "example_ringing.pdf"';echo 'set xtics 0,30'; echo 'set ytics 0,30'; echo "set cbtics ('-8' -8, '-15' -15, '-22' -22, '-29' -29, '-35' -35);") - | gnuplot
pdfcrop example_ringing.pdf
mv example_ringing-crop.pdf example_ringing.pdf
