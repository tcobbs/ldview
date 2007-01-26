libtool --mode=compile g++ -c -pipe -Wall -W -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m32 -march=i386 -mtune=generic -fasynchronous-unwind-tables -fPIC  -DQT_NO_DEBUG -DQT_PLUGIN -DQT_SHARED -DQT_THREAD_SUPPORT -I/usr/lib/qt-3.3/mkspecs/default -I. -I/usr/include/kde -I/usr/lib/qt-3.3/include -o ldviewthumbnailcreator.lo ldviewthumbnailcreator.cpp
rm -f libldviewthumbnail.so
libtool --mode=link g++ -shared -o libldviewthumbnail.la ldviewthumbnailcreator.lo -rpath /usr/lib/kde3  -L/usr/lib/qt-3.3/lib -lqt-mt

