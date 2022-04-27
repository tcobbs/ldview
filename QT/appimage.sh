#!/bin/sh
test -d AppImage && rm -rf AppImage
mkdir -p AppImage
if [ ! -x linuxdeployqt-continuous-x86_64.AppImage ] ; then
wget -nv https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt*.AppImage
fi
make INSTALL_ROOT=AppImage install
cp -f desktop/ldview.desktop AppImage/
cp -f images/LDViewIcon.png AppImage/gnome-ldraw.png
./linuxdeployqt*.AppImage AppImage/usr/bin/LDView -bundle-non-qt-libs
./linuxdeployqt*.AppImage AppImage/usr/bin/LDView -appimage
test -d AppImage && rm -rf AppImage
