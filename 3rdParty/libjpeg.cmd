@echo off
if exist jpegsr9f.zip goto srcfound
curl -OJL https://www.ijg.org/files/jpegsr9f.zip
:srcfound
if exist jpeg-9f goto dirfound
PowerShell Expand-Archive -Path jpegsr9f.zip  -DestinationPath .
:dirfound
cd jpeg-9f
nmake -f Makefile.vs setup-v16
powershell "(Get-Content jpeg.vcxproj).Replace('<Optimization>Full</Optimization>', """"<Optimization>Full</Optimization>`n<RuntimeLibrary>MultiThreaded</RuntimeLibrary>"""") | Set-Content jpeg.vcxproj"
 
msbuild jpeg.vcxproj /p:Platform=win32 /p:Configuration=Release /p:PlatformToolset=v143
msbuild jpeg.vcxproj /p:Platform=x64   /p:Configuration=Release /p:PlatformToolset=v143

copy jconfig.h   ..\libjpeg\Windows
copy jconfig.mac ..\libjpeg\MacOSX\jconfig.h
copy jinclude.h  ..\libjpeg\
copy jmorecfg.h  ..\libjpeg\
copy jpeglib.h   ..\libjpeg\
copy release\win32\jpeg.lib ..\..\lib\libjpeg-vs2022.lib
copy release\x64\jpeg.lib   ..\..\lib\x64\libjpeg-vs2022.lib
cd ..
