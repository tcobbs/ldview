#include "LDViewWindow.h"
#include <shlwapi.h>
#include "LDVExtensionsSetup.h"
#include "LDViewPreferences.h"
#include "SSModelWindow.h"
#include "ModelTreeDialog.h"
#include "BoundingBoxDialog.h"
#include "MpdDialog.h"
#include "Resource.h"
#include "ToolbarStrip.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCSortedStringArray.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCWebClient.h>
#include <CUI/CUIWindowResizer.h>
#include <CUI/CUIScaler.h>
#include <LDLib/LDLibraryUpdater.h>
#include <LDLib/LDPartsList.h>
#include <LDLoader/LDLPalette.h>
#include <LDLoader/LDLMainModel.h>
#include <TRE/TREMainModel.h>
#include "ModelWindow.h"
#include <TCFoundation/TCMacros.h>
#include <LDLib/LDHtmlInventory.h>
#include "PartsListDialog.h"
#include "LatLonDialog.h"
#include "StepDialog.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#define DOWNLOAD_TIMER 12
#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480

static char monthShortNames[12][4] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

TCStringArray* LDViewWindow::recentFiles = NULL;
TCStringArray* LDViewWindow::extraSearchDirs = NULL;

LDViewWindow::LDViewWindowCleanup LDViewWindow::ldViewWindowCleanup;

void debugOut(char *fmt, ...);

//TbButtonInfo::TbButtonInfo(void)
//	:tooltipText(NULL),
//	commandId(-1),
//	stdBmpId(-1),
//	tbBmpId(-1),
//	state(TBSTATE_ENABLED),
//	style(TBSTYLE_BUTTON)
//{
//}
//
//void TbButtonInfo::dealloc(void)
//{
//	delete tooltipText;
//	TCObject::dealloc();
//}
//
//void TbButtonInfo::setTooltipText(CUCSTR value)
//{
//	if (value != tooltipText)
//	{
//		delete tooltipText;
//		tooltipText = copyString(value);
//	}
//}
//
//int TbButtonInfo::getBmpId(int stdBitmapStartId, int tbBitmapStartId)
//{
//	if (stdBmpId == -1)
//	{
//		return tbBitmapStartId + tbBmpId;
//	}
//	else
//	{
//		return stdBitmapStartId + stdBmpId;
//	}
//}

LDViewWindow::LDViewWindowCleanup::~LDViewWindowCleanup(void)
{
	if (LDViewWindow::recentFiles)
	{
		LDViewWindow::recentFiles->release();
		LDViewWindow::recentFiles = NULL;
	}
	TCObject::release(LDViewWindow::extraSearchDirs);
	LDViewWindow::extraSearchDirs = NULL;
}

LDViewWindow::LDViewWindow(CUCSTR windowTitle, HINSTANCE hInstance, int x,
						   int y, int width, int height):
CUIWindow(windowTitle, hInstance, x, y, width, height),
modelWindow(NULL),
toolbarStrip(NULL),
hAboutWindow(NULL),
hLDrawDirWindow(NULL),
hOpenGLInfoWindow(NULL),
hExtraDirsWindow(NULL),
hExtraDirsImageList(NULL),
hStatusBar(NULL),
//hToolbar(NULL),
//hDeactivatedTooltip(NULL),
userLDrawDir(NULL),
fullScreen(false),
fullScreenActive(false),
switchingModes(false),
searchDirsInitialized(false),
videoModes(NULL),
numVideoModes(0),
currentVideoModeIndex(-1),
showStatusBarOverride(false),
skipMinimize(false),
screenSaver(false),
originalMouseX(-999999),
originalMouseY(-999999),
hFileMenu(NULL),
hViewMenu(NULL),
hToolsMenu(NULL),
loading(FALSE),
openGLInfoWindoResizer(NULL),
hOpenGLStatusBar(NULL),
hExamineIcon(NULL),
hFlythroughIcon(NULL),
hWalkIcon(NULL),
#ifndef TC_NO_UNICODE
hMonitor(NULL),
#endif // TC_NO_UNICODE
#if defined(USE_CPP11) || !defined(_NO_BOOST)
hLibraryUpdateWindow(NULL),
libraryUpdater(NULL),
#endif // !_NO_BOOST
productVersion(NULL),
legalCopyright(NULL),
//stdBitmapStartId(-1),
//tbBitmapStartId(-1),
prefs(NULL),
drawWireframe(false),
examineLatLong(TCUserDefaults::longForKey(EXAMINE_MODE_KEY,
			   LDrawModelViewer::EMFree, false) == LDrawModelViewer::EMLatLong),
initialShown(false),
modelTreeDialog(NULL),
boundingBoxDialog(NULL),
mpdDialog(NULL)
{
	CUIThemes::init();
	if (CUIThemes::isThemeLibLoaded())
	{
		if (TCUserDefaults::boolForKey(VISUAL_STYLE_ENABLED_KEY, true, false))
		{
			CUIThemes::setThemeAppProperties(STAP_ALLOW_NONCLIENT |
				STAP_ALLOW_CONTROLS);
		}
		else
		{
			CUIThemes::setThemeAppProperties(STAP_ALLOW_NONCLIENT);
		}
	}
	loadSettings();
	standardWindowStyle = windowStyle;
	if (!recentFiles)
	{
		recentFiles = new TCStringArray(10, FALSE);
		populateRecentFiles();
	}
	if (!extraSearchDirs)
	{
		extraSearchDirs = new TCStringArray;
		populateExtraSearchDirs();
	}
	loadStatusBarIcons();
	//hExamineIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_EXAMINE),
	//	IMAGE_ICON, 32, 16, LR_DEFAULTCOLOR);
	//hFlythroughIcon = (HICON)LoadImage(hInstance,
	//	MAKEINTRESOURCE(IDI_FLYTHROUGH), IMAGE_ICON, 32, 16, LR_DEFAULTCOLOR);
	TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
		(TCAlertCallback)&LDViewWindow::progressAlertCallback);
	UCCHAR ucUserAgent[256];
	sucprintf(ucUserAgent, COUNT_OF(ucUserAgent),
		_UC("LDView/%s  (Windows; ldview@gmail.com; ")
		_UC("http://ldview.sf.net/)"), getProductVersion());
	char *userAgent = ucstringtoutf8(ucUserAgent);
	TCWebClient::setUserAgent(userAgent);
	delete[] userAgent;
	maxStandardSize.cx = 0;
	maxStandardSize.cy = 0;
//	DeleteObject(hBackgroundBrush);
// 	hBackgroundBrush = CreateSolidBrush(RGB(backgroundColor & 0xFF,
//		(backgroundColor >> 8) & 0xFF, (backgroundColor >> 16) & 0xFF));
}

LDViewWindow::~LDViewWindow(void)
{
}

void LDViewWindow::dealloc(void)
{
	destroyStatusBarIcons();
	TCAlertManager::unregisterHandler(this);
	TCObject::release(modelTreeDialog);
	TCObject::release(boundingBoxDialog);
	TCObject::release(mpdDialog);
	TCObject::release(toolbarStrip);
	delete userLDrawDir;
	userLDrawDir = NULL;
	delete videoModes;
	videoModes = NULL;
	if (hOpenGLInfoWindow)
	{
		DestroyWindow(hOpenGLInfoWindow);
	}
	if (openGLInfoWindoResizer)
	{
		openGLInfoWindoResizer->release();
	}
	if (hExtraDirsWindow)
	{
		DestroyWindow(hExtraDirsWindow);
	}
	if (hExtraDirsImageList)
	{
		ImageList_Destroy(hExtraDirsImageList);
	}
#if defined(USE_CPP11) || !defined(_NO_BOOST)
	if (hLibraryUpdateWindow)
	{
		DestroyWindow(hLibraryUpdateWindow);
	}
#endif // !_NO_BOOST
	delete[] productVersion;
	delete[] legalCopyright;
	TCObject::release(prefs);
	CUIWindow::dealloc();
}

void LDViewWindow::destroyStatusBarIcons(void)
{
	if (hExamineIcon)
	{
		DestroyIcon(hExamineIcon);
		hExamineIcon = NULL;
	}
	if (hFlythroughIcon)
	{
		DestroyIcon(hFlythroughIcon);
		hFlythroughIcon = NULL;
	}
	if (hWalkIcon)
	{
		DestroyIcon(hWalkIcon);
		hWalkIcon = NULL;
	}
}

void LDViewWindow::loadStatusBarIcons(void)
{
	double scaleFactor = getScaleFactor();
	hExamineIcon = TCImage::loadIconFromPngResource(hInstance,
		IDR_TB_EXAMINE, scaleFactor, CUIScaler::use32bit());
	hFlythroughIcon = TCImage::loadIconFromPngResource(hInstance,
		IDR_TB_FLYTHROUGH, scaleFactor, CUIScaler::use32bit());
	hWalkIcon = TCImage::loadIconFromPngResource(hInstance,
		IDR_TB_WALK, scaleFactor, CUIScaler::use32bit());
}

void LDViewWindow::loadSettings(void)
{
	fsWidth = TCUserDefaults::longForKey(FULLSCREEN_WIDTH_KEY, 640);
	fsHeight = TCUserDefaults::longForKey(FULLSCREEN_HEIGHT_KEY, 480);
	fsDepth = TCUserDefaults::longForKey(FULLSCREEN_DEPTH_KEY, 32);
	showStatusBar = TCUserDefaults::boolForKey(STATUS_BAR_KEY, true, false);
	showToolbar = TCUserDefaults::boolForKey(TOOLBAR_KEY, true, false);
	topmost = TCUserDefaults::boolForKey(TOPMOST_KEY, false, false);
	visualStyleEnabled = TCUserDefaults::boolForKey(VISUAL_STYLE_ENABLED_KEY,
		true, false);
	keepRightSideUp = TCUserDefaults::boolForKey(KEEP_RIGHT_SIDE_UP_KEY, false,
		false);
}

HBRUSH LDViewWindow::getBackgroundBrush(void)
{
//	return CUIWindow::getBackgroundBrush();
	return NULL;
}

void LDViewWindow::showWindow(int nCmdShow)
{
	LDrawModelViewer *modelViewer;
	
	if (screenSaver)
	{
		x = 0;
		y = 0;
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		CUIWindow::showWindow(SW_SHOW);
	}
	else
	{
		CUIWindow::showWindow(nCmdShow);
	}
	modelViewer = modelWindow->getModelViewer();
	if (modelViewer && !searchDirsInitialized)
	{
		modelViewer->setExtraSearchDirs(extraSearchDirs);
		searchDirsInitialized = true;
	}
	modelWindow->finalSetup();
}

void LDViewWindow::setScreenSaver(bool flag)
{
	screenSaver = flag;
	if (screenSaver)
	{
		windowStyle |= WS_CLIPCHILDREN;
	}
	else
	{
		windowStyle &= ~WS_CLIPCHILDREN;
	}
}

const char* LDViewWindow::windowClassName(void)
{
	if (fullScreen || screenSaver)
	{
		return "LDViewFullScreenWindow";
	}
	else
	{
		return "LDViewWindow";
	}
}

LRESULT LDViewWindow::doEraseBackground(RECT* updateRect)
{
	BOOL noRect = FALSE;

	if (updateRect == NULL)
	{
		if (paintStruct)
		{
			return 0;
		}
		updateRect = new RECT;
		GetUpdateRect(hWindow, updateRect, FALSE);
		noRect = TRUE;
	}
	DWORD backgroundColor = LDViewPreferences::getColor(BACKGROUND_COLOR_KEY);
 	HBRUSH hBrush = CreateSolidBrush(RGB(backgroundColor & 0xFF,
		(backgroundColor >> 8) & 0xFF, (backgroundColor >> 16) & 0xFF));

	debugPrintf(2, "updateRect size1: %d, %d\n",
		updateRect->right - updateRect->left,
		updateRect->bottom - updateRect->top);
	if (!fullScreen && !screenSaver && !hParentWindow)
	{
		int bottomMargin = 2;
		int topMargin = 2;

		if (updateRect->left < 2)
		{
			updateRect->left = 2;
		}
		if (updateRect->top < 2)
		{
			updateRect->top = 2;
		}
		if (updateRect->right > width - 2)
		{
			updateRect->right = width - 2;
		}
		if (showStatusBar || showStatusBarOverride)
		{
			bottomMargin += getStatusBarHeight();
		}
		if (updateRect->bottom > height - bottomMargin)
		{
			updateRect->bottom = height - bottomMargin;
		}
		if (showToolbar)
		{
			topMargin += getToolbarHeight();
		}
		if (updateRect->top < topMargin)
		{
			updateRect->top = topMargin;
		}
	}
	debugPrintf(2, "updateRect size2: %d, %d\n",
		updateRect->right - updateRect->left,
		updateRect->bottom - updateRect->top);
	FillRect(hdc, updateRect, hBrush);
	DeleteObject(hBrush);
	CUIWindow::doEraseBackground(updateRect);
	if (toolbarStrip)
	{
		HRGN region = CreateRectRgn(updateRect->left, updateRect->top,
			updateRect->right, updateRect->bottom);
		RECT rect;
		POINT points[2];
		static int num = 0;
		HWND hToolbar = toolbarStrip->getHWindow();

		GetClientRect(hToolbar, &rect);
		points[0].x = rect.left;
		points[0].y = rect.top;
		points[1].x = rect.right;
		points[1].y = rect.bottom;
		MapWindowPoints(hWindow, hWindow, points, 2);
		rect.left = points[0].x;
		rect.top = points[0].y;
		rect.right = points[1].x;
		rect.bottom = points[1].y;
		if (RectInRegion(region, &rect) || updateRect->top ==
			getToolbarHeight() + 2)
		{
			// For some reason, the toolbar won't redraw itself until there's an
			// idle moment.  So it won't redraw while the model is spinning, for
			// example.  The folowing forces it to redraw itself right now.
			RedrawWindow(hToolbar, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		DeleteObject(region);
	}
	if (noRect)
	{
		delete updateRect;
		updateRect = NULL;
	}
	return 0;
}

void LDViewWindow::forceShowStatusBar(bool value)
{
	if (value != showStatusBarOverride)
	{
		showStatusBarOverride = value;
		if (fullScreenActive)
		{
			if (!hStatusBar && showStatusBarOverride)
			{
				addStatusBar();
			}
			else if (hStatusBar && !showStatusBarOverride)
			{
				removeStatusBar();
			}
		}
		else if (!showStatusBar)
		{
			if (showStatusBarOverride)
			{
				addStatusBar();
			}
			else
			{
				removeStatusBar();
			}
		}
	}
}

void LDViewWindow::showStatusIcon(
	LDrawModelViewer::ViewMode viewMode,
	bool redraw /*= true*/)
{
	if ((showStatusBar || showStatusBarOverride) && hStatusBar)
	{
		HICON hModeIcon = hExamineIcon;
		CUCSTR tipText = TCLocalStrings::get(_UC("ExamineMode"));
		int iconPart = 2;

		if (inLatLonMode())
		{
			iconPart = 3;
			SendMessage(hStatusBar, SB_SETICON, 2, (LPARAM)NULL);
			SendMessage(hStatusBar, SB_SETTIPTEXT, 2, (LPARAM)"");
		}
		if (viewMode == LDrawModelViewer::VMFlyThrough)
		{
			hModeIcon = hFlythroughIcon;
			tipText = TCLocalStrings::get(_UC("FlyThroughMode"));
		}
		else if (viewMode == LDrawModelViewer::VMWalk)
		{
			hModeIcon = hWalkIcon;
			tipText = TCLocalStrings::get(_UC("WalkMode"));
		}
		SendMessage(hStatusBar, SB_SETICON, iconPart, (LPARAM)hModeIcon);
		sendMessageUC(hStatusBar, SB_SETTIPTEXT, iconPart, (LPARAM)tipText);
		if (redraw)
		{
			redrawStatusBar();
		}
	}
}

void LDViewWindow::setHParentWindow(HWND hWnd)
{
	hParentWindow = hWnd;
}

bool LDViewWindow::handleDpiChange(void)
{
	if (toolbarStrip)
	{
		removeToolbar();
	}
	if (!showToolbar && !initialShown)
	{
		// Icons from the toolbar get applied to the main menu.  So we
		// need to create it here if it's not visible, then immediately
		// delete it.
		createToolbar();
		toolbarStrip->release();
		toolbarStrip = NULL;
	}
	removeStatusBar();
	destroyStatusBarIcons();
	loadStatusBarIcons();
	reflectToolbar();
	reflectStatusBar();
	if (prefs != NULL)
	{
		prefs->checkForDpiChange();
	}
	if (modelWindow != NULL)
	{
		modelWindow->updateModelViewerSize();
	}
	return true;
}

void LDViewWindow::createToolbar(void)
{
	toolbarStrip = new ToolbarStrip(getLanguageModule());
	toolbarStrip->create(this);
	if (showToolbar)
	{
		//TBADDBITMAP addBitmap;
		//TBBUTTON *buttons;
		//char buttonTitle[128];
		//int i;
		//int count;

		//populateTbButtonInfos();
		//ModelWindow::initCommonControls(ICC_BAR_CLASSES | ICC_WIN95_CLASSES);
		//hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		//	WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT,
		//	0, 0, 0, 0, hWindow, (HMENU)ID_TOOLBAR, hInstance, NULL);
		//SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0,
		//	TBSTYLE_EX_DRAWDDARROWS | WS_EX_TRANSPARENT);
		//memset(buttonTitle, 0, sizeof(buttonTitle));
		//SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		//SendMessage(hToolbar, TB_SETBUTTONWIDTH, 0, MAKELONG(25, 25));
		//SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(25, 16));
		//if (newToolbar())
		//{
		//	HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK,
		//		10, 10);
		//	// Should the toolbar bitmap be language-specific?
		//	HBITMAP hBitmap = (HBITMAP)LoadImage(getLanguageModule(),
		//		MAKEINTRESOURCE(IDB_TOOLBAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		//	HBITMAP hMask = createMask(hBitmap, RGB(255, 0, 254));

		//	// ImageList_AddMask works fine in XP, and avoids the necessity of
		//	// creating the mask via the createMask function above, but according
		//	// to the documentation, it isn't supposed to work on bitmaps whose
		//	// color depth is greater than 8bpp.  Ours is 24bpp.
		//	ImageList_Add(imageList, hBitmap, hMask);
		//	DeleteObject(hBitmap);
		//	DeleteObject(hMask);
		//	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)imageList);
		//	stdBitmapStartId = tbBitmapStartId = 0;
		//}
		//else
		//{
		//	addBitmap.hInst = HINST_COMMCTRL;
		//	addBitmap.nID = IDB_STD_SMALL_COLOR;
		//	stdBitmapStartId = SendMessage(hToolbar, TB_ADDBITMAP, 0,
		//		(LPARAM)&addBitmap);
		//	// Should the toolbar bitmap be language-specific?
		//	addBitmap.hInst = getLanguageModule();
		//	// This doesn't actually work!!!
		//	addBitmap.nID = IDB_TOOLBAR;//256;
		//	// The 10 on the following line is the number of buttons in the bitmap.
		//	tbBitmapStartId = SendMessage(hToolbar, TB_ADDBITMAP, 10,
		//		(LPARAM)&addBitmap);
		//}
		//// Note: buttonTitle is an empty string.  No need for Unicode.
		//SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM)buttonTitle);
		//count = (int)tbButtonInfos.size();
		//buttons = new TBBUTTON[count];
		//for (i = 0; i < count; i++)
		//{
		//	TbButtonInfo &buttonInfo = tbButtonInfos[i];

		//	buttons[i].iBitmap = buttonInfo.getBmpId(stdBitmapStartId,
		//		tbBitmapStartId);
		//	buttons[i].idCommand = buttonInfo.getCommandId();
		//	buttons[i].fsState = buttonInfo.getState();
		//	buttons[i].fsStyle = buttonInfo.getStyle();
		//	buttons[i].dwData = (DWORD)this;
		//	buttons[i].iString = -1;
		//}
		//SendMessage(hToolbar, TB_ADDBUTTONS, count, (LPARAM)buttons);
		//delete[] buttons;
		//ShowWindow(hToolbar, SW_SHOW);
		//SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
		//ShowWindow(hToolbar, SW_HIDE);
		toolbarStrip->show();
	}
}

LDrawModelViewer::ViewMode LDViewWindow::getViewMode(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();
	if (modelViewer != NULL)
	{
		return modelViewer->getViewMode();
	}
	return LDrawModelViewer::VMExamine;
}

bool LDViewWindow::inExamineMode(void)
{
	return getViewMode() == LDrawModelViewer::VMExamine;
}

bool LDViewWindow::inLatLonMode(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (inExamineMode() && modelViewer &&
		modelViewer->getExamineMode() == LDrawModelViewer::EMLatLong)
	{
		return true;
	}
	return false;
}

// Note: static method
int LDViewWindow::intRound(TCFloat value)
{
	if (value >= 0)
	{
		return (int)(value + 0.5f);
	}
	else
	{
		return (int)(value - 0.5f);
	}
}

void LDViewWindow::showStatusLatLon(bool redraw /*= true*/)
{
	if (hStatusBar)
	{
		if (inLatLonMode())
		{
			LDrawModelViewer *modelViewer = modelWindow->getModelViewer();
			int lat = intRound(modelViewer->getExamineLatitude());
			int lon = intRound(modelViewer->getExamineLongitude());
			UCCHAR buf[1024];

			// Rounding can give us -180, even though LDrawModelViewer won't
			// allow that as an actual value.
			if (lon == -180)
			{
				lon = 180;
			}
			sucprintf(buf, COUNT_OF(buf), ls(_UC("LatLonFormat")), lat, lon);
			setStatusText(hStatusBar, 2, buf, redraw);
		}
		else
		{
			setStatusText(hStatusBar, 2, "", redraw);
		}
	}
}

#ifndef TC_NO_UNICODE
void LDViewWindow::setStatusText(
	HWND hStatus,
	int part,
	const char *text,
	bool redraw /*= true*/)
{
	std::wstring temp;

	if (text)
	{
		mbstowstring(temp, text);
	}
	setStatusText(hStatus, part, temp.c_str(), redraw);
}
#endif // TC_NO_UNICODE

void LDViewWindow::setStatusText(
	HWND hStatus,
	int part,
	CUCSTR text,
	bool redraw /*= true*/)
{
	UCCHAR oldText[1024];

	sendMessageUC(hStatus, SB_GETTEXT, part, (LPARAM)oldText);
	if (ucstrcmp(text, oldText) != 0)
	{
		sendMessageUC(hStatus, SB_SETTEXT, part, (LPARAM)text);
		if (redraw)
		{
			redrawStatusBar();
		}
		debugPrintf(2, "0x%08X: %s\n", hStatus, text);
	}
}

void LDViewWindow::updateStatusParts(void)
{
	if (hStatusBar)
	{
		int parts[] = {100, 100, -1, -1};
		RECT rect;
		int numParts = 3;
		bool latLon = inLatLonMode();
		int rightMargin = scalePoints(20);
		int latLonWidth = scalePoints(100);

		if (latLon)
		{
			numParts = 4;
			parts[2] = 100;
			rightMargin += latLonWidth;
		}
		setStatusBarParts(hStatusBar, numParts, parts);
		SendMessage(hStatusBar, SB_GETRECT, numParts - 1, (LPARAM)&rect);
		parts[1] += rect.right - rect.left - rightMargin;
		if (latLon)
		{
			parts[2] = parts[1] + latLonWidth;
		}
		setStatusBarParts(hStatusBar, numParts, parts, false);
		showStatusIcon(getViewMode(), false);
		showStatusLatLon();
	}
}

void LDViewWindow::createStatusBar(void)
{
	if (showStatusBar || showStatusBarOverride)
	{
		HWND hProgressBar;
		RECT rect;

		ModelWindow::initCommonControls(ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES);
		hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | 
			SBARS_SIZEGRIP | SBT_TOOLTIPS, "", hWindow, ID_STATUS_BAR);
		SetWindowLongW(hStatusBar, GWL_EXSTYLE, WS_EX_TRANSPARENT);
		updateStatusParts();
		SendMessage(hStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)"");
		SendMessage(hStatusBar, SB_GETRECT, 0, (LPARAM)&rect);
		InflateRect(&rect, scalePoints(-4), scalePoints(-3));
		hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "",
			WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect.left,
			rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hStatusBar, NULL, hInstance, NULL);
		showStatusIcon(getViewMode());
		if (modelWindow)
		{
			modelWindow->setStatusBar(hStatusBar);
			modelWindow->setProgressBar(hProgressBar);
		}
		redrawStatusBar();
		//RedrawWindow(hStatusBar, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

void LDViewWindow::redrawStatusBar(void)
{
	//RECT statusRect;

	//GetWindowRect(hStatusBar, &statusRect);
	//screenToClient(hWindow, &statusRect);
	//RedrawWindow(hWindow, &statusRect, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ERASENOW | RDW_ALLCHILDREN);
	RedrawWindow(hStatusBar, NULL, NULL,
		RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
}

void LDViewWindow::reflectPovCameraAspect(bool saveSetting)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer->getPovCameraAspect() !=
		getMenuCheck(hToolsMenu, ID_TOOLS_POV_CAMERA_ASPECT))
	{
		switchPovCameraAspect(saveSetting);
	}
}

void LDViewWindow::reflectViewMode(bool saveSetting)
{
	LDrawModelViewer::ViewMode viewMode =
		(LDrawModelViewer::ViewMode)TCUserDefaults::longForKey(VIEW_MODE_KEY, 0,
		false);
	switchToViewMode(viewMode, saveSetting);
}

BOOL LDViewWindow::initWindow(void)
{
	if (!modelWindow)
	{
		createModelWindow();
	}
	if (fullScreen || screenSaver)
	{
		if (hWindowMenu)
		{
			DestroyMenu(hWindowMenu);
			hWindowMenu = NULL;
		}
		windowStyle = WS_POPUP | WS_MAXIMIZE;
		if (screenSaver)
		{
			windowStyle |= WS_CLIPCHILDREN;
		}
#ifndef _DEBUG
		exWindowStyle |= WS_EX_TOPMOST;
#endif
	}
	else if (hParentWindow)
	{
		//windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU;
		windowStyle = WS_CHILD;
	}
	else
	{
		hWindowMenu = LoadMenu(getLanguageModule(),
			MAKEINTRESOURCE(IDR_MAIN_MENU));
		windowStyle = standardWindowStyle;
		if (topmost)
		{
			exWindowStyle |= WS_EX_TOPMOST;
		}
		else
		{
			exWindowStyle &= ~WS_EX_TOPMOST;
		}
	}
	DWORD origThemeAppProps = CUIThemes::getThemeAppProperties();

	if (TCUserDefaults::boolForKey(FORCE_THEMED_MENUS_KEY, false, false))
	{
		CUIThemes::setThemeAppProperties(STAP_ALLOW_NONCLIENT |
			STAP_ALLOW_CONTROLS);
	}
	if (CUIWindow::initWindow())
	{
		CUIThemes::setThemeAppProperties(origThemeAppProps);
		hFileMenu = GetSubMenu(GetMenu(hWindow), 0);
		hViewMenu = GetSubMenu(GetMenu(hWindow), 2);
		hStepMenu = GetSubMenu(GetMenu(hWindow), 3);
		hToolsMenu = GetSubMenu(GetMenu(hWindow), 4);
		hViewAngleMenu = findSubMenu(hViewMenu, 0);
		hStandardSizesMenu = findSubMenu(hViewMenu, 1);
		if (!CUIThemes::isThemeLibLoaded())
		{
			RemoveMenu(hViewMenu, ID_VIEW_VISUALSTYLE, MF_BYCOMMAND);
		}
		else
		{
			reflectVisualStyle();
		}
//		hViewAngleMenu = GetSubMenu(hViewMenu, 7);
		//hToolbarMenu = LoadMenu(getLanguageModule(),
		//	MAKEINTRESOURCE(IDR_TOOLBAR_MENU));
		//hWireframeToolbarMenu = GetSubMenu(hToolbarMenu, 0);
		//hEdgesToolbarMenu = GetSubMenu(hToolbarMenu, 1);
		//hPrimitivesToolbarMenu = GetSubMenu(hToolbarMenu, 2);
		//hLightingToolbarMenu = GetSubMenu(hToolbarMenu, 3);
		//hBFCToolbarMenu = GetSubMenu(hToolbarMenu, 4);
		reflectViewMode(false);
		reflectPovCameraAspect(false);
		populateRecentFileMenuItems();
		updateModelMenuItems();
		if (!fullScreen && !screenSaver)
		{
			setMenuCheck(hViewMenu, ID_VIEW_ALWAYSONTOP, topmost);
		}
		return modelWindow->initWindow();
	}
	return FALSE;
}

std::string LDViewWindow::getFloatUdKey(const char* udKey)
{
	std::string floatUdKey = udKey;
	floatUdKey += "Float";
	return floatUdKey;
}

void LDViewWindow::savePixelSize(const char* udKey, int size)
{
	float scaleFactor = (float)getScaleFactor();
	float fsize = size / scaleFactor;
	TCUserDefaults::setFloatForKey(fsize, getFloatUdKey(udKey).c_str(), false);
	TCUserDefaults::setLongForKey((long)fsize, udKey, false);
}

int LDViewWindow::getSavedPixelSize(const char* udKey, int defaultSize)
{
	std::string floatUdKey = getFloatUdKey(udKey);
	double size = TCUserDefaults::floatForKey(floatUdKey.c_str(), -1.0, false);
	if (size == -1.0)
	{
		size = TCUserDefaults::longForKey(udKey, defaultSize, true);
	}
	return (int)(size * getScaleFactor());
}

int LDViewWindow::getSavedWindowWidth(int defaultValue /*= -1*/)
{
	return getSavedPixelSize(WINDOW_WIDTH_KEY,
		defaultValue == -1 ? DEFAULT_WIN_WIDTH : defaultValue);
}

int LDViewWindow::getSavedWindowHeight(int defaultValue /*= -1*/)
{
	return getSavedPixelSize(WINDOW_HEIGHT_KEY,
		defaultValue == -1 ? DEFAULT_WIN_HEIGHT : defaultValue);
}

void LDViewWindow::createModelWindow(void)
{
	int lwidth;
	int lheight;
	bool maximized;

	TCObject::release(modelWindow);
	lwidth = getSavedWindowWidth();
	lheight = getSavedWindowHeight();
	maximized = TCUserDefaults::longForKey(WINDOW_MAXIMIZED_KEY, 0, false) != 0;
	if (screenSaver)
	{
		modelWindow = new SSModelWindow(this, 0, 0, lwidth, lheight);
	}
	else
	{
		// Note that while the toolbar and status bar might be turned on, they
		// haven't been shown yet.  They'll resize the model window when they
		// get shown.
		modelWindow = new ModelWindow(this, 0, 0, lwidth, lheight);
	}
	prefs = modelWindow->getPrefs();
	prefs->retain();
}

BOOL LDViewWindow::showAboutBox(void)
{
	if (!hAboutWindow)
	{
		createAboutBox();
	}
	if (hAboutWindow)
	{
		runDialogModal(hAboutWindow);
		return TRUE;
	}
	return FALSE;
}

const UCCHAR *LDViewWindow::getProductVersion(void)
{
	if (!productVersion)
	{
		readVersionInfo();
	}
	return productVersion;
}

const UCCHAR *LDViewWindow::getLegalCopyright(void)
{
	if (!legalCopyright)
	{
		readVersionInfo();
	}
	return legalCopyright;
}

void LDViewWindow::readVersionInfo(void)
{
	UCCHAR moduleFilename[1024];

	if (productVersion != NULL)
	{
		return;
	}
	if (getModuleFileNameUC(NULL, moduleFilename, sizeof(moduleFilename)) > 0)
	{
		DWORD zero;
		DWORD versionInfoSize = getFileVersionInfoSizeUC(moduleFilename, &zero);

		if (versionInfoSize > 0)
		{
			BYTE *versionInfo = new BYTE[versionInfoSize];

			if (getFileVersionInfoUC(moduleFilename, NULL, versionInfoSize,
				versionInfo))
			{
				UCCHAR *value;
				UINT versionLength;

				if (verQueryValueUC(versionInfo,
					_UC("\\StringFileInfo\\040904B0\\ProductVersion"),
					(void**)&value, &versionLength))
				{
					productVersion = copyString(value);
				}
				if (verQueryValueUC(versionInfo,
					_UC("\\StringFileInfo\\040904B0\\LegalCopyright"),
					(void**)&value, &versionLength))
				{
					legalCopyright = copyString(value);
				}
			}
			delete[] versionInfo;
		}
	}
}

#include <time.h>

void LDViewWindow::createAboutBox(void)
{
	UCCHAR fullVersionFormat[1024];
	UCCHAR fullVersionString[1024];
	UCCHAR versionString[128];
	UCCHAR copyrightString[128];
	UCCHAR buildDateString[128];
	char *tmpString = stringByReplacingSubstring(__DATE__, "  ", " ");
	UCCHAR *tmpUCString = mbstoucstring(tmpString);
	int dateCount;
	UCCHAR **dateComponents = componentsSeparatedByString(tmpUCString, _UC(" "),
		dateCount);

	delete[] tmpString;
	delete[] tmpUCString;
	ucstrcpy(buildDateString, _UC("!UnknownDate!"));
	if (dateCount == 3)
	{
		const UCCHAR *buildMonth = TCLocalStrings::get(dateComponents[0]);

		if (buildMonth)
		{
			sucprintf(buildDateString, COUNT_OF(buildDateString),
				_UC("%s %s, %s"), dateComponents[1], buildMonth,
				dateComponents[2]);
		}
	}
	deleteStringArray(dateComponents, dateCount);
	ucstrcpy(versionString, TCLocalStrings::get(_UC("!UnknownVersion!")));
	ucstrcpy(copyrightString, TCLocalStrings::get(_UC("Copyright")));
	hAboutWindow = createDialog(IDD_ABOUT_BOX);
	sendDlgItemMessageUC(hAboutWindow, IDC_VERSION_LABEL, WM_GETTEXT,
		COUNT_OF(fullVersionFormat), (LPARAM)fullVersionFormat);
	readVersionInfo();
	if (productVersion != NULL)
	{
		ucstrcpy(versionString, productVersion);
	}
	if (legalCopyright != NULL)
	{
		ucstrcpy(copyrightString, legalCopyright);
	}
#ifdef _WIN64
	const UCCHAR *platform = _UC("x64");
#else // _WIN64
	const UCCHAR *platform = _UC("x86");
#endif // _WIN64
	sucprintf(fullVersionString, COUNT_OF(fullVersionString), fullVersionFormat,
		versionString, platform, buildDateString, copyrightString);
	sendDlgItemMessageUC(hAboutWindow, IDC_VERSION_LABEL, WM_SETTEXT,
		0, (LPARAM)fullVersionString);
}

BOOL LDViewWindow::doLDrawDirOK(HWND hDlg)
{
	int length = (int)SendDlgItemMessage(hDlg, IDC_LDRAWDIR, WM_GETTEXTLENGTH, 0, 0);

	if (length)
	{
		delete userLDrawDir;
		userLDrawDir = new char[length + 1];
		SendDlgItemMessage(hDlg, IDC_LDRAWDIR, WM_GETTEXT, (WPARAM)(length + 1),
			(LPARAM)userLDrawDir);
		doDialogClose(hDlg);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
	return TRUE;
}

LRESULT LDViewWindow::doMouseWheel(short keyFlags, short zDelta, int /*xPos*/,
								  int /*yPos*/)
{
	//debugPrintf("doMouseWheel(%d, %d)\n", keyFlags, zDelta);
	if (modelWindow)
	{
		modelWindow->mouseWheel(keyFlags, zDelta);
		//if (keyFlags & MK_CONTROL)
		//{
		//	modelWindow->setClipZoom(true);
		//}
		//else
		//{
		//	modelWindow->setClipZoom(false);
		//}
		//modelWindow->zoom((TCFloat)zDelta * -0.5f);
		return 0;
	}
	return 1;
}


UINT CALLBACK lDrawDirBrowseHook(HWND /*hDlg*/, UINT message, WPARAM /*wParam*/,
								 LPARAM lParam)
{
#ifdef _DEBUG
	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "hook message: 0x%X\n", message);
#endif // _DEBUG
	if (message == WM_NOTIFY)
	{
		LPOFNOTIFY notification = (LPOFNOTIFY)lParam;

		switch (notification->hdr.code)
		{
			case CDN_FILEOK:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "OK Pressed\n");
#endif // _DEBUG
				break;
			case CDN_FOLDERCHANGE:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
					"Folder Change: 0x%X\n", notification->hdr.hwndFrom);
#endif // _DEBUG
				break;
			case CDN_HELP:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Help Pressed\n");
#endif // _DEBUG
				break;
			case CDN_INITDONE:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Init done\n");
#endif // _DEBUG
				break;
			case CDN_SELCHANGE:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
					"Selection Change: 0x%X\n", notification->hdr.hwndFrom);
#endif // _DEBUG
				break;
			case CDN_SHAREVIOLATION:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Share violation\n");
#endif // _DEBUG
				break;
			case CDN_TYPECHANGE:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Type change\n");
#endif // _DEBUG
				break;
			default:
#ifdef _DEBUG
				_CrtDbgReport(_CRT_WARN, NULL, 0, NULL,
					"Unknown notification: 0x%X\n", notification->hdr.code);
#endif // _DEBUG
				break;
		}
	}
	return 0;
}

std::string LDViewWindow::getDisplayName(void)
{
#ifndef TC_NO_UNICODE
	MONITORINFOEX mi;

	if (hMonitor != NULL)
	{
		memset(&mi, 0, sizeof(mi));
		mi.cbSize = sizeof(mi);

		if (GetMonitorInfo(hMonitor, &mi))
		{
			return mi.szDevice;
		}
	}
#endif // TC_NO_UNICODE
	return "";
}

LONG LDViewWindow::changeDisplaySettings(DEVMODE *deviceMode, DWORD flags)
{
	std::string deviceName = getDisplayName();

	//debugPrintf("displayName: %s\n", deviceName.c_str());
	if (flags == CDS_FULLSCREEN)
	{
		DEVMODE curDevMode;

		memset(&curDevMode, 0, sizeof(curDevMode));
		curDevMode.dmSize = sizeof(curDevMode);
		if (deviceName.size() > 0)
		{
			EnumDisplaySettings(deviceName.c_str(), ENUM_CURRENT_SETTINGS,
				&curDevMode);
		}
		else
		{
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &curDevMode);
		}
		if (curDevMode.dmPelsWidth == deviceMode->dmPelsWidth &&
			curDevMode.dmPelsHeight == deviceMode->dmPelsHeight &&
			curDevMode.dmBitsPerPel == deviceMode->dmBitsPerPel)
		{
			if (deviceMode->dmFields & DM_DISPLAYFREQUENCY)
			{
				if (curDevMode.dmDisplayFrequency ==
					deviceMode->dmDisplayFrequency)
				{
					return DISP_CHANGE_SUCCESSFUL;
				}
			}
			else
			{
				return DISP_CHANGE_SUCCESSFUL;
			}
		}
	}
#ifndef TC_NO_UNICODE
	if (deviceName.size() > 0)
	{
		return ChangeDisplaySettingsEx(deviceName.c_str(), deviceMode, NULL,
			flags, NULL);
	}
	else
#endif // TC_NO_UNICODE
	{
		return ChangeDisplaySettings(deviceMode, flags);
	}
}

bool LDViewWindow::tryVideoMode(VideoModeT* videoMode, int refreshRate)
{
	if (videoMode)
	{
		DEVMODE deviceMode;
		long result;
		fullScreenActive = true;

		memset(&deviceMode, 0, sizeof DEVMODE);
		deviceMode.dmSize = sizeof DEVMODE;
		deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		deviceMode.dmPelsWidth = videoMode->width;
		deviceMode.dmPelsHeight = videoMode->height;
		deviceMode.dmBitsPerPel = videoMode->depth;
		if (refreshRate)
		{
			deviceMode.dmFields |= DM_DISPLAYFREQUENCY;
			deviceMode.dmDisplayFrequency = refreshRate;
		}
		result = changeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
		//result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
		switch (result)
		{
			case DISP_CHANGE_SUCCESSFUL:
				return true;
				break;
			case DISP_CHANGE_RESTART:
			case DISP_CHANGE_BADFLAGS:
			case DISP_CHANGE_BADPARAM:
			case DISP_CHANGE_FAILED:
			case DISP_CHANGE_BADMODE:
			case DISP_CHANGE_NOTUPDATED:
				fullScreenActive = false;
				return false;
		}
		fullScreenActive = false;
	}
	return false;
}

void LDViewWindow::setFullScreenDisplayMode(void)
{
	VideoModeT* videoMode = getCurrentVideoMode();

	if (!fullScreenActive && videoMode)
	{
		int refreshRate = TCUserDefaults::longForKey(FULLSCREEN_REFRESH_KEY);

		if (!tryVideoMode(videoMode, refreshRate) && refreshRate)
		{
			tryVideoMode(videoMode, 0);
		}
	}
}

void LDViewWindow::restoreDisplayMode(void)
{
	initialShown = false;
	if (fullScreenActive)
	{
		long result = ChangeDisplaySettings(NULL, 0);

		switch (result)
		{
			case DISP_CHANGE_SUCCESSFUL:
				break;
			case DISP_CHANGE_RESTART:
				break;
			case DISP_CHANGE_BADFLAGS:
				break;
			case DISP_CHANGE_BADPARAM:
				break;
			case DISP_CHANGE_FAILED:
				break;
			case DISP_CHANGE_BADMODE:
				break;
			case DISP_CHANGE_NOTUPDATED:
				break;
		}
		fullScreenActive = false;
	}
}

/*
void LDViewWindow::setModelWindow(ModelWindow *value)
{
	value->retain();
	if (modelWindow)
	{
		modelWindow->release();
	}
	modelWindow = value;
}
*/

void LDViewWindow::activateFullScreenMode(void)
{
	if (!fullScreenActive)
	{
		skipMinimize = true;
		if (!getCurrentVideoMode())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
//		modelWindow->uncompile();
		switchingModes = true;
		if (modelWindow != NULL && modelWindow->getHWindow() != NULL)
		{
			modelWindow->closeWindow();
//			modelWindowShown = false;
		}
		DestroyWindow(hWindow);
		switchingModes = false;
		setFullScreenDisplayMode();
		if (initWindow())
		{
			showWindow(SW_SHOW);
			modelWindow->uncompile();
			//modelWindow->setNeedsRecompile();
			modelWindow->forceRedraw(1);
		}
		skipMinimize = false;
	}
}

void LDViewWindow::deactivateFullScreenMode(void)
{
	modelWindow->uncompile();
	restoreDisplayMode();
	if (skipMinimize)
	{
//		modelWindow->forceRedraw(1);
	}
	else
	{
		// Minimize sets our x and y to -32000.  Save the current values, so
		// that when we create the window again later during activate, it will
		// go on the correct monitor.
		int savedX = x;
		int savedY = y;

		ShowWindow(hWindow, SW_MINIMIZE);
		modelWindow->closeWindow();
		x = savedX;
		y = savedY;
//		modelWindowShown = false;
	}
}

LRESULT LDViewWindow::doActivate(int activateFlag, BOOL /*minimized*/,
								  HWND /*previousHWindow*/)
{
	if (modelWindow)
	{
		if (!modelWindowIsShown() && !switchingModes &&
			activateFlag != WA_INACTIVE)
		{
			modelWindow->showWindow(SW_SHOWNORMAL);
		}
		if (activateFlag == WA_ACTIVE || activateFlag == WA_CLICKACTIVE)
		{
			modelWindow->startPolling();
		}
		else
		{
			if (modelWindow->getPollSetting() != POLL_BACKGROUND)
			{
				modelWindow->stopPolling();
			}
		}
	}
	return 0;
}

LRESULT LDViewWindow::doActivateApp(BOOL activateFlag, DWORD /*threadId*/)
{
	VideoModeT* videoMode = getCurrentVideoMode();

	if (fullScreen && videoMode)
	{
		if (activateFlag)
		{
			activateFullScreenMode();
		}
		else
		{
			deactivateFullScreenMode();
		}
		return 0;
	}
	else
	{
		return 1;
/*
		if (activateFlag)
		{
			SetActiveWindow(hWindow);
		}
*/
	}
}

BOOL LDViewWindow::doRemoveExtraDir(void)
{
	int index = (int)SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		extraSearchDirs->removeStringAtIndex(index);
		SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
		if (index >= extraSearchDirs->getCount())
		{
			index--;
		}
		if (index >= 0)
		{
			// ToDo: Unicode?  It's a filename, and I'm avoiding that.
			SendMessage(hExtraDirsList, LB_SELECTSTRING, index,
				(LPARAM)extraSearchDirs->stringAtIndex(index));
		}
	}
	updateExtraDirsEnabled();
	return TRUE;
}

BOOL LDViewWindow::doAddExtraDir(void)
{
	BROWSEINFO browseInfo;
	char displayName[MAX_PATH];
	LPITEMIDLIST itemIdList;
	char *currentSelection = NULL;
	int index = (int)SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		currentSelection = (*extraSearchDirs)[index];
	}
	browseInfo.hwndOwner = NULL; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = TCLocalStrings::get("AddExtraDirPrompt");
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)currentSelection;
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		char path[MAX_PATH+10];
	    LPMALLOC pMalloc = NULL;
		HRESULT hr;

		if (SHGetPathFromIDList(itemIdList, path))
		{
			stripTrailingPathSeparators(path);
			extraSearchDirs->addString(path);
			SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_LIST, LB_ADDSTRING, 0,
				(LPARAM)path);
			SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_LIST, LB_SETCURSEL,
				extraSearchDirs->getCount() - 1, (LPARAM)path);
			updateExtraDirsEnabled();
		}
	    hr = SHGetMalloc(&pMalloc);
		if (SUCCEEDED(hr))
		{
			pMalloc->Free(itemIdList);
			pMalloc->Release();
		}
	}
	return TRUE;
}

BOOL LDViewWindow::doMoveExtraDirUp(void)
{
	int index = (int)SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);
	char *extraDir;

	if (index == LB_ERR || index == 0)
	{
		// we shouldn't get here, but just in case...
		return TRUE;
	}
	extraDir = copyString((*extraSearchDirs)[index]);
	extraSearchDirs->removeStringAtIndex(index);
	SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
	extraSearchDirs->insertString(extraDir, index - 1);
	// ToDo: Unicode ?Maybe: filename
	SendMessage(hExtraDirsList, LB_INSERTSTRING, index - 1, (LPARAM)extraDir);
	SendMessage(hExtraDirsList, LB_SETCURSEL, index - 1, (LPARAM)extraDir);
	updateExtraDirsEnabled();
	delete extraDir;
	return TRUE;
}

void LDViewWindow::updateWindowMonitor(void)
{
#ifndef TC_NO_UNICODE
	hMonitor = MonitorFromWindow(hWindow, MONITOR_DEFAULTTOPRIMARY);
#endif // TC_NO_UNICODE
}

LRESULT LDViewWindow::doMove(int newX, int newY)
{
	LRESULT retVal = CUIWindow::doMove(newX, newY);

	updateWindowMonitor();
	return retVal;
}

BOOL LDViewWindow::doMoveExtraDirDown(void)
{
	int index = (int)SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);
	char *extraDir;

	if (index == LB_ERR || index >= extraSearchDirs->getCount() - 1)
	{
		// we shouldn't get here, but just in case...
		return TRUE;
	}
	extraDir = copyString((*extraSearchDirs)[index]);
	extraSearchDirs->removeStringAtIndex(index);
	SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
	extraSearchDirs->insertString(extraDir, index + 1);
	// ToDo: Unicode ?Maybe: filename
	SendMessage(hExtraDirsList, LB_INSERTSTRING, index + 1, (LPARAM)extraDir);
	// ToDo: Unicode ?Maybe: filename
	SendMessage(hExtraDirsList, LB_SETCURSEL, index + 1, (LPARAM)extraDir);
	updateExtraDirsEnabled();
	delete extraDir;
	return TRUE;
}

BOOL LDViewWindow::doExtraDirSelected(void)
{
	updateExtraDirsEnabled();
	return TRUE;
}

BOOL LDViewWindow::doExtraDirsCommand(int controlId, int notifyCode,
									  HWND hControlWnd)
{
	if (hControlWnd == hExtraDirsToolbar)
	{
		switch (controlId)
		{
		case 42:
			return doAddExtraDir();
			break;
		case 43:
			return doRemoveExtraDir();
			break;
		case 44:
			return doMoveExtraDirUp();
			break;
		case 45:
			return doMoveExtraDirDown();
			break;
		default:
			return FALSE;
		}
	}
	else if (notifyCode == BN_CLICKED)
	{
		switch (controlId)
		{
			case IDOK:
				recordExtraSearchDirs();
				doDialogClose(hExtraDirsWindow);
				break;
			case IDCANCEL:
				populateExtraSearchDirs();
				populateExtraDirsListBox();
				doDialogClose(hExtraDirsWindow);
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	else if (notifyCode == LBN_SELCHANGE)
	{
		return doExtraDirSelected();
	}
	else
	{
		return FALSE;
	}
}

BOOL LDViewWindow::doLDrawDirCommand(int controlId, int notifyCode,
									 HWND /*hControlWnd*/)
{
	if (notifyCode == BN_CLICKED)
	{
		switch (controlId)
		{
			case IDOK:
				return doLDrawDirOK(hLDrawDirWindow);
				break;
			case IDCANCEL:
				doDialogClose(hLDrawDirWindow);
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void LDViewWindow::doDialogOK(HWND hDlg)
{
	doDialogClose(hDlg);
}

void LDViewWindow::doDialogCancel(HWND hDlg)
{
#if defined(USE_CPP11) || !defined(_NO_BOOST)
	if (hDlg == hLibraryUpdateWindow)
	{
		libraryUpdateCanceled = true;
	}
#endif // !_NO_BOOST
	doDialogClose(hDlg);
}

BOOL LDViewWindow::doDialogCommand(HWND hDlg, int controlId, int notifyCode,
								   HWND hControlWnd)
{
	debugPrintf("LDViewWindow::doDialogCommand: 0x%04X, 0x%04X, 0x%04x\n", hDlg,
		controlId, notifyCode);
	if (hDlg)
	{
		if (hDlg == hExtraDirsWindow)
		{
			return doExtraDirsCommand(controlId, notifyCode, hControlWnd);
		}
		else if (hDlg == hLDrawDirWindow)
		{
			return doLDrawDirCommand(controlId, notifyCode, hControlWnd);
		}
	}
	if (notifyCode == BN_CLICKED)
	{
		switch (controlId)
		{
			case IDOK:
				doDialogOK(hDlg);
				break;
			case IDCANCEL:
				doDialogCancel(hDlg);
				break;
			default:
				return FALSE;
				break;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL LDViewWindow::checkMouseMove(HWND hWnd, LPARAM lParam)
{
	POINT mousePoint = { LOWORD(lParam), HIWORD(lParam)};

	ClientToScreen(hWnd, &mousePoint);
	if (originalMouseX == -999999)
	{
		originalMouseX = mousePoint.x;
		originalMouseY = mousePoint.y;
	}
	if (originalMouseX == mousePoint.x && originalMouseY == mousePoint.y)
	{
		return FALSE;
	}
	return TRUE;
}

void LDViewWindow::initMouseMove(void)
{
	originalMouseX = -999999;
}

LRESULT LDViewWindow::doEnterMenuLoop(bool /*isTrackPopupMenu*/)
{
	if (loading)
	{
		return 0;
	}
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			modelViewer->pause();
		}
	}
	return 1;
}

LRESULT LDViewWindow::doExitMenuLoop(bool /*isTrackPopupMenu*/)
{
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			modelViewer->unpause();
		}
		modelWindow->forceRedraw();
	}
	return 1;
}

LRESULT LDViewWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam,
								 LPARAM lParam)
{
	if (screenSaver && modelWindow &&
		((SSModelWindow*)modelWindow)->checkForExit(hWnd, message, wParam,
		lParam))
	{
		shutdown();
		return 1;
	}
	return CUIWindow::windowProc(hWnd, message, wParam, lParam);
}

void LDViewWindow::switchModes(void)
{
	if (hStatusBar)
	{
		removeStatusBar();
	}
	if (toolbarStrip)
	{
		removeToolbar();
	}
	skipMinimize = true;
	if (!getCurrentVideoMode())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	modelWindow->uncompile();
	switchingModes = true;
	if (modelWindow)
	{
		modelWindow->closeWindow();
	}
	DestroyWindow(hWindow);
	switchingModes = false;
	fullScreen = !fullScreen;
	if (fullScreen)
	{
		setFullScreenDisplayMode();
	}
	else
	{
		int newWidth = getSavedWindowWidth(0);
		int newHeight = getSavedWindowHeight(0);
		int dWidth = newWidth - width;
		int dHeight = newHeight - height;

		restoreDisplayMode();
		width = newWidth;
		height = newHeight;
		newWidth = modelWindow->getWidth() + dWidth;
		newHeight = modelWindow->getHeight() + dHeight;
		modelWindow->resize(newWidth, newHeight);
	}
	if (initWindow())
	{
		SetWindowPos(modelWindow->getHWindow(), HWND_TOP, 0, 0,
			width, height, 0);
		showWindow(SW_SHOW);
		modelWindow->uncompile();
		skipMinimize = false;
	}
	else
	{
		skipMinimize = false;
		if (fullScreen)
		{
			switchModes();
		}
		else
		{
			stopAnimation();
			MessageBox(hWindow, TCLocalStrings::get("SwitchBackError"),
				TCLocalStrings::get("Error"), MB_OK);
			shutdown();
		}
	}
	modelWindow->forceRedraw();
	SetActiveWindow(hWindow);
	debugPrintf(2, "0x0%08X: Just set active\n", hWindow);
}

void LDViewWindow::shutdown(void)
{
	skipMinimize = true;
	if (modelWindow)
	{
		ModelWindow *tmpModelWindow = modelWindow;

		modelWindow = NULL;
		tmpModelWindow->closeWindow();
		tmpModelWindow->release();
//		modelWindowShown = false;
	}
	DestroyWindow(hWindow);
}

LRESULT LDViewWindow::doChar(TCHAR characterCode, LPARAM /*keyData*/)
{
	switch (characterCode)
	{
		case 10: // Ctrl+Enter (Linefeed)
			switchModes();
			return 0;
			break;
		case 27: // Escape
			if (fullScreen)
			{
				switchModes();
//				shutdown();
				return 0;
			}
			break;
	}
	return 1;
}

void LDViewWindow::selectFSVideoModeMenuItem(int index, bool saveSetting)
{
	VideoModeT* videoMode = getCurrentVideoMode();
	HMENU bitDepthMenu;
	MENUITEMINFO itemInfo;
	int menuIndex;
	HMENU viewMenu = GetSubMenu(GetMenu(hWindow), 2);

	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_STATE;
	if (videoMode)
	{
		bitDepthMenu = menuForBitDepth(hWindow, videoMode->depth, &menuIndex);
		GetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
		itemInfo.fMask = MIIM_STATE;
		itemInfo.fState &= ~MFS_CHECKED;
		itemInfo.fState |= MFS_UNCHECKED;
		SetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
		GetMenuItemInfo(bitDepthMenu, menuIndex, TRUE, &itemInfo);
		itemInfo.fMask = MIIM_STATE;
		itemInfo.fState &= ~MFS_CHECKED;
		itemInfo.fState |= MFS_UNCHECKED;
		SetMenuItemInfo(viewMenu, menuIndex, TRUE, &itemInfo);
	}
	currentVideoModeIndex = index;
	videoMode = getCurrentVideoMode();
	fsWidth = videoMode->width;
	fsHeight = videoMode->height;
	fsDepth = videoMode->depth;
	if (saveSetting)
	{
		TCUserDefaults::setLongForKey(fsWidth, FULLSCREEN_WIDTH_KEY);
		TCUserDefaults::setLongForKey(fsHeight, FULLSCREEN_HEIGHT_KEY);
		TCUserDefaults::setLongForKey(fsDepth, FULLSCREEN_DEPTH_KEY);
	}
	if (videoMode)
	{
		bitDepthMenu = menuForBitDepth(hWindow, videoMode->depth, &menuIndex);
		GetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
		itemInfo.fMask = MIIM_STATE;
		itemInfo.fState |= MFS_CHECKED;
		itemInfo.fState &= ~MFS_UNCHECKED;
		SetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
		GetMenuItemInfo(bitDepthMenu, menuIndex, TRUE, &itemInfo);
		itemInfo.fMask = MIIM_STATE;
		itemInfo.fState |= MFS_CHECKED;
		itemInfo.fState &= ~MFS_UNCHECKED;
		SetMenuItemInfo(viewMenu, menuIndex, TRUE, &itemInfo);
	}
}

void LDViewWindow::reflectPolling(void)
{
	selectPollingMenuItem(ID_FILE_POLLING_DISABLED +
		TCUserDefaults::longForKey(POLL_KEY, 0, false));
}

void LDViewWindow::initPollingMenu(void)
{
	int pollSetting = TCUserDefaults::longForKey(POLL_KEY, 0, false);
	HMENU pollingMenu = getPollingMenu();
	int i;
	int count = GetMenuItemCount(pollingMenu);
	MENUITEMINFO itemInfo;
	char title[256];

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	// Did I mention that Windows sucks?  In order to change the type to be a
	// radio check instead of a regular check, we have to set the type to
	// MFT_STRING | MFT_RADIOCHECK.  The thing is, that requires us to set the
	// title of the menu item at the same time.  So we have to read the title of
	// the menu item into a buffer so that we can write the same value back,
	// just so that we can turn on the MFT_RADIO_CHECK bit.  All I can say is
	// whoever designed this system is a moron, and whoever approved it is also
	// a moron.
	itemInfo.fMask = MIIM_TYPE | MIIM_STATE;
	itemInfo.dwTypeData = title;
	for (i = 0; i < count; i++)
	{
		itemInfo.cch = 256;
		GetMenuItemInfo(pollingMenu, i, TRUE, &itemInfo);
		itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
		if (i == pollSetting)
		{
			itemInfo.fState |= MFS_CHECKED;
			itemInfo.fState &= ~MFS_UNCHECKED;
		}
		else
		{
			itemInfo.fState &= ~MFS_CHECKED;
			itemInfo.fState |= MFS_UNCHECKED;
		}
		SetMenuItemInfo(pollingMenu, i, TRUE, &itemInfo);
	}
}

void LDViewWindow::selectPollingMenuItem(int index)
{
	int i;
	HMENU pollingMenu = getPollingMenu();
	MENUITEMINFO itemInfo;

	for (i = ID_FILE_POLLING_DISABLED; i <= ID_FILE_POLLING_BACKGROUND; i++)
	{
		itemInfo.cbSize = sizeof(MENUITEMINFO);
		itemInfo.fMask = MIIM_STATE;
		GetMenuItemInfo(pollingMenu, i, FALSE, &itemInfo);
		if (i == index)
		{
			itemInfo.fState |= MFS_CHECKED;
			itemInfo.fState &= ~MFS_UNCHECKED;
		}
		else
		{
			itemInfo.fState &= ~MFS_CHECKED;
			itemInfo.fState |= MFS_UNCHECKED;
		}
		SetMenuItemInfo(pollingMenu, i, FALSE, &itemInfo);
	}
	TCUserDefaults::setLongForKey(index - ID_FILE_POLLING_DISABLED, POLL_KEY,
		false);
	if (modelWindow)
	{
		modelWindow->setPollSetting(index - ID_FILE_POLLING_DISABLED);
	}
}

HMENU LDViewWindow::getParentOfMenuItem(HMENU hParentMenu, int itemId)
{
	int i, j;
	int parentCount = GetMenuItemCount(hParentMenu);
	HMENU retValue = 0;
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_SUBMENU | MIIM_ID;
	for (i = 0; i < parentCount && !retValue; i++)
	{
		GetMenuItemInfo(hParentMenu, i, TRUE, &itemInfo);
		if (itemInfo.hSubMenu)
		{
			bool found = false;
			int childCount;
			
			retValue = itemInfo.hSubMenu;
			childCount = GetMenuItemCount(retValue);
			for (j = 0; j < childCount && !found; j++)
			{
				GetMenuItemInfo(retValue, j, TRUE, &itemInfo);
				if (itemInfo.wID == (DWORD)itemId)
				{
					found = true;
				}
			}
			if (!found)
			{
				retValue = 0;
			}
		}
	}
	return retValue;
}

HMENU LDViewWindow::getPollingMenu(void)
{
	return getParentOfMenuItem(GetSubMenu(GetMenu(hWindow), 0),
		ID_FILE_POLLING_DISABLED);
/*
	HMENU fileMenu = GetSubMenu(GetMenu(hWindow), 0);
	int i;
	int count = GetMenuItemCount(fileMenu);
	HMENU pollingMenu = 0;
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_SUBMENU | MIIM_ID;
	for (i = 0; i < count && !pollingMenu; i++)
	{
		GetMenuItemInfo(fileMenu, i, TRUE, &itemInfo);
		if (itemInfo.hSubMenu)
		{
			pollingMenu = itemInfo.hSubMenu;
			GetMenuItemInfo(pollingMenu, 0, TRUE, &itemInfo);
			if (itemInfo.wID != ID_FILE_POLLING_DISABLED)
			{
				pollingMenu = 0;
			}
		}
	}
	return pollingMenu;
*/
}

void LDViewWindow::showHelp(void)
{
	char *helpPath = LDViewPreferences::getLDViewPath(
		TCLocalStrings::get("HelpHtml"), true);

	shellExecute(helpPath);
	delete helpPath;
}

void LDViewWindow::shellExecute(const char *filename)
{
	HINSTANCE executeHandle;

	setWaitCursor();
	executeHandle = ShellExecute(hWindow, NULL, filename, NULL, ".",
		SW_SHOWNORMAL);
	setArrowCursor();
	if ((INT_PTR)executeHandle <= 32)
	{
		char errorString[1024] = "";

		switch ((INT_PTR)executeHandle)
		{
			case 0:
			case SE_ERR_OOM:
				strcpy(errorString, TCLocalStrings::get("HelpHtmlOom"));
				break;
			case ERROR_FILE_NOT_FOUND:
				sprintf(errorString,
					TCLocalStrings::get("HelpHtmlFileNotFound"),
					filename);
				break;
			case ERROR_PATH_NOT_FOUND:
				sprintf(errorString,
					TCLocalStrings::get("HelpHtmlPathNotFound"),
					filename);
				break;
			case SE_ERR_ACCESSDENIED:
				sprintf(errorString, TCLocalStrings::get("HelpHtmlAccess"),
					filename);
				break;
			case SE_ERR_SHARE:
				sprintf(errorString, TCLocalStrings::get("HelpHtmlShare"),
					filename);
				break;
			default:
				sprintf(errorString, TCLocalStrings::get("HelpHtmlError"),
					filename);
				break;
		}
		stopAnimation();
		MessageBox(hWindow, errorString, TCLocalStrings::get("Error"),
			MB_OK | MB_ICONEXCLAMATION);
	}
}

void LDViewWindow::openRecentFile(int index)
{
	char* filename = recentFiles->stringAtIndex(index);

	if (filename)
	{
		openModel(filename);
	}
}

void LDViewWindow::setMenuEnabled(HMENU hParentMenu, int itemID, bool enabled,
								  BOOL byPosition)
{
	CUIWindow::setMenuEnabled(hParentMenu, itemID, enabled, byPosition);

	//MENUITEMINFO itemInfo;
	////BYTE tbState = 0;

	//itemInfo.cbSize = sizeof(itemInfo);
	//itemInfo.fMask = MIIM_STATE;
	//if (GetMenuItemInfo(hParentMenu, itemID, byPosition, &itemInfo))
	//{
	//	if (enabled)
	//	{
	//		itemInfo.fState &= ~MFS_DISABLED;
	//	}
	//	else
	//	{
	//		itemInfo.fState |= MFS_DISABLED;
	//	}
	//	itemInfo.fMask = MIIM_STATE;
	//	SetMenuItemInfo(hParentMenu, itemID, byPosition, &itemInfo);
	//}
	//if (enabled)
	//{
	//	tbState |= TBSTATE_ENABLED;
	//}
	if (toolbarStrip)
	{
		toolbarStrip->enableMainToolbarButton(itemID, enabled);
		//SendMessage(hToolbar, TB_SETSTATE, (WPARAM)itemID,
		//	MAKELONG(tbState, 0));
	}
	else
	{
		//int i;
		//int count;

		//populateTbButtonInfos();
		//count = (int)tbButtonInfos.size();
		//for (i = 0; i < count; i++)
		//{
		//	TbButtonInfo &buttonInfo = tbButtonInfos[i];

		//	if (buttonInfo.getCommandId() == itemID)
		//	{
		//		buttonInfo.setState((BYTE)(tbState |
		//			(buttonInfo.getState() & ~TBSTATE_ENABLED)));
		//		break;
		//	}
		//}
	}
}

void LDViewWindow::updateStepMenuItems(void)
{
	bool nextEnabled = false;
	bool prevEnabled = false;
	bool goToEnabled = false;

	if (modelIsLoaded())
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer->getNumSteps() > 1)
		{
			goToEnabled = true;
			if (modelViewer->getStep() < modelViewer->getNumSteps())
			{
				nextEnabled = true;
			}
			if (modelViewer->getStep() > 1)
			{
				prevEnabled = true;
			}
		}
	}
	setMenuEnabled(hStepMenu, ID_NEXT_STEP, nextEnabled);
	setMenuEnabled(hStepMenu, ID_LAST_STEP, nextEnabled);
	setMenuEnabled(hStepMenu, ID_PREV_STEP, prevEnabled);
	setMenuEnabled(hStepMenu, ID_FIRST_STEP, prevEnabled);
	setMenuEnabled(hStepMenu, ID_GOTO_STEP, goToEnabled);
}

void LDViewWindow::updateModelMenuItems(void)
{
	bool haveModel = modelIsLoaded();
	HMENU hViewingAngleMenu = getParentOfMenuItem(hViewMenu, ID_VIEW_FRONT);

	setMenuEnabled(hFileMenu, ID_FILE_SAVE, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_OPEN, !loading);
	setMenuEnabled(hFileMenu, ID_FILE_EXPORT, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_RELOAD, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_PRINT, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_PAGESETUP, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_VIEW_INFO, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_POV_CAMERA, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_PARTSLIST, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_MODELTREE, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_BOUNDINGBOX, haveModel);
	setMenuEnabled(hToolsMenu, ID_TOOLS_MPD, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_FRONT, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_BACK, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_LEFT, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_RIGHT, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_TOP, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_BOTTOM, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_SPECIFYLATLON, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_ISO, haveModel);
	setMenuEnabled(hViewingAngleMenu, ID_VIEW_SAVE_DEFAULT, haveModel);
	updateStepMenuItems();
}

bool LDViewWindow::modelIsLoaded(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer && modelViewer->getMainTREModel())
		{
			return true;
		}
	}
	return false;
}

LRESULT LDViewWindow::doMenuSelect(UINT menuID, UINT /*flags*/, HMENU hMenu)
{
//	debugPrintf("LDViewWindow::doMenuSelect(%d, 0x%04X, 0x%04X)\n", menuID,
//		flags, hMenu);
	if (hMenu == GetMenu(hWindow) && (menuID == 0 || menuID == 2))
	{
		// This shouldn't ever be necessary, but it can't hurt.
		updateModelMenuItems();
		if (menuID == 2)
		{
			// User selected the View menu; update standard sizes list.
			setupStandardSizes();
		}
	}
	return 1;
}

/*
void LDViewWindow::showDefaultMatrix(const char *matrixString,
									 const char *title)
{
	char buf[1024];

	sprintf(buf, "Hit OK to copy the following matrix to the clipboard:\n\n"
		"%s\n\n(You can use this on the command line with the -DefaultMatrix "
		"command line option.)", matrixString);
	if (MessageBox(hWindow, buf, title, MB_OKCANCEL) ==
		IDOK)
	{
		copyToClipboard(matrixString);
	}
}

void LDViewWindow::showTransformationMatrix(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			TCFloat matrix[16];
			TCFloat rotationMatrix[16];
			TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
			char matrixString[1024];
			LDLCamera &camera = modelViewer->getCamera();
			TCVector cameraPosition = camera.getPosition();

			memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
				sizeof(rotationMatrix));
			TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
			matrix[12] = cameraPosition[0];
			matrix[13] = cameraPosition[1];
			matrix[14] = cameraPosition[2];
			LDrawModelViewer::cleanupFloats(matrix);
			sprintf(matrixString,
				"%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,"
				"%.6g,%.6g,%.6g,%.6g",
				matrix[0], matrix[4], matrix[8], matrix[12],
				matrix[1], matrix[5], matrix[9], matrix[13],
				matrix[2], matrix[6], matrix[10], matrix[14],
				matrix[3], matrix[7], matrix[11], matrix[15]);
			showDefaultMatrix(matrixString, "Transformation Matrix");
		}
	}
}
*/

void LDViewWindow::showPovCamera(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			UCSTR userMessage;
			char *povCamera;

			stopAnimation();
			modelViewer->getPovCameraInfo(userMessage, povCamera);
			if (userMessage && povCamera)
			{
				if (messageBoxUC(hWindow, userMessage,
					TCLocalStrings::get(_UC("PovCameraTitle")), MB_OKCANCEL) ==
					IDOK)
				{
					copyToClipboard(povCamera);
				}
			}
			else
			{
				messageBoxUC(hWindow, TCLocalStrings::get(_UC("NoModelLoaded")),
					TCLocalStrings::get(_UC("Error")), MB_OK);
			}
			delete userMessage;
			delete povCamera;
		}
	}
}

void LDViewWindow::showViewInfo(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			ucstring message;
			ucstring commandLine;
			
			if (modelViewer->getViewInfo(message, commandLine))
			{
				stopAnimation();
				if (messageBoxUC(hWindow, message.c_str(),
					TCLocalStrings::get(_UC("ViewInfoTitle")), MB_OKCANCEL) == IDOK)
				{
					copyToClipboard(commandLine.c_str());
				}
			}
		}
	}
}

/*
void LDViewWindow::showRotationMatrix(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			TCFloat matrix[16];
			TCFloat rotationMatrix[16];
			TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
			char matrixString[1024];

			memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
				sizeof(rotationMatrix));
			TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
			LDrawModelViewer::cleanupFloats(matrix);
			sprintf(matrixString,
				"%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g", matrix[0],
				matrix[4], matrix[8], matrix[1], matrix[5], matrix[9],
				matrix[2], matrix[6], matrix[10]);
			showDefaultMatrix(matrixString, "Rotation Matrix");
		}
	}
}
*/

void LDViewWindow::showLDrawCommandLine(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			char buf[1024];

			stopAnimation();
			modelViewer->getLDGLiteCommandLine(buf, sizeof(buf));
			MessageBox(hWindow, buf, TCLocalStrings::get("LDrawCommandLine"),
				MB_OK);
			copyToClipboard(buf);
		}
	}
}

BOOL LDViewWindow::doDialogSize(HWND hDlg, WPARAM sizeType, int newWidth,
							   int newHeight)
{
//	debugPrintf("LDViewWindow::doDialogSize(%d, %d, %d)\n", sizeType, newWidth,
//		newHeight);
	if (hDlg == hOpenGLInfoWindow)
	{
		SendMessage(hOpenGLStatusBar, WM_SIZE, sizeType,
			MAKELPARAM(newWidth, newHeight));
		openGLInfoWindoResizer->resize(newWidth, newHeight);
	}
	return FALSE;
}

LRESULT LDViewWindow::showOpenGLDriverInfo(void)
{
	if (!hOpenGLInfoWindow)
	{
//		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();
		int numOpenGlExtensions;
		UCSTR openGlMessage = modelWindow->getModelViewer()->
			getOpenGLDriverInfo(numOpenGlExtensions);
		const char *wglExtensionsString =
			LDVExtensionsSetup::getWglExtensionsString();
		UCSTR wglExtensionsList;
		size_t len;
		UCSTR message;
		int parts[2] = {100, -1};
		UCCHAR buf[128];
		int count = 0;

		if (wglExtensionsString)
		{
			char *temp = stringByReplacingSubstring(wglExtensionsString,
				" ", "\r\n");

			stripCRLF(temp);
			count = countStringLines(temp);
			wglExtensionsList = mbstoucstring(temp);
			delete temp;
		}
		else
		{
			wglExtensionsList =
				copyString(TCLocalStrings::get(_UC("*None*")));
		}
		len = ucstrlen(openGlMessage) + ucstrlen(wglExtensionsList) + 128;
		message = new UCCHAR[len];
		sucprintf(message, len, TCLocalStrings::get(_UC("OpenGl+WglInfo")),
			openGlMessage, wglExtensionsList);
		hOpenGLInfoWindow = createDialog(IDD_OPENGL_INFO);
		sendDlgItemMessageUC(hOpenGLInfoWindow, IDC_OPENGL_INFO, WM_SETTEXT, 0,
			(LPARAM)message);
		hOpenGLStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE |
			SBARS_SIZEGRIP, "", hOpenGLInfoWindow, ID_TOOLBAR);
		setStatusBarParts(hOpenGLStatusBar, 2, parts);
		if (numOpenGlExtensions == 1)
		{
			ucstrcpy(buf, TCLocalStrings::get(_UC("OpenGl1Extension")));
		}
		else
		{
			sucprintf(buf, COUNT_OF(buf),
				TCLocalStrings::get(_UC("OpenGlnExtensions")),
				numOpenGlExtensions);
		}
		sendMessageUC(hOpenGLStatusBar, SB_SETTEXT, 0, (LPARAM)buf);
		if (count == 1)
		{
			ucstrcpy(buf, TCLocalStrings::get(_UC("OpenGl1WglExtension")));
		}
		else
		{
			sucprintf(buf, COUNT_OF(buf),
				TCLocalStrings::get(_UC("OpenGlnWglExtensions")), count);
		}
		sendMessageUC(hOpenGLStatusBar, SB_SETTEXT, 1, (LPARAM)buf);
		calcSystemSizes();
		if (openGLInfoWindoResizer)
		{
			openGLInfoWindoResizer->release();
		}
		openGLInfoWindoResizer = new CUIWindowResizer;
		openGLInfoWindoResizer->setHWindow(hOpenGLInfoWindow);
		openGLInfoWindoResizer->addSubWindow(IDC_OPENGL_INFO,
			CUISizeHorizontal | CUISizeVertical);
		openGLInfoWindoResizer->addSubWindow(IDOK, CUIFloatLeft | CUIFloatTop);
		delete openGlMessage;
		delete message;
		delete wglExtensionsList;
	}
	ShowWindow(hOpenGLInfoWindow, SW_SHOW);
	return 0;
}

//void LDViewWindow::setMenuRadioCheck(HMENU hParentMenu, UINT uItem, bool checked)
//{
//	setMenuCheck(hParentMenu, uItem, checked, true);
//}

//bool LDViewWindow::getMenuCheck(HMENU hParentMenu, UINT uItem)
//{
//	MENUITEMINFO itemInfo;
//
//	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
//	itemInfo.cbSize = sizeof(MENUITEMINFO);
//	itemInfo.fMask = MIIM_STATE;
//	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
//	return (itemInfo.fState & MFS_CHECKED) != 0;
//}

//void LDViewWindow::setMenuCheck(HMENU hParentMenu, UINT uItem, bool checked,
//								bool radio)
//{
//	MENUITEMINFO itemInfo;
//	char title[256];
//
//	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
//	itemInfo.cbSize = sizeof(MENUITEMINFO);
//	itemInfo.fMask = MIIM_STATE | MIIM_TYPE;
//	itemInfo.dwTypeData = title;
//	itemInfo.cch = 256;
//	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
//	if (checked)
//	{
//		itemInfo.fState |= MFS_CHECKED;
//		itemInfo.fState &= ~MFS_UNCHECKED;
//	}
//	else
//	{
//		itemInfo.fState &= ~MFS_CHECKED;
//		itemInfo.fState |= MFS_UNCHECKED;
//	}
//	itemInfo.fType = MFT_STRING;
//	if (radio)
//	{
//		itemInfo.fType |= MFT_RADIOCHECK;
//	}
//	SetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
//}

LRESULT LDViewWindow::switchPovCameraAspect(bool saveSetting /*= true*/)
{
	bool checked = getMenuCheck(hToolsMenu, ID_TOOLS_POV_CAMERA_ASPECT);
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	setMenuCheck(hToolsMenu, ID_TOOLS_POV_CAMERA_ASPECT, !checked);
	if (modelViewer != NULL)
	{
		modelViewer->setPovCameraAspect(!checked, saveSetting);
	}
	return 0;
}

LRESULT LDViewWindow::switchToViewMode(
	LDrawModelViewer::ViewMode viewMode,
	bool saveSetting)
{
	setMenuRadioCheck(hViewMenu, ID_VIEW_EXAMINE, false);
	setMenuRadioCheck(hViewMenu, ID_VIEW_FLYTHROUGH, false);
	setMenuRadioCheck(hViewMenu, ID_VIEW_WALK, false);
	setMenuCheck(hViewMenu, ID_VIEW_EXAMINE_LAT_LONG, false);
	setMenuCheck(hViewMenu, ID_VIEW_KEEPRIGHTSIDEUP, false);
	modelWindow->setViewMode((LDInputHandler::ViewMode)viewMode, examineLatLong,
		saveSetting);
	updateStatusParts();
	switch (viewMode)
	{
	case LDrawModelViewer::VMExamine:
		setMenuRadioCheck(hViewMenu, ID_VIEW_EXAMINE, true);
		setMenuCheck(hViewMenu, ID_VIEW_EXAMINE_LAT_LONG, examineLatLong);
		break;
	case LDrawModelViewer::VMFlyThrough:
		setMenuRadioCheck(hViewMenu, ID_VIEW_FLYTHROUGH, true);
		setMenuCheck(hViewMenu, ID_VIEW_KEEPRIGHTSIDEUP, keepRightSideUp);
		modelWindow->setKeepRightSideUp(keepRightSideUp, saveSetting);
		break;
	case LDrawModelViewer::VMWalk:
		setMenuRadioCheck(hViewMenu, ID_VIEW_WALK, true);
		setMenuCheck(hViewMenu, ID_VIEW_KEEPRIGHTSIDEUP, true);
		break;
	}
	if (toolbarStrip)
	{
		toolbarStrip->viewModeReflect();
	}
	return 0;
}

BOOL LDViewWindow::doDialogNotify(HWND hDlg, int controlId,
								  LPNMHDR notification)
{
//	debugPrintf("LDViewWindow::doDialogNotify: 0x%04X, 0x%04X, 0x%04x\n", hDlg,
//		controlId, notification->code);
	if (hDlg)
	{
		if (hDlg == hExtraDirsWindow)
		{
			if (controlId >= 42 && controlId <= 45)
			{
				switch (notification->code)
				{
				case TTN_GETDISPINFOUC:
					{
						LPNMTTDISPINFOUC dispInfo = (LPNMTTDISPINFOUC)notification;
						bool gotTooltip = true;

						switch (controlId)
						{
						case 42:
							ucstrcpy(dispInfo->szText,
								ls(_UC("AddExtraDirTooltip")));
							break;
						case 43:
							ucstrcpy(dispInfo->szText,
								ls(_UC("RemoveExtraDirTooltip")));
							break;
						case 44:
							ucstrcpy(dispInfo->szText,
								ls(_UC("MoveExtraDirUpTooltip")));
							break;
						case 45:
							ucstrcpy(dispInfo->szText,
								ls(_UC("MoveExtraDirDownTooltip")));
							break;
						default:
							gotTooltip = false;
							break;
						}
						if (gotTooltip && CUIThemes::isThemeLibLoaded())
						{
							// Turning off theme support in the tooltip makes it
							// work properly.  With theme support on, it gets
							// erased by the OpenGL window immediately after
							// being drawn if it overlaps the OpenGL window.
							// Haven't the foggiest why this happens, but
							// turning off theme support solves the problem.
							// This has to be done ever time the tooltip is
							// about to pop up. Not sure why that is either, but
							// it is.
							CUIThemes::setWindowTheme(notification->hwndFrom,
								NULL, L"");
						}
						dispInfo->hinst = NULL;
					}
					break;
				case WM_COMMAND:
					debugPrintf("WM_COMMAND\n");
					break;
				}
			}
		}
	}
	return FALSE;
}

void LDViewWindow::populateExtraDirsListBox(void)
{
	int i;
	int count = extraSearchDirs->getCount();

	SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_LIST, LB_RESETCONTENT, 0, 0);
	for (i = 0; i < count; i++)
	{
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_LIST, LB_ADDSTRING, 0,
			(LPARAM)(*extraSearchDirs)[i]);
	}
	if (count)
	{
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_LIST, LB_SELECTSTRING,
			0, (LPARAM)(*extraSearchDirs)[0]);
	}
}

void LDViewWindow::updateExtraDirsEnabled(void)
{
	int index = (int)SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

	if (index == LB_ERR)
	{
		int i;

		for (i = 1; i < 4; i++)
		{
			SendMessage(hExtraDirsToolbar, TB_SETSTATE, 42 + i,
				MAKELONG(0, 0));
		}
	}
	else
	{
		// There's a selection; therefore it can be deleted.
		SendMessage(hExtraDirsToolbar, TB_SETSTATE, 43,
			MAKELONG(TBSTATE_ENABLED, 0));
		if (index == 0)
		{
			// Can't move up from the top
			SendMessage(hExtraDirsToolbar, TB_SETSTATE, 44, MAKELONG(0, 0));
		}
		else
		{
			SendMessage(hExtraDirsToolbar, TB_SETSTATE, 44,
				MAKELONG(TBSTATE_ENABLED, 0));
		}
		if (index == extraSearchDirs->getCount() - 1)
		{
			// Can't move down from the bottom
			SendMessage(hExtraDirsToolbar, TB_SETSTATE, 45, MAKELONG(0, 0));
		}
		else
		{
			SendMessage(hExtraDirsToolbar, TB_SETSTATE, 45,
				MAKELONG(TBSTATE_ENABLED, 0));
		}
	}
}

void LDViewWindow::chooseExtraDirs(void)
{
	if (!hExtraDirsWindow)
	{
		TBBUTTON buttons[4];
		char buttonTitle[128];
		int i;
		RECT tbRect;

		memset(buttonTitle, 0, sizeof(buttonTitle));
		strcpy(buttonTitle, "");
		ModelWindow::initCommonControls(ICC_WIN95_CLASSES);
		hExtraDirsWindow = createDialog(IDD_EXTRA_DIRS);
		hExtraDirsToolbar = GetDlgItem(hExtraDirsWindow, IDC_ESD_TOOLBAR);
		int tbImageSize = scalePoints(16);
		int tbButtonSize = scalePoints(25);
		SIZE tbImageFullSize = { tbImageSize * 4, tbImageSize };
		UINT flags = CUIScaler::imageListCreateFlags();
		hExtraDirsImageList = ImageList_Create(tbImageSize, tbImageSize, flags,
			4, 0);
		addImageToImageList(hExtraDirsImageList, IDR_EXTRA_DIRS_TOOLBAR,
			tbImageFullSize, getScaleFactor());
		SendMessage(hExtraDirsToolbar, TB_SETIMAGELIST, 0,
			(LPARAM)hExtraDirsImageList);
		hExtraDirsList = GetDlgItem(hExtraDirsWindow, IDC_ESD_LIST);
		populateExtraDirsListBox();
		GetClientRect(hExtraDirsToolbar, &tbRect);
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR,
			TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0); 
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_SETINDENT,
			tbRect.right - tbRect.left - tbButtonSize * 4, 0);
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_SETBUTTONWIDTH,
			0, MAKELONG(tbButtonSize, tbButtonSize));
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_ADDSTRING,
			0, (LPARAM)buttonTitle);
		for (i = 0; i < 4; i++)
		{
			buttons[i].iBitmap = i;
			buttons[i].idCommand = 42 + i;
			buttons[i].fsState = TBSTATE_ENABLED;
			buttons[i].fsStyle = TBSTYLE_BUTTON;
			buttons[i].dwData = (DWORD_PTR)this;
			buttons[i].iString = -1;
		}
		if (extraSearchDirs->getCount() < 2)
		{
			// Can't move down either.
			buttons[3].fsState = 0;
		}
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_ADDBUTTONS,
			4, (LPARAM)buttons);
		updateExtraDirsEnabled();
	}
	ShowWindow(hExtraDirsWindow, SW_SHOW);
}

void LDViewWindow::chooseNewLDrawDir(void)
{
	char *oldDir = getLDrawDir();

	if (!verifyLDrawDir(true))
	{
		if (oldDir)
		{
			TCUserDefaults::setPathForKey(oldDir, LDRAWDIR_KEY, false);
			LDLModel::setLDrawDir(oldDir);
		}
	}
	delete oldDir;
}

void LDViewWindow::reshapeModelWindow(void)
{
	if (modelWindow)
	{
		int newX = 0;
		int newY = getToolbarHeight();
		int newWidth = width;
		int newHeight = height - getDockedHeight();

		SetWindowPos(modelWindow->getHWindow(), HWND_TOP, newX, newY, newWidth,
			newHeight, 0);
/*
		SetWindowPos(modelWindow->getHWindow(), HWND_TOP, 2, 2, newWidth - 4,
			newHeight - 4, 0);
		SetWindowPos(hFrameWindow, HWND_BOTTOM, 0, 0, newWidth, newHeight,
			SWP_DRAWFRAME | SWP_NOZORDER);
//		MoveWindow(hFrameWindow, 0, 0, newWidth, newHeight, TRUE);
		RedrawWindow(hFrameWindow, NULL, NULL, RDW_FRAME | RDW_INVALIDATE |
			RDW_INTERNALPAINT | RDW_UPDATENOW);
*/
	}
}

void LDViewWindow::removeStatusBar(void)
{
	HWND hActiveWindow = GetActiveWindow();
	bool needActive = hActiveWindow == hStatusBar;

	debugPrintf(2, "0x%08X: removing status bar: 0x%08X\n", hWindow, hStatusBar);
	if (modelWindow)
	{
		modelWindow->setStatusBar(NULL);
		modelWindow->setProgressBar(NULL);
	}
	DestroyWindow(hStatusBar);
	hStatusBar = NULL;
	reshapeModelWindow();
	if (needActive)
	{
		SetActiveWindow(hWindow);
	}
}

int LDViewWindow::getDockedHeight(void)
{
	return getToolbarHeight() + getStatusBarHeight();
}

int LDViewWindow::getModelWindowTop(void)
{
	return getToolbarHeight();
}

int LDViewWindow::getToolbarHeight(void)
{
	if (toolbarStrip)
	{
		return toolbarStrip->getHeight();
	}
	//else if (hToolbar)
	//{
	//	RECT rect;

	//	GetWindowRect(hToolbar, &rect);
	//	return rect.bottom - rect.top;
	//}
	else
	{
		return 0;
	}
}

int LDViewWindow::getStatusBarHeight(void)
{
	if (hStatusBar)
	{
		RECT rect;

		GetWindowRect(hStatusBar, &rect);
		return rect.bottom - rect.top;
	}
	else
	{
		return 0;
	}
}

void LDViewWindow::removeToolbar(void)
{
	if (toolbarStrip)
	{
		HWND hActiveWindow = GetActiveWindow();
		bool needActive = hActiveWindow == toolbarStrip->getMainToolbar();

		//debugPrintf(2, "0x%08X: removing toolbar: 0x%08X\n", hWindow, hToolbar);
		//DestroyWindow(hToolbar);
		//hToolbar = NULL;
		TCObject::release(toolbarStrip);
		toolbarStrip = NULL;
		reshapeModelWindow();
		if (needActive)
		{
			SetActiveWindow(hWindow);
		}
	}
}

LRESULT LDViewWindow::switchKeepRightSideUp(void)
{
	keepRightSideUp = !keepRightSideUp;
	reflectViewMode(false);
	modelWindow->setKeepRightSideUp(keepRightSideUp);
	return 0;
}

void LDViewWindow::addToolbar(void)
{
	createToolbar();
	reshapeModelWindow();
}

LRESULT LDViewWindow::switchToolbar(void)
{
	showToolbar = !showToolbar;
	TCUserDefaults::setLongForKey(showToolbar ? 1 : 0, TOOLBAR_KEY, false);
	reflectToolbar();
	return 0;
}

void LDViewWindow::addStatusBar(void)
{
	createStatusBar();
	reshapeModelWindow();
}

LRESULT LDViewWindow::switchStatusBar(void)
{
	showStatusBar = !showStatusBar;
	TCUserDefaults::setBoolForKey(showStatusBar, STATUS_BAR_KEY, false);
	reflectStatusBar();
	return 0;
}

void LDViewWindow::reflectTopmost(void)
{
	bool oldTopmost = getMenuCheck(hViewMenu, ID_VIEW_ALWAYSONTOP);

	if (oldTopmost != topmost)
	{
		HWND hPlaceFlag = HWND_NOTOPMOST;
		RECT rect;

		GetWindowRect(hWindow, &rect);
		if (topmost)
		{
			setMenuCheck(hViewMenu, ID_VIEW_ALWAYSONTOP, true);
			hPlaceFlag = HWND_TOPMOST;
		}
		else
		{
			setMenuCheck(hViewMenu, ID_VIEW_ALWAYSONTOP, false);
		}
		SetWindowPos(hWindow, hPlaceFlag, rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top, 0);
	}
}

LRESULT LDViewWindow::switchTopmost(void)
{
	topmost = !topmost;
	TCUserDefaults::setBoolForKey(topmost, TOPMOST_KEY, false);
	reflectTopmost();
	toolbarChecksReflect();
	return 0;
}

void LDViewWindow::reflectVisualStyle(void)
{
	if (CUIThemes::isThemeLibLoaded())
	{
		setMenuCheck(hViewMenu, ID_VIEW_VISUALSTYLE, visualStyleEnabled);
	}
}

bool LDViewWindow::isTopmost(void)
{
	return topmost;
}

LRESULT LDViewWindow::switchVisualStyle(void)
{
	visualStyleEnabled = !visualStyleEnabled;
	TCUserDefaults::setBoolForKey(visualStyleEnabled, VISUAL_STYLE_ENABLED_KEY,
		false);
	reflectVisualStyle();
	return 0;
}

LRESULT LDViewWindow::switchExamineLatLong(void)
{
//	LDrawModelViewer::ExamineMode examineMode = LDrawModelViewer::EMFree;
	examineLatLong = !examineLatLong;
	//if (examineLatLong)
	//{
	//	examineMode = LDrawModelViewer::EMLatLong;
	//}
	//TCUserDefaults::setLongForKey(examineMode, EXAMINE_MODE_KEY, false);
	reflectViewMode();
	updateStatusParts();
	toolbarChecksReflect();
	modelWindow->forceRedraw();
	return 0;
}

#if defined(USE_CPP11) || !defined(_NO_BOOST)

void LDViewWindow::doLibraryUpdateFinished(int finishType)
{
	if (libraryUpdater)
	{
		UCCHAR statusText[1024] = _UC("");

		EnableWindow(hUpdateOkButton, TRUE);
		EnableWindow(hUpdateCancelButton, FALSE);
		if (libraryUpdater->getError() && ucstrlen(libraryUpdater->getError()))
		{
			sucprintf(statusText, COUNT_OF(statusText), _UC("%s:\n%s"),
				TCLocalStrings::get(_UC("LibraryUpdateError")),
				libraryUpdater->getError());
		}
		switch (finishType)
		{
		case LIBRARY_UPDATE_FINISHED:
			libraryUpdateFinished = true;
			ucstrcpy(statusText,
				TCLocalStrings::get(_UC("LibraryUpdateComplete")));
			break;
		case LIBRARY_UPDATE_CANCELED:
			ucstrcpy(statusText,
				TCLocalStrings::get(_UC("LibraryUpdateCanceled")));
			break;
		case LIBRARY_UPDATE_NONE:
			ucstrcpy(statusText,
				TCLocalStrings::get(_UC("LibraryUpdateUnnecessary")));
			break;
		}
		libraryUpdater->release();
		libraryUpdater = NULL;
		if (ucstrlen(statusText))
		{
			sendMessageUC(hUpdateStatus, WM_SETTEXT, 0, (LPARAM)statusText);
		}
	}
}

void LDViewWindow::createLibraryUpdateWindow(void)
{
	if (!hLibraryUpdateWindow)
	{
		hLibraryUpdateWindow = createDialog(IDD_LIBRARY_UPDATES, FALSE);
	}
	hUpdateProgressBar = GetDlgItem(hLibraryUpdateWindow, IDC_UPDATE_PROGRESS);
	hUpdateStatus = GetDlgItem(hLibraryUpdateWindow, IDC_UPDATE_STATUS_FIELD);
	hUpdateCancelButton = GetDlgItem(hLibraryUpdateWindow, IDCANCEL);
	hUpdateOkButton = GetDlgItem(hLibraryUpdateWindow, IDOK);
	sendMessageUC(hUpdateStatus, WM_SETTEXT, 0,
		(LPARAM)TCLocalStrings::get(_UC("CheckingForLibraryUpdates")));
	SendMessage(hUpdateProgressBar, PBM_SETPOS, 0, 0);
	RECT luRect;
	RECT rect;
	int spacing = GetSystemMetrics(SM_CYCAPTION) +
		GetSystemMetrics(SM_CXSIZEFRAME);
	GetWindowRect(hLibraryUpdateWindow, &luRect);
	GetWindowRect(hWindow, &rect);
	MoveWindow(hLibraryUpdateWindow, rect.left + spacing, rect.top + spacing,
		luRect.right - luRect.left, luRect.bottom - luRect.top, TRUE);
}

void LDViewWindow::showLibraryUpdateWindow(bool initialInstall)
{
	if (!hLibraryUpdateWindow)
	{
		createLibraryUpdateWindow();
	}
	EnableWindow(hUpdateOkButton, FALSE);
	EnableWindow(hUpdateCancelButton, TRUE);
	if (initialInstall)
	{
		runDialogModal(hLibraryUpdateWindow, true);
	}
	else
	{
		ShowWindow(hLibraryUpdateWindow, SW_SHOW);
	}
}

bool LDViewWindow::installLDraw(void)
{
	if (libraryUpdater)
	{
		debugPrintf("Ack!!! Initial install: already have libraryUpdater.\n");
		return false;
	}
	else
	{
		char *ldrawParentDir = getLDrawDir();
		char *ldrawDir = copyString(ldrawParentDir, 10);
		char originalDir[MAX_PATH];

		libraryUpdateFinished = false;
		strcat(ldrawDir, "\\LDRAW");
		GetCurrentDirectory(sizeof(originalDir), originalDir);
		if (SetCurrentDirectory(ldrawDir))
		{
			SetCurrentDirectory(originalDir);
		}
		else
		{
			CreateDirectory(ldrawDir, NULL);
		}
		libraryUpdater = new LDLibraryUpdater;
		libraryUpdateCanceled = false;
		libraryUpdater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
		libraryUpdater->setLdrawDir(ldrawDir);
		libraryUpdater->installLDraw();
		showLibraryUpdateWindow(true);
		while (libraryUpdater)
		{
			if (libraryUpdateCanceled)
			{
				doLibraryUpdateFinished(LIBRARY_UPDATE_CANCELED);
				libraryUpdateCanceled = false;
			}
			Sleep(100);
		}
		if (libraryUpdateFinished)
		{
			LDLModel::setLDrawDir(ldrawDir);
		}
		delete ldrawDir;
		return libraryUpdateFinished;
	}
}

void LDViewWindow::checkForLibraryUpdates(void)
{
	stopAnimation();
	if (libraryUpdater)
	{
		showLibraryUpdateWindow(false);
//		MessageBox(hWindow, TCLocalStrings::get("LibraryUpdateAlready"),
//			TCLocalStrings::get("Error"), MB_OK);
	}
	else
	{
		libraryUpdater = new LDLibraryUpdater;
		char *ldrawDir = getLDrawDir();
		UCSTR updateCheckError = NULL;

		libraryUpdateCanceled = false;
		libraryUpdater->setLibraryUpdateKey(LAST_LIBRARY_UPDATE_KEY);
		libraryUpdater->setLdrawDir(ldrawDir);
		delete ldrawDir;
		if (libraryUpdater->canCheckForUpdates(updateCheckError))
		{
			showLibraryUpdateWindow(false);
			libraryUpdater->checkForUpdates();
		}
		else
		{
			messageBoxUC(hWindow, updateCheckError, _UC("LDView"),
				MB_OK | MB_ICONWARNING);
			delete updateCheckError;
		}
	}
}

#endif // !_NO_BOOST

void LDViewWindow::progressAlertCallback(TCProgressAlert *alert)
{
#if defined(USE_CPP11) || !defined(_NO_BOOST)
	if (alert && strcmp(alert->getSource(), LD_LIBRARY_UPDATER) == 0)
	{
		debugPrintf("Updater progress (%s): %f\n", alert->getMessage(),
			alert->getProgress());
		sendMessageUC(hUpdateStatus, WM_SETTEXT, 0,
			(LPARAM)alert->getMessageUC());
		SendMessage(hUpdateProgressBar, PBM_SETPOS,
			(int)(alert->getProgress() * 100), 0);
		if (alert->getProgress() == 1.0f)
		{
			if (alert->getExtraInfo())
			{
				if (strcmp((*(alert->getExtraInfo()))[0], "None") == 0)
				{
					PostMessage(hWindow, WM_COMMAND,
						MAKELONG(BN_CLICKED, LIBRARY_UPDATE_NONE), 0);
				}
				else
				{
					PostMessage(hWindow, WM_COMMAND,
						MAKELONG(BN_CLICKED, LIBRARY_UPDATE_FINISHED), 0);
				}
			}
			else
			{
				PostMessage(hWindow, WM_COMMAND,
					MAKELONG(BN_CLICKED, LIBRARY_UPDATE_CANCELED), 0);
			}
		}
		else if (alert->getProgress() == 2.0f)
		{
			PostMessage(hWindow, WM_COMMAND,
				MAKELONG(BN_CLICKED, LIBRARY_UPDATE_ERROR), 0);
		}
		if (libraryUpdateCanceled)
		{
			alert->abort();
		}
	}
	else
#endif // !_NO_BOOST
	if (alert && strcmp(alert->getSource(), "TCImage") != 0)
	{
		//if (alert->getProgress() == 2.0 && hDeactivatedTooltip != NULL)
		if (alert->getProgress() == 2.0 && toolbarStrip)
		{
			toolbarStrip->activateDeactivatedTooltip();
			//SendMessage(hDeactivatedTooltip, TTM_ACTIVATE, 1, 0);
		}
	}
}

LRESULT LDViewWindow::doCommand(int itemId, int notifyCode, HWND controlHWnd)
{
//	char* message = NULL;

	if (modelWindow && controlHWnd == modelWindow->getHPrefsWindow())
	{
		SendMessage(modelWindow->getHWindow(), WM_COMMAND,
			MAKELONG(BN_CLICKED, notifyCode),
			(LPARAM)modelWindow->getHPrefsWindow());
	}
//	debugPrintf("LDViewWindow::doCommand(%d, %0x%08X, 0x%04X)\n", itemId,
//		notifyCode, controlHWnd);
	switch (itemId)
	{
		case ID_EDIT_PREFERENCES:
			modelWindow->showPreferences();
			return 0;
		case ID_FILE_OPEN:
			openModel();
			return 0;
		case ID_FILE_PRINT:
			printModel();
			return 0;
		case ID_FILE_PAGESETUP:
			pageSetup();
			return 0;
		case ID_FILE_SAVE:
			saveSnapshot();
			return 0;
		case ID_FILE_EXPORT:
			exportModel();
			return 0;
		case ID_FILE_LDRAWDIR:
			chooseNewLDrawDir();
			return 0;
		case ID_FILE_EXTRADIRS:
			chooseExtraDirs();
			return 0;
		case ID_FILE_CANCELLOAD:
			if (modelWindow)
			{
				modelWindow->setCancelLoad();
			}
			return 0;
		case ID_FILE_EXIT:
			shutdown();
			return 0;
		case ID_FILE_CHECKFORLIBUPDATES:
#if defined(USE_CPP11) || !defined(_NO_BOOST)
			checkForLibraryUpdates();
#endif // !_NO_BOOST
			return 0;
		case ID_VIEW_FULLSCREEN:
			switchModes();
			return 0;
		case ID_VIEW_RESET:
			resetView();
			return 0;
		case ID_VIEW_DEFAULT:
			resetView();
			return 0;
		case ID_VIEW_FRONT:
			resetView(LDVAngleFront);
			return 0;
		case ID_VIEW_BACK:
			resetView(LDVAngleBack);
			return 0;
		case ID_VIEW_LEFT:
			resetView(LDVAngleLeft);
			return 0;
		case ID_VIEW_RIGHT:
			resetView(LDVAngleRight);
			return 0;
		case ID_VIEW_TOP:
			resetView(LDVAngleTop);
			return 0;
		case ID_VIEW_BOTTOM:
			resetView(LDVAngleBottom);
			return 0;
		case ID_VIEW_SPECIFYLATLON:
			return specifyLatLon();
		case ID_VIEW_ISO:
			resetView(LDVAngleIso);
			return 0;
		case ID_VIEW_SAVE_DEFAULT:
			saveDefaultView();
			return 0;
		case ID_VIEW_ZOOMTOFIT:
			zoomToFit();
			return 0;
		case ID_VIEW_RIGHTSIDEUP:
			rightSideUp();
			return 0;
/*
		case ID_VIEW_RESET_DEFAULT:
			resetDefaultView();
			return 0;
			break;
*/
		case ID_VIEW_STATUSBAR:
			return switchStatusBar();
		case ID_VIEW_TOOLBAR:
			return switchToolbar();
		case ID_VIEW_ALWAYSONTOP:
			return switchTopmost();
		case ID_VIEW_VISUALSTYLE:
			return switchVisualStyle();
		case ID_VIEW_EXAMINE:
			return switchToViewMode(LDrawModelViewer::VMExamine);
		case ID_VIEW_EXAMINE_LAT_LONG:
			return switchExamineLatLong();
		case ID_VIEW_FLYTHROUGH:
			return switchToViewMode(LDrawModelViewer::VMFlyThrough);
		case ID_VIEW_WALK:
			return switchToViewMode(LDrawModelViewer::VMWalk);
		case ID_VIEW_KEEPRIGHTSIDEUP:
			return switchKeepRightSideUp();
		case ID_TOOLS_ERRORS:
			modelWindow->showErrors();
			return 0;
		case ID_TOOLS_VIEW_INFO:
			showViewInfo();
			return 0;
		case ID_TOOLS_POV_CAMERA:
			showPovCamera();
			return 0;
		case ID_TOOLS_POV_CAMERA_ASPECT:
			return switchPovCameraAspect();
		case ID_TOOLS_PARTSLIST:
			return generatePartsList();
		case ID_TOOLS_MODELTREE:
			return showModelTree();
		case ID_TOOLS_BOUNDINGBOX:
			return toggleBoundingBox();
		case ID_TOOLS_MPD:
			return showMpd();
/*
		case ID_VIEW_TRANS_MATRIX:
			showTransformationMatrix();
			return 0;
*/
/*
		case ID_VIEW_LDRAWCOMMANDLINE:
			showLDrawCommandLine();
			return 0;
*/
		case ID_FILE_RELOAD:
			modelWindow->reload();
			//modelWindow->update();
			return 0;
		case ID_HELP_ABOUT:
			showAboutBox();
			return 0;
		case ID_HELP_CONTENTS:
			if (!fullScreenActive)
			{
				showHelp();
				return 0;
			}
			break;
		case ID_HELP_OPENGL_INFO:
			return showOpenGLDriverInfo();
/*
		case ID_HELP_OPENGLINFO_VENDOR:
			message = (char*)glGetString(GL_VENDOR);
			break;
		case ID_HELP_OPENGLINFO_RENDERER:
			message = (char*)glGetString(GL_RENDERER);
			break;
		case ID_HELP_OPENGLINFO_VERSION:
			message = (char*)glGetString(GL_VERSION);
			break;
		case ID_HELP_OPENGLINFO_EXTENSIONS:
			message = (char*)glGetString(GL_EXTENSIONS);
			break;
		case ID_HELP_OPENGLINFO_WGLEXTENSIONS:
			message = LDVExtensionsSetup::getWglExtensions();
			if (!message)
			{
				message = "None";
			}
			break;
*/
		case BN_CLICKED:
#if defined(USE_CPP11) || !defined(_NO_BOOST)
			switch (notifyCode)
			{
			case LIBRARY_UPDATE_FINISHED:
			case LIBRARY_UPDATE_CANCELED:
			case LIBRARY_UPDATE_NONE:
			case LIBRARY_UPDATE_ERROR:
				doLibraryUpdateFinished(notifyCode);
				return 0;
			}
#endif //_NO_BOOST
			break;
		//case IDC_WIREFRAME:
		//	doWireframe();
		//	break;
		//case IDC_SEAMS:
		//	doSeams();
		//	break;
		//case IDC_HIGHLIGHTS:
		//	doEdges();
		//	break;
		//case IDC_PRIMITIVE_SUBSTITUTION:
		//	doPrimitiveSubstitution();
		//	break;
		//case IDC_LIGHTING:
		//	doLighting();
		//	break;
		//case IDC_BFC:
		//	doBfc();
		//	break;
		//case ID_WIREFRAME_FOG:
		//	doFog();
		//	break;
		//case ID_WIREFRAME_REMOVEHIDDENLINES:
		//	doRemoveHiddenLines();
		//	break;
		//case ID_EDGES_SHOWEDGESONLY:
		//	doShowEdgesOnly();
		//	break;
		//case ID_EDGES_CONDITIONALLINES:
		//	doConditionalLines();
		//	break;
		//case ID_EDGES_HIGHQUALITY:
		//	doHighQualityEdges();
		//	break;
		//case ID_EDGES_ALWAYSBLACK:
		//	doAlwaysBlack();
		//	break;
		//case ID_PRIMITIVES_TEXTURESTUDS:
		//	doTextureStuds();
		//	break;
		//case ID_LIGHTING_HIGHQUALITY:
		//	doQualityLighting();
		//	break;
		//case ID_LIGHTING_SUBDUED:
		//	doSubduedLighting();
		//	break;
		//case ID_LIGHTING_SPECULARHIGHLIGHT:
		//	doSpecularHighlight();
		//	break;
		//case ID_LIGHTING_ALTERNATESETUP:
		//	doAlternateLighting();
		//	break;
		//case ID_LIGHTING_DRAWDATS:
		//	doDrawLightDats();
		//	break;
		//case ID_LIGHTING_OPTIONALMAIN:
		//	doOptionalStandardLight();
		//	break;
		//case ID_BFC_REDBACKFACES:
		//	doRedBackFaces();
		//	break;
		//case ID_BFC_GREENFRONTFACES:
		//	doGreenFrontFaces();
		//	break;
		case ID_NEXT_STEP:
			changeStep(1);
			break;
		case ID_PREV_STEP:
			changeStep(-1);
			break;
		case ID_FIRST_STEP:
			changeStep(0);
			break;
		case ID_LAST_STEP:
			changeStep(2);
			break;
		case ID_GOTO_STEP:
			return doGotoStep();
	}
	if (itemId >= ID_HOT_KEY_0 && itemId <= ID_HOT_KEY_9)
	{
		if (modelWindow && modelWindow->performHotKey(itemId - ID_HOT_KEY_0))
		{
			return 0;
		}
	}
	if (itemId >= 20000 && itemId < 20000 + (int)standardSizes.size())
	{
		selectStandardSize(itemId - 20000);
	}
	if (itemId >= 30000 && itemId < 30000 + numVideoModes)
	{
		selectFSVideoModeMenuItem(itemId - 30000);
	}
	else if (itemId >= 31000 && itemId < 31100)
	{
		openRecentFile(itemId - 31000);
		return 0;
	}
	if (itemId >= ID_FILE_POLLING_DISABLED &&
		itemId <= ID_FILE_POLLING_BACKGROUND)
	{
		selectPollingMenuItem(itemId);
	}
	return CUIWindow::doCommand(itemId, notifyCode, controlHWnd);
}

LRESULT LDViewWindow::doGotoStep(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		StepDialog *dlg = new StepDialog(this, modelViewer);

		if (dlg->doModal() == IDOK)
		{
			setStep(dlg->getStep());
		}
		dlg->release();
	}
	return 0;
}

void LDViewWindow::changeStep(int action)
{
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			int newStep;
			
			if (action == 0)
			{
				newStep = 1;
			}
			else if (action == 2)
			{
				newStep = modelViewer->getNumSteps();
			}
			else
			{
				newStep = modelViewer->getStep() + action;
			}
			setStep(newStep);
		}
	}
}

int LDViewWindow::setStep(int newStep)
{
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			if (newStep < 1)
			{
				newStep = 1;
			}
			else if (newStep >= modelViewer->getNumSteps())
			{
				newStep = modelViewer->getNumSteps();
			}
			modelViewer->setStep(newStep);
			modelWindow->forceRedraw();
			if (toolbarStrip)
			{
				toolbarStrip->updateStep();
			}
		}
	}
	return newStep;
}

WNDCLASSEX LDViewWindow::getWindowClass(void)
{
	WNDCLASSEX windowClass = CUIWindow::getWindowClass();

	windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	return windowClass;
}

LRESULT LDViewWindow::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (!fullScreen && !screenSaver)
	{
		if (sizeType == SIZE_MAXIMIZED)
		{
			TCUserDefaults::setLongForKey(1, WINDOW_MAXIMIZED_KEY, false);
		}
		else if (IsWindowVisible(hWindow))
		{
			TCUserDefaults::setLongForKey(0, WINDOW_MAXIMIZED_KEY, false);
			if (sizeType == SIZE_RESTORED)
			{
				savePixelSize(WINDOW_WIDTH_KEY, newWidth);
				savePixelSize(WINDOW_HEIGHT_KEY, newHeight);
			}
		}
		if ((showStatusBar || showStatusBarOverride) && hStatusBar)
		{
			SendMessage(hStatusBar, WM_SIZE, SIZE_RESTORED,
				MAKELPARAM(newWidth, newHeight));
			updateStatusParts();
		}
		if (showToolbar && toolbarStrip)
		{
			toolbarStrip->autoSize();
		}
		//if (showToolbar && hToolbar)
		//{
		//	SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
		//}
	}
	LRESULT result = CUIWindow::doSize(sizeType, newWidth, newHeight);
	updateWindowMonitor();
	return result;
}

/*
int LDViewWindow::getDecorationHeight(void)
{
	int menuHeight = GetSystemMetrics(SM_CYMENU);

	return CUIWindow::getDecorationHeight() + menuHeight;
}
*/

LRESULT LDViewWindow::doClose(void)
{
#ifdef _DEBUG
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "doClose\n");
#endif // _DEBUG
	skipMinimize = true;
	if (modelWindow)
	{
		modelWindow->closeWindow();
//		modelWindowShown = false;
	}
	return CUIWindow::doClose();
}

LRESULT LDViewWindow::doDestroy(void)
{
	if (!switchingModes)
	{
		if (fullScreen)
		{
			restoreDisplayMode();
		}
		if (modelWindow)
		{
			modelWindow->release();
		}
		CUIWindow::doDestroy();
		TCAutoreleasePool::processReleases();
		debugOut("LDViewWindow::doDestroy\n");
		PostQuitMessage(0);
	}
	return 1;
}


HMENU LDViewWindow::menuForBitDepth(HWND hWnd, int bitDepth, int* index)
{
	HMENU viewMenu = GetSubMenu(GetMenu(hWnd), 2);
	int i;
	int count = GetMenuItemCount(viewMenu);
	HMENU bitDepthMenu = 0;
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_DATA | MIIM_SUBMENU;
	for (i = 0; i < count && !bitDepthMenu; i++)
	{
		GetMenuItemInfo(viewMenu, i, TRUE, &itemInfo);
		if (itemInfo.hSubMenu && itemInfo.dwItemData == (unsigned)bitDepth)
		{
			bitDepthMenu = itemInfo.hSubMenu;
//			bitDepthMenu = GetSubMenu(viewMenu, i);
			if (index)
			{
				*index = i;
			}
		}
	}
	return bitDepthMenu;
}

VideoModeT* LDViewWindow::getCurrentVideoMode(void)
{
	if (currentVideoModeIndex >= 0 && currentVideoModeIndex < numVideoModes)
	{
		return videoModes + currentVideoModeIndex;
	}
	else
	{
		return NULL;
	}
}

int LDViewWindow::getMenuItemIndex(HMENU hMenu, UINT itemID)
{
	int count = GetMenuItemCount(hMenu);
	int i;

	for (i = 0; i < count; i++)
	{
		if (GetMenuItemID(hMenu, i) == itemID)
		{
			return i;
		}
	}
	return -1;
}

int LDViewWindow::clearRecentFileMenuItems(void)
{
	int firstIndex = getMenuItemIndex(hFileMenu, ID_FILE_PRINT);
	int lastIndex = getMenuItemIndex(hFileMenu, ID_FILE_EXIT);

	if (firstIndex >= 0 && lastIndex >= 0)
	{
		int i;

		for (i = firstIndex + 1; i < lastIndex - 1; i++)
		{
			RemoveMenu(hFileMenu, firstIndex + 2, MF_BYPOSITION);
		}
		return firstIndex + 1;
	}
	return -1;
}

void LDViewWindow::populateExtraSearchDirs(void)
{
	int i;

	extraSearchDirs->removeAll();
	for (i = 1; true; i++)
	{
		char key[128];
		char *extraSearchDir;

		sprintf(key, "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i);
		extraSearchDir = TCUserDefaults::stringForKey(key, NULL, false);
		if (extraSearchDir)
		{
			extraSearchDirs->addString(extraSearchDir);
			delete extraSearchDir;
		}
		else
		{
			break;
		}
	}
}

void LDViewWindow::recordExtraSearchDirs(void)
{
	int i;
	int count = extraSearchDirs->getCount();

	for (i = 0; i <= count; i++)
	{
		char key[128];
		char *extraDir;

		sprintf(key, "%s/Dir%03d", EXTRA_SEARCH_DIRS_KEY, i + 1);
		extraDir = extraSearchDirs->stringAtIndex(i);
		if (extraDir)
		{
			TCUserDefaults::setStringForKey(extraDir, key, false);
		}
		else
		{
			TCUserDefaults::removeValue(key, false);
		}
	}
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			modelViewer->setExtraSearchDirs(extraSearchDirs);
			modelWindow->forceRedraw();
		}
	}
}

void LDViewWindow::populateRecentFiles(void)
{
	int i;
	long maxRecentFiles = TCUserDefaults::longForKey(MAX_RECENT_FILES_KEY, 10,
		false);

	recentFiles->removeAll();
	for (i = 1; i <= maxRecentFiles; i++)
	{
		char key[128];
		char *filename;

		sprintf(key, "%s/File%02d", RECENT_FILES_KEY, i);
		filename = TCUserDefaults::pathForKey(key, NULL, false);
		if (filename)
		{
			recentFiles->addString(filename);
			delete filename;
		}
		else
		{
			recentFiles->addString(NULL);
		}
	}
}

void LDViewWindow::recordRecentFiles(void)
{
	int i;
	long maxRecentFiles = TCUserDefaults::longForKey(MAX_RECENT_FILES_KEY, 10,
		false);

	for (i = 1; i <= maxRecentFiles; i++)
	{
		char key[128];
		char *filename;

		sprintf(key, "%s/File%02d", RECENT_FILES_KEY, i);
		filename = recentFiles->stringAtIndex(i - 1);
		if (filename)
		{
			TCUserDefaults::setPathForKey(filename, key, false);
		}
		else
		{
			TCUserDefaults::removeValue(key, false);
		}
	}
}

void LDViewWindow::populateRecentFileMenuItems(void)
{
	int index = clearRecentFileMenuItems();

	if (index >= 0 && recentFiles->stringAtIndex(0))
	{
		int i;
		MENUITEMINFO itemInfo;
		long maxRecentFiles = TCUserDefaults::longForKey(MAX_RECENT_FILES_KEY,
			10, false);

		memset(&itemInfo, 0, sizeof(MENUITEMINFO));
		itemInfo.cbSize = sizeof(MENUITEMINFO);
		itemInfo.fMask = MIIM_TYPE;
		itemInfo.fType = MFT_SEPARATOR;
		InsertMenuItem(hFileMenu, index, TRUE, &itemInfo);
//		SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
		for (i = 0; i < maxRecentFiles; i++)
		{
			char *filename = recentFiles->stringAtIndex(i);

			if (filename)
			{
				char title[2048];
				char *partialFilename = copyString(filename, 3);

				// Note: the following function sometimes increases the length
				// of the string.  I know that seems weird, but "..." takes up
				// less pixels that something like "WW", for example, so "WW"
				// could be replaced with "...".  I actually had this happen
				// with "C:\LDRAW\Texmapped\970c00px50.dat" getting changed to
				// "C:\LDRAW\Texmapp...\970c00px50.dat".  That's why the
				// copyString() call above asks for 3 characters of padding.
				PathCompactPath(hdc, partialFilename, 250);
				if (i < 10)
				{
					sprintf(title, "%s&%d %s", i == 9 ? "1" : "", (i + 1) % 10,
						partialFilename);
				}
				else
				{
					strcpy(title, partialFilename);
				}
				itemInfo.fMask = MIIM_TYPE | MIIM_ID;
				itemInfo.fType = MFT_STRING;
				itemInfo.dwTypeData = title;
				itemInfo.wID = 31000 + i;
				InsertMenuItem(hFileMenu, index + i + 1, TRUE, &itemInfo);
				delete partialFilename;
			}
			else
			{
				break;
			}
		}
	}
}

void LDViewWindow::populateDisplayModeMenuItems(void)
{
	int i;
	HMENU viewMenu = GetSubMenu(GetMenu(hWindow), 2);
//	VideoModeT* currentVideoMode = getCurrentVideoMode();
	
	for (i = 0; i < numVideoModes; i++)
	{
		int count = GetMenuItemCount(viewMenu);
		VideoModeT videoMode = videoModes[i];
		HMENU bitDepthMenu = menuForBitDepth(hWindow, videoMode.depth);
		UCCHAR title[128];
		MENUITEMINFOUC itemInfo;

		if (!bitDepthMenu)
		{
			memset(&itemInfo, 0, sizeof(itemInfo));
			sucprintf(title, COUNT_OF(title),
				TCLocalStrings::get(_UC("NBitModes")), videoMode.depth);
			bitDepthMenu = CreatePopupMenu();
			itemInfo.cbSize = sizeof(itemInfo);
			itemInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA;
			itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
			itemInfo.dwTypeData = title;
			itemInfo.dwItemData = videoMode.depth;
			itemInfo.hSubMenu = bitDepthMenu;
			insertMenuItemUC(viewMenu, count, TRUE, &itemInfo);
		}
		sucprintf(title, COUNT_OF(title), _UC("%dx%d"), videoMode.width,
			videoMode.height);
		memset(&itemInfo, 0, sizeof(itemInfo));
		itemInfo.cbSize = sizeof(itemInfo);
		itemInfo.fMask = MIIM_TYPE /*| MIIM_STATE*/ | MIIM_ID;
		itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
		itemInfo.dwTypeData = title;
		itemInfo.wID = 30000 + i;
		insertMenuItemUC(bitDepthMenu, GetMenuItemCount(bitDepthMenu), TRUE,
			&itemInfo);
	}
}

void LDViewWindow::checkVideoMode(int lwidth, int lheight, int depth)
{
	DEVMODE deviceMode;
	long result;

	memset(&deviceMode, 0, sizeof DEVMODE);
	deviceMode.dmSize = sizeof DEVMODE;
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	deviceMode.dmPelsWidth = lwidth;
	deviceMode.dmPelsHeight = lheight;
	deviceMode.dmBitsPerPel = depth;
	//result = ChangeDisplaySettings(&deviceMode, CDS_TEST | CDS_FULLSCREEN);
	result = changeDisplaySettings(&deviceMode, CDS_TEST | CDS_FULLSCREEN);
	if (result == DISP_CHANGE_SUCCESSFUL)
	{
		if (videoModes)
		{
			videoModes[numVideoModes].width = lwidth;
			videoModes[numVideoModes].height = lheight;
			videoModes[numVideoModes].depth = depth;
			if (videoModes[numVideoModes].width == fsWidth &&
				videoModes[numVideoModes].height == fsHeight)
			{
				if (currentVideoModeIndex < 0 ||
					videoModes[numVideoModes].depth == fsDepth)
				{
					currentVideoModeIndex = numVideoModes;
				}
			}
		}
		numVideoModes++;
	}
}

void LDViewWindow::checkLowResModes(void)
{
	checkVideoMode(320, 240, 16);
	checkVideoMode(400, 300, 16);
	checkVideoMode(480, 360, 16);
	checkVideoMode(512, 384, 16);
	checkVideoMode(320, 240, 32);
	checkVideoMode(400, 300, 32);
	checkVideoMode(480, 360, 32);
	checkVideoMode(512, 384, 32);
}

void LDViewWindow::getAllDisplayModes(void)
{
	int i;
	int count;
	DEVMODE deviceMode;

	memset(&deviceMode, 0, sizeof DEVMODE);
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	numVideoModes = 0;
	videoModes = NULL;
	checkLowResModes();
	for (i = 0; EnumDisplaySettings(NULL, i, &deviceMode); i++)
	{
		if (deviceMode.dmBitsPerPel >= 15)
		{
			numVideoModes++;
		}
	}
	count = i;
	videoModes = new VideoModeT[numVideoModes];
	numVideoModes = 0;
	checkLowResModes();
	for (i = 0; i < count; i++)
	{
		EnumDisplaySettings(NULL, i, &deviceMode);
		if (deviceMode.dmBitsPerPel >= 15)
		{
			int j;
			bool found = false;

			for (j = 0; j < numVideoModes && !found; j++)
			{
				VideoModeT videoMode = videoModes[j];

				if (videoMode.width == (int)deviceMode.dmPelsWidth &&
					videoMode.height == (int)deviceMode.dmPelsHeight &&
					videoMode.depth == (int)deviceMode.dmBitsPerPel)
				{
					found = true;
				}
			}
			if (!found)
			{
				videoModes[numVideoModes].width = deviceMode.dmPelsWidth;
				videoModes[numVideoModes].height = deviceMode.dmPelsHeight;
				videoModes[numVideoModes].depth = deviceMode.dmBitsPerPel;
				if (videoModes[numVideoModes].width == fsWidth &&
					videoModes[numVideoModes].height == fsHeight)
				{
					if (currentVideoModeIndex < 0 ||
						videoModes[numVideoModes].depth == fsDepth)
					{
						currentVideoModeIndex = numVideoModes;
					}
				}
				numVideoModes++;
			}
		}
	}
}

LRESULT LDViewWindow::doCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	LRESULT retVal;

	retVal = CUIWindow::doCreate(hWnd, lpcs);
	if (!videoModes)
	{
		getAllDisplayModes();
	}
	populateDisplayModeMenuItems();
	initPollingMenu();
	if (currentVideoModeIndex >= 0)
	{
		selectFSVideoModeMenuItem(currentVideoModeIndex, false);
	}
#if !defined(USE_CPP11) && defined(_NO_BOOST)
	HMENU hMenu = getParentOfMenuItem(GetMenu(hWindow),
		ID_FILE_CHECKFORLIBUPDATES);

	if (hMenu)
	{
		DeleteMenu(hMenu, ID_FILE_CHECKFORLIBUPDATES, MF_BYCOMMAND);
	}
#endif // _NO_BOOST
	return retVal;
}

void LDViewWindow::saveDefaultView(void)
{
	modelWindow->saveDefaultView();
}

void LDViewWindow::rightSideUp(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		modelViewer->rightSideUp();
	}
}

void LDViewWindow::zoomToFit(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		setWaitCursor();
		modelViewer->zoomToFit();
		modelWindow->forceRedraw();
		setArrowCursor();
	}
}

void LDViewWindow::resetDefaultView(void)
{
	modelWindow->resetDefaultView();
}

LRESULT LDViewWindow::specifyLatLon(void)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		LatLonDialog *dlg = new LatLonDialog(getLanguageModule());

		dlg->setDefaultDistance(modelViewer->getDefaultDistance());
		dlg->setCurrentDistance(modelViewer->getDistance());
		if (dlg->doModal(modelWindow) == IDOK)
		{
			modelViewer->setLatLon(dlg->getLat(), dlg->getLon(),
				dlg->getDistance());
		}
		dlg->release();
	}
	return 0;
}

void LDViewWindow::resetView(LDVAngle viewAngle)
{
	modelWindow->resetView(viewAngle);
}

void LDViewWindow::printModel(void)
{
	if (modelIsLoaded())
	{
		modelWindow->print();
	}
}

void LDViewWindow::pageSetup(void)
{
	if (modelIsLoaded())
	{
		modelWindow->pageSetup();
	}
}

void LDViewWindow::exportModel(void)
{
	if (modelIsLoaded())
	{
		modelWindow->exportModel();
	}
}

void LDViewWindow::saveSnapshot(void)
{
	if (modelIsLoaded())
	{
		modelWindow->getModelViewer()->pause();
		modelWindow->saveSnapshot();
		modelWindow->getModelViewer()->unpause();
	}
}

bool LDViewWindow::saveSnapshot(char *saveFilename)
{
	if (modelIsLoaded())
	{
		return modelWindow->saveSnapshot(saveFilename);
	}
	return false;
}

// Just as a note, skipLoad is set to true for command line snapshots.  This is
// done because parts of the load have to be redone at the time the snapshot is
// taken, and this lets it avoid the repeat work.  Inside this function, we'll
// use the same variable to signify that either the user canceled the open file
// dialog, or the load failed.
void LDViewWindow::openModel(const char* filename, bool skipLoad)
{
	char fullPathName[1024] = "";

	stopAnimation();
	if (filename && strlen(filename) > 0)
	{
		char* newFilename = NULL;

		if (!verifyLDrawDir())
		{
			return;
		}
		if (filename[0] == '"')
		{
			newFilename = copyString(filename + 1);
			size_t length = strlen(newFilename);

			if (length > 0 && newFilename[length - 1] == '"')
			{
				newFilename[length - 1] = 0;
			}
		}
		else
		{
			newFilename = copyString(filename);
		}
		if (!ModelWindow::chDirFromFilename(newFilename, fullPathName))
		{
			skipLoad = true;
		}
		delete newFilename;
	}
	else
	{
		OPENFILENAMEUC openStruct;
		UCCHAR fileTypes[1024];
		//char openFilename[1024] = "";
		UCSTR initialDir = lastOpenPathUC();
		UCCHAR fullPathNameUC[1024] = _UC("");

		if (initialDir)
		{
			// ToDo: Unicode: Actual filename isn't Unicode.
			UCSTR tmpUC = mbstoucstring(fullPathName);
			char *tmpA;

			ucstrcpy(fullPathNameUC, tmpUC);
			delete tmpUC;
			memset(fileTypes, 0, 2 * sizeof(UCCHAR));
			addFileType(fileTypes, ls(_UC("LDrawFileTypes")),
				_UC("*.ldr;*.dat;*.mpd"));
			addFileType(fileTypes, ls(_UC("LDrawModelFileTypes")),
				_UC("*.ldr;*.dat"));
			addFileType(fileTypes, ls(_UC("LDrawMpdFileTypes")), _UC("*.mpd"));
			addFileType(fileTypes, ls(_UC("AllFilesTypes")), _UC("*.*"));
			memset(&openStruct, 0, sizeof(openStruct));
			openStruct.lStructSize = getOpenFilenameSize(true);
			openStruct.hwndOwner = hWindow;
			openStruct.lpstrFilter = fileTypes;
			openStruct.nFilterIndex = 1;
			openStruct.lpstrFile = fullPathNameUC;
			openStruct.nMaxFile = 1024;
			openStruct.lpstrInitialDir = initialDir;
			openStruct.lpstrTitle = TCLocalStrings::get(_UC("SelectModelFile"));
			openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
				OFN_HIDEREADONLY;
			openStruct.lpstrDefExt = _UC("ldr");
			if (!getOpenFileNameUC(&openStruct))
			{
				skipLoad = true;
			}
			tmpA = ucstringtombs(fullPathNameUC);
			strcpy(fullPathName, tmpA);
			delete tmpA;
			delete initialDir;
		}
	}
	if (skipLoad)
	{
		modelWindow->setFilename(fullPathName);
	}
	else
	{
		char dir[1024];
		GetCurrentDirectory(sizeof(dir), dir);
		modelWindow->setFilename(fullPathName);
		// We need to create the error window before we start loading the
		// file. Otherwise, the progress tracking that happens when it
		// loads its images will confuse us and send the app into an
		// infinite loop waiting for the model to finish loading.
		modelWindow->createErrorWindow();
		if (modelWindow->loadModel())
		{
			updateModelMenuItems();
			setLastOpenFile(fullPathName);
		}
		else
		{
			if (!modelWindow->getLoadCanceled())
			{
				UCCHAR message[2048];
				UCSTR fullPathNameUC = mbstoucstring(fullPathName);

				sucprintf(message, COUNT_OF(message),
					TCLocalStrings::get(_UC("ErrorLoadingModel")), fullPathNameUC);
				delete fullPathNameUC;
				messageBoxUC(hWindow, message, _UC("LDView"), MB_OK | MB_ICONWARNING);
			}
			modelWindow->setFilename(NULL);
		}
	}
	populateRecentFileMenuItems();
	if (modelWindowIsShown())
	{
		// Don't activate if it hasn't been shown, because my activation handler
		// shows it, and this makes the window show up with command line image
		// generation.
		if (modelWindow->isErrorWindowVisible() &&
			modelWindow->getErrorCount() > 0)
		{
			// If the error window is visible, and we have errors (as opposed
			// to just warnings, or no errors OR warnings), activate the error
			// window.
			modelWindow->showErrors();
		}
		else
		{
			SetActiveWindow(hWindow);
		}
	}
}

char* LDViewWindow::getLDrawDir(void)
{
	char* lDrawDir = TCUserDefaults::pathForKey(LDRAWDIR_KEY, NULL, false);

	if (!lDrawDir)
	{
		char buf[1024];

		if (GetPrivateProfileString("LDraw", "BaseDirectory", NULL, buf, 1024,
			"ldraw.ini"))
		{
			buf[1023] = 0;
			lDrawDir = copyString(buf);
		}
		else
		{
			lDrawDir = copyString(LDLModel::lDrawDir());
		}
	}
	stripTrailingPathSeparators(lDrawDir);
	LDLModel::setLDrawDir(lDrawDir);
	return lDrawDir;
}

void LDViewWindow::createLDrawDirWindow(void)
{
	hLDrawDirWindow = createDialog(IDD_LDRAWDIR);
}

int CALLBACK LDViewWindow::pathBrowserCallback(HWND hwnd, UINT uMsg,
											   LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_SELCHANGED)
	{
		char path[MAX_PATH+10];
		ITEMIDLIST* itemIdList = (ITEMIDLIST*)lParam;

		// For some reason, computers on the network are considered directories,
		// and the ok button is enabled for them.  We don't want to allow that,
		// and if one is selected, the following method will fail.
		if (!SHGetPathFromIDList(itemIdList, path))
		{
			SendMessage(hwnd, BFFM_ENABLEOK, 0, 0);
		}
	}
	else if (lpData && uMsg == BFFM_INITIALIZED)
	{
		if (strcmp((const char *)lpData, "C:") == 0)
		{
			// LDLModel strips off the backslash.  We need to add it back.
			lpData = (LPARAM)"C:\\";
		}
		// ToDo: Unicode ?path
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

// Note: static method
std::string LDViewWindow::browseForDir(
	const char *prompt,
	const char *initialDir)
{
	BROWSEINFO browseInfo;
	char displayName[MAX_PATH];
	LPITEMIDLIST itemIdList;

	browseInfo.hwndOwner = NULL; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = prompt;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)initialDir;
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		char path[MAX_PATH+10];

		if (SHGetPathFromIDList(itemIdList, path))
		{
			stripTrailingPathSeparators(path);
			return path;
		}
		MessageBox(NULL/*hWindow*/, TCLocalStrings::get("InvalidDirSelected"),
			TCLocalStrings::get("Error"), MB_OK);
	}
	return "";
}

// Note: static method
BOOL LDViewWindow::promptForLDrawDir(const char *prompt)
{
	char *oldLDrawDir = getLDrawDir();
	std::string dir;

	if (!prompt)
	{
		prompt = TCLocalStrings::get("LDrawDirPrompt");
	}
	dir = browseForDir(prompt, oldLDrawDir);
	delete oldLDrawDir;
	if (dir.size() > 0)
	{
		TCUserDefaults::setPathForKey(dir.c_str(), LDRAWDIR_KEY, false);
		LDLModel::setLDrawDir(dir.c_str());
		return TRUE;
	}
	return FALSE;
}

/*
void LDViewWindow::stripTrailingSlash(char* value)
{
	int end = strlen(value) - 1;

	if (end >= 0 && (value[end] == '/' || value[end] == '\\'))
	{
		value[end] = 0;
	}
}
*/

BOOL LDViewWindow::verifyLDrawDir(char* value)
{
//	int length = strlen(value);
	char currentDir[1024];
	char newDir[1024];
	BOOL found = FALSE;

//	stripTrailingPathSeparators(value);
	sprintf(newDir, "%s\\parts", value);
	GetCurrentDirectory(1024, currentDir);
	if (SetCurrentDirectory(newDir))
	{
		sprintf(newDir, "%s\\p", value);
		if (SetCurrentDirectory(newDir))
		{
			found = TRUE;
		}
		SetCurrentDirectory(currentDir);
	}
	return found;
}

BOOL LDViewWindow::verifyLDrawDir(bool forceChoose)
{
	char* lDrawDir = getLDrawDir();
	BOOL found = FALSE;

	if (!forceChoose && 
		(!TCUserDefaults::longForKey(VERIFY_LDRAW_DIR_KEY, 1, false) ||
		verifyLDrawDir(lDrawDir)))
	{
		delete lDrawDir;
		found = TRUE;
	}
	else
	{
		delete lDrawDir;

		stopAnimation();
		if (forceChoose ||
			MessageBox(NULL, TCLocalStrings::get("LDrawDirExistsPrompt"),
			"LDView", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			while (!found)
			{
				if (promptForLDrawDir())
				{
					lDrawDir = getLDrawDir();
					if (verifyLDrawDir(lDrawDir))
					{
						found = TRUE;
					}
					else
					{
						MessageBox(NULL, TCLocalStrings::get("LDrawNotInDir"),
							TCLocalStrings::get("InvalidDir"),
							MB_OK | MB_ICONWARNING | MB_TASKMODAL);
					}
					delete lDrawDir;
				}
				else
				{
					break;
				}
			}
		}
#if defined(USE_CPP11) || !defined(_NO_BOOST)
		else
		{
			if (MessageBox(NULL, TCLocalStrings::get("WillDownloadLDraw"),
				"LDView", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
			{
				LDLModel::setLDrawDir("C:\\");
				if (promptForLDrawDir(
					TCLocalStrings::get("LDrawInstallDirPrompt")))
				{
					if (installLDraw())
					{
						found = true;
					}
				}
			}
		}
#endif // !_NO_BOOST
	}
	return found;
}

char* LDViewWindow::lastOpenPath(char* pathKey)
{
	if (!pathKey)
	{
		pathKey = LAST_OPEN_PATH_KEY;
	}
	if (verifyLDrawDir())
	{
		char* path = TCUserDefaults::pathForKey(pathKey, NULL, false);

		if (!path)
		{
			path = getLDrawDir();
		}
		return path;
	}
	else
	{
		return NULL;
	}
}

UCSTR LDViewWindow::lastOpenPathUC(char* pathKey)
{
	if (!pathKey)
	{
		pathKey = LAST_OPEN_PATH_KEY;
	}
	if (verifyLDrawDir())
	{
		char* path = TCUserDefaults::pathForKey(pathKey, NULL, false);
		UCCHAR* pathUC;

		if (!path)
		{
			path = getLDrawDir();
		}
		pathUC = mbstoucstring(path);
		delete path;
		return pathUC;
	}
	else
	{
		return NULL;
	}
}

void LDViewWindow::setLastOpenFile(const char* filename, char* pathKey)
{
	if (filename)
	{
		const char* spot = strrchr(filename, '\\');
		int index;

		SHAddToRecentDocs(SHARD_PATHA, filename);
		if (!pathKey)
		{
			pathKey = LAST_OPEN_PATH_KEY;
		}
		if (spot)
		{
			size_t length = spot - filename;
			char* path = new char[length + 1];

			strncpy(path, filename, length);
			path[length] = 0;
			TCUserDefaults::setPathForKey(path, pathKey, false);
			delete path;
		}
		if (recentFiles && !stringHasCaseInsensitiveSuffix(filename, ".tmp"))
		{
			index = recentFiles->indexOfString(filename);
			recentFiles->insertString(filename);
			if (index >= 0)
			{
				// Insert before removal.  Since the one being removed could
				// have the same pointer value as the string in the array, we
				// could otherwise access a pointer after it had been deleted.
				recentFiles->removeStringAtIndex(index + 1);
			}
			recordRecentFiles();
		}
	}
}

LRESULT LDViewWindow::doShowWindow(BOOL showFlag, LPARAM status)
{
	static bool busy = false;

	if (!busy)
	{
		busy = true;
//		debugPrintf("LDViewWindow::doShowWindow\n");
		if (modelWindow && showFlag)
		{
			if (!modelWindowIsShown())
			{
				modelWindow->showWindow(SW_NORMAL);
			}
			// For some reason, creating the status bar prior to the window being
			// shown results in a status bar that doesn't update properly in XP,
			// so show it here instead of in initWindow.
			reflectStatusBar();
			if (!showToolbar && !initialShown)
			{
				// Icons from the toolbar get applied to the main menu.  So we
				// need to create it here if it's not visible, then immediately
				// delete it.
				createToolbar();
				toolbarStrip->release();
				toolbarStrip = NULL;
			}
			reflectToolbar();
		}
		initialShown = true;
		busy = false;
	}
	return CUIWindow::doShowWindow(showFlag, status);
}

bool LDViewWindow::modelWindowIsShown(void)
{
	if (modelWindow)
	{
		HWND hModelWindow = modelWindow->getHWindow();
		return IsWindowVisible(hModelWindow) != FALSE;
/*
		WINDOWPLACEMENT windowPlacement;

		windowPlacement.length = sizeof(WINDOWPLACEMENT);
		if (GetWindowPlacement(hModelWindow, &windowPlacement))
		{
			return windowPlacement.showCmd != SW_HIDE;
		}
*/
	}
	return false;
}

LRESULT LDViewWindow::doKeyDown(int keyCode, LPARAM keyData)
{
	if (modelWindow)
	{
		return modelWindow->processKeyDown(keyCode, keyData);
	}
	else
	{
		return 1;
	}
}

LRESULT LDViewWindow::doKeyUp(int keyCode, LPARAM keyData)
{
	if (modelWindow)
	{
		return modelWindow->processKeyUp(keyCode, keyData);
	}
	else
	{
		return 1;
	}
}

LRESULT LDViewWindow::doDrawItem(HWND /*hControlWnd*/,
								 LPDRAWITEMSTRUCT drawItemStruct)
{
	if (drawItemStruct->hwndItem == hStatusBar)
	{
		if (drawItemStruct->itemID == 0)
		{
			DWORD backgroundColor =
				LDViewPreferences::getColor(BACKGROUND_COLOR_KEY);
 			HBRUSH hBrush = CreateSolidBrush(RGB(backgroundColor & 0xFF,
				(backgroundColor >> 8) & 0xFF, (backgroundColor >> 16) & 0xFF));

			FillRect(drawItemStruct->hDC, &drawItemStruct->rcItem, hBrush);
			DeleteObject(hBrush);
			return TRUE;
		}
	}
	return FALSE;
}

void LDViewWindow::startLoading(void)
{
}

void LDViewWindow::stopLoading(void)
{
}

void LDViewWindow::setLoading(bool value)
{
	if (value != loading)
	{
		loading = value;
		if (loading)
		{
			startLoading();
		}
		else
		{
			stopLoading();
		}
		updateModelMenuItems();
	}
}

//void LDViewWindow::setMenuItemsEnabled(HMENU hMenu, bool enabled)
//{
//	int i;
//	int count = GetMenuItemCount(hMenu);
//
//	for (i = 0; i < count; i++)
//	{
//		setMenuEnabled(hMenu, i, enabled, TRUE);
//	}
//}

LRESULT LDViewWindow::doInitMenuPopup(HMENU hPopupMenu, UINT /*uPos*/,
									  BOOL /*fSystemMenu*/)
{
	//if (hPopupMenu == hWireframeToolbarMenu)
	//{
	//	updateWireframeMenu();
	//}
	//else if (hPopupMenu == hEdgesToolbarMenu)
	//{
	//	updateEdgesMenu();
	//}
	//else if (hPopupMenu == hPrimitivesToolbarMenu)
	//{
	//	updatePrimitivesMenu();
	//}
	//else if (hPopupMenu == hLightingToolbarMenu)
	//{
	//	updateLightingMenu();
	//}
	//else if (hPopupMenu == hBFCToolbarMenu)
	//{
	//	updateBFCMenu();
	//}
	//else
	{
		setMenuItemsEnabled(hPopupMenu, !loading);
		if (loading)
		{
			if (hPopupMenu == hFileMenu)
			{
				setMenuEnabled(hFileMenu, ID_FILE_CANCELLOAD, true);
			}
		}
		else
		{
			if (hPopupMenu == hFileMenu)
			{
				setMenuEnabled(hFileMenu, ID_FILE_CANCELLOAD, false);
			}
			else if (hPopupMenu == hViewMenu)
			{
				setMenuEnabled(hViewMenu, ID_VIEW_EXAMINE_LAT_LONG,
					getMenuCheck(hViewMenu, ID_VIEW_EXAMINE));
				setMenuEnabled(hViewMenu, ID_VIEW_KEEPRIGHTSIDEUP,
					getMenuCheck(hViewMenu, ID_VIEW_FLYTHROUGH));
			}
			updateModelMenuItems();
		}
	}
	return 1;
}

BOOL LDViewWindow::doDialogGetMinMaxInfo(HWND hDlg, LPMINMAXINFO minMaxInfo)
{
	if (hDlg == hOpenGLInfoWindow)
	{
		calcSystemSizes();
		minMaxInfo->ptMaxSize.x = systemMaxWidth;
		minMaxInfo->ptMaxSize.y = systemMaxHeight;
		minMaxInfo->ptMinTrackSize.x = 250;
		minMaxInfo->ptMinTrackSize.y = 200;
		minMaxInfo->ptMaxTrackSize.x = systemMaxTrackWidth;
		minMaxInfo->ptMaxTrackSize.y = systemMaxTrackHeight;
		return TRUE;
	}
	return FALSE;
}

void LDViewWindow::reflectToolbar(void)
{
	if (fullScreen || screenSaver || hParentWindow)
	{
		return;
	}
	//if (showToolbar && !hToolbar)
	if (showToolbar && !toolbarStrip)
	{
		addToolbar();
		setMenuCheck(hViewMenu, ID_VIEW_TOOLBAR, true);
	}
	//else if (!showToolbar && hToolbar)
	else if (!showToolbar && toolbarStrip)
	{
		removeToolbar();
		setMenuCheck(hViewMenu, ID_VIEW_TOOLBAR, false);
	}
}

void LDViewWindow::reflectStatusBar(void)
{
	if (fullScreen || screenSaver || hParentWindow)
	{
		return;
	}
	if ((showStatusBar || showStatusBarOverride) && !hStatusBar)
	{
		addStatusBar();
		setMenuCheck(hViewMenu, ID_VIEW_STATUSBAR, true);
	}
	else if (!showStatusBar && !showStatusBarOverride && hStatusBar)
	{
		removeStatusBar();
		setMenuCheck(hViewMenu, ID_VIEW_STATUSBAR, false);
	}
}

void LDViewWindow::reflectVideoMode(void)
{
	int i;

	for (i = 0; i < numVideoModes; i++)
	{
		if (videoModes[i].width == fsWidth &&
			videoModes[i].height == fsHeight &&
			videoModes[i].depth == fsDepth)
		{
			selectFSVideoModeMenuItem(i);
			break;
		}
	}
}

void LDViewWindow::toolbarChecksReflect(void)
{
	if (toolbarStrip != NULL)
	{
		toolbarStrip->checksReflect();
	}
}

void LDViewWindow::applyPrefs(void)
{
	loadSettings();
	reflectViewMode();
	populateRecentFileMenuItems();
	reflectStatusBar();
	reflectToolbar();
	reflectTopmost();
	reflectPolling();
	reflectVideoMode();
	toolbarChecksReflect();
	if (TCUserDefaults::longForKey(WINDOW_MAXIMIZED_KEY, 0, false))
	{
		ShowWindow(hWindow, SW_MAXIMIZE);
	}
	else
	{
		ShowWindow(hWindow, SW_SHOWNORMAL);
	}
}

void LDViewWindow::generatePartsList(
	LDHtmlInventory *htmlInventory,
	LDPartsList *partsList,
	const char *filename)
{
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	// Note: if we get here, modelViewer is guaranteed to be non-NULL, so
	// there's no need to check it.
	if (htmlInventory->generateHtml(filename, partsList,
		modelViewer->getCurFilename().c_str()))
	{
		if (htmlInventory->isSnapshotNeeded())
		{
			char *snapshotPath = copyString(htmlInventory->getSnapshotPath());
			bool saveZoomToFit = modelWindow->getSaveZoomToFit();
			bool saveActualSize = modelWindow->getSaveActualSize();
			int saveWidth = modelWindow->getSaveWidth();
			int saveHeight = modelWindow->getSaveHeight();
			bool origSteps = TCUserDefaults::boolForKey(SAVE_STEPS_KEY, false,
				false);
			int origStep = modelViewer->getStep();

			TCUserDefaults::setBoolForKey(false, SAVE_STEPS_KEY, false);
			modelViewer->setStep(modelViewer->getNumSteps());
			htmlInventory->prepForSnapshot(modelViewer);
			modelWindow->setSaveZoomToFit(true);
			modelWindow->setSaveActualSize(false);
			modelWindow->setSaveWidth(400);
			modelWindow->setSaveHeight(300);
			// By saying it's from the command line, none of the above settings
			// will be written to TCUserDefaults.  I know it's not really from
			// the command line, but it produces the behavior we want.  By
			// saying true to the notReallyCommandLine parameter, we avoid
			// the behavior we don't want.
			modelWindow->saveSnapshot(snapshotPath, true, true);
			delete snapshotPath;
			htmlInventory->restoreAfterSnapshot(modelViewer);
			modelWindow->setSaveZoomToFit(saveZoomToFit);
			modelWindow->setSaveActualSize(saveActualSize);
			modelWindow->setSaveWidth(saveWidth);
			modelWindow->setSaveHeight(saveHeight);
			modelViewer->setStep(origStep);
			TCUserDefaults::setBoolForKey(origSteps, SAVE_STEPS_KEY, false);
			modelWindow->forceRedraw();
		}
		if (htmlInventory->getShowFileFlag())
		{
			shellExecute(filename);
		}
	}
	else
	{
		MessageBox(hWindow, TCLocalStrings::get("PLGenerateError"),
			TCLocalStrings::get("Error"), MB_OK | MB_ICONWARNING);
	}
}

LRESULT LDViewWindow::showModelTree(void)
{
	if (modelWindow)
	{
		if (!modelTreeDialog)
		{
			modelTreeDialog = new ModelTreeDialog(getLanguageModule(), hWindow);
		}
		modelTreeDialog->show(modelWindow, hWindow);
	}
	return 0;
}

bool LDViewWindow::isBoundingBoxVisible(void)
{
	if (boundingBoxDialog != NULL)
	{
		return boundingBoxDialog->isVisible();
	}
	return false;
}

LRESULT LDViewWindow::toggleBoundingBox(void)
{
	if (modelWindow)
	{
		if (!boundingBoxDialog)
		{
			boundingBoxDialog = new BoundingBoxDialog(getLanguageModule());
		}
		boundingBoxDialog->toggle(modelWindow);
	}
	return 0;
}

void LDViewWindow::boundingBoxToggled(void)
{
	toolbarChecksReflect();
	setMenuCheck(hToolsMenu, ID_TOOLS_BOUNDINGBOX, isBoundingBoxVisible());
}

LRESULT LDViewWindow::showMpd(void)
{
	if (modelWindow)
	{
		if (!mpdDialog)
		{
			mpdDialog = new MpdDialog(getLanguageModule());
		}
		mpdDialog->show(modelWindow);
	}
	return 0;
}

LRESULT LDViewWindow::generatePartsList(void)
{
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			LDPartsList *partsList = modelViewer->getPartsList();

			stopAnimation();
			if (partsList)
			{
				LDHtmlInventory *htmlInventory = new LDHtmlInventory;
				PartsListDialog *dialog = new PartsListDialog(this,
					htmlInventory);

				if (dialog->runModal() == IDOK)
				{
					OPENFILENAME openStruct;
					char fileTypes[1024];
					std::string filename = modelViewer->getCurFilename();
					std::string initialDir =
						modelWindow->getSaveDir(LDPreferences::SOPartsList);
					size_t findSpot = filename.find_last_of("/\\");

					if (findSpot < filename.size())
					{
						filename = filename.substr(findSpot + 1);
					}
					findSpot = filename.find_last_of('.');
					if (findSpot < filename.size())
					{
						filename = filename.substr(0, findSpot);
					}
					filename += ".html";
					filename.reserve(1024);
					memset(fileTypes, 0, 2);
					addFileType(fileTypes, TCLocalStrings::get("HtmlFileType"),
						"*.html");
					memset(&openStruct, 0, sizeof(OPENFILENAME));
					openStruct.lStructSize = getOpenFilenameSize(false);
					openStruct.hwndOwner = hWindow;
					openStruct.lpstrFilter = fileTypes;
					openStruct.nFilterIndex = 0;
					openStruct.lpstrFile = &filename[0];
					openStruct.nMaxFile = (DWORD)filename.capacity();
					openStruct.lpstrInitialDir = initialDir.c_str();
					openStruct.lpstrTitle =
						TCLocalStrings::get("GeneratePartsList");
					openStruct.Flags = OFN_EXPLORER | OFN_HIDEREADONLY |
						OFN_OVERWRITEPROMPT;
					openStruct.lpstrDefExt = NULL;
					openStruct.hInstance = getLanguageModule();
					if (GetSaveFileName(&openStruct))
					{
						generatePartsList(htmlInventory, partsList,
							filename.c_str());
					}
				}
				htmlInventory->release();
				partsList->release();
				dialog->release();
			}
		}
	}
	return 0;
}

void LDViewWindow::stopAnimation(void)
{
	if (modelWindow)
	{
		modelWindow->stopAnimation();
	}
}

RECT LDViewWindow::getWorkArea(void)
{
	RECT workAreaRect = { 0, 0, 0, 0};
	bool fromMonitor = false;

#ifndef TC_NO_UNICODE
	if (hMonitor)
	{
		MONITORINFO mi;

		mi.cbSize = sizeof(mi);
		if (::GetMonitorInfo(hMonitor, &mi))
		{
			workAreaRect = mi.rcWork;
			fromMonitor = true;
		}
	}
#endif // TC_NO_UNICODE
	if (!fromMonitor)
	{
		if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0))
		{
			::GetWindowRect(::GetDesktopWindow(), &workAreaRect);
		}
	}
	return workAreaRect;
}

void LDViewWindow::selectStandardSize(int index)
{
	const LDrawModelViewer::StandardSize &size = standardSizes[index];
	RECT windowRect;
	RECT clientRect;
	RECT workAreaRect = getWorkArea();

	if (IsZoomed(hWindow))
	{
		ShowWindow(hWindow, SW_RESTORE);
	}
	::GetWindowRect(hWindow, &windowRect);
	::GetClientRect(modelWindow->getHWindow(), &clientRect);
	windowRect.right += size.width - (clientRect.right - clientRect.left);
	windowRect.bottom += size.height - (clientRect.bottom - clientRect.top);
	if (windowRect.right > workAreaRect.right)
	{
		int delta = windowRect.right - workAreaRect.right;

		windowRect.left -= delta;
		windowRect.right -= delta;
	}
	if (windowRect.bottom > workAreaRect.bottom)
	{
		int delta = windowRect.bottom - workAreaRect.bottom;

		windowRect.top -= delta;
		windowRect.bottom -= delta;
	}
	MoveWindow(hWindow, windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		TRUE);
	::GetClientRect(modelWindow->getHWindow(), &clientRect);
	debugPrintf(-1, "New client size: %d x %d\n",
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top);
}

void LDViewWindow::setupStandardSizes(void)
{
	RECT windowRect;
	RECT clientRect;
	RECT workAreaRect = getWorkArea();
	SIZE workAreaSize;
	SIZE windowSize;
	SIZE clientSize;
	SIZE size;

	::GetWindowRect(hWindow, &windowRect);
	::GetClientRect(modelWindow->getHWindow(), &clientRect);
	workAreaSize.cx = workAreaRect.right - workAreaRect.left;
	workAreaSize.cy = workAreaRect.bottom - workAreaRect.top;
	windowSize.cx = windowRect.right - windowRect.left;
	windowSize.cy = windowRect.bottom - windowRect.top;
	if (::IsZoomed(hWindow))
	{
		LONG borderSize = ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;

		// GetWindowRect lies for maximized windows.
		windowSize.cx -= borderSize;
		windowSize.cy -= borderSize;
	}
	clientSize.cx = clientRect.right - clientRect.left;
	clientSize.cy = clientRect.bottom - clientRect.top;
	size.cx = workAreaSize.cx - windowSize.cx + clientSize.cx;
	size.cy = workAreaSize.cy - windowSize.cy + clientSize.cy;
	if (size.cx == maxStandardSize.cx && size.cy == maxStandardSize.cy)
	{
		// No change
		return;
	}
	maxStandardSize = size;
	LDrawModelViewer::getStandardSizes(size.cx, size.cy, standardSizes);
	while (GetMenuItemCount(hStandardSizesMenu) > 0)
	{
		DeleteMenu(hStandardSizesMenu, 0, MF_BYPOSITION);
	}
	for (UINT i = 0; i < (UINT)standardSizes.size(); i++)
	{
		MENUITEMINFOUC itemInfo;
		ucstring name = standardSizes[i].name;

		memset(&itemInfo, 0, sizeof(itemInfo));
		itemInfo.cbSize = sizeof(itemInfo);
		itemInfo.fMask = MIIM_TYPE | MIIM_ID;
		itemInfo.fType = MFT_STRING;
		itemInfo.dwTypeData = &name[0];
		itemInfo.wID = 20000 + i;
		insertMenuItemUC(hStandardSizesMenu, i, TRUE, &itemInfo);
	}
}
