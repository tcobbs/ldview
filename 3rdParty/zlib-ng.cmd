@echo off
set ZLIBVER=2.2.5
if "%1%"  == "uninstall" goto uninstall
if not exist c:\temp\zlib-ng-%ZLIBVER%.zip curl -OJL --output-dir c:\temp https://github.com/zlib-ng/zlib-ng/archive/refs/tags/%ZLIBVER%.zip
if not exist c:\temp\zlib-ng-%ZLIBVER% PowerShell Expand-Archive -Path c:\temp\zlib-ng-%ZLIBVER%.zip -DestinationPath c:\temp

for /d %%c in ("%VCToolsInstallDir%\..\14.16.*") do set CC1=%%~sc
for /d %%c in ("%VCToolsInstallDir%\..\14.29.*") do set CC2=%%~sc
set ORIG=%CD%
pushd %CD%
c:
cd \temp\zlib-ng-%ZLIBVER%
powershell "(Get-Content win32\Makefile.msc).Replace('-MD', '-MT') | Set-Content win32\Makefile.msc"

nmake /f win32\Makefile.msc ZLIB_COMPAT=1 "CC=%CC2%\bin\HostX64\x64\cl" clean zlib.lib

copy zlib.h %ORIG%\..\include
copy zconf.h %ORIG%\..\include
copy zlib_name_mangling.h %ORIG%\..\include
copy zlib.lib %ORIG%\..\lib\x64\zlib-vs2019.lib

nmake /f win32\Makefile.msc ZLIB_COMPAT=1 LOC=/MT "CC=%CC1%\bin\HostX86\x86\cl" clean zlib.lib

copy zlib.lib %ORIG%\..\lib\zlib-vs2017.lib
popd
rem del zlib-ng-win-x86-compat.zip zlib-ng-win-x86-64-compat.zip
rem rd /s /q zlib-ng-%ZLIBVER%
goto end

:uninstall
git checkout -- ..\include\zconf.h
git checkout -- ..\include\zlib.h
del ..\include\zlib_name_mangling.h
git checkout -- ..\lib\zlib-vs2017.lib
git checkout -- ..\lib\x64\zlib-vs2019.lib
del c:\temp\zlib-ng-%ZLIBVER%.zip
rd /s /q c:\temp\zlib-ng-%ZLIBVER%
:end

if exist zlib.h cd ..
