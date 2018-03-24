#ifndef __OptionUI_H__
#define __OptionUI_H__

#include <TCFoundation/TCDefines.h>
#include <TCFoundation/mystring.h>

class LDExporterSetting;
class OptionsCanvas;

/*
OptionUI

This is the base class for all the type-specific option UI classes.  Note that
many of its functions are pure virtuals.
*/
class OptionUI
{
public:
	OptionUI(OptionsCanvas *parent, LDExporterSetting &setting);
	virtual ~OptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth) = 0;
	virtual bool validate(ucstring & /*error*/) { return true; }
	virtual void commit(void) = 0;
	LDExporterSetting *getSetting(void) { return m_setting; }
	virtual void setEnabled(bool value) = 0;
	virtual void getRect(RECT *rect) = 0;
	virtual void doClick(HWND /*control*/) {}
	virtual int getLeftGroupMargin(void) const { return m_leftGroupMargin; }
	virtual int getRightGroupMargin(void) const { return m_rightGroupMargin; }
	virtual int getBottomGroupMargin(void) const { return m_bottomGroupMargin; }
	virtual bool getEnabled(void) { return true; }
	virtual void addTooltip(HWND hControl);
	virtual void reset(void);

protected:
	virtual int calcTextHeight(HDC hdc, int width, int &optimalWidth);
	virtual void updateTooltip(void);
	virtual void valueChanged(void) = 0;
	SIZE fitButtonText(CUCSTR localText);

	OptionsCanvas *m_canvas;
	HWND m_hParentWnd;
	HWND m_hTooltip;
	HWND m_hTooltipControl;
	ucstring m_tooltipText;
	bool m_shown;
	LDExporterSetting *m_setting;
	int m_leftGroupMargin;
	int m_rightGroupMargin;
	int m_bottomGroupMargin;
	int m_checkBoxWidth;
};

#endif // __OptionUI_H__
