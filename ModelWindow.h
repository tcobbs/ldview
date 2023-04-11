#ifndef __MODELWINDOW_H__
#define __MODELWINDOW_H__

#include <CUI/CUIOGLWindow.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCVector.h>
#include <LDLoader/LDLError.h>
#include <LDLib/LDInputHandler.h>
#include <LDLib/LDSnapshotTaker.h>
#include "LDViewPreferences.h"
#include <Commctrl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#if defined(USE_CPP11) || !defined(_NO_BOOST)
#ifdef USE_CPP11
#include <thread>
#include <mutex>
#include <future>
#else // USE_CPP11
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4244 4512)
#endif // WIN32
#include <boost/thread.hpp>
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32
#endif // !_NO_BOOST
#endif // !USE_CPP11


class LDrawModelViewer;
class TCStringArray;
class CUIWindowResizer;
class LDHtmlInventory;

#define POLL_NONE 0
#define POLL_PROMPT 1
#define POLL_AUTO 2
#define POLL_BACKGROUND 3

class ErrorInfo: public TCObject
{
public:
	ErrorInfo(void);
	void setType(LDLErrorType type) { m_type = type; }
	LDLErrorType getType(void) { return m_type; }
	void setTypeName(CUCSTR typeName);
	UCSTR getTypeName(void) const { return m_typeName; }
protected:
	~ErrorInfo(void);
	virtual void dealloc(void);

	LDLErrorType m_type;
	UCSTR m_typeName;
};

class ControlInfo: public TCObject
{
public:
	ControlInfo(void);
//	HWND handle;
//	int controlId;
//	RECT originalRect;
	LDLErrorType errorType;
};

typedef TCTypedObjectArray<LDLError> LDLErrorArray;
typedef TCTypedObjectArray<ControlInfo> ControlInfoArray;
typedef TCTypedObjectArray<ErrorInfo> ErrorInfoArray;

class ModelWindow: public CUIOGLWindow
{
public:
	ModelWindow(CUIWindow*, int, int, int, int);
	virtual void update(void);
	virtual void perspectiveView(void);
	virtual BOOL showPreferences(void);
	virtual void showErrors(void);
	virtual void createErrorWindow(void);
	virtual bool isErrorWindowVisible(void) const;
	virtual int getErrorCount(void) const { return errorCount; }
	virtual int getWarningCount(void) const { return warningCount; }
	virtual void setFilename(const char*);
	virtual char* getFilename(void);
	virtual int loadModel(void);
	virtual void mouseWheel(short keyFlags, short zDelta);
	virtual void setZoomSpeed(TCFloat);
	virtual TCFloat getZoomSpeed(void);
	//virtual void setRotationSpeed(TCFloat value);
	//virtual void setXRotate(TCFloat value);
	//virtual void setYRotate(TCFloat value);
	//TCFloat getRotationSpeed(void) { return rotationSpeed; }
	virtual void forceRedraw(int = 0);
	virtual void recompile(void);
	virtual void uncompile(void);
	virtual void openGlWillEnd(void);
	virtual void zoom(TCFloat);
	virtual void setClipZoom(bool);
	virtual bool getClipZoom(void);
	virtual void reload(void);
	virtual void startPolling(void);
	virtual BOOL stopPolling(void);
	virtual void checkFileForUpdates(void);
	virtual void updateFSAA(void);
	virtual void setPollSetting(int);
	int getPollSetting(void) { return pollSetting; }
	BOOL initWindow(void);
	//void setNeedsRecompile(void) { needsRecompile = true; }
	virtual void resetView(LDVAngle viewAngle = LDVAngleDefault);
	virtual void saveDefaultView(void);
	virtual void resetDefaultView(void);
	virtual void finalSetup(void);
	virtual void closeWindow(void);
	virtual bool print(void);
	virtual bool pageSetup(void);
	virtual bool saveSnapshot(void);
	void exportModel(void);
	virtual bool saveSnapshot(UCSTR saveFilename,
		bool fromCommandLine = false, bool notReallyCommandLine = false);
	virtual void setViewMode(LDInputHandler::ViewMode mode,
		bool examineLatLong, bool saveSetting = true);
	void setKeepRightSideUp(bool value, bool saveSetting = true);
	LDInputHandler::ViewMode getViewMode(void) { return viewMode; }
	LDrawModelViewer* getModelViewer(void) { return modelViewer; }
	HWND getHPrefsWindow(void) { return hPrefsWindow; }
	virtual LRESULT processKeyDown(int keyCode, LPARAM keyData);
	virtual LRESULT processKeyUp(int keyCode, LPARAM keyData);
	void setStatusBar(HWND value) { hStatusBar = value; }
	HWND getStatusBar(void) { return hStatusBar; }
	void setProgressBar(HWND value) { hProgressBar = value; }
	HWND getProgressBar(void) { return hProgressBar; }
	void setCancelLoad(void) { cancelLoad = true; }
	bool getLoadCanceled(void) { return loadCanceled; }
	bool performHotKey(int hotKeyIndex);
	LDViewPreferences *getPrefs(void) { return prefs; }
	void setSaveZoomToFit(bool value, bool commit = false);
	bool getSaveZoomToFit(void) const { return saveZoomToFit; }
	void setSaveActualSize(bool value) { saveActualSize = value; }
	bool getSaveActualSize(void) const { return saveActualSize; }
	void setSaveWidth(int value) { saveWidth = value; }
	int getSaveWidth(void) const { return saveWidth; }
	void setSaveHeight(int value) { saveHeight = value; }
	int getSaveHeight(void) const { return saveHeight; }
	virtual bool setupBitmapRender(int imageWidth, int imageHeight);
	virtual bool setupOffscreen(int imageWidth, int imageHeight,
		bool antialias = false);
	virtual bool setupPBuffer(int imageWidth, int imageHeight,
		bool antialias = false);
	virtual void cleanupOffscreen(void);
	void cleanupRenderSettings(void);
	virtual void cleanupPBuffer(void);
	virtual void cleanupBitmapRender(void);
	virtual void stopAnimation(void);

	void orthoView(void);
	void boundingBoxToggled(void);

	static void initCommonControls(DWORD mask);
	static bool chDirFromFilename(CUCSTR filename, UCSTR outFilename);
	static int roundUp(int value, int nearest);
	static const char *alertClass(void) { return "ModelWindowAlert"; }
	virtual std::string getSaveDir(LDPreferences::SaveOp saveOp);
	void updateModelViewerSize();
protected:
	virtual ~ModelWindow(void);
	virtual void dealloc(void);

	virtual BOOL setupPFD(void);
	virtual LRESULT doCreate(HWND, LPCREATESTRUCT);
	virtual void doPaint(void);
	virtual void doPostPaint(void);
	virtual LRESULT doNCDestroy(void);
	virtual LRESULT doDestroy(void);
	virtual void drawFPS(void);
	virtual void updateFPS(void);
	virtual void printSystemError(void);
	virtual void setupLighting(void);
	virtual void setupMaterial(void);
	virtual LRESULT doLButtonDown(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doLButtonUp(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doRButtonDown(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doRButtonUp(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doMButtonDown(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doMButtonUp(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doMouseMove(WPARAM keyFlags, int xPos, int yPos);
	virtual LRESULT doCaptureChanged(HWND hNewWnd);
	virtual LRESULT doSize(WPARAM, int, int);
	virtual LRESULT doMove(int newX, int newY);
	virtual LRESULT doShowWindow(BOOL showFlag, LPARAM status);
	//virtual void updateSpinRate(void);
	//virtual void updateSpinRateXY(int, int);
	//virtual void updatePanXY(int xPos, int yPos);
	//virtual void updateHeadXY(int xPos, int yPos);
	//virtual void updateZoom(int);
	virtual void captureMouse(void);
	virtual void releaseMouse(void);
	//virtual void createPreferences(void);
	//virtual void setupPreferences(void);
	//virtual BOOL doPreferencesOK(void);
	virtual BOOL doErrorOK(void);
	//virtual BOOL doPreferencesClick(int controlId, HWND controlHWnd);
	virtual BOOL doProgressClick(int controlId, HWND controlHWnd);
	virtual BOOL doErrorClick(int controlId, HWND controlHWnd);
	virtual BOOL doPageSetupClick(int controlId, HWND controlHWnd);
	//virtual void doPreferencesCancel(void);
	//virtual void doPreferencesApply(void);
	//virtual void doPreferencesFSRefresh(void);
	//virtual void doPreferencesStipple(void);
	//virtual void doPreferencesSort(void);
	virtual void setupErrorWindow(void);
	virtual int populateErrorTree(void);
	virtual void populateErrorList(void);
	virtual void showErrorsIfNeeded(BOOL onlyIfNeeded = TRUE);
	virtual BOOL doErrorTreeNotify(LPNMHDR notification);
	virtual BOOL doErrorWindowNotify(LPNMHDR notification);
	virtual BOOL doErrorTreeKeyDown(LPNMTVKEYDOWN notification);
	virtual BOOL doErrorTreeSelChanged(LPNMTREEVIEW notification);
	virtual BOOL doErrorTreeCopy(void);
	virtual BOOL doDialogInit(HWND hDlg, HWND hFocusWindow, LPARAM lParam);
	//virtual BOOL doDialogVScroll(HWND, int, int, HWND);
	virtual BOOL doDialogNotify(HWND hDlg, int controlId,
		LPNMHDR notification);
	virtual BOOL doDialogHelp(HWND hDlg, LPHELPINFO helpInfo);
	virtual BOOL doDialogCommand(HWND, int, int, HWND);
	virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
		int newHeight);
	virtual BOOL doSaveInitDone(OFNOTIFY *ofNotify);
	virtual BOOL doErrorSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual BOOL doSaveSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual BOOL doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo);
	virtual LRESULT doEraseBackground(RECT* updateRect);
	virtual LRESULT doTimer(UINT_PTR);
	virtual bool getFileInfo(FILETIME*, DWORD*, DWORD*);
	virtual bool getFileTime(FILETIME*);
	virtual void checkForPart(void);
	LDSnapshotTaker::ImageType getSaveImageType(void);
	virtual int progressCallback(CUCSTR message, float progress, bool fromImage,
		bool showErrors = false);
	//static bool staticImageProgressCallback(CUCSTR message, float progress,
	//	void* userData);
	virtual void hideProgress(void);
	virtual void createProgress(void);
	virtual void setupProgress(void);
	virtual void doProgressCancel(void);
	virtual int errorCallback(LDLError* error);
	virtual LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK staticErrorDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT errorDlgProc(HWND, UINT, WPARAM, LPARAM);
	virtual void registerErrorWindowClass(void);
	virtual bool addError(LDLError* error);
	virtual HTREEITEM addErrorLine(HTREEITEM parent, CUCSTR line,
		LDLError* error, int imageIndex = -1);
	virtual void clearErrors(void);
	virtual void clearErrorTree(void);
	virtual void getTreeViewLine(HWND hTreeView, HTREEITEM hItem,
		TCStringArray *lines);
	virtual char* getErrorKey(LDLErrorType errorType);
	virtual bool showsError(LDLError *error);
	virtual BOOL showsErrorType(LDLErrorType errorType);
	virtual void swapBuffers(void);
	virtual LRESULT doCommand(int itemId, int notifyCode, HWND controlHWnd);
	//virtual bool writePng(char *filename, int width, int height,
	//	BYTE *buffer, bool saveAlpha);
	//virtual bool writeBmp(char *filename, int width, int height,
	//	BYTE *buffer);
	//virtual bool writeImage(char *filename, int width, int height,
	//	BYTE *buffer, char *formatName, bool saveAlpha = false);
	//virtual BYTE *grabImage(int &imageWidth, int &imageHeight,
	//	bool zoomToFit, BYTE *buffer = NULL, bool *saveAlpha = NULL);
	virtual void grabSetup(int &imageWidth, int &imageHeight,
		RECT &origRect, bool &origSlowClear);
	virtual void grabCleanup(RECT origRect, bool origSlowClear);
	virtual bool saveImage(UCSTR filename, int imageWidth, int imageHeight,
		bool zoomToFit);
	virtual void renderOffscreenImage(void);
	virtual void setupSnapshotBackBuffer(int imageWidth, int imageHeight,
		RECT &rect);
	virtual void cleanupSnapshotBackBuffer(RECT &rect);
	virtual bool selectPrinter(PRINTDLG &pd);
	virtual bool printPage(const PRINTDLG &pd);
	virtual void calcTiling(HDC hPrinterDC, int &bitmapWidth,
		int &bitmapHeight, int &numXTiles, int &numYTiles);
	virtual void calcTiling(int desiredWidth, int desiredHeight,
		int &bitmapWidth, int &bitmapHeight, int &numXTiles,
		int &numYTiles);
	virtual void sizeView(void);
	virtual void setupSaveExtras(void);
	virtual void saveImageTypeUpdated(void);
	virtual void saveExportTypeUpdated(void);
	virtual void saveTypeUpdated(void);
	virtual void setupPrintExtras(void);
	virtual void setupPageSetupExtras(void);
	virtual void updateSaveSizeEnabled(void);
	virtual void disableSaveSize(void);
	virtual void enableSaveSize(BOOL enable = TRUE);
	virtual void updateSaveSeriesEnabled(void);
	virtual void disableSaveSeries(void);
	virtual void enableSaveSeries(BOOL enable = TRUE);
	virtual void updateSaveAllStepsEnabled(void);
	virtual void disableSaveAllSteps(void);
	virtual void enableSaveAllSteps(BOOL enable = TRUE);
	virtual void updateSaveWidth(void);
	virtual void updateSaveHeight(void);
	virtual void updateSaveDigits(void);
	virtual void updateStepSuffix(void);
	virtual std::string getSaveDir(void);
	static std::string defaultDir(LDPreferences::DefaultDirMode mode,
		const char *lastPath, const char *defaultPath);
	virtual void fillSnapshotFileTypes(UCSTR fileTypes);
	virtual void fillExportFileTypes(UCSTR fileTypes);
	virtual bool getSaveFilename(UCSTR saveFilename, int len);
	virtual bool calcSaveFilename(UCSTR saveFilename, int len);
	virtual void updateSaveFilename(void);
	CUCSTR saveExtension(int type = -1) const;
	virtual BOOL doSaveCommand(int controlId, int notifyCode,
		HWND hControlWnd);
	virtual BOOL doSaveNotify(int controlId, LPOFNOTIFY notification);
	virtual BOOL doSaveClick(int controlId, HWND hControlWnd);
	virtual void doSaveOptionsClick(void);
	virtual BOOL doSaveKillFocus(int controlId, HWND hControlWnd);
	virtual BOOL doPrintCommand(int controlId, int notifyCode,
		HWND hControlWnd);
	virtual BOOL doPrintClick(int controlId, HWND hControlWnd);
	virtual BOOL doPrintKillFocus(int controlId, HWND hControlWnd);
	virtual LRESULT doDropFiles(HDROP hDrop);
	virtual void updatePrintDPIField(void);
	virtual void updatePrintDPI(void);
	virtual bool parseDevMode(HGLOBAL hDevMode);
	virtual void applyPrefs(void);
	virtual bool frontBufferFPS(void);
	//virtual bool canSaveAlpha(void);
	virtual void makeCurrent(void);
	virtual void processModalMessage(MSG msg);
	virtual void setStatusText(HWND hStatus, int part, CUCSTR text,
		bool redraw = false);
	virtual void initFail(char *reason);
	void ldlErrorCallback(LDLError *error);
	void progressAlertCallback(TCProgressAlert *alert);
	void modelViewerAlertCallback(TCAlert *alert);
	void redrawAlertCallback(TCAlert *alert);
	void loadAlertCallback(TCAlert *alert);
	void captureAlertCallback(TCAlert *alert);
	void releaseAlertCallback(TCAlert *alert);
	void snapshotTakerAlertCallback(TCAlert *alert);
	void populateErrorInfos(void);
	BOOL setAllErrorsSelected(bool selected);
	void setupMultisample(void);

	void loadSettings(void);
	void loadPrintSettings(void);
	void loadSaveSettings(void);

	virtual void drawLight(GLenum, TCFloat, TCFloat, TCFloat);
	virtual void drawLights(void);
	virtual void setupLight(GLenum);
	void positionSaveAddOn(void);
	void positionSaveOptionsButton(void);

#if defined(USE_CPP11) || !defined(_NO_BOOST)
	enum RemoteCommands
	{
		RCUnknown,
		RCHighlightLine,
		RCGetVersion,
	};
	RemoteCommands parseRemoteMessage(const char *message, std::string &command,
		std::string &data);
	void launchRemoteListener(void);
	void shutDownRemoteListener(void);
	void listenerProc(void);
	void listenerProcInner(void);
	void remoteProc(HANDLE hPipe);
	void processRemoteMessage(char *message);
	void highlightLines(const std::string &paths);
	void sendVersionResponse(HANDLE hPipe);
	void sendResponseMessage(HANDLE hPipe, const char *message);
#endif // _NO_BOOST

	static UINT_PTR CALLBACK staticPrintHook(HWND hDlg, UINT uiMsg,
		WPARAM wParam, LPARAM lParam);
	static UINT_PTR CALLBACK staticPageSetupHook(HWND hDlg, UINT uiMsg,
		WPARAM wParam, LPARAM lParam);
	static UINT_PTR CALLBACK staticSaveHook(HWND hDlg, UINT uiMsg,
		WPARAM wParam, LPARAM lParam);
	static void swap(int &left, int &right);
	static bool altPressed(void);
	static TCULong convertKeyModifiers(TCULong osModifiers);
	static TCULong getCurrentKeyModifiers(void);
	static LDInputHandler::KeyCode convertKeyCode(TCULong osKeyCode);

	LDrawModelViewer* modelViewer;
	LDInputHandler *inputHandler;
	LDSnapshotTaker *snapshotTaker;
	DWORD referenceFrameTime;
	int numFramesSinceReference;
	TCFloat fps;
	bool firstFPSPass;
	//DWORD lastMoveTime;
	//TCFloat rotationSpeed;
	//TCFloat distance;
	//int lastX;
	//int lastY;
	//int originalZoomY;
	//bool lButtonDown;
	//bool rButtonDown;
	//bool mButtonDown;
	HWND oldMouseWindow;
	HWND hPrefsWindow;
	int captureCount;
	int redrawCount;
	int pollSetting;
	FILETIME lastWriteTime;
	DWORD lastFileSizeHigh;
	DWORD lastFileSizeLow;
	bool pollTimerRunning;
	HWND hProgressWindow;
	HWND hProgressCancelButton;
	HWND hProgress;
	HWND hProgressMessage;
	bool cancelLoad;
	bool loadCanceled;
	ULONGLONG lastProgressUpdate;
	bool loading;
	bool needsRecompile;
	HWND hErrorWindow;
	HWND hCopyErrorButton;
	HWND hErrorStatusWindow;
	HWND hErrorTree;
	HWND hErrorList;
	//HWND hErrorOk;
	//int errorTreeX;
	//int errorTreeY;
	//int errorTreeRight;
	//int errorTreeBottom;
	//int errorOkX;
	//int errorOkY;
	//int errorOkWidth;
	//int errorOkHeight;
	//int errorWindowWidth;
	WNDPROC originalErrorDlgProc;
	LDLErrorArray* errors;
	int errorImageIndices[LDLELastError + 2];
	bool errorTreePopulated;
	ErrorInfoArray *errorInfos;
	LDViewPreferences *prefs;
	bool applyingPrefs;
	bool offscreenActive;
	HPBUFFERARB hPBuffer;
	HDC hPBufferDC;
	HGLRC hPBufferGLRC;
	HDC hBitmapRenderDC;
	HGLRC hBitmapRenderGLRC;
	HBITMAP hRenderBitmap;
	HWND hPrintDialog;
	HWND hPageSetupDialog;
	float printLeftMargin;
	float printRightMargin;
	float printTopMargin;
	float printBottomMargin;
	int printOrientation;
	int printPaperSize;
	int printHDPI;
	int printVDPI;
	int printLMarginPixels;
	int printTMarginPixels;
	bool saveActualSize;
	int saveWidth;
	int saveHeight;
	bool saveZoomToFit;
	bool saveSeries;
	bool ignorePBuffer;
	bool ignoreFBO;
	bool ignorePixelFormat;
	int saveDigits;
	HWND hSaveDialog;
	HWND hSaveOptionsButton;
	HWND hSaveTransBgButton;
	HWND hSaveWidthLabel;
	HWND hSaveWidth;
	HWND hSaveHeightLabel;
	HWND hSaveHeight;
	HWND hSaveZoomToFitButton;
	HWND hSaveDigitsLabel;
	HWND hSaveDigitsField;
	HWND hSaveDigitsSpin;
	HWND hSaveStepSuffixLabel;
	HWND hSaveStepSuffixField;
	HWND hSaveStepsSameScaleButton;
	HWND hStatusBar;
	HWND hProgressBar;
	bool usePrinterDPI;
	int printDPI;
	bool printBackground;
	HWND hPrintDPI;
	int currentAntialiasType;
	int saveImageType;
	int saveType;
	int saveExportType;
	bool windowShown;
	bool saveAlpha;
	bool autoCrop;
	bool saveAllSteps;
	UCSTR saveStepSuffix;
	bool saveStepsSameScale;
	LDInputHandler::ViewMode viewMode;
	HDC hCurrentDC;
	HGLRC hCurrentGLRC;
	CUIWindowResizer *errorWindowResizer;
	CUIWindowResizer *saveWindowResizer;
	bool savingFromCommandLine;
	bool skipErrorUpdates;
	bool redrawRequested;
	bool releasingMouse;
	bool userLoad;
	int errorCount;
	int warningCount;
	LDPreferences::SaveOp curSaveOp;
#if defined(USE_CPP11) || !defined(_NO_BOOST)
	bool remoteListener;
#ifdef USE_CPP11
	std::thread *listenerThread;
	std::mutex mutex;
	std::promise<bool> listenerPromise;
	std::future<bool> listenerFuture;
#else
	boost::thread *listenerThread;
	boost::mutex mutex;
#endif
	UINT remoteMessageID;
	bool exiting;
	std::map<std::string, RemoteCommands> remoteCommandMap;
	std::string ldviewVersion;
#endif // _NO_BOOST
};

#endif