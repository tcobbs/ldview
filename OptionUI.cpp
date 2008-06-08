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
m_setting(&setting)
{
}

int OptionUI::calcTextHeight(HDC hdc, int width, int &optimalWidth)
{
	return calcTextHeight(hdc, m_setting->getName(), width, optimalWidth);
}

int OptionUI::calcTextHeight(
	HDC hdc,
	const ucstring &text,
	int width,
	int &optimalWidth)
{
	ucstring remaining = text;
	int height = 0;

	for (int i = 0; remaining.size() > 0; i++)
	{
		SIZE size;
		ucstring line = remaining;

		CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), line.size(),
			&size);
		while (size.cx > width)
		{
			size_t index = line.rfind(' ');

			if (line.size() == 0)
			{
				break;
			}
			if (index >= line.size())
			{
				// No spaces for split: chop off one character.
				index = line.size() - 1;
			}
			line = line.substr(0, index);
			CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), line.size(),
				&size);
		}
		if (line.size() > 0)
		{
			size_t j;

			remaining = remaining.substr(line.size());
			for (j = 0; isspace(remaining[j]); j++)
			{
				// Don't do anything.
			}
			if (j > 0)
			{
				remaining = remaining.substr(j);
			}
		}
		else
		{
			remaining.resize(0);
		}
		height += size.cy;
		if (size.cx > optimalWidth)
		{
			optimalWidth = size.cx;
		}
	}
	return height;
}
