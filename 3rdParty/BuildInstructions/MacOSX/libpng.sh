#!/bin/bash
PNGVER=1.6.50

if [ "$1" = -uninstall ] ; then
	git checkout -- ../../../lib/MacOSX/libpng16u.a
	git checkout -- ../../libpng/png.h
	git checkout -- ../../libpng/pngconf.h
	git checkout -- ../../libpng/pngpriv.h
	git checkout -- ../../libpng/MacOSX/pnglibconf.h
	exit 0
fi

test -f libpng-$PNGVER.tar.gz || curl -OJL https://download.sourceforge.net/libpng/libpng-$PNGVER.tar.gz

test -d libpng-$PNGVER-x64 || (tar zxf libpng-$PNGVER.tar.gz && mv libpng-$PNGVER libpng-$PNGVER-x64 )
test -d libpng-$PNGVER-arm64 || (tar zxf libpng-$PNGVER.tar.gz && mv libpng-$PNGVER libpng-$PNGVER-arm64 )
cd libpng-$PNGVER-x64
test -f config.log || ./configure --target x86_64-apple-darwin
make CFLAGS="-arch x86_64" LDFLAGS="-arch x86_64"
cd ..
cd libpng-$PNGVER-arm64
test -f config.log || ./configure --target arm64-apple-darwin
make CFLAGS="-arch arm64 -DPNG_ARM_NEON_OPT=0" LDFLAGS="-arch arm64"
cd ..
strip libpng-$PNGVER-*/.libs/libpng16.a
libtool -o libpng16u.a libpng-$PNGVER-arm64/.libs/libpng16.a libpng-$PNGVER-x64/.libs/libpng16.a
cp -af libpng16u.a ../../../lib/MacOSX
cp -af libpng-$PNGVER-x64/png.h \
       libpng-$PNGVER-x64/pngconf.h \
       libpng-$PNGVER-x64/pngpriv.h ../../libpng/
cp -af libpng-$PNGVER-x64/pnglibconf.h ../../libpng/MacOSX
