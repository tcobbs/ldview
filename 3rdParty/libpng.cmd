@echo off
rem libpng 1.6.53

if "%1%"  == "uninstall" goto uninstall

if not exist lpng1653.zip curl -OJL https://download.sourceforge.net/libpng/lpng1653.zip

if not exist lpng1653 PowerShell Expand-Archive -Path lpng1653.zip -DestinationPath .

if not exist zlib131.zip curl -OJL https://zlib.net/zlib131.zip

if not exist zlib-1.3.1 PowerShell Expand-Archive -Path zlib131.zip -DestinationPath .

:zipdirfound
cd lpng1653

:build
powershell "(Get-Content projects\vstudio\zlib.props).Replace('..\..\..\..\zlib<', '..\..\..\..\zlib-1.3.1<') | Set-Content projects\vstudio\zlib.props"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>', '<DebugInformationFormat>None</DebugInformationFormat>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<BrowseInformation>true</BrowseInformation>', '<BrowseInformation>false</BrowseInformation>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<GenerateDebugInformation>true</GenerateDebugInformation>', '<GenerateDebugInformation>false</GenerateDebugInformation>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content projects\vstudio\libpng\libpng.vcxproj"

rem nmake -f scripts\makefile.vcwin32 clean
copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
rem nmake -f scripts\makefile.vcwin32 AR="link -lib" CFLAGS="-nologo -MT -O2 -W3 -I..\zlib-1.3.1" all
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=Win32 /p:PlatformToolset=v141_xp /t:Rebuild

powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('Win32', 'X64') | Set-Content projects\vstudio\libpng\libpng.vcxproj"
msbuild projects\vstudio\libpng\libpng.vcxproj "/p:Configuration=Release Library" /p:SolutionDir=../ /p:Platform=X64 /p:PlatformToolset=v142 /t:Rebuild
powershell "(Get-Content projects\vstudio\libpng\libpng.vcxproj).Replace('X64', 'Win32') | Set-Content projects\vstudio\libpng\libpng.vcxproj"

if not "%1%"  == "install" goto end
copy png.h		..\libpng
copy pngconf.h		..\libpng
copy pngpriv.h		..\libpng
copy pnglibconf.h	..\libpng\Windows
copy "projects\vstudio\Release Library\libpng16.lib" ..\..\lib\libpng16-vs2017.lib
copy "projects\vstudio\X64\Release Library\libpng16.lib" ..\..\lib\X64\libpng16-vs2019.lib
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
