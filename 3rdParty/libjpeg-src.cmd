@echo off
set JPEGVER=10

if "%1%"  == "uninstall" goto uninstall

if not exist c:\temp\jpegsr%JPEGVER%.zip curl -OJL --output-dir c:\temp https://www.ijg.org/files/jpegsr%JPEGVER%.zip
if not exist c:\temp\jpeg-%JPEGVER% PowerShell Expand-Archive -Path c:\temp\jpegsr%JPEGVER%.zip -DestinationPath c:\temp
pushd %CD%
c:
cd \temp\jpeg-%JPEGVER%
if not exist jpeg.vcxproj nmake /nologo -f Makefile.vs setup-v16
popd

copy /y c:\temp\jpeg-%JPEGVER%\j*.c 		libjpeg >nul
copy /y c:\temp\jpeg-%JPEGVER%\j*.h 		libjpeg >nul
copy /y c:\temp\jpeg-%JPEGVER%\jpeg.vcxproj 	libjpeg >nul
copy /y c:\temp\jpeg-%JPEGVER%\jconfig.xc 	libjpeg\MacOSX\jconfig.h >nul
copy /y c:\temp\jpeg-%JPEGVER%\README 		libjpeg >nul
copy /y c:\temp\jpeg-%JPEGVER%\libjpeg.txt 	libjpeg >nul
copy /y c:\temp\jpeg-%JPEGVER%\change.log 	libjpeg >nul

copy libjpeg\jconfig.h libjpeg\Windows\jconfig.h >nul
del libjpeg\jconfig.h 2>nul

del libjpeg\jpegtran.c 2>nul
del libjpeg\jmemdos.c 2>nul
del libjpeg\jmemansi.c 2>nul
del libjpeg\jmemmac.c 2>nul
del libjpeg\jmemname.c 2>nul
cd ..
"c:\Program Files\Git\usr\bin\patch.exe" -N -p1 -i 3rdParty\libjpeg.patch
for /r %%f in (*.rej) do @del %%f
for /r %%f in (*.orig) do @del %%f
cd 3rdparty\libjpeg
rem powershell "(Get-Content jpeg.vcxproj).Replace('<Optimization>Full</Optimization>', """"<Optimization>Full</Optimization>`n<RuntimeLibrary>MultiThreaded</RuntimeLibrary>"""") | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj).Replace('<OutDir>$(ProjectDir)$(Configuration)\$(Platform)\</OutDir>', '<OutDir>$(SolutionDir)Build\$(Configuration)\</OutDir>') | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj).Replace('<IntDir>$(ProjectDir)$(Configuration)\$(Platform)\$(ProjectName)\</IntDir>', '<IntDir>$(SolutionDir)Build\$(ProjectName)\$(Configuration)-inter\</IntDir>') | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj -Raw) -replace '(?s)(PropertyGroup Condition[^|]*\|[^|]+\|x64..>.*?)\(Configuration\)-inter', '$1(Configuration)64-inter' | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj -Raw) -replace '(?s)(PropertyGroup Condition[^|]*\|[^|]+\|x64..>.*?)\(Configuration\)\\', '$1(Configuration)64\\' | Set-Content jpeg.vcxproj"
cd ..
goto end
:uninstall
rem del c:\temp\jpegsr%JPEGVER%.zip
rem rd /s /q c:\temp\jpeg-%JPEGVER%
:end
