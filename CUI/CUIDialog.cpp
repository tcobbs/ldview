#include "CUIDialog.h"
#include <TCFoundation/mystring.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

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
	return TRUE;
}

INT_PTR CUIDialog::dialogProc(
	HWND /*hDlg*/,
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
		if (doCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
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
		if (doNotify((int)(short)LOWORD(wParam), (LPNMHDR)lParam) == 0)
		{
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
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

LRESULT CUIDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (commandId == IDOK || commandId == IDCANCEL)
	{
		EndDialog(hWindow, (INT_PTR)commandId);
		return 0;
	}
	return CUIWindow::doCommand(notifyCode, commandId, control);
}

INT_PTR CUIDialog::doModal(UINT dialogId, HWND hWndParent /*= NULL*/)
{
	return doModal(MAKEINTRESOURCE(dialogId), hWndParent);
}

INT_PTR CUIDialog::doModal(LPCTSTR dialogName, HWND hWndParent /*= NULL*/)
{
	return ::DialogBoxParam(hInstance, dialogName, hWndParent, staticDialogProc,
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
		::SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)dialog);
	}
	else
	{
		dialog = (CUIDialog *)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
	}
	if (dialog)
	{
		return dialog->privateDialogProc(hDlg, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
	//return DefDlgProc(hDlg, message, wParam, lParam);
}

bool CUIDialog::getCheck(int buttonId)
{
	return SendDlgItemMessage(hWindow, buttonId, BM_GETCHECK, 0, 0) != 0;
}

void CUIDialog::setCheck(int buttonId, bool value)
{
	SendDlgItemMessage(hWindow, buttonId, BM_SETCHECK, value, 0);
}

void CUIDialog::setupSlider(
	int controlId,
	short min,
	short max,
	WORD frequency,
	int value)
{
	CUIWindow::setupDialogSlider(hWindow, controlId, min, max, frequency,
		value);
}

void CUIDialog::setSliderTic(int controlId, int position)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETTIC, 0, position);
}

void CUIDialog::setSliderValue(int controlId, int value)
{
	SendDlgItemMessage(hWindow, controlId, TBM_SETPOS, 1, value);
}

int CUIDialog::getSliderValue(int controlId)
{
	return SendDlgItemMessage(hWindow, controlId, TBM_GETPOS, 0, 0);
}

void CUIDialog::setupSpin(
	int controlId,
	short min,
	short max,
	int value,
	UDACCEL *accels	/*= NULL*/,
	int numAccels	/* = 0*/)
{

	SendDlgItemMessage(hWindow, controlId, UDM_SETRANGE, 0, MAKELONG((short)max,
		(short)min)); 
	setSpinValue(controlId, value);
	if (accels && numAccels > 0)
	{
		SendDlgItemMessage(hWindow, controlId, UDM_SETACCEL,
			(WPARAM)numAccels, (long)accels);
	}
}

void CUIDialog::setSpinValue(
	int controlId,
	int value)
{
	SendDlgItemMessage(hWindow, controlId, UDM_SETPOS, 0, value);
}
