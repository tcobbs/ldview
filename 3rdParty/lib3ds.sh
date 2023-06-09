#!/bin/sh
test -f lib3ds-20080909.zip || \
wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lib3ds/lib3ds-20080909.zip
test -d lib3ds-20080909 || unzip lib3ds-20080909.zip
cd lib3ds-20080909
./configure --disable-shared
cd src
make
