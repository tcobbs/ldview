@echo off
if exist lib3ds-20080909.zip goto srcfound
curl -OJL https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lib3ds/lib3ds-20080909.zip
:srcfound
if exist lib3ds-20080909 goto dirfound
PowerShell Expand-Archive -Path lib3ds-20080909.zip -DestinationPath .
:dirfound
cd lib3ds-20080909
cd src
for /d %%c in ("%VCToolsInstallDir%\..\14.16.*") do set CC1=%%c
for /d %%c in ("%VCToolsInstallDir%\..\14.29.*") do set CC2=%%c
"%CC1%"\bin\HostX64\x86\cl /nologo /MT /D WIN32 /D NDEBUG /D _WINDOWS /D LIB3DS_EXPORTS /c *.c
"%CC1%"\bin\HostX64\x86\link /lib /nologo /MACHINE:X86 /out:lib3ds.lib *.obj
copy lib3ds.lib ..\..\..\lib\lib3ds-vs2017.lib
"%CC2%"\bin\HostX64\x64\cl /nologo /MT /D WIN32 /D NDEBUG /D _WINDOWS /D LIB3DS_EXPORTS /c *.c
"%CC2%"\bin\HostX64\x64\link /lib /nologo /MACHINE:X64 /out:lib3ds.lib *.obj
copy lib3ds.lib ..\..\..\lib\x64\lib3ds-vs2019.lib
cd ..\..