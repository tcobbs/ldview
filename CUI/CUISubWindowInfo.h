#ifndef __CUISUBWINDOWINFO_H__
#define __CUISUBWINDOWINFO_H__

#include <TCFoundation/TCObject.h>
#include <windows.h>

class CUISubWindowInfo: public TCObject
{
public:
	CUISubWindowInfo(void);
	HWND getHWindow(void) { return hWindow; }
	void setup(HWND value, DWORD mask);
	RECT getOriginalRect(void) { return originalRect; }
	DWORD getResizeMask(void) { return resizeMask; }
protected:
	virtual ~CUISubWindowInfo(void);
	virtual void dealloc(void);

	HWND hWindow;
	RECT originalRect;
	DWORD resizeMask;
};


#endif // __CUISUBWINDOWINFO_H__
