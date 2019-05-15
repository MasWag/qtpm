#!/bin/bash

cd $(dirname $0)

head -n 70 ../experiments/data/10.0/BrCCPulse-60000.tsv | tail -n 30 | ../build/qtpm -af ../experiments/overshoot_unbounded.dot --minplus | ./toGnuplot.sh 450:700 | cat ./output_pdf_experiment.plt <(echo 'set output "example_overshoot_unbounded_tropical.pdf"';echo 'set xtics 450,100'; echo 'set ytics 450,100'; echo "set cbtics ('0' 0, '30' 30, '60' 60, '90' 90, '120' 120);") - | gnuplot
pdfcrop example_overshoot_unbounded_tropical.pdf
mv example_overshoot_unbounded_tropical-crop.pdf example_overshoot_unbounded_tropical.pdf
