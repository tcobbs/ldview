#ifndef __SSPREVIEW_H__
#define __SSPREVIEW_H__

#include <CUI/CUIWindow.h>

class SSPreview: public CUIWindow
{
public:
	SSPreview(HWND hParentWindow, HINSTANCE hInstance);
	virtual BOOL run(void);
protected:
	virtual ~SSPreview(void);
	virtual void dealloc(void);
	virtual void eventLoop(void);
	virtual void flushLoop(void);
	virtual HBRUSH getBackgroundBrush(void);
	virtual void doPaint(void);
	virtual LRESULT doDestroy(void);

	HBITMAP hBitmap;
	HDC hBmpDc;
};

#endif __SSPREVIEW_H__
