#!/bin/sh
mkdir -p AppImage
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/3/linuxdeployqt-3-x86_64.AppImage
chmod a+x linuxdeployqt*.AppImage
make INSTALL_ROOT=AppImage install
./linuxdeployqt*.AppImage AppImage/usr/bin/LDView -bundle-non-qt-libs
