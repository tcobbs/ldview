@echo off
set VS2019=/p:PlatformToolset=v142 /p:WindowsTargetPlatformVersion=10.0
set VS2015=/p:PlatformToolset=v140 /p:WindowsTargetPlatformVersion=8.1
set VS=%VS2015%
set CONFIG=/p:Configuration=Release
msbuild %CONFIG% %VS% LDView.sln
msbuild %CONFIG% %VS% LDView.sln /t:Launcher /p:Platform=win32
msbuild %CONFIG% %VS% Translations\Hungarian\Hungarian.vcxproj
msbuild %CONFIG% %VS% Translations\German\German.vcxproj
msbuild %CONFIG% %VS% Translations\Italian\Italian.vcxproj
msbuild %CONFIG% %VS% Translations\Czech\Czech.vcxproj
msbuild %CONFIG% %VS% LDViewThumbs\LDViewThumbs.vcxproj /p:SolutionDir=%CD%\ /p:Platform=win32
msbuild %CONFIG% %VS% LDViewThumbs\LDViewThumbs.vcxproj /p:SolutionDir=%CD%\

if exist "c:\Program Files (x86)\Inno Setup 6\iscc.exe" "c:\Program Files (x86)\Inno Setup 6\iscc.exe" LDView64.iss
