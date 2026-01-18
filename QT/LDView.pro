SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp LDViewErrors.cpp \
		   AlertHandler.cpp LDViewSnapshotSettings.cpp \
           LDViewPartList.cpp misc.cpp LDViewJpegOptions.cpp \
		   LDViewModelTree.cpp LDViewBoundingBox.cpp LDViewMainWindow.cpp \
		   LDViewLatitudeLongitude.cpp LDViewMpdModel.cpp \
		   LDViewExportOption.cpp LDViewCustomizeToolbar.cpp \
		   SnapshotTaker.cpp SnapshotAlertHandler.cpp \
		   LDViewCameraLocation.cpp LDViewRotationCenter.cpp \
           LDViewStatistics.cpp QtWebClientPlugin.cpp

HEADERS	+= ModelViewerWidget.h Preferences.h LDViewErrors.h \
		   AlertHandler.h LDViewPartList.h misc.h LDViewJpegOptions.h \
		   LDViewSnapshotSettings.h LDViewModelTree.h \
		   LDViewBoundingBox.h LDViewLatitudeLongitude.h LDViewMpdModel.h \
		   LDViewExportOption.h LDViewMainWindow.h Help.h About.h \
		   OpenGLExtensions.h LDViewCustomizeToolbar.h \
		   SnapshotTaker.h SnapshotAlertHandler.h LDViewCameraLocation.h \
		   LDViewRotationCenter.h LDViewStatistics.h QtWebClientPlugin.h

FORMS	= AboutPanel.ui BoundingBoxPanel.ui ErrorPanel.ui \
		  HelpPanel.ui JpegOptionsPanel.ui LDView.ui LatitudeLongitude.ui \
		  OpenGLExtensionsPanel.ui PreferencesPanel.ui \
		  SnapshotSettingsPanel.ui ExportOptionPanel.ui \
		  ModelTreePanel.ui MpdModelSelectionPanel.ui PartList.ui \
          CustomizeToolbar.ui CameraLocationPanel.ui RotationCenterPanel.ui \
		  StatisticsPanel.ui

LANGUAGE	= C++
TRANSLATIONS   =  	ldview_en.ts \
					ldview_de.ts \
					ldview_it.ts \
					ldview_cs.ts \
					ldview_hu.ts

RESOURCES 	= resources.qrc

TEMPLATE	= app
CONFIG		+= qt opengl thread warn_on release lrelease
QT  		+= opengl network
contains(QT_VERSION, ^6\\..*) {
   QT += widgets gui core openglwidgets printsupport
   DEFINES += QOPENGLWIDGET
}
message(Qt: $$QT_VERSION)
message(Arch: $$QMAKE_HOST.arch)
contains(QT_VERSION, ^5\\..*) {
QT		+= printsupport
}
DEFINES		+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . .. .ui$$QT_MAJOR_VERSION
DBFILE		= LDView.db

DEFINES		+= QOFFSCREEN
# QOPENGLWIDGET is experimental only.
# Known issues with QOpenGLWidget:
#      - Save Snapshot generates corrupted image
#      - Background is transparent for ModelViewerWidget
#DEFINES	+= QOPENGLWIDGET

isEmpty(PREFIX):PREFIX			= /usr
isEmpty(BINDIR):BINDIR			= $$PREFIX/bin
isEmpty(DATADIR):DATADIR		= $$PREFIX/share
isEmpty(DOCDIR):DOCDIR			= $$DATADIR/doc
isEmpty(MANDIR):MANDIR			= $$DATADIR/man
isEmpty(MIMEICONDIR):MIMEICONDIR= $$DATADIR/icons/gnome/32x32/mimetypes
isEmpty(SYSCONFDIR):SYSCONFDIR	= /etc
isEmpty(MAKE):MAKE=make

UI_DIR 		= .ui$$QT_MAJOR_VERSION
MOC_DIR 	= .moc$$QT_MAJOR_VERSION

contains(QT_VERSION, ^5\\..*) {
POSTFIX = -qt5
MAKEOPT += POSTFIX=$$POSTFIX
}
contains(QT_VERSION, ^6\\..*) {
POSTFIX = -qt6
MAKEOPT += POSTFIX=$$POSTFIX
}

OBJECTS_DIR = .obj$$POSTFIX
MAKEOPT += \"TESTING=-I$$[QT_INSTALL_HEADERS] $$QMAKE_CXXFLAGS_STATIC_LIB $(TESTING)\"

DEFINES 	+= EXPORT_3DS
#DEFINES 	+= _NO_BOOST

QMAKE_CXXFLAGS       += $(Q_CXXFLAGS)
QMAKE_CFLAGS_DEBUG   += $(Q_CFLAGS)
QMAKE_CFLAGS_RELEASE += $(Q_CFLAGS)
QMAKE_LFLAGS         += $(Q_LDFLAGS)
QMAKE_CXXFLAGS       += $(TESTING)

freebsd {
message("FreeBSD")
MAKE = gmake
DEFINES += USE_CPP11
MAKEOPT += EXPORT_3DS=
}

contains(USE_CPP11,NO){
message("NO CPP11")
} else {
DEFINES += USE_CPP11
}

unix:!macx {
  UNAME = $$system(uname -m)
  LDVDEV64 = $$(LDVDEV64)
  contains(UNAME, x86_64) {
    contains(LDVDEV64, NO) {
      QMAKE_CFLAGS_RELEASE += -m32
      QMAKE_CFLAGS_DEBUG += -m32
      QMAKE_CXXFLAGS_RELEASE += -m32
      QMAKE_CXXFLAGS_DEBUG += -m32
      QMAKE_LFLAGS_RELEASE += -m32
      QMAKE_LFLAGS_DEBUG += -m32
    }
  }
  BOOSTLIB = -lboost_thread
  exists(/usr/lib/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/local/lib/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/lib64/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/local/lib64/libboost_thread-mt.so*){
    BOOSTLIB = -lboost_thread-mt
  }
  exists(/usr/lib/i386-linux-gnu/libboost_system.so*){
    BOOSTLIB += -lboost_system
  }
  exists(/usr/lib/x86_64-linux-gnu/libboost_system.so*){
    BOOSTLIB += -lboost_system
  }
  exists(/usr/include/tinyxml2.h)|exists(/usr/local/include/tinyxml2.h){
    message("tinyxml2 found")
  } else {
    message("WARNING: no tinyxml2 found using local copy")
    LIBS+= -L../3rdParty/tinyxml2
	INCLUDEPATH += ../3rdParty/tinyxml2
    tinyxml2.target = ../3rdParty/tinyxml2/libtinyxml2.a
    tinyxml2.commands = cd ../3rdParty/tinyxml2 ; $${MAKE} -f Makefile.pbartfai
    tinyxml2.depends = ../3rdParty/tinyxml2/*.cpp ../3rdParty/tinyxml2/*.h
    QMAKE_EXTRA_TARGETS += tinyxml2
    PRE_TARGETDEPS += ../3rdParty/tinyxml2/libtinyxml2.a
    QMAKE_CLEAN += ../3rdParty/tinyxml2/*.a ../3rdParty/tinyxml2/.obj/*.o
  }
  exists(/usr/include/gl2ps.h)|exists(/usr/local/include/gl2ps.h){
    message("gl2ps found")
  } else {
    message("WARNING: no gl2ps found using local copy")
    LIBS+= -L../3rdParty/gl2ps
	INCLUDEPATH += ../3rdParty/gl2ps
    gl2ps.target = ../3rdParty/gl2ps/libgl2ps.a
    gl2ps.commands = cd ../3rdParty/gl2ps ; $${MAKE}
    gl2ps.depends = ../3rdParty/gl2ps/*.c ../3rdParty/gl2ps/*.h
    QMAKE_EXTRA_TARGETS += gl2ps
    PRE_TARGETDEPS += ../3rdParty/gl2ps/libgl2ps.a
    QMAKE_CLEAN += ../3rdParty/gl2ps/*.a ../3rdParty/gl2ps/.obj/*.o
  }
  exists(/usr/include/GL/osmesa.h){
    message("OSMesa found")
  }
  exists(/usr/include/minizip/unzip.h)|exists(/usr/local/include/minizip/unzip.h){
    message("minizip found")
    DEFINES += HAVE_MINIZIP
  } else {
    message("WARNING: no minizip found using local copy")
    DEFINES += HAVE_MINIZIP
    LIBS += -L../3rdParty/minizip
    INCLUDEPATH += ../3rdParty
    minizip.target = ../3rdParty/minizip/libminizip.a
    minizip.commands = $${MAKE} -C ../3rdParty/minizip TESTING=-DNOCRYPT
    minizip.depends = ../3rdParty/minizip/*.c ../3rdParty/minizip/*.h
    QMAKE_EXTRA_TARGETS += minizip
    PRE_TARGETDEPS += ../3rdParty/minizip/libminizip.a
    QMAKE_CLEAN += ../3rdParty/minizip/*.a ../3rdParty/minizip/.obj/*.o
  }
  exists($$[QT_INSTALL_BINS]/lrelease){
    LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    message("$$[QT_INSTALL_BINS]/lrelease found")
  } else {
    LRELEASE = lrelease
  }
  system("g++ --help -v 2>/dev/null| grep -q std=c++11"){
    message("C++11 feature found")
  }
  system("g++ --help -v 2>/dev/null| grep -q std=c++17"){
    message("C++17 feature found")
  }


  
  documentation.depends += compiler_translations_make_all
  documentation.path = $${DATADIR}/ldview
  documentation.files = ../Readme.txt ../Help.html ../license.txt \
						../m6459.ldr \
						../ChangeHistory.html ../8464.mpd todo.txt \
						../Textures/SansSerif.fnt \
						../LDExporter/LGEO.xml \
						ldview_de.qm ldview_cs.qm ldview_it.qm ldview_en.qm ldview_hu.qm
  target.path      = $${BINDIR}
  script.path      = $${BINDIR}
  script.extra     = $(INSTALL_PROGRAM) desktop/ldraw-thumbnailer $(INSTALL_ROOT)$${BINDIR}
  man.path         = $${MANDIR}/man1
  man.extra        = $(INSTALL_FILE) LDView.1 desktop/ldraw-thumbnailer.1 $(INSTALL_ROOT)$${MANDIR}/man1 ; $(COMPRESS) $(INSTALL_ROOT)$${MANDIR}/man1/LDView.1 $(INSTALL_ROOT)$${MANDIR}/man1/ldraw-thumbnailer.1
  metainfo.path    = $${DATADIR}/metainfo
  metainfo.files   = desktop/io.github.tcobbs.LDView.metainfo.xml
  mimeinfo.path    = $${DATADIR}/mime-info
  mimeinfo.files   = desktop/ldraw.mime desktop/ldraw.keys
  mimepack.path    = $${DATADIR}/mime/packages
  mimepack.files   = desktop/ldraw.xml
  appreg.path      = $${DATADIR}/application-registry
  appreg.files     = desktop/ldview.applications
  apps.path        = $${DATADIR}/applications
  apps.files       = desktop/ldview.desktop
  thumbnailer.path = $${DATADIR}/thumbnailers
  thumbnailer.files= desktop/ldview.thumbnailer
  icon1.path       = $${MIMEICONDIR}
  icon1.extra      = $(INSTALL_FILE) images/LDViewIcon.png $(INSTALL_ROOT)$${MIMEICONDIR}/gnome-mime-application-x-ldraw.png
  icon2.path       = $${MIMEICONDIR}
  icon2.extra      = $(INSTALL_FILE) images/LDViewIcon.png $(INSTALL_ROOT)$${MIMEICONDIR}/gnome-mime-application-x-multipart-ldraw.png
  icon3.path       = $${DATADIR}/pixmaps
  icon3.extra      = $(INSTALL_FILE) images/LDViewIcon128.png $(INSTALL_ROOT)$${DATADIR}/pixmaps/gnome-ldraw.png
  icon4.path	   = $${DATADIR}/pixmaps
  icon4.extra      = $(INSTALL_FILE) images/LDViewIcon128.png $(INSTALL_ROOT)$${DATADIR}/pixmaps/ldview.png
  INSTALLS += documentation target man metainfo mimeinfo mimepack appreg \
              apps thumbnailer icon1 icon2 icon3 icon4 script
  LIBS += -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -L../boost/lib \
          -lLDraw$$POSTFIX -L../LDExporter -lX11
  contains(DEFINES,USE_CPP11){
  contains(QT_VERSION, ^6\\..*) {
      MAKEOPT+= USE_CPP17=YES
      QMAKE_CXXFLAGS+= -std=c++17
      DEFINES+= _NO_BOOST
  } else {
	  MAKEOPT+= USE_CPP11=YES
	  QMAKE_CXXFLAGS+= -std=c++11
	  DEFINES+= _NO_BOOST
    }
  }
  contains(DEFINES,_NO_BOOST){
	MAKEOPT+= USE_BOOST=NO
  }
  contains(CONFIG,debug){
    MAKEOPT+= debug
  }
  lib3ds.target = ../3rdParty/lib3ds/lib3ds.a
  lib3ds.commands = cd ../3rdParty/lib3ds ; $${MAKE}
  lib3ds.depends = ../3rdParty/lib3ds/*.c ../3rdParty/lib3ds/*.h
  QMAKE_CLEAN += ../3rdParty/lib3ds/lib3ds.a  ../3rdParty/lib3ds/.obj/*.o
  PRE_TARGETDEPS += ../3rdParty/lib3ds/lib3ds.a
  LIBS += -L../3rdParty/lib3ds
  ldlib.target = ../LDLib/libLDraw$${POSTFIX}.a
  ldlib.commands = cd ../LDLib ; $${MAKE} $$MAKEOPT
  ldlib.depends = ../LDLib/*.cpp ../LDLib/*.h
  tre.target = ../TRE/libTRE$${POSTFIX}.a
  tre.commands = cd ../TRE ; $${MAKE} $$MAKEOPT
  tre.depends = ../TRE/*.cpp ../TRE/*.h
  tcfoundation.target = ../TCFoundation/libTCFoundation$${POSTFIX}.a
  tcfoundation.commands = cd ../TCFoundation ; $${MAKE} $$MAKEOPT
  tcfoundation.depends = ../TCFoundation/*.cpp ../TCFoundation/*.h
  ldloader.target = ../LDLoader/libLDLoader$${POSTFIX}.a
  ldloader.commands = cd ../LDLoader ; $${MAKE} $$MAKEOPT
  ldloader.depends = ../LDLoader/*.cpp ../LDLoader/*.h
  ldexporter.target = ../LDExporter/libLDExporter$${POSTFIX}.a
  ldexporter.commands = cd ../LDExporter ; $${MAKE} $$MAKEOPT
  ldexporter.depends = ../LDExporter/*.cpp ../LDExporter/*.h
  QMAKE_EXTRA_TARGETS += ldlib tre tcfoundation ldloader ldexporter lib3ds
  PRE_TARGETDEPS += ../LDLib/libLDraw$${POSTFIX}.a ../TRE/libTRE$${POSTFIX}.a \
                    ../TCFoundation/libTCFoundation$${POSTFIX}.a ../LDLoader/libLDLoader$${POSTFIX}.a \
					../LDExporter/libLDExporter$${POSTFIX}.a
  QMAKE_CLEAN += ../[TLg]*/.obj$$POSTFIX/*.o ../[TLg]*/lib*.a
  initrans_hu.path  = $${DATADIR}/ldview
  initrans_hu.extra = $(INSTALL_FILE) ../Translations/Hungarian/LDViewMessages.ini \
		$(INSTALL_ROOT)/$${DATADIR}/ldview/LDViewMessages_hu.ini
  initrans_cs.path  = $${DATADIR}/ldview
  initrans_cs.extra = $(INSTALL_FILE) ../Translations/Czech/LDViewMessages.ini \
		$(INSTALL_ROOT)/$${DATADIR}/ldview/LDViewMessages_cs.ini
  initrans_de.path  = $${DATADIR}/ldview
  initrans_de.extra = $(INSTALL_FILE) ../Translations/German/LDViewMessages.ini \
		$(INSTALL_ROOT)/$${DATADIR}/ldview/LDViewMessages_de.ini
  initrans_it.path  = $${DATADIR}/ldview
  initrans_it.extra= $(INSTALL_FILE) ../Translations/Italian/LDViewMessages.ini \
		$(INSTALL_ROOT)/$${DATADIR}/ldview/LDViewMessages_it.ini
  initrans_en.path  = $${DATADIR}/ldview
  initrans_en.extra = cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
		>$(INSTALL_ROOT)/$${DATADIR}/ldview/LDViewMessages.ini
  INSTALLS += initrans_hu initrans_de initrans_cs initrans_it initrans_en
  ini.depends = ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini
  ini.target = LDViewMessages.ini
  ini.commands = cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
                 > LDViewMessages.ini
  QMAKE_EXTRA_TARGETS += ini
  PRE_TARGETDEPS += LDViewMessages.ini
  QMAKE_CLEAN += LDViewMessages.ini
}

macx{
  contains(QT_VERSION, ^6\\..*) {
  QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
  }
  QMAKE_CLEAN += ../[TLg]*/.obj$$POSTFIX/*.o ../[TLg]*/lib*.a ../3rdParty/*/*.[ao] ../3rdParty/*/.obj*/*.o
  LIBS += -L../LDLib -lLDraw$$POSTFIX
  LIBS += -L../TRE  -lTRE$$POSTFIX
  LIBS += -L../TCFoundation -lTCFoundation$$POSTFIX
  LIBS += -L../LDLoader -lLDLoader$$POSTFIX
  LIBS += -L../LDExporter -lLDExporter$$POSTFIX
  DEFINES += GL_SILENCE_DEPRECATION
  MAKEOPT = TESTING=\"-F$$[QT_INSTALL_LIBS] -mmacosx-version-min=$${QMAKE_MACOSX_DEPLOYMENT_TARGET} -D_QT -arch x86_64 -arch arm64\" POSTFIX=$$POSTFIX
  contains(QT_VERSION, ^6\\..*) {
      MAKEOPT+= USE_CPP17=YES
      QMAKE_CXXFLAGS+= -std=c++17
      DEFINES+= _NO_BOOST
  }
  contains(CONFIG,debug){
    MAKEOPT+= debug
  }
  ldlib.target = ../LDLib/libLDraw$${POSTFIX}.a
  ldlib.commands = cd ../LDLib ; $${MAKE} $$MAKEOPT
  ldlib.depends = ../LDLib/*.cpp ../LDLib/*.h
  tre.target = ../TRE/libTRE$${POSTFIX}.a
  tre.commands = cd ../TRE ; $${MAKE} $$MAKEOPT
  tre.depends = ../TRE/*.cpp ../TRE/*.h
  tcfoundation.target = ../TCFoundation/libTCFoundation$${POSTFIX}.a
  tcfoundation.commands = cd ../TCFoundation ; $${MAKE} $$MAKEOPT
  tcfoundation.depends = ../TCFoundation/*.cpp ../TCFoundation/*.h
  ldloader.target = ../LDLoader/libLDLoader$${POSTFIX}.a
  ldloader.commands = cd ../LDLoader ; $${MAKE} $$MAKEOPT
  ldloader.depends = ../LDLoader/*.cpp ../LDLoader/*.h
  ldexporter.target = ../LDExporter/libLDExporter$${POSTFIX}.a
  ldexporter.commands = cd ../LDExporter ; $${MAKE} $$MAKEOPT
  ldexporter.depends = ../LDExporter/*.cpp ../LDExporter/*.h
  QMAKE_EXTRA_TARGETS += ldlib tre tcfoundation ldloader ldexporter
  PRE_TARGETDEPS += ../LDLib/libLDraw$${POSTFIX}.a \
                    ../TRE/libTRE$${POSTFIX}.a \
                    ../TCFoundation/libTCFoundation$${POSTFIX}.a \
                    ../LDLoader/libLDLoader$${POSTFIX}.a \
                    ../LDExporter/libLDExporter$${POSTFIX}.a
  LIBS+= -L../3rdParty/gl2ps
  gl2ps.target = ../3rdParty/gl2ps/libgl2ps.a
  gl2ps.commands = cd ../3rdParty/gl2ps ; $${MAKE} TESTING+=-mmacosx-version-min=$${QMAKE_MACOSX_DEPLOYMENT_TARGET} TESTING+=\"-arch x86_64 -arch arm64\"
  gl2ps.depends = ../3rdParty/gl2ps/*.c ../3rdParty/gl2ps/*.h
  QMAKE_EXTRA_TARGETS += gl2ps
  PRE_TARGETDEPS += ../3rdParty/gl2ps/libgl2ps.a
  INCLUDEPATH += ../3rdParty/gl2ps
  QMAKE_CLEAN += ../3rdParty/gl2ps/.obj/*.o ../3rdParty/gl2ps/libgl2ps.a

  lib3ds.target = ../3rdParty/lib3ds/lib3ds.a
  lib3ds.commands = cd ../3rdParty/lib3ds ; $${MAKE} TESTING+=-mmacosx-version-min=$${QMAKE_MACOSX_DEPLOYMENT_TARGET} TESTING+=\"-arch x86_64 -arch arm64\"
  lib3ds.depends = ../3rdParty/lib3ds/*.c ../3rdParty/lib3ds/*.h
  QMAKE_CLEAN += ../3rdParty/lib3ds/lib3ds.a  ../3rdParty/lib3ds/.obj/*.o
  PRE_TARGETDEPS += ../3rdParty/lib3ds/lib3ds.a
  LIBS += -L../3rdParty/lib3ds
  QMAKE_EXTRA_TARGETS += lib3ds

  LIBS+= -L../3rdParty/tinyxml2
  tinyxml2.target = ../3rdParty/tinyxml2/libtinyxml2.a
  tinyxml2.commands = cd ../3rdParty/tinyxml2 ; $${MAKE} -f Makefile.pbartfai TESTING=\"-mmacosx-version-min=$${QMAKE_MACOSX_DEPLOYMENT_TARGET} -arch x86_64 -arch arm64\"
  tinyxml2.depends = ../3rdParty/tinyxml2/*.cpp ../3rdParty/tinyxml2/*.h
  QMAKE_EXTRA_TARGETS += tinyxml2
  PRE_TARGETDEPS += ../3rdParty/tinyxml2/libtinyxml2.a
  QMAKE_CLEAN += ../3rdParty/tinyxml2/*.a ../3rdParty/tinyxml2/.obj/*.o
  INCLUDEPATH += ../3rdParty/tinyxml2
  LIBS += -L../lib/MacOSX -l3ds -lpng16u -ljpegu -lz
  minizip.target = ../3rdParty/minizip/libminizip.a
  minizip.commands = cd ../3rdParty/minizip ; $${MAKE} -e CFLAGS=\"-O -DUSE_FILE32API -mmacosx-version-min=$${QMAKE_MACOSX_DEPLOYMENT_TARGET} -arch x86_64 -arch arm64\" unzip.o ioapi.o zip.o ; ar rcs libminizip.a *.o
  minizip.depends  = ../3rdParty/minizip/*.c ../3rdParty/minizip/*.h
  QMAKE_CLEAN += ../3rdParty/minizip/libminizip.a ../3rdParty/minizip/.obj/*.o
  LIBS += -L ../3rdParty/minizip -lminizip
  DEFINES += HAVE_MINIZIP
  INCLUDEPATH += ../3rdParty/minizip
  QMAKE_EXTRA_TARGETS += minizip
  PRE_TARGETDEPS += ../3rdParty/minizip/libminizip.a
  INCLUDEPATH += ../3rdParty/libpng ../3rdParty/libpng/MacOSX ../3rdParty/libjpeg ../3rdParty/libjpeg/MacOSX
}

win32 {
  message(MSVC: $$MSVC_VER)
  CONFIG += static_runtime
  QMAKE_CXXFLAGS_RELEASE += /FI winsock2.h /FI winsock.h
#
# supress boost 1.42 warnings
#
  QMAKE_CXXFLAGS_RELEASE += /wd4675

  DEFINES += _CRT_SECURE_NO_WARNINGS

  ini.commands = copy /y /a ..\LDViewMessages.ini+..\LDExporter\LDExportMessages.ini LDViewMessages.ini
  ini.target = LDViewMessages.ini
  ini.depends = ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini
  QMAKE_EXTRA_WIN_TARGETS += ini
  PRE_TARGETDEPS += LDViewMessages.ini

  DEFINES += _TC_STATIC
#  INCLUDE += -I../../boost_1_33_1
  contains(QT_ARCH, i386) {
    LIBS += -L../Build/Release -L../lib -lunzip32-vs2017 -llibpng16-vs2017 -lzlib-vs2017 -llib3ds-vs2017 -llibjpeg-vs2017
  }
  exists ($(VCINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml2 -llibboost_thread-vc90-mt-s -L../Build/tinyxml/Release
  }
  exists ($(VCTOOLKITINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml_STL-vc2003 -llibboost_thread-vc71-mt-s
  }
  contains(DEFINES,EXPORT_3DS) {
    LIBS += -llib3ds
  }
  INCLUDEPATH += ../include ../3rdParty/libpng ../3rdParty/libpng/windows
  INCLUDEPATH += ../3rdParty/libjpeg ../3rdParty/libjpeg/windows ../3rdParty/gl2ps ../3rdParty/tinyxml2
  contains(QT_ARCH, x86_64) {
    LIBS += -L../Build/Release64 -L../lib/x64 -lunzip32-vs2019 -llibpng16-vs2019 -lzlib-vs2019 -llib3ds-vs2019 -llibjpeg-vs2019
  }
  LIBS += -ltinyxml_STL -lgl2ps -lLDExporter -lLDLib -lLDLoader -lTRE -lTCFoundation
  LIBS += glu32.lib Shlwapi.lib -NODEFAULTLIB:msvcrt libucrt.lib OpenGL32.lib
  LIBS += $$QMAKE_LIBS_NETWORK $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_GUI
}

translations.input = TRANSLATIONS
translations.output = ${QMAKE_FILE_BASE}.qm
translations.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
translations.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += translations
#PRE_TARGETDEPS += compiler_translations_make_all

QMAKE_CLEAN += *.qm

!win32{
LIBS	+= -lLDLoader$$POSTFIX -lTRE$$POSTFIX -lTCFoundation$$POSTFIX
}
unix:!macx {
		LIBS += -lz -ljpeg -lpng -lGLU -lGL
	contains(DEFINES,HAVE_MINIZIP) {
		LIBS += -lminizip
	}
}
!win32{
LIBS	+= -lgl2ps -lLDExporter$$POSTFIX
}

unix {
# This has to be down here, because -ltinyxml2 has to come after -lLDExporter.
	LIBS	+= -ltinyxml2
	LIBS += -L../lib
	UNAME = $$system(uname -m)
	LDVDEV64 = $$(LDVDEV64)
# Use static boost library on Ubuntu and other systems that have it in
# /usr/lib.
	exists(/usr/lib/libboost_thread-mt.a){
		BOOSTLIB = /usr/lib/libboost_thread-mt.a
	}
	exists(../boost/lib/libboost_thread.a){
		BOOSTLIB = ../boost/lib/libboost_thread.a
	}
	linux-g++-64{
		contains(DEFINES,EXPORT_3DS) {
			LIBS += -l3ds
		}
		exists(/usr/lib64/libboost_thread-mt.a){
			BOOSTLIB = /usr/lib64/libboost_thread-mt.a
		}
	}
	else {
		contains(DEFINES,EXPORT_3DS) {
			LIBS += -l3ds
		}
	}
}
contains(DEFINES,_NO_BOOST){
} else {
LIBS += $$BOOSTLIB
}

VERSION = 4.6.1
macx {
# This has to be down here, because -ltinyxml has to come after -lLDExporter.
	LIBS	+= -ltinyxml2
	RC_FILE = images/LDView.icns
	helpfile.target = LDView.app/Contents/MacOS/Help.html
	helpfile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../Help.html \
		LDView.app/Contents/MacOS
	helpfile.depends = ../Help.html
	messagefile.target = LDView.app/Contents/MacOS/LDViewMessages.ini
	messagefile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../LDViewMessages.ini \
		LDView.app/Contents/MacOS
	messagefile.depends = ../LDViewMessages.ini
	fontfile.target = LDView.app/Contents/MacOS/SansSerif.fnt
	fontfile.commands = @$(CHK_DIR_EXISTS) LDView.app/Contents/MacOS/ || \
		$(MKDIR) LDView.app/Contents/MacOS/ ; cp ../Textures/SansSerif.fnt \
		LDView.app/Contents/MacOS
	fontfile.depends = ../Textures/SansSerif.fnt
	QMAKE_EXTRA_UNIX_TARGETS += helpfile messagefile fontfile
	POST_TARGETDEPS += LDView.app/Contents/MacOS/LDViewMessages.ini \
		LDView.app/Contents/MacOS/Help.html \
		LDView.app/Contents/MacOS/SansSerif.fnt
}

test.target = test
test.commands = ./LDView ../8464.mpd -SaveSnapshot=/tmp/test.png -CheckPartTracker=0 -SaveWidth=256 -SaveHeight=256 -ShowErrors=0 -SaveActualSize=0
test.files = ../8464.mpd
QMAKE_EXTRA_TARGETS+= test
