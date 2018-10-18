#!/bin/bash

cd $(dirname $0)

../build/qtpm -i ../example/paper.txt -f ../example/paper.dot | ./toGnuplot.sh 0:30.5 | cat output_pdf.plt - | gnuplot 
pdfcrop example_qtpm.pdf
mv example_qtpm-crop.pdf example_qtpm.pdf
