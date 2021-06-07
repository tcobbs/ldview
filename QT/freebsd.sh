#!/bin/sh
cd freebsd
test -f distfile && rm -f distfile
make fetch
make makesum
make
make package
