#!/bin/bash

cd $(dirname $0)

../build/qtpm -i ../example/signal.txt -f ../example/small.dot | ./toGnuplot.sh | cat output_pdf.plt - | gnuplot 
pdfcrop example_qtpm.pdf
mv example_qtpm-crop.pdf example_qtpm.pdf
