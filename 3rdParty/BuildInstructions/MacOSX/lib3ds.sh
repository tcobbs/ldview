#!/bin/bash
test -f lib3ds-20080909.zip || \
curl -OJL https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lib3ds/lib3ds-20080909.zip
test -d lib3ds-20080909 || unzip lib3ds-20080909.zip 
cd lib3ds-20080909
test -f config.log || ./configure --disable-shared
cd src
make clean all
cp .libs/lib3ds.a ../lib3ds-x64.a
make CFLAGS="-arch arm64" LDFLAGS="-arch arm64" clean all
cp .libs/lib3ds.a ../lib3ds-arm64.a
cd ..
strip lib3ds-arm64.a lib3ds-x64.a
cd ..
libtool -o lib3dsu.a lib3ds-20080909/lib3ds-arm64.a lib3ds-20080909/lib3ds-x64.a

