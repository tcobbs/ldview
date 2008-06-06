#include "OptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

int OptionUI::calcTextHeight(HDC hdc, int width)
{
	return calcTextHeight(hdc, m_setting->getName(), width);
}

int OptionUI::calcTextHeight(HDC hdc, const ucstring &text, int width)
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

			if (index >= line.size())
			{
				// No spaces for split: chop off one character.
				index = line.size() - 1;
			}
			line = line.substr(0, index);
			CUIWindow::getTextExtentPoint32UC(hdc, line.c_str(), line.size(),
				&size);
		}
		remaining = remaining.substr(line.size());
		height += size.cy;
	}
	return height;
}
