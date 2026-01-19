@echo off
set JPEGVER=9f

if "%1%"  == "uninstall" goto uninstall

if not exist c:\temp\jpegsr%JPEGVER%.zip curl -OJL --output-dir c:\temp https://www.ijg.org/files/jpegsr%JPEGVER%.zip
if not exist c:\temp\jpeg-%JPEGVER% PowerShell Expand-Archive -Path c:\temp\jpegsr%JPEGVER%.zip -DestinationPath c:\temp
pushd %CD%
c:
cd \temp\jpeg-%JPEGVER%
nmake /nologo -f Makefile.vs setup-v16
powershell "(Get-Content jpeg.vcxproj).Replace('<Optimization>Full</Optimization>', """"<Optimization>Full</Optimization>`n<RuntimeLibrary>MultiThreaded</RuntimeLibrary>"""") | Set-Content jpeg.vcxproj"
powershell "(Get-Content jpeg.vcxproj).Replace('<WholeProgramOptimization>true</WholeProgramOptimization>', '<WholeProgramOptimization>false</WholeProgramOptimization>') | Set-Content jpeg.vcxproj"

msbuild jpeg.vcxproj /p:Platform=win32 /p:Configuration=Release /p:PlatformToolset=v141_xp
msbuild jpeg.vcxproj /p:Platform=x64   /p:Configuration=Release /p:PlatformToolset=v142
popd

if not "%1%"  == "install" goto end
copy c:\temp\jpeg-%JPEGVER%\jconfig.h   libjpeg\Windows
copy c:\temp\jpeg-%JPEGVER%\jconfig.mac libjpeg\MacOSX\jconfig.h
copy c:\temp\jpeg-%JPEGVER%\jinclude.h  libjpeg\
copy c:\temp\jpeg-%JPEGVER%\jmorecfg.h  libjpeg\
copy c:\temp\jpeg-%JPEGVER%\jpeglib.h   libjpeg\
copy c:\temp\jpeg-%JPEGVER%\release\win32\jpeg.lib ..\lib\libjpeg-vs2017.lib
copy c:\temp\jpeg-%JPEGVER%\release\x64\jpeg.lib   ..\lib\x64\libjpeg-vs2019.lib
goto end

:uninstall
git checkout -- ..\lib\libjpeg-vs2017.lib
git checkout -- ..\lib\x64\libjpeg-vs2019.lib
git checkout -- libjpeg\Windows\jconfig.h
git checkout -- libjpeg\MacOSX\jconfig.h
git checkout -- libjpeg\jinclude.h
git checkout -- libjpeg\jmorecfg.h
git checkout -- libjpeg\jpeglib.h
del c:\temp\jpegsr%JPEGVER%.zip
rd /s /q c:\temp\jpeg-%JPEGVER%
:end
if exist jconfig.h cd ..
