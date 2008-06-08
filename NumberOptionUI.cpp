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

	m_hLabel = CUIWindow::createWindowExUC(0, WC_STATICUC,
		setting.getName().c_str(), SS_RIGHT | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	m_hEdit = CUIWindow::createWindowExUC(WS_EX_CLIENTEDGE, WC_EDITUC, value,
		ES_AUTOHSCROLL | WS_CHILD | WS_TABSTOP, 0, 0, 100, 100, m_hParentWnd,
		NULL, GetWindowInstance(m_hParentWnd), NULL);
	SetWindowLongPtr(m_hEdit, GWLP_USERDATA, (LONG_PTR)this);
	SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	SendMessage(m_hEdit, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	MapDialogRect(m_hParentWnd, &editRect);
	m_editWidth = editRect.right;
	m_editHeight = editRect.bottom;
	m_spacing = editRect.left;
}

int NumberOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	int textWidth = width - m_editWidth - m_spacing;
	int newWidth = 0;
	int height = calcTextHeight(hdc, textWidth, newWidth);

	newWidth += m_editWidth + m_spacing;
	if (newWidth > optimalWidth)
	{
		optimalWidth = newWidth;
	}
	if (update)
	{
		int textY = y;
		int editY = y;

		if (m_editHeight > height)
		{
			textY += (m_editHeight - height) / 2;
		}
		else
		{
			editY += (height - m_editHeight) / 2;
		}
		MoveWindow(m_hLabel, x, textY, textWidth, height, FALSE);
		MoveWindow(m_hEdit, x + width - m_editWidth, editY, m_editWidth,
			m_editHeight, FALSE);
		if (!m_shown)
		{
			ShowWindow(m_hLabel, SW_SHOW);
			ShowWindow(m_hEdit, SW_SHOW);
			m_shown = true;
		}
	}
	return std::max(height, m_editHeight);
}

void NumberOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hLabel, enabled);
	EnableWindow(m_hEdit, enabled);
}

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
