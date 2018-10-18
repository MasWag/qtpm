#!/bin/bash

cd $(dirname $0)

./toGnuplotSignal.sh ../example/paper.txt | cat signal.plt - | gnuplot 
