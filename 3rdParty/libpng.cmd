rem libpng 1.5.1
nmake -f scripts\makefile.vcwin32 clean
copy /y scripts\pnglibconf.h.prebuilt pnglibconf.h
nmake -f scripts\makefile.vcwin32 AR="link -lib" CFLAGS="-nologo -MT -O2 -W3 -I..\zlib123" all