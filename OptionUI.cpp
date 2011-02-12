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
	return calcTextHeight(hdc, m_setting->getName(), width, optimalWidth);
}

// This calculates the height necessary in order to display a string of text in
// a check button of a given width.  If the button has a theme, the theme system
// is used for the calculation.  Otherwise, it's done manually.
int OptionUI::calcCheckHeight(
	HWND hCheck,
	HDC hdc,
	int checkBoxWidth,
	int width,
	int &optimalWidth)
{
	HTHEME hTheme = CUIThemes::getWindowTheme(hCheck);
	ucstring text;
	int height;
	int newWidth = 0;

	CUIWindow::windowGetText(hCheck, text);
	if (hTheme == NULL)
	{
		// Do the calculation manually using the width available in the check
		// button for the text.
		height = calcTextHeight(hdc, text, width - checkBoxWidth, newWidth);
		// The actual total width of the button is the width of the text plus
		// the width of the check box.
		newWidth += checkBoxWidth;
	}
	else
	{
		// Give it plenty of vertical space for the text.
		RECT boundingRect = { 0, 0, width, 1024 };
		RECT textRect;
		SIZE boxSize;
		int state = CBS_CHECKEDNORMAL;
#ifdef TC_NO_UNICODE
		std::wstring wText;

		stringtowstring(wText, text);
#else // TC_NO_UNICODE
		std::wstring &wText = text;
#endif // TC_NO_UNICODE
		// Get the size of the check box.
		CUIThemes::getThemePartSize(hTheme, hdc, BP_CHECKBOX, state, NULL,
			TS_TRUE, &boxSize);
		// The + 3 is for the space between the check box and the text.  (This
		// was determined empirically, but works at both the standard font DPI
		// of 96 and the "large fonts" DPI of 120.
		boundingRect.left += boxSize.cx + 3;
		// The following basically does exactly what we are trying to do in this
		// method.  It calculates the width and height needed for the text
		// string to fit into the given bounding box, wrapping as needed.
		CUIThemes::getThemeTextExtent(hTheme, hdc, BP_CHECKBOX, state,
			wText.c_str(), -1, DT_LEFT | DT_WORDBREAK | DT_CALCRECT,
			&boundingRect, &textRect);
		height = textRect.bottom - textRect.top;
		newWidth = textRect.right;
	}
	if (newWidth > optimalWidth)
	{
		optimalWidth = newWidth;
	}
	return height;
}

// This calculates the height necessary in order to display a string of text in
// a given width.  It also calculate the actual width needed, and if that is
// greater than optimalWidth, it updates optimalWidth to match.  If the text has
// to be wrapped to multiple lines, this does that at spaces.  If a word in the
// text is too long to fit on one line, it wraps at whatever location is
// necessary.
int OptionUI::calcTextHeight(
	HDC hdc,
	const ucstring &text,
	int width,
	int &optimalWidth)
{
	// remaining keeps track of all the text that hasn't yet been accounted for.
	ucstring remaining = text;
	int height = 0;

	for (int i = 0; remaining.size() > 0; i++)
	{
		SIZE size;
		ucstring line = remaining;

		// Start out with the size needed for the whole string.
		CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), (int)line.size(),
			&size);
		while (size.cx > width)
		{
			// If we get here, the string didn't fit in the given width.  Pull
			// a word off the right and try again.
			size_t index = line.rfind(' ');

			if (line.size() == 0)
			{
				// Nothing fit at all, so just bail to avoid an infinite loop.
				break;
			}
			if (index >= line.size())
			{
				// No spaces for split: chop off one character.
				index = line.size() - 1;
			}
			line = line.substr(0, index);
			// At this point, line has had one word removed from it (if it
			// contains any spaces, otherwise one character removed).  See what
			// size is neccessary for that.
			CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(),
				(int)line.size(), &size);
		}
		// When we get here, whatever is in line fits in the given width.
		if (line.size() > 0)
		{
			size_t j;

			// Pull the contents of line out of remaining.
			remaining = remaining.substr(line.size());
			// Find the first non-space character in remaining.
			for (j = 0; isspace(remaining[j]); j++)
			{
				// Don't do anything.
			}
			if (j > 0)
			{
				// If there were spaces on the beginning of remaining, trim them
				// off.
				remaining = remaining.substr(j);
			}
		}
		else
		{
			// Nothing in remaining fit in the given width, so bail.
			remaining.resize(0);
		}
		// Increment height by the height of this line of text.
		height += size.cy;
		if (size.cx > optimalWidth)
		{
			// This line of text is wider than optimalWidth, so update
			// optimalWidth to match this width.
			optimalWidth = size.cx;
		}
	}
	// Return the calculated height for the text string.
	return height;
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

		::GetClientRect(m_hTooltipControl, &rect);
		memset(&ti, 0, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_SUBCLASS;
		ti.hwnd = m_hTooltipControl;
		ti.hinst = GetWindowInstance(m_hTooltipControl);
		ti.lpszText = &m_tooltipText[0];
		ti.rect = rect;
		// If you don't set the maximum width, multi-line messages aren't
		// displayed, so set the maximum width to a somewhat reasonable value.
		::SendMessage(m_hTooltip, TTM_SETMAXTIPWIDTH, 0, 800);
		CUIWindow::sendMessageUC(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
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
