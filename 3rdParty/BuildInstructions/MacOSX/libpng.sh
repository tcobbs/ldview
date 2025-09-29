#!/bin/bash
PNGVER=1.6.50
test -f libpng-$PNGVER.tar.gz || curl -OJL https://download.sourceforge.net/libpng/libpng-$PNGVER.tar.gz

test -d libpng-$JPEGVER-x64 || (tar zxf libpng-$PNGVER.tar.gz && mv libpng-$PNGVER libpng-$PNGVER-x64 )
test -d libpng-$JPEGVER-arm64 || (tar zxf libpng-$PNGVER.tar.gz && mv libpng-$PNGVER libpng-$PNGVER-arm64 )
cd libpng-$PNGVER-x64
test -f config.log || ./configure --target x86_64-apple-darwin
make
cd ..
cd libpng-$PNGVER-arm64
test -f config.log || ./configure --target arm64-apple-darwin
make CFLAGS="-arch arm64 -DPNG_ARM_NEON_OPT=0" LDFLAGS="-arch arm64"
cd ..
strip libpng-$PNGVER-*/.libs/libpng16.a
libtool -o libpng16u.a libpng-$PNGVER-arm64/.libs/libpng16.a libpng-$PNGVER-x64/.libs/libpng16.a
