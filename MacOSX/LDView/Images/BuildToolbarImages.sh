#! /bin/sh
cd ../../../Icons
for i in `ls toolbar_*.png` ; do
convert $i -density 0 -bordercolor 'rgba(0,0,0,0)' -border '3x0' ../MacOSX/LDView/Images/$i
done
