SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp TCColorButton.cpp LDViewErrors.cpp LDViewExtraDir.cpp
HEADERS	+= ModelViewerWidget.h Preferences.h TCColorButton.h LDViewErrors.h LDViewExtraDir.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= LDView.ui PreferencesPanel.ui OpenGLExtensionsPanel.ui AboutPanel.ui ErrorPanel.ui ImageHolder.ui ExtraDirPanel.ui HelpPanel.ui
IMAGES	= images/fileopen.png images/filesave.png images/print.png images/LDViewIcon.png images/LDViewIcon16.png images/StudLogo.png images/toolbar_reload.png images/toolbar_wireframe.png images/toolbar_seams.png images/toolbar_edge.png images/toolbar_privsubs.png images/toolbar_snapshot.png images/toolbar_view.png images/toolbar_preferences.png images/toolbar_bfc.png images/toolbar_light.png
TEMPLATE	=app
CONFIG	+= qt opengl warn_on release
DEFINES	+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . ..
LIBS	+= -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -lLDraw -lLDLoader -lTRE -lTCFoundation
DBFILE	= LDView.db
LANGUAGE	= C++
