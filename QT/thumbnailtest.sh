#!/bin/sh
./LDView ../8464.mpd -SaveSnapshot=/tmp/8464.png \
-SaveWidth=128 -SaveHeight=128 -ShowErrors=0 -SaveActualSize=0
ls -l /tmp/8464.png
