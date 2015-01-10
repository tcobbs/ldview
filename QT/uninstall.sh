#!/bin/sh
rm -rf /usr/bin/LDView /usr/bin/ldview /usr/share/ldview 
find /usr -type f -name '*ldraw*' -exec rm -f {} \;
find /usr -type f -name '*ldview*' -exec rm -f {} \;
