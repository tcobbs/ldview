#ifndef __LDVIEWWINDOW_H__
#define __LDVIEWWINDOW_H__


#include <CUI/CUIWindow.h>

#include <LDLib/LDrawModelViewer.h>

typedef struct
{
	int width;
	int height;
	int depth;
} VideoModeT;

class ModelWindow;
class TCStringArray;
class CUIWindowResizer;
class TCWebClient;

class LDViewWindow: public CUIWindow
{
	public:
		LDViewWindow(char*, HINSTANCE, int, int, int, int);
		virtual char* windowClassName(void);
		virtual void setModelWindow(ModelWindow* value);
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
		virtual LRESULT switchStatusBar(void);

		static char* getLDrawDir(void);
		static char* lastOpenPath(char* pathKey = NULL);
		static void addFileType(char*, char*, char*);
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
		virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
			int newHeight);
		virtual BOOL doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo);
		virtual LRESULT showOpenGLDriverInfo(void);
		virtual HBRUSH getBackgroundBrush(void);
		virtual void createAboutBox(void);
		virtual BOOL showAboutBox(void);
		virtual BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
			HWND controlHWnd);
		virtual void createLDrawDirWindow(void);
		virtual BOOL doLDrawDirOK(HWND);
		virtual BOOL doLDrawDirBrowse(HWND);
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
		virtual void selectFSVideoModeMenuItem(int);
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
		virtual void setMenuEnabled(HMENU hParentMenu, int itemID,
			bool enabled, BOOL byPosition = FALSE);
		virtual bool modelIsLoaded(void);
		virtual void updateModelMenuItems(void);
		virtual void printModel(void);
		virtual void saveSnapshot(void);
		virtual void pageSetup(void);
		virtual void showDefaultMatrix(const char *matrixString,
			const char *title);
		virtual void showViewInfo(void);
		virtual void showRotationMatrix(void);
		virtual void showTransformationMatrix(void);
		virtual void showLDrawCommandLine(void);
		virtual bool modelWindowIsShown(void);
		virtual LRESULT switchToExamineMode(void);
		virtual LRESULT switchToFlythroughMode(void);
		virtual void setMenuRadioCheck(HMENU hParentMenu, UINT uItem,
			bool checked);
		virtual void setMenuCheck(HMENU hParentMenu, UINT uItem, bool checked,
			bool radio = false);
		virtual bool getMenuCheck(HMENU hParentMenu, UINT uItem);
		virtual void chooseExtraDirs(void);
		virtual void chooseNewLDrawDir(void);
		virtual void createStatusBar(void);
		virtual LRESULT switchTopmost(void);
		virtual void removeStatusBar(void);
		virtual void addStatusBar(void);
		virtual void resizeModelWindow(int newWidth, int newHeight);
		virtual void startLoading(void);
		virtual void stopLoading(void);
		virtual void showStatusIcon(bool examineMode);
		virtual void reflectViewMode(void);
		virtual void reflectTopmost(void);
		virtual void reflectStatusBar(void);
		virtual void reflectPolling(void);
		virtual void reflectVideoMode(void);
		virtual int getStatusBarHeight(void);
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
		void downloadTest(void);
		void fetchURLFinish(TCWebClient *webClient);
		void fetchHeaderFinish(TCWebClient *webClient);
		LRESULT doTimer(UINT timerID);

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
		HWND hFrameWindow;
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
		bool topmost;
		BOOL skipMinimize;
		BOOL screenSaver;
		int originalMouseX;
		int originalMouseY;
		HMENU hFileMenu;
		HMENU hViewMenu;
		bool loading;
//		bool modelWindowShown;
		CUIWindowResizer *openGLInfoWindoResizer;
		HWND hOpenGLStatusBar;
		HICON hExamineIcon;
		HICON hFlythroughIcon;

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
