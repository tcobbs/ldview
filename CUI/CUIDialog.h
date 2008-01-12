#ifndef __CUIDIALOG_H__
#define __CUIDIALOG_H__

#include <CUI/CUIWindow.h>
#include <commctrl.h>
#include <TCFoundation/mystring.h>

class CUIExport CUIDialog: public CUIWindow
{
public:
	CUIDialog(void);
	CUIDialog(CUIWindow* parentWindow);
	CUIDialog(HINSTANCE hInstance, HWND hParentWindow = NULL);
	virtual INT_PTR doModal(UINT dialogId, HWND hWndParent = NULL);
	virtual INT_PTR doModal(LPCSTR dialogName, HWND hWndParent = NULL);

	virtual bool checkGet(int buttonId);
	virtual void checkSet(int buttonId, bool value);

	virtual void sliderSetup(int controlId, short min, short max,
		WORD frequency, int value);
	virtual void sliderSetValue(int controlId, int value);
	virtual void sliderSetTic(int controlId, int position);
	virtual int sliderGetValue(int controlId);

	virtual void comboAddString(int controlId, CUCSTR string);
	virtual LRESULT comboSelectItem(int controlId, int index);
	virtual int comboGetSelectedItem(int controlId);

	virtual void spinSetup(int controlId, short min, short max, int value,
		UDACCEL *accels = NULL, int numAccels = 0);
	virtual void spinSetValue(int controlId, int value);

	virtual void windowGetText(int controlId, ucstring &text);
	virtual ucstring windowGetText(int controlId);
	virtual void windowSetText(int controlId, const ucstring &text);
#ifndef TC_NO_UNICODE
	virtual void windowGetText(int controlId, std::string &text);
	virtual void windowSetText(int controlId, const std::string &text);
#endif // TC_NO_UNICODE

	virtual void textFieldSetLimitText(int controlId, int value);
	virtual void textFieldGetSelection(int controlId, int &start, int &end);
	virtual void textFieldSetSelection(int controlId, int start, int end);
protected:
	virtual ~CUIDialog(void);
	virtual void dealloc(void);
	virtual INT_PTR dialogProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	virtual BOOL doInitDialog(HWND hKbControl);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual LRESULT doHScroll(int scrollCode, int position, HWND hScrollBar);
	virtual LRESULT doVScroll(int scrollCode, int position, HWND hScrollBar);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual void doOK(void);
	virtual void doCancel(void);
	static INT_PTR CALLBACK staticDialogProc(HWND hDlg, UINT message,
		WPARAM wParam, LPARAM lParam);

	HWND m_curHWnd;
	UINT m_curMessage;
	WPARAM m_curWParam;
	LPARAM m_curLParam;
private:
	INT_PTR privateDialogProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
};

#endif // __CUIDIALOG_H__
