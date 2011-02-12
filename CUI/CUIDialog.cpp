#include "CUIDialog.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

bool CUIDialog::sm_haveMessageForwardId = false;
UINT CUIDialog::sm_messageForwardId = 0;

CUIDialog::CUIDialog(void)
{
}

CUIDialog::CUIDialog(CUIWindow* parentWindow):
CUIWindow(parentWindow, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT)
{
}

CUIDialog::CUIDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIWindow(hParentWindow, hInstance, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT)
{
}

CUIDialog::~CUIDialog(void)
{
}

void CUIDialog::dealloc(void)
{
	CUIWindow::dealloc();
}

INT_PTR CUIDialog::privateDialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	m_curHWnd = hDlg;
	m_curMessage = message;
	m_curWParam = wParam;
	m_curLParam = lParam;
	return dialogProc(hDlg, message, wParam, lParam);
}

BOOL CUIDialog::doInitDialog(HWND /*hKbControl*/)
{
	windowStyle = GetWindowLong(hWindow, GWL_STYLE);
	exWindowStyle = GetWindowLong(hWindow, GWL_EXSTYLE);
	initialized = TRUE;
	return TRUE;
}

INT_PTR CUIDialog::dialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)doInitDialog((HWND)wParam);
		break;

	case WM_COMMAND:
		return (INT_PTR)doPrivateCommand(message, wParam, lParam);
	case WM_DESTROY:
		if (doDestroy() == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_VSCROLL:
		if (doVScroll((int)(short)LOWORD(wParam), (int)(short)HIWORD(wParam),
			(HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_HSCROLL:
		if (doHScroll((int)(short)LOWORD(wParam), (int)(short)HIWORD(wParam),
			(HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_NOTIFY:
		return (INT_PTR)doPrivateNotify(message, wParam, lParam);
	case WM_MOVE:
		if (doMove((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_SIZE:
		if (doSize(wParam, (int)(short)LOWORD(lParam),
			(int)(short)HIWORD(lParam)) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_ENTERMENULOOP:
		if (doEnterMenuLoop(wParam ? true : false) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_EXITMENULOOP:
		if (doExitMenuLoop(wParam ? true : false) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_INITMENUPOPUP:
		if (doInitMenuPopup((HMENU)wParam, (UINT)LOWORD(lParam),
			(BOOL)HIWORD(lParam)) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_KEYDOWN:
		if (doKeyDown((int)wParam, lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_KEYUP:
		if (doKeyUp((int)wParam, lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_CLOSE:
		if (doClose() == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_TIMER:
		if (doTimer(wParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_GETMINMAXINFO:
		if (doGetMinMaxInfo(hDlg, (LPMINMAXINFO)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_MOUSEWHEEL:
		if (doMouseWheel(LOWORD(wParam), HIWORD(wParam),
			(int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_CONTEXTMENU:
		if (doContextMenu((HWND)wParam, (int)(short)LOWORD(lParam),
			(int)(short)HIWORD(lParam)) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_DRAWITEM:
		return doPrivateDrawItem(message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

LRESULT CUIDialog::doContextMenu(HWND /*hWnd*/, int /*xPos*/, int /*yPos*/)
{
	return 1;
}

LRESULT CUIDialog::doVScroll(
	int /*scrollCode*/,
	int /*position*/,
	HWND /*hScrollBar*/)
{
	return 1;
}

LRESULT CUIDialog::doHScroll(
	int /*scrollCode*/,
	int /*position*/,
	HWND /*hScrollBar*/)
{
	return 1;
}

void CUIDialog::doOK(void)
{
	EndDialog(hWindow, (INT_PTR)IDOK);
}

void CUIDialog::doCancel(void)
{
	EndDialog(hWindow, (INT_PTR)IDCANCEL);
}

LRESULT CUIDialog::doTextFieldChange(int /*controlId*/, HWND /*control*/)
{
	return 1;
}

LRESULT CUIDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (notifyCode == BN_CLICKED)
	{
		if (commandId == IDOK)
		{
			doOK();
			return 0;
		}
		else if (commandId == IDCANCEL)
		{
			doCancel();
			return 0;
		}
	}
	else if (notifyCode == EN_CHANGE)
	{
		return doTextFieldChange(commandId, control);
	}
	return CUIWindow::doCommand(notifyCode, commandId, control);
}

void CUIDialog::createDialog(int templateNumber, HWND hParentWnd /*= NULL*/)
{
	createDialog(MAKEINTRESOURCEUC(templateNumber), hParentWnd);
}

void CUIDialog::createDialog(CUCSTR templateName, HWND hParentWnd /*= NULL*/)
{
	CreateDialogParamUC(hInstance, templateName, hParentWnd,
		staticDialogProc, (LPARAM)this);
}

INT_PTR CUIDialog::doModal(UINT dialogId, HWND hWndParent /*= NULL*/)
{
	return doModal(MAKEINTRESOURCEUC(dialogId), hWndParent);
}

INT_PTR CUIDialog::doModal(CUCSTR dialogName, HWND hWndParent /*= NULL*/)
{
	return DialogBoxParamUC(hInstance, dialogName, hWndParent, staticDialogProc,
		(LPARAM)this);
}

INT_PTR CALLBACK CUIDialog::staticDialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	CUIDialog *dialog;

	if (message == WM_INITDIALOG)
	{
		dialog = (CUIDialog *)lParam;
		dialog->hWindow = hDlg;
		SetWindowLongPtrUC(hDlg, GWLP_USERDATA, (LONG_PTR)dialog);
	}
	else
	{
		dialog = (CUIDialog *)GetWindowLongPtrUC(hDlg, GWLP_USERDATA);
	}
	if (dialog)
	{
		return dialog->privateDialogProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
	//return DefDlgProc(hDlg, message, wParam, lParam);
}

bool CUIDialog::checkGet(int buttonId)
{
	return SendDlgItemMessage(hWindow, buttonId, BM_GETCHECK, 0, 0) != 0;
}

void CUIDialog::checkSet(int buttonId, bool value)
{
	SendDlgItemMessage(hWindow, buttonId, BM_SETCHECK, value, 0);
}

void CUIDialog::sliderSetup(
	int controlId,
	short min,
	short max,
	WORD frequency,
	int value)
{
	CUIWindow::setupDialogSlider(hWindow, controlId, min, max, frequency,
		value);
}

void CUIDialog::sliderSetTic(int controlId, int position)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETTIC, 0, position);
}

void CUIDialog::sliderSetValue(int controlId, int value)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETPOS, 1, value);
}

int CUIDialog::sliderGetValue(int controlId)
{
	return (int)SendDlgItemMessage(hWindow, controlId, TBM_GETPOS, 0, 0);
}

void CUIDialog::spinSetup(
	int controlId,
	short min,
	short max,
	int value,
	UDACCEL *accels	/*= NULL*/,
	int numAccels	/* = 0*/)
{

	SendDlgItemMessage(hWindow, controlId, UDM_SETRANGE, 0, MAKELONG((short)max,
		(short)min)); 
	spinSetValue(controlId, value);
	if (accels && numAccels > 0)
	{
		SendDlgItemMessage(hWindow, controlId, UDM_SETACCEL,
			(WPARAM)numAccels, (long)accels);
	}
}

void CUIDialog::spinSetValue(
	int controlId,
	int value)
{
	SendDlgItemMessage(hWindow, controlId, UDM_SETPOS, 0, value);
}

void CUIDialog::listBoxResetContent(int controlId)
{
	SendDlgItemMessage(hWindow, controlId, LB_RESETCONTENT, 0, 0);
}

void CUIDialog::listBoxAddString(int controlId, CUCSTR string)
{
	sendDlgItemMessageUC(hWindow, controlId, LB_ADDSTRING, 0, (LPARAM)string);
}

#ifndef TC_NO_UNICODE
void CUIDialog::listBoxAddString(int controlId, LPCTSTR string)
{
	SendDlgItemMessageA(hWindow, controlId, LB_ADDSTRING, 0, (LPARAM)string);
}
#endif // TC_NO_UNICODE

LRESULT CUIDialog::listBoxSelectItem(int controlId, int index)
{
	return SendDlgItemMessage(hWindow, controlId, LB_SETCURSEL,
		(WPARAM)index, 0);
}

int CUIDialog::listBoxGetSelectedItem(int controlId)
{
	return (int)SendDlgItemMessage(hWindow, controlId, LB_GETCURSEL, 0, 0);
}

void CUIDialog::comboAddString(int controlId, CUCSTR string)
{
	sendDlgItemMessageUC(hWindow, controlId, CB_ADDSTRING, 0, (LPARAM)string);
}

LRESULT CUIDialog::comboSelectItem(int controlId, int index)
{
	return SendDlgItemMessage(hWindow, controlId, CB_SETCURSEL,
		(WPARAM)index, 0);
}

int CUIDialog::comboGetSelectedItem(int controlId)
{
	return (int)SendDlgItemMessage(hWindow, controlId, CB_GETCURSEL, 0, 0);
}

// Note: static method
void CUIDialog::windowGetText(HWND hDlg, int controlId, ucstring &text)
{
	text.resize(SendDlgItemMessage(hDlg, controlId, WM_GETTEXTLENGTH, 0, 0));
	sendDlgItemMessageUC(hDlg, controlId, WM_GETTEXT,
		(WPARAM)text.size() + 1, (LPARAM)&text[0]);
}

void CUIDialog::windowGetText(int controlId, ucstring &text)
{
	windowGetText(hWindow, controlId, text);
}

void CUIDialog::windowSetText(HWND hDlg, int controlId, const ucstring &text)
{
	sendDlgItemMessageUC(hDlg, controlId, WM_SETTEXT, 0, (LPARAM)text.c_str());
}

void CUIDialog::windowSetText(int controlId, const ucstring &text)
{
	windowSetText(hWindow, controlId, text);
}

#ifndef TC_NO_UNICODE

void CUIDialog::windowGetText(int controlId, std::string &text)
{
	windowGetText(hWindow, controlId, text);
}

void CUIDialog::windowGetText(HWND hDlg, int controlId, std::string &text)
{
	text.resize(SendDlgItemMessageA(hDlg, controlId, WM_GETTEXTLENGTH, 0,
		0));
	SendDlgItemMessageA(hDlg, controlId, WM_GETTEXT,
		(WPARAM)text.size() + 1, (LPARAM)&text[0]);
}

void CUIDialog::windowSetText(int controlId, const std::string &text)
{
	windowSetText(hWindow, controlId, text);
}

void CUIDialog::windowSetText(HWND hDlg, int controlId, const std::string &text)
{
	SendDlgItemMessageA(hDlg, controlId, WM_SETTEXT, 0, (LPARAM)text.c_str());
}

#endif // TC_NO_UNICODE

bool CUIDialog::windowGetValue(int controlId, long &value)
{
	std::string text;

	windowGetText(controlId, text);
	return sscanf(text.c_str(), "%d", &value) == 1;
}

bool CUIDialog::windowGetValue(int controlId, float &value)
{
	std::string text;

	windowGetText(controlId, text);
	return sscanf(text.c_str(), "%f", &value) == 1;
}

bool CUIDialog::windowGetValue(int controlId, double &value)
{
	std::string text;

	windowGetText(controlId, text);
	return sscanf(text.c_str(), "%lf", &value) == 1;
}

void CUIDialog::windowSetValue(int controlId, long value)
{
	char buf[32];

	sprintf(buf, "%d", value);
	windowSetText(controlId, buf);
}

void CUIDialog::windowSetValue(int controlId, double value)
{
	char buf[128];

	sprintf(buf, "%g", value);
	windowSetText(controlId, buf);
}

ucstring CUIDialog::windowGetText(int controlId)
{
	ucstring retValue;

	windowGetText(controlId, retValue);
	return retValue;
}

void CUIDialog::textFieldSetLimitText(int controlId, int value)
{
	SendDlgItemMessage(hWindow, controlId, EM_SETLIMITTEXT, (WPARAM)value, 0);
}

void CUIDialog::textFieldGetSelection(int controlId, int &start, int &end)
{
	SendDlgItemMessage(hWindow, controlId, EM_GETSEL, (WPARAM)&start,
		(LPARAM)&end);
}

void CUIDialog::textFieldSetSelection(int controlId, int start, int end)
{
	SendDlgItemMessage(hWindow, controlId, EM_SETSEL, (WPARAM)start,
		(LPARAM)end);
}

void CUIDialog::setIcon(int templateNumber)
{
	setIcon(MAKEINTRESOURCE(templateNumber));
}

void CUIDialog::setIcon(char* templateName)
{
	HICON hBigIcon = (HICON)LoadImage(hInstance, templateName, IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
		LR_DEFAULTCOLOR);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, templateName, IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	SendMessage(hWindow, WM_SETICON, ICON_BIG, (LPARAM)hBigIcon);
	SendMessage(hWindow, WM_SETICON, ICON_SMALL, (LPARAM)hSmallIcon);
}

CUIDialog *CUIDialog::fromHandle(HWND hWnd)
{
	if ((DLGPROC)GetWindowLongPtrUC(hWnd, DWLP_DLGPROC) == staticDialogProc)
	{
		return (CUIDialog *)GetWindowLongPtrUC(hWnd, GWLP_USERDATA);
	}
	return NULL;
}

void CUIDialog::addControl(HWND hWnd)
{
	// Make sure the message id variable is pre-populated.
	registerMessageForwardId();
	m_controls.insert(hWnd);
}

// Note: static method
void CUIDialog::registerMessageForwardId(void)
{
	if (!sm_haveMessageForwardId)
	{
		sm_messageForwardId = RegisterWindowMessage("CUIDialog::MessageForward");
		sm_haveMessageForwardId = true;
	}
}

bool CUIDialog::doPrivateForward(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	LRESULT &lResult)
{
	if (m_controls.find(hWnd) != m_controls.end())
	{
		ControlMessage cm;

		cm.msg = message;
		cm.lParam = lParam;
		cm.wParam = wParam;
		cm.processed = false;
		SendMessage(hWnd, sm_messageForwardId, 0, (LPARAM)&cm);
		if (cm.processed)
		{
			lResult = cm.lResult;
			return true;
		}
	}
	return false;
}

LRESULT CUIDialog::doPrivateNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR notification = (LPNMHDR)lParam;

	return doPrivateMessage1(notification->hwndFrom, message, wParam, lParam);
}

LRESULT CUIDialog::doPrivateCommand(UINT message, WPARAM wParam, LPARAM lParam)
{
	return doPrivateMessage2((HWND)lParam, message, wParam, lParam);
}

LRESULT CUIDialog::doPrivateDrawItem(UINT message, WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;

	return doPrivateMessage2(dis->hwndItem, message, wParam, lParam);
}

LRESULT CUIDialog::doPrivateMessage1(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT lResult;

	if (doPrivateForward(hWnd, message, wParam, lParam, lResult))
	{
		return lResult;
	}
	else
	{
		return doNotify((int)(short)LOWORD(wParam), (LPNMHDR)lParam);
	}
}

LRESULT CUIDialog::doPrivateMessage2(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT lResult;

	if (doPrivateForward(hWnd, message, wParam, lParam, lResult))
	{
		return lResult;
	}
	else
	{
		if (doCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		else
		{
			return (INT_PTR)FALSE;
		}
	}
}
