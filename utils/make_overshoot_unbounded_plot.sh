#!/bin/bash

cd $(dirname $0)

head -n 70 ../experiments/data/10.0/BrCCPulse-60000.tsv | tail -n 30 | ../build/qtpm -af ../experiments/overshoot_unbounded.dot | ./toGnuplot.sh 450:700 | cat ./output_pdf.plt <(echo 'set output "example_overshoot_unbounded.pdf"') - | gnuplot
pdfcrop example_overshoot_unbounded.pdf
mv example_overshoot_unbounded-crop.pdf example_overshoot_unbounded.pdf
