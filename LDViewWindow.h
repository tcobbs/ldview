#ifndef __LDVIEWWINDOW_H__
#define __LDVIEWWINDOW_H__


#include <CUI/CUIWindow.h>

#include <LDLib/LDrawModelViewer.h>
#include <shlobj.h>

typedef struct
{
	int width;
	int height;
	int depth;
} VideoModeT;

class TbButtonInfo: public TCObject
{
public:
	TbButtonInfo(void);
	const char *getTooltipText(void) { return tooltipText; }
	void setTooltipText(const char *value);
	int getCommandId(void) { return commandId; }
	void setCommandId(int value) { commandId = value; }
	int getBmpId(int stdBitmapStartId, int tbBitmapStartId);
	void setStdBmpId(int value) { stdBmpId = value; }
	void setTbBmpId(int value) { tbBmpId = value; }
	BYTE getState(void) { return state; }
	void setState(BYTE value) { state = value; }
	BYTE getStyle(void) { return style; }
	void setStyle(BYTE value) { style = value; }
protected:
	virtual void dealloc(void);

	char *tooltipText;
	int commandId;
	int stdBmpId;
	int tbBmpId;
	BYTE state;
	BYTE style;
};

class ModelWindow;
class TCStringArray;
class TCSortedStringArray;
class CUIWindowResizer;
class LDLibraryUpdater;
class LDViewPreferences;
template <class Type> class TCTypedObjectArray;

typedef TCTypedObjectArray<TbButtonInfo> TbButtonInfoArray;

#define LIBRARY_UPDATE_FINISHED 1
#define LIBRARY_UPDATE_CANCELED 2
#define LIBRARY_UPDATE_NONE 3
#define LIBRARY_UPDATE_ERROR 4

class LDViewWindow: public CUIWindow
{
	public:
		LDViewWindow(const char*, HINSTANCE, int, int, int, int);
		virtual char* windowClassName(void);
//		virtual void setModelWindow(ModelWindow* value);
//		virtual int getDecorationHeight(void);
		virtual void openModel(const char* = NULL, bool skipLoad = false);
		virtual BOOL initWindow(void);
		virtual void setScreenSaver(BOOL flag);
		BOOL getScreenSaver(void) { return screenSaver; }
		virtual void showWindow(int nCmdShow);
		BOOL checkMouseMove(HWND hWnd, LPARAM lParam);
		void initMouseMove(void);
		virtual void shutdown(void);
		virtual bool saveSnapshot(char *saveFilename);
		BOOL getFullScreen(void) { return fullScreen; }
		virtual void setLoading(bool value);
		bool getShowStatusBar(void) { return showStatusBar; }
		bool getTopmost(void) { return topmost; }
		void forceShowStatusBar(bool value);
		void applyPrefs(void);
		virtual LRESULT switchToolbar(void);
		virtual LRESULT switchStatusBar(void);
		virtual const char *getProductVersion(void);
		virtual const char *getLegalCopyright(void);
		virtual void setHParentWindow(HWND hWnd);
		ModelWindow *getModelWindow(void) { return modelWindow; }

		static char* getLDrawDir(void);
		static char* lastOpenPath(char* pathKey = NULL);
		static void addFileType(char*, const char*, char*);
		static void setLastOpenFile(const char* filename, char* pathKey = NULL);
	protected:
		virtual ~LDViewWindow(void);
		static BOOL verifyLDrawDir(bool forceChoose = false);
		static BOOL verifyLDrawDir(char*);
//		static void stripTrailingSlash(char*);
		static BOOL promptForLDrawDir(void);

		virtual void dealloc(void);
		virtual WNDCLASSEX getWindowClass(void);
		virtual LRESULT doMouseWheel(short keyFlags, short zDelta, int xPos,
			int yPos);
		virtual LRESULT doCreate(HWND, LPCREATESTRUCT);
		virtual LRESULT doClose(void);
		virtual LRESULT doDestroy(void);
		virtual LRESULT doChar(TCHAR, LPARAM);
		virtual LRESULT doCommand(int, int, HWND);
		virtual LRESULT doSize(WPARAM, int, int);
		virtual LRESULT doActivateApp(BOOL, DWORD);
		virtual LRESULT doActivate(int, BOOL, HWND);
		virtual LRESULT doEraseBackground(RECT* updateRect);
		virtual LRESULT doMenuSelect(UINT menuID, UINT flags, HMENU hMenu);
		virtual LRESULT doEnterMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doExitMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doShowWindow(BOOL showFlag, LPARAM status);
		virtual LRESULT doKeyDown(int keyCode, long keyData);
		virtual LRESULT doKeyUp(int keyCode, long keyData);
		virtual LRESULT doDrawItem(HWND hControlWnd,
			LPDRAWITEMSTRUCT drawItemStruct);
		virtual LRESULT doNotify(int controlId, LPNMHDR notification);
		virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
			int newHeight);
		virtual BOOL doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo);
		virtual LRESULT showOpenGLDriverInfo(void);
		virtual HBRUSH getBackgroundBrush(void);
		virtual void createAboutBox(void);
		virtual BOOL showAboutBox(void);
		virtual void createLibraryUpdateWindow(void);
		virtual void showLibraryUpdateWindow(void);
		virtual BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
			HWND controlHWnd);
		virtual void doDialogOK(HWND hDlg);
		virtual void doDialogCancel(HWND hDlg);
		virtual void createLDrawDirWindow(void);
		virtual BOOL doLDrawDirOK(HWND);
//		virtual BOOL doLDrawDirBrowse(HWND);
		virtual LRESULT doInitMenuPopup(HMENU hPopupMenu, UINT uPos,
			BOOL fSystemMenu);
		virtual BOOL tryVideoMode(VideoModeT*, int);
		virtual void setFullScreenDisplayMode(void);
		virtual void restoreDisplayMode(void);
		virtual void switchModes(void);
		virtual void checkVideoMode(int, int, int);
		virtual void checkLowResModes(void);
		virtual void getAllDisplayModes(void);
		virtual void populateExtraSearchDirs(void);
		virtual void recordExtraSearchDirs(void);
		virtual void populateExtraDirsListBox(void);
		virtual void populateDisplayModeMenuItems(void);
		virtual void selectFSVideoModeMenuItem(int index,
			bool saveSetting = true);
		virtual HMENU menuForBitDepth(HWND, int, int* = NULL);
		virtual void selectPollingMenuItem(int);
		virtual HMENU getPollingMenu(void);
		virtual VideoModeT* getCurrentVideoMode(void);
		virtual void activateFullScreenMode(void);
		virtual void deactivateFullScreenMode(void);
		virtual void initPollingMenu(void);
		virtual void showHelp(void);
		virtual LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);
		virtual void resetView(LDVAngle viewAngle = LDVAngleDefault);
		virtual void saveDefaultView(void);
		virtual void zoomToFit(void);
		virtual void resetDefaultView(void);
		virtual int getMenuItemIndex(HMENU hMenu, UINT itemID);
		virtual int clearRecentFileMenuItems(void);
		virtual void populateRecentFileMenuItems(void);
		virtual void populateRecentFiles(void);
		virtual void openRecentFile(int index);
		virtual void setMenuItemsEnabled(HMENU hMenu, bool enabled);
		virtual void setMenuEnabled(HMENU hParentMenu, int itemID,
			bool enabled, BOOL byPosition = FALSE);
		virtual bool modelIsLoaded(void);
		virtual void updateModelMenuItems(void);
		virtual void printModel(void);
		virtual void saveSnapshot(void);
		virtual void pageSetup(void);
//		virtual void showDefaultMatrix(const char *matrixString,
//			const char *title);
		virtual void showViewInfo(void);
		virtual void showPovCamera(void);
//		virtual void showRotationMatrix(void);
//		virtual void showTransformationMatrix(void);
		virtual void showLDrawCommandLine(void);
		virtual bool modelWindowIsShown(void);
		virtual LRESULT switchToExamineMode(bool saveSetting = true);
		virtual LRESULT switchToFlythroughMode(bool saveSetting = true);
		virtual void setMenuRadioCheck(HMENU hParentMenu, UINT uItem,
			bool checked);
		virtual void setMenuCheck(HMENU hParentMenu, UINT uItem, bool checked,
			bool radio = false);
		virtual bool getMenuCheck(HMENU hParentMenu, UINT uItem);
		virtual void chooseExtraDirs(void);
		virtual void chooseNewLDrawDir(void);
		virtual void createStatusBar(void);
		virtual void createToolbar(void);
		virtual LRESULT switchTopmost(void);
		virtual LRESULT switchVisualStyle(void);
		virtual void removeToolbar(void);
		virtual void addToolbar(void);
		virtual void removeStatusBar(void);
		virtual void addStatusBar(void);
		virtual void reshapeModelWindow(void);
		virtual void startLoading(void);
		virtual void stopLoading(void);
		virtual void showStatusIcon(bool examineMode);
		virtual void reflectViewMode(bool saveSetting = true);
		virtual void reflectTopmost(void);
		virtual void reflectVisualStyle(void);
		virtual void reflectStatusBar(void);
		virtual void reflectToolbar(void);
		virtual void reflectPolling(void);
		virtual void reflectVideoMode(void);
		virtual void toolbarCheckReflect(bool &value, bool prefsValue,
			LPARAM commandID);
		virtual void toolbarChecksReflect(void);
		virtual int getStatusBarHeight(void);
		virtual int getDockedHeight(void);
		virtual int getToolbarHeight(void);
		virtual int getModelWindowTop(void);
		virtual BOOL doDialogNotify(HWND hDlg, int controlId,
			LPNMHDR notification);
		virtual BOOL doExtraDirsCommand(int controlId, int notifyCode,
			HWND hControlWnd);
		virtual BOOL doLDrawDirCommand(int controlId, int notifyCode,
			HWND hControlWnd);
		virtual BOOL doAddExtraDir(void);
		virtual BOOL doRemoveExtraDir(void);
		virtual BOOL doMoveExtraDirUp(void);
		virtual BOOL doMoveExtraDirDown(void);
		virtual void updateExtraDirsEnabled(void);
		virtual BOOL doExtraDirSelected(void);
		void checkForLibraryUpdates(void);
//		virtual LRESULT doTimer(UINT timerID);
		virtual void doLibraryUpdateFinished(int finishType);
		virtual void readVersionInfo(void);
		virtual void createModelWindow(void);
		virtual void populateTbButtonInfos(void);
		virtual void addTbButtonInfo(const char *tooltipText, int commandId,
			int stdBmpId, int tbBmpId, BYTE style = TBSTYLE_BUTTON,
			BYTE state = TBSTATE_ENABLED);
		virtual void addTbCheckButtonInfo(const char *tooltipText,
			int commandId, int stdBmpId, int tbBmpId, bool checked,
			BYTE style = TBSTYLE_CHECK, BYTE state = TBSTATE_ENABLED);
		virtual void addTbSeparatorInfo(void);
		virtual void doWireframe(void);
		virtual void doSeams(void);
		virtual void doEdges(void);
		virtual void doPrimitiveSubstitution(void);
		virtual void doLighting(void);
		virtual void doBfc(void);
		virtual void doToolbarDropDown(LPNMTOOLBAR toolbarNot);
		virtual void doViewAngle(void);
		virtual void doFog(void);
		virtual void doRemoveHiddenLines(void);
		virtual void doShowEdgesOnly(void);
		virtual void doConditionalLines(void);
		virtual void doHighQualityEdges(void);
		virtual void doAlwaysBlack(void);
		virtual void doTextureStuds(void);
		virtual void doQualityLighting(void);
		virtual void doSubduedLighting(void);
		virtual void doSpecularHighlight(void);
		virtual void doAlternateLighting(void);
		virtual void doRedBackFaces(void);
		virtual void doGreenFrontFaces(void);
		virtual bool doToolbarCheck(bool &value, LPARAM commandId);
		virtual void updateWireframeMenu(void);
		virtual void updateEdgesMenu(void);
		virtual void updatePrimitivesMenu(void);
		virtual void updateLightingMenu(void);
		virtual void updateBFCMenu(void);
		void progressAlertCallback(TCProgressAlert *error);

		void loadSettings(void);

		static void recordRecentFiles(void);
		static int CALLBACK pathBrowserCallback(HWND hwnd, UINT uMsg,
			LPARAM lParam, LPARAM /*lpData*/);

		ModelWindow* modelWindow;
		HWND hAboutWindow;
		HWND hLDrawDirWindow;
		HWND hOpenGLInfoWindow;
		HWND hExtraDirsWindow;
		HWND hExtraDirsToolbar;
		HWND hExtraDirsList;
		HWND hStatusBar;
		HWND hToolbar;
		HWND hDeactivatedTooltip;
		HWND hFrameWindow;
		HWND hLibraryUpdateWindow;
		HWND hUpdateProgressBar;
		HWND hUpdateStatus;
		char* userLDrawDir;
		BOOL fullScreen;
		BOOL fullScreenActive;
		BOOL switchingModes;
		DWORD standardWindowStyle;

		VideoModeT* videoModes;
		int numVideoModes;
		int currentVideoModeIndex;
		int fsWidth;
		int fsHeight;
		int fsDepth;
		bool showStatusBar;
		bool showStatusBarOverride;
		bool showToolbar;
		bool topmost;
		bool visualStyleEnabled;
		BOOL skipMinimize;
		BOOL screenSaver;
		int originalMouseX;
		int originalMouseY;
		HMENU hFileMenu;
		HMENU hViewMenu;
		HMENU hViewAngleMenu;
		HMENU hToolbarMenu;
		HMENU hWireframeMenu;
		HMENU hEdgesMenu;
		HMENU hPrimitivesMenu;
		HMENU hLightingMenu;
		HMENU hBFCMenu;
		bool loading;
//		bool modelWindowShown;
		CUIWindowResizer *openGLInfoWindoResizer;
		HWND hOpenGLStatusBar;
		HICON hExamineIcon;
		HICON hFlythroughIcon;
		LDLibraryUpdater *libraryUpdater;
		char *productVersion;
		char *legalCopyright;
		TbButtonInfoArray *tbButtonInfos;
		int stdBitmapStartId;
		int tbBitmapStartId;
		LDViewPreferences *prefs;
		bool drawWireframe;
		bool seams;
		bool edges;
		bool primitiveSubstitution;
		bool lighting;
		bool bfc;
		LDVAngle lastViewAngle;
		bool libraryUpdateCanceled;

		static TCStringArray* recentFiles;
		static TCStringArray* extraSearchDirs;

		static class LDViewWindowCleanup
		{
		public:
			~LDViewWindowCleanup(void);
		} ldViewWindowCleanup;
		friend class LDViewWindowCleanup;
};

#endif // __LD3VIEWWINDOW_H__
