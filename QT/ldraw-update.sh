#!/bin/sh
COMPLETE=/usr/share/ldraw/complete.zip
if [ ! -f $COMPLETE ] ; then
	wget -nv https://library.ldraw.org/library/updates/complete.zip \
		-O $COMPLETE
	exit
fi
VER=`unzip -l $COMPLETE ldraw/models/Note* |\
grep models/ |awk ' {print $4}'|\
sed 's|ldraw/models/Note||'|\
sed 's/\.txt//g' |sed 's/CA$//g' | sort -n| tail -1`
VERSION=`unzip -p $COMPLETE ldraw/models/Note${VER}CA.txt | head -1 | grep -o '[0-9]*-[0-9]*'`
LATEST=`wget https://library.ldraw.org/updates?output=tab -q -O -| grep COMPLETE|cut -f2`
echo Current version: $VERSION
echo Latest version:  $LATEST
if [ $LATEST = $VERSION ] ; then
	true
else
	wget -nv https://library.ldraw.org/library/updates/complete.zip \
		-O $COMPLETE
fi
