#!/bin/bash

cd $(dirname $0)

../build/qtpm -i ../example/paper.txt -f ../example/paper.dot | ./toGnuplot.sh 0:30.5 | cat output_pdf.plt - | sed 's/set xlabel.*$/set xlabel "t" font "XITS,30" offset 11.0,1.8/;s/set ylabel.*$/set ylabel "t′" font "XITS,30" offset 0.7,4.8 rotate by 0/;' |  gnuplot 
pdfcrop example_qtpm.pdf
mv example_qtpm-crop.pdf example_qtpm.pdf
