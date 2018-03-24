#include "OptionUI.h"
#include "OptionsCanvas.h"
#include <CUI/CUIWindow.h>
#include <CUI/CUIThemes.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

OptionUI::OptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
m_canvas(parent),
m_hParentWnd(parent->getHWindow()),
m_hTooltip(NULL),
m_shown(false),
m_setting(&setting),
m_leftGroupMargin(0),
m_rightGroupMargin(0),
m_bottomGroupMargin(0)
{
	HDC hdc = GetDC(m_hParentWnd);
	SIZE size;

	GetTextExtentPoint32A(hdc, "X", 1, &size);
	ReleaseDC(m_hParentWnd, hdc);
	// The check box size is calculated based on the font height.  The + 4 is
	// for the space between the box and the text.
	m_checkBoxWidth = size.cy + 4;
}

// Just call calcTextHeight using the setting's name, which is the most common
// text that we need the text height for.
int OptionUI::calcTextHeight(HDC hdc, int width, int &optimalWidth)
{
	return CUIWindow::calcTextHeight(hdc, m_setting->getName(), width, optimalWidth);
}

void OptionUI::addTooltip(HWND hControl)
{
	m_tooltipText = m_setting->getTooltip();
	if (m_tooltipText.size() > 0)
	{
		m_hTooltip = CUIWindow::createWindowExUC(0, TOOLTIPS_CLASSUC,
			NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /*| TTS_BALLOON*/,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hControl, NULL, GetWindowInstance(hControl), NULL);
		m_hTooltipControl = hControl;
	}
}

void OptionUI::updateTooltip(void)
{
	if (m_hTooltip != NULL)
	{
		TOOLINFOUC ti;
		RECT rect;

		memset(&ti, 0, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd = m_hParentWnd;
		ti.uId = (UINT_PTR)m_hTooltipControl;
		ti.lpszText = &m_tooltipText[0];
		// If you don't set the maximum width, multi-line messages aren't
		// displayed, so set the maximum width to a somewhat reasonable value.
		::SendMessage(m_hTooltip, TTM_SETMAXTIPWIDTH, 0, 800);
		::SendMessage(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
		// Since the text might be long, leave the tooltip visible for about 32
		// seconds.  It's treated as a short, so 32,767 is the max.
		::SendMessage(m_hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 32767);
	}
}

void OptionUI::reset(void)
{
	m_setting->reset();
	valueChanged();
}

SIZE OptionUI::fitButtonText(
	CUCSTR localText)
{
	// "Browse..." needs 40 dialog units
	RECT buttonRect = { 0, 0, 40, 14 };
	SIZE englishSize;
	SIZE localSize;
	HDC hdc;
	SIZE retValue;

	// Convert the default size of a browse button in English to dialog units.
	MapDialogRect(m_hParentWnd, &buttonRect);
	hdc = GetDC(m_hParentWnd);
	// Figure out the width of the English text, which we know the button size
	// for.
	GetTextExtentPoint32A(hdc, "Browse...", (int)strlen("Browse..."),
		&englishSize);
	// Figure out the width of the localized button text, which we don't know
	// the button size for.
	CUIWindow::getTextExtentPoint32UC(hdc, localText, (int)ucstrlen(localText),
		&localSize);
	// The button width is the known good width for "Browse...", minus the width
	// of the string "Browse..." plus the width of the localized verison of
	// "Browse...".
	retValue.cx = buttonRect.right - englishSize.cx + localSize.cx;;
	// The height is purely dependent of the dialog box units mapping.
	retValue.cy = buttonRect.bottom;
	ReleaseDC(m_hParentWnd, hdc);
	return retValue;
}
