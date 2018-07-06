#!/bin/bash

# NOTE: This cannot handle 'pq-$kind-$num.dat' used for patten 6

# output:  Max_Resident_set_size
cat $@ | awk '/Maximum resident set size/{print $NF}' | grep -o '[0-9\.:]\+[0-9]' | awk -F : 'NF>1{$0=$1*60+$2}1;'
