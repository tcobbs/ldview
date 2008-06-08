#include "OptionsScroller.h"
#include "OptionsCanvas.h"
#include "Resource.h"
#include "BoolOptionUI.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

OptionsScroller::OptionsScroller(HINSTANCE hInstance):
CUIDialog(hInstance),
m_canvas(NULL),
m_style(0),
m_scrolls(false),
m_scrollBarWidth(GetSystemMetrics(SM_CXVSCROLL)),
m_y(0)
{
}

OptionsScroller::~OptionsScroller(void)
{
}

void OptionsScroller::dealloc(void)
{
	TCObject::release(m_canvas);
	CUIDialog::dealloc();
}

void OptionsScroller::create(CUIWindow *parent)
{
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OPTIONS_SCROLLER),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

BOOL OptionsScroller::doInitDialog(HWND /*hKbControl*/)
{
	m_style = GetWindowLong(hWindow, GWL_STYLE);
	m_canvas = new OptionsCanvas(hInstance);
	m_canvas->create(this);
	ShowWindow(m_canvas->getHWindow(), SW_SHOW);
	return TRUE;
}

void OptionsScroller::moveCanvas(int width, int height, int myHeight)
{
	if (m_y + myHeight > height && m_y > 0)
	{
		m_y = height - myHeight;
		if (m_y < 0)
		{
			m_y = 0;
		}
	}
	MoveWindow(m_canvas->getHWindow(), 0, -m_y, width, height, TRUE);
}

LRESULT OptionsScroller::doSize(
	WPARAM /*sizeType*/,
	int newWidth,
	int newHeight)
{
	int width = newWidth;
	int height;
	bool scrollNeeded;
	int optimalWidth = 0;

	if (m_scrolls)
	{
		width += m_scrollBarWidth;
	}
	height = m_canvas->calcHeight(width, optimalWidth);
	if (height > newHeight)
	{
		width -= m_scrollBarWidth;
		scrollNeeded = true;
	}
	else
	{
		scrollNeeded = false;
	}
	if (scrollNeeded != m_scrolls)
	{
		m_scrolls = scrollNeeded;
		ShowScrollBar(hWindow, SB_VERT, m_scrolls);
	}
	moveCanvas(width, height, newHeight);
	if (m_scrolls)
	{
		SCROLLINFO si;

		memset(&si, 0, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nPage = newHeight;
		si.nMin = 0;
		si.nMax = height;
		si.nPos = m_y;
		SetScrollInfo(hWindow, SB_VERT, &si, TRUE);
	}
	return 0;
}

void OptionsScroller::setY(int value)
{
	RECT rect;
	RECT canvasRect;

	GetClientRect(hWindow, &rect);
	GetClientRect(m_canvas->getHWindow(), &canvasRect);
	if (canvasRect.bottom - value < rect.bottom)
	{
		value = canvasRect.bottom - rect.bottom;
	}
	if (value < 0)
	{
		value = 0;
	}
	if (value != m_y)
	{
		int optimalWidth = 0;
		int height;

		m_y = value;
		height = m_canvas->calcHeight(rect.right, optimalWidth);
		moveCanvas(rect.right, height, rect.bottom);
	}
	SetScrollPos(hWindow, SB_VERT, m_y, TRUE);
}

LRESULT OptionsScroller::doVScroll(
	int scrollCode,
	int position,
	HWND hScrollBar)
{
	if (hScrollBar == NULL)
	{
		RECT rect;
		int newY = m_y;

		GetClientRect(hWindow, &rect);
		switch (scrollCode)
		{
		case SB_THUMBTRACK:
			newY = position;
			break;
		case SB_PAGEUP:
			newY -= rect.bottom;
			break;
		case SB_PAGEDOWN:
			newY += rect.bottom;
			break;
		case SB_LINEUP:
			newY--;
			break;
		case SB_LINEDOWN:
			newY++;
			break;
		}
		setY(newY);
		return 0;
	}
	return 1;
}

void OptionsScroller::scrollControlToVisible(HWND hControl)
{
	RECT clientRect;
	RECT controlRect;
	OptionUI *optionUI = (OptionUI *)GetWindowLongPtr(hControl, GWLP_USERDATA);

	GetClientRect(hWindow, &clientRect);
	if (optionUI != NULL)
	{
		optionUI->getRect(&controlRect);
	}
	else
	{
		GetWindowRect(hControl, &controlRect);
		screenToClient(GetParent(hControl), &controlRect);
	}
	if (controlRect.bottom > clientRect.bottom + m_y)
	{
		doVScroll(SB_THUMBTRACK, controlRect.bottom - clientRect.bottom, NULL);
	}
	else if (controlRect.top < clientRect.top + m_y)
	{
		doVScroll(SB_THUMBTRACK, controlRect.top - clientRect.top, NULL);
	}
}

LRESULT OptionsScroller::doMouseWheel(
	short /*keyFlags*/,
	short zDelta,
	int /*xPos*/,
	int /*yPos*/)
{
	setY(m_y - zDelta);
	return 0;
}
