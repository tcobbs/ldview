#include "CUIPropertySheet.h"
#include <commctrl.h>
#include <TCFoundation/mystring.h>

CUIPropertySheet *CUIPropertySheet::globalCUIPropertySheet = NULL;

CUIPropertySheet::CUIPropertySheet(char *windowTitle, HINSTANCE hInstance):
	CUIWindow(windowTitle, hInstance, 0, 0, 100, 100),
	hDlgParent(NULL),
	hPropSheet(NULL),
	hwndArray(new CUIHwndArray),
	hpageArray(new CUIHPropSheetPageArray),
	applyEnabledArray(new TCArray),
	isModeless(false)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "CUIOGLWindow");
#endif
}

CUIPropertySheet::~CUIPropertySheet(void)
{
}

void CUIPropertySheet::dealloc(void)
{
	closePropertySheet(true);
	if (hwndArray)
	{
		hwndArray->release();
	}
	if (hpageArray)
	{
		hpageArray->release();
	}
	if (applyEnabledArray)
	{
		applyEnabledArray->release();
	}
	CUIWindow::dealloc();
}

int CUIPropertySheet::run(void)
{
	int retValue;

	globalCUIPropertySheet = this;
	retValue = createPropSheet();
	if (!isModeless)
	{
		clear();
	}
	return retValue;
}

int CUIPropertySheet::show(void)
{
	if (hPropSheet)
	{
		SetForegroundWindow(hPropSheet);
		return (int)hPropSheet;
	}
	else
	{
		isModeless = true;
		return run();
	}
}

void CUIPropertySheet::clear(void)
{
	hPropSheet = NULL;
	hwndArray->removeAll();
	hpageArray->removeAll();
}

void CALLBACK timerProc(HWND /*hWnd*/, UINT /*uMsg*/, UINT idEvent,
						DWORD /*dwTime*/)
{
	((CUIPropertySheet*)idEvent)->closePropertySheet(true);
}

void CUIPropertySheet::closePropertySheet(bool immediate)
{
	if (immediate)
	{
		if (hDlgParent)
		{
			KillTimer(hDlgParent, (UINT)this);
		}
		if (hPropSheet)
		{
			DestroyWindow(hPropSheet);
			clear();
		}
	}
	else
	{
		if (hDlgParent && hPropSheet)
		{
			SetTimer(hDlgParent, (UINT)this, 0, timerProc);
		}
	}
}

int CUIPropertySheet::createPropSheet(void)
{
	PROPSHEETHEADER psHeader;
	void *phPages = hpageArray->getItems();

	memset(&psHeader, 0, sizeof(PROPSHEETHEADER));
	psHeader.dwSize = sizeof(PROPSHEETHEADER);
	psHeader.dwFlags = PSH_USECALLBACK;
	if (isModeless)
	{
		psHeader.dwFlags |= PSH_MODELESS;
	}
	if (hDlgParent)
	{
		psHeader.hwndParent = hDlgParent;
	}
	else
	{
		psHeader.hwndParent = hWindow;
	}
	psHeader.hInstance = hInstance;
	psHeader.pszCaption = windowTitle;
	psHeader.nPages = hwndArray->getCount();
	psHeader.nStartPage = 0;
	psHeader.phpage = (HPROPSHEETPAGE*)phPages;
	psHeader.pfnCallback = staticPropSheetProc;
	return PropertySheet(&psHeader);
}

void CUIPropertySheet::addPage(int resourceId, char* title)
{
	PROPSHEETPAGE page;

	memset(&page, 0, sizeof(PROPSHEETPAGE));
	page.dwSize = sizeof(PROPSHEETPAGE);
	if (title)
	{
		page.dwFlags = PSP_USETITLE;
	}
	else
	{
		page.dwFlags = PSP_DEFAULT;
	}
	page.hInstance = hInstance;
	page.pszTemplate = MAKEINTRESOURCE(resourceId);
//	page.hIcon = ???
	page.pszTitle = title;
	page.pfnDlgProc = staticPropDialogProc;
	page.lParam = (LPARAM)this;
	page.pfnCallback = NULL;
	hpageArray->addPointer(CreatePropertySheetPage(&page));
	hwndArray->addPointer(NULL);
	applyEnabledArray->addItem(NULL);
}

BOOL CALLBACK CUIPropertySheet::staticPropDialogProc(HWND hDlg, UINT message,
													 WPARAM wParam,
													 LPARAM lParam)
{
	HWND hParentDlg = (HWND)GetWindowLong(hDlg, GWL_HWNDPARENT);
	CUIPropertySheet *cuiPropertySheet =
		(CUIPropertySheet *)GetWindowLong(hParentDlg, GWL_USERDATA);

//	debugPrintf("CUIPropertySheet::staticPropDialogProc(0x%04X, 0x%08X, "
//		"0x%08X\n", message, wParam, lParam);
//	printMessageName(message);
	if (cuiPropertySheet)
	{
		return cuiPropertySheet->dialogProc(hDlg, message, wParam, lParam);
	}
	return TRUE;
}

int CALLBACK CUIPropertySheet::staticPropSheetProc(HWND hDlg, UINT,
												   LPARAM)
{
	if (hDlg)
	{
		if (globalCUIPropertySheet)
		{
			SetWindowLong(hDlg, GWL_USERDATA, (long)globalCUIPropertySheet);
			CUIPropertySheet::clearGlobalCUIPropertySheet();
		}
	}
	return 0;
}

BOOL CUIPropertySheet::doDialogNotify(HWND hDlg, int controlId,
									  LPNMHDR notification)
{
//	debugPrintf("CUIPropertySheet::doDialogNotify: %d\n", notification->code);
	if (hPropSheet == NULL)
	{
		hPropSheet = notification->hwndFrom;
	}
	switch (notification->code)
	{
	case PSN_SETACTIVE:
		{
			TCITEM *item;
			HWND hTabControl = PropSheet_GetTabControl(hPropSheet);
			int i;
			int count = hwndArray->getCount();

			if (!shouldSetActive(TabCtrl_GetCurSel(hTabControl)))
			{
				SetWindowLong(hDlg, DWL_MSGRESULT, -1);
				return TRUE;
			}
			item = (TCITEM*)malloc(sizeof(TCITEM) + 4);
			item->mask = TCIF_PARAM;
			for (i = 0; i < count; i++)
			{
				if (!hwndArray->pointerAtIndex(i))
				{
					if (TabCtrl_GetItem(hTabControl, i, item))
					{
						HWND hWnd = (HWND)item->lParam;

						if (hWnd)
						{
							hwndArray->replacePointer((HWND)item->lParam, i);
							setupPage(i);
						}
					}
				}
			}
			free(item);
		}
		return TRUE;
		break;
	case PSN_APPLY:
		if (doApply())
		{
			LPPSHNOTIFY pshNotification = (LPPSHNOTIFY)notification;
			WORD idButton;

			clearApplyEnabled();
			SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
			if (hDlgParent && pshNotification->lParam)
			{
				idButton = CUI_OK;
			}
			else
			{
				idButton = CUI_APPLY;
			}
			SendMessage(hDlgParent, WM_COMMAND, MAKELONG(BN_CLICKED, idButton),
				(LPARAM)hPropSheet);
			return TRUE;
		}
		else
		{
			SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
			return TRUE;
		}
		break;
	case PSN_RESET:
		{
			LPPSHNOTIFY pshNotification = (LPPSHNOTIFY)notification;

			clearApplyEnabled();
			if (pshNotification->lParam)
			{
				PropSheet_PressButton(hPropSheet, PSBTN_CANCEL);
			}
			else
			{
				doReset();
				if (hDlgParent && !pshNotification->lParam)
				{
					SendMessage(hDlgParent, WM_COMMAND,
						MAKELONG(BN_CLICKED, CUI_CANCEL), (LPARAM)hPropSheet);
				}
			}
			return TRUE;
		}
		break;
	}
	return CUIWindow::doDialogNotify(hDlg, controlId, notification);
}

bool CUIPropertySheet::doApply(void)
{
	return false;
}

void CUIPropertySheet::doReset(void)
{
}

void CUIPropertySheet::setupPage(int)
{
}

void CUIPropertySheet::enableApply(HWND hPage)
{
	int index = hwndArray->indexOfPointer(hPage);

	PropSheet_Changed(hPropSheet, hPage);
	if (index >= 0)
	{
		applyEnabledArray->replaceItem((void*)1, index);
	}
}

void CUIPropertySheet::disableApply(HWND hPage)
{
	int index = hwndArray->indexOfPointer(hPage);

	PropSheet_UnChanged(hPropSheet, hPage);
	if (index >= 0)
	{
		applyEnabledArray->replaceItem(NULL, index);
	}
}

void CUIPropertySheet::clearApplyEnabled(void)
{
	int count = applyEnabledArray->getCount();
	int i;

	for (i = 0; i < count; i++)
	{
		applyEnabledArray->replaceItem(NULL, i);
	}
}

bool CUIPropertySheet::getApplyEnabled(void)
{
	int count = applyEnabledArray->getCount();
	int i;

	for (i = 0; i < count; i++)
	{
		if (applyEnabledArray->itemAtIndex(i))
		{
			return true;
		}
	}
	return false;
}

bool CUIPropertySheet::shouldSetActive(int /*index*/)
{
	return true;
}
