@echo off
if "%1%"  == "uninstall" goto uninstall
if not exist c:\temp\zlib131.zip curl -OJL --output-dir c:\temp https://zlib.net/zlib131.zip
if not exist c:\temp\zlib-1.3.1 PowerShell Expand-Archive -Path c:\temp\zlib131.zip -DestinationPath c:\temp
pushd %CD%
c:
cd \temp\zlib-1.3.1
powershell "(Get-Content contrib\vstudio\vc17\zlibstat.vcxproj).Replace('MultiThreadedDLL', 'MultiThreaded') | Set-Content contrib\vstudio\vc17\zlibstat.vcxproj"
powershell "(Get-Content contrib\vstudio\vc17\zlibstat.vcxproj).Replace('ZLIB_WINAPI;', '') | Set-Content contrib\vstudio\vc17\zlibstat.vcxproj"
msbuild contrib\vstudio\vc17\zlibstat.vcxproj /p:Platform=Win32 /p:Configuration=Release /p:PlatformToolset=v141_xp
msbuild contrib\vstudio\vc17\zlibstat.vcxproj /p:Platform=x64   /p:Configuration=Release /p:PlatformToolset=v142
popd

if not "%1%"  == "install" goto end
copy \temp\zlib-1.3.1\contrib\vstudio\vc17\x64\ZlibStatRelease\zlibstat.lib ..\lib\x64\zlib-vs2019.lib
copy \temp\zlib-1.3.1\contrib\vstudio\vc17\x86\ZlibStatRelease\zlibstat.lib ..\lib\zlib-vs2017.lib
copy \temp\zlib-1.3.1\zconf.h ..\include\
copy \temp\zlib-1.3.1\zlib.h ..\include\
goto end

:uninstall
git checkout -- ..\include\zconf.h
git checkout -- ..\include\zlib.h
git checkout -- ..\lib\zlib-vs2017.lib
git checkout -- ..\lib\x64\zlib-vs2019.lib
del c:\temp\zlib*.zip
rd /s /q c:\temp\zlib-1.3.1
:end
if exist zconf.h cd ..
