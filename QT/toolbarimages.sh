#!/bin/sh
rm images/toolbar_*.png
X=0
for i in reload wireframe seams edge privsubs snapshot view light preferences bfc ; do
Popup=0
case $i in ( view ) Popup=1;; esac
if [ $Popup -eq 1 ]; then
	bmptoppm ../Icons/toolbar.bmp | pnmcut -left $X -top 0 -width 16 -height 16\
	| pnmcat -lr - images/toolbar_popup_arrow.ppm | pnmtopng -transparent\
	rgb:c0/c0/c0 >images/toolbar_$i.png
else
	bmptoppm ../Icons/toolbar.bmp | pnmcut -left $X -top 0 -width 16 -height 16\
	| pnmtopng -transparent rgb:c0/c0/c0 >images/toolbar_$i.png
fi
X=`expr $X + 16`
done
