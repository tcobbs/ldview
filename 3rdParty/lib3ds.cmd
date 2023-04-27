@echo off
if exist lib3ds-20080909.zip goto srcfound
curl -OJL https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/lib3ds/lib3ds-20080909.zip
:srcfound
if exist lib3ds-20080909 goto dirfound
PowerShell Expand-Archive -Path lib3ds-20080909.zip -DestinationPath .
:dirfound
cd lib3ds-20080909
cd src
cl /nologo /MT /D WIN32 /D NDEBUG /D _WINDOWS /D LIB3DS_EXPORTS /c *.c
link /lib /nologo /out:lib3ds.lib *.obj