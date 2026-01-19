@echo off
rem libpng 1.6.54
set PNGVER=1654
if "%1%"  == "uninstall" goto uninstall

if not exist c:\temp\lpng%PNGVER%.zip curl -OJL --output-dir c:\temp https://download.sourceforge.net/libpng/lpng%PNGVER%.zip

if not exist c:\temp\lpng%PNGVER% PowerShell Expand-Archive -Path c:\temp\lpng%PNGVER%.zip -DestinationPath c:\temp

if not exist c:\temp\zlib131.zip curl -OJL --output-dir c:\temp https://zlib.net/zlib131.zip

if not exist c:\temp\zlib-1.3.1 PowerShell Expand-Archive -Path c:\temp\zlib131.zip -DestinationPath c:\temp

:zipdirfound
pushd %CD%
c:
cd \temp\lpng%PNGVER%

:build
powershell "(Get-Content projects\vstudio\zlib.props).Replace('..\..\..\..\zlib<', '..\..\..\..\zlib-1.3.1<') | Set-Content projects\vstudio\zlib.props"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>', '<DebugInformationFormat>None</DebugInformationFormat>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<BrowseInformation>true</BrowseInformation>', '<BrowseInformation>false</BrowseInformation>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<GenerateDebugInformation>true</GenerateDebugInformation>', '<GenerateDebugInformation>false</GenerateDebugInformation>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"

copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=Win32 /p:PlatformToolset=v141_xp /t:Rebuild

powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('Win32', 'X64') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=X64 /p:PlatformToolset=v142 /t:Rebuild
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('X64', 'Win32') | Set-Content projects\vstudio\libpng\libpng.vcxproj"

popd
if not "%1%"  == "install" goto end
copy \temp\lpng%PNGVER%\png.h		libpng
copy \temp\lpng%PNGVER%\pngconf.h		libpng
copy \temp\lpng%PNGVER%\pngpriv.h		libpng
copy \temp\lpng%PNGVER%\pnglibconf.h	libpng\Windows
copy "\temp\lpng%PNGVER%\projects\vstudio\Release Library\libpng16.lib" ..\lib\libpng16-vs2017.lib
copy "\temp\lpng%PNGVER%\projects\vstudio\X64\Release Library\libpng16.lib" ..\lib\X64\libpng16-vs2019.lib
goto end

:uninstall
git checkout -- libpng\png.h
git checkout -- libpng\pngconf.h
git checkout -- libpng\pngpriv.h
git checkout -- libpng\Windows\pnglibconf.h
git checkout -- ..\lib\libpng16-vs2017.lib
git checkout -- ..\lib\x64\libpng16-vs2019.lib
rd /s /q c:\temp\lpng%PNGVER%
rd /s /q c:\temp\zlib-1.3.1
del c:\temp\lpng*.zip
del c:\temp\zlib*.zip

:end
if exist png.h cd ..
