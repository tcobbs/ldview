@echo off
if exist zlib1213.zip goto zipfound
curl -OJL https://zlib.net/zlib1213.zip

:zipfound
if exist zlib-1.2.13 goto zipdirfound
PowerShell Expand-Archive -Path zlib1213.zip -DestinationPath .

:zipdirfound
cd zlib-1.2.13
nmake -f win32/Makefile.msc AS=ml64 CFLAGS="-MT -O2 -nologo -I. -DASMV -DASMINF"  clean zlib.lib