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

test -d jpeg-$JPEGVER-x64 || (tar zxf jpegsrc.v${JPEGVER}.tar.gz && mv jpeg-${JPEGVER} jpeg-$JPEGVER-x64 )
test -d jpeg-$JPEGVER-arm64 || (tar zxf jpegsrc.v${JPEGVER}.tar.gz && mv jpeg-${JPEGVER} jpeg-$JPEGVER-arm64 )

cd jpeg-$JPEGVER-x64
./configure --target x86_64-apple-darwin
make CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64"
cd ..
cd jpeg-$JPEGVER-arm64
./configure --target arm64-apple-darwin
make CFLAGS="-arch arm64" LDFLAGS="-arch arm64"
cd ..
strip jpeg-$JPEGVER-*/.libs/libjpeg.a
libtool -o libjpegu.a jpeg-$JPEGVER-arm64/.libs/libjpeg.a jpeg-$JPEGVER-x64/.libs/libjpeg.a
cp -af libjpegu.a ../../../lib/MacOSX/
cp -af jpeg-$JPEGVER-x64/jpeglib.h ../../libjpeg/
cp -af jpeg-$JPEGVER-x64/jmorecfg.h ../../libjpeg/
cp -af jpeg-$JPEGVER-x64/jinclude.h ../../libjpeg/
cp -af jpeg-$JPEGVER-x64/jconfig.h ../../libjpeg/MacOSX
