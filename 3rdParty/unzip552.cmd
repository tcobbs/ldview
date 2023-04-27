@echo off
if exist unzip552.zip goto srcfound
curl -OJL https://sourceforge.net/projects/infozip/files/UnZip%205.x%20and%20earlier/5.52/unzip552.zip/download
:srcfound
if exist unzip552 goto dirfound
PowerShell Expand-Archive -Path unzip552.zip -DestinationPath unzip552
:dirfound
cd unzip552
nmake -f win32\Makefile USEZLIB=1 clean lib LOCAL_UNZIP="-I..\zlib -DUSE_ZLIB" cvarsdll=$(cvarsmt)
