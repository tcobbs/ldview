@echo off
msbuild /p:Configuration=Release LDView.vcxproj
cd Translations\Hungarian
msbuild /p:Configuration=Release Hungarian.vcxproj
cd ..\..

cd Translations\German
msbuild /p:Configuration=Release German.vcxproj
cd ..\..

cd Translations\Italian
msbuild /p:Configuration=Release Italian.vcxproj
cd ..\..

cd Translations\Czech
msbuild /p:Configuration=Release Czech.vcxproj
cd ..\..