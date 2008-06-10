#ifndef __PathOptionUI_H__
#define __PathOptionUI_H__

#include "StringOptionUI.h"
#include <TCFoundation/mystring.h>

class PathOptionUI: public StringOptionUI
{
public:
	PathOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~PathOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual void doClick(HWND control);

protected:
	virtual int getEditWidth(int width)
	{
		return width - m_spacing - m_browseWidth;
	}

	HWND m_hBrowseButton;
	int m_browseWidth;
	int m_browseHeight;
};

#endif // __PathOptionUI_H__
