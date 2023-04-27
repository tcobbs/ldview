@echo off
if exist jpegsr9d.zip goto srcfound
curl -OJL https://www.ijg.org/files/jpegsr9d.zip
:srcfound
if exist jpeg-9d goto dirfound
PowerShell Expand-Archive -Path jpegsr9d.zip  -DestinationPath .
:dirfound
cd jpeg-9d
nmake -f Makefile.vs setup-v16
msbuild jpeg.vcxproj /p:Platform=win32 /p:Configuration=Release /p:PlatformToolset=v143
msbuild jpeg.vcxproj /p:Platform=x64 /p:Configuration=Release /p:PlatformToolset=v143
