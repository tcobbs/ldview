#!/bin/bash
PNGVER=1.6.53

if [ "$1" = -uninstall ] ; then
	git checkout -- ../../../lib/MacOSX/libpng16u.a
	git checkout -- ../../libpng/png.h
	git checkout -- ../../libpng/pngconf.h
	git checkout -- ../../libpng/pngpriv.h
	git checkout -- ../../libpng/MacOSX/pnglibconf.h
	exit 0
fi

test -f libpng-$PNGVER.tar.gz || curl -OJL https://download.sourceforge.net/libpng/libpng-$PNGVER.tar.gz

test -d libpng-$PNGVER || tar zxf libpng-$PNGVER.tar.gz
cd libpng-$PNGVER
test -f config.log || ./configure CFLAGS="-arch x86_64 -arch arm64 -mmacosx-version-min=10.13" --disable-arm-neon --enable-shared=no LDFLAGS="-arch x86_64 -arch arm64"
make
cp -af .libs/libpng16.a ../../../../lib/MacOSX/libpng16u.a
cp -af png.h \
       pngconf.h \
       pngpriv.h ../../../libpng/
cp -af pnglibconf.h ../../../libpng/MacOSX
