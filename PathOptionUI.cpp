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
	//RECT browseRect = { 0, 0, 40, 14 };
	//SIZE englishSize;
	//SIZE localSize;
	//HDC hdc;
	CUCSTR localText = TCObject::ls(_UC("LDXBrowse..."));

	// Note that the window location in this constructor is just a placeholder.
	// The window will never be visible at that locations and size.
	m_hBrowseButton = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		localText, BS_NOTIFY | BS_PUSHBUTTON  | WS_CHILD | WS_TABSTOP, 0, 0,
		100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	// When the browse button's command goes to the canvas, or it receives the
	// keyboard focus, we need a way to get back to here.
	SetWindowLongPtr(m_hBrowseButton, GWLP_USERDATA, (LONG_PTR)this);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hBrowseButton, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	m_browseSize = fitButtonText(localText);
	//// Convert the default size of a browse button in English to dialog units.
	//MapDialogRect(m_hParentWnd, &browseRect);
	//hdc = GetDC(m_hParentWnd);
	//// Figure out the width of the English text, which we know the button size
	//// for.
	//GetTextExtentPoint32A(hdc, "Browse...", strlen("Browse..."), &englishSize);
	//// Figure out the width of the localized button text, which we don't know
	//// the button size for.
	//CUIWindow::getTextExtentPoint32UC(hdc, localText, ucstrlen(localText),
	//	&localSize);
	//// The button width is the known good width for "Browse...", minus the width
	//// of the string "Browse..." plus the width of the localized verison of
	//// "Browse...".
	//m_browseWidth = browseRect.right - englishSize.cx + localSize.cx;
	//ReleaseDC(m_hParentWnd, hdc);
	// The height is purely dependent of the dialog box units mapping.
}

// This does all the work of calculating the location and size of the check box
// in this option UI.
int PathOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	// Store the original value of m_shown, so that later on we can show our
	// window if necessary.
	bool origShown = m_shown;
	// Our job here is relatively easy, since the browse button is the same
	// height as the edit box from our parent class.  Just have it calculate
	// its layout, then add in the browse button.
	int height = StringOptionUI::updateLayout(hdc, x, y, width, update,
		optimalWidth);

	if (update)
	{
		// Note that the window gets sized and placed before is is shown for the
		// first time.
		MoveWindow(m_hBrowseButton, x + width - m_browseSize.cx,
			y + height - m_browseSize.cy, m_browseSize.cx, m_browseSize.cy, FALSE);
		// Notice that origShown was grabbed before calling
		// StringOptionUI::updateLayout.
		if (!origShown)
		{
			// Now that we've calculated the proper location of the window, it's
			// safe to show it.
			ShowWindow(m_hBrowseButton, SW_SHOW);
		}
	}
	updateTooltip();
	return height;
}

void PathOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	StringOptionUI::setEnabled(value);
	EnableWindow(m_hBrowseButton, enabled);
}

// While we do have another rect, it's guaranteed to be completely inside the
// union of the two rects in our parent.
void PathOptionUI::getRect(RECT *rect)
{
	StringOptionUI::getRect(rect);
}

void PathOptionUI::doClick(HWND control)
{
	if (control == m_hBrowseButton)
	{
		// Browse button was clicked, so browse for a file.
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
		// In any OS before Win2K, you must use the old size of the OPENFILENAME
		// struct in order for it to work.  You gotta love the fact that they
		// made room for that size in the struct, but instead of just ignoring
		// anything beyond their known fields, they instead failed.  The true
		// below is there because we have a UC version of the OPENFILENAME
		// struct.
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
		openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
			OFN_HIDEREADONLY;
		if (CUIWindow::getOpenFileNameUC(&openStruct))
		{
			CUIWindow::setWindowTextUC(m_hEdit, fullPathName);
		}
	}
}
