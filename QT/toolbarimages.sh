#!/bin/sh
rm images/toolbar_*.png
X=0
for i in reload wireframe seams edge privsubs snapshot view light preferences bfc ; do
bmptoppm ../Icons/toolbar.bmp | pnmcut -left $X -top 0 -width 16 -height 16 |\
pnmtopng -transparent rgb:c0/c0/c0 >images/toolbar_$i.png
X=`expr $X + 16`
done
