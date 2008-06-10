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

	m_hLabel = CUIWindow::createWindowExUC(0, WC_STATICUC,
		setting.getName().c_str(), SS_LEFT | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	m_hEdit = CUIWindow::createWindowExUC(WS_EX_CLIENTEDGE, WC_EDITUC,
		setting.getStringValue().c_str(),
		ES_AUTOHSCROLL | WS_CHILD | WS_TABSTOP, 0, 0, 100, 100, m_hParentWnd,
		NULL, GetWindowInstance(m_hParentWnd), NULL);
	SetWindowLongPtr(m_hEdit, GWLP_USERDATA, (LONG_PTR)this);
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	MapDialogRect(m_hParentWnd, &spacingRect);
	m_spacing = spacingRect.top;
	m_editHeight = spacingRect.bottom;
}

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
		optimalWidth = width;
	}
	if (update)
	{
		int editY = y + labelHeight + m_spacing;

		MoveWindow(m_hLabel, x, y, width, labelHeight, FALSE);
		MoveWindow(m_hEdit, x, editY, getEditWidth(width), m_editHeight, FALSE);
		if (!m_shown)
		{
			ShowWindow(m_hLabel, SW_SHOW);
			ShowWindow(m_hEdit, SW_SHOW);
			m_shown = true;
		}
	}
	return height;
}

void StringOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hLabel, enabled);
	EnableWindow(m_hEdit, enabled);
}

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
