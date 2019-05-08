#!/bin/bash

echo 'set sample 300'
echo 'set isosample 300,300'
echo 'set pm3d map'
echo 'set size ratio -1'
echo 'set grid lw 2'
echo 'set xlabel "t" font "XITS,40" offset 8.0,2.2'
echo "set ylabel \"t'\" font \"XITS,40\" offset 1.3,2.7 rotate by 0"

echo "set palette defined ( -30 '#66FFFF', 10 '#000099')"
sed '/===/ d;s/----*//g;s/Weight://;s/$/ \&\&/' |
    gsed 'N;N;N;s/\n/ /g'  | sort -rn |
    sed "s/t' - t/y - x \&\& y - x/;s/t'/y \&\& y/;s/ t / x \&\& x /;" |
    awk 'BEGIN{ORS=" ";print "f(x,y)="}{NF-=1;fml=$0;sub(/^[^&]*\&\& /,"",fml);print "(",fml,")?"$1":"}END{print "1/0\n"}'
echo "splot [$1] [$1] f(x,y) notitle"
