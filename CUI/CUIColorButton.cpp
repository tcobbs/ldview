#include "CUIColorButton.h"
#include "CUIDialog.h"
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

bool CUIColorButton::sm_haveColorChangedId = false;
UINT CUIColorButton::sm_colorChangedId = 0;

CUIColorButton::CUIColorButton(
	HWND hWnd,
	const char *defaultColorsUDKey,
	int r /*= 0*/,
	int g /*= 0*/,
	int b /*= 0*/):
m_oldWindowProc(NULL),
m_hTheme(NULL),
m_hBrush(NULL),
m_mouseOver(false),
m_r(r),
m_g(g),
m_b(b),
m_defaultColorsUDKey(defaultColorsUDKey)
{
	hWindow = hWnd;
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
	m_oldWindowProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC,
		(LONG_PTR)staticWindowProc);
	CUIDialog *dialog = CUIDialog::fromHandle(GetParent(hWnd));
	if (dialog != NULL)
	{
		dialog->addControl(hWnd);
	}
	if (CUIThemes::isThemeLibLoaded())
	{
		m_hTheme = CUIThemes::openThemeData(hWnd, L"Button");
	}
	updateBrush();
	m_defaultColors.resize(16);
	m_defaultColors = TCUserDefaults::longVectorForKey(
		m_defaultColorsUDKey.c_str(), m_defaultColors, false);
	SetWindowLong(hWnd, GWL_STYLE,
		GetWindowLong(hWnd, GWL_STYLE) | BS_OWNERDRAW);
}

CUIColorButton::~CUIColorButton(void)
{
}

void CUIColorButton::updateBrush(void)
{
	if (m_hBrush != NULL)
	{
		DeleteObject(m_hBrush);
	}
	m_hBrush = CreateSolidBrush(RGB(m_r, m_g, m_b));
}

void CUIColorButton::setColor(int r, int g, int b)
{
	m_r = r;
	m_g = g;
	m_b = b;
	updateBrush();
}

void CUIColorButton::getColor(int &r, int &g, int &b)
{
	r = m_r;
	g = m_g;
	b = m_b;
}

void CUIColorButton::dealloc(void)
{
	if (m_hTheme != NULL)
	{
		CUIThemes::closeThemeData(m_hTheme);
	}
	if (m_hBrush != NULL)
	{
		DeleteObject(m_hBrush);
	}
	CUIWindow::dealloc();
}

bool CUIColorButton::doMyDrawItem(
	WPARAM /*controlId*/,
	LPDRAWITEMSTRUCT drawItem,
	LRESULT &lResult)
{
	HDC diHdc = drawItem->hDC;
	RECT itemRect = drawItem->rcItem;
	RECT origItemRect = itemRect;
	bool bIsPressed = (drawItem->itemState & ODS_SELECTED) != 0;
	bool bIsFocused = (drawItem->itemState & ODS_FOCUS) != 0;
	bool bDrawFocusRect = (drawItem->itemState & ODS_NOFOCUSRECT) == 0;
	RECT colorBoxRect;

	SetBkMode(diHdc, TRANSPARENT);
	// Prepare draw... paint button background
	if (m_hTheme != NULL)
	{
		DWORD state = (bIsPressed) ? PBS_PRESSED : PBS_NORMAL;

		if (state == PBS_NORMAL)
		{
			if (bIsFocused)
			{
				state = PBS_DEFAULTED;
			}
			if (m_mouseOver)
			{
				state = PBS_HOT;
			}
		}
		CUIThemes::drawThemeBackground(m_hTheme, diHdc, BP_PUSHBUTTON, state,
			&itemRect, NULL);
	}
	else
	{
		if (bIsFocused)
		{
			HBRUSH br = CreateSolidBrush(RGB(0,0,0));  
			FrameRect(diHdc, &itemRect, br);
			InflateRect(&itemRect, -1, -1);
			DeleteObject(br);
		} // if		

		COLORREF crColor = GetSysColor(COLOR_BTNFACE);

		HBRUSH	brBackground = CreateSolidBrush(crColor);

		FillRect(diHdc, &itemRect, brBackground);

		DeleteObject(brBackground);

		// Draw pressed button
		if (bIsPressed)
		{
			HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			FrameRect(diHdc, &itemRect, brBtnShadow);
			DeleteObject(brBtnShadow);
		}

		else // ...else draw non pressed button
		{
			UINT uState = DFCS_BUTTONPUSH |
                  (m_mouseOver ? DFCS_HOT : 0) |
                  ((bIsPressed) ? DFCS_PUSHED : 0);

			DrawFrameControl(diHdc, &itemRect, DFC_BUTTON, uState);
		} // else
	}

	// Draw the focus rect
	if (bIsFocused && bDrawFocusRect)
	{
		RECT focusRect = itemRect;

		// Do NOT scale
		InflateRect(&focusRect, -3, -3);
		DrawFocusRect(diHdc, &focusRect);
	}

	// Draw the color box
	if (m_hTheme != NULL)
	{
		int margin = scalePoints(3);
		CUIThemes::getThemeBackgroundContentRect(m_hTheme, NULL,
			BP_PUSHBUTTON, PBS_HOT, &origItemRect, &colorBoxRect);
		InflateRect(&colorBoxRect, -margin, -margin);
	}
	else
	{
		int margin = scalePoints(5);
		colorBoxRect = origItemRect;
		InflateRect(&colorBoxRect, -margin, -margin);
	}
	FillRect(diHdc, &colorBoxRect, m_hBrush);
	if (m_hTheme != NULL)
	{
		CUIThemes::drawThemeEdge(m_hTheme, diHdc, BP_PUSHBUTTON, PBS_PRESSED,
			&colorBoxRect, EDGE_SUNKEN, BF_SOFT | BF_RECT, NULL);
	}
	else
	{
		DrawEdge(diHdc, &colorBoxRect, EDGE_SUNKEN, BF_BOTTOMLEFT | BF_TOPRIGHT);
	}
	lResult = TRUE;
	return true;
}

LRESULT CUIColorButton::windowProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	if (message == CUIDialog::getMessageForwardId())
	{
		CUIDialog::ControlMessage *cm = (CUIDialog::ControlMessage *)lParam;

		switch (cm->msg)
		{
		case WM_DRAWITEM:
			cm->processed = doMyDrawItem(cm->wParam,
				(LPDRAWITEMSTRUCT)cm->lParam, cm->lResult);
			break;
		case WM_COMMAND:
			cm->processed = doMyCommand(HIWORD(cm->wParam), cm->lResult);
			break;
		}
		return 0;
	}
	else
	{
		switch (message)
		{
		case WM_MOUSEMOVE:
			CUIWindow::windowProc(hWnd, message, wParam, lParam);
			break;
		case WM_DESTROY:
			doMyDestroy();
			break;
		case WM_MOUSELEAVE:
			doMyMouseLeave();
			break;
		}
	}
	return CallWindowProc(m_oldWindowProc, hWnd, message, wParam, lParam);
}

bool CUIColorButton::doMyCommand(int code, LRESULT &lResult)
{
	if (code == BN_CLICKED)
	{
		COLORREF initialColor = RGB(m_r, m_g, m_b);
		CHOOSECOLOR chooseColor;
		COLORREF customColors[16];
		size_t i;

		memset(customColors, 0, sizeof(customColors));
		for (i = 0; i < 16; i++)
		{
			customColors[i] = (COLORREF)m_defaultColors[i];
		}
		memset(&chooseColor, 0, sizeof(chooseColor));
		chooseColor.lStructSize = sizeof(chooseColor);
		chooseColor.hwndOwner = hWindow;
		chooseColor.rgbResult = initialColor;
		chooseColor.lpCustColors = customColors;
		chooseColor.Flags = CC_ANYCOLOR | CC_RGBINIT;
		if (ChooseColor(&chooseColor))
		{
			if (chooseColor.rgbResult != initialColor)
			{
				NMHDR notification;
				bool customChanged = false;

				m_r = GetRValue(chooseColor.rgbResult);
				m_g = GetGValue(chooseColor.rgbResult);
				m_b = GetBValue(chooseColor.rgbResult);
				for (i = 0; i < 16; i++)
				{
					if (m_defaultColors[i] != (long)customColors[i])
					{
						customChanged = true;
					}
					m_defaultColors[i] = (long)customColors[i];
				}
				if (customChanged)
				{
					TCUserDefaults::setLongVectorForKey(m_defaultColors,
						m_defaultColorsUDKey.c_str(), false);
				}
				updateBrush();
				InvalidateRect(hWindow, NULL, TRUE);
				notification.hwndFrom = hWindow;
				notification.idFrom = (WPARAM)GetWindowLong(hWindow, GWL_ID);
				notification.code = CCBN_COLOR_CHANGED;
				SendMessage(GetParent(hWindow), WM_NOTIFY,
					(WPARAM)notification.idFrom, (LPARAM)&notification);
			}
		}
		lResult = 0;
		return true;
	}
	return false;
}

void CUIColorButton::doMyDestroy(void)
{
	if (m_mouseOver)
	{
		trackMouseEvent(TME_CANCEL);
	}
}

void CUIColorButton::doMyMouseLeave(void)
{
	InvalidateRect(hWindow, NULL, TRUE);
	m_mouseOver = false;
}

void CUIColorButton::trackMouseEvent(DWORD dwFlags, DWORD dwHoverTime /*= 0*/)
{
	TRACKMOUSEEVENT tme;

	m_mouseOver = true;
	memset(&tme, 0, sizeof(tme));
	tme.cbSize = sizeof(tme);
	tme.dwFlags = dwFlags;
	tme.hwndTrack = hWindow;
	tme.dwHoverTime = dwHoverTime;
	_TrackMouseEvent(&tme);
}

LRESULT CUIColorButton::doMouseMove(WPARAM, int /*x*/, int /*y*/)
{
	if (!m_mouseOver)
	{
		m_mouseOver = true;
		trackMouseEvent(TME_LEAVE);
		InvalidateRect(hWindow, NULL, TRUE);
	}
	return 0;
}

// Note: static method
void CUIColorButton::registerColorChangedId(void)
{
	if (!sm_haveColorChangedId)
	{
		sm_colorChangedId = RegisterWindowMessage(_UC("CUIColorButton::ColorChanged"));
		sm_haveColorChangedId = true;
	}
}

// Note: static method
UINT CUIColorButton::getColorChangedId(void)
{
	registerColorChangedId();
	return sm_colorChangedId;
}
