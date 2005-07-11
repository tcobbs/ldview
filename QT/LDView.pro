SOURCES	+= QTMain.cpp ModelViewerWidget.cpp Preferences.cpp LDViewErrors.cpp LDViewExtraDir.cpp
HEADERS	+= ModelViewerWidget.h Preferences.h LDViewErrors.h LDViewExtraDir.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  documentation.path = /usr/local/share/ldview
  documentation.files = ../Readme.txt ../Help.html ../license.txt ../m6459.ldr ../LDViewMessages.ini ../ChangeHistory.html
  target.path = /usr/local/bin
  INSTALLS += documentation target
}
FORMS	= LDView.ui PreferencesPanel.ui OpenGLExtensionsPanel.ui AboutPanel.ui ErrorPanel.ui ImageHolder.ui ExtraDirPanel.ui HelpPanel.ui
IMAGES	= images/fileopen.png images/filesave.png images/print.png images/LDViewIcon.png images/LDViewIcon16.png images/StudLogo.png images/toolbar_reload.png images/toolbar_wireframe.png images/toolbar_seams.png images/toolbar_edge.png images/toolbar_privsubs.png images/toolbar_snapshot.png images/toolbar_view.png images/toolbar_preferences.png images/toolbar_bfc.png images/toolbar_light.png images/error_colinear.png images/error_color.png images/error_concave_quad.png images/error_determinant.png images/error_fnf.png images/error_info.png images/error_matching_points.png images/error_matrix.png images/error_non_flat_quad.png images/error_opengl.png images/error_parse.png images/error_vertex_order.png
TEMPLATE	=app
CONFIG	+= qt opengl warn_on release
DEFINES	+= QT_THREAD_SUPPORT _QT
INCLUDEPATH	+= . ..
exists($(QTDIR)/include/Qt3Support/q3button.h){
	CONFIG 	+= uic3
	DEFINES += QT3_SUPPORT
	QT 	+= qt3support opengl
	INCLUDEPATH	+= $(QTDIR)/include/Qt $(QTDIR)/include/QtCore
message(QT4)
}
LIBS	+= -L../TCFoundation -L../LDLib -L../LDLoader -L../TRE -lLDraw -lLDLoader -lTRE -lTCFoundation
DBFILE	= LDView.db
LANGUAGE	= C++
