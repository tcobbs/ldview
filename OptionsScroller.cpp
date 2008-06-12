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
		// Given the new width/height, m_y is too big, so adjust it.
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
		// If we currently have a scroll bar, newWidth will contain the width
		// of our window to the left of the scroll bar.  In order to see if the
		// canvas fits without a scroll bar, we need to have it calulate its
		// size based on the full window width, so add the width of the
		// scrollbar onto width, which is used when calculating the canvas's
		// needed height.
		width += m_scrollBarWidth;
	}
	// First, figure out how tall m_canvas needs to be for the given width that
	// is available to it after the resize.  This is done purely to decide if
	// we need a scroll bar.  The actual canvas layout isn't updated.
	height = m_canvas->calcHeight(width, optimalWidth);
	if (height > newHeight)
	{
		// We need a vertical scroll bar.  Pull the width of the scrollbar off
		// of width, which will be used below as the new width of the canvas
		// window.
		width -= m_scrollBarWidth;
		scrollNeeded = true;
		// Since width changed, height may have also changed, so recalculate
		// the height based on the new width.
		height = m_canvas->calcHeight(width, optimalWidth);
	}
	else
	{
		// We don't need a vertical scroll bar.
		scrollNeeded = false;
	}
	if (scrollNeeded != m_scrolls)
	{
		// Change in scroll bar visibility.
		m_scrolls = scrollNeeded;
		ShowScrollBar(hWindow, SB_VERT, m_scrolls);
	}
	// At this point, width and height contain the proper values for the full
	// size of the canvas, so update its size.  We pass newHeight in so that the
	// scroll position can be updated if needed.
	moveCanvas(width, height, newHeight);
	if (m_scrolls)
	{
		SCROLLINFO si;

		// Update the position, page size, and range of the scroll bar.
		memset(&si, 0, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nPage = newHeight;
		si.nMin = 0;
		si.nMax = height;
		si.nPos = m_y;
		// If you do this without showing the scroll bar first, it drops the XP
		// themes temporarily.  Really strange.
		SetScrollInfo(hWindow, SB_VERT, &si, TRUE);
	}
	return 0;
}

// Scroll vertically to a new location.
void OptionsScroller::setY(int value)
{
	RECT rect;
	RECT canvasRect;

	// Note: the client rect for a window doesn't include the area taken up by
	// its scroll bars, and that's what we want.
	GetClientRect(hWindow, &rect);
	GetClientRect(m_canvas->getHWindow(), &canvasRect);
	if (canvasRect.bottom - value < rect.bottom)
	{
		// Don't allow the user to scroll off the bottom.
		value = canvasRect.bottom - rect.bottom;
	}
	if (value < 0)
	{
		// Don't allow the user to scroll off the top.
		value = 0;
	}
	if (value != m_y)
	{
		int optimalWidth = 0;
		int height;

		// Our scroll position changed, so update it, and scroll.
		m_y = value;
		height = m_canvas->calcHeight(rect.right, optimalWidth);
		// Update the canvas's vertical position.
		moveCanvas(rect.right, height, rect.bottom);
	}
	// Just to play it safe, update the scroll bar even if we don't think there
	// was a change.
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

// Make it so that the given control is fully visible in the scroll region.
void OptionsScroller::scrollControlToVisible(HWND hControl)
{
	RECT clientRect;
	RECT controlRect;
	// All controls in the canvas that might receive the keyboard focus have
	// their GWLP_USERDATA setting set to point to the OptionUI they're part of.
	OptionUI *optionUI = (OptionUI *)GetWindowLongPtr(hControl, GWLP_USERDATA);

	GetClientRect(hWindow, &clientRect);
	if (optionUI != NULL)
	{
		// If the control has a label above it, or a multi-line label that
		// extends above and below it, we want that visible also, so let the
		// OptionUI object tell us the important rectangle.
		optionUI->getRect(&controlRect);
	}
	else
	{
		// We shouldn't ever get here, but if we do, it should work fine.
		GetWindowRect(hControl, &controlRect);
		screenToClient(GetParent(hControl), &controlRect);
	}
	if (controlRect.bottom > clientRect.bottom + m_y)
	{
		// Note that we're relying on setY to contrain things to avoid
		// scrolling off the bottom;
		setY(controlRect.bottom - clientRect.bottom);
	}
	else if (controlRect.top < clientRect.top + m_y)
	{
		// Note that we're relying on setY to contrain things to avoid
		// scrolling off the top.
		setY(controlRect.top - clientRect.top);
	}
	// Redraw the canvas and all its child windows.
	RedrawWindow(m_canvas->getHWindow(), NULL, NULL,
		RDW_INVALIDATE | RDW_ALLCHILDREN);
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
