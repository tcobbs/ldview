#include "SSPreview.h"
#include "AppResources.h"

SSPreview::SSPreview(HWND hParentWindow, HINSTANCE hInstance)
		  :CUIWindow(hParentWindow, hInstance, 0, 0, 0, 0),
		   hBitmap(NULL),
		   hBrush(NULL)
{
	setTitle("LDView SS Preview");
}

SSPreview::~SSPreview(void)
{
}

void SSPreview::dealloc(void)
{
	DeleteObject(hBitmap);
	DeleteObject(hBrush);
}

BOOL SSPreview::run(void)
{
	RECT rect;

	if (GetClientRect(hParentWindow, &rect))
	{
		width = rect.right;
		height = rect.bottom;

		initWindow();
		showWindow(SW_SHOW);
		flushLoop();
		eventLoop();
		return TRUE;
	}
	return FALSE;
}

void SSPreview::eventLoop(void)
{
    MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
	}
}

void SSPreview::flushLoop(void)
{
    MSG msg;
//	DWORD originalTickCount = GetTickCount();

	PostMessage(hWindow, WM_USER, 0, 0);
    msg.message = WM_NULL;
    while (msg.message != WM_QUIT && msg.message != WM_DESTROY &&
		msg.message != WM_USER)
    {
		if (!GetMessage(&msg, NULL, 0, 0))
		{
			return;
		}
        TranslateMessage(&msg);
        DispatchMessage(&msg);
		if (GetParent(hWindow))
		{
			WINDOWPLACEMENT wp;

			wp.length = sizeof(WINDOWPLACEMENT);
			if (GetWindowPlacement(hParentWindow, &wp))
			{
				if (wp.showCmd != SW_SHOWNORMAL)
				{
					return;
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
    }
}

HBRUSH SSPreview::getBackgroundBrush(void)
{
	if (!hBackgroundBrush)
	{
 		hBackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	}
 	return hBackgroundBrush;
}

void SSPreview::doPaint(void)
{
	RECT rect = {x, y, width, height};

	if (!hBitmap)
	{
//		hBitmap = LoadBitmap(getLanguageModule(),
//			MAKEINTRESOURCE(IDB_SS_PREVIEW));
		hBitmap = (HBITMAP)LoadImage(getLanguageModule(),
			MAKEINTRESOURCE(IDB_SS_PREVIEW), IMAGE_BITMAP, width, height,
			LR_DEFAULTCOLOR);
		hBrush = CreatePatternBrush(hBitmap);
	}
	FillRect(hdc, &rect, hBrush);
}

LRESULT SSPreview::doDestroy(void)
{
	PostQuitMessage(0);
	return 0;
}
