SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp LDViewErrors.cpp \
		   LDViewExtraDir.cpp AlertHandler.cpp LDViewSnapshotSettings.cpp \
           LDViewPartList.cpp misc.cpp LDViewJpegOptions.cpp \
		   LDViewModelTree.cpp LDViewBoundingBox.cpp LDViewMainWindow.cpp \
		   LDViewLatitudeLongitude.cpp LDViewMpdModel.cpp \
		   LDViewExportOption.cpp LDViewCustomizeToolbar.cpp

HEADERS	+= ModelViewerWidget.h Preferences.h LDViewErrors.h LDViewExtraDir.h \
		   AlertHandler.h LDViewPartList.h misc.h LDViewJpegOptions.h \
		   LDViewSnapshotSettings.h LDViewModelTree.h \
		   LDViewBoundingBox.h LDViewLatitudeLongitude.h LDViewMpdModel.h \
		   LDViewExportOption.h LDViewMainWindow.h Help.h About.h \
		   OpenGLExtensions.h LDViewCustomizeToolbar.h

FORMS	= AboutPanel.ui BoundingBoxPanel.ui ErrorPanel.ui ExtraDirPanel.ui \
		  HelpPanel.ui JpegOptionsPanel.ui LDView.ui LatitudeLongitude.ui \
		  OpenGLExtensionsPanel.ui PreferencesPanel.ui \
		  SnapshotSettingsPanel.ui ExportOptionPanel.ui \
		  ModelTreePanel.ui MpdModelSelectionPanel.ui PartList.ui \
          CustomizeToolbar.ui

LANGUAGE	= C++
TRANSLATIONS   =  	ldview_en.ts \
					ldview_de.ts \
					ldview_it.ts \
					ldview_cz.ts \
					ldview_hu.ts

RESOURCES 	= resources.qrc

TEMPLATE	= app
CONFIG		+= qt opengl thread warn_on release
QT  		+= opengl
contains(QT_VERSION, ^5\\..*) {
QT		+= printsupport
}
DEFINES		+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . .. ../include 
DBFILE		= LDView.db
PREFIX		= /usr

UI_DIR 		= .ui
MOC_DIR 	= .moc

contains(QT_VERSION, ^5\\..*) {
POSTFIX = -qt5
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

unix {
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
  exists(/usr/include/tinyxml.h){
    message("tinyxml found")
  } else {
    message("WARNING: no tinyxml found using local copy")
    LIBS+= -L../3rdParty/tinyxml
    tinyxml.target = ../3rdParty/tinyxml/libtinyxml.a
    tinyxml.commands = cd ../3rdParty/tinyxml ; make -f Makefile.pbartfai
    tinyxml.depends = ../3rdParty/tinyxml/*.cpp ../3rdParty/tinyxml/*.h
    QMAKE_EXTRA_TARGETS += tinyxml
    PRE_TARGETDEPS += ../3rdParty/tinyxml/libtinyxml.a
    QMAKE_CLEAN += ../3rdParty/tinyxml/*.a ../3rdParty/tinyxml/.obj/*.o
  }
  exists(/usr/include/gl2ps.h){
    message("gl2ps found")
  } else {
    message("WARNING: no gl2ps found using local copy")
    LIBS+= -L../gl2ps
    gl2ps.target = ../gl2ps/libgl2ps.a
    gl2ps.commands = cd ../gl2ps ; make
    gl2ps.depends = ../gl2ps/*.c ../gl2ps/*.h
    QMAKE_EXTRA_TARGETS += gl2ps
    PRE_TARGETDEPS += ../gl2ps/libgl2ps.a
  }
  
  documentation.depends += compiler_translations_make_all
  documentation.path = $${PREFIX}/share/ldview
  documentation.files = ../Readme.txt ../Help.html ../license.txt \
						../m6459.ldr \
						../ChangeHistory.html ../8464.mpd todo.txt \
						../Textures/SansSerif.fnt \
						ldview_de.qm ldview_cz.qm ldview_it.qm ldview_en.qm
  target.path = $${PREFIX}/bin
  INSTALLS += documentation target
  LIBS += -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -L../boost/lib \
          -lLDraw$$POSTFIX -L../LDExporter 
  contains(DEFINES,_NO_BOOST){
	MAKEOPT+= USE_BOOST=NO
  }
  contains(CONFIG,debug){
    MAKEOPT+= debug
  }
  ldlib.target = ../LDLib/libLDraw$$POSTFIX.a
  ldlib.commands = cd ../LDLib ; make $$MAKEOPT
  ldlib.depends = ../LDLib/*.cpp ../LDLib/*.h
  tre.target = ../TRE/libTRE$$POSTFIX.a
  tre.commands = cd ../TRE ; make $$MAKEOPT
  tre.depends = ../TRE/*.cpp ../TRE/*.h
  tcfoundation.target = ../TCFoundation/libTCFoundation$$POSTFIX.a
  tcfoundation.commands = cd ../TCFoundation ; make $$MAKEOPT
  tcfoundation.depends = ../TCFoundation/*.cpp ../TCFoundation/*.h
  ldloader.target = ../LDLoader/libLDLoader$$POSTFIX.a
  ldloader.commands = cd ../LDLoader ; make $$MAKEOPT
  ldloader.depends = ../LDLoader/*.cpp ../LDLoader/*.h
  ldexporter.target = ../LDExporter/libLDExporter$$POSTFIX.a
  ldexporter.commands = cd ../LDExporter ; make $$MAKEOPT
  ldexporter.depends = ../LDExporter/*.cpp ../LDExporter/*.h
  QMAKE_EXTRA_TARGETS += ldlib tre tcfoundation ldloader ldexporter
  PRE_TARGETDEPS += ../LDLib/libLDraw$$POSTFIX.a ../TRE/libTRE$$POSTFIX.a \
                    ../TCFoundation/libTCFoundation$$POSTFIX.a ../LDLoader/libLDLoader$$POSTFIX.a \
					../LDExporter/libLDExporter$$POSTFIX.a
  QMAKE_CLEAN += ../[TLg]*/.obj$$POSTFIX/*.o ../[TLg]*/lib*.a
  initrans.path = $${PREFIX}/share/ldview
  initrans.extra = cp ../Translations/Hungarian/LDViewMessages.ini \
$(INSTALL_ROOT)/$${PREFIX}/share/ldview/LDViewMessages_hu.ini ;\
cp ../Translations/Czech/LDViewMessages.ini \
$(INSTALL_ROOT)/$${PREFIX}/share/ldview/LDViewMessages_cz.ini ;\
cp ../Translations/German/LDViewMessages.ini \
$(INSTALL_ROOT)/$${PREFIX}/share/ldview/LDViewMessages_de.ini ; \
cp -f ../Translations/Italian/LDViewMessages.ini \
$(INSTALL_ROOT)/$${PREFIX}/share/ldview/LDViewMessages_it.ini ;\
cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
>$(INSTALL_ROOT)/$${PREFIX}/share/ldview/LDViewMessages.ini
  INSTALLS += initrans
  ini.depends = ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini
  ini.target = LDViewMessages.ini
  ini.commands = cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini \
                 > LDViewMessages.ini
  QMAKE_EXTRA_TARGETS += ini
  PRE_TARGETDEPS += LDViewMessages.ini
  QMAKE_CLEAN += LDViewMessages.ini
}

win32 {
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
  INCLUDE += -I../../boost_1_33_1
  LIBS += -L../Build/TCFoundation/Release -L../Build/LDLib/Release -L../Build/LDLoader/Release \
          -L../Build/TRE/Release -lLDLib -L../Build/LDExporter/Release -L../lib -lunzip32 \
		 -L../Build/gl2ps/Release
  exists ($(VCINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml -llibboost_thread-vc90-mt-s -L../Build/tinyxml/Release
  }
  exists ($(VCTOOLKITINSTALLDIR)/bin/cl.exe) {
    LIBS    += -ltinyxml_STL-vc2003 -llibboost_thread-vc71-mt-s
  }
  contains(DEFINES,EXPORT_3DS) {
    LIBS += -llib3ds
  }
}

translations.input = TRANSLATIONS
translations.output = ${QMAKE_FILE_BASE}.qm
translations.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
translations.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += translations
#PRE_TARGETDEPS += compiler_translations_make_all

QMAKE_CLEAN += *.qm

LIBS	+= -lLDLoader$$POSTFIX -lTRE$$POSTFIX -lTCFoundation$$POSTFIX
unix {
		LIBS += -lz -ljpeg -lpng -lGLU
}
win32 {
		LIBS += -llibjpeg-vc2005
}
LIBS	+= -lgl2ps -lLDExporter$$POSTFIX

unix {
# This has to be down here, because -ltinyxml has to come after -lLDExporter.
	LIBS	+= -ltinyxml
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
			LIBS += -l3ds-64
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

VERSION = 4.0.1
macx {
# This has to be down here, because -ltinyxml has to come after -lLDExporter.
	LIBS	+= -ltinyxml
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
