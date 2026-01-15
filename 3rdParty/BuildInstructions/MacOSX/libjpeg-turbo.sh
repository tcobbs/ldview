#!/bin/bash
JPEGVER=3.1.2

if [ "$1" = -uninstall ] ; then
	git checkout -- ../../libjpeg/jpeglib.h
	git checkout -- ../../libjpeg/jmorecfg.h
	git checkout -- ../../libjpeg/MacOSX/jconfig.h
	git checkout -- ../../../lib/MacOSX/libjpegu.a
	exit 0
fi
test -f libjpeg-turbo-$JPEGVER.dmg || \
curl -OJL https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/$JPEGVER/libjpeg-turbo-$JPEGVER.dmg
hdiutil attach -quiet libjpeg-turbo-$JPEGVER.dmg
pkgutil --expand /Volumes/libjpeg-turbo-$JPEGVER/libjpeg-turbo.pkg /tmp/libjpeg-turbo
tar xf /tmp/libjpeg-turbo/libjpeg-turbo.pkg/Payload -C /tmp '*.h' '*.a'
cp -af /tmp/opt/libjpeg-turbo/lib/libjpeg.a ../../../lib/MacOSX/libjpegu.a
cp -af /tmp/opt/libjpeg-turbo/include/jpeglib.h ../../libjpeg/
cp -af /tmp/opt/libjpeg-turbo/include/jmorecfg.h ../../libjpeg/
cp -af /tmp/opt/libjpeg-turbo/include/jconfig.h ../../libjpeg/MacOSX/
hdiutil detach -quiet /Volumes/libjpeg-turbo-$JPEGVER
rm -rf /tmp/libjpeg-turbo /tmp/opt/libjpeg-turbo
