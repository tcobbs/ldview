#include "NumberOptionUI.h"
#include <TCFoundation/mystring.h>
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

int controlId = 1000;

NumberOptionUI::NumberOptionUI(
	OptionsCanvas *parent,
	LDExporterSetting &setting,
	CUCSTR value):
OptionUI(parent, setting)
{
	RECT editRect = { 3, 0, 50, 14 };

	// Note that all of the window locations in this constructor are just
	// placeholders.  They will never be visible at those locations and sizes.
	m_hLabel = CUIWindow::createWindowExUC(0, WC_STATICUC,
		setting.getName().c_str(), SS_RIGHT | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	m_hEdit = CUIWindow::createWindowExUC(WS_EX_CLIENTEDGE, WC_EDITUC, value,
		ES_AUTOHSCROLL | WS_CHILD | WS_TABSTOP, 0, 0, 100, 100, m_hParentWnd,
		NULL, GetWindowInstance(m_hParentWnd), NULL);
	// When the edit box receives the keyboard focus, we need a way to get back
	// to here from the canvas's message handler.
	SetWindowLongPtr(m_hEdit, GWLP_USERDATA, (LONG_PTR)this);
	// The default font is the Windows 3.1 bold system font.  Gotta love
	// "backwards compatibility".
	HFONT parentFont = (HFONT)SendMessage(m_hParentWnd, WM_GETFONT, 0, 0);
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM)parentFont, 0);
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)parentFont, 0);
	// Convert our desired fixed layout into dialog box units.  Note that
	// m_spacing is the space between the label and the edit control.
	MapDialogRect(m_hParentWnd, &editRect);
	m_editWidth = editRect.right;
	m_editHeight = editRect.bottom;
	m_spacing = editRect.left;
	addTooltip(m_hEdit);
}

// This does all the work of calculating the location and size of the controls
// in this option UI.
int NumberOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	// The edit box goes to the right of the label, so subtract it's width and
	// m_spacing from the input width.
	int textWidth = width - m_editWidth - m_spacing;
	int newWidth = 0;
	int height = calcTextHeight(hdc, textWidth, newWidth);

	newWidth += m_editWidth + m_spacing;
	if (newWidth > optimalWidth)
	{
		// As the canvas updates the layouts of all the number option UIs it
		// has, optimalWidth gradually grows until it matches the width of the
		// widest number option's name.  That width is then used for all
		// number option UIs in the whole canvas, so that all of their edit
		// controls line up.
		optimalWidth = newWidth;
	}
	if (update)
	{
		int textY = y;
		int editY = y;

		if (m_editHeight > height)
		{
			// The edit control is taller than the label text; move the label
			// text down so that its center lines up with the edit control's
			// center.
			textY += (m_editHeight - height) / 2;
		}
		else
		{
			// The label text is taller than the edit control; move the edit
			// control down so that its center lines up with the label text's
			// center.
			editY += (height - m_editHeight) / 2;
		}
		// Note that the windows get sized and placed before they are shown for
		// the first time.  Also note that this is making a really short box;
		// we'll make it taller (to hold its contents) in the close method.
		MoveWindow(m_hLabel, x, textY, textWidth, height, FALSE);
		MoveWindow(m_hEdit, x + width - m_editWidth, editY, m_editWidth,
			m_editHeight, FALSE);
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
	// Our overall height is either the height of the edit control, or the
	// height of the label text, whichever is greater.
	return std::max(height, m_editHeight);
}

void NumberOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hLabel, enabled);
	EnableWindow(m_hEdit, enabled);
}

// This is called when the focus changes in order to make sure the whole useful
// control is visible.  In this case, we want the union of the label text rect
// and the edit control rect.
void NumberOptionUI::getRect(RECT *rect)
{
	RECT labelRect;
	RECT editRect;

	GetWindowRect(m_hLabel, &labelRect);
	GetWindowRect(m_hEdit, &editRect);
	CUIWindow::screenToClient(m_hParentWnd, &labelRect);
	CUIWindow::screenToClient(m_hParentWnd, &editRect);
	UnionRect(rect, &labelRect, &editRect);
}

void NumberOptionUI::valueChanged(void)
{
	CUIWindow::setWindowTextUC(m_hEdit, m_setting->getStringValue().c_str());
}
