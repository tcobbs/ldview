#!/bin/sh
rm -rf build
mkdir -p build
cd build
cmake -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DCMAKE_INSTALL_PREFIX=`kf5-config --prefix` ..
cd ..
make
make install

