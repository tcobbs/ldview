#ifndef __GroupOptionUI_H__
#define __GroupOptionUI_H__

#include "OptionUI.h"
#include <TCFoundation/mystring.h>

/*
GroupOptionUI

This class is used for a grouping of settings.  It displays a group box.  It
will display a check box in place of the label at the top of the group box if
the given setting is a boolean setting.  If it does this, unchecking the check
disables all the contents of the group.
*/
class GroupOptionUI: public OptionUI
{
public:
	GroupOptionUI(OptionsCanvas *parent, LDExporterSetting &setting,
		int spacing);
	virtual ~GroupOptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth);
	virtual bool validate(ucstring &/*error*/) { return true; }
	virtual void commit(void);
	virtual void close(int y);
	virtual bool getEnabled(void);
	virtual void setEnabled(bool value);
	virtual void getRect(RECT *rect);
	virtual void doClick(HWND control);
	virtual int getBottomGroupMargin(void) const
	{
		return m_bottomGroupMargin + m_resetSize.cy + m_spacing;
	}

protected:
	virtual void valueChanged(void);
	static BOOL CALLBACK staticEnumReset(HWND hChild, LPARAM lParam);

	HWND m_hBox;
	HWND m_hCheck;
	HWND m_hLabel;
	HWND m_hResetButton;
	SIZE m_resetSize;
	ucstring m_label;
	int m_spacing;
};

#endif // __GroupOptionUI_H__
