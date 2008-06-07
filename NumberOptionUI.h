#ifndef __NumberOptionUI_H__
#define __NumberOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

class NumberOptionUI: public OptionUI
{
public:
	NumberOptionUI(HWND hParentWnd, LDExporterSetting &setting, CUCSTR value);
	virtual ~NumberOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
protected:
	HWND m_hLabel;
	HWND m_hEdit;
	int m_editWidth;
	int m_editHeight;
	int m_spacing;
};

#endif // __NumberOptionUI_H__
