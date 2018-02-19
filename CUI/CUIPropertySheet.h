#ifndef __CUIPROPERTYSHEET_H__
#define __CUIPROPERTYSHEET_H__

#include <CUI/CUIWindow.h>
#include <TCFoundation/TCTypedPointerArray.h>

#define CUI_OK IDOK
#define CUI_CANCEL IDCANCEL
#define CUI_APPLY IDYES

typedef TCTypedPointerArray<HPROPSHEETPAGE> CUIHPropSheetPageArray;

#ifdef TC_NO_UNICODE
#define PROPSHEETHEADERUC PROPSHEETHEADERA
#else // TC_NO_UNICODE
#define PROPSHEETHEADERUC PROPSHEETHEADERW
#endif // TC_NO_UNICODE

class CUIExport CUIPropertySheet: public CUIWindow
{
public:
	CUIPropertySheet(CUCSTR windowTitle, HINSTANCE hInstance);
	virtual INT_PTR run(void);
	virtual INT_PTR show(void);
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

#ifdef TC_NO_UNICODE
	static INT_PTR propertySheetUC(LPCPROPSHEETHEADERA lppsh);
#else // TC_NO_UNICODE
	static INT_PTR propertySheetUC(LPCPROPSHEETHEADERW lppsh);
#endif // TC_NO_UNICODE
	static void clearGlobalCUIPropertySheet(void)
	{
		globalCUIPropertySheet = NULL;
	}
protected:
	virtual ~CUIPropertySheet(void);
	virtual void dealloc(void);
	virtual INT_PTR createPropSheet(void);
	virtual BOOL doDialogNotify(HWND hDlg, int controlId,
		LPNMHDR notification);
	virtual bool doApply(void);
	virtual void doReset(void);
	virtual void setupPage(int pageNumber);
	virtual void clear(void);
	virtual void clearApplyEnabled(void);
	virtual bool shouldSetActive(int index);
	virtual void initScaler(void);

	static INT_PTR CALLBACK staticPropDialogProc(HWND hDlg, UINT message,
		WPARAM wParam, LPARAM lParam);
	static int CALLBACK staticPropSheetProc(HWND hDlg, UINT uMsg,
		LPARAM lParam);

	HWND hDlgParent;
	HWND hPropSheet;
	CUIHwndArray *hwndArray;
	CUIHPropSheetPageArray *hpageArray;
	TCArray<> *applyEnabledArray;
	bool isModeless;

	static CUIPropertySheet *globalCUIPropertySheet;
};

#endif // __CUIPROPERTYSHEET_H__
