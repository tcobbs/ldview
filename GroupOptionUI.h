#ifndef __GroupOptionUI_H__
#define __GroupOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

class GroupOptionUI: public OptionUI
{
public:
	GroupOptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~GroupOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual bool validate(ucstring &/*error*/) { return true; }
	virtual void commit(void);
	virtual int getLeftMargin(void) const { return m_leftMargin; }
	virtual int getRightMargin(void) const { return m_rightMargin; }
	virtual int getBottomMargin(void) const { return m_bottomMargin; }
	virtual void close(int y);
	virtual bool getEnabled(void);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual void doClick(HWND control);

protected:
	HWND m_hBox;
	HWND m_hCheck;
	HWND m_hLabel;
	int m_leftMargin;
	int m_rightMargin;
	int m_bottomMargin;
};

#endif // __GroupOptionUI_H__
