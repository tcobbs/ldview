#ifndef __PARTLISTDIALOG_H__
#define __PARTLISTDIALOG_H__

#include <TCFoundation/TCObject.h>

class CUIWindow;
class LDHtmlInventory;

class PartsListDialog : public TCObject
{
public:
	PartsListDialog(CUIWindow *parentWindow, LDHtmlInventory *htmlInventory);
	int runModal(void);
protected:
	~PartsListDialog();
	void dealloc(void);
	void createDialog(void);
	INT_PTR dialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	INT_PTR doInitDialog(HWND hDlg, HWND hFocus, LPARAM lParam);
	INT_PTR doCommand(int controlId, int notifyCode, HWND controlHWnd);
	INT_PTR doClick(int controlId, HWND hControl);
	void saveSettings(void);
	void setCheck(int controlId, bool value);
	bool getCheck(int controlId);

	static INT_PTR CALLBACK staticDialogProc(HWND hDlg,
		UINT message, WPARAM wParam, LPARAM lParam);

	CUIWindow *m_parentWindow;
	LDHtmlInventory *m_htmlInventory;
	HWND m_hDlg;
	int m_modalReturn;
};

#endif // __PARTLISTDIALOG_H__
