#include "GroupOptionUI.h"
#include "OptionsCanvas.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

extern int controlId;

GroupOptionUI::GroupOptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
OptionUI(parent, setting),
m_hCheck(NULL),
m_hLabel(NULL)
{
	bool hasCheck = setting.getType() == LDExporterSetting::TBool;
	CUCSTR text = _UC("");
	RECT marginRect = { 5, 0, 5, 5 };

	if (!hasCheck)
	{
		text = setting.getName().c_str();
	}
	m_hBox = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		_UC(""), BS_GROUPBOX | WS_CHILD, 0, 0, 100, 100,
		m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
	SendMessage(m_hBox, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
		WM_GETFONT, 0, 0), 0);
	if (hasCheck)
	{
		m_hCheck = CUIWindow::createWindowExUC(WS_EX_TRANSPARENT, WC_BUTTONUC,
			setting.getName().c_str(),
			BS_NOTIFY | BS_AUTOCHECKBOX | BS_MULTILINE | WS_CHILD | WS_TABSTOP,
			0, 0, 100, 100, m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
		SendMessage(m_hCheck, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
			WM_GETFONT, 0, 0), 0);
		if (setting.getBoolValue())
		{
			SendMessage(m_hCheck, BM_SETCHECK, TRUE, 0);
		}
		SetWindowLongPtr(m_hCheck, GWLP_USERDATA, (LONG_PTR)this);
	}
	else
	{
		m_hLabel = CUIWindow::createWindowExUC(WS_EX_TRANSPARENT, WC_STATICUC,
			setting.getName().c_str(), SS_LEFT | WS_CHILD, 0, 0, 100, 100,
			m_hParentWnd, NULL, GetWindowInstance(m_hParentWnd), NULL);
		SendMessage(m_hLabel, WM_SETFONT, (WPARAM)SendMessage(m_hParentWnd,
			WM_GETFONT, 0, 0), 0);
	}
	MapDialogRect(m_hParentWnd, &marginRect);
	m_leftMargin = marginRect.left;
	m_rightMargin = marginRect.right;
	m_bottomMargin = marginRect.bottom;
}

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

	boxHeight = calcTextHeight(hdc, 1920, newWidth);
	height = boxHeight;
	if (m_hLabel != NULL)
	{
		labelHeight = calcTextHeight(hdc, textWidth, labelWidth);
	}
	if (m_hCheck != NULL)
	{
		textWidth -= 16;
		labelHeight = calcTextHeight(hdc, textWidth, labelWidth);
		labelWidth += 16;
	}
	if (labelWidth > optimalWidth)
	{
		optimalWidth = labelWidth;
	}
	if (labelHeight > boxHeight)
	{
		boxY = y + (labelHeight - boxHeight) / 2;
		height = labelHeight;
	}
	if (update)
	{
		HWND hLabel = m_hLabel;

		if (hLabel == NULL)
		{
			hLabel = m_hCheck;
		}
		MoveWindow(m_hBox, x, boxY, width, boxHeight, FALSE);
		MoveWindow(hLabel, x + textOffset, y, labelWidth, labelHeight,
			FALSE);
		if (!m_shown)
		{
			ShowWindow(m_hBox, SW_SHOW);
			ShowWindow(hLabel, SW_SHOW);
			m_shown = true;
		}
	}
	return height;
}

void GroupOptionUI::commit(void)
{
	m_setting->setValue(CUIWindow::checkGet(m_hCheck), true);
}

void GroupOptionUI::close(int y)
{
	RECT rect;

	GetWindowRect(m_hBox, &rect);
	CUIWindow::screenToClient(GetParent(m_hBox), &rect);
	rect.bottom = y + m_bottomMargin;
	MoveWindow(m_hBox, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top, FALSE);
}

bool GroupOptionUI::getEnabled(void)
{
	if (m_hCheck)
	{
		return CUIWindow::checkGet(m_hCheck);
	}
	else
	{
		return true;
	}
}


void GroupOptionUI::setEnabled(bool value)
{
	BOOL enabled = value ? TRUE : FALSE;

	EnableWindow(m_hBox, enabled);
	if (m_hCheck)
	{
		EnableWindow(m_hCheck, enabled);
	}
}

void GroupOptionUI::getRect(RECT *rect)
{
	HWND hControl = m_hCheck;

	if (hControl == NULL)
	{
		hControl = m_hLabel;
	}
	GetWindowRect(hControl, rect);
	CUIWindow::screenToClient(m_hParentWnd, rect);
}

void GroupOptionUI::doClick(HWND control)
{
	if (control == m_hCheck)
	{
		m_canvas->updateEnabled();
	}
}
