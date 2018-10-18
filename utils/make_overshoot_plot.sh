#!/bin/bash

cd $(dirname $0)

head -n 70 ../experiments/data/10.0/BrCCPulse-60000.tsv | tail -n 30 | ../build/qtpm -af ../experiments/overshoot.dot | ./toGnuplot.sh 450:700 | cat ./output_pdf_experiment.plt <(echo 'set output "example_overshoot.pdf"'; echo 'set xtics 450,100'; echo 'set ytics 450,100'; echo "set cbtics ('-2' -2, '-6' -6, '-10' -10, '-14' -14, '-18' -18);") - | gnuplot
pdfcrop example_overshoot.pdf
mv example_overshoot-crop.pdf example_overshoot.pdf
