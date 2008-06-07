#include "BoolOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

BoolOptionUI::BoolOptionUI(HWND hParentWnd, LDExporterSetting &setting):
OptionUI(hParentWnd, setting)
{
	m_hCheck = CUIWindow::createWindowExUC(0, WC_BUTTONUC,
		setting.getName().c_str(),
		BS_AUTOCHECKBOX | BS_MULTILINE | WS_CHILD | WS_TABSTOP, 0, 0, 100, 100,
		hParentWnd, NULL, GetWindowInstance(hParentWnd), NULL);
	SendMessage(m_hCheck, WM_SETFONT, (WPARAM)SendMessage(hParentWnd,
		WM_GETFONT, 0, 0), 0);
	if (setting.getBoolValue())
	{
		SendMessage(m_hCheck, BM_SETCHECK, TRUE, 0);
	}
}

int BoolOptionUI::updateLayout(
	HDC hdc,
	int x,
	int y,
	int width,
	bool update,
	int &optimalWidth)
{
	int textWidth = width - 16;
	int newWidth = 0;
	int height = calcTextHeight(hdc, textWidth, newWidth);

	newWidth += 32;
	if (newWidth > optimalWidth)
	{
		optimalWidth = newWidth;
	}
	if (update)
	{
		MoveWindow(m_hCheck, x, y, width, height, FALSE);
		if (!m_shown)
		{
			ShowWindow(m_hCheck, SW_SHOW);
			m_shown = true;
		}
	}
	return height;
}

void BoolOptionUI::commit(void)
{
	m_setting->setValue(CUIWindow::checkGet(m_hCheck), true);
}
