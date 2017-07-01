#include "CUIWindow.h"
#include "CUIWindowResizer.h"
#include <assert.h>
#include <commctrl.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUserDefaults.h>
#include <stdio.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

CUIExport int			CUIWindow::systemMaxWidth = -1;
CUIExport int			CUIWindow::systemMaxHeight = -1;
CUIExport int			CUIWindow::systemMinTrackWidth = -1;
CUIExport int			CUIWindow::systemMinTrackHeight = -1;
CUIExport int			CUIWindow::systemMaxTrackWidth = -1;
CUIExport int			CUIWindow::systemMaxTrackHeight = -1;
CUIExport HCURSOR		CUIWindow::hArrowCursor = NULL;
CUIExport HCURSOR		CUIWindow::hWaitCursor = NULL;
CUIExport HINSTANCE		CUIWindow::hLanguageModule = NULL;
CUIExport bool			CUIWindow::appVersionPopulated = false;
CUIExport DWORD			CUIWindow::appVersionMS;
CUIExport DWORD			CUIWindow::appVersionLS;
CUIExport UIntUIntMap	CUIWindow::ucMessages;

CUIWindow::CUIWindow(void)
		  :windowTitle(copyString(_UC(""))),
		   hInstance(NULL),
		   x(0),
		   y(0),
		   width(0),
		   height(0),
		   maxWidth(-1),
		   maxHeight(-1),
		   minWidth(-1),
		   minHeight(-1),
		   hdc(NULL),
		   windowStyle(0),
		   windowClassStyle(0),
		   exWindowStyle(0),
		   hWindow(NULL),
		   hWindowMenu(NULL),
		   parentWindow(NULL),
		   hParentWindow(NULL),
		   initialized(FALSE),
		   created(FALSE),
		   numChildren(0),
		   hBackgroundBrush(NULL),
		   paintStruct(NULL),
		   autosaveName(NULL)
{
	init();
}

CUIWindow::CUIWindow(CUCSTR windowTitle, HINSTANCE hInstance, int x, int y,
					 int width, int height)
		  :windowTitle(copyString(windowTitle)),
		   hInstance(hInstance),
		   x(x),
		   y(y),
		   width(width),
		   height(height),
		   maxWidth(-1),
		   maxHeight(-1),
		   minWidth(-1),
		   minHeight(-1),
		   hdc(NULL),
		   windowStyle(WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
		    WS_SYSMENU | WS_THICKFRAME),
		   windowClassStyle(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS),
		   exWindowStyle(WS_EX_CONTROLPARENT),
		   hWindow(NULL),
		   hWindowMenu(NULL),
		   parentWindow(NULL),
		   hParentWindow(NULL),
		   initialized(FALSE),
		   created(FALSE),
		   numChildren(0),
		   hBackgroundBrush(NULL),
		   paintStruct(NULL),
		   autosaveName(NULL)
{
	init();
}

CUIWindow::CUIWindow(CUIWindow* parentWindow, int x, int y, int width,
					 int height)
		  :windowTitle(copyString(_UC(""))),
		   hInstance(parentWindow->getHInstance()),
		   x(x),
		   y(y),
		   width(width),
		   height(height),
		   maxWidth(-1),
		   maxHeight(-1),
		   minWidth(-1),
		   minHeight(-1),
		   hdc(NULL),
		   windowStyle(WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/),
		   windowClassStyle(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS),
		   exWindowStyle(WS_EX_CONTROLPARENT),
		   hWindow(NULL),
		   hWindowMenu(NULL),
		   parentWindow(parentWindow),
		   hParentWindow(parentWindow->getHWindow()),
		   initialized(FALSE),
		   created(FALSE),
		   numChildren(0),
		   hBackgroundBrush(NULL),
		   paintStruct(NULL),
		   autosaveName(NULL)
{
	parentWindow->addChild(this);
	init();
}

CUIWindow::CUIWindow(HWND hParentWindow, HINSTANCE hInstance, int x, int y,
					 int width, int height)
		  :windowTitle(copyString(_UC(""))),
		   x(x),
		   y(y),
		   width(width),
		   height(height),
		   maxWidth(-1),
		   maxHeight(-1),
		   minWidth(-1),
		   minHeight(-1),
		   hdc(NULL),
		   windowStyle(WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/),
		   windowClassStyle(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS),
		   exWindowStyle(WS_EX_CONTROLPARENT),
		   hInstance(hInstance),
		   hWindow(NULL),
		   hWindowMenu(NULL),
		   parentWindow(NULL),
		   hParentWindow(hParentWindow),
		   initialized(FALSE),
		   created(FALSE),
		   numChildren(0),
		   hBackgroundBrush(NULL),
		   paintStruct(NULL),
		   autosaveName(NULL)
{
//	parentWindow->addChild(this);
	init();
}

BOOL CUIWindow::initWindow(void)
{
	registerWindowClass();
	if (createWindow())
	{
		initialized = TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

const char* CUIWindow::windowClassName(void)
{
	if (parentWindow || hParentWindow)
	{
		return "CUISubWindow";
	}
	else
	{
		return "CUIMainWindow";
	}
}

CUIWindow::~CUIWindow(void)
{
}

void CUIWindow::dealloc(void)
{
	if (hWindow)
	{
		DestroyWindow(hWindow);
		hWindow = NULL;
	}
	if (hWindowMenu)
	{
		DestroyMenu(hWindowMenu);
		hWindowMenu = NULL;
	}
	delete windowTitle;
	windowTitle = NULL;
	if (!parentWindow)
	{
		deleteSystemColorPens();
		delete systemColors;
		systemColors = NULL;
		delete systemColorPens;
		systemColorPens = NULL;
		if (hBackgroundBrush)
		{
 			DeleteObject(hBackgroundBrush);
		}
	}
	delete autosaveName;
	TCObject::dealloc();
}

void CUIWindow::closeWindow(void)
{
	DestroyWindow(hWindow);
}

void CUIWindow::setArrowCursor(void)
{
	if (!hArrowCursor)
	{
		hArrowCursor = LoadCursor(NULL, IDC_ARROW);
	}
	SetCursor(hArrowCursor);
}

void CUIWindow::setWaitCursor(void)
{
	if (!hWaitCursor)
	{
		hWaitCursor = LoadCursor(NULL, IDC_WAIT);
	}
	SetCursor(hWaitCursor);
}

void CUIWindow::deleteSystemColorPens(void)
{
	if (!parentWindow)
	{
		int i;

		for (i = 0; i < NUM_SYSTEM_COLORS; i++)
		{
			if (systemColorPens[i])
			{
				DeleteObject(systemColorPens[i]);
				systemColorPens[i] = 0;
			}
		}
	}
}

void CUIWindow::init(void)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "CUIWindow");
#endif
	initUcMessages();
	initSystemColors();
}

void CUIWindow::setRBGFillColor(BYTE r, BYTE g, BYTE b)
{
	setFillColor(RGB(r, g, b));
}

void CUIWindow::setFillColor(DWORD newColor)
{
	fillColor = newColor;
}

void CUIWindow::updateSystemColors(void)
{
	if (!parentWindow)
	{
		int i;

		deleteSystemColorPens();
		for (i = 0; i < NUM_SYSTEM_COLORS; i++)
		{
			systemColors[i] = GetSysColor(i);
			systemColorPens[i] = CreatePen(PS_SOLID, 0, systemColors[i]);
		}
	}
}

void CUIWindow::initUcMessages(void)
{
	ucMessages[WM_SETTEXT] = WM_SETTEXT;
	ucMessages[WM_GETTEXT] = WM_GETTEXT;
	ucMessages[CB_DELETESTRING] = CB_DELETESTRING;
	ucMessages[CB_ADDSTRING] = CB_ADDSTRING;
	ucMessages[LB_ADDSTRING] = LB_ADDSTRING;
	ucMessages[CB_SELECTSTRING] = CB_SELECTSTRING;
#ifdef TC_NO_UNICODE
	ucMessages[SB_SETTIPTEXT] = SB_SETTIPTEXTA;
	ucMessages[SB_GETTIPTEXT] = SB_GETTIPTEXTA;
	ucMessages[SB_SETTEXT] = SB_SETTEXTA;
	ucMessages[SB_GETTEXT] = SB_GETTEXTA;
	ucMessages[TTM_ADDTOOL] = TTM_ADDTOOLA;
#else // TC_NO_UNICODE
	ucMessages[SB_SETTIPTEXT] = SB_SETTIPTEXTW;
	ucMessages[SB_GETTIPTEXT] = SB_GETTIPTEXTW;
	ucMessages[SB_SETTEXT] = SB_SETTEXTW;
	ucMessages[SB_GETTEXT] = SB_GETTEXTW;
	ucMessages[TTM_ADDTOOL] = TTM_ADDTOOLW;
#endif // TC_NO_UNICODE
}

void CUIWindow::initSystemColors(void)
{
	if (parentWindow)
	{
		systemColors = parentWindow->systemColors;
		systemColorPens = parentWindow->systemColorPens;
	}
	else
	{
		systemColors = new DWORD[NUM_SYSTEM_COLORS];
		systemColorPens = new HPEN[NUM_SYSTEM_COLORS];
		memset(systemColorPens, 0, sizeof(HPEN) * NUM_SYSTEM_COLORS);
		updateSystemColors();
	}
}

void CUIWindow::resize(int newWidth, int newHeight)
{
	if (initialized)
	{
		SIZE decorationSize = {0, 0};

		if (parentWindow)
		{
			decorationSize = getDecorationSize();
		}
		MoveWindow(hWindow, x - decorationSize.cx / 2, y - decorationSize.cy / 2,
			newWidth, newHeight, TRUE);
	}
	else
	{
		width = newWidth;
		height = newHeight;
	}
}

UINT_PTR CUIWindow::setTimer(UINT timerID, UINT elapse)
{
	return SetTimer(hWindow, timerID, elapse, NULL);
}

BOOL CUIWindow::killTimer(UINT timerID)
{
	return KillTimer(hWindow, timerID);
}

LRESULT CUIWindow::doCommand(int, int, HWND)
{
	return 1;
}

LRESULT CUIWindow::doTimer(UINT_PTR)
{
	return 1;
}

LRESULT CUIWindow::doHelp(LPHELPINFO)
{
	return 0;
}

LRESULT CUIWindow::doMenuSelect(UINT, UINT, HMENU)
{
	return 1;
}

LRESULT CUIWindow::doThemeChanged(void)
{
	return 1;
}

LRESULT CUIWindow::doEnterMenuLoop(bool /*isTrackPopupMenu*/)
{
	return 1;
}

LRESULT CUIWindow::doExitMenuLoop(bool /*isTrackPopupMenu*/)
{
	return 1;
}

LRESULT CUIWindow::doInitMenuPopup(HMENU /*hPopupMenu*/, UINT /*uPos*/,
								   BOOL /*fSystemMenu*/)
{
	return 0;
}

LRESULT CUIWindow::doDrawItem(HWND /*hControlWnd*/,
							  LPDRAWITEMSTRUCT /*drawItemStruct*/)
{
	return FALSE;
}

LRESULT CUIWindow::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (sizeType == SIZE_MINIMIZED)
	{
		return 1;
	}
	else
	{
		int deltaWidth = newWidth - width;
		int deltaHeight = newHeight - height;
		int i;

		for (i = 0; i < numChildren; i++)
		{
			CUIWindow* child = children[i];
			SIZE decorationSize = child->getDecorationSize();

			child->resize(child->width + decorationSize.cx + deltaWidth,
				child->height + decorationSize.cy + deltaHeight);
		}
		width = newWidth;
		height = newHeight;
		if (autosaveName != NULL)
		{
			int saveX, saveY, saveWidth, saveHeight, saveMaximized;

			if (sizeType != SIZE_MAXIMIZED || !readAutosaveInfo(saveX, saveY,
				saveWidth, saveHeight, saveMaximized))
			{
				RECT rect;

				GetWindowRect(hWindow, &rect);
				saveX = rect.left;
				saveY = rect.top;
				saveWidth = rect.right - rect.left;
				saveHeight = rect.bottom - rect.top;
			}
			saveMaximized = sizeType == SIZE_MAXIMIZED;
			writeAutosaveInfo(saveX, saveY, saveWidth, saveHeight,
				saveMaximized);
		}
		if (autosaveName != NULL)
		{
		}
		return 0;
	}
}

LRESULT CUIWindow::doMove(int newX, int newY)
{
	x = newX;
	y = newY;
	if (autosaveName != NULL)
	{
		if ((GetWindowLong(hWindow, GWL_STYLE) & WS_MAXIMIZE) == 0)
		{
			RECT rect;
			GetWindowRect(hWindow, &rect);

			writeAutosaveInfo(rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, 0);
		}
	}
	return 1;
}

LRESULT CUIWindow::doEraseBackground(RECT *)
{
	created = TRUE;
	return 0;
}

SIZE CUIWindow::getDecorationSize(void)
{
	RECT windowRect = { 0, 0, 100, 100 };
	WNDCLASSEX windowClass = getWindowClass();
	SIZE size;

	AdjustWindowRectEx(&windowRect, windowStyle,
		windowClass.lpszMenuName != NULL || hWindowMenu, exWindowStyle);
	size.cx = windowRect.right - windowRect.left - 100;
	size.cy = windowRect.bottom - windowRect.top - 100;
	if (!(windowStyle & WS_POPUP) && !(windowStyle & WS_CHILD))
	{
		size.cx += 2;
		size.cy += GetSystemMetrics(SM_CYCAPTION) + 2;
	}
	return size;
}

LRESULT CUIWindow::doCreate(HWND hWnd, LPCREATESTRUCT createStruct)
{
	SIZE decorationSize = getDecorationSize();
	int dx = decorationSize.cx;
	int dy = decorationSize.cy;
	RECT windowRect;
	int windowWidth;
	int windowHeight;

	hWindow = hWnd;
	GetWindowRect(hWnd, &windowRect);
	windowWidth = windowRect.right - windowRect.left;
	windowHeight = windowRect.bottom - windowRect.top;
	if (windowStyle & WS_CHILD)
	{
		return 0;
	}
//	if (windowStyle & WS_MAXIMIZE)
//	{
//		doSize(SIZE_MAXIMIZED, windowWidth - dx, windowHeight - dy);
//	}
	if (windowWidth == createStruct->cx && windowHeight == createStruct->cy)
	{
		doSize(SIZE_RESTORED, createStruct->cx - dx, createStruct->cy - dy);
	}
	else if (windowWidth > createStruct->cx || windowHeight > createStruct->cy)
	{
		doSize(SIZE_MAXIMIZED, windowWidth - dx, windowHeight - dy);
	}
	else
	{
		doSize(SIZE_MINIMIZED, windowWidth - dx, windowHeight - dy);
	}
//	x = createStruct->x;
//	y = createStruct->y;
	return 0;
}

LRESULT CUIWindow::doClose(void)
{
	return 1;
}

LRESULT CUIWindow::doDestroy(void)
{
	return 1;
}

LRESULT CUIWindow::doNCDestroy(void)
{
	// Clean up.
	SetWindowLongPtrUC(hWindow, GWLP_USERDATA, (LONG_PTR)NULL);
	hWindow = NULL;
	hdc = NULL;
	hParentWindow = NULL;
	initialized = FALSE;
	created = FALSE;
	return 1;
}

void CUIWindow::doPaint(void)
{
}

void CUIWindow::doPostPaint(void)
{
}

LRESULT CUIWindow::doLButtonDown(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doLButtonUp(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doLButtonDoubleClick(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doRButtonDown(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doRButtonUp(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doRButtonDoubleClick(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doMButtonDown(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doMButtonUp(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doMButtonDoubleClick(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doMouseMove(WPARAM, int, int)
{
	return 1;
}

LRESULT CUIWindow::doMouseWheel(short, short, int, int)
{
	return 1;
}

LRESULT CUIWindow::doCaptureChanged(HWND)
{
	return 1;
}

LRESULT CUIWindow::doKeyDown(int, LPARAM)
{
	return 1;
}

LRESULT CUIWindow::doKeyUp(int, LPARAM)
{
	return 1;
}

LRESULT CUIWindow::doDropFiles(HDROP)
{
	return 1;
}

LRESULT CUIWindow::doChar(TCHAR, LPARAM)
{
	return 1;
}

LRESULT CUIWindow::doShowWindow(BOOL, LPARAM)
{
	return 1;
}

LRESULT CUIWindow::doActivateApp(BOOL, DWORD)
{
	return 1;
}

LRESULT CUIWindow::doActivate(int, BOOL, HWND)
{
	return 1;
}

void CUIWindow::redrawChildren(BOOL recurse)
{
	int i;

	for (i = 0; i < numChildren; i++)
	{
		children[i]->doPaint();
		if (recurse)
		{
			children[i]->redrawChildren(TRUE);
		}
	}
}

void CUIWindow::doSystemColorChange(void)
{
	updateSystemColors();
	redrawChildren(TRUE);
}

void CUIWindow::calcSystemSizes(void)
{
	if (systemMaxWidth == -1)
	{
		systemMaxWidth = GetSystemMetrics(SM_CXMAXIMIZED);
		systemMaxHeight = GetSystemMetrics(SM_CYMAXIMIZED);
		systemMinTrackWidth = GetSystemMetrics(SM_CXMINTRACK);
		systemMinTrackHeight = GetSystemMetrics(SM_CYMINTRACK);
		systemMaxTrackWidth = GetSystemMetrics(SM_CXMAXTRACK);
		systemMaxTrackHeight = GetSystemMetrics(SM_CYMAXTRACK);
	}
}

LRESULT CUIWindow::doNotify(int /*controlId*/, LPNMHDR /*notification*/)
{
	return 0;
}

LRESULT CUIWindow::doGetMinMaxInfo(HWND hWnd, LPMINMAXINFO minMaxInfo)
{
	if (initialized && (windowStyle & WS_CHILD) == 0)
	{
		if (minWidth >= 0 || minHeight >= 0)
		{
			int realMaxWidth;
			int realMaxHeight;
			int minTrackWidth;
			int minTrackHeight;
			int maxTrackWidth;
			int maxTrackHeight;
			int decorationWidth;
			int decorationHeight;
			RECT clientRect;
			RECT windowRect;

			GetClientRect(hWnd, &clientRect);
			GetWindowRect(hWnd, &windowRect);
			decorationWidth = (windowRect.right - windowRect.left) -
				(clientRect.right - clientRect.left);
			decorationHeight = (windowRect.bottom - windowRect.top) -
				(clientRect.bottom - clientRect.top);
			calcSystemSizes();
			if (maxWidth == -1)
			{
				realMaxWidth = systemMaxWidth;
			}
			else
			{
				realMaxWidth = maxWidth;
			}
			if (maxHeight == -1)
			{
				realMaxHeight = systemMaxHeight;
			}
			else
			{
				realMaxHeight = maxHeight;
			}
			if (minWidth == -1)
			{
				minTrackWidth = systemMinTrackWidth;
			}
			else if (minWidth + decorationWidth > systemMaxTrackWidth)
			{
				minTrackWidth = systemMaxTrackWidth;
			}
			else
			{
				minTrackWidth = minWidth + decorationWidth;
			}
			if (minHeight == -1)
			{
				minTrackHeight = systemMinTrackHeight;
			}
			else if (minHeight + decorationHeight > systemMaxTrackHeight)
			{
				minTrackHeight = systemMaxTrackHeight;
			}
			else
			{
				minTrackHeight = minHeight + decorationHeight;
			}
			if (maxWidth == -1)
			{
				maxTrackWidth = systemMaxTrackWidth;
			}
			else
			{
				maxTrackWidth = maxWidth;
			}
			if (maxHeight == -1)
			{
				maxTrackHeight = systemMaxTrackHeight;
			}
			else
			{
				maxTrackHeight = maxHeight;
			}
			minMaxInfo->ptMaxSize.x = realMaxWidth;
			minMaxInfo->ptMaxSize.y = realMaxHeight;
			minMaxInfo->ptMinTrackSize.x = minTrackWidth;
			minMaxInfo->ptMinTrackSize.y = minTrackHeight;
			minMaxInfo->ptMaxTrackSize.x = maxTrackWidth;
			minMaxInfo->ptMaxTrackSize.y = maxTrackHeight;
			return 0;
		}
	}
	return 1;
}

void CUIWindow::printMessageName(UINT message)
{
	printf("message: %s\n", getMessageName(message).c_str());
}

std::string CUIWindow::getMessageName(UINT message)
{
	switch (message)
	{
	case WM_NULL:
		return "WM_NULL";
	case WM_CREATE:
		return "WM_CREATE";
	case WM_DESTROY:
		return "WM_DESTROY";
	case WM_MOVE:
		return "WM_MOVE";
	case WM_SIZE:
		return "WM_SIZE";
	case WM_ACTIVATE:
		return "WM_ACTIVATE";
	case WM_SETFOCUS:
		return "WM_SETFOCUS";
	case WM_KILLFOCUS:
		return "WM_KILLFOCUS";
	case WM_ENABLE:
		return "WM_ENABLE";
	case WM_SETREDRAW:
		return "WM_SETREDRAW";
	case WM_SETTEXT:
		return "WM_SETTEXT";
	case WM_GETTEXT:
		return "WM_GETTEXT";
	case WM_GETTEXTLENGTH:
		return "WM_GETTEXTLENGTH";
	case WM_PAINT:
		return "WM_PAINT";
	case WM_CLOSE:
		return "WM_CLOSE";
	case WM_QUERYENDSESSION:
		return "WM_QUERYENDSESSION";
	case WM_QUIT:
		return "WM_QUIT";
	case WM_QUERYOPEN:
		return "WM_QUERYOPEN";
	case WM_ERASEBKGND:
		return "WM_ERASEBKGND";
	case WM_SYSCOLORCHANGE:
		return "WM_SYSCOLORCHANGE";
	case WM_ENDSESSION:
		return "WM_ENDSESSION";
	case WM_SHOWWINDOW:
		return "WM_SHOWWINDOW";
	case WM_SETTINGCHANGE:
		return "WM_SETTINGCHANGE";
	case WM_DEVMODECHANGE:
		return "WM_DEVMODECHANGE";
	case WM_ACTIVATEAPP:
		return "WM_ACTIVATEAPP";
	case WM_FONTCHANGE:
		return "WM_FONTCHANGE";
	case WM_TIMECHANGE:
		return "WM_TIMECHANGE";
	case WM_CANCELMODE:
		return "WM_CANCELMODE";
	case WM_SETCURSOR:
		return "WM_SETCURSOR";
	case WM_MOUSEACTIVATE:
		return "WM_MOUSEACTIVATE";
	case WM_CHILDACTIVATE:
		return "WM_CHILDACTIVATE";
	case WM_QUEUESYNC:
		return "WM_QUEUESYNC";
	case WM_GETMINMAXINFO:
		return "WM_GETMINMAXINFO";
	case WM_PAINTICON:
		return "WM_PAINTICON";
	case WM_ICONERASEBKGND:
		return "WM_ICONERASEBKGND";
	case WM_NEXTDLGCTL:
		return "WM_NEXTDLGCTL";
	case WM_SPOOLERSTATUS:
		return "WM_SPOOLERSTATUS";
	case WM_DRAWITEM:
		return "WM_DRAWITEM";
	case WM_MEASUREITEM:
		return "WM_MEASUREITEM";
	case WM_DELETEITEM:
		return "WM_DELETEITEM";
	case WM_VKEYTOITEM:
		return "WM_VKEYTOITEM";
	case WM_CHARTOITEM:
		return "WM_CHARTOITEM";
	case WM_SETFONT:
		return "WM_SETFONT";
	case WM_GETFONT:
		return "WM_GETFONT";
	case WM_SETHOTKEY:
		return "WM_SETHOTKEY";
	case WM_GETHOTKEY:
		return "WM_GETHOTKEY";
	case WM_QUERYDRAGICON:
		return "WM_QUERYDRAGICON";
	case WM_COMPAREITEM:
		return "WM_COMPAREITEM";
	case WM_COMPACTING:
		return "WM_COMPACTING";
	case WM_COMMNOTIFY:
		return "WM_COMMNOTIFY";
	case WM_WINDOWPOSCHANGING:
		return "WM_WINDOWPOSCHANGING";
	case WM_WINDOWPOSCHANGED:
		return "WM_WINDOWPOSCHANGED";
	case WM_POWER:
		return "WM_POWER";
	case WM_COPYDATA:
		return "WM_COPYDATA";
	case WM_CANCELJOURNAL:
		return "WM_CANCELJOURNAL";
	case WM_NOTIFY:
		return "WM_NOTIFY";
	case WM_INPUTLANGCHANGEREQUEST:
		return "WM_INPUTLANGCHANGEREQUEST";
	case WM_INPUTLANGCHANGE:
		return "WM_INPUTLANGCHANGE";
	case WM_TCARD:
		return "WM_TCARD";
	case WM_HELP:
		return "WM_HELP";
	case WM_USERCHANGED:
		return "WM_USERCHANGED";
	case WM_NOTIFYFORMAT:
		return "WM_NOTIFYFORMAT";
	case WM_CONTEXTMENU:
		return "WM_CONTEXTMENU";
	case WM_STYLECHANGING:
		return "WM_STYLECHANGING";
	case WM_STYLECHANGED:
		return "WM_STYLECHANGED";
	case WM_DISPLAYCHANGE:
		return "WM_DISPLAYCHANGE";
	case WM_GETICON:
		return "WM_GETICON";
	case WM_SETICON:
		return "WM_SETICON";
	case WM_NCCREATE:
		return "WM_NCCREATE";
	case WM_NCDESTROY:
		return "WM_NCDESTROY";
	case WM_NCCALCSIZE:
		return "WM_NCCALCSIZE";
	case WM_NCHITTEST:
		return "WM_NCHITTEST";
	case WM_NCPAINT:
		return "WM_NCPAINT";
	case WM_NCACTIVATE:
		return "WM_NCACTIVATE";
	case WM_GETDLGCODE:
		return "WM_GETDLGCODE";
	case WM_SYNCPAINT:
		return "WM_SYNCPAINT";
	case WM_NCMOUSEMOVE:
		return "WM_NCMOUSEMOVE";
	case WM_NCLBUTTONDOWN:
		return "WM_NCLBUTTONDOWN";
	case WM_NCLBUTTONUP:
		return "WM_NCLBUTTONUP";
	case WM_NCLBUTTONDBLCLK:
		return "WM_NCLBUTTONDBLCLK";
	case WM_NCRBUTTONDOWN:
		return "WM_NCRBUTTONDOWN";
	case WM_NCRBUTTONUP:
		return "WM_NCRBUTTONUP";
	case WM_NCRBUTTONDBLCLK:
		return "WM_NCRBUTTONDBLCLK";
	case WM_NCMBUTTONDOWN:
		return "WM_NCMBUTTONDOWN";
	case WM_NCMBUTTONUP:
		return "WM_NCMBUTTONUP";
	case WM_NCMBUTTONDBLCLK:
		return "WM_NCMBUTTONDBLCLK";
	case WM_KEYDOWN:
		return "WM_KEYDOWN";
	case WM_KEYUP:
		return "WM_KEYUP";
	case WM_CHAR:
		return "WM_CHAR";
	case WM_DEADCHAR:
		return "WM_DEADCHAR";
	case WM_SYSKEYDOWN:
		return "WM_SYSKEYDOWN";
	case WM_SYSKEYUP:
		return "WM_SYSKEYUP";
	case WM_SYSCHAR:
		return "WM_SYSCHAR";
	case WM_SYSDEADCHAR:
		return "WM_SYSDEADCHAR";
	case WM_KEYLAST:
		return "WM_KEYLAST";
	case WM_IME_STARTCOMPOSITION:
		return "WM_IME_STARTCOMPOSITION";
	case WM_IME_ENDCOMPOSITION:
		return "WM_IME_ENDCOMPOSITION";
	case WM_IME_COMPOSITION:
		return "WM_IME_COMPOSITION";
	case WM_INITDIALOG:
		return "WM_INITDIALOG";
	case WM_COMMAND:
		return "WM_COMMAND";
	case WM_SYSCOMMAND:
		return "WM_SYSCOMMAND";
	case WM_TIMER:
		return "WM_TIMER";
	case WM_HSCROLL:
		return "WM_HSCROLL";
	case WM_VSCROLL:
		return "WM_VSCROLL";
	case WM_INITMENU:
		return "WM_INITMENU";
	case WM_INITMENUPOPUP:
		return "WM_INITMENUPOPUP";
	case WM_MENUSELECT:
		return "WM_MENUSELECT";
	case WM_MENUCHAR:
		return "WM_MENUCHAR";
	case WM_ENTERIDLE:
		return "WM_ENTERIDLE";
	case WM_CTLCOLORMSGBOX:
		return "WM_CTLCOLORMSGBOX";
	case WM_CTLCOLOREDIT:
		return "WM_CTLCOLOREDIT";
	case WM_CTLCOLORLISTBOX:
		return "WM_CTLCOLORLISTBOX";
	case WM_CTLCOLORBTN:
		return "WM_CTLCOLORBTN";
	case WM_CTLCOLORDLG:
		return "WM_CTLCOLORDLG";
	case WM_CTLCOLORSCROLLBAR:
		return "WM_CTLCOLORSCROLLBAR";
	case WM_CTLCOLORSTATIC:
		return "WM_CTLCOLORSTATIC";
	case WM_MOUSEMOVE:
		return "WM_MOUSEMOVE";
	case WM_LBUTTONDOWN:
		return "WM_LBUTTONDOWN";
	case WM_LBUTTONUP:
		return "WM_LBUTTONUP";
	case WM_LBUTTONDBLCLK:
		return "WM_LBUTTONDBLCLK";
	case WM_RBUTTONDOWN:
		return "WM_RBUTTONDOWN";
	case WM_RBUTTONUP:
		return "WM_RBUTTONUP";
	case WM_RBUTTONDBLCLK:
		return "WM_RBUTTONDBLCLK";
	case WM_MBUTTONDOWN:
		return "WM_MBUTTONDOWN";
	case WM_MBUTTONUP:
		return "WM_MBUTTONUP";
	case WM_MBUTTONDBLCLK:
		return "WM_MBUTTONDBLCLK";
	case WM_MOUSEWHEEL:
		return "WM_MOUSEWHEEL";
	case WM_PARENTNOTIFY:
		return "WM_PARENTNOTIFY";
	case WM_ENTERMENULOOP:
		return "WM_ENTERMENULOOP";
	case WM_EXITMENULOOP:
		return "WM_EXITMENULOOP";
	case WM_SIZING:
		return "WM_SIZING";
	case WM_CAPTURECHANGED:
		return "WM_CAPTURECHANGED";
	case WM_MOVING:
		return "WM_MOVING";
	case WM_POWERBROADCAST:
		return "WM_POWERBROADCAST";
	case WM_DEVICECHANGE:
		return "WM_DEVICECHANGE";
	case WM_MDICREATE:
		return "WM_MDICREATE";
	case WM_MDIDESTROY:
		return "WM_MDIDESTROY";
	case WM_MDIACTIVATE:
		return "WM_MDIACTIVATE";
	case WM_MDIRESTORE:
		return "WM_MDIRESTORE";
	case WM_MDINEXT:
		return "WM_MDINEXT";
	case WM_MDIMAXIMIZE:
		return "WM_MDIMAXIMIZE";
	case WM_MDITILE:
		return "WM_MDITILE";
	case WM_MDICASCADE:
		return "WM_MDICASCADE";
	case WM_MDIICONARRANGE:
		return "WM_MDIICONARRANGE";
	case WM_MDIGETACTIVE:
		return "WM_MDIGETACTIVE";
	case WM_MDISETMENU:
		return "WM_MDISETMENU";
	case WM_ENTERSIZEMOVE:
		return "WM_ENTERSIZEMOVE";
	case WM_EXITSIZEMOVE:
		return "WM_EXITSIZEMOVE";
	case WM_DROPFILES:
		return "WM_DROPFILES";
	case WM_MDIREFRESHMENU:
		return "WM_MDIREFRESHMENU";
	case WM_IME_SETCONTEXT:
		return "WM_IME_SETCONTEXT";
	case WM_IME_NOTIFY:
		return "WM_IME_NOTIFY";
	case WM_IME_CONTROL:
		return "WM_IME_CONTROL";
	case WM_IME_COMPOSITIONFULL:
		return "WM_IME_COMPOSITIONFULL";
	case WM_IME_SELECT:
		return "WM_IME_SELECT";
	case WM_IME_CHAR:
		return "WM_IME_CHAR";
	case WM_IME_KEYDOWN:
		return "WM_IME_KEYDOWN";
	case WM_IME_KEYUP:
		return "WM_IME_KEYUP";
	case WM_MOUSEHOVER:
		return "WM_MOUSEHOVER";
	case WM_MOUSELEAVE:
		return "WM_MOUSELEAVE";
	case WM_CUT:
		return "WM_CUT";
	case WM_COPY:
		return "WM_COPY";
	case WM_PASTE:
		return "WM_PASTE";
	case WM_CLEAR:
		return "WM_CLEAR";
	case WM_UNDO:
		return "WM_UNDO";
	case WM_RENDERFORMAT:
		return "WM_RENDERFORMAT";
	case WM_RENDERALLFORMATS:
		return "WM_RENDERALLFORMATS";
	case WM_DESTROYCLIPBOARD:
		return "WM_DESTROYCLIPBOARD";
	case WM_DRAWCLIPBOARD:
		return "WM_DRAWCLIPBOARD";
	case WM_PAINTCLIPBOARD:
		return "WM_PAINTCLIPBOARD";
	case WM_VSCROLLCLIPBOARD:
		return "WM_VSCROLLCLIPBOARD";
	case WM_SIZECLIPBOARD:
		return "WM_SIZECLIPBOARD";
	case WM_ASKCBFORMATNAME:
		return "WM_ASKCBFORMATNAME";
	case WM_CHANGECBCHAIN:
		return "WM_CHANGECBCHAIN";
	case WM_HSCROLLCLIPBOARD:
		return "WM_HSCROLLCLIPBOARD";
	case WM_QUERYNEWPALETTE:
		return "WM_QUERYNEWPALETTE";
	case WM_PALETTEISCHANGING:
		return "WM_PALETTEISCHANGING";
	case WM_PALETTECHANGED:
		return "WM_PALETTECHANGED";
	case WM_HOTKEY:
		return "WM_HOTKEY";
	case WM_PRINT:
		return "WM_PRINT";
	case WM_PRINTCLIENT:
		return "WM_PRINTCLIENT";
	case BM_GETCHECK:
		return "BM_GETCHECK";
	case BM_SETCHECK:
		return "BM_SETCHECK";
	case BM_GETSTATE:
		return "BM_GETSTATE";
	case BM_SETSTATE:
		return "BM_SETSTATE";
	case BM_SETSTYLE:
		return "BM_SETSTYLE";
	case BM_CLICK:
		return "BM_CLICK";
	case BM_GETIMAGE:
		return "BM_GETIMAGE";
	case BM_SETIMAGE:
		return "BM_SETIMAGE";
	default:
		{
			char string[128];

			sprintf(string, "0x%04X", message);
			return string;
		}
	}
}

LRESULT CUIWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam,
							  LPARAM lParam)
{
	LRESULT rval;
	RECT updateRect;

	if ((windowClassStyle & CS_OWNDC) && !hdc)
	{
		hdc = GetDC(hWnd);
	}
//	printf("%s\n", windowClassName());
//	printf("0x%08X 0x%08X 0x%08X ", hWnd, wParam, lParam);
//	printMessageName(message);
#ifdef _DEBUG
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "wMsg: 0x%X\n", message);
#endif // _DEBUG
	switch (message)
	{
		case WM_CREATE:
			return doCreate(hWnd, (LPCREATESTRUCT)lParam);
			break;
		case WM_GETMINMAXINFO:
			if (!doGetMinMaxInfo(hWnd, (LPMINMAXINFO)lParam))
			{
				return 0;
			}
			break;
		case WM_SHOWWINDOW:
			if (!doShowWindow((BOOL)wParam, lParam))
			{
				return 0;
			}
			break;
		case WM_ACTIVATEAPP:
			if (!doActivateApp((BOOL)wParam, (DWORD)lParam))
			{
				return 0;
			}
			break;
		case WM_ACTIVATE:
			if (!doActivate((int)(unsigned short)LOWORD(wParam), 
				(BOOL)(unsigned short)HIWORD(wParam), (HWND)lParam))
			{
				return 0;
			}
			break;
		case WM_SIZE:
			if (!doSize(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MOVE:
			if (!doMove((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_ERASEBKGND:
			{
				BOOL hadHdc = hdc != NULL;

				if (!hadHdc)
				{
					hdc = (HDC)wParam;
				}
				if ((rval = doEraseBackground()) != 0)
				{
					if (!hadHdc)
					{
						hdc = NULL;
					}
					return rval;
				}
				if (!hadHdc)
				{
					hdc = NULL;
				}
			}
			break;
		case WM_CLOSE:
			if (!doClose())
			{
				return 0;
			}
			break;
		case WM_DESTROY:
			if (!doDestroy())
			{
				return 0;
			}
			break;
		case WM_NCDESTROY:
			if (!doNCDestroy())
			{
				return 0;
			}
			break;
		case WM_COMMAND:
			if (!doCommand((int)(unsigned short)LOWORD(wParam),
				(int)(unsigned short)HIWORD(wParam), (HWND)lParam))
			{
				return 0;
			}
			break;
		case WM_PAINT:
			if (GetUpdateRect(hWnd, &updateRect, FALSE))
			{
				bool paintStructAllocated = false;

				if (!paintStruct)
				{
					paintStruct = new PAINTSTRUCT;
					paintStructAllocated = true;
				}
				if (windowClassStyle & CS_OWNDC)
				{
					if (BeginPaint(hWnd, paintStruct))
					{
						doPaint();
						EndPaint(hWnd, paintStruct);
					}
				}
				else
				{
					hdc = BeginPaint(hWnd, paintStruct);
					if (hdc)
					{
						if (paintStruct->fErase)
						{
							doEraseBackground(&updateRect);
						}
						doPaint();
						hdc = NULL;
						EndPaint(hWnd, paintStruct);
					}
				}
				if (paintStructAllocated)
				{
					delete paintStruct;
					paintStruct = NULL;
				}
				doPostPaint();
				return 0;
			}
			break;
		case WM_LBUTTONDOWN:
			if (!doLButtonDown(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_LBUTTONUP:
			if (!doLButtonUp(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_LBUTTONDBLCLK:
			if (!doLButtonDoubleClick(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_RBUTTONDOWN:
			if (!doRButtonDown(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_RBUTTONUP:
			if (!doRButtonUp(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_RBUTTONDBLCLK:
			if (!doRButtonDoubleClick(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MBUTTONDOWN:
			if (!doMButtonDown(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MBUTTONUP:
			if (!doMButtonUp(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MBUTTONDBLCLK:
			if (!doMButtonDoubleClick(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MOUSEMOVE:
			if (!doMouseMove(wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_MOUSEWHEEL:
			if (!doMouseWheel(LOWORD(wParam), HIWORD(wParam),
				(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_CAPTURECHANGED:
			if (!doCaptureChanged((HWND)lParam))
			{
				return 0;
			}
			break;
		case WM_KEYDOWN:
			if (!doKeyDown((int)wParam, lParam))
			{
				return 0;
			}
			break;
		case WM_KEYUP:
			if (!doKeyUp((int)wParam, lParam))
			{
				return 0;
			}
			break;
		case WM_DROPFILES:
			if (!doDropFiles((HDROP)wParam))
			{
				return 0;
			}
			break;
		case WM_SYSCOLORCHANGE:
			doSystemColorChange();
			break;
		case WM_CHAR:
			if (!doChar((TCHAR)wParam, lParam))
			{
				return 0;
			}
			break;
		case WM_TIMER:
			if (!lParam && !doTimer((UINT_PTR)wParam))
			{
				return 0;
			}
			break;
		case WM_HELP:
			if (doHelp((LPHELPINFO)lParam))
			{
				return TRUE;
			}
			break;
		case WM_MENUSELECT:
			if (!doMenuSelect(LOWORD(wParam), HIWORD(wParam), (HMENU)lParam))
			{
				return 0;
			}
			break;
		case WM_ENTERMENULOOP:
			if (!doEnterMenuLoop(wParam ? true : false))
			{
				return 0;
			}
			break;
		case WM_EXITMENULOOP:
			if (!doExitMenuLoop(wParam ? true : false))
			{
				return 0;
			}
			break;
		case WM_INITMENUPOPUP:
			if (!doInitMenuPopup((HMENU)wParam, (UINT)LOWORD(lParam),
				(BOOL)HIWORD(lParam)))
			{
				return 0;
			}
			break;
		case WM_DRAWITEM:
			if (doDrawItem((HWND)wParam, (LPDRAWITEMSTRUCT)lParam))
			{
				return TRUE;
			}
			break;
		case WM_THEMECHANGED:
			if (!doThemeChanged())
			{
				return 0;
			}
			break;
		case WM_NOTIFY:
			return doNotify((int)(short)LOWORD(wParam), (LPNMHDR)lParam);
			break;
		default:
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CUIWindow::staticWindowProc(HWND hWnd, UINT message,
											WPARAM wParam, LPARAM lParam)
{
	CUIWindow* cuiWindow = (CUIWindow*)GetWindowLongPtrUC(hWnd, GWLP_USERDATA);

	if (!cuiWindow)
	{
		if (message == WM_CREATE || message == WM_NCCREATE)
		{
			CREATESTRUCT* createStruct = (LPCREATESTRUCT)lParam;

			cuiWindow = (CUIWindow*)(createStruct->lpCreateParams);
			if (cuiWindow)
			{
				SetWindowLongPtrUC(hWnd, GWLP_USERDATA, (LONG_PTR)cuiWindow);
			}
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	if (cuiWindow)
	{
		return cuiWindow->windowProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

BOOL CALLBACK CUIWindow::disableNonModalWindow(HWND hWnd, LPARAM hModalDialog)
{
	if (hWnd != (HWND)hModalDialog)
	{
		EnableWindow(hWnd, FALSE);
	}
	return TRUE;
}

BOOL CALLBACK CUIWindow::enableNonModalWindow(HWND hWnd, LPARAM hModalDialog)
{
	if (hWnd != (HWND)hModalDialog)
	{
		EnableWindow(hWnd, TRUE);
	}
	return TRUE;
}

void CUIWindow::runDialogModal(HWND hDlg, bool allowMessages)
{
	DWORD processId = GetCurrentProcessId();
	DWORD dlgProcessId;

	if (hDlg == NULL || GetWindowThreadProcessId(hDlg, &dlgProcessId) == 0 ||
		processId != dlgProcessId)
	{
		// If we get garbage input to this function, we end up disabling
		// ALL windows on the whole screen, instead of just the LDView ones.
		// That makes life difficult on the user.
		return;
	}
	ShowWindow(hDlg, SW_SHOWNORMAL);
	EnumThreadWindows(GetWindowThreadProcessId(hDlg, NULL),
		disableNonModalWindow, (LPARAM)hDlg);
	while (1)
	{
		MSG msg;

		if (!GetMessage(&msg, NULL, 0, 0))
		{
			doDialogClose(hDlg);
		}
		if (allowMessages || msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd))
		{
			if (!IsDialogMessage(hDlg, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
			}
		}
		else
		{
			if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST &&
				msg.message != WM_MOUSEMOVE && msg.message != WM_ACTIVATE)
			{
				MessageBeep(MB_OK);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (!IsWindow(hDlg) || !IsWindowVisible(hDlg))
		{
			break;
		}
	}
}

bool CUIWindow::flushDialogModal(HWND hDlg)
{
	return flushModal(hDlg, true);
}

void CUIWindow::processModalMessage(MSG msg)
{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

bool CUIWindow::flushModal(HWND hWnd, bool isDialog, int maxFlush)
{
	int i = maxFlush;
	MSG lastMsg = {0};
	bool firstMessage = true;

	if (!IsWindowVisible(hWnd))
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	EnumThreadWindows(GetWindowThreadProcessId(hWnd, NULL),
		disableNonModalWindow, (LPARAM)hWnd);
	while (1)
	{
		MSG msg;

		if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			break;
		}
		if (msg.message == WM_QUIT)
		{
			return false;
		}
//		printf("0x%08x\n", msg.hwnd);
//		printMessageName(msg.message);
		if (msg.hwnd == hWnd || IsChild(hWnd, msg.hwnd))
		{
			if (!isDialog || !IsDialogMessage(hWnd, &msg))
			{
				// This message checking code is a hack, but for some reason
				// the toolbar goes into an infinite paint loop if I don't do
				// it.
				if (firstMessage || lastMsg.hwnd != msg.hwnd ||
					lastMsg.message != msg.message)
				{
					processModalMessage(msg);
				}
				else
				{
					if (msg.message == WM_PAINT)
					{
						// More of the hack: break the infinite paint loop by
						// claiming it's no longer needed.
						ValidateRect(msg.hwnd, NULL);
					}
				}
			}
			else
			{
			}
		}
		else
		{
			if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST &&
				msg.message != WM_MOUSEMOVE && msg.message != WM_ACTIVATE)
			{
				MessageBeep(MB_OK);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (!IsWindow(hWnd) || !IsWindowVisible(hWnd))
		{
			break;
		}
		if (i-- == 0)
		{
			break;
		}
		firstMessage = false;
		lastMsg = msg;
	}
	return true;
}

void CUIWindow::doDialogClose(HWND hDlg)
{
	EnumThreadWindows(GetWindowThreadProcessId(hDlg, NULL),
		enableNonModalWindow, (LPARAM)hDlg);
	ShowWindow(hDlg, SW_HIDE);
}

BOOL CUIWindow::doDialogThemeChanged(void)
{
	return TRUE;
}

BOOL CUIWindow::doDialogCommand(HWND, int, int, HWND)
{
	return FALSE;
}

BOOL CUIWindow::doDialogVScroll(HWND, int, int, HWND)
{
	return FALSE;
}

BOOL CUIWindow::doDialogHScroll(HWND, int, int, HWND)
{
	return FALSE;
}

BOOL CUIWindow::doDialogInit(HWND, HWND, LPARAM)
{
	return TRUE;
}

BOOL CUIWindow::doDialogNotify(HWND, int, LPNMHDR)
{
	return FALSE;
}

BOOL CUIWindow::doDialogSize(HWND, WPARAM, int, int)
{
	return FALSE;
}

BOOL CUIWindow::doDialogGetMinMaxInfo(HWND, LPMINMAXINFO)
{
	return FALSE;
}

BOOL CUIWindow::doDialogChar(HWND, TCHAR, LPARAM)
{
	return FALSE;
}

BOOL CUIWindow::doDialogHelp(HWND, LPHELPINFO)
{
	return FALSE;
}

BOOL CUIWindow::doDialogCtlColorStatic(HDC /*hdcStatic*/, HWND)
{
	return FALSE;
}

BOOL CUIWindow::doDialogCtlColorBtn(HDC /*hdcStatic*/, HWND)
{
	return FALSE;
}

INT_PTR CUIWindow::dialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
//	printf("CUIWindow::dialogProc(0x%04X, 0x%04X, 0x%08X, 0x%08X)\n", hDlg,
//		message, wParam, lParam);
	BOOL retValue;

	switch (message)
	{
		case WM_INITDIALOG:
			return doDialogInit(hDlg, (HWND)wParam, lParam);
			break;
		case WM_CLOSE:
			doDialogClose(hDlg);
			break;
		case WM_COMMAND:
			return doDialogCommand(hDlg, (int)(short)LOWORD(wParam),
				(int)(short)HIWORD(wParam), (HWND)lParam);
			break;
		case WM_VSCROLL:
			return doDialogVScroll(hDlg, (int)(short)LOWORD(wParam),
				(int)(short)HIWORD(wParam), (HWND)lParam);
			break;
		case WM_HSCROLL:
			return doDialogHScroll(hDlg, (int)(short)LOWORD(wParam),
				(int)(short)HIWORD(wParam), (HWND)lParam);
			break;
		case WM_NOTIFY:
			return doDialogNotify(hDlg, (int)(short)LOWORD(wParam),
				(LPNMHDR)lParam);
			break;
		case WM_SIZE:
			return doDialogSize(hDlg, wParam, (int)(short)LOWORD(lParam),
				(int)(short)HIWORD(lParam));
			break;
		case WM_GETMINMAXINFO:
			return doDialogGetMinMaxInfo(hDlg, (LPMINMAXINFO)lParam);
			break;
		case WM_CHAR:
			return doDialogChar(hDlg, (TCHAR)wParam, lParam);
			break;
		case WM_HELP:
			return doDialogHelp(hDlg, (LPHELPINFO)lParam);
			break;
		case WM_THEMECHANGED:
			return doDialogThemeChanged();
			break;
		case WM_CTLCOLORSTATIC:
			retValue = doDialogCtlColorStatic((HDC)wParam, (HWND)lParam);
			if (retValue)
			{
				return retValue;
			}
			break;
		case WM_CTLCOLORBTN:
			retValue = doDialogCtlColorBtn((HDC)wParam, (HWND)lParam);
			if (retValue)
			{
				return retValue;
			}
			break;
		default:
			return FALSE;
			break;
	}
	return FALSE;
}

INT_PTR CALLBACK CUIWindow::staticDialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	CUIWindow* cuiWindow;

//	debugPrintf("CUIWindow::staticDialogProc\n");
//	printMessageName(message);
	if (message == WM_INITDIALOG)
	{
		cuiWindow = (CUIWindow*)lParam;
		SetWindowLongPtrUC(hDlg, DWLP_USER, lParam);
	}
	else
	{
		cuiWindow = (CUIWindow*)GetWindowLongPtrUC(hDlg, DWLP_USER);
	}
	if (cuiWindow)
	{
		return cuiWindow->dialogProc(hDlg, message, wParam, lParam);
	}
	else
	{
		return FALSE;
	}
}

void CUIWindow::populateAppVersion(void)
{
	if (!appVersionPopulated)
	{
		char appFilename[1024];

		if (GetModuleFileName(NULL, appFilename, sizeof(appFilename)) > 0)
		{
			DWORD zero;
			DWORD versionInfoSize = GetFileVersionInfoSize(appFilename, &zero);
			
			if (versionInfoSize > 0)
			{
				BYTE *versionInfo = new BYTE[versionInfoSize];

				if (GetFileVersionInfo(appFilename, NULL, versionInfoSize,
					versionInfo))
				{
					VS_FIXEDFILEINFO *fixedVersionInfo;
					UINT versionLength;

					if (VerQueryValue(versionInfo, "\\",
						(void**)&fixedVersionInfo, &versionLength))
					{
						appVersionMS = fixedVersionInfo->dwProductVersionMS;
						appVersionLS = fixedVersionInfo->dwProductVersionLS;
						appVersionPopulated = true;
					}
				}
				delete versionInfo;
			}
		}
	}
}

bool CUIWindow::loadLanguageModule(LCID lcid, bool includeSub)
{
	char localeInfo[1024];
	LCTYPE lcType = LOCALE_SENGLANGUAGE;

	if (includeSub)
	{
		lcType = LOCALE_SLANGUAGE;
	}
	//GetLocaleInfo(lcid, LOCALE_SISO639LANGNAME, localeInfo,
	//	sizeof(localeInfo));
	//GetLocaleInfo(lcid, LOCALE_SISO3166CTRYNAME, localeInfo,
	//	sizeof(localeInfo));
	if (GetLocaleInfo(lcid, lcType, localeInfo, sizeof(localeInfo)) > 0)
	{
		const char *appName = TCUserDefaults::getAppName();
		char languageModuleName[1024];
		char *nameOverride =
			TCUserDefaults::stringForKey("LanguageModuleName", NULL, false);

		if (strchr(appName, '/'))
		{
			appName = strrchr(appName, '/') + 1;
		}
		if (strchr(appName, '\\'))
		{
			appName = strrchr(appName, '\\') + 1;
		}
		if (nameOverride)
		{
			strcpy(languageModuleName, nameOverride);
			delete nameOverride;
		}
		else
		{
			sprintf(languageModuleName, "%s-%s.dll", appName, localeInfo);
		}
		hLanguageModule = LoadLibrary(languageModuleName);
		if (hLanguageModule)
		{
			DWORD (*getCUIAppLanguageModuleVersion)(const char *, int) =
				(DWORD (*)(const char *, int))GetProcAddress(hLanguageModule,
				"getCUIAppLanguageModuleVersion");

			if (getCUIAppLanguageModuleVersion)
			{
				DWORD dllVersionMS;
				DWORD dllVersionLS;

				dllVersionMS = getCUIAppLanguageModuleVersion(appName, 0);
				dllVersionLS = getCUIAppLanguageModuleVersion(appName, 1);
				// Note that we don't care about the least significant version
				// part, so we mask that part out of our comparison.
				if (dllVersionMS != appVersionMS ||
					(dllVersionLS & 0xFFFF0000) != (appVersionLS & 0xFFFF0000))
				{
					char message[1024];

					sprintf(message, "Language module %s found.\n"
						"This language module was created for a different "
						"version of LDView, and therefore cannot be used.",
						languageModuleName);
					MessageBox(NULL, message, "Wrong Language Module",
						MB_ICONWARNING | MB_OK);
					FreeLibrary(hLanguageModule);
					hLanguageModule = NULL;
				}
			}
			else
			{
				FreeLibrary(hLanguageModule);
				hLanguageModule = NULL;
			}
		}
	}
	if (includeSub && hLanguageModule == NULL)
	{
		return loadLanguageModule(lcid, false);
	}
	else
	{
		return hLanguageModule != NULL;
	}
}

void CUIWindow::populateLanguageModule(HINSTANCE hDefaultModule)
{
	if (!hLanguageModule)
	{
		populateAppVersion();
		if (!loadLanguageModule(LOCALE_USER_DEFAULT))
		{
			hLanguageModule = hDefaultModule;
		}
	}
}

HINSTANCE CUIWindow::getLanguageModule(void)
{
	if (!hLanguageModule)
	{
		populateLanguageModule(GetModuleHandle(NULL));
	}
	return hLanguageModule;
}

HWND CUIWindow::createDialog(UCSTR templateName, BOOL asChildWindow,
							 DLGPROC dialogProc, LPARAM lParam)
{
	HWND hWnd;

	if (asChildWindow)
	{
		hWnd = hWindow;
	}
	else
	{
		hWnd = NULL;
	}
	if (!lParam)
	{
		lParam = (LPARAM)this;
	}
	return CreateDialogParamUC(getLanguageModule(), templateName, hWnd,
		dialogProc, lParam);
}

HWND CUIWindow::createDialog(int templateNumber, BOOL asChildWindow,
							 DLGPROC dialogProc, LPARAM lParam)
{
	return createDialog(MAKEINTRESOURCEUC(templateNumber), asChildWindow,
		dialogProc, lParam);
}

HBRUSH CUIWindow::getBackgroundBrush(void)
{
	if (!hBackgroundBrush)
	{
 		hBackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	}
 	return hBackgroundBrush;
//	return (HBRUSH)(COLOR_WINDOW + 1);
}

WNDCLASSEX CUIWindow::getWindowClass(void)
{
	WNDCLASSEX windowClass;

	// Set up and window class
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = windowClassStyle;
	windowClass.lpfnWndProc = staticWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = getLanguageModule();
	windowClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = getBackgroundBrush();
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName();
	windowClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	return windowClass;
}

void CUIWindow::registerWindowClass(void)
{
	WNDCLASSEX windowClass;

	if (!GetClassInfoEx(getLanguageModule(), windowClassName(), &windowClass))
	{
		windowClass = getWindowClass();
		RegisterClassEx(&windowClass);
	}
}

void CUIWindow::setTitle(CUCSTR value)
{
	delete windowTitle;
	windowTitle = copyString(value);
	if (hWindow)
	{
		setWindowTextUC(hWindow, windowTitle);
	}
}

BOOL CUIWindow::createMainWindow(void)
{
	SIZE decorationSize = getDecorationSize();
	int dx = decorationSize.cx;
	int dy = decorationSize.cy;
	ucstring className;

	mbstoucstring(className, windowClassName());
	hWindow = createWindowExUC(exWindowStyle, className.c_str(), windowTitle,
		windowStyle, x, y, width + dx, height + dy, NULL, hWindowMenu,
		getLanguageModule(), this);
	if (!hWindow)
	{
		DWORD error = GetLastError();

		if (error)
		{
			return FALSE;
		}
	}
	SetWindowLongPtrUC(hWindow, GWLP_USERDATA, (LONG_PTR)this);
	return TRUE;
}

BOOL CUIWindow::createSubWindow(void)
{
	SIZE decorationSize = getDecorationSize();
	int dx = decorationSize.cx;
	int dy = decorationSize.cy;
	ucstring className;

	mbstoucstring(className, windowClassName());
	dx = 0;
	dy = 0;
	if (!hParentWindow)
	{
		hParentWindow = parentWindow->hWindow;
	}
	hWindow = createWindowExUC(exWindowStyle, className.c_str(), windowTitle,
		windowStyle, x - dx / 2, y - dy / 2, width, height, hParentWindow,
		hWindowMenu, getLanguageModule(), this);
	if (!hWindow)
	{
		DWORD error = GetLastError();

		if (error)
		{
			return FALSE;
		}
	}
	SetWindowLongPtrUC(hWindow, GWLP_USERDATA, (LONG_PTR)this);
	return TRUE;
}

BOOL CUIWindow::createWindow(void)
{
	if (parentWindow || hParentWindow)
	{
		return createSubWindow();
	}
	else
	{
		return createMainWindow();
	}
}

void CUIWindow::showWindow(int nCmdShow)
{
	ShowWindow(hWindow, nCmdShow);
	UpdateWindow(hWindow);
}

void CUIWindow::maximize(void)
{
//	printf("About to maximize...\n");
	showWindow(SW_MAXIMIZE);
//	printf("Maximized.\n");
}

void CUIWindow::minimize(void)
{
	showWindow(SW_MINIMIZE);
}

void CUIWindow::restore(void)
{
	showWindow(SW_RESTORE);
}

void CUIWindow::addChild(CUIWindow* childWindow)
{
	if (numChildren < CUI_MAX_CHILDREN)
	{
		children[numChildren] = childWindow;
		numChildren++;
	}
}

void CUIWindow::removeChild(CUIWindow* childWindow)
{
	int i;

	for (i = 0; i < numChildren && children[i] != childWindow; i++)
	{
	}
	for (i++; i < numChildren; i++)
	{
		children[i-1] = children[i];
	}
	numChildren--;
}

void CUIWindow::setupDialogSlider(HWND hDlg, int controlId, short min,
								  short max, WORD frequency, int value)
{
	SendDlgItemMessage(hDlg, controlId, TBM_SETRANGE, TRUE,
		(LPARAM)MAKELONG(min, max));
	if (GetWindowLong(GetDlgItem(hDlg, controlId), GWL_STYLE) & TBS_AUTOTICKS)
	{
		SendDlgItemMessage(hDlg, controlId, TBM_SETTICFREQ, frequency, 0);
	}
	SendDlgItemMessage(hDlg, controlId, TBM_SETPOS, 1, value);
}

bool CUIWindow::copyToClipboard(const char *value)
{
	size_t len = strlen(value) + 1;
	HGLOBAL hBuf = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len);

	if (hBuf)
	{
		char *buf = (char*)GlobalLock(hBuf);

		strcpy(buf, value);
		GlobalUnlock(hBuf);
		if (OpenClipboard(hWindow))
		{
			EmptyClipboard();
			SetClipboardData(CF_OEMTEXT, hBuf);
			CloseClipboard();
			return true;
		}
		else
		{
			GlobalFree(hBuf);
		}
	}
	return false;
}

#ifndef TC_NO_UNICODE
bool CUIWindow::copyToClipboard(const wchar_t *value)
{
	size_t len = wcslen(value) + 1;
	HGLOBAL hBuf = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
		len * sizeof(wchar_t));

	if (hBuf)
	{
		wchar_t *buf = (wchar_t*)GlobalLock(hBuf);

		wcscpy(buf, value);
		GlobalUnlock(hBuf);
		if (OpenClipboard(hWindow))
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hBuf);
			CloseClipboard();
			return true;
		}
		else
		{
			GlobalFree(hBuf);
		}
	}
	return false;
}
#endif // TC_NO_UNICODE

HMENU CUIWindow::findSubMenu(HMENU hParentMenu, int subMenuIndex, int *index)
{
	int i;
	int count = GetMenuItemCount(hParentMenu);
	int foundCount = 0;
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_DATA | MIIM_SUBMENU;
	for (i = 0; i < count; i++)
	{
		GetMenuItemInfo(hParentMenu, i, TRUE, &itemInfo);
		if (itemInfo.hSubMenu)
		{
			if (foundCount == subMenuIndex)
			{
				if (index)
				{
					*index = i;
				}
				return itemInfo.hSubMenu;
			}
			foundCount++;
		}
	}
	return NULL;
}

// Note: static method.
HWND CUIWindow::createWindowExUC(DWORD dwExStyle, CUCSTR lpClassName,
	CUCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
	int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
	LPVOID lpParam)
{
#ifdef TC_NO_UNICODE
	return ::CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x,
		y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
#else // TC_NO_UNICODE
	return ::CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x,
		y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
#endif // TC_NO_UNICODE
}

// Note: static method.
BOOL CUIWindow::setWindowTextUC(HWND hWnd, CUCSTR text)
{
#ifdef TC_NO_UNICODE
	return ::SetWindowTextA(hWnd, text);
#else // TC_NO_UNICODE
	return ::SetWindowTextW(hWnd, text);
#endif // TC_NO_UNICODE
}

// Note: static method.
LRESULT CUIWindow::sendMessageUC(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	UINT actualMessage = ucMessages[uMsg];

	assert(actualMessage);
#ifdef TC_NO_UNICODE
	return ::SendMessageA(hWnd, actualMessage, wParam, lParam);
#else // TC_NO_UNICODE
	return ::SendMessageW(hWnd, actualMessage, wParam, lParam);
#endif // TC_NO_UNICODE
}

// Note: static method.
LRESULT CUIWindow::sendDlgItemMessageUC(
	HWND hDlg,
	int nIDDlgItem,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	UINT actualMessage = ucMessages[uMsg];

	assert(actualMessage);
#ifdef TC_NO_UNICODE
	return ::SendDlgItemMessageA(hDlg, nIDDlgItem, actualMessage, wParam,
		lParam);
#else // TC_NO_UNICODE
	return ::SendDlgItemMessageW(hDlg, nIDDlgItem, actualMessage, wParam,
		lParam);
#endif // TC_NO_UNICODE
}

// Note: static method.
int CUIWindow::messageBoxUC(
	HWND hWnd,
	CUCSTR lpText,
	CUCSTR lpCaption,
	UINT uType)
{
#ifdef TC_NO_UNICODE
	return ::MessageBoxA(hWnd, lpText, lpCaption, uType);
#else // TC_NO_UNICODE
	return ::MessageBoxW(hWnd, lpText, lpCaption, uType);
#endif // TC_NO_UNICODE
}

// Note: static method.
HWND CUIWindow::createStatusWindowUC(
	LONG style,
	CUCSTR lpszText,
	HWND hwndParent,
	UINT wID)
{
#ifdef TC_NO_UNICODE
	return ::CreateStatusWindowA(style, lpszText, hwndParent, wID);
#else // TC_NO_UNICODE
	return ::CreateStatusWindowW(style, lpszText, hwndParent, wID);
#endif // TC_NO_UNICODE
}

// Note: static method.
BOOL CUIWindow::insertMenuItemUC(
	HMENU hmenu,
	UINT item,
	BOOL fByPosition,
	MENUITEMINFOUC *lpmi)
{
#ifdef TC_NO_UNICODE
	return ::InsertMenuItemA(hmenu, item, fByPosition, lpmi);
#else // TC_NO_UNICODE
	return ::InsertMenuItemW(hmenu, item, fByPosition, lpmi);
#endif // TC_NO_UNICODE
}

// Note: static method.
BOOL CUIWindow::getOpenFileNameUC(OPENFILENAMEUC *lpofn)
{
#ifdef TC_NO_UNICODE
	return ::GetOpenFileNameA(lpofn);
#else // TC_NO_UNICODE
	return ::GetOpenFileNameW(lpofn);
#endif // TC_NO_UNICODE
}


// Note: static method
BOOL CUIWindow::getTextExtentPoint32UC(
	HDC hdc,
	CUCSTR lpString,
	int cbString,
	LPSIZE lpSize)
{
#ifdef TC_NO_UNICODE
	return ::GetTextExtentPoint32A(hdc, lpString, cbString, lpSize);
#else // TC_NO_UNICODE
	return ::GetTextExtentPoint32W(hdc, lpString, cbString, lpSize);
#endif // TC_NO_UNICODE
}

// Note: static method.
BOOL CUIWindow::screenToClient(HWND hWnd, RECT *rect)
{
	POINT tempPoint = {rect->left, rect->top};
	if (::ScreenToClient(hWnd, &tempPoint))
	{
		rect->left = tempPoint.x;
		rect->top = tempPoint.y;
		tempPoint.x = rect->right;
		tempPoint.y = rect->bottom;
		if (::ScreenToClient(hWnd, &tempPoint))
		{
			rect->right = tempPoint.x;
			rect->bottom = tempPoint.y;
			return TRUE;
		}
	}
	return FALSE;
}

// Note: static method.
BOOL CUIWindow::clientToScreen(HWND hWnd, RECT *rect)
{
	POINT tempPoint = {rect->left, rect->top};
	if (::ClientToScreen(hWnd, &tempPoint))
	{
		rect->left = tempPoint.x;
		rect->top = tempPoint.y;
		tempPoint.x = rect->right;
		tempPoint.y = rect->bottom;
		if (::ClientToScreen(hWnd, &tempPoint))
		{
			rect->right = tempPoint.x;
			rect->bottom = tempPoint.y;
			return TRUE;
		}
	}
	return FALSE;
}

// Note: static method
void CUIWindow::setMenuCheck(
	HMENU hParentMenu,
	UINT uItem,
	bool checked,
	bool radio)
{
	MENUITEMINFO itemInfo;
	//char title[256];

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_STATE;// | MIIM_TYPE;
	//itemInfo.fType = MFT_STRING;
	//itemInfo.dwTypeData = title;
	//itemInfo.cch = 256;
	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
	if (checked)
	{
		itemInfo.fState |= MFS_CHECKED;
		itemInfo.fState &= ~MFS_UNCHECKED;
	}
	else
	{
		itemInfo.fState &= ~MFS_CHECKED;
		itemInfo.fState |= MFS_UNCHECKED;
	}
	itemInfo.fType = MFT_STRING;
	if (radio)
	{
		itemInfo.fType |= MFT_RADIOCHECK;
	}
	SetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
}

// Note: static method
void CUIWindow::setMenuRadioCheck(HMENU hParentMenu, UINT uItem, bool checked)
{
	setMenuCheck(hParentMenu, uItem, checked, true);
}

// Note: static method
bool CUIWindow::getMenuCheck(HMENU hParentMenu, UINT uItem)
{
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_STATE;
	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
	return (itemInfo.fState & MFS_CHECKED) != 0;
}

void CUIWindow::setMenuItemsEnabled(HMENU hMenu, bool enabled)
{
	int i;
	int count = GetMenuItemCount(hMenu);

	for (i = 0; i < count; i++)
	{
		setMenuEnabled(hMenu, i, enabled, TRUE);
	}
}

void CUIWindow::setMenuEnabled(
	HMENU hParentMenu,
	int itemID,
	bool enabled,
	BOOL byPosition)
{
	MENUITEMINFO itemInfo;
	//BYTE tbState = 0;

	itemInfo.cbSize = sizeof(itemInfo);
	itemInfo.fMask = MIIM_STATE;
	if (GetMenuItemInfo(hParentMenu, itemID, byPosition, &itemInfo))
	{
		if (enabled)
		{
			itemInfo.fState &= ~MFS_DISABLED;
		}
		else
		{
			itemInfo.fState |= MFS_DISABLED;
		}
		itemInfo.fMask = MIIM_STATE;
		SetMenuItemInfo(hParentMenu, itemID, byPosition, &itemInfo);
	}
}

// Note: static method
void CUIWindow::windowGetText(HWND hWnd, ucstring &text)
{
	text.resize(SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0));
	sendMessageUC(hWnd, WM_GETTEXT, (WPARAM)text.size() + 1, (LPARAM)&text[0]);
}

#ifndef TC_NO_UNICODE

// Note: static method
void CUIWindow::windowGetText(HWND hWnd, std::string &text)
{
	text.resize(SendMessageA(hWnd, WM_GETTEXTLENGTH, 0, 0));
	SendMessageA(hWnd, WM_GETTEXT, (WPARAM)text.size() + 1, (LPARAM)&text[0]);
}
#endif // TC_NO_UNICODE

// Note: static method
bool CUIWindow::checkGet(HWND hWnd)
{
	return SendMessage(hWnd, BM_GETCHECK, 0, 0) != 0;
}

// Note: static method
void CUIWindow::checkSet(HWND hWnd, bool value)
{
	SendMessage(hWnd, BM_SETCHECK, value, 0);
}

void CUIWindow::positionResizeGrip(HWND hSizeGrip)
{
	RECT clientRect;
	GetClientRect(hWindow, &clientRect);
	positionResizeGrip(hSizeGrip, clientRect.right, clientRect.bottom);
	RedrawWindow(hWindow, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void CUIWindow::positionResizeGrip(
	HWND hSizeGrip,
	int parentWidth,
	int parentHeight)
{
	int sbSize = GetSystemMetrics(SM_CXVSCROLL);

	MoveWindow(hSizeGrip, parentWidth - sbSize, parentHeight - sbSize, sbSize,
		sbSize, FALSE);
}

void CUIWindow::writeAutosaveInfo(
	int saveX,
	int saveY,
	int saveWidth,
	int saveHeight,
	int saveMaximized)
{
	if (autosaveName != NULL)
	{
		char info[1024];

		sprintf(info, "%d %d %d %d %d", saveX, saveY, saveWidth, saveHeight,
			saveMaximized);
		TCUserDefaults::setStringForKey(info, autosaveName, false);
	}
}

bool CUIWindow::readAutosaveInfo(
	int &saveX,
	int &saveY,
	int &saveWidth,
	int &saveHeight,
	int &saveMaximized)
{
	bool retValue = false;

	if (autosaveName != NULL)
	{
		char *info = TCUserDefaults::stringForKey(autosaveName, NULL, false);

		if (info != NULL)
		{
			if (sscanf(info, "%d %d %d %d %d", &saveX, &saveY, &saveWidth,
				&saveHeight, &saveMaximized) == 5)
			{
				retValue = true;
			}
			delete info;
		}
	}
	return retValue;
}

void CUIWindow::setAutosaveName(const char *value)
{
	int saveX, saveY, saveWidth, saveHeight, saveMaximized;

	autosaveName = new char[strlen(value) + 32];
	sprintf(autosaveName, "WindowAutosave/%s", value);
	if (readAutosaveInfo(saveX, saveY, saveWidth, saveHeight,
		saveMaximized))
	{
		RECT testRect = { 0, 0, 10, 10 };
		int adjustWidth;
		int adjustHeight;

		AdjustWindowRectEx(&testRect, GetWindowLong(hWindow, GWL_STYLE), FALSE,
			GetWindowLong(hWindow, GWL_EXSTYLE));
		adjustWidth = testRect.right - testRect.left - 10;
		adjustHeight = testRect.bottom - testRect.top - 10;
		if (saveWidth < minWidth + adjustWidth)
		{
			saveWidth = minWidth + adjustWidth;
		}
		if (saveHeight < minHeight + adjustHeight)
		{
			saveHeight = minHeight + adjustHeight;
		}
		RECT newRect = { saveX, saveY, saveX + saveWidth, saveY + saveHeight };
		HMONITOR hMonitor = MonitorFromRect(&newRect, MONITOR_DEFAULTTONULL);
		bool okRect = true;
		if (hMonitor == NULL)
		{
			// There is no longer a monitor where the window was last displayed.
			okRect = false;
		}
		else
		{
			MONITORINFO monitorInfo;
			memset(&monitorInfo, 0, sizeof(monitorInfo));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			if (GetMonitorInfo(hMonitor, &monitorInfo))
			{
				RECT visibleRect;
				IntersectRect(&visibleRect, &newRect, &monitorInfo.rcWork);
				int totalArea = (newRect.right - newRect.left) *
					(newRect.bottom - newRect.top);
				int visibleArea = (visibleRect.right - visibleRect.left) *
					(visibleRect.bottom - visibleRect.top);
				if (visibleArea * 2 < totalArea)
				{
					// Less than half the window is visible.
					okRect = false;
				}
			}
			else
			{
				// We shouldn't get here; I think it means that the monitor that
				// was originally found for the rect went away between when we
				// asked for the rect's monitor and when we asked for info about
				// that monitor.
				okRect = false;
			}
		}
		if (!okRect)
		{
			// The saved window rect is less than half visible.
			// Use the saved window size, but use the default location.
			GetWindowRect(hWindow, &newRect);
			saveX = newRect.left;
			saveY = newRect.top;
		}
		MoveWindow(hWindow, saveX, saveY, saveWidth, saveHeight,
			saveMaximized == 0);
		if (saveMaximized)
		{
			ShowWindow(hWindow, SW_MAXIMIZE);
		}
	}
}

#ifndef TC_NO_UNICODE
// Note: static method
void CUIWindow::addFileType(
	char *fileTypes,
	const char *description,
	const char *filter)
{
	char* spot = fileTypes;

	for (spot = fileTypes; spot[0] != 0 || spot[1] != 0; spot++)
		;
	if (spot != fileTypes)
	{
		spot++;
	}
	strcpy(spot, description);
	spot += strlen(description) + 1;
	strcpy(spot, filter);
	spot += strlen(filter) + 1;
	*spot = 0;
}
#endif // TC_NO_UNICODE

// Note: static method
void CUIWindow::addFileType(
	UCSTR fileTypes,
	CUCSTR description,
	CUCSTR filter)
{
	UCSTR spot = fileTypes;

	for (spot = fileTypes; spot[0] != 0 || spot[1] != 0; spot++)
		;
	if (spot != fileTypes)
	{
		spot++;
	}
	ucstrcpy(spot, description);
	spot += ucstrlen(description) + 1;
	ucstrcpy(spot, filter);
	spot += ucstrlen(filter) + 1;
	*spot = 0;
}

// Note: static method
int CUIWindow::getOpenFilenameSize(bool uc)
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(osvi);
#pragma warning(push)
#pragma warning(disable:4996)
	GetVersionEx(&osvi);
#pragma warning(pop)
	if (osvi.dwMajorVersion < 5)
	{
		if (uc)
		{
			return sizeof(OPENFILENAME_NT4UC);
		}
		else
		{
			return sizeof(OPENFILENAME_NT4A);
		}
	}
	else
	{
		if (uc)
		{
			return sizeof(OPENFILENAMEUC);
		}
		else
		{
			return sizeof(OPENFILENAMEA);
		}
	}
}

// Note: static method
void CUIWindow::comboAddString(HWND hWnd, CUCSTR string)
{
	sendMessageUC(hWnd, CB_ADDSTRING, 0, (LPARAM)string);
}

// Note: static method
LRESULT CUIWindow::comboSelectItem(HWND hWnd, int index)
{
	return SendMessage(hWnd, CB_SETCURSEL, (WPARAM)index, 0);
}

// Note: static method
int CUIWindow::comboGetSelectedItem(HWND hWnd)
{
	return (int)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}

// Note: static method
const std::string CUIWindow::notificationName(UINT code)
{
	static char buf[128];

	switch (code)
	{
	case TBN_GETBUTTONINFOA:
		return "TBN_GETBUTTONINFOA";
	case TBN_BEGINDRAG:
		return "TBN_BEGINDRAG";
	case TBN_ENDDRAG:
		return "TBN_ENDDRAG";
	case TBN_BEGINADJUST:
		return "TBN_BEGINADJUST";
	case TBN_ENDADJUST:
		return "TBN_ENDADJUST";
	case TBN_RESET:
		return "TBN_RESET";
	case TBN_QUERYINSERT:
		return "TBN_QUERYINSERT";
	case TBN_QUERYDELETE:
		return "TBN_QUERYDELETE";
	case TBN_TOOLBARCHANGE:
		return "TBN_TOOLBARCHANGE";
	case TBN_CUSTHELP:
		return "TBN_CUSTHELP";
	case TBN_DROPDOWN:
		return "TBN_DROPDOWN";
	case TBN_GETOBJECT:
		return "TBN_GETOBJECT";
	case TBN_HOTITEMCHANGE:
		return "TBN_HOTITEMCHANGE";
	case TBN_DRAGOUT:
		return "TBN_DRAGOUT";
	case TBN_DELETINGBUTTON:
		return "TBN_DELETINGBUTTON";
	case TBN_GETDISPINFOA:
		return "TBN_GETDISPINFOA";
	case TBN_GETDISPINFOW:
		return "TBN_GETDISPINFOW";
	case TBN_GETINFOTIPA:
		return "TBN_GETINFOTIPA";
	case TBN_GETINFOTIPW:
		return "TBN_GETINFOTIPW";
	case TBN_GETBUTTONINFOW:
		return "TBN_GETBUTTONINFOW";
	case TBN_RESTORE:
		return "TBN_RESTORE";
	case TBN_SAVE:
		return "TBN_SAVE";
	case TBN_INITCUSTOMIZE:
		return "TBN_INITCUSTOMIZE";

	case TTN_GETDISPINFOA:
		return "TTN_GETDISPINFOA";
	case TTN_GETDISPINFOW:
		return "TTN_GETDISPINFOW";
	case TTN_SHOW:
		return "TTN_SHOW";
	case TTN_POP:
		return "TTN_POP";
	case TTN_LINKCLICK:
		return "TTN_LINKCLICK";

	case NM_OUTOFMEMORY:
		return "NM_OUTOFMEMORY";
	case NM_CLICK:
		return "NM_CLICK";
	case NM_DBLCLK:
		return "NM_DBLCLK";
	case NM_RETURN:
		return "NM_RETURN";
	case NM_RCLICK:
		return "NM_RCLICK";
	case NM_RDBLCLK:
		return "NM_RDBLCLK";
	case NM_SETFOCUS:
		return "NM_SETFOCUS";
	case NM_KILLFOCUS:
		return "NM_KILLFOCUS";
	case NM_CUSTOMDRAW:
		return "NM_CUSTOMDRAW";
	case NM_HOVER:
		return "NM_HOVER";
	case NM_NCHITTEST:
		return "NM_NCHITTEST";
	case NM_KEYDOWN:
		return "NM_KEYDOWN";
	case NM_RELEASEDCAPTURE:
		return "NM_RELEASEDCAPTURE";
	case NM_SETCURSOR:
		return "NM_SETCURSOR";
	case NM_CHAR:
		return "NM_CHAR";
	case NM_TOOLTIPSCREATED:
		return "NM_TOOLTIPSCREATED";
	case NM_LDOWN:
		return "NM_LDOWN";
	case NM_RDOWN:
		return "NM_RDOWN";
	case NM_THEMECHANGED:
		return "NM_THEMECHANGED";
	default:
		sprintf(buf, "0x%08X", code);
		return buf;
	}
}

HBITMAP CUIWindow::createDIBSection(
	HDC hBitmapDC,
	int bitmapWidth,
	int bitmapHeight,
	int hDPI,
	int vDPI,
	BYTE **bmBuffer)
{
	BITMAPINFO bmi;

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = bitmapWidth;
	bmi.bmiHeader.biHeight = bitmapHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;//roundUp(bitmapWidth * 3, 4) * bitmapHeight;
	bmi.bmiHeader.biXPelsPerMeter = (long)(hDPI * 39.37);
	bmi.bmiHeader.biYPelsPerMeter = (long)(vDPI * 39.37);
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	return CreateDIBSection(hBitmapDC, &bmi, DIB_RGB_COLORS,
		(void**)bmBuffer, NULL, 0);
}

