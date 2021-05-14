#ifndef __LDVIEWWINDOW_H__
#define __LDVIEWWINDOW_H__


#include <CUI/CUIWindow.h>

#include <LDLib/LDrawModelViewer.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <shlobj.h>
#pragma warning(pop)
//#include "TbButtonInfo.h"

class LDHtmlInventory;
class ModelTreeDialog;
class BoundingBoxDialog;
class MpdDialog;
class ToolbarStrip;

typedef struct
{
	int width;
	int height;
	int depth;
} VideoModeT;


class ModelWindow;
class TCStringArray;
class TCSortedStringArray;
class CUIWindowResizer;
class LDViewPreferences;

#if defined(USE_CPP11) || !defined(_NO_BOOST)
class LDLibraryUpdater;
#endif // USE_CPP11 || !_NO_BOOST

template <class Type> class TCTypedObjectArray;

#define LIBRARY_UPDATE_FINISHED 1
#define LIBRARY_UPDATE_CANCELED 2
#define LIBRARY_UPDATE_NONE 3
#define LIBRARY_UPDATE_ERROR 4

class LDViewWindow: public CUIWindow
{
	public:
		LDViewWindow(CUCSTR, HINSTANCE, int, int, int, int);
		virtual const UCCHAR* windowClassName(void);
		virtual void openModel(CUCSTR = NULL, bool skipLoad = false);
		virtual BOOL initWindow(void);
		virtual void setScreenSaver(bool flag);
		BOOL getScreenSaver(void) { return screenSaver; }
		virtual void showWindow(int nCmdShow);
		BOOL checkMouseMove(HWND hWnd, LPARAM lParam);
		void initMouseMove(void);
		virtual void shutdown(void);
		virtual bool saveSnapshot(UCSTR saveFilename);
		BOOL getFullScreen(void) { return fullScreen; }
		virtual void setLoading(bool value);
		bool getShowStatusBar(void) { return showStatusBar; }
		bool getTopmost(void) { return topmost; }
		void forceShowStatusBar(bool value);
		void applyPrefs(void);
		virtual LRESULT switchToolbar(void);
		virtual LRESULT switchStatusBar(void);
		virtual LRESULT switchKeepRightSideUp(void);
		static const std::string getAppVersion(void);
		static const std::string getAppAsciiCopyright(void);
		static const UCCHAR *getProductVersion(void);
		static const UCCHAR *getLegalCopyright(void);
		virtual void setHParentWindow(HWND hWnd);
		ModelWindow *getModelWindow(void) { return modelWindow; }
		void redrawStatusBar(void);
		void updateStatusParts(void);
		bool inExamineMode(void);
		LDrawModelViewer::ViewMode getViewMode(void);
		bool inLatLonMode(void);
		void showStatusLatLon(void);
		bool getLoading(void) const { return loading; }
		HMENU getViewAngleMenu(void) { return hViewAngleMenu; }
		void changeStep(int action);
		LRESULT doGotoStep(void);
		int setStep(int newStep);
		virtual void setStatusText(HWND hStatus, TCByte part, CUCSTR text,
			bool redraw = false);
		virtual void stopAnimation(void);
		virtual void switchModes(void);
		virtual void zoomToFit(void);
		void rightSideUp(void);
		LRESULT cameraLocation(void);
		virtual LRESULT toggleBoundingBox(void);
		bool isBoundingBoxVisible(void);
		void boundingBoxToggled(void);
		bool isTopmost(void);
		virtual LRESULT switchTopmost(void);
		bool isVisualStyleEnabled(void) { return visualStyleEnabled; }

		static std::string getLDrawDir(void);
		static ucstring getLDrawDirUC(void);
		std::string lastOpenPath(char* pathKey = NULL);
		ucstring lastOpenPathUC(char* pathKey = NULL);
		static void setLastOpenFile(const char* filename, char* pathKey = NULL);
		static ucstring browseForDir(CUCSTR prompt, CUCSTR initialDir);
		static std::string getFloatUdKey(const char* udKey);
protected:
		virtual ~LDViewWindow(void);
		static BOOL verifyLDrawDir(const char*);
		static BOOL promptForLDrawDir(CUCSTR prompt = NULL);

		virtual BOOL verifyLDrawDir(bool forceChoose = false);
		virtual void dealloc(void);
		virtual WNDCLASSEX getWindowClass(void);
		virtual bool handleDpiChange(void);
		virtual LRESULT doMouseWheel(short keyFlags, short zDelta, int xPos,
			int yPos);
		virtual LRESULT doCreate(HWND, LPCREATESTRUCT);
		virtual LRESULT doClose(void);
		virtual LRESULT doDestroy(void);
		virtual LRESULT doChar(UCCHAR, LPARAM);
		virtual LRESULT doCommand(int, int, HWND);
		virtual LRESULT doSize(WPARAM, int, int);
		virtual LRESULT doActivateApp(BOOL, DWORD);
		virtual LRESULT doActivate(int, BOOL, HWND);
		virtual LRESULT doEraseBackground(RECT* updateRect);
		virtual LRESULT doMenuSelect(UINT menuID, UINT flags, HMENU hMenu);
		virtual LRESULT doEnterMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doExitMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doShowWindow(BOOL showFlag, LPARAM status);
		virtual LRESULT doKeyDown(int keyCode, LPARAM keyData);
		virtual LRESULT doKeyUp(int keyCode, LPARAM keyData);
		virtual LRESULT doDrawItem(HWND hControlWnd,
			LPDRAWITEMSTRUCT drawItemStruct);
		virtual LRESULT doMove(int newX, int newY);
		virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
			int newHeight);
		virtual BOOL doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo);
		virtual LRESULT showOpenGLDriverInfo(void);
		virtual HBRUSH getBackgroundBrush(void);
		virtual void createAboutBox(void);
		virtual BOOL showAboutBox(void);
		virtual LRESULT specifyLatLon(void);
#if defined(USE_CPP11) || !defined(_NO_BOOST)
		virtual void createLibraryUpdateWindow(void);
		virtual void showLibraryUpdateWindow(bool initialInstall);
		void checkForLibraryUpdates();
		virtual void doLibraryUpdateFinished(int finishType);
		bool installLDraw();
#endif // _NO_BOOST
		virtual BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
			HWND controlHWnd);
		virtual void doDialogOK(HWND hDlg);
		virtual void doDialogCancel(HWND hDlg);
		virtual void createLDrawDirWindow(void);
		virtual BOOL doLDrawDirOK(HWND);
//		virtual BOOL doLDrawDirBrowse(HWND);
		virtual LRESULT doInitMenuPopup(HMENU hPopupMenu, UINT uPos,
			BOOL fSystemMenu);
		virtual bool tryVideoMode(VideoModeT*, int);
		ucstring getDisplayName(void);
		LONG changeDisplaySettings(DEVMODE *deviceMode, DWORD flags);
		virtual void setFullScreenDisplayMode(void);
		virtual void restoreDisplayMode(void);
		virtual void checkVideoMode(int, int, int);
		virtual void checkLowResModes(void);
		virtual void getAllDisplayModes(void);
		virtual void populateExtraSearchDirs(void);
		virtual void recordExtraSearchDirs(void);
		virtual void populateExtraDirsListBox(void);
		virtual void populateDisplayModeMenuItems(void);
		virtual void selectFSVideoModeMenuItem(int index,
			bool saveSetting = true);
		virtual void selectStandardSize(int index);
		virtual HMENU menuForBitDepth(HWND, int, int* = NULL);
		virtual void selectPollingMenuItem(int);
		virtual HMENU getParentOfMenuItem(HMENU hParentMenu, int itemId);
		virtual HMENU getPollingMenu(void);
		virtual VideoModeT* getCurrentVideoMode(void);
		virtual void activateFullScreenMode(void);
		virtual void deactivateFullScreenMode(void);
		virtual void initPollingMenu(void);
		virtual void showHelp(void);
		virtual void shellExecute(CUCSTR filename);
		virtual LRESULT windowProc(HWND, UINT, WPARAM, LPARAM);
		virtual void resetView(LDVAngle viewAngle = LDVAngleDefault);
		virtual void saveDefaultView(void);
		virtual void resetDefaultView(void);
		virtual int getMenuItemIndex(HMENU hMenu, UINT itemID);
		virtual int clearRecentFileMenuItems(void);
		virtual void populateRecentFileMenuItems(void);
		virtual void populateRecentFiles(void);
		virtual void openRecentFile(int index);
		//virtual void setMenuItemsEnabled(HMENU hMenu, bool enabled);
		virtual void setMenuEnabled(HMENU hParentMenu, int itemID,
			bool enabled, BOOL byPosition = FALSE);
		virtual bool modelIsLoaded(void);
		virtual void updateModelMenuItems(void);
		virtual void updateStepMenuItems(void);
		virtual void printModel(void);
		virtual void saveSnapshot(void);
		virtual void exportModel(void);
		virtual void pageSetup(void);
//		virtual void showDefaultMatrix(const char *matrixString,
//			const char *title);
		virtual void showViewInfo(void);
		virtual void showPovCamera(void);
//		virtual void showRotationMatrix(void);
//		virtual void showTransformationMatrix(void);
		virtual void showLDrawCommandLine(void);
		virtual bool modelWindowIsShown(void);
		LRESULT switchPovCameraAspect(bool saveSetting = true);
		virtual LRESULT switchExamineLatLong(void);
		virtual LRESULT switchToViewMode(LDrawModelViewer::ViewMode viewMode,
			bool saveSetting = true);
		//virtual void setMenuRadioCheck(HMENU hParentMenu, UINT uItem,
		//	bool checked);
		//virtual void setMenuCheck(HMENU hParentMenu, UINT uItem, bool checked,
		//	bool radio = false);
		//virtual bool getMenuCheck(HMENU hParentMenu, UINT uItem);
		virtual void chooseExtraDirs(void);
		virtual void chooseNewLDrawDir(void);
		virtual void createStatusBar(void);
		virtual void createToolbar(void);
		virtual LRESULT switchVisualStyle(void);
		virtual void removeToolbar(void);
		virtual void addToolbar(void);
		virtual void removeStatusBar(void);
		virtual void addStatusBar(void);
		virtual void reshapeModelWindow(void);
		virtual void startLoading(void);
		virtual void stopLoading(void);
		virtual void showStatusIcon(LDrawModelViewer::ViewMode viewMode,
			bool redraw = true);
		virtual void reflectViewMode(bool saveSetting = true);
		virtual void reflectPovCameraAspect(bool saveSetting = true);
		virtual void reflectTopmost(void);
		virtual void reflectVisualStyle(void);
		virtual void reflectStatusBar(void);
		virtual void reflectToolbar(void);
		virtual void reflectPolling(void);
		virtual void reflectVideoMode(void);
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
		static void readVersionInfo(void);
		virtual void createModelWindow(void);
		virtual LRESULT generatePartsList(void);
		virtual LRESULT showModelTree(void);
		virtual LRESULT showMpd(void);
		virtual void generatePartsList(LDHtmlInventory *htmlInventory,
			LDPartsList *partsList, CUCSTR filename);
		void progressAlertCallback(TCProgressAlert *alert);
		void updateWindowMonitor(void);
		void destroyStatusBarIcons(void);
		void loadStatusBarIcons(void);
		void registerOpenGLInfoWindowClass(void);

		int getSavedPixelSize(const char* udKey, int defaultSize);
		void savePixelSize(const char* udKey, int size);
		int getSavedWindowWidth(int defaultValue = -1);
		int getSavedWindowHeight(int defaultValue = -1);

		void loadSettings(void);
		RECT getWorkArea(void);
		void setupStandardSizes(void);

		int intRound(TCFloat value);
		static void recordRecentFiles(void);
		static int CALLBACK pathBrowserCallback(HWND hwnd, UINT uMsg,
			LPARAM lParam, LPARAM /*lpData*/);

		ModelWindow* modelWindow;
		ToolbarStrip *toolbarStrip;
		HWND hAboutWindow;
		HWND hLDrawDirWindow;
		HWND hOpenGLInfoWindow;
		HWND hExtraDirsWindow;
		HWND hExtraDirsToolbar;
		HWND hExtraDirsList;
		HIMAGELIST hExtraDirsImageList;
		HWND hStatusBar;
		HWND hFrameWindow;
		HWND hUpdateProgressBar;
		HWND hUpdateStatus;
		HWND hUpdateCancelButton;
		HWND hUpdateOkButton;
		bool fullScreen;
		bool fullScreenActive;
		bool switchingModes;
		bool searchDirsInitialized;
		DWORD standardWindowStyle;
		SIZE maxStandardSize;

		VideoModeT* videoModes;
		int numVideoModes;
		int currentVideoModeIndex;
		int fsWidth;
		int fsHeight;
		int fsDepth;
		bool showStatusBar;
		bool showStatusBarOverride;
		bool showToolbar;
		bool keepRightSideUp;
		bool topmost;
		bool visualStyleEnabled;
		bool skipMinimize;
		bool screenSaver;
		int originalMouseX;
		int originalMouseY;
		HMENU hFileMenu;
		HMENU hViewMenu;
		HMENU hViewAngleMenu;
		HMENU hStandardSizesMenu;
		HMENU hStepMenu;
		HMENU hToolsMenu;
		bool loading;
		CUIWindowResizer *openGLInfoWindoResizer;
		HWND hOpenGLStatusBar;
		HICON hExamineIcon;
		HICON hFlythroughIcon;
		HICON hWalkIcon;
#ifndef TC_NO_UNICODE
		HMONITOR hMonitor;
#endif // TC_NO_UNICODE
#if defined(USE_CPP11) || !defined(_NO_BOOST)
		HWND hLibraryUpdateWindow;
		LDLibraryUpdater *libraryUpdater;
		bool libraryUpdateFinished;
		bool libraryUpdateCanceled;
#endif // !_NO_BOOST
		LDViewPreferences *prefs;
		bool drawWireframe;
		bool seams;
		bool edges;
		bool primitiveSubstitution;
		bool lighting;
		bool bfc;
		bool examineLatLong;
		bool initialShown;
		ModelTreeDialog *modelTreeDialog;
		BoundingBoxDialog *boundingBoxDialog;
		MpdDialog *mpdDialog;
		LDrawModelViewer::StandardSizeVector standardSizes;

		static TCStringArray* recentFiles;
		static TCStringArray* extraSearchDirs;
		static UCCHAR *productVersion;
		static UCCHAR *legalCopyright;

		static class LDViewWindowCleanup
		{
		public:
			~LDViewWindowCleanup(void);
		} ldViewWindowCleanup;
		friend class LDViewWindowCleanup;
};

#endif // __LD3VIEWWINDOW_H__
