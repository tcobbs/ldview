@echo off
set CFLAGS=/nologo /MT /W4 /EHsc /Ox /D NDEBUG /D WIN32 /FoRelease\ /FdRelease\ /FD /G7 %EXTRACFLAGS%

rem Remove the rem the following 2 lines to force usage of Visual C++ Toolkit 2003 & Platform SDK
rem Leave rem to use the default Visual C++ environment
rem set LIB=
rem set INCLUDE=

for %%i IN ( hcw.exe ) do set HCW="%%~$PATH:i"
if not %HCW%=="" goto INNOCHECK

set HCW="%ProgramFiles%\Help Workshop\hcw.exe"
if exist %HCW% goto INNOCHECK
set HCW="%LEGO%\Help Workshop\hcw.exe"
if exist %HCW% goto INNOCHECK
echo Please download Help Workshop from: ftp://ftp.microsoft.com/Softlib/MSLFILES/hcwsetup.EXE
goto END

:INNOCHECK

set ISCC="%ProgramFiles%\Inno Setup 5\iscc.exe"
if exist %ISCC% goto SDKCHECK
set ISCC="%LEGO%\Inno Setup 5\iscc.exe"
if exist %ISCC% goto SDKCHECK
echo Please Download Inno Setup from http://www.jrsoftware.org/isdl.php#qsp
goto END

:SDKCHECK

if not "x%INCLUDE%x" == "xx" goto Skip

if exist "%ProgramFiles%\Microsoft Platform SDK\setenv.cmd" goto PSDK
if exist "%ProgramFiles%\Microsoft SDK\setenv.bat" goto PSDK2
if exist "%ProgramFiles%\Microsoft Platform SDK for Windows Server 2003 R2\setenv.cmd" goto PDSK3

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

:PDSK3

call "%ProgramFiles%\Microsoft Platform SDK for Windows Server 2003 R2\setenv.cmd"
if exist "%ProgramFiles%\Microsoft Platform SDK for Windows Server 2003 R2\include\shlwapi.h" goto PSDKBOTH
goto IESDK

:PSDKBOTH

if exist "%ProgramFiles%\Microsoft Visual C++ Toolkit 2003\vcvars32.bat" goto VCT
echo Visual C++ Toolkit 2003 missing
echo Download from http://msdn.microsoft.com/visualc/vctoolkit2003/
goto END

:VCT

call "%ProgramFiles%\Microsoft Visual C++ Toolkit 2003\vcvars32.bat"

:Skip




cd Help
echo Compiling Help using %HCW%
%HCW% /c /e LDView.hlp
cd ..



cd TCFoundation
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ../include/ /I ../include/libjpegWindows /D _MBCS /D _LIB /D _BUILDING_TCFOUNDATION_LIB /FpRelease\TCFoundation.pch /YX /c *.cpp *.c

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


cd ..\LDExporter
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ..\include /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\LDExporter.pch /YX /c *.cpp

link -lib /nologo /out:Release\LDExporter.lib Release\*.obj

cd ..\LGEOTables
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ..\include /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\LGEOTables.pch /YX /c *.cpp

link -lib /nologo /out:Release\LGEOTables.lib Release\*.obj

cd ..\gl2ps
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.lib *.idb
cd ..

cl %CFLAGS% /I .. /I ..\include /D _MBCS /D _LIB /D _TC_STATIC /FpRelease\gl2ps.pch /YX /c *.c

link -lib /nologo /out:Release\gl2ps.lib Release\*.obj


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

rc /l 0x409 /foRelease\LDView.res /d NDEBUG /d LDVIEW_APP LDView.rc

cl %CFLAGS% /I . /I ./include /D _WINDOWS /D _WIN32_WINDOWS=0x0410 /D _TC_STATIC /D LDVIEW_APP /c *.cpp

link user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib libjpeg.lib version.lib unzip32.lib /nologo /subsystem:windows /incremental:no /pdb:Release\LDView.pdb /machine:I386 /nodefaultlib:"libc.lib" /out:Release\LDView.exe /libpath:lib Release\*.obj Release\LDView.res CUI\Release\CUI.lib TRE\Release\TRE.lib LDLib\Release\LDLib.lib LDLoader\Release\LDLoader.lib TCFoundation\Release\TCFoundation.lib LDExporter\Release\LDExporter.lib gl2ps\Release\gl2ps.lib tinyxml_STL-vc2003.lib %EXTRALFLAGS%


cd Translations\German
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll
cd ..

rc /d NDEBUG /l 0x409 /foRelease\LDView.res LDView.rc

cl %CFLAGS% /D _WINDOWS /D _MBCS /D _USRDLL /D GERMAN_EXPORTS /c German.cpp

link version.lib /nologo /dll /incremental:no /pdb:Release\LDView-German.pdb /machine:I386 /out:Release\LDView-German.dll /implib:Release\LDView-German.lib Release\German.obj  Release\LDView.res

cd ..\..

cd Translations\Czech
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll
cd ..

rc /d NDEBUG /l 0x409 /foRelease\LDView.res LDView.rc
cl %CFLAGS% /D _WINDOWS /D _MBCS /D _USRDLL /D CZECH_EXPORTS /c Czech.cpp
link version.lib /nologo /dll /incremental:no /pdb:Release\LDView-Czech.pdb /machine:I386 /out:Release\LDView-Czech.dll /implib:Release\LDView-Czech.lib Release\Czech.obj  Release\LDView.res

cd ..\..

cd Translations\Italian
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll
cd ..

rc /d NDEBUG /l 0x409 /foRelease\LDView.res LDView.rc
cl %CFLAGS% /D _WINDOWS /D _MBCS /D _USRDLL /D ITALIAN_EXPORTS /c Italian.cpp
link version.lib /nologo /dll /incremental:no /pdb:Release\LDView-Italian.pdb /machine:I386 /out:Release\LDView-Italian.dll /implib:Release\LDView-Italian.lib Release\Italian.obj  Release\LDView.res

cd ..\..

cd Translations\Hungarian
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll
cd ..

rc /d NDEBUG /l 0x409 /foRelease\LDView.res LDView.rc
cl %CFLAGS% /D _WINDOWS /D _MBCS /D _USRDLL /D HUNGARIAN_EXPORTS /c Hungarian.cpp
link version.lib /nologo /dll /incremental:no /pdb:Release\LDView-Hungarian.pdb /machine:I386 /out:Release\LDView-Hungarian.dll /implib:Release\LDView-Hungarian.lib Release\Hungarian.obj  Release\LDView.res

cd ..\..

cd Launcher
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll *.com
cd ..
cl %CFLAGS% /D _CONSOLE /D _UNICODE /D UNICODE /D _MBCS /FpRelease\Launcher.pch /FoRelease\ /Yc /Zi /GL /c *.cpp
link kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib Release\Launcher.obj /nologo /subsystem:console /machine:I386 /incremental:no /pdb:Release\LDView.pdb /out:Release\LDView.com

cd ..

cd LDViewThumbs
if not exist Release mkdir Release
cd Release
del /q *.pch *.obj *.idb  *.res *lib *.dll
cd ..
midl   /tlb LDViewThumbs.tlb /h LDViewThumbs.h /iid LDViewThumbs_i.c /Oicf LDViewThumbs.idl
cl %CFLAGS% /I .. /D _WINDOWS /D _WINDLL /D _AFXDLL /D _MBCS /D _USRDLL /D _TC_STATIC /FpRelease\LDViewThumbs.pch /Yc"stdafx.h" /c *.cpp
rc /l 0x409 /d NDEBUG /foRelease\LDViewThumbs.res LDViewThumbs.rc
link gdi32.lib libpng.lib zlib.lib TCFoundation.lib libjpeg.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:LDViewThumbs.def /out:Release\LDViewThumbs.dll /implib:Release\LDViewThumbs.lib /libpath:..\lib /libpath:..\TCFoundation\Release Release\LDViewThumbExtractor.obj  Release\LDViewThumbs.obj Release\StdAfx.obj  Release\LDViewThumbs.res
cd ..

%ISCC% LDView.iss

if not exist "%QTDIR%\bin\qmake.exe" goto End
cd QT
%QTDIR%\bin\qmake.exe
nmake /nologo clean all
cd ..

:End
