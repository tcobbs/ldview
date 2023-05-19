@echo off
if exist tinyxml_2_6_2.zip goto srcfound
curl -OJL https://sourceforge.net/projects/tinyxml/files/tinyxml/2.6.2/tinyxml_2_6_2.zip/download
:srcfound
if exist tinyxml goto dirfound
PowerShell Expand-Archive -Path tinyxml_2_6_2.zip -DestinationPath .
:dirfound
cd tinyxml
cl /MT /nologo /W3 /GX /O2 /D WIN32 /D NDEBUG /D _MBCS /D _LIB /D TIXML_USE_STL /FR /FD /c tiny*.cpp
link -lib /nologo /out:tinyxml_STL.lib tiny*.obj
rem msbuild /p:PlatformToolset=v143 /p:Configuration=Release tinyxml.vcxproj