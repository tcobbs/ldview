#ifndef __MODELTREEDIALOG_H__
#define __MODELTREEDIALOG_H__

class LDLMainModel;
class LDModelTree;
class CUIWindowResizer;

#include <CUI/CUIDialog.h>

class ModelTreeDialog: public CUIDialog
{
public:
	ModelTreeDialog(HINSTANCE hInstance, HWND hParentWindow);
	void show(LDLMainModel *model, HWND hParentWnd = NULL);
protected:
	virtual LRESULT doClose(void);

	virtual ~ModelTreeDialog(void);
	virtual void dealloc(void);
	HTREEITEM addLine(HTREEITEM parent, const LDModelTree *tree);
	void addChildren(HTREEITEM parent, const LDModelTree *tree);
	void fillTreeView(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doNotify(int controlId, LPNMHDR notification);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);

	virtual LRESULT doItemExpanding(LPNMTREEVIEW notification);

	LDLMainModel *m_model;
	LDModelTree *m_modelTree;
	HWND m_hTreeView;
	CUIWindowResizer *m_resizer;
};

#endif // __MODELTREEDIALOG_H__
