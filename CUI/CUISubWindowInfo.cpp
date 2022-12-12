#include "CUISubWindowInfo.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

CUISubWindowInfo::CUISubWindowInfo(void)
	: hWindow(NULL)
	, resizeMask(0)
{
	originalRect.left = 0;
	originalRect.right = 0;
	originalRect.top = 0;
	originalRect.bottom = 0;
}

CUISubWindowInfo::~CUISubWindowInfo(void)
{
}

void CUISubWindowInfo::dealloc(void)
{
	TCObject::dealloc();
}

void CUISubWindowInfo::setup(HWND value, DWORD mask)
{
	RECT windowRect;
	HWND hParentWindow = GetParent(value);
	POINT clientPoint;

	hWindow = value;
	resizeMask = mask;
	GetClientRect(hParentWindow, &windowRect);
	clientPoint.x = windowRect.left;
	clientPoint.y = windowRect.top;
	ClientToScreen(hParentWindow, &clientPoint);
	GetWindowRect(hWindow, &originalRect);
	originalRect.left -= clientPoint.x;
	originalRect.right -= clientPoint.x;
	originalRect.top -= clientPoint.y;
	originalRect.bottom -= clientPoint.y;
}
