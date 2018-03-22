#include "CUIWindowResizer.h"
#include "CUISubWindowInfo.h"
#include <commctrl.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

const DWORD CUIFloatLeft = 0x01;
const DWORD CUIFloatRight = 0x02;
const DWORD CUIFloatTop = 0x04;
const DWORD CUIFloatBottom = 0x08;
const DWORD CUISizeHorizontal = 0x10;
const DWORD CUISizeVertical = 0x20;

CUIWindowResizer::CUIWindowResizer(void)
	:hWindow(NULL),
	originalWidth(0),
	originalHeight(0),
	subWindowInfos(new CUISubWindowInfoArray)
{
}

CUIWindowResizer::~CUIWindowResizer(void)
{
}

void CUIWindowResizer::dealloc(void)
{
	if (subWindowInfos)
	{
		subWindowInfos->release();
	}
	TCObject::dealloc();
}

void CUIWindowResizer::setHWindow(HWND value)
{
	RECT rect;

	hWindow = value;
	GetClientRect(hWindow, &rect);
	originalWidth = rect.right - rect.left;
	originalHeight = rect.bottom - rect.top;
	subWindowInfos->removeAll();
}

void CUIWindowResizer::resize(int newWidth, int newHeight)
{
	int widthDelta = newWidth - originalWidth;
	int heightDelta = newHeight - originalHeight;
	int i;
	int count = subWindowInfos->getCount();

	for (i = 0; i < count; i++)
	{
		resizeSubWindow((*subWindowInfos)[i], widthDelta, heightDelta);
	}
	RedrawWindow(hWindow, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void CUIWindowResizer::resizeSubWindow(CUISubWindowInfo *subWindowInfo,
									   int widthDelta, int heightDelta)
{
	int widthSprings = 0;
	int heightSprings = 0;
	DWORD resizeMask = subWindowInfo->getResizeMask();
	HWND hSubWindow = subWindowInfo->getHWindow();
	RECT rect = subWindowInfo->getOriginalRect();

	if (resizeMask & CUIFloatLeft)
	{
		widthSprings++;
	}
	if (resizeMask & CUIFloatRight)
	{
		widthSprings++;
	}
	if (resizeMask & CUISizeHorizontal)
	{
		widthSprings++;
	}
	if (resizeMask & CUIFloatTop)
	{
		heightSprings++;
	}
	if (resizeMask & CUIFloatBottom)
	{
		heightSprings++;
	}
	if (resizeMask & CUISizeVertical)
	{
		heightSprings++;
	}
	if (widthSprings || heightSprings)
	{
		int x, y, width, height;

		x = rect.left;
		y = rect.top;
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		if (widthSprings)
		{
			if (resizeMask & CUIFloatLeft)
			{
				x = (int)(x + widthDelta / widthSprings);
			}
			if (resizeMask & CUISizeHorizontal)
			{
				width = (int)(width + widthDelta / widthSprings);
			}
		}
		if (heightSprings)
		{
			if (resizeMask & CUIFloatTop)
			{
				y = (int)(y + heightDelta / heightSprings);
			}
			if (resizeMask & CUISizeVertical)
			{
				height = (int)(height + heightDelta / heightSprings);
			}
		}
		MoveWindow(hSubWindow, x, y, width, height, FALSE);
//		RedrawWindow(hSubWindow, NULL, NULL,
//			RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW);
	}
}

void CUIWindowResizer::addSubWindow(HWND hSubWindow, DWORD resizeMask)
{
	if (hSubWindow)
	{
		CUISubWindowInfo *info = new CUISubWindowInfo;

		info->setup(hSubWindow, resizeMask);
		subWindowInfos->addObject(info);
		info->release();
	}
}

void CUIWindowResizer::addSubWindow(int controlID, DWORD resizeMask)
{
	addSubWindow(GetDlgItem(hWindow, controlID), resizeMask);
}

void CUIWindowResizer::removeSubWindow(HWND hSubWindow)
{
	if (hSubWindow)
	{
		for (int i = 0; i < subWindowInfos->getCount(); i++)
		{
			CUISubWindowInfo *info = (*subWindowInfos)[i];

			if (info->getHWindow() == hSubWindow)
			{
				subWindowInfos->removeItemAtIndex(i);
				break;
			}
		}
	}
}

void CUIWindowResizer::removeSubWindow(int controlID)
{
	removeSubWindow(GetDlgItem(hWindow, controlID));
}

void CUIWindowResizer::addResizeGrip(void)
{
	RECT clientRect;
	HWND hGrip;

	GetClientRect(hWindow, &clientRect);
	hGrip = createResizeGrip(hWindow, clientRect.left, clientRect.top,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
	addSubWindow(hGrip, CUIFloatLeft | CUIFloatTop);
}

// Note: static method.
HWND CUIWindowResizer::createResizeGrip(
	HWND hParent,
	int x,
	int y,
	int width,
	int height)
{
	return CreateWindow(WC_SCROLLBAR, _UC(""),
		SBS_SIZEGRIP | SBS_SIZEBOXBOTTOMRIGHTALIGN | WS_CHILD | WS_VISIBLE |
		WS_CLIPSIBLINGS, x, y, width, height, hParent, NULL,
		(HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE), 0);
}
