#!/bin/bash

awk '{exit NF!=4}' "$1" && # It appends the result only if the number of fields is 4
    paste "$1" <(sed 1d "$1") | # It juxtapose a copy of the log without the first line
        awk 'BEGIN{OFS="\t"}{print $5,$6,$7,$8,$6-$2}' | # It calculates the difference
        sponge "$1" # It writes the content to the original file.
