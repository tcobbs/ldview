SOURCES	+= ldviewthumbnailcreator.cpp

INCLUDEPATH += /usr/include/kde
TEMPLATE	= lib
CONFIG	+= plugin qt warn_on release
LANGUAGE	= C++
VERSION = 3.1
TARGET = ldviewthumbnail
LIBS += -lkio
QMAKE_LFLAGS_PLUGIN += -Wl,-soname -Wl,libldviewthumbmail.so
