#include "ToolbarStrip.h"
#include "Resource.h"
#include "LDViewWindow.h"
#include "ModelWindow.h"
#include "LDViewPreferences.h"
#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <CUI/CUIScaler.h>
#if _MSC_VER >= 1300 && !defined(TC_NO_UNICODE)	// VC >= VC 2003
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#pragma warning(push)
#pragma warning(disable:4458)
#include <gdiplus.h>
#include <gdiplusflat.h>
#pragma warning(pop)
#define USE_GDIPLUS
#endif // VC >= VC 2005

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG


#define NUM_DEFAULT_TB_BUTTONS 13
#define STEP_COUNT_CHANGED_TIMER 42

#ifdef USE_GDIPLUS

typedef Gdiplus::Status (WINAPI *PFNGDIPLUSSTARTUP)(
    OUT ULONG_PTR *token,
    const Gdiplus::GdiplusStartupInput *input,
    OUT Gdiplus::GdiplusStartupOutput *output);
typedef VOID (WINAPI *PFNGDIPLUSSHUTDOWN)(ULONG_PTR token);
typedef Gdiplus::GpStatus (WINGDIPAPI *PFNGDIPCREATEBITMAPFROMHICON)(
	HICON hicon, Gdiplus::GpBitmap** bitmap);
typedef Gdiplus::GpStatus (WINGDIPAPI *PFNGDIPCREATEHBITMAPFROMBITMAP)(
	Gdiplus::GpBitmap* bitmap, HBITMAP* hbmReturn, Gdiplus::ARGB background);
typedef Gdiplus::GpStatus (WINGDIPAPI *PFNGDIPDISPOSEIMAGE)(
	Gdiplus::GpImage *image);

static PFNGDIPLUSSTARTUP GdiplusStartup = NULL;
static PFNGDIPLUSSHUTDOWN GdiplusShutdown = NULL;
static PFNGDIPCREATEBITMAPFROMHICON GdipCreateBitmapFromHICON = NULL;
static PFNGDIPCREATEHBITMAPFROMBITMAP GdipCreateHBITMAPFromBitmap = NULL;
static PFNGDIPDISPOSEIMAGE GdipDisposeImage = NULL;
#endif // USE_GDIPLUS

ToolbarStrip::ToolbarStrip(HINSTANCE hInstance):
CUIDialog(hInstance),
m_stdBitmapStartId(-1),
m_tbBitmapStartId(-1),
m_numSteps(0),
m_hDeactivatedTooltip(NULL),
m_hGdiPlus(NULL),
m_showMain(TCUserDefaults::boolForKey(SHOW_MAIN_TOOLBAR_KEY, true, false)),
m_showSteps(TCUserDefaults::boolForKey(SHOW_STEPS_TOOLBAR_KEY, true, false))
{
	ModelWindow::initCommonControls(ICC_BAR_CLASSES | ICC_WIN95_CLASSES);
	TCAlertManager::registerHandler(ModelWindow::alertClass(), this,
		(TCAlertCallback)&ToolbarStrip::modelAlertCallback);
	m_commandMap[ID_TOOLS_ERRORS] = IDR_TB_ERRORS;
	m_commandMap[ID_VIEW_EXAMINE] = IDR_TB_EXAMINE;
	m_commandMap[ID_VIEW_EXAMINE_LAT_LONG] = IDR_TB_LATLONROT;
	m_commandMap[ID_VIEW_FLYTHROUGH] = IDR_TB_FLYTHROUGH;
	m_commandMap[ID_VIEW_WALK] = IDR_TB_WALK;
	m_commandMap[ID_VIEW_FULLSCREEN] = IDR_TB_FULLSCREEN;
	m_commandMap[ID_FILE_OPEN] = IDR_TB_OPEN;
	m_commandMap[ID_EDIT_PREFERENCES] = IDR_TB_PREFERENCES;
	m_commandMap[ID_FILE_RELOAD] = IDR_TB_RELOAD;
	m_commandMap[ID_VIEW_RIGHTSIDEUP] = IDR_TB_RIGHTSIDEUP;
	m_commandMap[ID_FILE_SAVE] = IDR_TB_SAVE;
	m_commandMap[ID_VIEW_ZOOMTOFIT] = IDR_TB_ZOOMTOFIT;
	m_commandMap[ID_FIRST_STEP] = IDR_TB_FIRST_STEP;
	m_commandMap[ID_LAST_STEP] = IDR_TB_LAST_STEP;
	m_commandMap[ID_NEXT_STEP] = IDR_TB_NEXT_STEP;
	m_commandMap[ID_PREV_STEP] = IDR_TB_PREV_STEP;
	m_commandMap[ID_HELP_CONTENTS] = IDR_TB_HELP;
	m_commandMap[ID_TOOLS_MODELTREE] = IDR_TB_MODELTREE;
	m_commandMap[ID_TOOLS_BOUNDINGBOX] = IDR_TB_MODELBBOX;
	m_commandMap[ID_VIEW_ALWAYSONTOP] = IDR_TB_TOPMOST;
	m_commandMap[ID_VIEW_FRONT] = IDR_TB_VIEW_FRONT;
	m_commandMap[ID_VIEW_BACK] = IDR_TB_VIEW_BACK;
	m_commandMap[ID_VIEW_LEFT] = IDR_TB_VIEW_LEFT;
	m_commandMap[ID_VIEW_RIGHT] = IDR_TB_VIEW_RIGHT;
	m_commandMap[ID_VIEW_TOP] = IDR_TB_VIEW_TOP;
	m_commandMap[ID_VIEW_BOTTOM] = IDR_TB_VIEW_BOTTOM;
	m_commandMap[ID_VIEW_ISO] = IDR_TB_VIEW_2_3RDS;
	m_commandMap[ID_VIEW_SPECIFYLATLON] = IDR_TB_LATLON;
	m_commandMap[ID_PRIMITIVES_TEXTURESTUDS] = IDR_TB_STUD;
	m_commandMap[ID_PRIMITIVES_TEXMAPS] = IDR_TB_TEXMAPS;
	m_commandMap[ID_FILE_EXPORT] = IDR_TB_EXPORT;
	m_commandMap[ID_FILE_PRINT] = IDR_TB_PRINT;
	m_commandMap[ID_TOOLS_PARTSLIST] = IDR_TB_PARTSLIST;
	m_commandMap[ID_TOOLS_MPD] = IDR_TB_MPD;
	m_commandMap[ID_TOOLS_POV_CAMERA] = IDR_TB_POVCAMERA;
}

ToolbarStrip::~ToolbarStrip(void)
{
}

void ToolbarStrip::dealloc(void)
{
	if (m_hGdiPlus != NULL)
	{
		FreeLibrary(m_hGdiPlus);
		m_hGdiPlus = NULL;
	}
	for (size_t i = 0; i < m_imageLists.size(); i++)
	{
		ImageList_Destroy(m_imageLists[i]);
	}
	TCAlertManager::unregisterHandler(this);
	m_prefs->release();
	CUIDialog::dealloc();
}

void ToolbarStrip::modelAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_ldviewWindow->getModelWindow())
	{
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelParsed")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelParseCanceled")) == 0)
		{
			updateStep();
			updateNumSteps();
		}
	}
}

void ToolbarStrip::create(LDViewWindow *parent)
{
	m_ldviewWindow = parent;
	m_prefs = m_ldviewWindow->getModelWindow()->getPrefs();
	m_prefs->retain();
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_TOOLBAR),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

void ToolbarStrip::show(void)
{
	::ShowWindow(hWindow, SW_SHOW);
}

void ToolbarStrip::hide(void)
{
	::ShowWindow(hWindow, SW_HIDE);
}

void ToolbarStrip::updateMenus(void)
{
	HMENU hMenu = NULL;

	hMenu = GetMenu(GetParent(hWindow));
	if (hMenu != NULL)
	{
		updateMenuImages(hMenu, true);
		updateMenuImages(m_hMainToolbarMenu);
		updateMenuImages(m_hContextMenu);
		updateMenuImages(m_hPrimitivesMenu);
	}
}

void ToolbarStrip::initToolbar(
	HWND hToolbar,
	TbButtonInfoVector &infos,
	HIMAGELIST hImageList)
{
	TBBUTTON *buttons;
	char buttonTitle[128];
	int i;
	int count;

#ifndef TC_NO_UNICODE
	SendMessage(hToolbar, TB_SETUNICODEFORMAT, (WPARAM)TRUE, 0);
#endif // !TC_NO_UNICODE
	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0,
		TBSTYLE_EX_DRAWDDARROWS | WS_EX_TRANSPARENT);
	memset(buttonTitle, 0, sizeof(buttonTitle));
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hToolbar, TB_SETBUTTONWIDTH, 0, MAKELONG(m_buttonWidth, m_buttonWidth));
	SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
	m_stdBitmapStartId = m_tbBitmapStartId = 0;
	// Note: buttonTitle is an empty string.  No need for Unicode.
	SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM)buttonTitle);
	count = (int)infos.size();
	buttons = new TBBUTTON[count];
	for (i = 0; i < count; i++)
	{
		fillTbButton(buttons[i], infos[i]);
		buttons[i].iBitmap += infos[i].getSelection();
	}
	SendMessage(hToolbar, TB_ADDBUTTONS, count, (LPARAM)buttons);
	if (!CUIThemes::isThemeActive() ||
		((CUIThemes::getThemeAppProperties() & STAP_ALLOW_CONTROLS) == 0))
	{
		SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(m_buttonWidth, m_buttonHeight));
	}
	sizeToolbar(hToolbar, count);
	delete[] buttons;
	SendMessage(hToolbar, TB_SETHOTITEM, (WPARAM)-1, 0);
	ShowWindow(hToolbar, SW_SHOW);
}

void ToolbarStrip::sizeToolbar(HWND hToolbar, int count)
{
	RECT buttonRect;
	RECT rect;
	int lwidth;
	int lheight;

	GetWindowRect(hToolbar, &rect);
	screenToClient(hWindow, &rect);
	if (!SendMessage(hToolbar, TB_GETITEMRECT, count - 1, (LPARAM)&buttonRect))
	{
		buttonRect = rect;
		buttonRect.left -= rect.left;
		buttonRect.right -= rect.right;
	}
	lwidth = buttonRect.right;
	lheight = buttonRect.bottom - buttonRect.top;
	MoveWindow(hToolbar, rect.left, rect.top, lwidth, lheight, FALSE);
}

void ToolbarStrip::fillTbButton(
	TBBUTTON &button,
	const TbButtonInfo &buttonInfo)
{
	button.iBitmap = buttonInfo.getBmpIndex();
	button.idCommand = buttonInfo.getCommandId();
	button.fsState = buttonInfo.getState();
	button.fsStyle = buttonInfo.getStyle();
	button.dwData = (DWORD_PTR)this;
	button.iString = -1;
}

void ToolbarStrip::loadMainToolbarMenus(void)
{
	m_hMainToolbarMenu = LoadMenu(getLanguageModule(),
		MAKEINTRESOURCE(IDR_TOOLBAR_MENU));
	m_hContextMenu = GetSubMenu(m_hMainToolbarMenu, 0);
	m_hWireframeMenu = GetSubMenu(m_hMainToolbarMenu, 1);
	m_hEdgesMenu = GetSubMenu(m_hMainToolbarMenu, 2);
	m_hPrimitivesMenu = GetSubMenu(m_hMainToolbarMenu, 3);
	m_hLightingMenu = GetSubMenu(m_hMainToolbarMenu, 4);
	m_hBFCMenu = GetSubMenu(m_hMainToolbarMenu, 5);
}

void ToolbarStrip::initMainToolbar(void)
{
	DWORD style = GetWindowLong(m_hToolbar, GWL_STYLE);

	loadMainToolbarMenus();
	populateMainTbButtonInfos();
	SendMessage(m_hToolbar, TB_SETSTYLE, 0, style | CCS_ADJUSTABLE);
	fillMainToolbar();
}

void ToolbarStrip::fillMainToolbar(void)
{
	size_t i;
	LongVector mainButtonIDs;
	LongSizeTMap mainButtonsMap;
	TbButtonInfoVector buttonInfos;

	for (i = 0; i < m_mainButtonInfos.size(); i++)
	{
		long commandID = m_mainButtonInfos[i].getCommandId();

		if (i < NUM_DEFAULT_TB_BUTTONS)
		{
			mainButtonIDs.push_back(commandID);
		}
		mainButtonsMap[commandID] = i;
	}
	mainButtonIDs = TCUserDefaults::longVectorForKey(MAIN_TOOLBAR_IDS_KEY,
		mainButtonIDs, false);
	for (i = 0; i < mainButtonIDs.size(); i++)
	{
		int commandId = mainButtonIDs[i];

		if (commandId > 0)
		{
			LongSizeTMap::const_iterator it = mainButtonsMap.find(commandId);

			if (it != mainButtonsMap.end())
			{
				buttonInfos.push_back(m_mainButtonInfos[it->second]);
			}
		}
		else
		{
			addTbSeparatorInfo(buttonInfos);
		}
	}
	initToolbar(m_hToolbar, buttonInfos, m_imageLists.front());
}

void ToolbarStrip::initStepToolbar(void)
{
	populateStepTbButtonInfos();
	initToolbar(m_hStepToolbar, m_stepButtonInfos, m_imageLists.back());
}

void ToolbarStrip::autoSize(void)
{
	RECT rect;
	RECT parentRect;

	GetWindowRect(hWindow, &rect);
	GetClientRect(m_ldviewWindow->getHWindow(), &parentRect);
	screenToClient(m_ldviewWindow->getHWindow(), &rect);
	MoveWindow(hWindow, rect.left, rect.top, parentRect.right - parentRect.left,
		m_stripHeight, TRUE);
}

void ToolbarStrip::initLayout(void)
{
	RECT tbRect;
	RECT rect;
	int delta;
	int left;
	//int right;
	int lmaxHeight;
	size_t i;
	int show = m_showSteps ? SW_SHOW : SW_HIDE;

	ShowWindow(m_hToolbar, m_showMain ? SW_SHOW : SW_HIDE);
	ShowWindow(m_hStepLabel, show);
	ShowWindow(m_hNumStepsLabel, show);
	GetWindowRect(m_hToolbar, &tbRect);
	screenToClient(hWindow, &tbRect);
	GetWindowRect(m_hStepLabel, &rect);
	screenToClient(hWindow, &rect);
	if (m_showMain)
	{
		delta = tbRect.right + 16 - rect.left;
	}
	else
	{
		delta = 4 - rect.left;
	}
	lmaxHeight = tbRect.bottom - tbRect.top;
	for (i = 1; i < m_controls.size(); i++)
	{
		int lheight;

		ShowWindow(m_controls[i], show);
		GetWindowRect(m_controls[i], &rect);
		screenToClient(hWindow, &rect);
		lheight = rect.bottom - rect.top;
		MoveWindow(m_controls[i], rect.left + delta, rect.top,
			rect.right - rect.left, lheight, FALSE);
		if (lheight > lmaxHeight)
		{
			lmaxHeight = lheight;
		}
	}
	m_stripHeight = lmaxHeight + 4;
	lmaxHeight += 4;
	left = 4;
	for (i = 0; i < m_controls.size(); i++)
	{
		int lheight;

		GetWindowRect(m_controls[i], &rect);
		screenToClient(hWindow, &rect);
		lheight = rect.bottom - rect.top;
		MoveWindow(m_controls[i], left, (lmaxHeight - lheight) / 2,
			rect.right - rect.left, lheight, TRUE);
		left += rect.right - rect.left + 2;
	}
	autoSize();
	RedrawWindow(hWindow, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void ToolbarStrip::stepChanged(void)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		std::string text;
		int step;

		windowGetText(IDC_STEP, text);
		if (sscanf(text.c_str(), "%d", &step) == 1)
		{
			m_step = m_ldviewWindow->setStep(step);
			updateStep();
		}
	}
	else
	{
		updateStep();
	}
}

void ToolbarStrip::enableToolbarButton(
	HWND hToolbar,
	UINT buttonId,
	bool enable)
{
	BYTE state = (BYTE)SendMessage(hToolbar, TB_GETSTATE, (WPARAM)buttonId, 0);

	if (state == -1)
	{
		state = 0;
	}
	if (enable)
	{
		state |= TBSTATE_ENABLED;
	}
	else
	{
		state &= ~TBSTATE_ENABLED;
	}
	SendMessage(hToolbar, TB_SETSTATE, (WPARAM)buttonId, MAKELONG(state, 0));
}

void ToolbarStrip::updateStep(void)
{
	LDrawModelViewer *modelViewer =
		m_ldviewWindow->getModelWindow()->getModelViewer();
	bool prevEnabled = false;
	bool nextEnabled = false;

	if (modelViewer)
	{
		m_step = modelViewer->getStep();
	}
	else
	{
		m_step = 0;
	}
	if (m_step > 0)
	{
		UCCHAR buf[1024];

		sucprintf(buf, COUNT_OF(buf), _UC("%d"), m_step);
		windowSetText(IDC_STEP, buf);
		if (GetFocus() == m_hStepField)
		{
			SendMessage(m_hStepField, EM_SETSEL, 0, (LPARAM)-1);
		}
		prevEnabled = m_step > 1;
		nextEnabled = m_step < modelViewer->getNumSteps();
	}
	else
	{
		windowSetText(IDC_STEP, "--");
	}
	enableToolbarButton(m_hStepToolbar, ID_FIRST_STEP, prevEnabled);
	enableToolbarButton(m_hStepToolbar, ID_PREV_STEP, prevEnabled);
	enableToolbarButton(m_hStepToolbar, ID_NEXT_STEP, nextEnabled);
	enableToolbarButton(m_hStepToolbar, ID_LAST_STEP, nextEnabled);
	RedrawWindow(m_hStepField, NULL, NULL, RDW_ERASE | RDW_INVALIDATE |
		RDW_ERASENOW | RDW_UPDATENOW);
	RedrawWindow(m_hStepToolbar, NULL, NULL, RDW_ERASE | RDW_INVALIDATE |
		RDW_ERASENOW | RDW_UPDATENOW);
}

LRESULT ToolbarStrip::doTimer(UINT_PTR timerID)
{
	switch (timerID)
	{
	case STEP_COUNT_CHANGED_TIMER:
		stepCountChanged();
		break;
	}
	return 0;
}

void ToolbarStrip::stepCountChanged(void)
{
	KillTimer(hWindow, STEP_COUNT_CHANGED_TIMER);
	UCCHAR buf[1024] = _UC("");

	if (m_numSteps > 0)
	{
		sucprintf(buf, COUNT_OF(buf), m_numStepsFormat.c_str(), m_numSteps);
	}
	SIZE size;
	HDC hNumStepsLabelDC = ::GetDC(m_hNumStepsLabel);
	HANDLE hFont = (HANDLE)::SendMessage(m_hNumStepsLabel, WM_GETFONT, 0, 0);
	HANDLE hOldFont = ::SelectObject(hNumStepsLabelDC, hFont);
	CUIWindow::getTextExtentPoint32UC(hNumStepsLabelDC, buf, (int)ucstrlen(buf), &size);
	::SelectObject(hNumStepsLabelDC, hOldFont);
	::ReleaseDC(m_hNumStepsLabel, hNumStepsLabelDC);
	RECT rect;
	::GetWindowRect(m_hNumStepsLabel, &rect);
	CUIWindow::screenToClient(hWindow, &rect);
	::MoveWindow(m_hNumStepsLabel, rect.left, rect.top, size.cx + 4,
		rect.bottom - rect.top, FALSE);
	windowSetText(IDC_NUM_STEPS, buf);
	initLayout();
}

void ToolbarStrip::updateNumSteps(void)
{
	LDrawModelViewer *modelViewer =
		m_ldviewWindow->getModelWindow()->getModelViewer();

	if (modelViewer)
	{
		m_numSteps = modelViewer->getNumSteps();
	}
	else
	{
		m_numSteps = 0;
	}
	::SetTimer(hWindow, STEP_COUNT_CHANGED_TIMER, 0, NULL);
}

#ifdef USE_GDIPLUS
void ToolbarStrip::updateMenuImages(HMENU hMenu, bool topMenu /*= false*/)
{
	bool themed = m_ldviewWindow->isVisualStyleEnabled() ||
		TCUserDefaults::boolForKey(FORCE_THEMED_MENUS_KEY, false, false);

	//if (m_hGdiPlus == NULL)
	//{
	//	return;
	//}
	int count = GetMenuItemCount(hMenu);

	for (int i = 0; i < count; i++)
	{
		MENUITEMINFOUC mii;
		UCCHAR stringBuf[1024];
		HBITMAP hOldBitmap = NULL;

		memset(&mii, 0, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_BITMAP;
		if ((!themed || !have32BitBmps) && !topMenu)
		{
			mii.fMask |= MIIM_STRING;
			mii.dwTypeData = stringBuf;
			mii.cch = COUNT_OF(stringBuf);
		}
		GetMenuItemInfoUC(hMenu, i, TRUE, &mii);
		hOldBitmap = mii.hbmpItem;
		if ((!themed || !have32BitBmps) && !topMenu)
		{
			// Window sucks.  When themes are disabled, menu item icons encroach
			// into the beginning of the menu item text.  So, to combat this, we
			// need to add a space to the beginning of all menu items to allow
			// space for the icon.  Since the app's main menu doesn't have any
			// icons, we'll skip the spaces on that.
			ucstring menuString(_UC(" "));
			MENUITEMINFOUC mii2 = mii;

			menuString += mii.dwTypeData;
			mii2.dwTypeData = &menuString[0];
			mii2.fMask = MIIM_STRING;
			SetMenuItemInfoUC(hMenu, i, TRUE, &mii2);
		}
		if (mii.hSubMenu)
		{
			updateMenuImages(mii.hSubMenu);
		}
		else
		{
			ImageIndexMap::const_iterator it = m_imagesMap.find(mii.wID);

			if (it != m_imagesMap.end())
			{
				HBITMAP hMenuBitmap = NULL;

				if (CUIScaler::use32bit())
				{
					double scaleFactor = m_scaleFactor;
					// Menu sizes don't scale exactly with screen scale
					// factor, and we want the icon to be as big as possible
					// without being too big.
					if (scaleFactor > 1.0 && scaleFactor <= 1.25)
					{
						// For some reason, the icon gets squished by Windows
						// if we don't do this. I have no idea why.
						scaleFactor = 1.0;
					}
					hMenuBitmap = TCImage::loadBmpFromPngResource(NULL,
						translateCommandId(mii.wID), scaleFactor, true, 1);
				}
				else
				{
					HIMAGELIST hImageList = m_imageLists[it->second.first];
					HICON hIcon = ImageList_GetIcon(hImageList,
						it->second.second, ILD_TRANSPARENT);
					Gdiplus::GpBitmap *pBitmap;
					if (have32BitBmps)
					{
						if (GdipCreateBitmapFromHICON(hIcon, &pBitmap) ==
							Gdiplus::Ok)
						{
							if (GdipCreateHBITMAPFromBitmap(pBitmap,
								&hMenuBitmap, 0) != Gdiplus::Ok)
							{
								hMenuBitmap = NULL;
							}
							GdipDisposeImage(pBitmap);
						}
					}
					else if (hIcon != NULL)
					{
						ICONINFO ii;
						BITMAP bi;

						if (::GetIconInfo(hIcon, &ii) &&
							::GetObject(ii.hbmColor, sizeof(bi), &bi))
						{
							HWND hParentWnd = ::GetParent(hWindow);
							HDC hdcWin = ::GetDC(hParentWnd);
							HDC hCompatDC = ::CreateCompatibleDC(hdcWin);
							RECT rect;

							rect.left = rect.top = 0;
							// Windows has an off by one error, where it
							// clobbers the right pixel of menu images.
							rect.right = bi.bmWidth + 1;
							rect.bottom = bi.bmHeight;
							hMenuBitmap = ::CreateCompatibleBitmap(hdcWin,
								bi.bmWidth + 1, bi.bmHeight);
							HBITMAP hOldBitmap =
								(HBITMAP)::SelectObject(hCompatDC, hMenuBitmap);
							::FillRect(hCompatDC, &rect,
								::GetSysColorBrush(COLOR_MENU));
							::DrawIconEx(hCompatDC, 0, 0, hIcon, bi.bmWidth,
								bi.bmHeight, 0, NULL, DI_NORMAL);
							::SelectObject(hCompatDC, hOldBitmap);
							::ReleaseDC(hParentWnd, hdcWin);
							::ReleaseDC(NULL, hCompatDC);
						}
					}
					if (hIcon != NULL)
					{
						::DestroyIcon(hIcon);
					}
				}
				if (hMenuBitmap != NULL)
				{
					mii.fMask = MIIM_BITMAP;
					mii.hbmpItem = hMenuBitmap;
					SetMenuItemInfoUC(hMenu, i, TRUE, &mii);
					if (hOldBitmap != NULL)
					{
						DeleteObject(hOldBitmap);
					}
				}
			}
		}
	}
}
#else // USE_GDIPLUS
void ToolbarStrip::updateMenuImages(HMENU, bool)
{
}
#endif // USE_GDIPLUS

BOOL ToolbarStrip::doInitDialog(HWND /*hKbControl*/)
{
	m_hToolbar = GetDlgItem(hWindow, IDC_TOOLBAR);
	m_hStepLabel = GetDlgItem(hWindow, IDC_STEP_LABEL);
	m_hStepField = GetDlgItem(hWindow, IDC_STEP);
	m_hNumStepsLabel = GetDlgItem(hWindow, IDC_NUM_STEPS);
	m_hStepToolbar = GetDlgItem(hWindow, IDC_STEP_TOOLBAR);
	m_controls.push_back(m_hToolbar);
	m_controls.push_back(m_hStepLabel);
	m_controls.push_back(m_hStepField);
	m_controls.push_back(m_hNumStepsLabel);
	m_controls.push_back(m_hStepToolbar);

#ifdef USE_GDIPLUS
#pragma warning(push)
#pragma warning(disable:4996)
	if ((GetVersion() & 0xFF) >= 6)
	{
		// We use GDI+ for creation of 32-bit color menu item bitmaps.  These
		// are only supported in menus in Vista and beyond, so don't even try
		// to load it in earlier OSes.
		m_hGdiPlus = LoadLibrary("gdiplus.dll");
	}
#pragma warning(pop)
	ULONG_PTR gdiplusToken = 0;

	if (m_hGdiPlus != NULL)
	{
		bool started = false;

		GdiplusStartup = (PFNGDIPLUSSTARTUP)GetProcAddress(m_hGdiPlus,
			"GdiplusStartup");
		GdiplusShutdown = (PFNGDIPLUSSHUTDOWN)GetProcAddress(m_hGdiPlus,
			"GdiplusShutdown");
		GdipCreateBitmapFromHICON =
			(PFNGDIPCREATEBITMAPFROMHICON)GetProcAddress(m_hGdiPlus,
			"GdipCreateBitmapFromHICON");
		GdipCreateHBITMAPFromBitmap =
			(PFNGDIPCREATEHBITMAPFROMBITMAP)GetProcAddress(m_hGdiPlus,
			"GdipCreateHBITMAPFromBitmap");
		GdipDisposeImage = (PFNGDIPDISPOSEIMAGE)GetProcAddress(m_hGdiPlus,
			"GdipDisposeImage");

		if (GdiplusStartup != NULL &&
			GdiplusShutdown != NULL &&
			GdipCreateBitmapFromHICON != NULL &&
			GdipCreateHBITMAPFromBitmap != NULL &&
			GdipDisposeImage != NULL)
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;

			// Initialize GDI+.
			if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) ==
				Gdiplus::Ok)
			{
				started = true;
				have32BitBmps = true;
			}
		}
		if (!started)
		{
			FreeLibrary(m_hGdiPlus);
			m_hGdiPlus = NULL;
		}
	}
#endif // USE_GDIPLUS

	windowGetText(IDC_NUM_STEPS, m_numStepsFormat);
	windowSetText(IDC_NUM_STEPS, _UC(""));
	m_scaleFactor = m_ldviewWindow->getScaleFactor();
	if (m_scaleFactor > 1.0)
	{
		m_imageSize = (int)(16.0 * m_scaleFactor);
		m_buttonWidth = (int)(22.0 * m_scaleFactor);
		m_buttonHeight = (int)(24.0 * m_scaleFactor);
	}
	else
	{
		m_imageSize = 16;
		m_buttonWidth = 22;
		m_buttonHeight = 24;
	}
	initMainToolbar();
	initStepToolbar();
	checksReflect();
	updateMenus();
	initLayout();
	updateStep();
	updateNumSteps();
#ifdef USE_GDIPLUS
	if (m_hGdiPlus != NULL)
	{
		GdiplusShutdown(gdiplusToken);
	}
#endif // USE_GDIPLUS
	return TRUE;
}

LRESULT ToolbarStrip::doTextFieldChange(int controlId, HWND control)
{
	return CUIDialog::doTextFieldChange(controlId, control);
}

LRESULT ToolbarStrip::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	switch (commandId)
	{
	case IDOK:
	case EN_KILLFOCUS:
		stepChanged();
		return 0;
	case ID_EDIT_PREFERENCES:
	case ID_FILE_OPEN:
	case ID_FILE_SAVE:
	case ID_FILE_RELOAD:
	case ID_VIEW_RESET:
	case ID_VIEW_DEFAULT:
	case ID_VIEW_FRONT:
	case ID_VIEW_BACK:
	case ID_VIEW_LEFT:
	case ID_VIEW_RIGHT:
	case ID_VIEW_TOP:
	case ID_VIEW_BOTTOM:
	case ID_VIEW_SPECIFYLATLON:
	case ID_VIEW_ISO:
	case ID_VIEW_SAVE_DEFAULT:
	case ID_NEXT_STEP:
	case ID_PREV_STEP:
	case ID_FIRST_STEP:
	case ID_LAST_STEP:
	case ID_TOOLS_ERRORS:
	case ID_VIEW_FULLSCREEN:
	case ID_VIEW_ZOOMTOFIT:
	case ID_HELP_CONTENTS:
	case ID_TOOLS_MODELTREE:
	case ID_TOOLS_BOUNDINGBOX:
	case ID_VIEW_ALWAYSONTOP:
	case ID_FILE_EXPORT:
	case ID_FILE_PRINT:
	case ID_VIEW_EXAMINE_LAT_LONG:
	case ID_TOOLS_PARTSLIST:
	case ID_TOOLS_MPD:
	case ID_TOOLS_POV_CAMERA:
	case ID_VIEW_RIGHTSIDEUP:
		// Forward all these messages to LDViewWindow.
		return SendMessage(m_ldviewWindow->getHWindow(), WM_COMMAND,
			MAKEWPARAM(commandId, notifyCode), (LPARAM)control);
	case IDC_WIREFRAME:
		doWireframe();
		break;
	case IDC_SEAMS:
		doSeams();
		break;
	case IDC_HIGHLIGHTS:
		doEdges();
		break;
	case IDC_PRIMITIVE_SUBSTITUTION:
		doPrimitiveSubstitution();
		break;
	case IDC_LIGHTING:
		doLighting();
		break;
	case IDC_BFC:
		doBfc();
		break;
	case IDC_SHOW_AXES:
		doShowAxes();
		break;
	case IDC_RANDOM_COLORS:
		doRandomColors();
		break;
	case IDC_ALL_CONDITIONAL:
		doAllConditionals();
		break;
	case IDC_CONDITIONAL_CONTROLS:
		doConditionalControls();
		break;
	case IDC_FLAT_SHADING:
		doFlatShading();
		break;
	case IDC_CUTAWAY:
		doCutaway();
		break;
	case IDC_STUD_QUALITY:
		doStudQuality();
		break;
	case IDC_PART_BOUNDING_BOXES:
		doPartBoundingBoxes();
		break;
	case IDC_SMOOTH_CURVES:
		doSmoothCurves();
		break;
	case IDC_TRANS_DEFAULT_COLOR:
		doTransDefaultColor();
		break;
	case ID_WIREFRAME_FOG:
		doFog();
		break;
	case ID_WIREFRAME_REMOVEHIDDENLINES:
		doRemoveHiddenLines();
		break;
	case ID_EDGES_SHOWEDGESONLY:
		doShowEdgesOnly();
		break;
	case ID_EDGES_CONDITIONALLINES:
		doConditionalLines();
		break;
	case ID_EDGES_HIGHQUALITY:
		doHighQualityEdges();
		break;
	case ID_EDGES_ALWAYSBLACK:
		doAlwaysBlack();
		break;
	case ID_PRIMITIVES_TEXTURESTUDS:
		doTextureStuds();
		break;
	case ID_PRIMITIVES_TEXMAPS:
		doTexmaps();
		break;
	case ID_LIGHTING_HIGHQUALITY:
		doQualityLighting();
		break;
	case ID_LIGHTING_SUBDUED:
		doSubduedLighting();
		break;
	case ID_LIGHTING_SPECULARHIGHLIGHT:
		doSpecularHighlight();
		break;
	case ID_LIGHTING_ALTERNATESETUP:
		doAlternateLighting();
		break;
	case ID_LIGHTING_DRAWDATS:
		doDrawLightDats();
		break;
	case ID_LIGHTING_OPTIONALMAIN:
		doOptionalStandardLight();
		break;
	case ID_BFC_REDBACKFACES:
		doRedBackFaces();
		break;
	case ID_BFC_GREENFRONTFACES:
		doGreenFrontFaces();
		break;
	case ID_BFC_BLUENEUTRALFACES:
		doBlueNeutralFaces();
		break;
	case ID_TBCONTEXT_CUSTOMIZE:
		doCustomizeMainToolbar();
		break;
	case ID_TBCONTEXT_MAIN:
		doMainToolbar();
		break;
	case ID_TBCONTEXT_STEPS:
		doStepsToolbar();
		break;
	case ID_VIEW_EXAMINE:
		doViewMode();
		break;
	default:
		return CUIDialog::doCommand(notifyCode, commandId, control);
	}
	return 0;
}

int ToolbarStrip::translateCommandId(int commandId)
{
	IntIntMap::const_iterator it = m_commandMap.find(commandId);
	if (it != m_commandMap.end())
	{
		return it->second;
	}
	return commandId;
}

int ToolbarStrip::addToImageList(int commandId)
{
	int newCommandId = translateCommandId(commandId);
	TCImage *image;
	static bool disableHighRes = false;

//#ifdef _DEBUG
//	// Make sure scaling works, but also make sure high-res versions work.
//	disableHighRes = !disableHighRes;
//#endif
	image = TCImage::createFromResource(NULL, newCommandId, 4, true,
		disableHighRes ? 1.0 : m_scaleFactor);
	if (image != NULL)
	{
		SIZE imageSize = { m_imageSize, m_imageSize };
		int imageIndex = addImageToImageList(m_imageLists.back(), image, imageSize);
		image->release();
		if (imageIndex >= 0)
		{
			m_imagesMap[commandId].first = m_imageLists.size() - 1;
			m_imagesMap[commandId].second = imageIndex;
		}
		return imageIndex;
	}
	return -1;
}

void ToolbarStrip::addTbButtonInfo(
	TbButtonInfoVector &infos,
	CUCSTR tooltipText,
	int commandId,
	BYTE style,
	BYTE state)
{
	infos.resize(infos.size() + 1);
	TbButtonInfo &buttonInfo = infos.back();
	buttonInfo.setTooltipText(tooltipText);
	buttonInfo.setCommandId(commandId);
	buttonInfo.setBmpIndex(addToImageList(commandId));
	buttonInfo.setStyle(style);
	buttonInfo.setState(state);
}

void ToolbarStrip::addTbCheckButtonInfo(
	TbButtonInfoVector &infos,
	CUCSTR tooltipText,
	int commandId,
	bool checked,
	BYTE style,
	BYTE state)
{
	state &= ~TBSTATE_CHECKED;
	if (checked)
	{
		state |= TBSTATE_CHECKED;
	}
	addTbButtonInfo(infos, tooltipText, commandId, style, state);
}

void ToolbarStrip::addTbStateButtonInfo(
	TbButtonInfoVector &infos,
	CUCSTR tooltipText,
	IntVector commandIds,
	int selection,
	BYTE style /*= TBSTYLE_BUTTON*/,
	BYTE state /*= TBSTATE_ENABLED*/)
{
	addTbButtonInfo(infos, tooltipText, commandIds.front(), style,
		state | TBSTATE_CHECKED);
	TbButtonInfo &buttonInfo = infos.back();
	buttonInfo.setSelection(selection);
	for (size_t i = 1; i < commandIds.size(); i++)
	{
		buttonInfo.addCommandId(commandIds[i]);
		addToImageList(commandIds[i]);
	}
}

void ToolbarStrip::addTbSeparatorInfo(TbButtonInfoVector &infos)
{
	infos.resize(infos.size() + 1);
	TbButtonInfo &buttonInfo = infos.back();

	buttonInfo.setStyle(TBSTYLE_SEP);
}

void ToolbarStrip::populateStepTbButtonInfos(void)
{
	if (m_stepButtonInfos.size() == 0)
	{
		m_imageLists.push_back(ImageList_Create(m_imageSize, m_imageSize,
			CUIScaler::imageListCreateFlags(), 0, 10));
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("FirstStep")), ID_FIRST_STEP);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("PrevStep")), ID_PREV_STEP);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("NextStep")), ID_NEXT_STEP);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("LastStep")), ID_LAST_STEP);
	}
}

void ToolbarStrip::syncViewMode(void)
{
	m_viewMode =
		(LDInputHandler::ViewMode)TCUserDefaults::longForKey(VIEW_MODE_KEY, 0,
		false);
}

void ToolbarStrip::populateMainTbButtonInfos(void)
{
	if (m_mainButtonInfos.size() == 0)
	{
		m_imageLists.push_back(ImageList_Create(m_imageSize, m_imageSize,
			CUIScaler::imageListCreateFlags(), 0, 100));
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("OpenFile")),
			ID_FILE_OPEN);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("SaveSnapshot")), ID_FILE_SAVE);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Reload")),
			ID_FILE_RELOAD);
		addTbSeparatorInfo(m_mainButtonInfos);
		m_drawWireframe = m_prefs->getDrawWireframe();
		m_seams = m_prefs->getUseSeams() != 0;
		m_edges = m_prefs->getShowsHighlightLines();
		m_primitiveSubstitution = m_prefs->getAllowPrimitiveSubstitution();
		m_lighting = m_prefs->getUseLighting();
		m_bfc = m_prefs->getBfc();
		m_showAxes = m_prefs->getShowAxes();
		m_randomColors = m_prefs->getRandomColors();
		m_allConditionals = m_prefs->getShowAllConditionalLines();
		m_conditionalControls = m_prefs->getShowConditionalControlPoints();
		m_flat = m_prefs->getUseFlatShading();
		m_wireframeCutaway = m_prefs->getCutawayMode() == LDVCutawayWireframe;
		m_lowStuds = !m_prefs->getQualityStuds();
		m_partBBoxes = m_prefs->getBoundingBoxesOnly();
		m_smoothCurves = m_prefs->getPerformSmoothing();
		m_transDefaultColor = m_prefs->getTransDefaultColor();
		m_texmaps = m_prefs->getTexmaps();
		m_examineLatLong = TCUserDefaults::longForKey(EXAMINE_MODE_KEY,
			LDrawModelViewer::EMFree, false) == LDrawModelViewer::EMLatLong;
		m_modelBoundingBox = m_ldviewWindow->isBoundingBoxVisible();
		m_topmost = m_ldviewWindow->isTopmost();
		syncViewMode();
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Wireframe")), IDC_WIREFRAME,
			m_drawWireframe, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Seams")), IDC_SEAMS, m_seams);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("EdgeLines")), IDC_HIGHLIGHTS, m_edges,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("PrimitiveSubstitution")),
			IDC_PRIMITIVE_SUBSTITUTION, m_primitiveSubstitution,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Lighting")), IDC_LIGHTING, m_lighting,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("RightSideUp")), ID_VIEW_RIGHTSIDEUP);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("SelectView")), ID_VIEWANGLE,
			TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN);
		IntVector viewCommandIds;
		viewCommandIds.push_back(ID_VIEW_EXAMINE);
		viewCommandIds.push_back(ID_VIEW_FLYTHROUGH);
		viewCommandIds.push_back(ID_VIEW_WALK);
		addTbStateButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ViewMode")), viewCommandIds, m_viewMode);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Preferences")), ID_EDIT_PREFERENCES);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ViewLatLonRot")),
			ID_VIEW_EXAMINE_LAT_LONG, m_examineLatLong);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("TransDefaultColor")),
			IDC_TRANS_DEFAULT_COLOR, m_transDefaultColor);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("RandomColors")), IDC_RANDOM_COLORS,
			m_randomColors);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ShowAxes")), IDC_SHOW_AXES, m_showAxes);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("PartBoundingBoxesOnly")),
			IDC_PART_BOUNDING_BOXES, m_partBBoxes);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("BFC")),
			IDC_BFC, m_bfc, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ShowAllConditionals")), IDC_ALL_CONDITIONAL,
			m_allConditionals);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ShowConditionalControls")),
			IDC_CONDITIONAL_CONTROLS, m_conditionalControls);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("WireframeCutaway")), IDC_CUTAWAY,
			m_wireframeCutaway);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("FlatShading")), IDC_FLAT_SHADING, m_flat);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("SmoothCurves")), IDC_SMOOTH_CURVES,
			m_smoothCurves);
		addTbCheckButtonInfo(m_mainButtonInfos, ls(_UC("Texmaps")),
			ID_PRIMITIVES_TEXMAPS, m_texmaps);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("TextureStuds")),
			ID_PRIMITIVES_TEXTURESTUDS, m_prefs->getTextureStuds());
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("LowQualityStuds")), IDC_STUD_QUALITY,
			m_lowStuds);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Export")),
			ID_FILE_EXPORT);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Print")),
			ID_FILE_PRINT);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("FullScreen")), ID_VIEW_FULLSCREEN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("AlwaysOnTop")), ID_VIEW_ALWAYSONTOP,
			m_topmost);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ZoomToFit")), ID_VIEW_ZOOMTOFIT);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("FrontView")), ID_VIEW_FRONT);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("BackView")), ID_VIEW_BACK);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("LeftView")),
			ID_VIEW_LEFT);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("RightView")), ID_VIEW_RIGHT);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("TopView")),
			ID_VIEW_TOP);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("BottomView")), ID_VIEW_BOTTOM);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("LatLonView")), ID_VIEW_SPECIFYLATLON);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("TwoThirdsView")), ID_VIEW_ISO);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ErrorsAndWarnings")), ID_TOOLS_ERRORS);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("PartsList")), ID_TOOLS_PARTSLIST);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ModelTree")), ID_TOOLS_MODELTREE);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ModelBBox")), ID_TOOLS_BOUNDINGBOX,
			m_modelBoundingBox);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("MPDModelSelection")), ID_TOOLS_MPD);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("POVCameraInfo")), ID_TOOLS_POV_CAMERA);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Help")),
			ID_HELP_CONTENTS);
	}
}

LRESULT ToolbarStrip::doToolbarGetInfotip(
	TbButtonInfoVector &infos,
	LPNMTBGETINFOTIPUC dispInfo)
{
	int i;
	int count = (int)infos.size();

	for (i = 0; i < count; i++)
	{
		TbButtonInfo &buttonInfo = infos[i];

		if (buttonInfo.getCommandId() == dispInfo->iItem)
		{
			if (CUIThemes::isThemeLibLoaded())
			{
				//HWND hTooltip = (HWND)SendMessage(dispInfo->hdr.hwndFrom,
				//	TB_GETTOOLTIPS, 0, 0);
				//// Turning off theme support in the tooltip makes it
				//// work properly.  With theme support on, it gets erased
				//// by the OpenGL window immediately after being drawn.
				//// Haven't the foggiest why this happens, but turning
				//// off theme support solves the problem.  This has to
				//// be done every time the tooltip is about to pop up.
				//// Not sure why that is either, but it is.
				//CUIThemes::setWindowTheme(hTooltip, NULL, L"");
			}
			if (m_ldviewWindow->getLoading())
			{
				// Don't allow tooltips to pop up while loading; they
				// prevent the loading from continuing until they go
				// away.
				dispInfo->pszText = NULL;
			}
			else
			{
				ucstrncpy(dispInfo->pszText, buttonInfo.getTooltipText(),
					dispInfo->cchTextMax);
				dispInfo->pszText[dispInfo->cchTextMax - 1] = 0;
			}
			break;
		}
	}
	return 0;
}

LRESULT ToolbarStrip::doMainTbGetButtonInfo(NMTOOLBARUC *notification)
{
	int index = notification->iItem;
	int skipCount = 0;

	for (int i = 0; i <= index && i + skipCount < (int)m_mainButtonInfos.size();
		i++)
	{
		if (m_mainButtonInfos[i].getStyle() == TBSTYLE_SEP)
		{
			skipCount++;
		}
	}
	if (index + skipCount < (int)m_mainButtonInfos.size())
	{
		TbButtonInfo &buttonInfo = m_mainButtonInfos[index + skipCount];

		ucstrncpy(notification->pszText, buttonInfo.getTooltipText(),
			notification->cchText);
		notification->pszText[notification->cchText - 1] = 0;
		notification->cchText = (int)ucstrlen(notification->pszText);
		fillTbButton(notification->tbButton, buttonInfo);
		SetWindowLongPtr(hWindow, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	SetWindowLongPtr(hWindow, DWLP_MSGRESULT, FALSE);
	return FALSE;
}

LRESULT ToolbarStrip::doMainToolbarReset(void)
{
	for (int i = 0; true; i++)
	{
		if (SendMessage(m_hToolbar, TB_DELETEBUTTON, 0, 0))
		{
			char key[128];

			sprintf(key, "%s%02d", MAIN_TOOLBAR_IDS_KEY, i);
			TCUserDefaults::removeValue(key, false);
		}
		else
		{
			break;
		}

	}
	fillMainToolbar();
	initLayout();
	return 0;
}

LRESULT ToolbarStrip::doMainToolbarChange(void)
{
	LongVector mainButtonIDs;

	for (int i = 0; true; i++)
	{
		TBBUTTON button;

		if (SendMessage(m_hToolbar, TB_GETBUTTON, i, (LPARAM)&button))
		{
			mainButtonIDs.push_back(button.idCommand);
		}
		else
		{
			break;
		}
	}
	TCUserDefaults::setLongVectorForKey(mainButtonIDs, MAIN_TOOLBAR_IDS_KEY,
		false);
	sizeToolbar(m_hToolbar, (int)mainButtonIDs.size());
	initLayout();
	return 0;	// ignored
}

LRESULT ToolbarStrip::doMainToolbarNotify(int controlId, LPNMHDR notification)
{
	//debugPrintf("LDViewWindow::doMainToolbarNotify: 0x%04X, %s\n", controlId,
	//	notificationName(notification->code).c_str());
	switch (notification->code)
	{
	case TBN_GETINFOTIPUC:
		return doToolbarGetInfotip(m_mainButtonInfos,
			(LPNMTBGETINFOTIPUC)notification);
	case TBN_DROPDOWN:
		if (notification->idFrom == ID_TOOLBAR ||
			notification->idFrom == IDC_TOOLBAR)
		{
			doDropDown((LPNMTOOLBAR)notification);
		}
		break;
	case TBN_QUERYDELETE:
	case TBN_QUERYINSERT:
		SetWindowLongPtr(hWindow, DWLP_MSGRESULT, TRUE);
		return TRUE;
	case TBN_GETBUTTONINFOUC:
		return doMainTbGetButtonInfo((NMTOOLBARUC *)notification);
	case TBN_TOOLBARCHANGE:
		return doMainToolbarChange();
	case TBN_RESET:
		return doMainToolbarReset();
	default:
		return CUIWindow::doNotify(controlId, notification);
	}
	return 0;
}

LRESULT ToolbarStrip::doStepToolbarNotify(int controlId, LPNMHDR notification)
{
	//debugPrintf("LDViewWindow::doStepToolbarNotify: 0x%04X, %s\n", controlId,
	//	notificationName(notification->code).c_str());
	switch (notification->code)
	{
	case TBN_GETINFOTIPUC:
		return doToolbarGetInfotip(m_stepButtonInfos,
			(LPNMTBGETINFOTIPUC)notification);
	default:
		return CUIWindow::doNotify(controlId, notification);
	}
	return 0;
}

LRESULT ToolbarStrip::doNotify(int controlId, LPNMHDR notification)
{
	if (notification->hwndFrom == m_hToolbar)
	{
		return doMainToolbarNotify(controlId, notification);
	}
	else if (notification->hwndFrom == m_hStepToolbar)
	{
		return doStepToolbarNotify(controlId, notification);
	}
	else
	{
		return CUIDialog::doNotify(controlId, notification);
	}
}

void ToolbarStrip::enableMainToolbarButton(UINT buttonId, bool enable)
{
	enableToolbarButton(m_hToolbar, buttonId, enable);
}

void ToolbarStrip::updateContextMenu(void)
{
	setMenuCheck(m_hContextMenu, ID_TBCONTEXT_MAIN, m_showMain);
	setMenuCheck(m_hContextMenu, ID_TBCONTEXT_STEPS, m_showSteps);
}

void ToolbarStrip::updateWireframeMenu(void)
{
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_FOG,
		m_prefs->getUseWireframeFog());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_REMOVEHIDDENLINES,
		m_prefs->getRemoveHiddenLines());
	setMenuItemsEnabled(m_hWireframeMenu, m_drawWireframe);
}

void ToolbarStrip::updateEdgesMenu(void)
{
	setMenuCheck(m_hEdgesMenu, ID_EDGES_SHOWEDGESONLY,
		m_prefs->getEdgesOnly());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_CONDITIONALLINES,
		m_prefs->getDrawConditionalHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_HIGHQUALITY,
		m_prefs->getUsePolygonOffset());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_ALWAYSBLACK,
		m_prefs->getBlackHighlights());
	setMenuItemsEnabled(m_hEdgesMenu, m_edges);
}

void ToolbarStrip::updatePrimitivesMenu(void)
{
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXTURESTUDS,
		m_prefs->getTextureStuds());
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXMAPS,
		m_texmaps);
	setMenuItemsEnabled(m_hPrimitivesMenu, m_primitiveSubstitution);
}

void ToolbarStrip::updateLightingMenu(void)
{
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_HIGHQUALITY,
		m_prefs->getQualityLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SUBDUED,
		m_prefs->getSubduedLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SPECULARHIGHLIGHT,
		m_prefs->getUsesSpecular());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_ALTERNATESETUP,
		m_prefs->getOneLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_DRAWDATS,
		m_prefs->getDrawLightDats());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN,
		m_prefs->getOptionalStandardLight());
	setMenuItemsEnabled(m_hLightingMenu, m_lighting);
	if (!m_prefs->getDrawLightDats())
	{
		setMenuEnabled(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN, false,
			FALSE);
		setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN, false);
	}
}

void ToolbarStrip::updateBFCMenu(void)
{
	setMenuCheck(m_hBFCMenu, ID_BFC_REDBACKFACES,
		m_prefs->getRedBackFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_GREENFRONTFACES,
		m_prefs->getGreenFrontFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_BLUENEUTRALFACES,
		m_prefs->getBlueNeutralFaces());
	setMenuItemsEnabled(m_hBFCMenu, m_bfc);
}

void ToolbarStrip::doDropDown(LPNMTOOLBAR toolbarNot)
{
	RECT rect;
	TPMPARAMS tpm;
	HMENU hMenu = NULL;

    SendMessage(toolbarNot->hdr.hwndFrom, TB_GETRECT, (WPARAM)toolbarNot->iItem,
		(LPARAM)&rect);
	MapWindowPoints(toolbarNot->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rect, 2);
	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude.top    = rect.top;
	tpm.rcExclude.left   = rect.left;
	tpm.rcExclude.bottom = rect.bottom;
	tpm.rcExclude.right  = rect.right;
	switch (toolbarNot->iItem)
	{
	case IDC_WIREFRAME:
		hMenu = m_hWireframeMenu;
		break;
	case IDC_HIGHLIGHTS:
		hMenu = m_hEdgesMenu;
		break;
	case IDC_PRIMITIVE_SUBSTITUTION:
		hMenu = m_hPrimitivesMenu;
		break;
	case IDC_LIGHTING:
		hMenu = m_hLightingMenu;
		break;
	case IDC_BFC:
		hMenu = m_hBFCMenu;
		break;
	case ID_VIEWANGLE:
		hMenu = m_ldviewWindow->getViewAngleMenu();
		break;
	}
	if (hMenu)
	{
		BOOL fade;

		// Note: selection fade causes a display glitch.  This is an obscenely
		// ugly hack to get rid of that, but it does work.  The problem still
		// exists when the menu item is selected from the main menu, though.
		// Did I mention yet today that Microsoft sucks?
		SystemParametersInfo(SPI_GETSELECTIONFADE, 0, &fade, 0);
		if (fade)
		{
			// We better pray it doesn't crash between here and where we put
			// things back below.
			SystemParametersInfo(SPI_SETSELECTIONFADE, FALSE, NULL, 0);
		}
		TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			rect.left, rect.bottom, hWindow, &tpm);
		if (fade)
		{
			SystemParametersInfo(SPI_SETSELECTIONFADE, 0, &fade, 0);
		}
	}
}

void ToolbarStrip::forceRedraw(void)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

	if (modelWindow)
	{
		modelWindow->forceRedraw();
	}
}

void ToolbarStrip::doWireframe(void)
{
	if (doCheck(m_drawWireframe, IDC_WIREFRAME))
	{
		m_prefs->setDrawWireframe(m_drawWireframe);
		forceRedraw();
	}
}

void ToolbarStrip::doSeams(void)
{
	if (doCheck(m_seams, IDC_SEAMS))
	{
		m_prefs->setUseSeams(m_seams);
		forceRedraw();
	}
}

void ToolbarStrip::doEdges(void)
{
	if (doCheck(m_edges, IDC_HIGHLIGHTS))
	{
		m_prefs->setShowsHighlightLines(m_edges);
		forceRedraw();
	}
}

void ToolbarStrip::doPrimitiveSubstitution(void)
{
	if (doCheck(m_primitiveSubstitution, IDC_PRIMITIVE_SUBSTITUTION))
	{
		m_prefs->setAllowPrimitiveSubstitution(m_primitiveSubstitution);
		forceRedraw();
	}
}

void ToolbarStrip::doLighting(void)
{
	if (doCheck(m_lighting, IDC_LIGHTING))
	{
		m_prefs->setUseLighting(m_lighting);
		forceRedraw();
	}
}

void ToolbarStrip::doFog(void)
{
	m_prefs->setUseWireframeFog(!m_prefs->getUseWireframeFog());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_FOG,
		m_prefs->getUseWireframeFog());
}

void ToolbarStrip::doRemoveHiddenLines(void)
{
	m_prefs->setRemoveHiddenLines(!m_prefs->getRemoveHiddenLines());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_REMOVEHIDDENLINES,
		m_prefs->getRemoveHiddenLines());
}

void ToolbarStrip::doShowEdgesOnly(void)
{
	m_prefs->setEdgesOnly(!m_prefs->getEdgesOnly());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_SHOWEDGESONLY,
		m_prefs->getEdgesOnly());
}

void ToolbarStrip::doConditionalLines(void)
{
	m_prefs->setDrawConditionalHighlights(!m_prefs->getDrawConditionalHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_CONDITIONALLINES,
		m_prefs->getDrawConditionalHighlights());
}

void ToolbarStrip::doHighQualityEdges(void)
{
	m_prefs->setUsePolygonOffset(!m_prefs->getUsePolygonOffset());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_HIGHQUALITY,
		m_prefs->getUsePolygonOffset());
}

void ToolbarStrip::doAlwaysBlack(void)
{
	m_prefs->setBlackHighlights(!m_prefs->getBlackHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_ALWAYSBLACK,
		m_prefs->getBlackHighlights());
}

void ToolbarStrip::doTexmaps(void)
{
	m_texmaps = !m_texmaps;
	m_prefs->setTexmaps(m_texmaps);
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXMAPS, m_texmaps);
	forceRedraw();
	checksReflect();
}

void ToolbarStrip::doTextureStuds(void)
{
	m_prefs->setTextureStuds(!m_prefs->getTextureStuds());
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXTURESTUDS,
		m_prefs->getTextureStuds());
	forceRedraw();
	checksReflect();
}

void ToolbarStrip::doQualityLighting(void)
{
	m_prefs->setQualityLighting(!m_prefs->getQualityLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_HIGHQUALITY,
		m_prefs->getQualityLighting());
}

void ToolbarStrip::doSubduedLighting(void)
{
	m_prefs->setSubduedLighting(!m_prefs->getSubduedLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SUBDUED,
		m_prefs->getSubduedLighting());
}

void ToolbarStrip::doSpecularHighlight(void)
{
	m_prefs->setUsesSpecular(!m_prefs->getUsesSpecular());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SPECULARHIGHLIGHT,
		m_prefs->getUsesSpecular());
}

void ToolbarStrip::doAlternateLighting(void)
{
	m_prefs->setOneLight(!m_prefs->getOneLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_ALTERNATESETUP,
		m_prefs->getOneLight());
}

void ToolbarStrip::doDrawLightDats(void)
{
	m_prefs->setDrawLightDats(!m_prefs->getDrawLightDats());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_DRAWDATS,
		m_prefs->getDrawLightDats());
}

void ToolbarStrip::doOptionalStandardLight(void)
{
	m_prefs->setOptionalStandardLight(!m_prefs->getOptionalStandardLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN,
		m_prefs->getOptionalStandardLight());
}

void ToolbarStrip::doRedBackFaces(void)
{
	m_prefs->setRedBackFaces(!m_prefs->getRedBackFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_REDBACKFACES,
		m_prefs->getRedBackFaces());
}

void ToolbarStrip::doGreenFrontFaces(void)
{
	m_prefs->setGreenFrontFaces(!m_prefs->getGreenFrontFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_GREENFRONTFACES,
		m_prefs->getGreenFrontFaces());
}

void ToolbarStrip::doBlueNeutralFaces(void)
{
	m_prefs->setBlueNeutralFaces(!m_prefs->getBlueNeutralFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_BLUENEUTRALFACES,
		m_prefs->getBlueNeutralFaces());
}

void ToolbarStrip::doCustomizeMainToolbar(void)
{
	SendMessage(m_hToolbar, TB_CUSTOMIZE, 0, 0);
}

void ToolbarStrip::doMainToolbar(void)
{
	m_showMain = !m_showMain;
	TCUserDefaults::setBoolForKey(m_showMain, SHOW_MAIN_TOOLBAR_KEY, false);
	initLayout();
}

void ToolbarStrip::doViewMode(void)
{
	int commandId = ID_VIEW_EXAMINE;

	if (m_viewMode == LDInputHandler::VMExamine)
	{
		commandId = ID_VIEW_FLYTHROUGH;
	}
	else if (m_viewMode == LDInputHandler::VMFlyThrough)
	{
		commandId = ID_VIEW_WALK;
	}
	SendMessage(m_ldviewWindow->getHWindow(), WM_COMMAND,
		MAKEWPARAM(commandId, BN_CLICKED), (LPARAM)m_hToolbar);
}

void ToolbarStrip::doStepsToolbar(void)
{
	m_showSteps = !m_showSteps;
	TCUserDefaults::setBoolForKey(m_showSteps, SHOW_STEPS_TOOLBAR_KEY, false);
	initLayout();
}

void ToolbarStrip::doBfc(void)
{
	if (doCheck(m_bfc, IDC_BFC))
	{
		m_prefs->setBfc(m_bfc);
		forceRedraw();
	}
}

void ToolbarStrip::doShowAxes(void)
{
	if (doCheck(m_showAxes, IDC_SHOW_AXES))
	{
		m_prefs->setShowAxes(m_showAxes);
		forceRedraw();
	}
}

void ToolbarStrip::doRandomColors(void)
{
	if (doCheck(m_randomColors, IDC_RANDOM_COLORS))
	{
		m_prefs->setRandomColors(m_randomColors);
		forceRedraw();
	}
}

void ToolbarStrip::doAllConditionals(void)
{
	if (doCheck(m_allConditionals, IDC_ALL_CONDITIONAL))
	{
		m_prefs->setShowAllConditionalLines(m_allConditionals);
		forceRedraw();
	}
}

void ToolbarStrip::doConditionalControls(void)
{
	if (doCheck(m_conditionalControls, IDC_CONDITIONAL_CONTROLS))
	{
		m_prefs->setShowConditionalControlPoints(m_conditionalControls);
		forceRedraw();
	}
}

void ToolbarStrip::doFlatShading(void)
{
	if (doCheck(m_flat, IDC_FLAT_SHADING))
	{
		m_prefs->setUsesFlatShading(m_flat);
		if (m_flat && m_smoothCurves)
		{
			m_prefs->setPerformSmoothing(false);
			checksReflect();
		}
		forceRedraw();
	}
}

void ToolbarStrip::doCutaway(void)
{
	if (doCheck(m_wireframeCutaway, IDC_CUTAWAY))
	{
		m_prefs->setCutawayMode(m_wireframeCutaway ? LDVCutawayWireframe :
			LDVCutawayNormal);
		forceRedraw();
	}
}

void ToolbarStrip::doStudQuality(void)
{
	if (doCheck(m_lowStuds, IDC_STUD_QUALITY))
	{
		m_prefs->setQualityStuds(!m_lowStuds);
		forceRedraw();
	}
}

void ToolbarStrip::doPartBoundingBoxes(void)
{
	if (doCheck(m_partBBoxes, IDC_PART_BOUNDING_BOXES))
	{
		m_prefs->setBoundingBoxesOnly(m_partBBoxes);
		forceRedraw();
	}
}

void ToolbarStrip::doSmoothCurves(void)
{
	if (doCheck(m_smoothCurves, IDC_SMOOTH_CURVES))
	{
		m_prefs->setPerformSmoothing(m_smoothCurves);
		if (m_flat && m_smoothCurves)
		{
			m_prefs->setUseFlatShading(false);
			checksReflect();
		}
		forceRedraw();
	}
}

void ToolbarStrip::doTransDefaultColor(void)
{
	if (doCheck(m_transDefaultColor, IDC_TRANS_DEFAULT_COLOR))
	{
		m_prefs->setTransDefaultColor(m_transDefaultColor);
		forceRedraw();
	}
}

bool ToolbarStrip::doCheck(bool &value, LPARAM commandId)
{
	BYTE state = (BYTE)SendMessage(m_hToolbar, TB_GETSTATE, commandId, 0);
	bool newValue = false;

	if (state & TBSTATE_CHECKED)
	{
		newValue = true;
	}
	if (newValue != value)
	{
		if (m_hDeactivatedTooltip)
		{
			SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 1, 0);
		}
		m_hDeactivatedTooltip = (HWND)SendMessage(m_hToolbar, TB_GETTOOLTIPS, 0,
			0);
		SendMessage(m_hDeactivatedTooltip, TTM_POP, 0, 0);
		SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 0, 0);
		value = newValue;
		return true;
	}
	else
	{
		return false;
	}
}

void ToolbarStrip::activateDeactivatedTooltip(void)
{
	if (m_hDeactivatedTooltip)
	{
		SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 1, 0);
	}
}

LRESULT ToolbarStrip::doInitMenuPopup(
	HMENU hPopupMenu,
	UINT /*uPos*/,
	BOOL /*fSystemMenu*/)
{
	if (hPopupMenu == m_hContextMenu)
	{
		updateContextMenu();
	}
	else if (hPopupMenu == m_hWireframeMenu)
	{
		updateWireframeMenu();
	}
	else if (hPopupMenu == m_hEdgesMenu)
	{
		updateEdgesMenu();
	}
	else if (hPopupMenu == m_hPrimitivesMenu)
	{
		updatePrimitivesMenu();
	}
	else if (hPopupMenu == m_hLightingMenu)
	{
		updateLightingMenu();
	}
	else if (hPopupMenu == m_hBFCMenu)
	{
		updateBFCMenu();
	}
	return 1;
}

void ToolbarStrip::checkReflect(bool &value, bool prefsValue, LPARAM commandID)
{
	if (value != prefsValue)
	{
		BYTE state = (BYTE)SendMessage(m_hToolbar, TB_GETSTATE, commandID, 0);

		value = prefsValue;
		state &= ~TBSTATE_CHECKED;
		if (value)
		{
			state |= TBSTATE_CHECKED;
		}
		SendMessage(m_hToolbar, TB_SETSTATE, commandID, MAKELONG(state, 0));
	}
}

void ToolbarStrip::viewModeReflect(void)
{
	int oldViewMode = m_viewMode;
	syncViewMode();
	int bmpIndex = (int)SendMessage(m_hToolbar, TB_GETBITMAP, ID_VIEW_EXAMINE, 0);
	TBBUTTONINFO bi;
	bmpIndex += m_viewMode - oldViewMode;
	memset(&bi, 0, sizeof(bi));
	bi.cbSize = sizeof(bi);
	bi.dwMask = TBIF_IMAGE;
	bi.iImage = bmpIndex;
	SendMessage(m_hToolbar, TB_SETBUTTONINFO, ID_VIEW_EXAMINE, (LPARAM)&bi);
	checksReflect();
}

void ToolbarStrip::checksReflect(void)
{
	checkReflect(m_drawWireframe, m_prefs->getDrawWireframe(), IDC_WIREFRAME);
	checkReflect(m_seams, m_prefs->getUseSeams() != 0, IDC_SEAMS);
	checkReflect(m_edges, m_prefs->getShowsHighlightLines(), IDC_HIGHLIGHTS);
	checkReflect(m_primitiveSubstitution,
		m_prefs->getAllowPrimitiveSubstitution(), IDC_PRIMITIVE_SUBSTITUTION);
	checkReflect(m_lighting, m_prefs->getUseLighting(), IDC_LIGHTING);
	checkReflect(m_bfc, m_prefs->getBfc(), IDC_BFC);
	checkReflect(m_showAxes, m_prefs->getShowAxes(), IDC_SHOW_AXES);
	checkReflect(m_randomColors, m_prefs->getRandomColors(), IDC_RANDOM_COLORS);
	checkReflect(m_allConditionals, m_prefs->getShowAllConditionalLines(),
		IDC_ALL_CONDITIONAL);
	checkReflect(m_conditionalControls,
		m_prefs->getShowConditionalControlPoints(), IDC_CONDITIONAL_CONTROLS);
	checkReflect(m_flat, m_prefs->getUseFlatShading(), IDC_FLAT_SHADING);
	checkReflect(m_lowStuds, !m_prefs->getQualityStuds(), IDC_STUD_QUALITY);
	checkReflect(m_partBBoxes, m_prefs->getBoundingBoxesOnly(),
		IDC_PART_BOUNDING_BOXES);
	checkReflect(m_smoothCurves, m_prefs->getPerformSmoothing(),
		IDC_SMOOTH_CURVES);
	checkReflect(m_transDefaultColor, m_prefs->getTransDefaultColor(),
		IDC_TRANS_DEFAULT_COLOR);
	checkReflect(m_modelBoundingBox, m_ldviewWindow->isBoundingBoxVisible(),
		ID_TOOLS_BOUNDINGBOX);
	checkReflect(m_topmost, m_ldviewWindow->isTopmost(), ID_VIEW_ALWAYSONTOP);
	checkReflect(m_wireframeCutaway,
		m_prefs->getCutawayMode() == LDVCutawayWireframe ||
		m_prefs->getCutawayMode() == LDVCutawayStencil, IDC_CUTAWAY);
	bool dummy = !m_prefs->getTextureStuds();
	checkReflect(dummy, !dummy, ID_PRIMITIVES_TEXTURESTUDS);
	checkReflect(m_texmaps, m_prefs->getTexmaps(), ID_PRIMITIVES_TEXMAPS);
	if (m_viewMode == LDInputHandler::VMExamine)
	{
		checkReflect(m_examineLatLong,
			TCUserDefaults::longForKey(EXAMINE_MODE_KEY,
			LDrawModelViewer::EMFree, false) == LDrawModelViewer::EMLatLong,
			ID_VIEW_EXAMINE_LAT_LONG);
		enableMainToolbarButton(ID_VIEW_EXAMINE_LAT_LONG, true);
	}
	else
	{
		checkReflect(m_examineLatLong, false, ID_VIEW_EXAMINE_LAT_LONG);
		enableMainToolbarButton(ID_VIEW_EXAMINE_LAT_LONG, false);
	}
}

LRESULT ToolbarStrip::doEnterMenuLoop(bool /*isTrackPopupMenu*/)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

	if (m_ldviewWindow->getLoading())
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

LRESULT ToolbarStrip::doExitMenuLoop(bool /*isTrackPopupMenu*/)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

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

LRESULT ToolbarStrip::doContextMenu(HWND hWnd, int xPos, int yPos)
{
	if (m_hContextMenu != NULL)
	{
		BOOL fade;

		//setMenuEnabled(m_hContextMenu, ID_TBCONTEXT_CUSTOMIZE,
		//	hWnd == m_hToolbar);
		// Note: selection fade causes a display glitch.  This is an obscenely
		// ugly hack to get rid of that, but it does work.  The problem still
		// exists when the menu item is selected from the main menu, though.
		// Did I mention yet today that Microsoft sucks?
		SystemParametersInfo(SPI_GETSELECTIONFADE, 0, &fade, 0);
		if (fade)
		{
			// We better pray it doesn't crash between here and where we put
			// things back below.
			SystemParametersInfo(SPI_SETSELECTIONFADE, FALSE, NULL, 0);
		}
		TrackPopupMenuEx(m_hContextMenu,
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			xPos, yPos, hWindow, NULL);
		if (fade)
		{
			SystemParametersInfo(SPI_SETSELECTIONFADE, 0, &fade, 0);
		}
		return 0;
	}
	return CUIDialog::doContextMenu(hWnd, xPos, yPos);
}
