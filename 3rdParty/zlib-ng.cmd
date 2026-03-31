@echo off
set ZLIBVER=2.3.2
if "%1%"  == "uninstall" goto uninstall
if not exist c:\temp\zlib-ng-win-x86-64-compat.zip curl -OJL --output-dir c:\temp https://github.com/zlib-ng/zlib-ng/releases/download/2.3.2/zlib-ng-win-x86-64-compat.zip
if not exist c:\temp\zlib-ng-win-x86-compat.zip curl -OJL --output-dir c:\temp https://github.com/zlib-ng/zlib-ng/releases/download/2.3.2/zlib-ng-win-x86-compat.zip
if not exist c:\temp\zlib-ng-win-x86-64-compat PowerShell Expand-Archive -Path c:\temp\zlib-ng-win-x86-64-compat.zip -DestinationPath c:\temp\zlib-ng-win-x86-64-compat
if not exist c:\temp\zlib-ng-win-x86-compat PowerShell Expand-Archive -Path c:\temp\zlib-ng-win-x86-compat.zip -DestinationPath c:\temp\zlib-ng-win-x86-compat

copy c:\temp\zlib-ng-win-x86-compat\include\zlib.h ..\include
copy c:\temp\zlib-ng-win-x86-compat\include\zconf.h ..\include
copy c:\temp\zlib-ng-win-x86-compat\include\zlib_name_mangling.h ..\include
copy c:\temp\zlib-ng-win-x86-64-compat\lib\zlibstatic.lib ..\lib\x64\zlib-vs2019.lib
copy c:\temp\zlib-ng-win-x86-compat\lib\zlibstatic.lib ..\lib\zlib-vs2017.lib
goto end
:uninstall
git checkout -- ..\include\zconf.h
git checkout -- ..\include\zlib.h
del ..\include\zlib_name_mangling.h
git checkout -- ..\lib\zlib-vs2017.lib
git checkout -- ..\lib\x64\zlib-vs2019.lib
del c:\temp\zlib-ng-win-x86*-compat.zip
rd /s /q c:\temp\zlib-ng-win-x86-64-compat
rd /s /q c:\temp\zlib-ng-win-x86-compat
:end

