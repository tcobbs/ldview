#ifndef __EnumOptionUI_H__
#define __EnumOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

/*
EnumOptionUI

This class is used for a string setting.  It displays the name of the setting on
one or more lines, followed by an edit control that fills the following line.
*/
class EnumOptionUI: public OptionUI
{
public:
	EnumOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~EnumOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual void commit(void);

protected:
	virtual int getEditWidth(int width) { return width; }
	virtual void valueChanged(void);

	HWND m_hLabel;
	HWND m_hCombo;
	int m_spacing;
	int m_comboHeight;
};

#endif // __EnumOptionUI_H__
