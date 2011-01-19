cd src
cl /nologo /MT /D WIN32 /D NDEBUG /D _WINDOWS /D LIB3DS_EXPORTS /c *.c
link /lib /nologo /out:lib3ds.lib *.obj