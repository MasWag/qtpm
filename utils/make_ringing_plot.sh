#!/bin/bash

cd $(dirname $0)

head -n 20 ../experiments/data/10.0/BrCCPulse-60000.tsv | ../build/qtpm -af ../experiments/ringing.dot | ./toGnuplot.sh 0:120 | cat ./output_pdf.plt <(echo 'set output "example_ringing.pdf"') - | gnuplot
pdfcrop example_ringing.pdf
mv example_ringing-crop.pdf example_ringing.pdf
