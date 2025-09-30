@echo off
set JPEGVER=9f

if "%1%"  == "uninstall" goto uninstall

if exist jpegsr%JPEGVER%.zip goto srcfound
curl -OJL https://www.ijg.org/files/jpegsr%JPEGVER%.zip
:srcfound
if exist jpeg-%JPEGVER% goto dirfound
PowerShell Expand-Archive -Path jpegsr%JPEGVER%.zip  -DestinationPath .
cd jpeg-%JPEGVER%
nmake /nologo -f Makefile.vs setup-v16
powershell "(Get-Content jpeg.vcxproj).Replace('<Optimization>Full</Optimization>', """"<Optimization>Full</Optimization>`n<RuntimeLibrary>MultiThreaded</RuntimeLibrary>"""") | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content jpeg.vcxproj"
cd ..

:dirfound
cd jpeg-%JPEGVER%
msbuild jpeg.vcxproj /p:Platform=win32 /p:Configuration=Release /p:PlatformToolset=v141_xp
msbuild jpeg.vcxproj /p:Platform=x64   /p:Configuration=Release /p:PlatformToolset=v142

if not "%1%"  == "install" goto end
copy jconfig.h   ..\libjpeg\Windows
copy jconfig.mac ..\libjpeg\MacOSX\jconfig.h
copy jinclude.h  ..\libjpeg\
copy jmorecfg.h  ..\libjpeg\
copy jpeglib.h   ..\libjpeg\
copy release\win32\jpeg.lib ..\..\lib\libjpeg-vs2017.lib
copy release\x64\jpeg.lib   ..\..\lib\x64\libjpeg-vs2019.lib
goto end

:uninstall
git checkout -- ..\lib\libjpeg-vs2017.lib
git checkout -- ..\lib\x64\libjpeg-vs2019.lib
git checkout -- libjpeg\Windows\jconfig.h
git checkout -- libjpeg\MacOSX\jconfig.h
git checkout -- libjpeg\jinclude.h
git checkout -- libjpeg\jmorecfg.h
git checkout -- libjpeg\jpeglib.h
:end
if exist jconfig.h cd ..
