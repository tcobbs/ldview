@echo off
rem libpng 1.6.39
if exist lpng1639.zip goto srcfound
curl -OJL https://download.sourceforge.net/libpng/lpng1639.zip

:srcfound
if exist lpng1639 goto dirfound
PowerShell Expand-Archive -Path lpng1639.zip -DestinationPath .

:dirfound
if exist zlib1213.zip goto zipfound
curl -OJL https://zlib.net/zlib1213.zip

:zipfound
if exist zlib-1.2.13 goto zipdirfound
PowerShell Expand-Archive -Path zlib1213.zip -DestinationPath .

:zipdirfound
cd lpng1639

:build
nmake -f scripts\makefile.vcwin32 clean
copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
nmake -f scripts\makefile.vcwin32 AR="link -lib" CFLAGS="-nologo -MT -O2 -W3 -I..\zlib-1.2.13" all
rem msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=win32 /p:PlatformToolset=v142
