#!/bin/bash

cd $(dirname $0)

head -n 20 ../experiments/data/10.0/BrCCPulse-60000.tsv | ../build/qtpm -af ../experiments/ringing.dot --minplus | ./toGnuplot.sh 0:120 | cat ./output_pdf_experiment.plt <(echo 'set output "example_ringing_tropical.pdf"';echo 'set xtics 0,30'; echo 'set ytics 0,30'; echo "set cbtics ('-10' -10, '-13' -13, '-16' -16, '-19' -19);") - | gnuplot
pdfcrop example_ringing_tropical.pdf
mv example_ringing_tropical-crop.pdf example_ringing_tropical.pdf
