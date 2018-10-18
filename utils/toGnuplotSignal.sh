#!/bin/bash

cat $@ | awk 'BEGIN{printf "f(x)="} {printf "((x<%s)?%s:",T+$1,$2;T+=$1;}END{printf "null";while(NR-->0){printf ")"}}'
echo ''
echo "plot f(x) with lines title 'signal $\sigma$' lc 7 lw 3"
