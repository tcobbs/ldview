#ifndef __PARTLISTDIALOG_H__
#define __PARTLISTDIALOG_H__

#include <TCFoundation/TCObject.h>
#include <Commctrl.h>

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
	INT_PTR doNotify(int controlId, LPNMHDR pnmh);
	void doListViewItemChanged(int controlId, LPNMLISTVIEW pnmlv);
	INT_PTR doClick(int controlId, HWND hControl);
	void saveSettings(void);
	void setCheck(int controlId, bool value);
	bool getCheck(int controlId);
	void registerWindowClass(void);
	void populateColumnList(void);
	void setupToolbar(void);
	void updateOverwriteCheck(void);
	INT_PTR doMoveColumn(int distance);

	static INT_PTR CALLBACK staticDialogProc(HWND hDlg,
		UINT message, WPARAM wParam, LPARAM lParam);

	CUIWindow *m_parentWindow;
	LDHtmlInventory *m_htmlInventory;
	HWND m_hDlg;
	HWND m_hColumnList;
	HWND m_hToolbar;
	HIMAGELIST m_hImageList;
	HWND m_hOverwrite;
	int m_modalReturn;
	bool m_showFile;
	bool m_overwriteSnapshot;
};

#endif // __PARTLISTDIALOG_H__
