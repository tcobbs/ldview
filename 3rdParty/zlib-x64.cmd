@echo off
if "%1%"  == "uninstall" goto uninstall
if exist zlib131.zip goto zipfound
curl -OJL https://zlib.net/zlib131.zip

:zipfound
if exist zlib-1.3.1 goto zipdirfound
PowerShell Expand-Archive -Path zlib131.zip -DestinationPath .

:zipdirfound
cd zlib-1.3.1
powershell "(Get-Content contrib\vstudio\vc17\zlibstat.vcxproj).Replace('MultiThreadedDLL', 'MultiThreaded') | Set-Content contrib\vstudio\vc17\zlibstat.vcxproj"
rem nmake -f win32/Makefile.msc AS=ml64 CFLAGS="-MT -O2 -nologo -I. -DASMV -DASMINF"  clean zlib.lib
msbuild contrib\vstudio\vc17\zlibstat.vcxproj /p:Platform=win32 /p:Configuration=Release /p:PlatformToolset=v142
msbuild contrib\vstudio\vc17\zlibstat.vcxproj /p:Platform=x64   /p:Configuration=Release /p:PlatformToolset=v142

if not "%1%"  == "install" goto end
copy contrib\vstudio\vc17\x64\ZlibStatRelease\zlibstat.lib ..\..\lib\x64\zlib-vs2019.lib
copy contrib\vstudio\vc17\x86\ZlibStatRelease\zlibstat.lib ..\..\lib\zlib-vs2019.lib
copy zconf.h ..\..\include\
copy zlib.h ..\..\include\
copy contrib\minizip\unzip.h ..\..\include
rem copy zlib.lib ..\..\lib\x64\zlib-vs2019.lib
goto end

:uninstall
git checkout -- ..\include\zconf.h
git checkout -- ..\include\zlib.h
git checkout -- ..\include\unzip.h
git checkout -- ..\lib\zlib-vs2017.lib
git checkout -- ..\lib\x64\zlib-vs2019.lib
:end
if exist zconf.h cd ..
