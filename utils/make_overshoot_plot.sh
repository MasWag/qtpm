#!/bin/bash

cd $(dirname $0)

head -n 70 ../experiments/data/10.0/BrCCPulse-60000.tsv | tail -n 30 | ../build/qtpm -af ../experiments/overshoot.dot | ./toGnuplot.sh 450:700 | cat ./output_pdf.plt <(echo 'set output "example_overshoot.pdf"') - | gnuplot
pdfcrop example_overshoot.pdf
mv example_overshoot-crop.pdf example_overshoot.pdf
