#ifndef __MODELTREEDIALOG_H__
#define __MODELTREEDIALOG_H__

class LDLMainModel;
class LDModelTree;
class CUIWindowResizer;
class ModelWindow;
class TCAlert;

#include <CUI/CUIDialog.h>

class ModelTreeDialog: public CUIDialog
{
public:
	ModelTreeDialog(HINSTANCE hInstance, HWND hParentWindow);
	void show(ModelWindow *modelWindow, HWND hParentWnd = NULL);
protected:
	virtual LRESULT doClose(void);

	virtual ~ModelTreeDialog(void);
	virtual void dealloc(void);
	HTREEITEM addLine(HTREEITEM parent, const LDModelTree *tree);
	void addChildren(HTREEITEM parent, const LDModelTree *tree);
	void fillTreeView(void);
	void setModel(LDLMainModel *model);
	void setModelWindow(ModelWindow *modelWindow);
	void modelAlertCallback(TCAlert *alert);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doNotify(int controlId, LPNMHDR notification);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);

	virtual LRESULT doItemExpanding(LPNMTREEVIEW notification);

	ModelWindow *m_modelWindow;
	LDLMainModel *m_model;
	LDModelTree *m_modelTree;
	HWND m_hTreeView;
	CUIWindowResizer *m_resizer;
};

#endif // __MODELTREEDIALOG_H__
