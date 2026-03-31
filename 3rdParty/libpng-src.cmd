@echo off
if "%1%"  == "uninstall" goto uninstall
if not exist c:\temp\lpng1655.zip curl -OJL --output-dir c:\temp https://download.sourceforge.net/libpng/lpng1655.zip
if not exist c:\temp\lpng1655 PowerShell Expand-Archive -Path c:\temp\lpng1655.zip -DestinationPath c:\temp

copy /y \temp\lpng1655\*.c       libpng >nul
copy /y \temp\lpng1655\*.h       libpng >nul
del libpng\pngtest.c
del libpng\example.c
copy /y \temp\lpng1655\AUTHORS   libpng >nul
copy /y \temp\lpng1655\CHANGES   libpng >nul
copy /y \temp\lpng1655\LICENSE   libpng >nul
copy /y \temp\lpng1655\README    libpng >nul
copy /y \temp\lpng1655\projects\vstudio\zlib.props    libpng >nul
copy /y \temp\lpng1655\projects\vstudio\libpng\libpng.vcxproj    libpng >nul
copy /y \temp\lpng1655\scripts\pnglibconf.h.prebuilt libpng\pnglibconf.h >nul
copy /y \temp\lpng1655\scripts\pngwin.rc libpng\ >nul
copy /y \temp\lpng1655\arm\*.* libpng\arm >nul

cd ..
"c:\Program Files\Git\usr\bin\patch.exe" -N -p1 -i 3rdParty\libpng.patch
for /r %%f in (*.rej) do @del %%f
for /r %%f in (*.orig) do @del %%f
cd 3rdparty\libpng

powershell "(Get-Content pngwin.rc).Replace('../', '') | Set-Content pngwin.rc"

powershell "(Get-Content zlib.props).Replace('..\..\..\..\zlib<', '..\zlib<') | Set-Content zlib.props"
powershell "(Get-Content libpng.vcxproj).Replace('<DebugInformationFormat>ProgramDatabase</DebugInformationFormat>', '<DebugInformationFormat>None</DebugInformationFormat>') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('<BrowseInformation>true</BrowseInformation>', '<BrowseInformation>false</BrowseInformation>') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('<GenerateDebugInformation>true</GenerateDebugInformation>', '<GenerateDebugInformation>false</GenerateDebugInformation>') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('..\..\..\', '') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('scripts\', '') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('$(SolutionDir)\zlib.props', 'zlib.props') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('ARM64', 'X64') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('<PrecompiledHeader>Use</PrecompiledHeader>', '<PrecompiledHeader>NotUsing</PrecompiledHeader>') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('DynamicLibrary', 'StaticLibrary') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('MultiThreadedDebugDLL', 'MultiThreadedDebug') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('Release|', 'Release_old|') | Set-Content libpng.vcxproj"
powershell "(Get-Content libpng.vcxproj).Replace('Release Library|', 'Release|') | Set-Content libpng.vcxproj"
cd ..
goto end

:uninstall
cd  ..
git checkout -- *
rem del c:\temp\lpng*.zip
rem rd /s /q c:\temp\lpng1655
git status
cd 3rdparty
:end
