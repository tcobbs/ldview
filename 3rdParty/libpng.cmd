@echo off
rem libpng 1.6.50

if "%1%"  == "uninstall" goto uninstall

if exist lpng1650.zip goto srcfound
curl -OJL https://download.sourceforge.net/libpng/lpng1650.zip

:srcfound
if exist lpng1650 goto dirfound
PowerShell Expand-Archive -Path lpng1650.zip -DestinationPath .

:dirfound
if exist zlib131.zip goto zipfound
curl -OJL https://zlib.net/zlib131.zip

:zipfound
if exist zlib-1.3.1 goto zipdirfound
PowerShell Expand-Archive -Path zlib131.zip -DestinationPath .

:zipdirfound
cd lpng1650

:build
powershell "(Get-Content projects\vstudio\zlib.props).Replace('..\..\..\..\zlib<', '..\..\..\..\zlib-1.3.1<') | Set-Content projects\vstudio\zlib.props"
rem nmake -f scripts\makefile.vcwin32 clean
copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
rem nmake -f scripts\makefile.vcwin32 AR="link -lib" CFLAGS="-nologo -MT -O2 -W3 -I..\zlib-1.3.1" all
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=Win32 /p:PlatformToolset=v143 /t:Rebuild

powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('Win32', 'X64') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=X64 /p:PlatformToolset=v143 /t:Rebuild
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('X64', 'Win32') | Set-Content projects\vstudio\libpng\libpng.vcxproj"

if not "%1%"  == "install" goto end
copy png.h		..\libpng
copy pngconf.h		..\libpng
copy pngpriv.h		..\libpng
copy pnglibconf.h	..\libpng\Windows
copy "projects\vstudio\Release Library\libpng16.lib" ..\..\lib\libpng16-vs2022.lib
copy "projects\vstudio\X64\Release Library\libpng16.lib" ..\..\lib\X64\libpng16-vs2022.lib
goto end

:uninstall
git checkout -- libpng\png.h
git checkout -- libpng\pngconf.h
git checkout -- libpng\pngpriv.h
git checkout -- libpng\Windows\pnglibconf.h
git checkout -- ..\lib\libpng16-vs2017.lib
git checkout -- ..\lib\x64\libpng16-vs2019.lib

:end
if exist png.h cd ..
