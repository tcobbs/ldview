#!/bin/sh
cd ../Icons
rm -f ../QT/images/error_*.png
for i in `ls error*.bmp| grep -v mask| sed 's/\.bmp//'` ; do
bmptoppm $i.bmp | pnmtopng -transparent rgb:00/00/00 > ../QT/images/$i.png
done
