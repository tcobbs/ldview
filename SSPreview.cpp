#include "SSPreview.h"
#include "Resource.h"
#include <TCFoundation/TCImage.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

SSPreview::SSPreview(HWND hParentWindow, HINSTANCE hInstance)
		  :CUIWindow(hParentWindow, hInstance, 0, 0, 0, 0),
		   hBitmap(NULL),
		   hBmpDc(NULL)
{
	setTitle(_UC("LDView SS Preview"));
}

SSPreview::~SSPreview(void)
{
}

void SSPreview::dealloc(void)
{
	if (hBitmap)
	{
		DeleteObject(hBitmap);
	}
	if (hBmpDc)
	{
		DeleteDC(hBmpDc);
	}
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
	if (!hBitmap)
	{
		HRSRC hPngResource = FindResource(getLanguageModule(),
			MAKEINTRESOURCE(IDR_SS_PREVIEW), RT_PNGDATA_1X);

		if (hPngResource)
		{
			HGLOBAL hPng = LoadResource(getLanguageModule(), hPngResource);

			if (hPng)
			{
				TCByte *data = (TCByte *)LockResource(hPng);

				if (data)
				{
					DWORD length = SizeofResource(getLanguageModule(),
						hPngResource);

					if (length)
					{
						TCImage *image = new TCImage;
						BITMAPINFO bmInfo;
						TCByte *bmData;
						TCByte *imageData;
						int imageWidth;
						int imageHeight;
						int rowSize;
						int row;
						int col;

						image->setFlipped(true);
						image->setLineAlignment(4);
						image->loadData(data, length);
						SetLastError(0);
						bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);
						bmInfo.bmiHeader.biWidth = image->getWidth();
						bmInfo.bmiHeader.biHeight = image->getHeight();
						bmInfo.bmiHeader.biPlanes = 1;
						bmInfo.bmiHeader.biBitCount = 24;
						bmInfo.bmiHeader.biCompression = BI_RGB;
						bmInfo.bmiHeader.biSizeImage = 0;
						bmInfo.bmiHeader.biXPelsPerMeter = 1;
						bmInfo.bmiHeader.biYPelsPerMeter = 1;
						bmInfo.bmiHeader.biClrUsed = 0;
						bmInfo.bmiHeader.biClrImportant = 0;
						hBitmap = CreateDIBSection(hdc, &bmInfo, DIB_RGB_COLORS,
							(void**)&bmData, NULL, 0);
						imageData = image->getImageData();
						imageWidth = image->getWidth();
						imageHeight = image->getHeight();
						rowSize = image->getRowSize();
						for (row = 0; row < imageHeight; row++)
						{
							for (col = 0; col < imageWidth; col++)
							{
								int offset = row * rowSize + col * 3;

								bmData[offset] = imageData[offset + 2];
								bmData[offset + 1] = imageData[offset + 1];
								bmData[offset + 2] = imageData[offset];
							}
						}
						image->release();
						hBmpDc = CreateCompatibleDC(hdc);
						SelectObject(hBmpDc, hBitmap);
					}
				}
			}
		}
	}
	BitBlt(hdc, 0, 0, width, height, hBmpDc, 0, 0, SRCCOPY);
}

LRESULT SSPreview::doDestroy(void)
{
	PostQuitMessage(0);
	return 0;
}
