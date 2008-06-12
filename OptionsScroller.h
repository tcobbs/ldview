#ifndef __OptionsScroller_H__
#define __OptionsScroller_H__

#include <CUI/CUIDialog.h>
#include <LDExporter/LDExporter.h>

class OptionsCanvas;

/*
OptionsScroller

Window that holds OptionsCanvas, which can have an arbitrary height, and
provides a vertical scroll bar when the height of the OptionsCanvas is larger
than the client rect of this window.
*/
class OptionsScroller: public CUIDialog
{
public:
	OptionsScroller(HINSTANCE hInstance);
	void create(CUIWindow *parent);
	OptionsCanvas *getCanvas(void) { return m_canvas; }
	bool scrolls(void) { return m_scrolls; }
	void scrollControlToVisible(HWND hControl);
	virtual LRESULT doMouseWheel(short keyFlags, short zDelta, int xPos,
		int yPos);
protected:
	virtual ~OptionsScroller(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual LRESULT doVScroll(int scrollCode, int position, HWND hScrollBar);

	void moveCanvas(int width, int height, int myHeight);
	void setY(int value);

	OptionsCanvas *m_canvas;
	LONG m_style;
	bool m_scrolls;
	int m_scrollBarWidth;
	int m_y;
};

#endif // __OptionsScroller_H__
