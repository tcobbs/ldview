SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp TCColorButton.cpp LDViewErrors.cpp 
HEADERS	+= ModelViewerWidget.h Preferences.h TCColorButton.h LDViewErrors.h 
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= LDView.ui PreferencesPanel.ui OpenGLExtensionsPanel.ui AboutPanel.ui ErrorPanel.ui ImageHolder.ui 
IMAGES	= images/fileopen.png images/filesave.png images/print.png images/LDViewIcon.png images/LDViewIcon16.png images/StudLogo.png 
TEMPLATE	=app
CONFIG	+= qt opengl warn_on release
DEFINES	+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . ..
LIBS	+= -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -lLDraw -lLDLoader -lTRE -lTCFoundation
DBFILE	= LDView.db
LANGUAGE	= C++
