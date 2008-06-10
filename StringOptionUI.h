#ifndef __StringOptionUI_H__
#define __StringOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

class StringOptionUI: public OptionUI
{
public:
	StringOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~StringOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual void commit(void);

protected:
	virtual int getEditWidth(int width) { return width; }

	HWND m_hLabel;
	HWND m_hEdit;
	int m_spacing;
	int m_editHeight;
};

#endif // __StringOptionUI_H__
