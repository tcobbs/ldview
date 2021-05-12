#ifndef __MODELTREEDIALOG_H__
#define __MODELTREEDIALOG_H__

class LDLMainModel;
class CUIWindowResizer;
class CUIColorButton;
class ModelWindow;
class TCAlert;

#include <CUI/CUIDialog.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>
#include <LDLib/LDModelTree.h>

typedef std::vector<HWND> HWndVector;
typedef std::map<UINT, LDLLineType> UIntLineTypeMap;
typedef std::map<LDLLineType, UINT> LineTypeUIntMap;

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
	void refreshTreeView(void);
	void clearTreeView(void);
	void setModel(LDLMainModel *model);
	void setModelWindow(ModelWindow *modelWindow);
	void modelAlertCallback(TCAlert *alert);
	void setupLineCheck(UINT checkId, LDLLineType lineType);
	void updateLineChecks(void);
	void hideOptions(void);
	void showOptions(void);
	void adjustWindow(int widthDelta);
	void swapWindowText(UCCHAR oldChar, UCCHAR newChar);
	void selectFromHighlightPath(std::string path);
	HTREEITEM getChild(HTREEITEM hParent, int index);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doNotify(int controlId, LPNMHDR notification);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	virtual LRESULT doCommand(int notifyCode, int commandId,
		HWND control);
	virtual LRESULT doTimer(UINT_PTR);
	virtual void doOK(void);

	BOOL doTreeCopy(void);
	LRESULT doTreeKeyDown(LPNMTVKEYDOWN notification);
	LRESULT doTreeCustomDraw(LPNMTVCUSTOMDRAW notification);
	LRESULT doToggleOptions(void);
	LRESULT doHighlightCheck(void);
	LRESULT doSearchChange(void);
	LRESULT doSearch(LDModelTree::SearchMode mode, bool updateFocus);
	LRESULT doTreeItemExpanding(LPNMTREEVIEW notification);
	LRESULT doLineCheck(UINT checkId, LDLLineType lineType);
	LRESULT doTreeSelChanged(LPNMTREEVIEW notification);
	LRESULT doHighlightColorChanged(void);
	void highlightItem(HTREEITEM hItem);
	void initStatusBar(void);
	void updateStatusText(void);
	LDModelTree* getItemTree(HTREEITEM hItem);
	void addToolToTooltip(int controlId, const ucstring& tooltipText);

	ModelWindow *m_modelWindow;
	LDLMainModel *m_model;
	LDModelTree *m_modelTree;
	HWND m_hTreeView;
	HWND m_hStatus;
	HWND m_hTooltip;
	CUIWindowResizer *m_resizer;
	HWndVector m_lineChecks;
	UIntLineTypeMap m_checkLineTypes;
	LineTypeUIntMap m_checkIds;
	bool m_optionsShown;
	bool m_highlight;
	bool m_clearing;
	int m_optionsDelta;
	//HWND m_hResizeGrip;
	CUIColorButton *m_colorButton;
	int m_highlightR;
	int m_highlightG;
	int m_highlightB;
	std::string m_searchPath;
};

#endif // __MODELTREEDIALOG_H__
