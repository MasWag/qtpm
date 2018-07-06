#!/bin/bash


echo 'set sample 300'
echo 'set isosample 300,300'
echo 'set pm3d map'
echo 'set size ratio -1'
echo 'set grid on'
#echo 'set xlabel font "Arial,20"'
#echo 'set ylabel font "Arial,20"'
#echo 'set xlabel "t"'
#echo "set ylabel \"t'\""

echo "set palette defined ( -30 '#66FFFF', 10 '#000099')"
sed '/===/ d;s/----*//g;s/Weight://;s/$/ \&\&/' |
    gsed 'N;N;N;s/\n/ /g' |
    sed "s/t' - t/y - x \&\& y - x/;s/t'/y \&\& y/;s/ t / x \&\& x /;" |
    awk 'BEGIN{ORS=" ";print "f(x,y)="}{NF-=1;fml=$0;sub(/^[^&]*\&\& /,"",fml);print "(",fml,")?"$1":"}END{print "1/0\n"}'
echo 'splot [0:6.5] [0:6.5] f(x,y) notitle'
