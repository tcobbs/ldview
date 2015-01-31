#ifndef __MODELVIEWERWIDGET_H__
#define __MODELVIEWERWIDGET_H__

#include <qgl.h>
#include <QDateTime>
#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCAlertManager.h>

#include "Preferences.h"
#include "LDViewExtraDir.h"
#include "LDViewSnapshotSettings.h"
#include "LDViewJpegOptions.h"
#include "LDViewPartList.h"
#include <LDLib/LDLibraryUpdater.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDSnapshotTaker.h>
#include "LDViewBoundingBox.h"
#include "LDViewMpdModel.h"
#include "LDViewLatitudeLongitude.h"
#include "LDViewModelTree.h"
#include "About.h"
#include "Help.h"
#include "OpenGLExtensions.h"
#include <QFileDialog>

class LDrawModelViewer;
class LDViewMainWindow;
class QProgressBar;
class QApplication;
class LDViewErrors;
class QMenuBar;
class QPopupMenu;
class TCStringArray;
class LDLError;
class TCProgressAlert;
class QTextBrowser;
class AlertHandler;
class QProgressDialog;
class LDHtmlInventory;
class JpegOptions;

#define MAX_MOUSE_BUTTONS 10

#define LIBRARY_UPDATE_FINISHED 1
#define LIBRARY_UPDATE_CANCELED 2
#define LIBRARY_UPDATE_NONE 3
#define LIBRARY_UPDATE_ERROR 4

class ModelViewerWidget : public QGLWidget
{
	Q_OBJECT
public:
	ModelViewerWidget(QWidget *parent=NULL);
	~ModelViewerWidget(void);
	void loadModel(const char *filename);
	void finishLoadModel(void);
	void showPreferences(void);
	LDrawModelViewer *getModelViewer(void) { return modelViewer; }
	void setMainWindow(LDViewMainWindow *value);

	void ldlErrorCallback(LDLError *error);
	void progressAlertCallback(TCProgressAlert *alert);
	void modelViewerAlertCallback(TCAlert *alert);
	void redrawAlertCallback(TCAlert *alert);
	void captureAlertCallback(TCAlert *alert);
	void releaseAlertCallback(TCAlert *alert);
	void lightVectorChangedAlertCallback(TCAlert *alert);
	void snapshotTakerAlertCallback(TCAlert *alert);

	void setShowFPS(bool value) { showFPS = value; }
	void setApplication(QApplication *value);
	void createAboutPanel(void);

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
	void doPartList(LDHtmlInventory *htmlInventory,
			LDPartsList *partsList, const char *filename);
	void doPartList();
	void doModelTree();
	void doBoundingBox();
	void doMpdModel();
	void doHelpOpenGLDriverInfo(void);
	void doHelpContents(void);
	void doHelpAbout(void);
	void doHelpAboutQt(void);
	void doApply(void);
	void doPollChanged(LDVPollMode);
	void doViewModeChanged(LDInputHandler::ViewMode newMode);
	void doZoomToFit(void);
	void doRightSideUp(void);
	void doFilePrint(void);
	bool doFileSave(void);
	void doFileSaveSettings(void);
	void fileExport(void);
	void fileExportOption(void);
	void file3DSExportOption(void);
	void doFileJPEGOptions(void);
	bool fileExists(const char* filename);
	bool shouldOverwriteFile(char* filename);
	bool doFileSave(char *saveFilename);
	bool getSaveFilename(char* saveFilename, int len);
	static bool staticImageProgressCallback(const wchar_t* message,
		float progress, void* userData);
	bool writeImage(char *filename, int width, int height, TCByte *buffer,
		const char *formatName, bool saveAlpha = false);
	bool saveImage(char *filename, int imageWidth, int imageHeight,
		bool fromCommandLine = false);
	bool writePng(char *filename, int width, int height, TCByte *buffer,
		bool saveAlpha);
    bool writeBmp(char *filename, int width, int height, TCByte *buffer);
	TCByte *grabImage(int &imageWidth, int &imageHeight, TCByte *buffer,
		bool zoomToFit = true, bool *saveAlpha = NULL);
	int roundUp(int value, int nearest);

	void switchExamineLatLong(bool b);
	void keepRightSideUp(bool b);
	void doFrontViewAngle(void);
    void doBackViewAngle(void);
    void doLeftViewAngle(void);
    void doRightViewAngle(void);
    void doTopViewAngle(void);
    void doBottomViewAngle(void);
	void doLatLongViewAngle(void);
    void doIsoViewAngle(void);
    void doSaveDefaultViewAngle(void);
	void doShowViewInfo(void);
	void cleanupFloats(TCFloat *array, int count = 16);
	void doShowPovCamera(void);
	void doShowPovAspectRatio(bool);
	QSize minimumSize(void);
	void doWireframe(bool);
	void doWireframeFog(bool);
	void doWireframeRemoveHiddenLines(bool);
	void doTextureStud(bool);
	void doShowEdgeOnly(bool);
	void doConditionalLine(bool);
	void doHighQuality(bool);
	void doAlwaysBlack(bool);
	void doRedBackFaces(bool);
	void doGreenFrontFaces(bool);
	void doBlueNeutralFaces(bool);
	void doEdge(bool);
	void doLighting(bool);
	void doBFC(bool);
	void doAxes(bool);
	void doPrimitiveSubstitution(bool);
	void doSeams(bool);
	void reflectSettings(void);
	int mwidth, mheight;
	void calcTiling(int desiredWidth, int desiredHeight,
                    int &bitmapWidth, int &bitmapHeight,
                    int &numXTiles, int &numYTiles);
	void setupSnapshotBackBuffer(int imageWidth, int imageHeight);
	void renderOffscreenImage(void);
	bool canSaveAlpha(void);
	bool saveAlpha;
	bool installLDraw();
    void createLibraryUpdateWindow(void);
    void showLibraryUpdateWindow(bool initialInstall);
	void doLibraryUpdateFinished(int finishType);
	void checkForLibraryUpdates(void);
	void userDefaultChangedAlertCallback(TCAlert *alert);
	static QString findPackageFile(const QString &filename);
	void doRecentFile(int index);
	void nextStep();
	void prevStep();
	void firstStep();
	void lastStep();
	void updateStep();
	void gotoStep();
	LDViewModelTree *modeltree;
    BoundingBox *boundingbox;
	MpdModel *mpdmodel;
	bool isLoading() { return loading;}
	LDInputHandler::ViewMode getViewMode() { return viewMode;}

protected slots:
	virtual void doAboutOK(void);
	virtual void doLibraryUpdateCanceled(void);
	virtual void doPreferences(void);

protected:
	// GL Widget overrides
	void initializeGL(void);
	void resizeGL(int width, int height);
	void swap_Buffers(void);
	void paintGL(void);

	//void updateSpinRateXY(int xPos, int yPos);
	//void updatePanXY(int xPos, int yPos);
	//void updateHeadXY(int xPos, int yPos);
	//void updateZoom(int yPos);
	//void spinButtonPress(QMouseEvent *event);
	//void spinButtonRelease(QMouseEvent *event);
	//void zoomButtonPress(QMouseEvent *event);
	//void zoomButtonRelease(QMouseEvent *event);
	//void updateSpinRate(void);

	// Events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void timerEvent(QTimerEvent *event);
	void paintEvent(QPaintEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	int progressCallback(const QString &message, float progress,
		bool showErrors);
	void setupProgress(void);
	void endLoad(void);
	void updateFPS(void);
	void drawFPS(void);
	void updateLatlong(void);
	void startPaintTimer(void);
	void killPaintTimer(void);
	void startPollTimer(bool immediate=false);
	void killPollTimer(void);
	void startLoadTimer(void);
	void killLoadTimer(void);
	bool verifyLDrawDir(bool forceChoose = false);
	bool verifyLDrawDir(char *value);
	char *getLDrawDir(void);
	bool promptForLDrawDir(QString prompt = QString (""));
	int errorCallback(LDLError *error);
	void clearErrors(void);
	void preLoad(void);
	void postLoad(void);
	void showErrorsIfNeeded(bool onlyIfNeeded);
	void setLastOpenFile(const char *filename);
	bool chDirFromFilename(const char *filename);
	void checkFileForUpdates(void);
	void getFileTime(const char *filename, QDateTime &value);
	void lock(void);
	void unlock(void);
	void windowActivationChange(bool oldActive);
	//void processKey(QKeyEvent *event, bool press);
	void setViewMode(LDInputHandler::ViewMode value, bool examineLatLong, 
                     bool keepRightSideUp, bool saveSettings=true);
	void setMenuItemsEnabled(QPopupMenu *menu, bool enabled);
	void libraryUpdateProgress(TCProgressAlert *alert);
	void setLibraryUpdateProgress(float progress);
	void setupUserAgent(void);
	virtual bool calcSaveFilename(char* saveFilename, int len);
	bool grabImage(int &imageWidth, int &imageHeight,
		bool fromCommandLine = false);
	LDSnapshotTaker::ImageType getSaveImageType(void);

//	static int staticProgressCallback(char *message, float progress,
//		void *userData);
//	static int staticErrorCallback(LDLError *error, void *userData);
	static bool staticFileCaseCallback(char *filename);
	static bool staticFileCaseLevel(QDir &dir, char *filename);
	static TCULong convertKeyModifiers(Qt::KeyboardModifiers osModifiers);
	static LDInputHandler::KeyCode convertKeyCode(int osKey);
	static LDInputHandler::MouseButton convertMouseButton(int button);

	LDrawModelViewer *modelViewer;
	LDInputHandler *inputHandler;
	LDSnapshotTaker *snapshotTaker;
	bool mouseButtonsDown[MAX_MOUSE_BUTTONS];
	int lastX;
	int lastY;
	int originalZoomY;
	TCFloat rotationSpeed;
	LDInputHandler::ViewMode viewMode;
	bool examineLatLong;
	bool keepRightSide;
	int spinButton;
	int zoomButton;
	QTime lastMoveTime;
	QTime lastProgressTime;
	QTime referenceFrameTime;
	Preferences *preferences;
	ExtraDir *extradir;
	SnapshotSettings *snapshotsettings;
	JpegOptions *jpegoptions;
	OpenGLExtensions *extensionsPanel;
	QString openGLDriverInfo;
	LatitudeLongitude *latitudelongitude;
	About *aboutPanel;
	Help *helpContents;
	LDViewMainWindow *mainWindow;
	QStatusBar *statusBar;
	QProgressBar *progressBar;
	QLabel *progressLabel, *progressLatlong, *progressMode;
	bool loading,saving,printing;
	bool cancelLoad;
	QApplication *app;
	bool painting;
	float fps;
	int numFramesSinceReference;
	int paintTimer;
	int pollTimer;
	int loadTimer;
	int libraryUpdateTimer;
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
	int exportType;
	int fullscreen;
	AlertHandler *alertHandler;
#ifndef _NO_BOOST
	LDLibraryUpdater *libraryUpdater;
#endif
	bool libraryUpdateFinished;
	bool libraryUpdateCanceled;
	bool libraryUpdateFinishNotified;
	QString libraryUpdateProgressMessage;
	float libraryUpdateProgressValue;
	bool libraryUpdateProgressReady;
	int libraryUpdateFinishCode;
	QProgressDialog *libraryUpdateWindow;
	bool redrawRequested;
	int saveImageWidth;
	int saveImageHeight;
	const char *saveImageFilename;
	bool saveImageZoomToFit;
	bool saveImageResult;

	int lightingSelection;
	LDPreferences::SaveOp curSaveOp;
};

#endif // __MODELVIEWERWIDGET_H__
