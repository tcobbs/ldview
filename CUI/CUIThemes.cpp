#include "CUIThemes.h"
#include <windows.h>
#include <atlbase.h>
#include <atlconv.h>

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
		sm_hModThemes = LoadLibrary("UXTHEME.DLL");
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

			if (sm_openThemeData && sm_closeThemeData && sm_drawThemeBackground
				&& sm_drawThemeText && sm_getThemeBackgroundContentRect &&
				sm_drawThemeEdge && sm_setWindowTheme)
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

HRESULT CUIThemes::setWindowTheme(HWND hWnd, LPCSTR pszSubAppName,
								  LPCSTR pszSubIdList)
{
	USES_CONVERSION;
	return setWindowTheme(hWnd, A2W(pszSubAppName), A2W(pszSubIdList));
}
