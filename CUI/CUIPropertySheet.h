#ifndef __CUIPROPERTYSHEET_H__
#define __CUIPROPERTYSHEET_H__

#include <CUI/CUIWindow.h>
#include <TCFoundation/TCTypedPointerArray.h>

#define CUI_OK IDOK
#define CUI_CANCEL IDCANCEL
#define CUI_APPLY IDYES

typedef TCTypedPointerArray<HPROPSHEETPAGE> CUIHPropSheetPageArray;

class CUIExport CUIPropertySheet: public CUIWindow
{
public:
	CUIPropertySheet(char* windowTitle, HINSTANCE hInstance);
	virtual int run(void);
	virtual int show(void);
	virtual void addPage(int resourceId, char* title = NULL);
	virtual void enableApply(HWND hPage);
	virtual void disableApply(HWND hPage);
	void setIsModeless(bool value) { isModeless = value; }
	bool getIsModeless(void) { return isModeless; }
	void setHDlgParent(HWND hWnd) { hDlgParent = hWnd; }
	HWND getHDlgParent(void) { return hDlgParent; }
	virtual void closePropertySheet(bool immediate = false);
	HWND getHPropSheet(void) { return hPropSheet; }
	virtual bool getApplyEnabled(void);

	static void clearGlobalCUIPropertySheet(void)
	{
		globalCUIPropertySheet = NULL;
	}
protected:
	virtual ~CUIPropertySheet(void);
	virtual void dealloc(void);
	virtual int createPropSheet(void);
	virtual BOOL doDialogNotify(HWND hDlg, int controlId,
		LPNMHDR notification);
	virtual bool doApply(void);
	virtual void doReset(void);
	virtual void setupPage(int pageNumber);
	virtual void clear(void);
	virtual void clearApplyEnabled(void);
	virtual bool shouldSetActive(int index);

	static BOOL CALLBACK staticPropDialogProc(HWND hDlg, UINT message,
		WPARAM wParam, LPARAM lParam);
	static int CALLBACK staticPropSheetProc(HWND hDlg, UINT uMsg,
		LPARAM lParam);

	HWND hDlgParent;
	HWND hPropSheet;
	CUIHwndArray *hwndArray;
	CUIHPropSheetPageArray *hpageArray;
	TCArray *applyEnabledArray;
	bool isModeless;

	static CUIPropertySheet *globalCUIPropertySheet;
};

#endif // __CUIPROPERTYSHEET_H__
