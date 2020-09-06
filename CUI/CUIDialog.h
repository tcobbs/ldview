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
	virtual void setIcon(CUCSTR templateName);

	virtual void createDialog(int templateNumber, HWND hParentWnd = NULL);
	virtual void createDialog(CUCSTR templateName, HWND hParentWnd = NULL);

	virtual bool checkGet(int buttonId);
	virtual void checkSet(int buttonId, bool value);

	static int comboAddString(HWND hDlg, int controlId, CUCSTR string);
	static int comboAddString(HWND hDlg, int controlId,
		const ucstring &string);
	virtual int comboAddString(int controlId, CUCSTR string);
	virtual int comboAddString(int controlId, const ucstring &string);
	static int comboDeleteString(HWND hDlg, int controlId, int index);
	virtual int comboDeleteString(int controlId, int index);
	static int comboSelectString(HWND hDlg, int controlId, int startIndex,
		CUCSTR string);
	static int comboSelectString(HWND hDlg, int controlId, int startIndex,
		const ucstring &string);
	virtual int comboSelectString(int controlId, int startIndex,
		CUCSTR string);
	virtual int comboSelectString(int controlId, int startIndex,
		const ucstring &string);
	static void comboResetContent(HWND hDlg, int controlId);
	virtual void comboResetContent(int controlId);
	static int comboGetCurSel(HWND hDlg, int controlId);
	virtual int comboGetCurSel(int controlId);
	static int comboSetCurSel(HWND hDlg, int controlId, int index);
	virtual int comboSetCurSel(int controlId, int index);
	static int comboGetCount(HWND hDlg, int controlId);
	virtual int comboGetCount(int controlId);

	static void buttonSetCheck(HWND hDlg, int controlId, int state);
	virtual void buttonSetCheck(int controlId, int state);
	static void buttonSetChecked(HWND hDlg, int controlId, bool checked);
	virtual void buttonSetChecked(int controlId, bool checked);
	static int buttonGetCheck(HWND hDlg, int controlId);
	virtual int buttonGetCheck(int controlId);
	static bool buttonIsChecked(HWND hDlg, int controlId);
	virtual bool buttonIsChecked(int controlId);
	static HBITMAP buttonSetBitmap(HWND hDlg, int controlId, HBITMAP hBitmap);
	static HICON buttonSetIcon(HWND hDlg, int controlId, HICON hIcon);
	static HBITMAP buttonGetBitmap(HWND hWnd, int controlId);
	static HICON buttonGetIcon(HWND hWnd, int controlId);

	static void trackBarSetup(HWND hDlg, int controlId, short min, short max,
		WORD frequency, int pos);
	virtual void trackBarSetup(int controlId, short min, short max,
		WORD frequency, int pos);
	static void trackBarSetPos(HWND hDlg, int controlId, int pos,
		bool redraw = true);
	virtual void trackBarSetPos(int controlId, int pos, bool redraw = true);
	virtual void trackBarSetTic(int controlId, int position);
	static int trackBarGetPos(HWND hDlg, int controlId);
	virtual int trackBarGetPos(int controlId);

	static void upDownSetup(HWND hDlg, int controlId, short min, short max,
		int pos, UDACCEL *accels = NULL, int numAccels = 0);
	virtual void upDownSetup(int controlId, short min, short max, int pos,
		UDACCEL *accels = NULL, int numAccels = 0);
	static void upDownSetPos(HWND hDlg, int controlId, int pos);
	virtual void upDownSetPos(int controlId, int pos);

	static void listBoxResetContent(HWND hDlg, int controlId);
	virtual void listBoxResetContent(int controlId);
	static int listBoxAddString(HWND hDlg, int controlId, const ucstring& string);
	static int listBoxAddString(HWND hDlg, int controlId, CUCSTR string);
	virtual int listBoxAddString(int controlId, const ucstring& string);
	virtual int listBoxAddString(int controlId, CUCSTR string);
	static int listBoxSelectItem(HWND hDlg, int controlId, int index);
	virtual int listBoxSelectItem(int controlId, int index);
	static int listBoxGetSelectedItem(HWND hDlg, int controlId);
	virtual int listBoxGetSelectedItem(int controlId);

	static void windowGetText(HWND hDlg, int controlId, ucstring &text);
	virtual void windowGetText(int controlId, ucstring &text);
	static void windowSetText(HWND hWnd, CUCSTR text);
	static void windowSetText(HWND hWnd, const ucstring &text);
	static void windowSetText(HWND hWnd, int controlId, CUCSTR text);
	static void windowSetText(HWND hWnd, int controlId, const ucstring &text);
	virtual void windowSetText(int controlId, const ucstring &text);
	static bool windowGetValue(HWND hDlg, int controlId, long &value);
	static bool windowGetValue(HWND hDlg, int controlId, int &value);
	static bool windowGetValue(HWND hDlg, int controlId, float &value);
	static bool windowGetValue(HWND hDlg, int controlId, double &value);
	virtual bool windowGetValue(int controlId, long &value);
	virtual bool windowGetValue(int controlId, int &value);
	virtual bool windowGetValue(int controlId, float &value);
	virtual bool windowGetValue(int controlId, double &value);
	static void windowSetValue(HWND hDlg, int controlId, long value);
	static void windowSetValue(HWND hDlg, int controlId, int value);
	static void windowSetValue(HWND hDlg, int controlId, double value);
	virtual void windowSetValue(int controlId, long value);
	virtual void windowSetValue(int controlId, int value);
	virtual void windowSetValue(int controlId, double value);

	virtual void textFieldSetLimitText(int controlId, int value);
	virtual void textFieldGetSelection(int controlId, int &start, int &end);
	virtual void textFieldSetSelection(int controlId, int start, int end);

	void addControl(HWND hWnd);

	static CUIDialog *fromHandle(HWND hWnd);
	static UINT getMessageForwardId(void) { return sm_messageForwardId; }

	static UINT_PTR DoHtmlHelp(HWND hDlg, LPHELPINFO helpInfo);

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
