#!/bin/bash
JPEGVER=9f

if [ "$1" = -uninstall ] ; then
	git checkout -- ../../libjpeg/jpeglib.h
	git checkout -- ../../libjpeg/jmorecfg.h
	git checkout -- ../../libjpeg/jinclude.h
	git checkout -- ../../libjpeg/MacOSX/jconfig.h
	git checkout -- ../../../lib/MacOSX/libjpegu.a
	exit 0
fi

test -f jpegsrc.v$JPEGVER.tar.gz || curl -OJL https://www.ijg.org/files/jpegsrc.v$JPEGVER.tar.gz

test -d jpeg-$JPEGVER || tar zxf jpegsrc.v${JPEGVER}.tar.gz

cd jpeg-$JPEGVER
./configure CFLAGS="-arch x86_64 -arch arm64 -mmacosx-version-min=10.13" LDFLAGS="-arch x86_64 -arch arm64"
make
cp -af .libs/libjpeg.a ../../../../lib/MacOSX/libjpegu.a
cp -af jpeglib.h ../../../libjpeg/
cp -af jmorecfg.h ../../../libjpeg/
cp -af jinclude.h ../../../libjpeg/
cp -af jconfig.h ../../../libjpeg/MacOSX
