#include "PathOptionUI.h"
#include <TCFoundation/mystring.h>
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

PathOptionUI::PathOptionUI(
	OptionsCanvas *parent,
	LDExporterSetting &setting):
StringOptionUI(parent, setting)
{
	RECT browseRect = { 0, 0, 40, 14 };
	SIZE englishSize;
	SIZE localSize;
	HDC hdc;
	CUCSTR localText = TCObject::ls(_UC("LDXBrowse..."));

	m_hBrowseButton = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		localText, BS_NOTIFY | BS_PUSHBUTTON  | WS_CHILD | WS_TABSTOP, 0, 0,
		100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	SetWindowLongPtr(m_hBrowseButton, GWLP_USERDATA, (LONG_PTR)this);
	SendMessage(m_hBrowseButton, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	MapDialogRect(m_hParentWnd, &browseRect);
	hdc = GetDC(m_hParentWnd);
	GetTextExtentPoint32A(hdc, "Browse...", strlen("Browse..."), &englishSize);
	CUIWindow::getTextExtentPoint32UC(hdc, localText, ucstrlen(localText),
		&localSize);
	m_browseWidth = browseRect.right - englishSize.cx + localSize.cx;
	ReleaseDC(m_hParentWnd, hdc);
	m_browseHeight = browseRect.bottom;
}

int PathOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	bool origShown = m_shown;
	int height = StringOptionUI::updateLayout(hdc, x, y, width, update,
		optimalWidth);

	if (update)
	{
		MoveWindow(m_hBrowseButton, x + width - m_browseWidth,
			y + height - m_browseHeight, m_browseWidth, m_browseHeight, FALSE);
		if (!origShown)
		{
			ShowWindow(m_hBrowseButton, SW_SHOW);
		}
	}
	return height;
}

void PathOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	StringOptionUI::setEnabled(value);
	EnableWindow(m_hBrowseButton, enabled);
}

void PathOptionUI::getRect(RECT *rect)
{
	StringOptionUI::getRect(rect);
}

void PathOptionUI::doClick(HWND control)
{
	if (control == m_hBrowseButton)
	{
		ucstring value;
		OPENFILENAMEUC openStruct;
		UCCHAR fileTypes[1024];
		UCCHAR fullPathName[1024] = _UC("");
		ucstring initialDir;

		CUIWindow::windowGetText(m_hEdit, value);
		if (value.size() > 0)
		{
			initialDir = directoryFromPath(value);
		}
		memset(fileTypes, 0, 2 * sizeof(UCCHAR));
		CUIWindow::addFileType(fileTypes, TCObject::ls(_UC("AllFilesTypes")),
			_UC("*.*"));
		memset(&openStruct, 0, sizeof(openStruct));
		openStruct.lStructSize = CUIWindow::getOpenFilenameSize(true);
		openStruct.hwndOwner = m_hParentWnd;
		openStruct.lpstrFilter = fileTypes;
		openStruct.nFilterIndex = 1;
		openStruct.lpstrFile = fullPathName;
		openStruct.nMaxFile = COUNT_OF(fullPathName);
		if (initialDir.size() > 0)
		{
			openStruct.lpstrInitialDir = &initialDir[0];
		}
		//openStruct.lpstrTitle = TCLocalStrings::get(_UC("SelectModelFile"));
		openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
			OFN_HIDEREADONLY;
		if (CUIWindow::getOpenFileNameUC(&openStruct))
		{
			CUIWindow::setWindowTextUC(m_hEdit, fullPathName);
		}
	}
}
