#include "EnumOptionUI.h"
#include <TCFoundation/mystring.h>
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

EnumOptionUI::EnumOptionUI(
	OptionsCanvas *parent,
	LDExporterSetting &setting):
OptionUI(parent, setting)
{
	RECT spacingRect = { 0, 3, 0, 14 };
	const UCStringVector &options = setting.getOptions();

	// Note that all of the window locations in this constructor are just
	// placeholders.  They will never be visible at those locations and sizes.
	m_hLabel = CUIWindow::createWindowExUC(0, WC_STATICUC,
		setting.getName().c_str(), SS_LEFT | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	m_hCombo = CUIWindow::createWindowExUC(WS_EX_CLIENTEDGE, WC_COMBOBOXUC,
		_UC(""), WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST, 0, 0, 100, 100, m_hParentWnd, NULL,
		GetWindowInstance(m_hParentWnd), NULL);
	// When the combo box receives the keyboard focus, we need a way to get back
	// to here from the canvas's message handler.
	SetWindowLongPtr(m_hCombo, GWLP_USERDATA, (LONG_PTR)this);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	SendMessage(m_hCombo, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	// Convert our desired fixed layout into dialog box units.  Note that
	// m_spacing is the vertical space between the label and the combo box
	// control.
	MapDialogRect(m_hParentWnd, &spacingRect);
	m_spacing = spacingRect.top;
	m_comboHeight = spacingRect.bottom;
	for (size_t i = 0; i < options.size(); i++)
	{
		CUIWindow::comboAddString(m_hCombo, options[i].c_str());
	}
	CUIWindow::comboSetCurSel(m_hCombo, (int)setting.getSelectedOption());
	addTooltip(m_hCombo);
}

// This does all the work of calculating the location and size of the controls
// in this option UI.
int EnumOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	int newWidth = 0;
	int labelHeight = calcTextHeight(hdc, width, newWidth);
	int height = labelHeight + m_spacing + m_comboHeight;

	if (width > optimalWidth)
	{
		// Technically, we don't need to bother with this, since this particular
		// width is never used, but do it anyway for consistency, and in case it
		// does get used in the future.
		optimalWidth = width;
	}
	if (update)
	{
		int comboY = y + labelHeight + m_spacing;

		// Note that the windows get sized and placed before they are shown for
		// the first time.  Also note that this is making a really short box;
		// we'll make it taller (to hold its contents) in the close method.
		MoveWindow(m_hLabel, x, y, width, labelHeight, FALSE);
		// For some unknown reason, the height you provide for a combo box is
		// the height when the drop-down list is visible.  The height of the
		// actual box is auto-computed by the combo box based on its item height
		// (I think).  So make one with enough vertical space for 20 items.
		// Note that if there are less than 20 items, the height will
		// automatically be made shorter.
		MoveWindow(m_hCombo, x, comboY, width, m_comboHeight * 20, FALSE);
		if (!m_shown)
		{
			// Now that we've calculated the proper location of the windows,
			// it's safe to show them.
			ShowWindow(m_hLabel, SW_SHOW);
			ShowWindow(m_hCombo, SW_SHOW);
			m_shown = true;
		}
	}
	updateTooltip();
	// Return the overall height of this option UI.
	return height;
}

void EnumOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hLabel, enabled);
	EnableWindow(m_hCombo, enabled);
}

// This is called when the focus changes in order to make sure the whole useful
// control is visible.  In this case, we want the union of the label text rect
// and the combo box control rect.
void EnumOptionUI::getRect(RECT *rect)
{
	RECT labelRect;
	RECT comboRect;

	GetWindowRect(m_hLabel, &labelRect);
	GetWindowRect(m_hCombo, &comboRect);
	CUIWindow::screenToClient(m_hParentWnd, &labelRect);
	// Note that even though we set the combo box height to m_comboHeight * 20,
	// the window rect we get back when asked is the shorter size.
	CUIWindow::screenToClient(m_hParentWnd, &comboRect);
	UnionRect(rect, &labelRect, &comboRect);
}

void EnumOptionUI::commit(void)
{
	m_setting->selectOption(CUIWindow::comboGetCurSel(m_hCombo), true);
}

void EnumOptionUI::valueChanged(void)
{
	CUIWindow::comboSetCurSel(m_hCombo, (int)m_setting->getSelectedOption());
}
