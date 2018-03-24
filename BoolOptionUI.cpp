#include "BoolOptionUI.h"
#include "OptionsCanvas.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

BoolOptionUI::BoolOptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
OptionUI(parent, setting)
{
	RECT mapRect = { 10, 0, 0, 0 };

	// Note that the window location in this constructor is just a placeholder.
	// The window will never be visible at that locations and size.
	m_hCheck = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		setting.getName().c_str(),
		BS_NOTIFY | BS_AUTOCHECKBOX | BS_MULTILINE | WS_CHILD | WS_TABSTOP, 0,
		0, 100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	SendMessage(m_hCheck, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	if (setting.getBoolValue())
	{
		CUIWindow::buttonSetChecked(m_hCheck, true);
	}
	// When the check box's command goes to the canvas, or it receives the
	// keyboard focus, we need a way to get back to here.
	SetWindowLongPtr(m_hCheck, GWLP_USERDATA, (LONG_PTR)this);
	// Convert our desired left margin into dialog box units.  This margin is
	// used if this setting has group members under it.  The given margin is
	// applied to those controls, not this one.  The check box width is used
	// when calculating the size of a check box when themes are disabled or
	// don't exist.
	MapDialogRect(m_hParentWnd, &mapRect);
	m_leftGroupMargin = mapRect.left;
	addTooltip(m_hCheck);
}

// This does all the work of calculating the location and size of the check box
// in this option UI.
int BoolOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	//int textWidth = width;
	//int newWidth = 0;
	int height = CUIWindow::calcCheckHeight(m_hCheck, hdc, m_checkBoxWidth, width,
		optimalWidth);
	//int height = calcTextHeight(hdc, textWidth, newWidth);

	// I have absolutely no idea why the following needs to be 32 instead of 16,
	// but if it is 16, the calculated widths come out wrong.
	//newWidth += m_checkBoxWidth;
	//if (newWidth > optimalWidth)
	//{
	//	// Technically, we don't need to bother with this, since this particular
	//	// width is never used, but do it anyway for consistency, and in case it
	//	// does get used in the future.
	//	optimalWidth = newWidth;
	//}
	if (update)
	{
		// Note that the window gets sized and placed before is is shown for the
		// first time.
		MoveWindow(m_hCheck, x, y, width, height, FALSE);
		if (!m_shown)
		{
			// Now that we've calculated the proper location of the window, it's
			// safe to show it.
			ShowWindow(m_hCheck, SW_SHOW);
			m_shown = true;
		}
	}
	updateTooltip();
	// Return the overall height of this option UI for the given width.
	return height;
}

void BoolOptionUI::commit(void)
{
	m_setting->setValue(CUIWindow::checkGet(m_hCheck), true);
}

void BoolOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hCheck, enabled);
}

// This is called when the focus changes in order to make sure the whole useful
// control is visible.  Since we only have one control, just use its rect.
void BoolOptionUI::getRect(RECT *rect)
{
	GetWindowRect(m_hCheck, rect);
	CUIWindow::screenToClient(m_hParentWnd, rect);
}

// If this boolean has group contents, this determines if they are enabled.
bool BoolOptionUI::getEnabled(void)
{
	if (m_setting->getGroupSize() > 0)
	{
		return CUIWindow::checkGet(m_hCheck);
	}
	else
	{
		// If there are no group contents, then there's nothing to be disabled.
		return true;
	}
}

void BoolOptionUI::doClick(HWND control)
{
	// Note that the check box itself tracks its state, so all we care about is
	// if this click is going to change the enabled state of the boolean's
	// group.
	if (m_setting->getGroupSize() > 0 && control == m_hCheck)
	{
		// The following will cause the contents of the group to be enabled or
		// disabled, depending on the new check state.
		m_canvas->updateEnabled();
	}
}

void BoolOptionUI::valueChanged(void)
{
	CUIWindow::checkSet(m_hCheck, m_setting->getBoolValue());
}
