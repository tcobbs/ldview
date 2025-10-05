#!/bin/bash
JPEGVER=9f
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
