@echo off
set JPEGVER=3.1.2

if "%1%"  == "uninstall" goto uninstall

if not exist libjpeg-turbo-%JPEGVER%-vc-x64.exe curl -OJL https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/%JPEGVER%/libjpeg-turbo-%JPEGVER%-vc-x64.exe
if not exist libjpeg-turbo-%JPEGVER%-vc-x86.exe curl -OJL https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/%JPEGVER%/libjpeg-turbo-%JPEGVER%-vc-x86.exe
if not exist libjpeg-turbo-%JPEGVER%-vc-x64\jpeg-static.lib 7z e -y -olibjpeg-turbo-%JPEGVER%-vc-x64 libjpeg-turbo-%JPEGVER%-vc-x64.exe
if not exist libjpeg-turbo-%JPEGVER%-vc-x86\jpeg-static.lib 7z e -y -olibjpeg-turbo-%JPEGVER%-vc-x86 libjpeg-turbo-%JPEGVER%-vc-x86.exe

if not "%1%"  == "install" goto end
copy libjpeg-turbo-%JPEGVER%-vc-x64\jconfig.h   libjpeg\Windows
copy libjpeg-turbo-%JPEGVER%-vc-x64\jmorecfg.h  libjpeg\
copy libjpeg-turbo-%JPEGVER%-vc-x64\jpeglib.h   libjpeg\
copy libjpeg-turbo-%JPEGVER%-vc-x86\jpeg-static.lib ..\lib\libjpeg-vs2017.lib
copy libjpeg-turbo-%JPEGVER%-vc-x64\jpeg-static.lib ..\lib\x64\libjpeg-vs2019.lib
goto end

:uninstall
git checkout -- ..\lib\libjpeg-vs2017.lib
git checkout -- ..\lib\x64\libjpeg-vs2019.lib
git checkout -- libjpeg\Windows\jconfig.h
git checkout -- libjpeg\MacOSX\jconfig.h
git checkout -- libjpeg\jinclude.h
git checkout -- libjpeg\jmorecfg.h
git checkout -- libjpeg\jpeglib.h
:end

