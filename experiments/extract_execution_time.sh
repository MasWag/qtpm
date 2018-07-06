#!/bin/bash

# NOTE: This cannot handle 'pq-$kind-$num.dat' used for patten 6

# output:  Epalsed_Time[second]
cat $@ | awk '/Elapsed/{print $NF}' | grep -o '[0-9\.:]\+[0-9]' | mawk -F : 'NF>1{$0=$1*60+$2}1;'
