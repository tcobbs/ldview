#ifndef __CUITHEMES_H__
#define __CUITHEMES_H__

#include <TCFoundation/TCDefines.h>
#include <windows.h>
#include <CUI/CUIDefines.h>

#if WINVER >= 0x0501
// XP specific stuff
#include <uxtheme.h>
// _MSC_VER 1500 = Visual Studio 2008.
#if (defined(_MSC_VER) && _MSC_VER >= 1400)
#include <vssym32.h>
#else
#include <tmschema.h>
#endif
#else
typedef HANDLE HTHEME;          // handle to a section of theme data for class
#endif

typedef HTHEME (__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
	DWORD dwTextFlags,  DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,
	HDC hdc, int iPartId, int iStateId,  const RECT *pBoundingRect,
	RECT *pContentRect);
typedef HRESULT (_stdcall *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, UINT uFlags,
	RECT *pContentRect);
typedef HRESULT (_stdcall *PFNSETWINDOWTHEME)(HWND hWnd, LPCWSTR pwszSubAppName,
	LPCWSTR pwszSubIdList);
typedef HTHEME (_stdcall *PFNGETWINDOWTHEME)(HWND hWnd);
typedef HRESULT (_stdcall *PFNGETTHEMECOLOR)(HTHEME hTheme, int iPartId,
	int iStateId, int iPropId, COLORREF *pColor);
typedef void (_stdcall *PFNSETTHEMEAPPPROPERTIED)(DWORD dwFlags);
typedef DWORD (_stdcall *PFNGETTHEMEAPPPROPERTIED)(void);
typedef HRESULT (_stdcall *PFNGETTHEMERECT)(HTHEME hTheme, int iPartId,
	int iStateId, int iPropId, RECT *pRect);
typedef HRESULT (_stdcall *PFNGETTHEMEPOSITION)(HTHEME hTheme, int iPartId,
	int iStateId, int iPropId, POINT *pPoint);
typedef HRESULT (_stdcall *PFNGETTHEMEMARGINS)(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId, int iPropId, RECT *pRect, MARGINS *pMargins);
typedef HRESULT (_stdcall *PFNGETTHEMETEXTEXTENT)(HTHEME hTheme, HDC hdc,
    int iPartId, int iStateId, LPCWSTR pszText, int iCharCount,
	DWORD dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect);
typedef HRESULT (_stdcall *PFNDDRAWTHEMEPARENTBACKGROUND)(HWND hwnd, HDC hdc,
	RECT* prc);
typedef HRESULT (_stdcall *PFNGETTHEMEPARTSIZE)(HTHEME hTheme, HDC hdc,
	int iPartId, int iStateId, RECT *prc, enum THEMESIZE eSize, SIZE *psz);
typedef HRESULT (_stdcall *PFNENABLETHEMEDIALOGTEXTURE)(HWND hwnd,
	DWORD dwFlags);
typedef COLORREF (_stdcall *PFNGETTHEMESYSCOLOR)(HTHEME hTheme, int iColorID);
typedef BOOL (_stdcall *PFNISTHEMEACTIVE)(void);

class CUIExport CUIThemes
{
public:
	static void init(void);
	static bool isThemeLibLoaded(void) { return sm_themeLibLoaded; }
	static HTHEME openThemeData(HWND hwnd, LPCWSTR pszClassList);
	static HRESULT closeThemeData(HTHEME hTheme);
	static HRESULT drawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, const RECT *pRect,  const RECT *pClipRect);
	static HRESULT drawThemeText(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
		DWORD dwTextFlags2, const RECT *pRect);
	static HRESULT getThemeBackgroundContentRect(HTHEME hTheme, HDC hdc,
		int iPartId, int iStateId,  const RECT *pBoundingRect,
		RECT *pContentRect);
	static HRESULT drawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, const RECT *pDestRect, UINT uEdge, UINT uFlags,
		RECT *pContentRect);
	static HRESULT setWindowTheme(HWND hWnd, LPCWSTR pwszSubAppName,
		LPCWSTR pwszSubIdList);
	static HTHEME getWindowTheme(HWND hWnd);
	static HRESULT getThemeColor(HTHEME hTheme, int iPartId, int iStateId,
		int iPropId, COLORREF *pColor);
	static HRESULT getThemeRect(HTHEME hTheme, int iPartId, int iStateId,
		int iPropId, RECT *pRect);
	static HRESULT getThemePosition(HTHEME hTheme, int iPartId, int iStateId,
		int iPropId, POINT *pPoint);
	static HRESULT getThemeMargins(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, int iPropId, RECT *pRect, MARGINS *pMargins);
	static void setThemeAppProperties(DWORD dwFlags);
	static DWORD getThemeAppProperties(void);
	static HRESULT getThemeTextExtent(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags,
		const RECT *pBoundingRect, RECT *pExtentRect);
	static HRESULT drawThemeParentBackground(HWND hwnd, HDC hdc, RECT* prc);
	static HRESULT getThemePartSize(HTHEME hTheme, HDC hdc, int iPartId,
		int iStateId, RECT *prc, enum THEMESIZE eSize, SIZE *psz);
	static HRESULT enableThemeDialogTexture(HWND hwnd, DWORD dwFlags);
	static COLORREF getThemeSysColor(HTHEME, int iColorID);
	static BOOL isThemeActive(void);

protected:
	CUIThemes(void);
	~CUIThemes(void);

	static bool sm_initTried;
	static bool sm_themeLibLoaded;
	static HMODULE sm_hModThemes;
	static PFNOPENTHEMEDATA sm_openThemeData;
	static PFNCLOSETHEMEDATA sm_closeThemeData;
	static PFNDRAWTHEMEBACKGROUND sm_drawThemeBackground;
	static PFNDRAWTHEMETEXT sm_drawThemeText;
	static PFNGETTHEMEBACKGROUNDCONTENTRECT sm_getThemeBackgroundContentRect;
	static PFNDRAWTHEMEEDGE sm_drawThemeEdge;
	static PFNSETWINDOWTHEME sm_setWindowTheme;
	static PFNGETWINDOWTHEME sm_getWindowTheme;
	static PFNGETTHEMECOLOR sm_getThemeColor;
	static PFNSETTHEMEAPPPROPERTIED sm_setThemeAppProperties;
	static PFNGETTHEMEAPPPROPERTIED sm_getThemeAppProperties;
	static PFNGETTHEMERECT sm_getThemeRect;
	static PFNGETTHEMEPOSITION sm_getThemePosition;
	static PFNGETTHEMEMARGINS sm_getThemeMargins;
	static PFNGETTHEMETEXTEXTENT sm_getThemeTextExtent;
	static PFNDDRAWTHEMEPARENTBACKGROUND sm_drawThemeParentBackground;
	static PFNGETTHEMEPARTSIZE sm_getThemePartSize;
	static PFNENABLETHEMEDIALOGTEXTURE sm_enableThemeDialogTexture;
	static PFNGETTHEMESYSCOLOR sm_getThemeSysColor;
	static PFNISTHEMEACTIVE sm_isThemeActive;
	static void deinit(void);

	static class CUIThemesCleanup
	{
	public:
		~CUIThemesCleanup(void);
	} sm_cuiThemesCleanup;
	friend class CUIThemesCleanup;
};

#endif // __CUITHEMES_H__