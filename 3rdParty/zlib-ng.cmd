@echo off
set ZLIBVER=2.2.5
if "%1%"  == "uninstall" goto uninstall
if not exist zlib-ng-%ZLIBVER%.zip curl -OJL https://github.com/zlib-ng/zlib-ng/archive/refs/tags/2.2.5.zip
if not exist zlib-ng-%ZLIBVER% PowerShell Expand-Archive -Path zlib-ng-%ZLIBVER%.zip -DestinationPath .

for /d %%c in ("%VCToolsInstallDir%\..\14.16.*") do set CC1=%%~sc
for /d %%c in ("%VCToolsInstallDir%\..\14.29.*") do set CC2=%%~sc


cd zlib-ng-%ZLIBVER%
nmake /f win32\Makefile.msc ZLIB_COMPAT=1 LOC=/MT "CC=%CC2%\bin\HostX64\x64\cl" clean zlib.lib

copy zlib.h ..\..\include
copy zconf.h ..\..\include
copy zlib_name_mangling.h ..\..\include
copy zlib.lib ..\..\lib\x64\zlib-vs2019.lib

nmake /f win32\Makefile.msc ZLIB_COMPAT=1 LOC=/MT "CC=%CC1%\bin\HostX86\x86\cl" clean zlib.lib

copy zlib.lib ..\..\lib\zlib-vs2017.lib

rem del zlib-ng-win-x86-compat.zip zlib-ng-win-x86-64-compat.zip
rem rd /s /q zlib-ng-%ZLIBVER%
goto end

:uninstall
git checkout -- ..\include\zconf.h
git checkout -- ..\include\zlib.h
del ..\include\zlib_name_mangling.h
git checkout -- ..\lib\zlib-vs2017.lib
git checkout -- ..\lib\x64\zlib-vs2019.lib
:end

if exist zlib.h cd ..