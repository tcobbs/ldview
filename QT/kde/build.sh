#!/bin/sh
rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
make
make install

