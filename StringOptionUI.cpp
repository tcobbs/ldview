#include "StringOptionUI.h"
#include <TCFoundation/mystring.h>
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

StringOptionUI::StringOptionUI(
	OptionsCanvas *parent,
	LDExporterSetting &setting):
OptionUI(parent, setting)
{
	RECT spacingRect = { 0, 3, 0, 14 };

	// Note that all of the window locations in this constructor are just
	// placeholders.  They will never be visible at those locations and sizes.
	m_hLabel = CUIWindow::createWindowExUC(0, WC_STATICUC,
		setting.getName().c_str(), SS_LEFT | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	m_hEdit = CUIWindow::createWindowExUC(WS_EX_CLIENTEDGE, WC_EDITUC,
		setting.getStringValue().c_str(),
		ES_AUTOHSCROLL | WS_CHILD | WS_TABSTOP, 0, 0, 100, 100, m_hParentWnd,
		NULL, GetWindowInstance(m_hParentWnd), NULL);
	// When the edit box receives the keyboard focus, we need a way to get back
	// to here from the canvas's message handler.
	SetWindowLongPtr(m_hEdit, GWLP_USERDATA, (LONG_PTR)this);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	// Convert our desired fixed layout into dialog box units.  Note that
	// m_spacing is the vertical space between the label and the edit control.
	MapDialogRect(m_hParentWnd, &spacingRect);
	m_spacing = spacingRect.top;
	m_editHeight = spacingRect.bottom;
	addTooltip(m_hEdit);
}

// This does all the work of calculating the location and size of the controls
// in this option UI.
int StringOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	int newWidth = 0;
	int labelHeight = calcTextHeight(hdc, width, newWidth);
	int height = labelHeight + m_spacing + m_editHeight;

	if (width > optimalWidth)
	{
		// Technically, we don't need to bother with this, since this particular
		// width is never used, but do it anyway for consistency, and in case it
		// does get used in the future.
		optimalWidth = width;
	}
	if (update)
	{
		int editY = y + labelHeight + m_spacing;

		// Note that the windows get sized and placed before they are shown for
		// the first time.  Also note that this is making a really short box;
		// we'll make it taller (to hold its contents) in the close method.
		MoveWindow(m_hLabel, x, y, width, labelHeight, FALSE);
		// Note: getEditWidth is used below, because PathOptionUI, which is a
		// subclass of this, makes the edit box narrower in order to accomodate
		// a browse button.
		MoveWindow(m_hEdit, x, editY, getEditWidth(width), m_editHeight, FALSE);
		if (!m_shown)
		{
			// Now that we've calculated the proper location of the windows,
			// it's safe to show them.
			ShowWindow(m_hLabel, SW_SHOW);
			ShowWindow(m_hEdit, SW_SHOW);
			m_shown = true;
		}
	}
	updateTooltip();
	// Return the overall height of this option UI.
	return height;
}

void StringOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hLabel, enabled);
	EnableWindow(m_hEdit, enabled);
}

// This is called when the focus changes in order to make sure the whole useful
// control is visible.  In this case, we want the union of the label text rect
// and the edit control rect.
void StringOptionUI::getRect(RECT *rect)
{
	RECT labelRect;
	RECT editRect;

	GetWindowRect(m_hLabel, &labelRect);
	GetWindowRect(m_hEdit, &editRect);
	CUIWindow::screenToClient(m_hParentWnd, &labelRect);
	CUIWindow::screenToClient(m_hParentWnd, &editRect);
	UnionRect(rect, &labelRect, &editRect);
}

void StringOptionUI::commit(void)
{
	ucstring value;

	CUIWindow::windowGetText(m_hEdit, value);
	m_setting->setValue(value.c_str(), true);
}

void StringOptionUI::valueChanged(void)
{
	CUIWindow::setWindowTextUC(m_hEdit, m_setting->getStringValue().c_str());
}
