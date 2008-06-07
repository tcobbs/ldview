#ifndef __OptionUI_H__
#define __OptionUI_H__

#include <TCFoundation/TCDefines.h>
#include <TCFoundation/mystring.h>

class LDExporterSetting;

class OptionUI
{
public:
	OptionUI(HWND hParentWnd, LDExporterSetting &setting):
		m_hParentWnd(hParentWnd), m_shown(false), m_setting(&setting) {}
	virtual ~OptionUI(void) {}

	virtual int updateLayout(HDC hdc, int x, int y, int width, bool update,
		int &optimalWidth) = 0;
	virtual bool validate(ucstring &error) = 0;
	virtual void commit(void) = 0;
	LDExporterSetting *getSetting(void) { return m_setting; }

protected:
	virtual int calcTextHeight(HDC hdc, int width, int &optimalWidth);

	static int calcTextHeight(HDC hdc, const ucstring &text, int width,
		int &optimalWidth);

	HWND m_hParentWnd;
	bool m_shown;
	LDExporterSetting *m_setting;
};

#endif // __OptionUI_H__
