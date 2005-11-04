@echo off
set CFLAGS=/nologo /ML /W4 /GX /O2 /D NDEBUG /D WIN32 /FoRelease\ /FdRelease\ /FD

rem Remove the rem the following 2 lines to force usage of Visual C++ Toolkit 2003 & Platform SDK
rem Leave rem to use the default Visual C++ environment
rem set LIB=
rem set INCLUDE=





if not "x%INCLUDE%x" == "xx" goto Skip

if exist "%ProgramFiles%\Microsoft Platform SDK\setenv.cmd" goto PSDK
if exist "%ProgramFiles%\Microsoft SDK\setenv.bat" goto PSDK2
echo Platform SDK missing
echo Download from http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
goto END

:PSDK

call "%ProgramFiles%\Microsoft Platform SDK\setenv.cmd"
if exist "%ProgramFiles%\Microsoft Platform SDK\include\shlwapi.h" goto PSDKBOTH
:IESDK
echo Internet Development SDK component of Platform SDK is not installed
goto END

:PSDK2

call "%ProgramFiles%\Microsoft SDK\setenv.bat"
if exist "%ProgramFiles%\Microsoft SDK\include\shlwapi.h" goto PSDKBOTH
goto IESDK

:PSDKBOTH

if exist "%ProgramFiles%\Microsoft Visual C++ Toolkit 2003\vcvars32.bat" goto VCT
echo Visual C++ Toolkit 2003 missing
echo Download from http://msdn.microsoft.com/visualc/vctoolkit2003/
goto END

:VCT

call "%ProgramFiles%\Microsoft Visual C++ Toolkit 2003\vcvars32.bat"

:Skip






cd TCFoundation
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ../include/ /D _MBCS /D _LIB /D _BUILDING_TCFOUNDATION_LIB /FpRelease\TCFoundation.pch /YX /c *.cpp

link -lib /nologo /out:Release\TCFoundation.lib Release\*.obj





cd ..\LDLoader
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\LDLoader.pch /YX /c *.cpp *.c

link -lib /nologo /out:Release\LDLoader.lib Release\*.obj 





cd ..\LDLib
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ../include /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\LDLib.pch /YX /c *.cpp 

link -lib /nologo /out:Release\LDLib.lib Release\*.obj 





cd ..\TRE
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ..\include /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\TRE.pch /YX /c *.cpp

link -lib /nologo /out:Release\TRE.lib Release\*.obj





cd ..\CUI
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /D _MBCS /D _LIB /D _WIN32_WINDOWS=0x0410 /D _BUILDING_CUI_LIB /D _TC_STATIC /FpRelease\CUI.pch /YX /c *.cpp

link -lib /nologo /out:Release\CUI.lib Release\*.obj 





cd ..
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb *.exe *.res
cd ..

rc /l 0x409 /foRelease\AppResources.res /d NDEBUG AppResources.rc

cl %CFLAGS% /I . /I ./include /D _WINDOWS /D _WIN32_WINDOWS=0x0410 /D _TC_STATIC /c *.cpp

link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib unzip32.lib /nologo /subsystem:windows /incremental:no /pdb:Release\LDView.pdb /machine:I386 /out:Release\LDView.exe /libpath:lib Release\*.obj Release\AppResources.res CUI\Release\CUI.lib TRE\Release\TRE.lib LDLib\Release\LDLib.lib LDLoader\Release\LDLoader.lib TCFoundation\Release\TCFoundation.lib

:End
