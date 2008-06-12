#include "OptionUI.h"
#include "OptionsCanvas.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

OptionUI::OptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
m_canvas(parent),
m_hParentWnd(parent->getHWindow()),
m_shown(false),
m_setting(&setting),
m_leftGroupMargin(0),
m_rightGroupMargin(0),
m_bottomGroupMargin(0)
{
}

// Just call calcTextHeight using the setting's name, which is the most common
// text that we need the text height for.
int OptionUI::calcTextHeight(HDC hdc, int width, int &optimalWidth)
{
	return calcTextHeight(hdc, m_setting->getName(), width, optimalWidth);
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
		CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), line.size(),
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
			CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), line.size(),
				&size);
		}
		// When we get hear, whatever is in line fits in the given width.
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
