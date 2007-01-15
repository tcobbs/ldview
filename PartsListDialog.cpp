#include "PartsListDialog.h"
#include <CUI/CUIWindow.h>
#include <LDLib/LDHtmlInventory.h>
#include "AppResources.h"

PartsListDialog::PartsListDialog(CUIWindow *parentWindow,
								 LDHtmlInventory *htmlInventory) :
	m_parentWindow(parentWindow),
	m_htmlInventory(htmlInventory),
	m_hDlg(NULL)
{
}

PartsListDialog::~PartsListDialog(void)
{
}

void PartsListDialog::dealloc(void)
{
	if (m_hDlg)
	{
		DestroyWindow(m_hDlg);
		m_hDlg = NULL;
	}
	TCObject::dealloc();
}

void PartsListDialog::createDialog(void)
{
	if (!m_hDlg)
	{
		m_hDlg = m_parentWindow->createDialog(IDD_PARTS_LIST, TRUE,
			staticDialogProc, (LPARAM)this);
	}
}

int PartsListDialog::runModal(void)
{
	createDialog();
	m_modalReturn = IDCANCEL;
	m_parentWindow->runDialogModal(m_hDlg);
	return m_modalReturn;
}

INT_PTR CALLBACK PartsListDialog::staticDialogProc(HWND hDlg, UINT message,
												   WPARAM wParam, LPARAM lParam)
{
	PartsListDialog* dialog;

	if (message == WM_INITDIALOG)
	{
		dialog = (PartsListDialog*)lParam;
		SetWindowLong(hDlg, DWL_USER, lParam);
	}
	else
	{
		dialog = (PartsListDialog*)GetWindowLong(hDlg, DWL_USER);
	}
	if (dialog)
	{
		return dialog->dialogProc(hDlg, message, wParam, lParam);
	}
	else
	{
		return FALSE;
	}
}

INT_PTR PartsListDialog::dialogProc(HWND hDlg, UINT message, WPARAM wParam,
									LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return doInitDialog(hDlg, (HWND)wParam, lParam);
			break;
		case WM_CLOSE:
			m_parentWindow->doDialogClose(hDlg);
			break;
		case WM_COMMAND:
			return doCommand((int)(short)LOWORD(wParam),
				(int)(short)HIWORD(wParam), (HWND)lParam);
			break;
/*
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
*/
		default:
			return FALSE;
			break;
	}
	return FALSE;
}

INT_PTR PartsListDialog::doInitDialog(HWND hDlg, HWND /*hFocus*/,
									  LPARAM /*lParam*/)
{
	m_hDlg = hDlg;
	setCheck(IDC_SHOW_MODEL, m_htmlInventory->getShowModelFlag());
	setCheck(IDC_EXTERNAL_CSS, m_htmlInventory->getExternalCssFlag());
	setCheck(IDC_SHOW_PART_IMAGES, m_htmlInventory->getPartImagesFlag());
	return TRUE;
}

void PartsListDialog::setCheck(int controlId, bool value)
{
	SendDlgItemMessage(m_hDlg, controlId, BM_SETCHECK, value, 0);
}

bool PartsListDialog::getCheck(int controlId)
{
	return SendDlgItemMessage(m_hDlg, controlId, BM_GETCHECK, 0, 0) != 0;
}

void PartsListDialog::saveSettings(void)
{
	m_htmlInventory->setShowModelFlag(getCheck(IDC_SHOW_MODEL));
	m_htmlInventory->setExternalCssFlag(getCheck(IDC_EXTERNAL_CSS));
	m_htmlInventory->setPartImagesFlag(getCheck(IDC_SHOW_PART_IMAGES));
}

INT_PTR PartsListDialog::doClick(int controlId, HWND /*hControl*/)
{
	switch (controlId)
	{
	case IDOK:
		m_modalReturn = IDOK;
		saveSettings();
		m_parentWindow->doDialogClose(m_hDlg);
		break;
	case IDCANCEL:
		m_parentWindow->doDialogClose(m_hDlg);
		break;
	}
	return 0;
}

INT_PTR PartsListDialog::doCommand(int controlId, int notifyCode, HWND hControl)
{
	if (notifyCode == BN_CLICKED)
	{
		return doClick(controlId, hControl);
	}
	return FALSE;
}

