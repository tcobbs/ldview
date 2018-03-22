#include "CUIThemes.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

bool CUIThemes::sm_initTried = false;
bool CUIThemes::sm_themeLibLoaded = false;
HMODULE CUIThemes::sm_hModThemes = NULL;
PFNOPENTHEMEDATA CUIThemes::sm_openThemeData = NULL;
PFNCLOSETHEMEDATA CUIThemes::sm_closeThemeData = NULL;
PFNDRAWTHEMEBACKGROUND CUIThemes::sm_drawThemeBackground = NULL;
PFNDRAWTHEMETEXT CUIThemes::sm_drawThemeText = NULL;
PFNGETTHEMEBACKGROUNDCONTENTRECT CUIThemes::sm_getThemeBackgroundContentRect =
	NULL;
PFNDRAWTHEMEEDGE CUIThemes::sm_drawThemeEdge = NULL;
PFNSETWINDOWTHEME CUIThemes::sm_setWindowTheme = NULL;
PFNGETWINDOWTHEME CUIThemes::sm_getWindowTheme = NULL;
PFNGETTHEMECOLOR CUIThemes::sm_getThemeColor = NULL;
PFNSETTHEMEAPPPROPERTIED CUIThemes::sm_setThemeAppProperties = NULL;
PFNGETTHEMEAPPPROPERTIED CUIThemes::sm_getThemeAppProperties = NULL;
PFNGETTHEMERECT CUIThemes::sm_getThemeRect = NULL;
PFNGETTHEMEPOSITION CUIThemes::sm_getThemePosition = NULL;
PFNGETTHEMEMARGINS CUIThemes::sm_getThemeMargins = NULL;
PFNGETTHEMETEXTEXTENT CUIThemes::sm_getThemeTextExtent = NULL;
PFNDDRAWTHEMEPARENTBACKGROUND CUIThemes::sm_drawThemeParentBackground = NULL;
PFNGETTHEMEPARTSIZE CUIThemes::sm_getThemePartSize = NULL;
PFNENABLETHEMEDIALOGTEXTURE CUIThemes::sm_enableThemeDialogTexture = NULL;
PFNGETTHEMESYSCOLOR CUIThemes::sm_getThemeSysColor = NULL;
PFNISTHEMEACTIVE CUIThemes::sm_isThemeActive = NULL;

CUIThemes::CUIThemesCleanup::~CUIThemesCleanup(void)
{
	CUIThemes::deinit();
}

CUIThemes::CUIThemes(void)
{
}

CUIThemes::~CUIThemes(void)
{
}

void CUIThemes::init(void)
{
	if (!sm_initTried)
	{
		sm_initTried = true;
		sm_hModThemes = LoadLibrary(_UC("UXTHEME.DLL"));
		if (sm_hModThemes)
		{
			sm_openThemeData =
				(PFNOPENTHEMEDATA)GetProcAddress(sm_hModThemes,
				"OpenThemeData");
			sm_closeThemeData =
				(PFNCLOSETHEMEDATA)GetProcAddress(sm_hModThemes,
				"CloseThemeData");
			sm_drawThemeBackground =
				(PFNDRAWTHEMEBACKGROUND)GetProcAddress(sm_hModThemes,
				"DrawThemeBackground");
			sm_drawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(sm_hModThemes,
				"DrawThemeText");
			sm_getThemeBackgroundContentRect =
				(PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(sm_hModThemes,
				"GetThemeBackgroundContentRect");
			sm_drawThemeEdge = (PFNDRAWTHEMEEDGE)GetProcAddress(sm_hModThemes,
				"DrawThemeEdge");
			sm_setWindowTheme = (PFNSETWINDOWTHEME)GetProcAddress(sm_hModThemes,
				"SetWindowTheme");
			sm_getWindowTheme = (PFNGETWINDOWTHEME)GetProcAddress(sm_hModThemes,
				"GetWindowTheme");
			sm_getThemeColor = (PFNGETTHEMECOLOR)GetProcAddress(sm_hModThemes,
				"GetThemeColor");
			sm_setThemeAppProperties =
				(PFNSETTHEMEAPPPROPERTIED)GetProcAddress(sm_hModThemes,
				"SetThemeAppProperties");
			sm_getThemeAppProperties =
				(PFNGETTHEMEAPPPROPERTIED)GetProcAddress(sm_hModThemes,
				"GetThemeAppProperties");
			sm_getThemeRect = (PFNGETTHEMERECT)GetProcAddress(sm_hModThemes,
				"GetThemeRect");
			sm_getThemePosition =
				(PFNGETTHEMEPOSITION)GetProcAddress(sm_hModThemes,
				"GetThemePosition");
			sm_getThemeMargins =
				(PFNGETTHEMEMARGINS)GetProcAddress(sm_hModThemes,
				"GetThemeMargins");
			sm_getThemeTextExtent =
				(PFNGETTHEMETEXTEXTENT)GetProcAddress(sm_hModThemes,
				"GetThemeTextExtent");
			sm_drawThemeParentBackground =
				(PFNDDRAWTHEMEPARENTBACKGROUND)GetProcAddress(sm_hModThemes,
				"DrawThemeParentBackground");
			sm_getThemePartSize =
				(PFNGETTHEMEPARTSIZE)GetProcAddress(sm_hModThemes,
				"GetThemePartSize");
			sm_enableThemeDialogTexture =
				(PFNENABLETHEMEDIALOGTEXTURE)GetProcAddress(sm_hModThemes,
				"EnableThemeDialogTexture");
			sm_getThemeSysColor =
				(PFNGETTHEMESYSCOLOR)GetProcAddress(sm_hModThemes,
				"GetThemeSysColor");
			sm_isThemeActive =
				(PFNISTHEMEACTIVE)GetProcAddress(sm_hModThemes,
				"IsThemeActive");


			if (sm_openThemeData && sm_closeThemeData && sm_drawThemeBackground
				&& sm_drawThemeText && sm_getThemeBackgroundContentRect &&
				sm_drawThemeEdge && sm_setWindowTheme && sm_getWindowTheme &&
				sm_getThemeColor && sm_setThemeAppProperties &&
				sm_getThemeRect && sm_getThemePosition && sm_getThemeMargins &&
				sm_getThemeTextExtent && sm_drawThemeParentBackground &&
				sm_getThemePartSize && sm_enableThemeDialogTexture &&
				sm_getThemeSysColor && sm_isThemeActive &&
				sm_getThemeAppProperties)
			{
				sm_themeLibLoaded = true;			
			}
			else
			{
				FreeLibrary(sm_hModThemes);
				sm_hModThemes = NULL;
			}
		}
	}
}

void CUIThemes::deinit(void)
{
	sm_initTried = false;
	if (sm_themeLibLoaded)
	{
		FreeLibrary(sm_hModThemes);
		sm_hModThemes = NULL;
		sm_themeLibLoaded = false;
	}
}

HTHEME CUIThemes::openThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if (sm_themeLibLoaded)
	{
		return sm_openThemeData(hwnd, pszClassList);
	}
	else
	{
		return NULL;
	}
}

HRESULT CUIThemes::closeThemeData(HTHEME hTheme)
{
	if (sm_themeLibLoaded)
	{
		return sm_closeThemeData(hTheme);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::drawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId,
	int iStateId, const RECT *pRect,  const RECT *pClipRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_drawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect,
			pClipRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::drawThemeText(HTHEME hTheme, HDC hdc, int iPartId,
	int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
	DWORD dwTextFlags2, const RECT *pRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_drawThemeText(hTheme, hdc, iPartId, iStateId, pszText,
			iCharCount, dwTextFlags, dwTextFlags2, pRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::getThemeBackgroundContentRect(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId,  const RECT *pBoundingRect,
	RECT *pContentRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeBackgroundContentRect(hTheme, hdc, iPartId, iStateId,
			pBoundingRect, pContentRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::drawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, const RECT *pDestRect, UINT uEdge, UINT uFlags,
		RECT *pContentRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_drawThemeEdge(hTheme, hdc, iPartId, iStateId,
			pDestRect, uEdge, uFlags, pContentRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::setWindowTheme(HWND hWnd, LPCWSTR pwszSubAppName,
								  LPCWSTR pwszSubIdList)
{
	if (sm_themeLibLoaded)
	{
		return sm_setWindowTheme(hWnd, pwszSubAppName, pwszSubIdList);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HTHEME CUIThemes::getWindowTheme(HWND hWnd)
{
	if (sm_themeLibLoaded)
	{
		return sm_getWindowTheme(hWnd);
	}
	else
	{
		return NULL;
	}
}

HRESULT CUIThemes::getThemeColor(HTHEME hTheme, int iPartId, int iStateId,
								 int iPropId, COLORREF *pColor)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeColor(hTheme, iPartId, iStateId, iPropId, pColor);
	}
	else
	{
		return E_NOTIMPL;
	}
}

void CUIThemes::setThemeAppProperties(DWORD dwFlags)
{
	if (sm_themeLibLoaded)
	{
		sm_setThemeAppProperties(dwFlags);
	}
}

DWORD CUIThemes::getThemeAppProperties(void)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeAppProperties();
	}
	else
	{
		return 0;
	}
}

HRESULT CUIThemes::getThemeRect(HTHEME hTheme, int iPartId, int iStateId,
								int iPropId, RECT *pRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeRect(hTheme, iPartId, iStateId, iPropId, pRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::getThemePosition(
	HTHEME hTheme,
	int iPartId,
	int iStateId,
	int iPropId,
	POINT *pPoint)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemePosition(hTheme, iPartId, iStateId, iPropId, pPoint);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::getThemeMargins(
	HTHEME hTheme,
	HDC hdc,
	int iPartId,
	int iStateId,
	int iPropId,
	RECT *pRect,
	MARGINS *pMargins)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeMargins(hTheme, hdc, iPartId, iStateId, iPropId,
			pRect, pMargins);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::getThemeTextExtent(HTHEME hTheme, HDC hdc, int iPartId,
									  int iStateId, LPCWSTR pszText,
									  int iCharCount, DWORD dwTextFlags,
									  const RECT *pBoundingRect,
									  RECT *pExtentRect)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeTextExtent(hTheme, hdc, iPartId, iStateId, pszText,
			iCharCount, dwTextFlags, pBoundingRect, pExtentRect);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::drawThemeParentBackground(HWND hwnd, HDC hdc, RECT* prc)
{
	if (sm_themeLibLoaded)
	{
		return sm_drawThemeParentBackground(hwnd, hdc, prc);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::getThemePartSize(HTHEME hTheme, HDC hdc, int iPartId,
									int iStateId, RECT *prc,
									enum THEMESIZE eSize, SIZE *psz)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemePartSize(hTheme, hdc, iPartId, iStateId, prc, eSize,
			psz);
	}
	else
	{
		return E_NOTIMPL;
	}
}

HRESULT CUIThemes::enableThemeDialogTexture(HWND hwnd, DWORD dwFlags)
{
	if (sm_themeLibLoaded)
	{
		return sm_enableThemeDialogTexture(hwnd, dwFlags);
	}
	else
	{
		return E_NOTIMPL;
	}
}

COLORREF CUIThemes::getThemeSysColor(HTHEME hTheme, int iColorID)
{
	if (sm_themeLibLoaded)
	{
		return sm_getThemeSysColor(hTheme, iColorID);
	}
	else
	{
		return GetSysColor(iColorID);
	}
}

BOOL CUIThemes::isThemeActive(void)
{
	if (sm_themeLibLoaded)
	{
		return sm_isThemeActive();
	}
	else
	{
		return FALSE;
	}
}
