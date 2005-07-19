#ifndef __MODELVIEWERWIDGET_H__
#define __MODELVIEWERWIDGET_H__

#include <qgl.h>
#include <qdatetime.h>
#include <TCFoundation/TCObject.h>

#include "Preferences.h"
#include "LDViewExtraDir.h"

class LDrawModelViewer;
class LDView;
class QStatusBar;
class QToolBar;
class QProgressBar;
class QLabel;
class QApplication;
class OpenGLExtensionsPanel;
class AboutPanel;
class HelpPanel;
class ExtraDirPanel;
class QFileDialog;
class LDViewErrors;
class QMenuBar;
class QPopupMenu;
class QFileInfo;
class QAction;
class QDir;
class TCStringArray;
class LDLError;
class TCProgressAlert;
class QTextBrowser;

#define MAX_MOUSE_BUTTONS 10

// Ahhh!!! Nobody said anything about multiple inheritance!!!!
class ModelViewerWidget : public QGLWidget, TCObject
{
	Q_OBJECT
public:
	ModelViewerWidget(QWidget *parent=NULL, const char *name=NULL);
	~ModelViewerWidget(void);
	void loadModel(const char *filename);
	void finishLoadModel(void);
	void showPreferences(void);
	LDrawModelViewer *getModelViewer(void) { return modelViewer; }
	void setMainWindow(LDView *value);

	void setShowFPS(bool value) { showFPS = value; }
	void setApplication(QApplication *value);

	void doFileOpen(void);
	void doFileReload(void);
	void doFileLDrawDir(void);
	void showFileExtraDir(void);
	void doFileCancelLoad(void);
	void doViewFullScreen(void);
	void doViewReset(void);
	void doViewStatusBar(bool flag);
	void doViewToolBar(bool flag);
	void doViewErrors(void);
	void doHelpOpenGLDriverInfo(void);
	void doHelpContents(void);
	void doHelpAbout(void);
	void doHelpAboutQt(void);
	void doApply(void);
	void doPollChanged(QAction *action);
	void doViewModeChanged(QAction *action);
	void doZoomToFit(void);
	bool doFileSave(void);
	bool fileExists(char* filename);
	bool shouldOverwriteFile(char* filename);
	bool doFileSave(char *saveFilename);
	bool getSaveFilename(char* saveFilename, int len);
	static bool staticImageProgressCallback(char* message, float progress,
            void* userData);
	bool writeImagee(char *filename, int width, int height,
                     char *buffer, char *formatName);
	bool saveImage(char *filename, int imageWidth, int imageHeight);
	bool writePng(char *filename, int width, int height,
            char *buffer);
    bool writeBmp(char *filename, int width, int height,
            char *buffer);
	bool writeImage(char *filename, int width, int height,
            char *buffer, char *formatName);
	char *grabImage(int imageWidth, int imageHeight,
            char *buffer = NULL);
	int roundUp(int value, int nearest);

	void doFrontViewAngle(void);
    void doBackViewAngle(void);
    void doLeftViewAngle(void);
    void doRightViewAngle(void);
    void doTopViewAngle(void);
    void doBottomViewAngle(void);
    void doIsoViewAngle(void);
    void doSaveDefaultViewAngle(void);
	void doShowViewInfo(void);
	QSize minimumSize(void);
	void doWireframe(bool);
	void doEdge(bool);
	void doLighting(bool);
	void doPrimitiveSubstitution(bool);
	void doSeams(bool);
	void reflectSettings(void);

protected slots:
	virtual void doAboutOK(void);
	virtual void doRecentFile(int index);
	virtual void doFileMenuAboutToShow(void);
	virtual void doEditMenuAboutToShow(void);
	virtual void doViewMenuAboutToShow(void);
	virtual void doHelpMenuAboutToShow(void);

protected:
	// GL Widget overrides
	void initializeGL(void);
	void resizeGL(int width, int height);
	void swap_Buffers(void);
	void paintGL(void);

	void updateSpinRateXY(int xPos, int yPos);
	void updatePanXY(int xPos, int yPos);
	void updateHeadXY(int xPos, int yPos);
	void updateZoom(int yPos);
	void spinButtonPress(QMouseEvent *event);
	void spinButtonRelease(QMouseEvent *event);
	void zoomButtonPress(QMouseEvent *event);
	void zoomButtonRelease(QMouseEvent *event);
	void updateSpinRate(void);

	// Events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void timerEvent(QTimerEvent *event);
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	int progressCallback(char *message, float progress, bool showErrors);
	void setupProgress(void);
	void endLoad(void);
	void updateFPS(void);
	void drawFPS(void);
	void startPaintTimer(void);
	void killPaintTimer(void);
	void startPollTimer(bool immediate=false);
	void killPollTimer(void);
	void startLoadTimer(void);
	void killLoadTimer(void);
	bool verifyLDrawDir(bool forceChoose = false);
	bool verifyLDrawDir(char *value);
	char *getLDrawDir(void);
	bool promptForLDrawDir(void);
	int errorCallback(LDLError *error);
	void clearErrors(void);
	void preLoad(void);
	void postLoad(void);
	void showErrorsIfNeeded(bool onlyIfNeeded);
	void clearRecentFileMenuItems(void);
	void populateRecentFileMenuItems(void);
	void setLastOpenFile(const char *filename);
	char *truncateFilename(const char *filename);
	bool chDirFromFilename(const char *filename);
	void checkFileForUpdates(void);
	void getFileTime(const char *filename, QDateTime &value);
	void lock(void);
	void unlock(void);
	void windowActivationChange(bool oldActive);
	void processKey(QKeyEvent *event, bool press);
	void setViewMode(LDVViewMode value);
	void connectMenuShows(void);
	void setMenuItemsEnabled(QPopupMenu *menu, bool enabled);
	void ldlErrorCallback(LDLError *error);
	void progressAlertCallback(TCProgressAlert *alert);

	static void populateRecentFiles(void);
	static void recordRecentFiles(void);
//	static int staticProgressCallback(char *message, float progress,
//		void *userData);
//	static int staticErrorCallback(LDLError *error, void *userData);
	static bool staticFileCaseCallback(char *filename);
	static bool staticFileCaseLevel(QDir &dir, char *filename);

	LDrawModelViewer *modelViewer;
	bool mouseButtonsDown[MAX_MOUSE_BUTTONS];
	int lastX;
	int lastY;
	int originalZoomY;
	float rotationSpeed;
	LDVViewMode viewMode;
	int spinButton;
	int zoomButton;
	QTime lastMoveTime;
	QTime lastProgressTime;
	QTime referenceFrameTime;
	Preferences *preferences;
	ExtraDir *extradir;
	OpenGLExtensionsPanel *extensionsPanel;
	char *openGLDriverInfo;
	AboutPanel *aboutPanel;
	HelpPanel *helpContents;
	LDView *mainWindow;
	QMenuBar *menuBar;
	QPopupMenu *fileMenu;
	QPopupMenu *editMenu;
	QPopupMenu *viewMenu;
	QPopupMenu *helpMenu;
	int fileSeparatorIndex;
	int fileCancelLoadId;
	int fileReloadId;
	int fileSaveSnapshotId;
	QStatusBar *statusBar;
	QToolBar *toolBar;
	QProgressBar *progressBar;
	QLabel *progressLabel, *progressMode;
	bool loading;
	bool cancelLoad;
	QApplication *app;
	bool painting;
	float fps;
	int numFramesSinceReference;
	int paintTimer;
	int pollTimer;
	int loadTimer;
	QFileDialog *fileDialog, *saveDialog;
	bool showFPS;
	LDViewErrors *errors;
	QDateTime lastWriteTime;
	QFileInfo *fileInfo;
	int lockCount;
    bool saveActualSize;
    int saveWidth;
    int saveHeight;
    bool saveZoomToFit;
    bool saveSeries;
    int saveDigits;
	int saveImageType;
	int fullscreen;

	static TCStringArray* recentFiles;
};

#endif // __MODELVIEWERWIDGET_H__
