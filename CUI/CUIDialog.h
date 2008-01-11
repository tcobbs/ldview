#ifndef __CUIDIALOG_H__
#define __CUIDIALOG_H__

#include <CUI/CUIWindow.h>
#include <commctrl.h>

class CUIExport CUIDialog: public CUIWindow
{
public:
	CUIDialog(void);
	CUIDialog(CUIWindow* parentWindow);
	CUIDialog(HINSTANCE hInstance, HWND hParentWindow = NULL);
	virtual INT_PTR doModal(UINT dialogId, HWND hWndParent = NULL);
	virtual INT_PTR doModal(LPCTSTR dialogName, HWND hWndParent = NULL);
	virtual bool getCheck(int buttonId);
	virtual void setCheck(int buttonId, bool value);
	virtual void setupSlider(int controlId, short min, short max,
		WORD frequency, int value);
	virtual void setSliderValue(int controlId, int value);
	virtual void setSliderTic(int controlId, int position);
	virtual int getSliderValue(int controlId);
	void setupSpin(int controlId, short min, short max, int value,
		UDACCEL *accels = NULL, int numAccels = 0);
	void CUIDialog::setSpinValue(int controlId, int value);
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
