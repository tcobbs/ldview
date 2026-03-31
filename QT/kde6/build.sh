#!/bin/sh
rm -rf build
mkdir -p build
cd build
cmake -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DQT_DEFAULT_MAJOR_VERSION=6 ..
make
make install

