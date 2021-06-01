@echo off
rem libpng 1.6.37
if exist lpng1637.zip goto srcfound
curl -OJL https://download.sourceforge.net/libpng/lpng1637.zip

:srcfound
if exist lpng1637 goto dirfound
PowerShell Expand-Archive -Path lpng1637.zip -DestinationPath .

:dirfound
if exist zlib1211.zip goto zipfound
curl -OJL https://zlib.net/zlib1211.zip

:zipfound
if exist zlib-1.2.11 goto zipdirfound
PowerShell Expand-Archive -Path zlib1211.zip -DestinationPath .

:zipdirfound
cd lpng1637

:build
nmake -f scripts\makefile.vcwin32 clean
copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
nmake -f scripts\makefile.vcwin32 AR="link -lib" CFLAGS="-nologo -MT -O2 -W3 -I..\zlib-1.2.11" all
rem msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=win32 /p:PlatformToolset=v142
