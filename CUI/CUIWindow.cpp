#include "CUIWindow.h"
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
		   paintStruct(NULL)
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
		   paintStruct(NULL)
{
	init();
}

CUIWindow::CUIWindow(CUIWindow* parentWindow, int x, int y, int width,
					 int height)
		  :windowTitle(copyString(_UC(""))),
		   hInstance(parentWindow->getHInstance()/*(void*)GetWindowLong(parentWindow, GWL_HINSTANCE)*/),
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
		   paintStruct(NULL)
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
		   paintStruct(NULL)
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
	ucMessages[CB_SELECTSTRING] = CB_SELECTSTRING;
#ifdef TC_NO_UNICODE
	ucMessages[SB_SETTIPTEXT] = SB_SETTIPTEXTA;
	ucMessages[SB_GETTIPTEXT] = SB_GETTIPTEXTA;
	ucMessages[SB_SETTEXT] = SB_SETTEXTA;
	ucMessages[SB_GETTEXT] = SB_GETTEXTA;
#else // TC_NO_UNICODE
	ucMessages[SB_SETTIPTEXT] = SB_SETTIPTEXTW;
	ucMessages[SB_GETTIPTEXT] = SB_GETTIPTEXTW;
	ucMessages[SB_SETTEXT] = SB_SETTEXTW;
	ucMessages[SB_GETTEXT] = SB_GETTEXTW;
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

UINT CUIWindow::setTimer(UINT timerID, UINT elapse)
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

LRESULT CUIWindow::doTimer(UINT)
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
		return 0;
	}
}

LRESULT CUIWindow::doMove(int newX, int newY)
{
	x = newX;
	y = newY;
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
	SetWindowLong(hWindow, GWL_USERDATA, (long)NULL);
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

LRESULT CUIWindow::doKeyDown(int, long)
{
	return 1;
}

LRESULT CUIWindow::doKeyUp(int, long)
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
	if (initialized && !parentWindow && !hParentWindow)
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
	char *string = NULL;

	switch (message)
	{
	case WM_NULL:
		string = "WM_NULL";
		break;
	case WM_CREATE:
		string = "WM_CREATE";
		break;
	case WM_DESTROY:
		string = "WM_DESTROY";
		break;
	case WM_MOVE:
		string = "WM_MOVE";
		break;
	case WM_SIZE:
		string = "WM_SIZE";
		break;
	case WM_ACTIVATE:
		string = "WM_ACTIVATE";
		break;
	case WM_SETFOCUS:
		string = "WM_SETFOCUS";
		break;
	case WM_KILLFOCUS:
		string = "WM_KILLFOCUS";
		break;
	case WM_ENABLE:
		string = "WM_ENABLE";
		break;
	case WM_SETREDRAW:
		string = "WM_SETREDRAW";
		break;
	case WM_SETTEXT:
		string = "WM_SETTEXT";
		break;
	case WM_GETTEXT:
		string = "WM_GETTEXT";
		break;
	case WM_GETTEXTLENGTH:
		string = "WM_GETTEXTLENGTH";
		break;
	case WM_PAINT:
		string = "WM_PAINT";
		break;
	case WM_CLOSE:
		string = "WM_CLOSE";
		break;
	case WM_QUERYENDSESSION:
		string = "WM_QUERYENDSESSION";
		break;
	case WM_QUIT:
		string = "WM_QUIT";
		break;
	case WM_QUERYOPEN:
		string = "WM_QUERYOPEN";
		break;
	case WM_ERASEBKGND:
		string = "WM_ERASEBKGND";
		break;
	case WM_SYSCOLORCHANGE:
		string = "WM_SYSCOLORCHANGE";
		break;
	case WM_ENDSESSION:
		string = "WM_ENDSESSION";
		break;
	case WM_SHOWWINDOW:
		string = "WM_SHOWWINDOW";
		break;
	case WM_SETTINGCHANGE:
		string = "WM_SETTINGCHANGE";
		break;
	case WM_DEVMODECHANGE:
		string = "WM_DEVMODECHANGE";
		break;
	case WM_ACTIVATEAPP:
		string = "WM_ACTIVATEAPP";
		break;
	case WM_FONTCHANGE:
		string = "WM_FONTCHANGE";
		break;
	case WM_TIMECHANGE:
		string = "WM_TIMECHANGE";
		break;
	case WM_CANCELMODE:
		string = "WM_CANCELMODE";
		break;
	case WM_SETCURSOR:
		string = "WM_SETCURSOR";
		break;
	case WM_MOUSEACTIVATE:
		string = "WM_MOUSEACTIVATE";
		break;
	case WM_CHILDACTIVATE:
		string = "WM_CHILDACTIVATE";
		break;
	case WM_QUEUESYNC:
		string = "WM_QUEUESYNC";
		break;
	case WM_GETMINMAXINFO:
		string = "WM_GETMINMAXINFO";
		break;
	case WM_PAINTICON:
		string = "WM_PAINTICON";
		break;
	case WM_ICONERASEBKGND:
		string = "WM_ICONERASEBKGND";
		break;
	case WM_NEXTDLGCTL:
		string = "WM_NEXTDLGCTL";
		break;
	case WM_SPOOLERSTATUS:
		string = "WM_SPOOLERSTATUS";
		break;
	case WM_DRAWITEM:
		string = "WM_DRAWITEM";
		break;
	case WM_MEASUREITEM:
		string = "WM_MEASUREITEM";
		break;
	case WM_DELETEITEM:
		string = "WM_DELETEITEM";
		break;
	case WM_VKEYTOITEM:
		string = "WM_VKEYTOITEM";
		break;
	case WM_CHARTOITEM:
		string = "WM_CHARTOITEM";
		break;
	case WM_SETFONT:
		string = "WM_SETFONT";
		break;
	case WM_GETFONT:
		string = "WM_GETFONT";
		break;
	case WM_SETHOTKEY:
		string = "WM_SETHOTKEY";
		break;
	case WM_GETHOTKEY:
		string = "WM_GETHOTKEY";
		break;
	case WM_QUERYDRAGICON:
		string = "WM_QUERYDRAGICON";
		break;
	case WM_COMPAREITEM:
		string = "WM_COMPAREITEM";
		break;
	case WM_COMPACTING:
		string = "WM_COMPACTING";
		break;
	case WM_COMMNOTIFY:
		string = "WM_COMMNOTIFY";
		break;
	case WM_WINDOWPOSCHANGING:
		string = "WM_WINDOWPOSCHANGING";
		break;
	case WM_WINDOWPOSCHANGED:
		string = "WM_WINDOWPOSCHANGED";
		break;
	case WM_POWER:
		string = "WM_POWER";
		break;
	case WM_COPYDATA:
		string = "WM_COPYDATA";
		break;
	case WM_CANCELJOURNAL:
		string = "WM_CANCELJOURNAL";
		break;
	case WM_NOTIFY:
		string = "WM_NOTIFY";
		break;
	case WM_INPUTLANGCHANGEREQUEST:
		string = "WM_INPUTLANGCHANGEREQUEST";
		break;
	case WM_INPUTLANGCHANGE:
		string = "WM_INPUTLANGCHANGE";
		break;
	case WM_TCARD:
		string = "WM_TCARD";
		break;
	case WM_HELP:
		string = "WM_HELP";
		break;
	case WM_USERCHANGED:
		string = "WM_USERCHANGED";
		break;
	case WM_NOTIFYFORMAT:
		string = "WM_NOTIFYFORMAT";
		break;
	case WM_CONTEXTMENU:
		string = "WM_CONTEXTMENU";
		break;
	case WM_STYLECHANGING:
		string = "WM_STYLECHANGING";
		break;
	case WM_STYLECHANGED:
		string = "WM_STYLECHANGED";
		break;
	case WM_DISPLAYCHANGE:
		string = "WM_DISPLAYCHANGE";
		break;
	case WM_GETICON:
		string = "WM_GETICON";
		break;
	case WM_SETICON:
		string = "WM_SETICON";
		break;
	case WM_NCCREATE:
		string = "WM_NCCREATE";
		break;
	case WM_NCDESTROY:
		string = "WM_NCDESTROY";
		break;
	case WM_NCCALCSIZE:
		string = "WM_NCCALCSIZE";
		break;
	case WM_NCHITTEST:
		string = "WM_NCHITTEST";
		break;
	case WM_NCPAINT:
		string = "WM_NCPAINT";
		break;
	case WM_NCACTIVATE:
		string = "WM_NCACTIVATE";
		break;
	case WM_GETDLGCODE:
		string = "WM_GETDLGCODE";
		break;
	case WM_SYNCPAINT:
		string = "WM_SYNCPAINT";
		break;
	case WM_NCMOUSEMOVE:
		string = "WM_NCMOUSEMOVE";
		break;
	case WM_NCLBUTTONDOWN:
		string = "WM_NCLBUTTONDOWN";
		break;
	case WM_NCLBUTTONUP:
		string = "WM_NCLBUTTONUP";
		break;
	case WM_NCLBUTTONDBLCLK:
		string = "WM_NCLBUTTONDBLCLK";
		break;
	case WM_NCRBUTTONDOWN:
		string = "WM_NCRBUTTONDOWN";
		break;
	case WM_NCRBUTTONUP:
		string = "WM_NCRBUTTONUP";
		break;
	case WM_NCRBUTTONDBLCLK:
		string = "WM_NCRBUTTONDBLCLK";
		break;
	case WM_NCMBUTTONDOWN:
		string = "WM_NCMBUTTONDOWN";
		break;
	case WM_NCMBUTTONUP:
		string = "WM_NCMBUTTONUP";
		break;
	case WM_NCMBUTTONDBLCLK:
		string = "WM_NCMBUTTONDBLCLK";
		break;
	case WM_KEYDOWN:
		string = "WM_KEYDOWN";
		break;
	case WM_KEYUP:
		string = "WM_KEYUP";
		break;
	case WM_CHAR:
		string = "WM_CHAR";
		break;
	case WM_DEADCHAR:
		string = "WM_DEADCHAR";
		break;
	case WM_SYSKEYDOWN:
		string = "WM_SYSKEYDOWN";
		break;
	case WM_SYSKEYUP:
		string = "WM_SYSKEYUP";
		break;
	case WM_SYSCHAR:
		string = "WM_SYSCHAR";
		break;
	case WM_SYSDEADCHAR:
		string = "WM_SYSDEADCHAR";
		break;
	case WM_KEYLAST:
		string = "WM_KEYLAST";
		break;
	case WM_IME_STARTCOMPOSITION:
		string = "WM_IME_STARTCOMPOSITION";
		break;
	case WM_IME_ENDCOMPOSITION:
		string = "WM_IME_ENDCOMPOSITION";
		break;
	case WM_IME_COMPOSITION:
		string = "WM_IME_COMPOSITION";
		break;
	case WM_INITDIALOG:
		string = "WM_INITDIALOG";
		break;
	case WM_COMMAND:
		string = "WM_COMMAND";
		break;
	case WM_SYSCOMMAND:
		string = "WM_SYSCOMMAND";
		break;
	case WM_TIMER:
		string = "WM_TIMER";
		break;
	case WM_HSCROLL:
		string = "WM_HSCROLL";
		break;
	case WM_VSCROLL:
		string = "WM_VSCROLL";
		break;
	case WM_INITMENU:
		string = "WM_INITMENU";
		break;
	case WM_INITMENUPOPUP:
		string = "WM_INITMENUPOPUP";
		break;
	case WM_MENUSELECT:
		string = "WM_MENUSELECT";
		break;
	case WM_MENUCHAR:
		string = "WM_MENUCHAR";
		break;
	case WM_ENTERIDLE:
		string = "WM_ENTERIDLE";
		break;
	case WM_CTLCOLORMSGBOX:
		string = "WM_CTLCOLORMSGBOX";
		break;
	case WM_CTLCOLOREDIT:
		string = "WM_CTLCOLOREDIT";
		break;
	case WM_CTLCOLORLISTBOX:
		string = "WM_CTLCOLORLISTBOX";
		break;
	case WM_CTLCOLORBTN:
		string = "WM_CTLCOLORBTN";
		break;
	case WM_CTLCOLORDLG:
		string = "WM_CTLCOLORDLG";
		break;
	case WM_CTLCOLORSCROLLBAR:
		string = "WM_CTLCOLORSCROLLBAR";
		break;
	case WM_CTLCOLORSTATIC:
		string = "WM_CTLCOLORSTATIC";
		break;
	case WM_MOUSEMOVE:
		string = "WM_MOUSEMOVE";
		break;
	case WM_LBUTTONDOWN:
		string = "WM_LBUTTONDOWN";
		break;
	case WM_LBUTTONUP:
		string = "WM_LBUTTONUP";
		break;
	case WM_LBUTTONDBLCLK:
		string = "WM_LBUTTONDBLCLK";
		break;
	case WM_RBUTTONDOWN:
		string = "WM_RBUTTONDOWN";
		break;
	case WM_RBUTTONUP:
		string = "WM_RBUTTONUP";
		break;
	case WM_RBUTTONDBLCLK:
		string = "WM_RBUTTONDBLCLK";
		break;
	case WM_MBUTTONDOWN:
		string = "WM_MBUTTONDOWN";
		break;
	case WM_MBUTTONUP:
		string = "WM_MBUTTONUP";
		break;
	case WM_MBUTTONDBLCLK:
		string = "WM_MBUTTONDBLCLK";
		break;
	case WM_MOUSEWHEEL:
		string = "WM_MOUSEWHEEL";
		break;
	case WM_PARENTNOTIFY:
		string = "WM_PARENTNOTIFY";
		break;
	case WM_ENTERMENULOOP:
		string = "WM_ENTERMENULOOP";
		break;
	case WM_EXITMENULOOP:
		string = "WM_EXITMENULOOP";
		break;
	case WM_SIZING:
		string = "WM_SIZING";
		break;
	case WM_CAPTURECHANGED:
		string = "WM_CAPTURECHANGED";
		break;
	case WM_MOVING:
		string = "WM_MOVING";
		break;
	case WM_POWERBROADCAST:
		string = "WM_POWERBROADCAST";
		break;
	case WM_DEVICECHANGE:
		string = "WM_DEVICECHANGE";
		break;
	case WM_MDICREATE:
		string = "WM_MDICREATE";
		break;
	case WM_MDIDESTROY:
		string = "WM_MDIDESTROY";
		break;
	case WM_MDIACTIVATE:
		string = "WM_MDIACTIVATE";
		break;
	case WM_MDIRESTORE:
		string = "WM_MDIRESTORE";
		break;
	case WM_MDINEXT:
		string = "WM_MDINEXT";
		break;
	case WM_MDIMAXIMIZE:
		string = "WM_MDIMAXIMIZE";
		break;
	case WM_MDITILE:
		string = "WM_MDITILE";
		break;
	case WM_MDICASCADE:
		string = "WM_MDICASCADE";
		break;
	case WM_MDIICONARRANGE:
		string = "WM_MDIICONARRANGE";
		break;
	case WM_MDIGETACTIVE:
		string = "WM_MDIGETACTIVE";
		break;
	case WM_MDISETMENU:
		string = "WM_MDISETMENU";
		break;
	case WM_ENTERSIZEMOVE:
		string = "WM_ENTERSIZEMOVE";
		break;
	case WM_EXITSIZEMOVE:
		string = "WM_EXITSIZEMOVE";
		break;
	case WM_DROPFILES:
		string = "WM_DROPFILES";
		break;
	case WM_MDIREFRESHMENU:
		string = "WM_MDIREFRESHMENU";
		break;
	case WM_IME_SETCONTEXT:
		string = "WM_IME_SETCONTEXT";
		break;
	case WM_IME_NOTIFY:
		string = "WM_IME_NOTIFY";
		break;
	case WM_IME_CONTROL:
		string = "WM_IME_CONTROL";
		break;
	case WM_IME_COMPOSITIONFULL:
		string = "WM_IME_COMPOSITIONFULL";
		break;
	case WM_IME_SELECT:
		string = "WM_IME_SELECT";
		break;
	case WM_IME_CHAR:
		string = "WM_IME_CHAR";
		break;
	case WM_IME_KEYDOWN:
		string = "WM_IME_KEYDOWN";
		break;
	case WM_IME_KEYUP:
		string = "WM_IME_KEYUP";
		break;
	case WM_MOUSEHOVER:
		string = "WM_MOUSEHOVER";
		break;
	case WM_MOUSELEAVE:
		string = "WM_MOUSELEAVE";
		break;
	case WM_CUT:
		string = "WM_CUT";
		break;
	case WM_COPY:
		string = "WM_COPY";
		break;
	case WM_PASTE:
		string = "WM_PASTE";
		break;
	case WM_CLEAR:
		string = "WM_CLEAR";
		break;
	case WM_UNDO:
		string = "WM_UNDO";
		break;
	case WM_RENDERFORMAT:
		string = "WM_RENDERFORMAT";
		break;
	case WM_RENDERALLFORMATS:
		string = "WM_RENDERALLFORMATS";
		break;
	case WM_DESTROYCLIPBOARD:
		string = "WM_DESTROYCLIPBOARD";
		break;
	case WM_DRAWCLIPBOARD:
		string = "WM_DRAWCLIPBOARD";
		break;
	case WM_PAINTCLIPBOARD:
		string = "WM_PAINTCLIPBOARD";
		break;
	case WM_VSCROLLCLIPBOARD:
		string = "WM_VSCROLLCLIPBOARD";
		break;
	case WM_SIZECLIPBOARD:
		string = "WM_SIZECLIPBOARD";
		break;
	case WM_ASKCBFORMATNAME:
		string = "WM_ASKCBFORMATNAME";
		break;
	case WM_CHANGECBCHAIN:
		string = "WM_CHANGECBCHAIN";
		break;
	case WM_HSCROLLCLIPBOARD:
		string = "WM_HSCROLLCLIPBOARD";
		break;
	case WM_QUERYNEWPALETTE:
		string = "WM_QUERYNEWPALETTE";
		break;
	case WM_PALETTEISCHANGING:
		string = "WM_PALETTEISCHANGING";
		break;
	case WM_PALETTECHANGED:
		string = "WM_PALETTECHANGED";
		break;
	case WM_HOTKEY:
		string = "WM_HOTKEY";
		break;
	case WM_PRINT:
		string = "WM_PRINT";
		break;
	case WM_PRINTCLIENT:
		string = "WM_PRINTCLIENT";
		break;
	}
	if (string)
	{
		printf("message: %s\n", string);
	}
	else
	{
		printf("message: 0x%04X\n", message);
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
			if (!lParam && !doTimer((UINT)wParam))
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
	CUIWindow* cuiWindow = (CUIWindow*)GetWindowLong(hWnd, GWL_USERDATA);

	if (!cuiWindow)
	{
		if (message == WM_CREATE || message == WM_NCCREATE)
		{
			CREATESTRUCT* createStruct = (LPCREATESTRUCT)lParam;

			cuiWindow = (CUIWindow*)(createStruct->lpCreateParams);
			if (cuiWindow)
			{
				SetWindowLong(hWnd, GWL_USERDATA, (long)cuiWindow);
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
					TranslateMessage(&msg);
					DispatchMessage(&msg);
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

BOOL CUIWindow::dialogProc(HWND hDlg, UINT message, WPARAM wParam,
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

BOOL CALLBACK CUIWindow::staticDialogProc(HWND hDlg, UINT message,
										  WPARAM wParam, LPARAM lParam)
{
	CUIWindow* cuiWindow;

//	debugPrintf("CUIWindow::staticDialogProc\n");
//	printMessageName(message);
	if (message == WM_INITDIALOG)
	{
		cuiWindow = (CUIWindow*)lParam;
		SetWindowLong(hDlg, DWL_USER, lParam);
	}
	else
	{
		cuiWindow = (CUIWindow*)GetWindowLong(hDlg, DWL_USER);
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

HWND CUIWindow::createDialog(char* templateName, BOOL asChildWindow,
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
	return CreateDialogParam(getLanguageModule(), templateName, hWnd,
		dialogProc, lParam);
}

HWND CUIWindow::createDialog(int templateNumber, BOOL asChildWindow,
							 DLGPROC dialogProc, LPARAM lParam)
{
	return createDialog(MAKEINTRESOURCE(templateNumber), asChildWindow,
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
	SetWindowLong(hWindow, GWL_USERDATA, (long)this);
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
	SetWindowLong(hWindow, GWL_USERDATA, (long)this);
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
	int len = strlen(value) + 1;
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
	int len = wcslen(value) + 1;
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
