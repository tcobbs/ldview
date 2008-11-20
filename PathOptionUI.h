#ifndef __PathOptionUI_H__
#define __PathOptionUI_H__

#include "StringOptionUI.h"
#include <TCFoundation/mystring.h>

/*
StringOptionUI

This class is used for a path setting.  It is identical to a string setting,
except that the edit box is made narrower in order to accomodate a browse
button on its right.
*/
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
		// Leave space for the browse button to the right of the edit control.
		return width - m_spacing - m_browseSize.cx;
	}

	HWND m_hBrowseButton;
	SIZE m_browseSize;
};

#endif // __PathOptionUI_H__
