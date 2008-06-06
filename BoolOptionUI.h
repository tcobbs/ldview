#ifndef __BoolOptionUI_H__
#define __BoolOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

class BoolOptionUI: public OptionUI
{
public:
	BoolOptionUI(HWND hParentWnd, LDExporterSetting &setting);
	virtual ~BoolOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update);
	virtual bool validate(ucstring &/*error*/) { return true; }
	virtual void commit(void);

protected:
	HWND m_hCheck;
};

#endif // __BoolOptionUI_H__
