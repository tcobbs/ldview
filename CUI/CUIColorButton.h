#ifndef __CUICOLORBUTTON_H__
#define __CUICOLORBUTTON_H__

#include <CUI/CUIWindow.h>
#include <CUI/CUIThemes.h>
#include <commctrl.h>
#include <TCFoundation/mystring.h>

#define CCBN_COLOR_CHANGED 0xFFFF0000

typedef std::vector<long> LongVector;

class CUIExport CUIColorButton: public CUIWindow
{
public:
	CUIColorButton(HWND hWnd, const char *defaultColorsUDKey, int r = 0,
		int g = 0, int b = 0);
	void setColor(int r, int g, int b);
	void getColor(int &r, int &g, int &b);
	static UINT getColorChangedId(void);
protected:
	virtual ~CUIColorButton(void);
	virtual void dealloc(void);
	virtual LRESULT windowProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	void updateBrush(void);

	virtual LRESULT doMouseMove(WPARAM, int x, int y);
	void trackMouseEvent(DWORD dwFlags, DWORD dwHoverTime = 0);
	void doMyDestroy(void);
	void doMyMouseLeave(void);
	bool doMyDrawItem(WPARAM controlId, LPDRAWITEMSTRUCT drawItem,
		LRESULT &lResult);
	bool doMyCommand(int code, LRESULT &lResult);

	static void registerColorChangedId(void);

	WNDPROC m_oldWindowProc;
	HTHEME m_hTheme;
	HBRUSH m_hBrush;
	bool m_mouseOver;
	int m_r;
	int m_g;
	int m_b;
	std::string m_defaultColorsUDKey;
	LongVector m_defaultColors;

	static bool sm_haveColorChangedId;
	static UINT sm_colorChangedId;
};

#endif // __CUICOLORBUTTON_H__
