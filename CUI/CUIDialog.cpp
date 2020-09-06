#include "CUIDialog.h"
#include <HtmlHelp.h>

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
	fixDialogSizes(hWindow);
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

// Note: static method
void CUIDialog::trackBarSetup(HWND hDlg, int controlId, short min,
	short max, WORD frequency, int pos)
{
	SendDlgItemMessage(hDlg, controlId, TBM_SETRANGE, TRUE,
		(LPARAM)MAKELONG(min, max));
	if (GetWindowLong(GetDlgItem(hDlg, controlId), GWL_STYLE) & TBS_AUTOTICKS)
	{
		SendDlgItemMessage(hDlg, controlId, TBM_SETTICFREQ, frequency, 0);
	}
	trackBarSetPos(hDlg, controlId, pos, true);
}

void CUIDialog::trackBarSetup(
	int controlId,
	short min,
	short max,
	WORD frequency,
	int pos)
{
	trackBarSetup(hWindow, controlId, min, max, frequency, pos);
}

void CUIDialog::trackBarSetTic(int controlId, int position)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETTIC, 0, position);
}

// Note: static method
void CUIDialog::trackBarSetPos(HWND hDlg, int controlId, int pos, bool redraw)
{
	SendDlgItemMessage(hDlg, controlId, TBM_SETPOS, redraw ? TRUE : FALSE, pos);
}

void CUIDialog::trackBarSetPos(int controlId, int pos, bool redraw)
{
	trackBarSetPos(hWindow, controlId, pos, redraw);
}

// Note: static method
int CUIDialog::trackBarGetPos(HWND hDlg, int controlId)
{
	return (int)SendDlgItemMessage(hDlg, controlId, TBM_GETPOS, 0, 0);
}

int CUIDialog::trackBarGetPos(int controlId)
{
	return trackBarGetPos(hWindow, controlId);
}

// Note: static method
void CUIDialog::upDownSetup(
	HWND hDlg,
	int controlId,
	short min,
	short max,
	int pos,
	UDACCEL *accels	/*= NULL*/,
	int numAccels	/* = 0*/)
{

	SendDlgItemMessage(hDlg, controlId, UDM_SETRANGE, 0, MAKELONG(max, min));
	upDownSetPos(hDlg, controlId, pos);
	if (accels && numAccels > 0)
	{
		SendDlgItemMessage(hDlg, controlId, UDM_SETACCEL,
			(WPARAM)numAccels, (LPARAM)accels);
	}
}

void CUIDialog::upDownSetup(
	int controlId,
	short min,
	short max,
	int pos,
	UDACCEL *accels	/*= NULL*/,
	int numAccels	/* = 0*/)
{
	upDownSetup(hWindow, controlId, min, max, pos, accels, numAccels);
}

// Note: static method
void CUIDialog::upDownSetPos(HWND hDlg, int controlId, int pos)
{
	SendDlgItemMessage(hDlg, controlId, UDM_SETPOS, 0, pos);
}

void CUIDialog::upDownSetPos(int controlId, int pos)
{
	upDownSetPos(hWindow, controlId, pos);
}

// Note: static method
HBITMAP CUIDialog::buttonSetBitmap(HWND hDlg, int controlId, HBITMAP hBitmap)
{
	return (HBITMAP)SendDlgItemMessage(hDlg, controlId, BM_SETIMAGE,
		IMAGE_BITMAP, (LPARAM)hBitmap);
}

// Note: static method
HICON CUIDialog::buttonSetIcon(HWND hDlg, int controlId, HICON hIcon)
{
	return (HICON)SendDlgItemMessage(hDlg, controlId, BM_SETIMAGE,
		IMAGE_ICON, (LPARAM)hIcon);
}

// Note: static method
HBITMAP CUIDialog::buttonGetBitmap(HWND hDlg, int controlId)
{
	return (HBITMAP)SendDlgItemMessage(hDlg, controlId, BM_GETIMAGE, IMAGE_BITMAP, 0);
}

// Note: static method
HICON CUIDialog::buttonGetIcon(HWND hDlg, int controlId)
{
	return (HICON)SendDlgItemMessage(hDlg, controlId, BM_GETIMAGE, IMAGE_ICON, 0);
}

// Note: static method
void CUIDialog::listBoxResetContent(HWND hDlg, int controlId)
{
	SendDlgItemMessage(hDlg, controlId, LB_RESETCONTENT, 0, 0);
}

void CUIDialog::listBoxResetContent(int controlId)
{
	listBoxResetContent(hWindow, controlId);
}

// Note: static method
int CUIDialog::listBoxAddString(HWND hDlg, int controlId, CUCSTR string)
{
	return SendDlgItemMessage(hDlg, controlId, LB_ADDSTRING, 0,
		(LPARAM)string);
}

// Note: static method
int CUIDialog::listBoxAddString(
	HWND hDlg,
	int controlId,
	const ucstring& string)
{
	return listBoxAddString(hDlg, controlId, string.c_str());
}

int CUIDialog::listBoxAddString(int controlId, CUCSTR string)
{
	return listBoxAddString(hWindow, controlId, string);
}

int CUIDialog::listBoxAddString(int controlId, const ucstring& string)
{
	return listBoxAddString(hWindow, controlId, string);
}

// Note: static method
int CUIDialog::listBoxSelectItem(HWND hDlg, int controlId, int index)
{
	return (int)SendDlgItemMessage(hDlg, controlId, LB_SETCURSEL,
		(WPARAM)index, 0);
}

int CUIDialog::listBoxSelectItem(int controlId, int index)
{
	return listBoxSelectItem(hWindow, controlId, index);
}

// Note: static method
int CUIDialog::listBoxGetSelectedItem(HWND hDlg, int controlId)
{
	return (int)SendDlgItemMessage(hDlg, controlId, LB_GETCURSEL, 0, 0);
}

int CUIDialog::listBoxGetSelectedItem(int controlId)
{
	return listBoxGetSelectedItem(hWindow, controlId);
}

// Note: static method
int CUIDialog::comboSetCurSel(HWND hDlg, int controlId, int index)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_SETCURSEL, index, 0);
}

int CUIDialog::comboSetCurSel(int controlId, int index)
{
	return comboSetCurSel(hWindow, controlId, index);
}

// Note: static method
int CUIDialog::comboAddString(HWND hDlg, int controlId, CUCSTR string)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_ADDSTRING, 0,
		(LPARAM)string);
}

// Note: static method
int CUIDialog::comboAddString(HWND hDlg, int controlId, const ucstring &string)
{
	return comboAddString(hDlg, controlId, string.c_str());
}

int CUIDialog::comboAddString(int controlId, CUCSTR string)
{
	return comboAddString(hWindow, controlId, string);
}

int CUIDialog::comboAddString(int controlId, const ucstring &string)
{
	return comboAddString(hWindow, controlId, string);
}

// Note: static method
int CUIDialog::comboDeleteString(HWND hDlg, int controlId, int index)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_DELETESTRING,
		(WPARAM)index, 0);
}

int CUIDialog::comboDeleteString(int controlId, int index)
{
	return comboDeleteString(hWindow, controlId, index);
}

// Note: static method
int CUIDialog::comboSelectString(
	HWND hDlg,
	int controlId,
	int startIndex,
	CUCSTR string)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_SELECTSTRING,
		(WPARAM)startIndex, (LPARAM)string);
}

// Note: static method
int CUIDialog::comboSelectString(
	HWND hDlg,
	int controlId,
	int startIndex,
	const ucstring &string)
{
	return comboSelectString(hDlg, controlId, startIndex, string.c_str());
}

int CUIDialog::comboSelectString(int controlId, int startIndex, CUCSTR string)
{
	return comboSelectString(hWindow, controlId, startIndex, string);
}

int CUIDialog::comboSelectString(
	int controlId,
	int startIndex,
	const ucstring &string)
{
	return comboSelectString(hWindow, controlId, startIndex, string);
}

// Note: static method
void CUIDialog::comboResetContent(HWND hDlg, int controlId)
{
	SendDlgItemMessage(hDlg, controlId, CB_RESETCONTENT, 0, 0);
}

void CUIDialog::comboResetContent(int controlId)
{
	comboResetContent(hWindow, controlId);
}

// Note: static method
int CUIDialog::comboGetCurSel(HWND hDlg, int controlId)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_GETCURSEL, 0, 0);
}

int CUIDialog::comboGetCurSel(int controlId)
{
	return comboGetCurSel(hWindow, controlId);
}

// Note: static method
int CUIDialog::comboGetCount(HWND hDlg, int controlId)
{
	return (int)SendDlgItemMessage(hDlg, controlId, CB_GETCOUNT, 0, 0);
}

int CUIDialog::comboGetCount(int controlId)
{
	return comboGetCount(hWindow, controlId);
}

// Note: static method
void CUIDialog::buttonSetCheck(HWND hDlg, int controlId, int state)
{
	SendDlgItemMessage(hDlg, controlId, BM_SETCHECK, (WPARAM)state, 0);
}

void CUIDialog::buttonSetCheck(int controlId, int state)
{
	buttonSetCheck(hWindow, controlId, state);
}

// Note: static method
void CUIDialog::buttonSetChecked(HWND hDlg, int controlId, bool checked)
{
	buttonSetCheck(hDlg, controlId, checked ? BST_CHECKED : BST_UNCHECKED);
}

void CUIDialog::buttonSetChecked(int controlId, bool checked)
{
	buttonSetChecked(hWindow, controlId, checked);
}

// Note: static method
int CUIDialog::buttonGetCheck(HWND hDlg, int controlId)
{
	return (int)SendDlgItemMessage(hDlg, controlId, BM_GETCHECK, 0, 0);
}

int CUIDialog::buttonGetCheck(int controlId)
{
	return buttonGetCheck(hWindow, controlId);
}

// Note: static method
bool CUIDialog::buttonIsChecked(HWND hDlg, int controlId)
{
	return buttonGetCheck(hDlg, controlId) == BST_CHECKED;
}

bool CUIDialog::buttonIsChecked(int controlId)
{
	return buttonIsChecked(hWindow, controlId);
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

// Note: static method
void CUIDialog::windowSetText(HWND hWnd, CUCSTR text)
{
	CUIWindow::windowSetText(hWnd, text);
}

// Note: static method
void CUIDialog::windowSetText(HWND hWnd, const ucstring &text)
{
	CUIWindow::windowSetText(hWnd, text);
}

// Note: static method
void CUIDialog::windowSetText(HWND hDlg, int controlId, CUCSTR text)
{
	SendDlgItemMessage(hDlg, controlId, WM_SETTEXT, 0, (LPARAM)text);
}

// Note: static method
void CUIDialog::windowSetText(HWND hDlg, int controlId, const ucstring &text)
{
	windowSetText(hDlg, controlId, text.c_str());
}

void CUIDialog::windowSetText(int controlId, const ucstring &text)
{
	windowSetText(hWindow, controlId, text);
}

// Note: static method
bool CUIDialog::windowGetValue(HWND hDlg, int controlId, long &value)
{
	ucstring text;

	windowGetText(hDlg, controlId, text);
	return sucscanf(text.c_str(), _UC("%ld"), &value) == 1;
}

// Note: static method
bool CUIDialog::windowGetValue(HWND hDlg, int controlId, int &value)
{
	ucstring text;

	windowGetText(hDlg, controlId, text);
	return sucscanf(text.c_str(), _UC("%d"), &value) == 1;
}

// Note: static method
bool CUIDialog::windowGetValue(HWND hDlg, int controlId, float &value)
{
	ucstring text;

	windowGetText(hDlg, controlId, text);
	return sucscanf(text.c_str(), _UC("%f"), &value) == 1;
}

// Note: static method
bool CUIDialog::windowGetValue(HWND hDlg, int controlId, double &value)
{
	ucstring text;

	windowGetText(hDlg, controlId, text);
	return sucscanf(text.c_str(), _UC("%lf"), &value) == 1;
}

bool CUIDialog::windowGetValue(int controlId, long &value)
{
	return windowGetValue(hWindow, controlId, value);
}

bool CUIDialog::windowGetValue(int controlId, int &value)
{
	return windowGetValue(hWindow, controlId, value);
}

bool CUIDialog::windowGetValue(int controlId, float &value)
{
	return windowGetValue(hWindow, controlId, value);
}

bool CUIDialog::windowGetValue(int controlId, double &value)
{
	return windowGetValue(hWindow, controlId, value);
}

// Note: static method
void CUIDialog::windowSetValue(HWND hDlg, int controlId, long value)
{
	UCCHAR buf[32];

	sucprintf(buf, COUNT_OF(buf), _UC("%ld"), value);
	windowSetText(hDlg, controlId, buf);
}

// Note: static method
void CUIDialog::windowSetValue(HWND hDlg, int controlId, int value)
{
	UCCHAR buf[32];

	sucprintf(buf, COUNT_OF(buf), _UC("%d"), value);
	windowSetText(hDlg, controlId, buf);
}

// Note: static method
void CUIDialog::windowSetValue(HWND hDlg, int controlId, double value)
{
	UCCHAR buf[128];

	sucprintf(buf, COUNT_OF(buf), _UC("%g"), value);
	windowSetText(hDlg, controlId, buf);
}

void CUIDialog::windowSetValue(int controlId, long value)
{
	windowSetValue(hWindow, controlId, value);
}

void CUIDialog::windowSetValue(int controlId, int value)
{
	windowSetValue(hWindow, controlId, value);
}

void CUIDialog::windowSetValue(int controlId, double value)
{
	windowSetValue(hWindow, controlId, value);
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

void CUIDialog::setIcon(CUCSTR templateName)
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
		sm_messageForwardId = RegisterWindowMessage(_UC("CUIDialog::MessageForward"));
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

UINT_PTR CUIDialog::DoHtmlHelp(HWND hDlg, LPHELPINFO helpInfo)
{
	RECT itemRect;
	GetWindowRect((HWND)helpInfo->hItemHandle, &itemRect);
	DWORD helpId;
	HH_POPUP hhp;

	memset(&hhp, 0, sizeof(hhp));
	hhp.cbStruct = sizeof(hhp);
	hhp.pt = helpInfo->MousePos;
	hhp.clrForeground = hhp.clrBackground = -1;
	memset(&hhp.rcMargins, -1, sizeof(hhp.rcMargins));

	helpId = helpInfo->dwContextId;

	UCCHAR stringBuffer[65536];
	if (LoadString(getLanguageModule(), helpId, stringBuffer, COUNT_OF(stringBuffer)) > 0)
	{
		// If HtmlHelp loads the string, it can be truncated. :-(
		hhp.pszText = stringBuffer;
	}
	else
	{
		hhp.idString = 1; // No Help topic...
	}
	HtmlHelp((HWND)helpInfo->hItemHandle, NULL, HH_DISPLAY_TEXT_POPUP,
		(DWORD_PTR)&hhp);
	return TRUE;
}
