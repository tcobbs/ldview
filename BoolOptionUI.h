#ifndef __BoolOptionUI_H__
#define __BoolOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

/*
BoolOptionUI

This class is used for a boolean setting.  It displays a check box.
*/
class BoolOptionUI: public OptionUI
{
public:
	BoolOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~BoolOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual void commit(void);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual bool getEnabled(void);
	virtual void doClick(HWND control);

protected:
	virtual void valueChanged(void);

	HWND m_hCheck;
};

#endif // __BoolOptionUI_H__
