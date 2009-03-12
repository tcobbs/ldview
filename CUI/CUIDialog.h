#ifndef __CUIDIALOG_H__
#define __CUIDIALOG_H__

#include <CUI/CUIWindow.h>
#include <commctrl.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStlIncludes.h>

typedef std::set<HWND> HWndSet;

class CUIExport CUIDialog: public CUIWindow
{
public:
	CUIDialog(void);
	CUIDialog(CUIWindow* parentWindow);
	CUIDialog(HINSTANCE hInstance, HWND hParentWindow = NULL);
	virtual INT_PTR doModal(UINT dialogId, HWND hWndParent = NULL);
	virtual INT_PTR doModal(CUCSTR dialogName, HWND hWndParent = NULL);
	virtual void setIcon(int templateNumber);
	virtual void setIcon(char* templateName);

	virtual void createDialog(int templateNumber, HWND hParentWnd = NULL);
	virtual void createDialog(CUCSTR templateName, HWND hParentWnd = NULL);

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

	virtual void listBoxResetContent(int controlId);
	virtual void listBoxAddString(int controlId, CUCSTR string);
#ifndef TC_NO_UNICODE
	virtual void listBoxAddString(int controlId, LPCTSTR string);
#endif // TC_NO_UNICODE
	virtual LRESULT listBoxSelectItem(int controlId, int index);
	virtual int listBoxGetSelectedItem(int controlId);

	virtual void windowGetText(int controlId, ucstring &text);
	static void windowGetText(HWND hDlg, int controlId, ucstring &text);
	virtual ucstring windowGetText(int controlId);
	virtual void windowSetText(int controlId, const ucstring &text);
	virtual void windowSetText(HWND hDlg, int controlId, const ucstring &text);
#ifndef TC_NO_UNICODE
	virtual void windowGetText(int controlId, std::string &text);
	static void windowGetText(HWND hDlg, int controlId, std::string &text);
	virtual void windowSetText(int controlId, const std::string &text);
	static void windowSetText(HWND hDlg, int controlId,
		const std::string &text);
#endif // TC_NO_UNICODE
	virtual bool windowGetValue(int controlId, long &value);
	virtual bool windowGetValue(int controlId, float &value);
	virtual bool windowGetValue(int controlId, double &value);
	virtual void windowSetValue(int controlId, long value);
	virtual void windowSetValue(int controlId, double value);

	virtual void textFieldSetLimitText(int controlId, int value);
	virtual void textFieldGetSelection(int controlId, int &start, int &end);
	virtual void textFieldSetSelection(int controlId, int start, int end);

	void addControl(HWND hWnd);

	static CUIDialog *fromHandle(HWND hWnd);
	static UINT getMessageForwardId(void) { return sm_messageForwardId; }

	struct ControlMessage
	{
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
		LRESULT lResult;
		bool processed;
	};
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
	virtual LRESULT doContextMenu(HWND hWnd, int xPos, int yPos);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual void doOK(void);
	virtual void doCancel(void);

	static INT_PTR CALLBACK staticDialogProc(HWND hDlg, UINT message,
		WPARAM wParam, LPARAM lParam);

	HWND m_curHWnd;
	UINT m_curMessage;
	WPARAM m_curWParam;
	LPARAM m_curLParam;
	HWndSet m_controls;

	static bool sm_haveMessageForwardId;
	static UINT sm_messageForwardId;
private:
	INT_PTR privateDialogProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	LRESULT doPrivateMessage1(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	LRESULT doPrivateMessage2(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	LRESULT doPrivateNotify(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT doPrivateCommand(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT doPrivateDrawItem(UINT message, WPARAM wParam, LPARAM lParam);
	bool doPrivateForward(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam, LRESULT &lResult);
	static void registerMessageForwardId(void);
};

#endif // __CUIDIALOG_H__
