#include "LDViewWindow.h"
#include <shlobj.h>
#include <shlwapi.h>
#include "LDVExtensionsSetup.h"
#include "LDViewPreferences.h"
#include "SSModelWindow.h"
#include "AppResources.h"
#include "UserDefaultsKeys.h"
#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCWebClient.h>
#include <TCFoundation/TCThreadManager.h>
#include <CUI/CUIWindowResizer.h>
#include <afxres.h>

#include "ModelWindow.h"
#include <TCFoundation/TCMacros.h>

#define DOWNLOAD_TIMER 12

TCStringArray* LDViewWindow::recentFiles = NULL;
TCStringArray* LDViewWindow::extraSearchDirs = NULL;

LDViewWindow::LDViewWindowCleanup LDViewWindow::ldViewWindowCleanup;

void debugOut(char *fmt, ...);

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

LDViewWindow::LDViewWindow(char* windowTitle, HINSTANCE hInstance, int x,
						   int y, int width, int height)
			  :CUIWindow(windowTitle, hInstance, x, y, width, height),
			   modelWindow(NULL),
			   hAboutWindow(NULL),
			   hLDrawDirWindow(NULL),
			   hOpenGLInfoWindow(NULL),
			   hExtraDirsWindow(NULL),
			   hStatusBar(NULL),
			   userLDrawDir(NULL),
			   fullScreen(NO),
			   fullScreenActive(NO),
			   switchingModes(NO),
			   videoModes(NULL),
			   numVideoModes(0),
			   currentVideoModeIndex(-1),
			   showStatusBarOverride(false),
			   skipMinimize(NO),
			   screenSaver(NO),
			   originalMouseX(-999999),
			   originalMouseY(-999999),
			   hFileMenu(NULL),
			   hViewMenu(NULL),
			   loading(FALSE),
			   openGLInfoWindoResizer(NULL),
			   hOpenGLStatusBar(NULL),
			   hExamineIcon(NULL),
			   hFlythroughIcon(NULL)
//			   modelWindowShown(false)
{
//	DWORD backgroundColor = TCUserDefaults::longForKey(BACKGROUND_COLOR_KEY);
	loadSettings();
	standardWindowStyle = windowStyle;
	if (!recentFiles)
	{
		recentFiles = new TCStringArray(10);
		populateRecentFiles();
	}
	if (!extraSearchDirs)
	{
		extraSearchDirs = new TCStringArray;
		populateExtraSearchDirs();
	}
	hExamineIcon = (HICON)LoadImage(getLanguageModule(),
		MAKEINTRESOURCE(IDI_EXAMINE), IMAGE_ICON, 32, 16, LR_DEFAULTCOLOR);
	hFlythroughIcon = (HICON)LoadImage(getLanguageModule(),
		MAKEINTRESOURCE(IDI_FLYTHROUGH), IMAGE_ICON, 32, 16, LR_DEFAULTCOLOR);
//	DeleteObject(hBackgroundBrush);
// 	hBackgroundBrush = CreateSolidBrush(RGB(backgroundColor & 0xFF,
//		(backgroundColor >> 8) & 0xFF, (backgroundColor >> 16) & 0xFF));
}

LDViewWindow::~LDViewWindow(void)
{
}

void LDViewWindow::dealloc(void)
{
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
	CUIWindow::dealloc();
}

void LDViewWindow::loadSettings(void)
{
	fsWidth = TCUserDefaults::longForKey(FULLSCREEN_WIDTH_KEY, 640);
	fsHeight = TCUserDefaults::longForKey(FULLSCREEN_HEIGHT_KEY, 480);
	fsDepth = TCUserDefaults::longForKey(FULLSCREEN_DEPTH_KEY, 32);
	showStatusBar = TCUserDefaults::longForKey(STATUS_BAR_KEY, 1, false) != 0;
	topmost = TCUserDefaults::longForKey(TOPMOST_KEY, 0, false) != 0;
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
	if (modelViewer)
	{
		modelViewer->setExtraSearchDirs(extraSearchDirs);
	}
	modelWindow->finalSetup();
}

void LDViewWindow::setScreenSaver(BOOL flag)
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

char* LDViewWindow::windowClassName(void)
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
	DWORD backgroundColor = TCUserDefaults::longForKey(BACKGROUND_COLOR_KEY);
 	HBRUSH hBrush = CreateSolidBrush(RGB(backgroundColor & 0xFF,
		(backgroundColor >> 8) & 0xFF, (backgroundColor >> 16) & 0xFF));

	debugPrintf(2, "updateRect size1: %d, %d\n",
		updateRect->right - updateRect->left,
		updateRect->bottom - updateRect->top);
	if (!fullScreen && !screenSaver)
	{
		int bottomMargin = 2;
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
	}
	debugPrintf(2, "updateRect size2: %d, %d\n",
		updateRect->right - updateRect->left,
		updateRect->bottom - updateRect->top);
	FillRect(hdc, updateRect, hBrush);
	DeleteObject(hBrush);
	if (noRect)
	{
		delete updateRect;
		updateRect = NULL;
	}

	CUIWindow::doEraseBackground(updateRect);
	return 0;
}

void LDViewWindow::forceShowStatusBar(bool value)
{
	if (value != showStatusBarOverride)
	{
		showStatusBarOverride = value;
		if (!showStatusBar)
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

void LDViewWindow::showStatusIcon(bool examineMode)
{
	if ((showStatusBar || showStatusBarOverride) && hStatusBar)
	{
		HICON hModeIcon = hExamineIcon;
		char *tipText = "Examine Mode";

		if (!examineMode)
		{
			hModeIcon = hFlythroughIcon;
			tipText = "Fly-through Mode";
		}
		SendMessage(hStatusBar, SB_SETICON, 2, (LPARAM)hModeIcon);
		SendMessage(hStatusBar, SB_SETTIPTEXT, 2, (LPARAM)tipText);
	}
}

void LDViewWindow::createStatusBar(void)
{
	if (showStatusBar || showStatusBarOverride)
	{
		int parts[] = {100, 100, -1};
		HWND hProgressBar;
		RECT rect;

		ModelWindow::initCommonControls(ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES);
		hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP |
			SBT_TOOLTIPS, "", hWindow, 42000);
		SetWindowLong(hStatusBar, GWL_EXSTYLE, WS_EX_TRANSPARENT);
		SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM)parts);
//		SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)"");
//		SendMessage(hStatusBar, SB_SETTEXT, 0 | SBT_OWNERDRAW, (LPARAM)"");
		SendMessage(hStatusBar, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)"");
		SendMessage(hStatusBar, SB_GETRECT, 0, (LPARAM)&rect);
		InflateRect(&rect, -4, -3);
//		OffsetRect(&rect, -2, 0);
		hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "",
			WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rect.left,
			rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hStatusBar, NULL, hInstance, NULL);
		SendMessage(hStatusBar, SB_GETRECT, 2, (LPARAM)&rect);
		parts[1] += rect.right - rect.left - 32;
		SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM)parts);
		if (modelWindow && modelWindow->getViewMode() == LDVViewFlythrough)
		{
			showStatusIcon(false);
		}
		else
		{
			showStatusIcon(true);
		}
//		SendMessage(hProgressBar, PBM_SETPOS, 50, 0);
		if (modelWindow)
		{
			modelWindow->setStatusBar(hStatusBar);
			modelWindow->setProgressBar(hProgressBar);
		}
		RedrawWindow(hStatusBar, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

void LDViewWindow::reflectViewMode(void)
{
	switch (TCUserDefaults::longForKey(VIEW_MODE_KEY, 0, false))
	{
	case LDVViewExamine:
		switchToExamineMode();
		break;
	case LDVViewFlythrough:
		switchToFlythroughMode();
		break;
	default:
		switchToExamineMode();
		break;
	}
}

BOOL LDViewWindow::initWindow(void)
{
	BOOL retValue;

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
	else
	{
		hWindowMenu = LoadMenu(getLanguageModule(),
			MAKEINTRESOURCE(IDR_MAIN_MENU));
		windowStyle = standardWindowStyle;
		if (TCUserDefaults::longForKey(WINDOW_MAXIMIZED_KEY, 0, false))
		{
			windowStyle |= WS_MAXIMIZE;
		}
		if (topmost)
		{
			exWindowStyle |= WS_EX_TOPMOST;
		}
		else
		{
			exWindowStyle &= ~WS_EX_TOPMOST;
		}
	}
	retValue = CUIWindow::initWindow();
	hFileMenu = GetSubMenu(GetMenu(hWindow), 0);
	hViewMenu = GetSubMenu(GetMenu(hWindow), 2);
	reflectViewMode();
	populateRecentFileMenuItems();
	updateModelMenuItems();
	if (!fullScreen && !screenSaver)
	{
		setMenuCheck(hViewMenu, ID_VIEW_ALWAYSONTOP, topmost);
	}
	return retValue;
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

void LDViewWindow::createAboutBox(void)
{
	hAboutWindow = createDialog(IDD_ABOUT_BOX);
}

BOOL LDViewWindow::doLDrawDirOK(HWND hDlg)
{
	int length = SendDlgItemMessage(hDlg, IDC_LDRAWDIR, WM_GETTEXTLENGTH, 0, 0);

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
	if (modelWindow)
	{
		if (keyFlags & MK_CONTROL)
		{
			modelWindow->setClipZoom(YES);
		}
		else
		{
			modelWindow->setClipZoom(NO);
		}
		modelWindow->zoom((float)zDelta * -0.5f);
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

BOOL LDViewWindow::tryVideoMode(VideoModeT* videoMode, int refreshRate)
{
	if (videoMode)
	{
		DEVMODE deviceMode;
		long result;

		fullScreenActive = YES;

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
		result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
		switch (result)
		{
			case DISP_CHANGE_SUCCESSFUL:
				return YES;
				break;
			case DISP_CHANGE_RESTART:
				fullScreenActive = NO;
				return NO;
				break;
			case DISP_CHANGE_BADFLAGS:
				fullScreenActive = NO;
				return NO;
				break;
			case DISP_CHANGE_BADPARAM:
				fullScreenActive = NO;
				return NO;
				break;
			case DISP_CHANGE_FAILED:
				fullScreenActive = NO;
				return NO;
				break;
			case DISP_CHANGE_BADMODE:
				fullScreenActive = NO;
				return NO;
				break;
			case DISP_CHANGE_NOTUPDATED:
				fullScreenActive = NO;
				return NO;
				break;
		}
		fullScreenActive = NO;
	}
	return NO;
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
		fullScreenActive = NO;
	}
}

void LDViewWindow::setModelWindow(ModelWindow *value)
{
	value->retain();
	if (modelWindow)
	{
		modelWindow->release();
	}
	modelWindow = value;
}

void LDViewWindow::activateFullScreenMode(void)
{
	if (!fullScreenActive)
	{
		skipMinimize = YES;
		if (!getCurrentVideoMode())
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return;
		}
//		modelWindow->uncompile();
		switchingModes = YES;
		if (modelWindow)
		{
			modelWindow->closeWindow();
//			modelWindowShown = false;
		}
		DestroyWindow(hWindow);
		switchingModes = NO;
		setFullScreenDisplayMode();
		if (initWindow())
		{
			if (modelWindow->initWindow())
			{
//				modelWindow->showWindow(SW_SHOW);
//				modelWindowShown = true;
				showWindow(SW_SHOW);
				modelWindow->setNeedsRecompile();
				modelWindow->forceRedraw(1);
			}
		}
		skipMinimize = NO;
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
		ShowWindow(hWindow, SW_MINIMIZE);
		modelWindow->closeWindow();
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

BOOL LDViewWindow::doLDrawDirBrowse(HWND hDlg)
{
	OPENFILENAME openStruct;
	char fileTypes[1024];
	char filename[1024] = "";
	char initialDir[1024];

	SendDlgItemMessage(hDlg, IDC_LDRAWDIR, WM_GETTEXT, (WPARAM)(1024),
		(LPARAM)initialDir);
	memset(fileTypes, 0, 2);
	addFileType(fileTypes, "All Files (*.*)", "*.*");
	memset(&openStruct, 0, sizeof(OPENFILENAME));
	openStruct.lStructSize = sizeof(OPENFILENAME);
	openStruct.lpstrFilter = fileTypes;
	openStruct.nFilterIndex = 1;
	openStruct.lpstrFile = filename;
	openStruct.nMaxFile = 1024;
	openStruct.lpstrInitialDir = initialDir;
	openStruct.lpstrTitle = "Select the LDraw directory";
	openStruct.lpfnHook = lDrawDirBrowseHook;
	openStruct.Flags = OFN_EXPLORER | OFN_NOTESTFILECREATE | OFN_HIDEREADONLY |
		OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK |
		OFN_ENABLESIZING;
	if (GetSaveFileName(&openStruct))
	{
		SendDlgItemMessage(hDlg, IDC_LDRAWDIR, WM_SETTEXT, 0,
			(LPARAM)filename);
	}
	return TRUE;
}

BOOL LDViewWindow::doRemoveExtraDir(void)
{
	int index = SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		extraSearchDirs->removeString(index);
		SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
		if (index >= extraSearchDirs->getCount())
		{
			index--;
		}
		if (index >= 0)
		{
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
	ITEMIDLIST* itemIdList;
	char *currentSelection = NULL;
	int index = SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		currentSelection = (*extraSearchDirs)[index];
	}
	browseInfo.hwndOwner = NULL; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = "Please select the directory you want added to the "
		"search list.";
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)currentSelection;
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		char path[MAX_PATH+10];

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
	}
	return TRUE;
}

BOOL LDViewWindow::doMoveExtraDirUp(void)
{
	int index = SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);
	char *extraDir;

	if (index == LB_ERR || index == 0)
	{
		// we shouldn't get here, but just in case...
		return TRUE;
	}
	extraDir = copyString((*extraSearchDirs)[index]);
	extraSearchDirs->removeString(index);
	SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
	extraSearchDirs->insertString(extraDir, index - 1);
	SendMessage(hExtraDirsList, LB_INSERTSTRING, index - 1, (LPARAM)extraDir);
	SendMessage(hExtraDirsList, LB_SETCURSEL, index - 1, (LPARAM)extraDir);
	updateExtraDirsEnabled();
	delete extraDir;
	return TRUE;
}

BOOL LDViewWindow::doMoveExtraDirDown(void)
{
	int index = SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);
	char *extraDir;

	if (index == LB_ERR || index >= extraSearchDirs->getCount() - 1)
	{
		// we shouldn't get here, but just in case...
		return TRUE;
	}
	extraDir = copyString((*extraSearchDirs)[index]);
	extraSearchDirs->removeString(index);
	SendMessage(hExtraDirsList, LB_DELETESTRING, index, 0);
	extraSearchDirs->insertString(extraDir, index + 1);
	SendMessage(hExtraDirsList, LB_INSERTSTRING, index + 1, (LPARAM)extraDir);
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
				doDialogClose(hDlg);
				break;
			case IDCANCEL:
				doDialogClose(hDlg);
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
	skipMinimize = YES;
	if (!getCurrentVideoMode())
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
	modelWindow->uncompile();
	switchingModes = YES;
	if (modelWindow)
	{
		modelWindow->closeWindow();
//		modelWindowShown = false;
	}
	DestroyWindow(hWindow);
//	modelWindow->forceRedraw(3);
	switchingModes = NO;
	fullScreen = !fullScreen;
	if (fullScreen)
	{
//		debugPrintf("switching to fullscreen...\n");
		setFullScreenDisplayMode();
//		debugPrintf("done.\n");
	}
	else
	{
		int newWidth = TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, 0, false);
		int newHeight = TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, 0, false);
		int dWidth = newWidth - width;
		int dHeight = newHeight - height;

//		debugPrintf("switching from fullscreen...\n");
		restoreDisplayMode();
//		debugPrintf("done\n");
		width = newWidth;
		height = newHeight;
		newWidth = modelWindow->getWidth() + dWidth;
		newHeight = modelWindow->getHeight() + dHeight;
		modelWindow->resize(newWidth, newHeight);
	}
	if (initWindow())
	{
		if (modelWindow->initWindow())
		{
			SetWindowPos(modelWindow->getHWindow(), HWND_TOP, 0, 0,
				width, height, 0);
			showWindow(SW_SHOW);
			if (!fullScreen)
			{
//				debugPrintf("normal.\n");
			}
			modelWindow->setNeedsRecompile();
			skipMinimize = NO;
		}
		else
		{
			skipMinimize = NO;
			if (fullScreen)
			{
				switchModes();
			}
			else
			{
				MessageBox(hWindow, "Error switching back.  Aborting.", "Error",
					MB_OK);
				shutdown();
			}
		}
	}
	modelWindow->forceRedraw();
	SetActiveWindow(hWindow);
	debugPrintf(2, "0x0%08X: Just set active\n", hWindow);
}

void LDViewWindow::shutdown(void)
{
	skipMinimize = YES;
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

void LDViewWindow::selectFSVideoModeMenuItem(int index)
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
		itemInfo.fState = MFS_UNCHECKED;
		SetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
		SetMenuItemInfo(viewMenu, menuIndex, TRUE, &itemInfo);
	}
	currentVideoModeIndex = index;
	videoMode = getCurrentVideoMode();
	fsWidth = videoMode->width;
	fsHeight = videoMode->height;
	fsDepth = videoMode->depth;
	TCUserDefaults::setLongForKey(fsWidth, FULLSCREEN_WIDTH_KEY);
	TCUserDefaults::setLongForKey(fsHeight, FULLSCREEN_HEIGHT_KEY);
	TCUserDefaults::setLongForKey(fsDepth, FULLSCREEN_DEPTH_KEY);
	if (videoMode)
	{
		bitDepthMenu = menuForBitDepth(hWindow, videoMode->depth, &menuIndex);
		itemInfo.fState = MFS_CHECKED;
		SetMenuItemInfo(bitDepthMenu, 30000 + currentVideoModeIndex, FALSE,
			&itemInfo);
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
	itemInfo.fMask = MIIM_TYPE | MIIM_STATE;
	itemInfo.dwTypeData = title;
	for (i = 0; i < count; i++)
	{
		itemInfo.cch = 256;
		GetMenuItemInfo(pollingMenu, i, TRUE, &itemInfo);
		itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
		if (i == pollSetting)
		{
			itemInfo.fState = MFS_CHECKED;
		}
		else
		{
			itemInfo.fState = MFS_UNCHECKED;
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
		if (i == index)
		{
			itemInfo.fState = MFS_CHECKED;
		}
		else
		{
			itemInfo.fState = MFS_UNCHECKED;
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

HMENU LDViewWindow::getPollingMenu(void)
{
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
//			pollingMenu = GetSubMenu(viewMenu, i);
			GetMenuItemInfo(pollingMenu, 0, TRUE, &itemInfo);
			if (itemInfo.wID != ID_FILE_POLLING_DISABLED)
			{
				pollingMenu = 0;
			}
		}
	}
	return pollingMenu;
}

void LDViewWindow::showHelp(void)
{
	HINSTANCE executeHandle;
	char* helpPath = LDViewPreferences::getLDViewPath("Help.html", true);

	setWaitCursor();
	executeHandle = ShellExecute(hWindow, NULL, helpPath, NULL, ".",
		SW_SHOWNORMAL);
	delete helpPath;
	setArrowCursor();
	if ((int)executeHandle <= 32)
	{
		char* errorString;

		switch ((int)executeHandle)
		{
			case 0:
			case SE_ERR_OOM:
				errorString = "Not enough memory.";
				break;
			case ERROR_FILE_NOT_FOUND:
				errorString = "Help.html not found.";
				break;
			case ERROR_PATH_NOT_FOUND:
				errorString = "Couldn't determine path to Help.html.";
				break;
			case SE_ERR_ACCESSDENIED:
				errorString = "Access denied opening Help.html.";
				break;
			case SE_ERR_SHARE:
				errorString = "Sharing violation opening Help.html.";
				break;
			default:
				errorString = "Error opening Help.html";
				break;
		}
		MessageBox(hWindow, errorString, "Error", MB_OK | MB_ICONEXCLAMATION);
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
	MENUITEMINFO itemInfo;

	itemInfo.cbSize = sizeof(itemInfo);
	itemInfo.fMask = MIIM_STATE;
	GetMenuItemInfo(hParentMenu, itemID, byPosition, &itemInfo);
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

void LDViewWindow::updateModelMenuItems(void)
{
	bool haveModel = modelIsLoaded();

	setMenuEnabled(hFileMenu, ID_FILE_SAVE, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_PRINT, haveModel);
	setMenuEnabled(hFileMenu, ID_FILE_PAGESETUP, haveModel);
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
	if (hMenu == GetMenu(hWindow) && menuID == 0)
	{
		// This shouldn't ever be necessary, but it can't hurt.
		updateModelMenuItems();
	}
	return 1;
}

void cleanupMatrix(float *matrix)
{
	int i;

	for (i = 0; i < 16; i++)
	{
		if (fabs(matrix[i]) < 1e-6)
		{
			matrix[i] = 0.0f;
		}
	}
}

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
			float matrix[16];
			float rotationMatrix[16];
			float otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
			char matrixString[1024];
			TRECamera &camera = modelViewer->getCamera();
			TCVector cameraPosition = camera.getPosition();

			memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
				sizeof(rotationMatrix));
			TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
			matrix[12] = cameraPosition[0];
			matrix[13] = cameraPosition[1];
			matrix[14] = cameraPosition[2];
			cleanupMatrix(matrix);
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

void LDViewWindow::showViewInfo(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			float matrix[16];
			float rotationMatrix[16];
			float otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
			char matrixString[1024];
			char zoomString[128];
			char message[4096];
			TRECamera &camera = modelViewer->getCamera();
			float defaultDistance = modelViewer->getDefaultDistance();
			float distanceMultiplier = modelViewer->getDistanceMultiplier();
			float cameraDistance;

			memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
				sizeof(rotationMatrix));
			TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
			cleanupMatrix(matrix);
			sprintf(matrixString,
				"%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g", matrix[0],
				matrix[4], matrix[8], matrix[1], matrix[5], matrix[9],
				matrix[2], matrix[6], matrix[10]);
			cameraDistance = camera.getPosition().length();
			if (distanceMultiplier == 0.0f || cameraDistance == 0.0f)
			{
				// If we don't have a model, we don't know the default zoom, so
				// just say 1.
				strcpy(zoomString, "1");
			}
			else
			{
				sprintf(zoomString, "%.6g", defaultDistance /
					distanceMultiplier / cameraDistance);
			}
			sprintf(message, "The following is the current rotation matrix:\n\n"
				"%s\n\nThe following is the current zoom level:\n\n"
				"%s\n\nHit OK to copy the above information to the clipboard "
				"in a format suitable for the LDView command line.",
				matrixString, zoomString);
			if (MessageBox(hWindow, message, "View info", MB_OKCANCEL) == IDOK)
			{
				char commandLine[1024];

				sprintf(commandLine, "-DefaultMatrix=%s -DefaultZoom=%s",
					matrixString, zoomString);
				copyToClipboard(commandLine);
			}
		}
	}
}

void LDViewWindow::showRotationMatrix(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			float matrix[16];
			float rotationMatrix[16];
			float otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
			char matrixString[1024];

			memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
				sizeof(rotationMatrix));
			TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
			cleanupMatrix(matrix);
			sprintf(matrixString,
				"%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g", matrix[0],
				matrix[4], matrix[8], matrix[1], matrix[5], matrix[9],
				matrix[2], matrix[6], matrix[10]);
			showDefaultMatrix(matrixString, "Rotation Matrix");
		}
	}
}

void LDViewWindow::showLDrawCommandLine(void)
{
	if (modelWindow)
	{
		LDrawModelViewer* modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			HGLOBAL hBuf = GlobalAlloc(GMEM_DDESHARE, 1024);
			char *buf = (char*)GlobalLock(hBuf);
//			char shortFilename[1024];

//			GetShortPathName(modelViewer->getFilename(), shortFilename, 1024);
//			modelViewer->getLDrawCommandLine(shortFilename, buf, 1024);
			modelViewer->getLDGLiteCommandLine(buf, 1024);
			MessageBox(hWindow, buf, "LDraw Command Line", MB_OK);
//			debugPrintf("%s\n", buf);
			GlobalUnlock(hBuf);
			if (OpenClipboard(hWindow))
			{
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hBuf);
				CloseClipboard();
//				GlobalFree(hBuf);
			}
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
		char *vendorString = (char*)glGetString(GL_VENDOR);
		char *rendererString = (char*)glGetString(GL_RENDERER);
		char *versionString = (char*)glGetString(GL_VERSION);
		char *extensionsString = (char*)glGetString(GL_EXTENSIONS);
		char *wglExtensionsString = LDVExtensionsSetup::getWglExtensions();
		int len;
		char *message;
		int parts[2] = {100, -1};
		char buf[128];
		int count;

		if (!vendorString)
		{
			vendorString = "*Unknown*";
		}
		if (!rendererString)
		{
			rendererString = "*Unknown*";
		}
		if (!versionString)
		{
			versionString = "*Unknown*";
		}
		if (!extensionsString)
		{
			extensionsString = "*None*";
		}
		if (!wglExtensionsString)
		{
			wglExtensionsString = "*None*";
		}
		len = strlen(extensionsString);
		if (len && (extensionsString[len - 1] == ' '))
		{
			extensionsString[len - 1] = 0;
		}
		len = strlen(wglExtensionsString);
		if (len && (wglExtensionsString[len - 1] == ' '))
		{
			wglExtensionsString[len - 1] = 0;
		}
		extensionsString = stringByReplacingSubstring(extensionsString, " ",
			"\r\n");
		wglExtensionsString = stringByReplacingSubstring(wglExtensionsString,
			" ", "\r\n");
		len = strlen(vendorString) + strlen(rendererString) +
			strlen(versionString) + strlen(extensionsString) +
			strlen(wglExtensionsString) + 128;
		message = new char[len];
		sprintf(message, "Vendor: %s\r\n"
			"Renderer: %s\r\n"
			"Version: %s\r\n\r\n"
			"Extenstions:\r\n%s\r\n\r\n"
			"WGL Extensions:\r\n%s",
			vendorString, rendererString, versionString, extensionsString,
			wglExtensionsString);
		hOpenGLInfoWindow = createDialog(IDD_OPENGL_INFO);
		SendDlgItemMessage(hOpenGLInfoWindow, IDC_OPENGL_INFO, WM_SETTEXT, 0,
			(LPARAM)message);
		hOpenGLStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE |
			SBARS_SIZEGRIP, "", hOpenGLInfoWindow, 42001);
		SendMessage(hOpenGLStatusBar, SB_SETPARTS, 2, (LPARAM)parts);
		count = countStringLines(extensionsString);
		if (count == 1)
		{
			strcpy(buf, "1 Extension");
		}
		else
		{
			sprintf(buf, "%d Extensions", count);
		}
		SendMessage(hOpenGLStatusBar, SB_SETTEXT, 0, (LPARAM)buf);
		count = countStringLines(wglExtensionsString);
		if (count == 1)
		{
			strcpy(buf, "1 WGL Extension");
		}
		else
		{
			sprintf(buf, "%d WGL Extensions", count);
		}
		SendMessage(hOpenGLStatusBar, SB_SETTEXT, 1, (LPARAM)buf);
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
		delete message;
		delete extensionsString;
		delete wglExtensionsString;
	}
	ShowWindow(hOpenGLInfoWindow, SW_SHOW);
	return 0;
}
/*
{
	int pollSetting = TCUserDefaults::longForKey(POLL_KEY, 0, false);
	HMENU pollingMenu = getPollingMenu();
	int i;
	int count = GetMenuItemCount(pollingMenu);
	MENUITEMINFO itemInfo;
	char title[256];

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_TYPE | MIIM_STATE;
	itemInfo.dwTypeData = title;
	for (i = 0; i < count; i++)
	{
		itemInfo.cch = 256;
		GetMenuItemInfo(pollingMenu, i, TRUE, &itemInfo);
		itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
		if (i == pollSetting)
		{
			itemInfo.fState = MFS_CHECKED;
		}
		else
		{
			itemInfo.fState = MFS_UNCHECKED;
		}
		SetMenuItemInfo(pollingMenu, i, TRUE, &itemInfo);
	}
}
*/

void LDViewWindow::setMenuRadioCheck(HMENU hParentMenu, UINT uItem, bool checked)
{
	setMenuCheck(hParentMenu, uItem, checked, true);
}

bool LDViewWindow::getMenuCheck(HMENU hParentMenu, UINT uItem)
{
	MENUITEMINFO itemInfo;

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_STATE;
	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
	return (itemInfo.fState & MFS_CHECKED) != 0;
}

void LDViewWindow::setMenuCheck(HMENU hParentMenu, UINT uItem, bool checked,
								bool radio)
{
	MENUITEMINFO itemInfo;
	char title[256];

	memset(&itemInfo, 0, sizeof(MENUITEMINFO));
	itemInfo.cbSize = sizeof(MENUITEMINFO);
	itemInfo.fMask = MIIM_STATE | MIIM_TYPE;
	itemInfo.dwTypeData = title;
	itemInfo.cch = 256;
	GetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
	if (checked)
	{
		itemInfo.fState = MFS_CHECKED;
	}
	else
	{
		itemInfo.fState = MFS_UNCHECKED;
	}
	itemInfo.fType = MFT_STRING;
	if (radio)
	{
		itemInfo.fType |= MFT_RADIOCHECK;
	}
	SetMenuItemInfo(hParentMenu, uItem, FALSE, &itemInfo);
}

LRESULT LDViewWindow::switchToExamineMode(void)
{
	setMenuRadioCheck(hViewMenu, ID_VIEW_EXAMINE, true);
	setMenuRadioCheck(hViewMenu, ID_VIEW_FLYTHROUGH, false);
	modelWindow->setViewMode(LDVViewExamine);
	showStatusIcon(true);
	return 0;
}

LRESULT LDViewWindow::switchToFlythroughMode(void)
{
	setMenuRadioCheck(hViewMenu, ID_VIEW_EXAMINE, false);
	setMenuRadioCheck(hViewMenu, ID_VIEW_FLYTHROUGH, true);
	modelWindow->setViewMode(LDVViewFlythrough);
	showStatusIcon(false);
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
				case TTN_GETDISPINFO:
					{
						LPNMTTDISPINFO dispInfo = (LPNMTTDISPINFO)notification;

						switch (controlId)
						{
						case 42:
							strcpy(dispInfo->szText, "Add directory to search list");
							break;
						case 43:
							strcpy(dispInfo->szText,
								"Remove directory from search list");
							break;
						case 44:
							strcpy(dispInfo->szText, "Move directory up");
							break;
						case 45:
							strcpy(dispInfo->szText, "Move directory down");
							break;
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
	int index = SendMessage(hExtraDirsList, LB_GETCURSEL, 0, 0);

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
		TBADDBITMAP addBitmap;
		TBBUTTON buttons[4];
		char buttonTitle[128];
		int i;
		RECT tbRect;

		memset(buttonTitle, 0, sizeof(buttonTitle));
		strcpy(buttonTitle, "");
		ModelWindow::initCommonControls(ICC_WIN95_CLASSES);
		hExtraDirsWindow = createDialog(IDD_EXTRA_DIRS);
		hExtraDirsToolbar = GetDlgItem(hExtraDirsWindow, IDC_ESD_TOOLBAR);
		hExtraDirsList = GetDlgItem(hExtraDirsWindow, IDC_ESD_LIST);
		populateExtraDirsListBox();
		GetClientRect(hExtraDirsToolbar, &tbRect);
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR,
			TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(buttons[0]), 0);
		addBitmap.hInst = getLanguageModule();
		addBitmap.nID = IDB_EXTRA_DIRS;
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_SETINDENT,
			tbRect.right - tbRect.left - 100, 0);
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_SETBUTTONWIDTH,
			0, MAKELONG(25, 25));
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_ADDBITMAP, 4,
			(LPARAM)&addBitmap);
		SendDlgItemMessage(hExtraDirsWindow, IDC_ESD_TOOLBAR, TB_ADDSTRING,
			0, (LPARAM)buttonTitle);
		for (i = 0; i < 4; i++)
		{
			buttons[i].iBitmap = i;
			buttons[i].idCommand = 42 + i;
			buttons[i].fsState = TBSTATE_ENABLED;
			buttons[i].fsStyle = TBSTYLE_BUTTON;
			buttons[i].dwData = (DWORD)this;
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
			TCUserDefaults::setStringForKey(oldDir, LDRAWDIR_KEY, false);
			LDLModel::setLDrawDir(oldDir);
		}
	}
	delete oldDir;
}

void LDViewWindow::resizeModelWindow(int newWidth, int newHeight)
{
	if (modelWindow)
	{
		SetWindowPos(modelWindow->getHWindow(), HWND_TOP, 0, 0, newWidth,
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
	resizeModelWindow(width, height);
	if (modelWindow)
	{
		modelWindow->setStatusBar(NULL);
		modelWindow->setProgressBar(NULL);
	}
	DestroyWindow(hStatusBar);
	hStatusBar = NULL;
	if (needActive)
	{
		SetActiveWindow(hWindow);
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

void LDViewWindow::addStatusBar(void)
{
	createStatusBar();
	resizeModelWindow(width, height - getStatusBarHeight());
}

LRESULT LDViewWindow::switchStatusBar(void)
{
	showStatusBar = !showStatusBar;
	TCUserDefaults::setLongForKey(showStatusBar ? 1 : 0, STATUS_BAR_KEY, false);
	reflectStatusBar();
/*
	if (showStatusBar || showStatusBarOverride)
	{
		addStatusBar();
		setMenuCheck(hViewMenu, ID_VIEW_STATUSBAR, true);
	}
	else
	{
		removeStatusBar();
		setMenuCheck(hViewMenu, ID_VIEW_STATUSBAR, false);
	}
*/
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
	TCUserDefaults::setLongForKey(topmost ? 1 : 0, TOPMOST_KEY, false);
	reflectTopmost();
	return 0;
}

LRESULT LDViewWindow::doTimer(UINT timerID)
{
	switch (timerID)
	{
	case DOWNLOAD_TIMER:
		TCThreadManager *threadManager = TCThreadManager::threadManager();

		if (threadManager->timedWaitForFinishedThread(0))
		{
			TCThread *finishedThread;

			while ((finishedThread = threadManager->getFinishedThread()) !=
				NULL)
			{
				threadManager->removeFinishedThread(finishedThread);
			}
		}
		break;
	}
	return 0;
}

void LDViewWindow::fetchHeaderFinish(TCWebClient* webClient)
{
	debugPrintf("fetchHeaderFinish: 0x%08X\n", GetCurrentThreadId());
	webClient->fetchURLInBackground();
}

void LDViewWindow::fetchURLFinish(TCWebClient* webClient)
{
	int dataLength = webClient->getPageLength();

	debugPrintf("fetchURLFinish: 0x%08X\n", GetCurrentThreadId());
	if (dataLength)
	{
		BYTE *data = webClient->getPageData();
		char *string = new char[dataLength + 1];

		memcpy(string, data, dataLength);
		string[dataLength] = 0;
		debugPrintf("Got Page Data!\n");
		debugPrintf(3, "%s\n", string);
		delete string;
	}
	else
	{
		debugPrintf("No Page Data!\n");
	}
	webClient->release();
	setDebugLevel(0);
}

void LDViewWindow::downloadTest(void)
{
	TCWebClient *webClient;
//	TCThreadManager *threadManager = TCThreadManager::threadManager();

	setDebugLevel(3);
	debugPrintf("downloadTest: 0x%08X\n", GetCurrentThreadId());
//	webClient = new TCWebClient("http://www.google.com/");
	webClient = new TCWebClient("http://www.ldraw.org/cgi-bin/ptreleases.cgi");
	webClient->setOwner(this);
	webClient->setFinishHeaderMemberFunction((WebClientFinishMemberFunction)
		&LDViewWindow::fetchHeaderFinish);
	webClient->setFinishURLMemberFunction((WebClientFinishMemberFunction)
		&LDViewWindow::fetchURLFinish);
/*
	if (webClient->fetchURL())
	{
		fetchURLFinish(webClient);
	}
	else
	{
		webClient->release();
		setDebugLevel(0);
	}
*/
	webClient->fetchHeaderInBackground();
	setTimer(DOWNLOAD_TIMER, 250);
}

LRESULT LDViewWindow::doCommand(int itemId, int notifyCode, HWND controlHWnd)
{
	char* message = NULL;

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
			break;
		case ID_FILE_OPEN:
			openModel();
			return 0;
			break;
		case ID_FILE_PRINT:
			printModel();
			return 0;
			break;
		case ID_FILE_PAGESETUP:
			pageSetup();
			return 0;
			break;
		case ID_FILE_SAVE:
			saveSnapshot();
			return 0;
			break;
		case ID_FILE_LDRAWDIR:
			chooseNewLDrawDir();
			return 0;
			break;
		case ID_FILE_EXTRADIRS:
			chooseExtraDirs();
			return 0;
			break;
		case ID_FILE_CANCELLOAD:
			if (modelWindow)
			{
				modelWindow->setCancelLoad();
			}
			return 0;
			break;
		case ID_FILE_EXIT:
			shutdown();
			return 0;
			break;
		case ID_FILE_DOWNLOADTEST:
			downloadTest();
			return 0;
			break;
		case ID_VIEW_FULLSCREEN:
			switchModes();
			return 0;
			break;
		case ID_VIEW_RESET:
			resetView();
			return 0;
			break;
		case ID_VIEW_DEFAULT:
			resetView();
			return 0;
			break;
		case ID_VIEW_FRONT:
			resetView(LDVAngleFront);
			return 0;
			break;
		case ID_VIEW_BACK:
			resetView(LDVAngleBack);
			return 0;
			break;
		case ID_VIEW_LEFT:
			resetView(LDVAngleLeft);
			return 0;
			break;
		case ID_VIEW_RIGHT:
			resetView(LDVAngleRight);
			return 0;
			break;
		case ID_VIEW_TOP:
			resetView(LDVAngleTop);
			return 0;
			break;
		case ID_VIEW_BOTTOM:
			resetView(LDVAngleBottom);
			return 0;
			break;
		case ID_VIEW_ISO:
			resetView(LDVAngleIso);
			return 0;
			break;
		case ID_VIEW_SAVE_DEFAULT:
			saveDefaultView();
			return 0;
			break;
		case ID_VIEW_ZOOMTOFIT:
			zoomToFit();
			return 0;
			break;
/*
		case ID_VIEW_RESET_DEFAULT:
			resetDefaultView();
			return 0;
			break;
*/
		case ID_VIEW_ERRORS:
			modelWindow->showErrors();
			return 0;
			break;
		case ID_VIEW_STATUSBAR:
			return switchStatusBar();
			break;
		case ID_VIEW_ALWAYSONTOP:
			return switchTopmost();
			break;
		case ID_VIEW_EXAMINE:
			return switchToExamineMode();
			break;
		case ID_VIEW_FLYTHROUGH:
			return switchToFlythroughMode();
			break;
		case ID_VIEW_INFO:
			showViewInfo();
			return 0;
			break;
/*
		case ID_VIEW_TRANS_MATRIX:
			showTransformationMatrix();
			return 0;
			break;
*/
/*
		case ID_VIEW_LDRAWCOMMANDLINE:
			showLDrawCommandLine();
			return 0;
			break;
*/
		case ID_FILE_RELOAD:
			modelWindow->reload();
			modelWindow->update();
			return 0;
			break;
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
			break;
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
	}
	if (itemId >= ID_HOT_KEY_0 && itemId <= ID_HOT_KEY_9)
	{
		if (modelWindow && modelWindow->performHotKey(itemId - ID_HOT_KEY_0))
		{
			return 0;
		}
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
	if (message)
	{
		float rotationSpeed = modelWindow->getRotationSpeed();
		float zoomSpeed = modelWindow->getZoomSpeed();

		modelWindow->setRotationSpeed(0.0f);
		modelWindow->setZoomSpeed(0.0f);
		MessageBox(NULL, message, "Open GL Info",
			MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
		modelWindow->setRotationSpeed(rotationSpeed);
		modelWindow->setZoomSpeed(zoomSpeed);
		return 0;
	}
	return CUIWindow::doCommand(itemId, notifyCode, controlHWnd);
}

WNDCLASSEX LDViewWindow::getWindowClass(void)
{
	WNDCLASSEX windowClass = CUIWindow::getWindowClass();

	windowClass.hIcon = LoadIcon(getLanguageModule(),
		MAKEINTRESOURCE(IDI_APP_ICON));
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
		else
		{
			TCUserDefaults::setLongForKey(0, WINDOW_MAXIMIZED_KEY, false);
			if (sizeType == SIZE_RESTORED)
			{
				TCUserDefaults::setLongForKey(newWidth, WINDOW_WIDTH_KEY,
					false);
				TCUserDefaults::setLongForKey(newHeight, WINDOW_HEIGHT_KEY,
					false);
			}
		}
		if ((showStatusBar || showStatusBarOverride) && hStatusBar)
		{
			int parts[] = {100, 150, -1};
			RECT rect;

			SendMessage(hStatusBar, WM_SIZE, SIZE_RESTORED,
				MAKELPARAM(newWidth, newHeight));
			SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM)parts);
			SendMessage(hStatusBar, SB_GETRECT, 2, (LPARAM)&rect);
			parts[1] += rect.right - rect.left - 32;
			SendMessage(hStatusBar, SB_SETPARTS, 3, (LPARAM)parts);
		}
	}
	return CUIWindow::doSize(sizeType, newWidth, newHeight);
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
	skipMinimize = YES;
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
			bitDepthMenu = GetSubMenu(viewMenu, i);
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
		filename = TCUserDefaults::stringForKey(key, NULL, false);
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
			TCUserDefaults::setStringForKey(filename, key, false);
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
				char *partialFilename = copyString(filename);

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
		char title[128];
		MENUITEMINFO itemInfo;

		if (!bitDepthMenu)
		{
			memset(&itemInfo, 0, sizeof(MENUITEMINFO));
			sprintf(title, "%d-Bit Modes", videoMode.depth);
			bitDepthMenu = CreatePopupMenu();
			itemInfo.cbSize = sizeof(MENUITEMINFO);
			itemInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA;
			itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
			itemInfo.dwTypeData = title;
			itemInfo.dwItemData = videoMode.depth;
			itemInfo.hSubMenu = bitDepthMenu;
			InsertMenuItem(viewMenu, count, TRUE, &itemInfo);
		}
		sprintf(title, "%dx%d", videoMode.width, videoMode.height);
		memset(&itemInfo, 0, sizeof(MENUITEMINFO));
		itemInfo.cbSize = sizeof(MENUITEMINFO);
		itemInfo.fMask = MIIM_TYPE /*| MIIM_STATE*/ | MIIM_ID;
		itemInfo.fType = MFT_STRING | MFT_RADIOCHECK;
		itemInfo.dwTypeData = title;
		itemInfo.wID = 30000 + i;
/*
		if (currentVideoMode && videoMode.width == currentVideoMode->width &&
			videoMode.height == currentVideoMode->height &&
			videoMode.depth == currentVideoMode->depth)
		{
			itemInfo.fState = MFS_CHECKED;
		}
		else
		{
			itemInfo.fState = MFS_UNCHECKED;
		}
*/
		InsertMenuItem(bitDepthMenu, GetMenuItemCount(bitDepthMenu), TRUE,
			&itemInfo);
	}
}

void LDViewWindow::checkVideoMode(int width, int height, int depth)
{
	DEVMODE deviceMode;
	long result;

	memset(&deviceMode, 0, sizeof DEVMODE);
	deviceMode.dmSize = sizeof DEVMODE;
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	deviceMode.dmPelsWidth = width;
	deviceMode.dmPelsHeight = height;
	deviceMode.dmBitsPerPel = depth;
	result = ChangeDisplaySettings(&deviceMode, CDS_TEST | CDS_FULLSCREEN);
	if (result == DISP_CHANGE_SUCCESSFUL)
	{
		if (videoModes)
		{
			videoModes[numVideoModes].width = width;
			videoModes[numVideoModes].height = height;
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
			BOOL found = NO;

			for (j = 0; j < numVideoModes && !found; j++)
			{
				VideoModeT videoMode = videoModes[j];

				if (videoMode.width == (int)deviceMode.dmPelsWidth &&
					videoMode.height == (int)deviceMode.dmPelsHeight &&
					videoMode.depth == (int)deviceMode.dmBitsPerPel)
				{
					found = YES;
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
		selectFSVideoModeMenuItem(currentVideoModeIndex);
	}
	return retVal;
}

void LDViewWindow::saveDefaultView(void)
{
	modelWindow->saveDefaultView();
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

void LDViewWindow::saveSnapshot(void)
{
	if (modelIsLoaded())
	{
		modelWindow->saveSnapshot();
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

void LDViewWindow::openModel(const char* filename, bool skipLoad)
{
	if (filename && strlen(filename) > 0)
	{
		char* newFilename = NULL;
		char fullPathName[1024];

		if (!verifyLDrawDir())
		{
			return;
		}
		if (filename[0] == '"')
		{
			newFilename = copyString(filename + 1);
			int length = strlen(newFilename);

			if (newFilename[length - 1] == '"')
			{
				newFilename[length - 1] = 0;
			}
		}
		else
		{
			newFilename = copyString(filename);
		}
		if (ModelWindow::chDirFromFilename(newFilename, fullPathName))
		{
			modelWindow->setFilename(fullPathName);
			if (!skipLoad && modelWindow->loadModel())
			{
				updateModelMenuItems();
				setLastOpenFile(fullPathName);
			}
		}
		delete newFilename;
	}
	else
	{
		OPENFILENAME openStruct;
		char fileTypes[1024];
		char openFilename[1024] = "";
		char* initialDir = lastOpenPath();

		if (initialDir)
		{
			memset(fileTypes, 0, 2);
			addFileType(fileTypes, "LDraw Files (*.ldr,*.dat,*.mpd)",
				"*.ldr;*.dat;*.mpd");
			addFileType(fileTypes, "LDraw Model Files (*.ldr,*.dat)",
				"*.ldr;*.dat");
			addFileType(fileTypes, "LDraw Multi-Part Files (*.mpd)", "*.mpd");
			addFileType(fileTypes, "All Files (*.*)", "*.*");
			memset(&openStruct, 0, sizeof(OPENFILENAME));
			openStruct.lStructSize = sizeof(OPENFILENAME);
			openStruct.hwndOwner = hWindow;
			openStruct.lpstrFilter = fileTypes;
			openStruct.nFilterIndex = 1;
			openStruct.lpstrFile = openFilename;
			openStruct.nMaxFile = 1024;
			openStruct.lpstrInitialDir = initialDir;
			openStruct.lpstrTitle = "Select a Model file";
			openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
				OFN_HIDEREADONLY;
			openStruct.lpstrDefExt = "ldr";
			if (GetOpenFileName(&openStruct))
			{
				modelWindow->setFilename(openStruct.lpstrFile);
				setLastOpenFile(openStruct.lpstrFile);
				modelWindow->loadModel();
				updateModelMenuItems();
			}
			delete initialDir;
		}
	}
	populateRecentFileMenuItems();
	if (modelWindowIsShown())
	{
		// Don't activate if it hasn't been shown, because my activation handler
		// shows it, and this makes the window show up with command line image
		// generation.
		SetActiveWindow(hWindow);
	}
}

void LDViewWindow::addFileType(char* fileTypes, char* description, char* filter)
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

char* LDViewWindow::getLDrawDir(void)
{
	char* lDrawDir = TCUserDefaults::stringForKey(LDRAWDIR_KEY, NULL, false);

	if (!lDrawDir)
	{
		char buf[1024];

		if (GetPrivateProfileString("LDraw", "BaseDirectory",
			LDLModel::lDrawDir(), buf, 1024, "ldraw.ini"))
		{
			buf[1023] = 0;
			lDrawDir = copyString(buf);
		}
		else
		{
			// We shouldn't be able to get here, but just to be safe...
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
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

BOOL LDViewWindow::promptForLDrawDir(void)
{
	BROWSEINFO browseInfo;
	char displayName[MAX_PATH];
	ITEMIDLIST* itemIdList;
	char *oldLDrawDir = getLDrawDir();

	browseInfo.hwndOwner = NULL; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = "Please select the directory in which you installed"
		" LDraw.";
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)oldLDrawDir;
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		char path[MAX_PATH+10];

		delete oldLDrawDir;
		if (SHGetPathFromIDList(itemIdList, path))
		{
			stripTrailingPathSeparators(path);
			TCUserDefaults::setStringForKey(path, LDRAWDIR_KEY, false);
			LDLModel::setLDrawDir(path);
			return TRUE;
		}
		MessageBox(NULL/*hWindow*/, "Invalid directory selected.", "Error",
			MB_OK);
	}
	delete oldLDrawDir;
	return FALSE;
/*
	if (!hLDrawDirWindow)
	{
		createLDrawDirWindow();
	}
	if (hLDrawDirWindow)
	{
		runDialogModal(hLDrawDirWindow);
		if (userLDrawDir)
		{
			stripTrailingPathSeparators(userLDrawDir);
			TCUserDefaults::setStringForKey(userLDrawDir, LDRAWDIR_KEY, false);
			LDLModel::setLDrawDir(userLDrawDir);
			delete userLDrawDir;
			userLDrawDir = NULL;
			return TRUE;
		}
	}
	return FALSE;
*/
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

	if (!forceChoose && verifyLDrawDir(lDrawDir))
	{
		delete lDrawDir;
		found = TRUE;
	}
	else
	{
		delete lDrawDir;
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
					MessageBox(NULL, "The directory you selected does not "
						"contain LDraw.", "Invalid directory",
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
		char* path = TCUserDefaults::stringForKey(pathKey, NULL, false);

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

void LDViewWindow::setLastOpenFile(const char* filename, char* pathKey)
{
	if (filename)
	{
		char* spot = strrchr(filename, '\\');
		int index;

		if (!pathKey)
		{
			pathKey = LAST_OPEN_PATH_KEY;
		}
		if (spot)
		{
			int length = spot - filename;
			char* path = new char[length + 1];

			strncpy(path, filename, length);
			path[length] = 0;
			TCUserDefaults::setStringForKey(path, pathKey, false);
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
				recentFiles->removeString(index + 1);
			}
			recordRecentFiles();
		}
	}
}

LRESULT LDViewWindow::doShowWindow(BOOL showFlag, LPARAM status)
{
//	debugPrintf("LDViewWindow::doShowWindow\n");
	if (modelWindow && showFlag)
	{
		if (!modelWindowIsShown())
		{
			modelWindow->showWindow(SW_NORMAL);
		}
		// For some reason, creating the status bar prior to the window being
		// shown results in a status bar that doesn't update properly in XP, so
		// show it here instead of in initWindow.
		reflectStatusBar();
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

LRESULT LDViewWindow::doKeyDown(int keyCode, long keyData)
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

LRESULT LDViewWindow::doKeyUp(int keyCode, long keyData)
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
				TCUserDefaults::longForKey(BACKGROUND_COLOR_KEY);
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
	loading = value;
	if (loading)
	{
		startLoading();
	}
	else
	{
		stopLoading();
	}
}

LRESULT LDViewWindow::doInitMenuPopup(HMENU hPopupMenu, UINT /*uPos*/,
									  BOOL /*fSystemMenu*/)
{
	int i;
	int count = GetMenuItemCount(hPopupMenu);

	for (i = 0; i < count; i++)
	{
		setMenuEnabled(hPopupMenu, i, !loading, TRUE);
	}
	if (hPopupMenu == hFileMenu)
	{
		if (loading)
		{
			setMenuEnabled(hFileMenu, ID_FILE_CANCELLOAD, true);
		}
		else
		{
			setMenuEnabled(hFileMenu, ID_FILE_CANCELLOAD, false);
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

void LDViewWindow::reflectStatusBar(void)
{
	if (fullScreen || screenSaver)
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

void LDViewWindow::applyPrefs(void)
{
	loadSettings();
	reflectViewMode();
	populateRecentFileMenuItems();
	reflectStatusBar();
	reflectTopmost();
	reflectPolling();
	reflectVideoMode();
	if (TCUserDefaults::longForKey(WINDOW_MAXIMIZED_KEY, 0, false))
	{
		ShowWindow(hWindow, SW_MAXIMIZE);
	}
	else
	{
		ShowWindow(hWindow, SW_SHOWNORMAL);
	}
}

