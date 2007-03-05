#ifndef __MODELWINDOW_H__
#define __MODELWINDOW_H__

#include <CUI/CUIOGLWindow.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCVector.h>
#include <LDLoader/LDLError.h>
#include "LDViewPreferences.h"
#include <Commctrl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

class LDrawModelViewer;
class TCStringArray;
class CUIWindowResizer;

#define POLL_NONE 0
#define POLL_PROMPT 1
#define POLL_AUTO 2
#define POLL_BACKGROUND 3

typedef enum
{
	LDVViewExamine,
	LDVViewFlythrough
} LDVViewMode;

class ErrorInfo: public TCObject
{
public:
	ErrorInfo(void);
	void setType(LDLErrorType type) { m_type = type; }
	LDLErrorType getType(void) { return m_type; }
	void setTypeName(const char *typeName);
	char *getTypeName(void) { return m_typeName; }
protected:
	~ErrorInfo(void);
	virtual void dealloc(void);

	LDLErrorType m_type;
	char *m_typeName;
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
		virtual void setFilename(const char*);
		virtual char* getFilename(void);
		virtual int loadModel(void);
		virtual void setZoomSpeed(TCFloat);
		virtual TCFloat getZoomSpeed(void);
//		void setRotationSpeed(TCFloat value) { rotationSpeed = value; }
		virtual void setRotationSpeed(TCFloat value);
		virtual void setXRotate(TCFloat value);
		virtual void setYRotate(TCFloat value);
		TCFloat getRotationSpeed(void) { return rotationSpeed; }
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
		void setNeedsRecompile(void) { needsRecompile = true; }
		virtual void resetView(LDVAngle viewAngle = LDVAngleDefault);
		virtual void saveDefaultView(void);
		virtual void resetDefaultView(void);
		virtual void finalSetup(void);
		virtual void closeWindow(void);
		virtual bool print(void);
		virtual bool pageSetup(void);
		virtual bool saveSnapshot(void);
		virtual bool saveSnapshot(char *saveFilename,
			bool fromCommandLine = false);
		virtual void setViewMode(LDVViewMode mode, bool saveSetting = true);
		LDVViewMode getViewMode(void) { return viewMode; }
		LDrawModelViewer* getModelViewer(void) { return modelViewer; }
		HWND getHPrefsWindow(void) { return hPrefsWindow; }
		virtual LRESULT processKeyDown(int keyCode, long keyData);
		virtual LRESULT processKeyUp(int keyCode, long keyData);
		void setStatusBar(HWND value) { hStatusBar = value; }
		HWND getStatusBar(void) { return hStatusBar; }
		void setProgressBar(HWND value) { hProgressBar = value; }
		HWND getProgressBar(void) { return hProgressBar; }
		void setCancelLoad(void) { cancelLoad = true; }
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

		void orthoView(void);

		static void initCommonControls(DWORD mask);
		static bool chDirFromFilename(const char* filename, char* outFilename);
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
//		virtual LRESULT doMButtonUp(WPARAM keyFlags, int xPos, int yPos);
		virtual LRESULT doMouseMove(WPARAM keyFlags, int xPos, int yPos);
		virtual LRESULT doSize(WPARAM, int, int);
		virtual LRESULT doMove(int newX, int newY);
		virtual LRESULT doShowWindow(BOOL showFlag, LPARAM status);
		virtual void updateSpinRateXY(int, int);
		virtual void updatePanXY(int xPos, int yPos);
		virtual void updateHeadXY(int xPos, int yPos);
		virtual void updateZoom(int);
		virtual void updateSpinRate(void);
		virtual void captureMouse(void);
		virtual void releaseMouse(void);
//		virtual void createPreferences(void);
//		virtual void setupPreferences(void);
//		virtual BOOL doPreferencesOK(void);
		virtual BOOL doErrorOK(void);
//		virtual BOOL doPreferencesClick(int controlId, HWND controlHWnd);
		virtual BOOL doProgressClick(int controlId, HWND controlHWnd);
		virtual BOOL doErrorClick(int controlId, HWND controlHWnd);
		virtual BOOL doPageSetupClick(int controlId, HWND controlHWnd);
//		virtual void doPreferencesCancel(void);
//		virtual void doPreferencesApply(void);
//		virtual void doPreferencesFSRefresh(void);
//		virtual void doPreferencesStipple(void);
//		virtual void doPreferencesSort(void);
		virtual void createErrorWindow(void);
		virtual void setupErrorWindow(void);
		virtual int populateErrorTree(void);
		virtual void populateErrorList(void);
		virtual void showErrorsIfNeeded(BOOL onlyIfNeeded = TRUE);
		virtual BOOL doErrorTreeNotify(LPNMHDR notification);
		virtual BOOL doErrorWindowNotify(LPNMHDR notification);
		virtual BOOL doErrorTreeKeyDown(LPNMTVKEYDOWN notification);
		virtual BOOL doErrorTreeCopy(void);
		virtual BOOL doDialogInit(HWND hDlg, HWND hFocusWindow, LPARAM lParam);
//		virtual BOOL doDialogVScroll(HWND, int, int, HWND);
		virtual BOOL doDialogNotify(HWND hDlg, int controlId,
			LPNMHDR notification);
		virtual BOOL doDialogHelp(HWND hDlg, LPHELPINFO helpInfo);
		virtual BOOL doDialogCommand(HWND, int, int, HWND);
		virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
			int newHeight);
		virtual BOOL doErrorSize(WPARAM sizeType, int newWidth, int newHeight);
		virtual BOOL doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo);
		virtual LRESULT doEraseBackground(RECT* updateRect);
		virtual LRESULT doTimer(UINT);
		virtual BOOL getFileTime(FILETIME*);
		virtual void checkForPart(void);
#ifndef TC_NO_UNICODE
		virtual int progressCallback(const char* message, float progress,
			bool showErrors = false);
#endif // TC_NO_UNICODE
		virtual int progressCallback(CUCSTR message, float progress,
			bool showErrors = false);
		static int staticProgressCallback(char* message, float progress,
			void* userData);
		static bool staticImageProgressCallback(char* message, float progress,
			void* userData);
//		virtual BOOL showProgress(void);
		virtual void hideProgress(void);
		virtual void createProgress(void);
		virtual void setupProgress(void);
		virtual void doProgressCancel(void);
		virtual int errorCallback(LDLError* error);
//		static int staticErrorCallback(LDMError* error, void* userData);
		virtual LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK staticErrorDlgProc(HWND, UINT, WPARAM, LPARAM);
		virtual LRESULT errorDlgProc(HWND, UINT, WPARAM, LPARAM);
		virtual void registerErrorWindowClass(void);
		virtual bool addError(LDLError* error);
		virtual HTREEITEM addErrorLine(HTREEITEM parent, char* line,
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
		virtual bool writePng(char *filename, int width, int height,
			BYTE *buffer, bool saveAlpha);
		virtual bool writeBmp(char *filename, int width, int height,
			BYTE *buffer);
		virtual bool writeImage(char *filename, int width, int height,
			BYTE *buffer, char *formatName, bool saveAlpha = false);
		virtual BYTE *grabImage(int &imageWidth, int &imageHeight,
			bool zoomToFit, BYTE *buffer = NULL, bool *saveAlpha = NULL);
		virtual bool saveImage(char *filename, int imageWidth, int imageHeight,
			bool zoomToFit);
		virtual void renderOffscreenImage(void);
		virtual bool setupPBuffer(int imageWidth, int imageHeight,
			bool antialias = false);
		virtual void cleanupPBuffer(void);
		virtual void setupSnapshotBackBuffer(int imageWidth, int imageHeight,
			RECT &rect);
		virtual void cleanupSnapshotBackBuffer(RECT &rect);
		virtual bool selectPrinter(PRINTDLG &pd);
		virtual bool printPage(const PRINTDLG &pd);
		virtual HBITMAP createDIBSection(HDC hBitmapDC, int bitmapWidth,
			int bitmapHeight, int hDPI, int vDPI, BYTE **bmBuffer);
		virtual void calcTiling(HDC hPrinterDC, int &bitmapWidth,
			int &bitmapHeight, int &numXTiles, int &numYTiles);
		virtual void calcTiling(int desiredWidth, int desiredHeight,
			int &bitmapWidth, int &bitmapHeight, int &numXTiles,
			int &numYTiles);
		virtual void sizeView(void);
		virtual void setupSaveExtras(void);
		virtual void setupPrintExtras(void);
		virtual void setupPageSetupExtras(void);
		virtual void disableSaveSize(void);
		virtual void enableSaveSize(void);
		virtual void disableSaveSeries(void);
		virtual void enableSaveSeries(void);
		virtual void updateSaveWidth(void);
		virtual void updateSaveHeight(void);
		virtual void updateSaveDigits(void);
		virtual bool getSaveFilename(char* saveFilename, int len);
		virtual bool shouldOverwriteFile(char* filename);
		virtual bool calcSaveFilename(char* saveFilename, int len);
		virtual BOOL doSaveCommand(int controlId, int notifyCode,
			HWND hControlWnd);
		virtual BOOL doSaveNotify(int controlId, LPOFNOTIFY notification);
		virtual BOOL doSaveClick(int controlId, HWND hControlWnd);
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
		virtual bool canSaveAlpha(void);
		virtual void makeCurrent(void);
#ifndef TC_NO_UNICODE
		virtual void setStatusText(HWND hStatus, int part, const char *text);
#endif // TC_NO_UNICODE
		virtual void setStatusText(HWND hStatus, int part, CUCSTR text);
		virtual void initFail(char *reason);
		void ldlErrorCallback(LDLError *error);
		void progressAlertCallback(TCProgressAlert *alert);
		void modelViewerAlertCallback(TCAlert *alert);
		void populateErrorInfos(void);
		BOOL setAllErrorsSelected(bool selected);
		void setupMultisample(void);

		void loadSettings(void);

		virtual void drawLight(GLenum, TCFloat, TCFloat, TCFloat);
		virtual void drawLights(void);
		virtual void setupLight(GLenum);

		static UINT CALLBACK staticPrintHook(HWND hDlg, UINT uiMsg,
			WPARAM wParam, LPARAM lParam);
		static UINT CALLBACK staticPageSetupHook(HWND hDlg, UINT uiMsg,
			WPARAM wParam, LPARAM lParam);
		static UINT CALLBACK staticSaveHook(HWND hDlg, UINT uiMsg,
			WPARAM wParam, LPARAM lParam);
		static int roundUp(int value, int nearest);
		static void swap(int &left, int &right);
		static bool altPressed(void);

		LDrawModelViewer* modelViewer;
		DWORD referenceFrameTime;
		int numFramesSinceReference;
		TCFloat fps;
		bool firstFPSPass;
		DWORD lastMoveTime;
		TCFloat rotationSpeed;
		TCFloat distance;
		int lastX;
		int lastY;
		int originalZoomY;
		bool lButtonDown;
		bool rButtonDown;
		bool mButtonDown;
		HWND oldMouseWindow;
		HWND hPrefsWindow;
		int captureCount;
		int redrawCount;
		int pollSetting;
		FILETIME lastWriteTime;
		bool pollTimerRunning;
		HWND hProgressWindow;
		HWND hProgressCancelButton;
		HWND hProgress;
		HWND hProgressMessage;
		bool cancelLoad;
		DWORD lastProgressUpdate;
		bool loading;
		bool needsRecompile;
		HWND hErrorWindow;
		HWND hErrorStatusWindow;
		HWND hErrorTree;
		HWND hErrorList;
//		HWND hErrorOk;
//		int errorTreeX;
//		int errorTreeY;
//		int errorTreeRight;
//		int errorTreeBottom;
//		int errorOkX;
//		int errorOkY;
//		int errorOkWidth;
//		int errorOkHeight;
//		int errorWindowWidth;
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
		int saveDigits;
		HWND hSaveDialog;
		HWND hSaveWidthLabel;
		HWND hSaveWidth;
		HWND hSaveHeightLabel;
		HWND hSaveHeight;
		HWND hSaveZoomToFitButton;
		HWND hSaveDigitsLabel;
		HWND hSaveDigitsField;
		HWND hSaveDigitsSpin;
		HWND hStatusBar;
		HWND hProgressBar;
		bool usePrinterDPI;
		int printDPI;
		bool printBackground;
		HWND hPrintDPI;
		int currentAntialiasType;
		int saveImageType;
		bool windowShown;
		bool saveAlpha;
		LDVViewMode viewMode;
		HDC hCurrentDC;
		HGLRC hCurrentGLRC;
		CUIWindowResizer *errorWindowResizer;
		bool savingFromCommandLine;
		bool skipErrorUpdates;
};

#endif