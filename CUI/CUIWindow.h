#ifndef __CUIWINDOW_H__
#define __CUIWINDOW_H__

#include <TCFoundation/TCObject.h>
#include <windows.h>
//#include <windowsx.h>
#include <CUI/CUIDefines.h>
#include <TCFoundation/TCTypedPointerArray.h>

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED                 0x031A
#endif

typedef TCTypedPointerArray<HWND> CUIHwndArray;

#define CUI_MAX_CHILDREN 100
#define NUM_SYSTEM_COLORS 25

class CUIExport CUIWindow : public TCObject
{
	public:
		CUIWindow(void);
		CUIWindow(const char* windowTitle, HINSTANCE hInstance, int x,
			int y, int width, int height);
		CUIWindow(CUIWindow* parentWindow, int x, int y, int width,
			int height);
		CUIWindow(HWND hParentWindow, HINSTANCE, int x, int y,
			int width, int height);

		HWND getHWindow(void) { return hWindow; }
		int getX(void) { return x; }
		int getY(void) { return y; }
		int getWidth(void) { return width; }
		int getHeight(void) { return height; }
		HINSTANCE getHInstance(void) { return hInstance; }
		BOOL isInitialized(void) { return initialized; }
		char* getWindowTitle(void) { return windowTitle; }
		virtual void showWindow(int nCmdShow);
		virtual BOOL initWindow(void);
		virtual void resize(int newWidth, int newHeight);
		virtual void setRBGFillColor(BYTE r, BYTE g, BYTE b);
		virtual void setFillColor(DWORD newColor);
		virtual void setTitle(char* value);
		void setMinWidth(int value) { minWidth = value; }
		void setMinHeight(int value) { minHeight = value; }
		void setMaxWidth(int value) { maxWidth = value; }
		void setMaxHeight(int value) { maxHeight = value; }
		virtual SIZE getDecorationSize(void);
		virtual void runDialogModal(HWND hDlg);
		virtual bool flushModal(HWND hWnd, bool isDialog, int maxFlush = -1);
		virtual bool flushDialogModal(HWND hDlg);
		virtual void closeWindow(void);
		virtual void maximize(void);
		virtual void minimize(void);
		virtual void restore(void);
		virtual UINT setTimer(UINT timerID, UINT elapse);
		virtual BOOL killTimer(UINT timerID);
		HWND getHParentWindow(void) { return hParentWindow; }

		static void setArrowCursor(void);
		static void setWaitCursor(void);
		static HMENU findSubMenu(HMENU hParentMenu, int subMenuIndex,
			int *index = NULL);
	protected:
		~CUIWindow(void);
		virtual void dealloc(void);
		void init(void);
		virtual void registerWindowClass(void);
		virtual BOOL createWindow(void);
		virtual BOOL createMainWindow(void);
		virtual BOOL createSubWindow(void);
		virtual char* windowClassName(void);
		virtual LRESULT windowProc(HWND hWnd, UINT message,
			WPARAM wParam, LPARAM lParam);
		virtual BOOL dialogProc(HWND hDlg, UINT message,
			WPARAM wParam, LPARAM lParam);
		virtual WNDCLASSEX getWindowClass(void);
		virtual void addChild(CUIWindow* childWindow);
		virtual void removeChild(CUIWindow* childWindow);
		virtual void updateSystemColors(void);
		virtual void initSystemColors(void);
		virtual void deleteSystemColorPens(void);
		virtual void redrawChildren(BOOL recurse);
		virtual HBRUSH getBackgroundBrush(void);

		virtual LRESULT doSize(WPARAM sizeType, int newWidth,
			int newHeight);
		virtual LRESULT doMove(int newX, int newY);
		virtual LRESULT doEraseBackground(RECT* updateRect = NULL);
		virtual LRESULT doCreate(HWND hWnd,
			LPCREATESTRUCT createStruct);
		virtual LRESULT doClose(void);
		virtual LRESULT doDestroy(void);
		virtual LRESULT doNCDestroy(void);
		virtual void doPaint(void);
		virtual void doPostPaint(void);
		virtual void doSystemColorChange(void);
		virtual LRESULT doLButtonDown(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doLButtonUp(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doLButtonDoubleClick(WPARAM keyFlags,
			int xPos, int yPos);
		virtual LRESULT doRButtonDown(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doRButtonUp(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doRButtonDoubleClick(WPARAM keyFlags,
			int xPos, int yPos);
		virtual LRESULT doMButtonDown(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doMButtonUp(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doMButtonDoubleClick(WPARAM keyFlags,
			int xPos, int yPos);
		virtual LRESULT doMouseMove(WPARAM keyFlags, int xPos,
			int yPos);
		virtual LRESULT doMouseWheel(short keyFlags, short zDelta,
			int xPos, int yPos);
		virtual LRESULT doKeyDown(int keyCode, long keyData);
		virtual LRESULT doKeyUp(int keyCode, long keyData);
		virtual LRESULT doDropFiles(HDROP hDrop);
		virtual LRESULT doChar(TCHAR characterCode, LPARAM keyData);
		virtual LRESULT doShowWindow(BOOL showFlag, LPARAM status);
		virtual LRESULT doActivateApp(BOOL activateFlag,
			DWORD threadId);
		virtual LRESULT doActivate(int activateFlag, BOOL minimized,
			HWND previousHWindow);
		virtual LRESULT doGetMinMaxInfo(HWND hWnd,
			LPMINMAXINFO minMaxInfo);
		virtual LRESULT doCommand(int itemId, int notifyCode,
			HWND controlHWnd);
		virtual LRESULT doTimer(UINT timerID);
		virtual LRESULT doHelp(LPHELPINFO helpInfo);
		virtual LRESULT doMenuSelect(UINT menuID, UINT flags, HMENU hMenu);
		virtual LRESULT doEnterMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doExitMenuLoop(bool isTrackPopupMenu);
		virtual LRESULT doInitMenuPopup(HMENU hPopupMenu, UINT uPos,
			BOOL fSystemMenu);
		virtual LRESULT doDrawItem(HWND hControlWnd,
			LPDRAWITEMSTRUCT drawItemStruct);
		virtual LRESULT doThemeChanged(void);
		virtual LRESULT doNotify(int controlId, LPNMHDR notification);
		virtual BOOL doDialogThemeChanged(void);
		virtual BOOL doDialogCtlColorStatic(HDC hdcStatic, HWND hwndStatic);
		virtual BOOL doDialogCtlColorBtn(HDC hdcStatic, HWND hwndStatic);
		virtual void doDialogClose(HWND hDlg);
		virtual BOOL doDialogCommand(HWND hDlg, int controlId,
			int notifyCode, HWND controlHWnd);
		virtual BOOL doDialogVScroll(HWND hDlg, int scrollCode,
			int position, HWND hScrollBar);
		virtual BOOL doDialogHScroll(HWND hDlg, int scrollCode,
			int position, HWND hScrollBar);
		virtual BOOL doDialogInit(HWND hDlg, HWND hFocusWindow, LPARAM lParam);
		virtual BOOL doDialogNotify(HWND hDlg, int controlId,
			LPNMHDR notification);
		virtual BOOL doDialogSize(HWND hDlg, WPARAM sizeType,
			int newWidth, int newHeight);
		virtual BOOL doDialogGetMinMaxInfo(HWND hDlg,
			LPMINMAXINFO minMaxInfo);
		virtual BOOL doDialogChar(HWND hDlg, TCHAR characterCode,
			LPARAM keyData);
		virtual BOOL doDialogHelp(HWND hDlg, LPHELPINFO helpInfo);
		virtual HWND createDialog(char* templateName,
			BOOL asChildWindow = TRUE);
		virtual HWND createDialog(int templateNumber,
			BOOL asChildWindow = TRUE);
		virtual void setupDialogSlider(HWND hDlg, int controlId, short min,
			short max, WORD frequency, int value);
		virtual bool copyToClipboard(const char *value);

		static void printMessageName(UINT message);
		static LRESULT CALLBACK staticWindowProc(HWND hWnd,
			UINT message, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK staticDialogProc(HWND hDlg,
			UINT message, WPARAM wParam, LPARAM lParam);
		static BOOL CALLBACK disableNonModalWindow(HWND hWnd,
			LPARAM hModalDialog);
		static BOOL CALLBACK enableNonModalWindow(HWND hWnd,
			LPARAM hModalDialog);
		static void calcSystemSizes(void);
		static void populateAppVersion(void);
		static HINSTANCE getLanguageModule(void);

		char* windowTitle;
		int x;
		int y;
		int width;
		int height;
		int minWidth;
		int minHeight;
		int maxWidth;
		int maxHeight;
		HINSTANCE hInstance;
		HWND hWindow;
		HMENU hWindowMenu;
		HDC hdc;
		CUIWindow* parentWindow;
		HWND hParentWindow;
		DWORD exWindowStyle;
		DWORD windowStyle;
		UINT windowClassStyle;
		BOOL initialized;
		CUIWindow* children[CUI_MAX_CHILDREN];
		int numChildren;
		BOOL created;
		DWORD fillColor;
		DWORD* systemColors;
		HPEN* systemColorPens;
		HBRUSH hBackgroundBrush;
		PAINTSTRUCT* paintStruct;

		static int systemMaxWidth;
		static int systemMaxHeight;
		static int systemMinTrackWidth;
		static int systemMinTrackHeight;
		static int systemMaxTrackWidth;
		static int systemMaxTrackHeight;
		static HCURSOR hArrowCursor;
		static HCURSOR hWaitCursor;
		static bool appVersionPopulated;
		static DWORD appVersionMS;
		static DWORD appVersionLS;
	private:
		static void populateLanguageModule(HINSTANCE hDefaultModule);
		static bool loadLanguageModule(LCID lcid, bool includeSub = true);

		static HINSTANCE hLanguageModule;	// Always use getLanguageModule()
};

#endif
