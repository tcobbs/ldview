#ifndef __CUITHEMES_H__
#define __CUITHEMES_H__

#include <windows.h>
#include <CUI/CUIDefines.h>

#if WINVER >= 0x0501
// XP specific stuff
#include <uxtheme.h>
#include <tmschema.h>
#else
typedef HANDLE HTHEME;          // handle to a section of theme data for class
#endif

typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
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
	static void deinit(void);

	static class CUIThemesCleanup
	{
	public:
		~CUIThemesCleanup(void);
	} sm_cuiThemesCleanup;
	friend class CUIThemesCleanup;
};

#endif // __CUITHEMES_H__