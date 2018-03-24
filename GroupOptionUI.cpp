#include "GroupOptionUI.h"
#include "OptionsCanvas.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

GroupOptionUI::GroupOptionUI(
	OptionsCanvas *parent,
	LDExporterSetting &setting,
	int spacing):
OptionUI(parent, setting),
m_hCheck(NULL),
m_hLabel(NULL),
m_spacing(spacing)
{
	bool hasCheck = setting.getType() == LDExporterSetting::TBool;
	RECT marginRect = { 5, 0, 5, 5 };

	if (!hasCheck)
	{
		// Put a space before the label so it's not right against the box's
		// line.
		m_label = _UC(" ");
	}
	m_label += setting.getName();
	// Put a space after the label (or check box text) so it's not right against
	// the box's line.
	m_label += _UC(" ");
	// Don't put the label in the box at all.  We'll use either a static label
	// or a check box.  Granted, that means that the color will be wrong, but I
	// strongly suspect that nobody will notice.  The reason not to put the
	// label on the box is two-fold.  First and foremost, box labels can't wrap,
	// and we need it to be capable of wrapping.  Secondly, if we use the box's
	// native label, it will come out in a different color with XP themese, and
	// that will draw attention to the fact that it's not in a different color
	// on the check boxes.
	// Note that all of the window locations in this constructor are just
	// placeholders.  They will never be visible at those locations and sizes.
	m_hBox = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		_UC(""), BS_GROUPBOX | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hBox, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	if (hasCheck)
	{
		// Use a check box as the box's label.
		m_hCheck = CUIWindow::createWindowExUC(WS_EX_TRANSPARENT, WC_BUTTONUC,
			m_label.c_str(),
			BS_NOTIFY | BS_AUTOCHECKBOX | BS_MULTILINE | WS_CHILD | WS_TABSTOP,
			0, 0, 100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
		SendMessage(m_hCheck, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
			WM_GETFONT, 0, 0), 0);
		if (setting.getBoolValue())
		{
			CUIWindow::buttonSetChecked(m_hCheck, true);
		}
		// When the check box's command goes to the canvas, or it receives the
		// keyboard focus, we need a way to get back to here.
		SetWindowLongPtr(m_hCheck, GWLP_USERDATA, (LONG_PTR)this);
	}
	else
	{
		// Use static text as the label.
		m_hLabel = CUIWindow::createWindowExUC(WS_EX_TRANSPARENT, WC_STATICUC,
			m_label.c_str(), SS_LEFT | WS_CHILD, 0, 0, 100, 100,
			m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
		SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
			WM_GETFONT, 0, 0), 0);
	}
	// Convert our desired margins into dialog box units.
	MapDialogRect(m_hParentWnd, &marginRect);
	m_leftGroupMargin = marginRect.left;
	m_rightGroupMargin = marginRect.right;
	m_bottomGroupMargin = marginRect.bottom;
	m_hResetButton = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		TCObject::ls(_UC("LDXResetGroup")),
		BS_NOTIFY | BS_PUSHBUTTON  | WS_CHILD | WS_TABSTOP, 0, 0,
		100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	// When the reset button's command goes to the canvas, or it receives the
	// keyboard focus, we need a way to get back to here.
	SetWindowLongPtr(m_hResetButton, GWLP_USERDATA, (LONG_PTR)this);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hResetButton, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	m_resetSize = fitButtonText(TCObject::ls(_UC("LDXResetGroup")));
}

// This does all the work of calculating the location and size of the controls
// in this option UI.
int GroupOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	int boxHeight;
	int labelWidth = 0;
	int labelHeight = 0;
	int textOffset = 8;
	int textWidth = width - textOffset * 2;
	int newWidth = 0;
	int boxY = y;
	int height;

	// boxHeight below the height of one line of label text.  We need this in
	// order to center the top line of the box with the actual label.
	boxHeight = CUIWindow::calcTextHeight(hdc, m_label, 1920, newWidth);
	height = boxHeight;
	if (m_hLabel != NULL)
	{
		// Label height will be the height of one line of text times how ever
		// many lines are needed to fit in the given width.
		labelHeight = CUIWindow::calcTextHeight(hdc, m_label, textWidth, labelWidth);
	}
	if (m_hCheck != NULL)
	{
		labelHeight = CUIWindow::calcCheckHeight(m_hCheck, hdc, m_checkBoxWidth, textWidth,
			labelWidth);
		//// The check is 16 pixels wide (need to check if this needs to be
		//// another number that gets converted into dialog units).
		//textWidth -= 16;
		//// The check box is acting as the label, so just use the same height
		//// variable.  The two are mutually exclusive.
		//labelHeight = calcTextHeight(hdc, m_label, textWidth, labelWidth);
		//labelWidth += 16;
	}
	if (labelWidth > optimalWidth)
	{
		// Technically, we don't need to bother with this, since this particular
		// width is never used, but do it anyway for consistency, and in case it
		// does get used in the future.
		optimalWidth = labelWidth;
	}
	if (labelHeight > boxHeight)
	{
		// The label is more than one line tall, so move the top of the box down
		// so that the line at the top of the box stays in the center of the
		// label.
		boxY = y + (labelHeight - boxHeight) / 2;
		height = labelHeight;
	}
	if (update)
	{
		HWND hLabel = m_hLabel;

		if (hLabel == NULL)
		{
			// We'll either have a label or a check, and they both use the same
			// calculated size.
			hLabel = m_hCheck;
		}
		// Note that the windows get sized and placed before they are shown for
		// the first time.  Also note that this is making a really short box;
		// we'll make it taller (to hold its contents) in the close method.
		MoveWindow(m_hBox, x, boxY, width, boxHeight, FALSE);
		MoveWindow(hLabel, x + textOffset, y, labelWidth, labelHeight,
			FALSE);
		if (!m_shown)
		{
			// Now that we've calculated the proper location of the windows,
			// it's safe to show them.
			ShowWindow(m_hBox, SW_SHOW);
			ShowWindow(hLabel, SW_SHOW);
			ShowWindow(m_hResetButton, SW_SHOW);
			m_shown = true;
		}
		else
		{
			RedrawWindow(m_hResetButton, NULL, NULL,
				RDW_ERASE | RDW_INVALIDATE);
		}
	}
	// Return the overall height of this option UI (not including the group box
	// contents, which each have their own calculated heights, and the bottom
	// line, which gets taken care of in the close method).
	return height;
}

void GroupOptionUI::commit(void)
{
	if (m_setting->getType() == LDExporterSetting::TBool)
	{
		m_setting->setValue(CUIWindow::checkGet(m_hCheck), true);
	}
}

// Resize the box vertically so that y represents the bottom of its contents.
void GroupOptionUI::close(int y)
{
	RECT rect;

	GetWindowRect(m_hBox, &rect);
	CUIWindow::screenToClient(GetParent(m_hBox), &rect);
	MoveWindow(m_hResetButton, rect.right - m_resetSize.cx - m_rightGroupMargin,
		y + m_spacing, m_resetSize.cx, m_resetSize.cy, FALSE);
	// m_bottomGroupMargin gives space for the actual bottom line.  When we
	// enter this function, y is the bottom of the last control in the box.
	rect.bottom = y + m_bottomGroupMargin + m_resetSize.cy + m_spacing;
	MoveWindow(m_hBox, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top, FALSE);
}

// This determines if the group's contents should be enabled.
bool GroupOptionUI::getEnabled(void)
{
	if (m_hCheck)
	{
		return CUIWindow::checkGet(m_hCheck);
	}
	else
	{
		// If ther's no boolean, the group is always enabled.
		return true;
	}
}

// Since group UIs are never used for nested groups, we should in theory never
// have to disable.  But implement it correctly so that if that changes in the
// future, it will work.
void GroupOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hBox, enabled);
	if (m_hCheck)
	{
		EnableWindow(m_hCheck, enabled);
	}
	else
	{
		EnableWindow(m_hLabel, enabled);
	}
}

// This is called when the focus changes in order to make sure the whole useful
// control is visible.
void GroupOptionUI::getRect(RECT *rect)
{
	HWND hControl = m_hCheck;

	if (hControl == NULL)
	{
		// This should never happen, since the label will never get the focus.
		hControl = m_hLabel;
	}
	GetWindowRect(hControl, rect);
	CUIWindow::screenToClient(m_hParentWnd, rect);
}

BOOL CALLBACK GroupOptionUI::staticEnumReset(HWND hChild, LPARAM lParam)
{
	OptionUI *optionUI = (OptionUI *)GetWindowLongPtr(hChild, GWLP_USERDATA);

	if (optionUI != NULL)
	{
		const RECT &rect = *(RECT *)lParam;
		RECT childRect;

		GetWindowRect(hChild, &childRect);
		CUIWindow::screenToClient(GetParent(hChild), &childRect);
		if (childRect.top > rect.top && childRect.bottom < rect.bottom)
		{
			optionUI->reset();
		}
	}
	return TRUE;
}

void GroupOptionUI::doClick(HWND control)
{
	if (control == m_hCheck)
	{
		// The following will cause the contents of the group to be enabled or
		// disabled, depending on the new check state.
		m_canvas->updateEnabled();
	}
	else if (control == m_hResetButton)
	{
		RECT rect;

		GetWindowRect(m_hBox, &rect);
		CUIWindow::screenToClient(m_hParentWnd, &rect);
		EnumChildWindows(m_hParentWnd, staticEnumReset, (LPARAM)&rect);
		m_canvas->updateEnabled();
	}
}

void GroupOptionUI::valueChanged(void)
{
	if (m_hCheck != NULL)
	{
		CUIWindow::checkSet(m_hCheck, m_setting->getBoolValue());
	}
}
