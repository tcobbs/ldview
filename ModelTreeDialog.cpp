#include "ModelTreeDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCAlertManager.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLib/LDModelTree.h>
#include <LDLib/LDUserDefaultsKeys.h>
#include <CUI/CUIWindowResizer.h>
#include <CommCtrl.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

ModelTreeDialog::ModelTreeDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIDialog(hInstance, hParentWindow),
m_modelWindow(NULL),
m_model(NULL),
m_modelTree(NULL),
m_resizer(NULL),
m_optionsShown(true),
m_highlight(TCUserDefaults::boolForKey(MODEL_TREE_HIGHLIGHT_KEY, false, false))
{
	TCAlertManager::registerHandler(ModelWindow::alertClass(), this,
		(TCAlertCallback)&ModelTreeDialog::modelAlertCallback);
}

ModelTreeDialog::~ModelTreeDialog(void)
{
}

void ModelTreeDialog::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	TCObject::release(m_modelWindow);
	TCObject::release(m_modelTree);
	TCObject::release(m_model);
	TCObject::release(m_resizer);
	CUIDialog::dealloc();
}

void ModelTreeDialog::setModel(LDLMainModel *model)
{
	if (model != m_model)
	{
		TreeView_DeleteAllItems(m_hTreeView);
		TCObject::release(m_model);
		TCObject::release(m_modelTree);
		m_modelTree = NULL;
		m_model = model;
		TCObject::retain(m_model);
	}
}

void ModelTreeDialog::modelAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_modelWindow)
	{
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0)
		{
			setModel(m_modelWindow->getModelViewer()->getMainModel());
			fillTreeView();
		}
		else if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
		{
			setModel(NULL);
			fillTreeView();
		}
	}
}

void ModelTreeDialog::setModelWindow(ModelWindow *modelWindow)
{
	if (modelWindow != m_modelWindow)
	{
		if (m_modelWindow)
		{
			m_modelWindow->release();
		}
		m_modelWindow = modelWindow;
		TCObject::retain(m_modelWindow);
	}
	setModel(m_modelWindow->getModelViewer()->getMainModel());
}

void ModelTreeDialog::show(ModelWindow *modelWindow, HWND hParentWnd /*= NULL*/)
{
	setModelWindow(modelWindow);
	if (hWindow == NULL)
	{
		createDialog(IDD_MODELTREE, hParentWnd);
	}
	ShowWindow(hWindow, SW_SHOW);
	fillTreeView();
}

LRESULT ModelTreeDialog::doTreeItemExpanding(LPNMTREEVIEW notification)
{
	if (notification->action == TVE_EXPAND)
	{
		LDModelTree *tree = (LDModelTree *)notification->itemNew.lParam;

		if (!tree->getViewPopulated())
		{
			addChildren(notification->itemNew.hItem, tree);
			tree->setViewPopulated(true);
		}
		return 0;
	}
	return 1;
}

void ModelTreeDialog::highlightItem(HTREEITEM hItem)
{
	if (hItem != NULL)
	{
		TVITEMEX item;

		memset(&item, 0, sizeof(item));
		item.mask = TVIF_PARAM;
		item.hItem = hItem;
		if (TreeView_GetItem(m_hTreeView, &item))
		{
			LDModelTree *tree = (LDModelTree *)item.lParam;

			if (tree != NULL)
			{
				m_modelWindow->getModelViewer()->setHighlightPaths(
					tree->getTreePath());
			}
		}
	}
	else
	{
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	}
}

LRESULT ModelTreeDialog::doTreeSelChanged(LPNMTREEVIEW notification)
{
	if (m_highlight)
	{
		highlightItem(notification->itemNew.hItem);
	}
	return 1;	// Ignored
}

LRESULT ModelTreeDialog::doNotify(int controlId, LPNMHDR notification)
{
	if (controlId == IDC_MODEL_TREE)
	{
		switch (notification->code)
		{
		case TVN_ITEMEXPANDING:
			return doTreeItemExpanding((LPNMTREEVIEW)notification);
		case TVN_KEYDOWN:
			return doTreeKeyDown((LPNMTVKEYDOWN)notification);
		case TVN_SELCHANGED:
			return doTreeSelChanged((LPNMTREEVIEW)notification);
		case NM_CUSTOMDRAW:
			return doTreeCustomDraw((LPNMTVCUSTOMDRAW)notification);
		}
	}
	return 1;
}

void ModelTreeDialog::addChildren(HTREEITEM parent, const LDModelTree *tree)
{
	if (tree != NULL && tree->hasChildren(true))
	{
		const LDModelTreeArray *children = tree->getChildren(true);
		int count = children->getCount();

		for (int i = 0; i < count; i++)
		{
			const LDModelTree *child = (*children)[i];
			addLine(parent, child);
		}
	}
}

HTREEITEM ModelTreeDialog::addLine(HTREEITEM parent, const LDModelTree *tree)
{
	TVINSERTSTRUCT insertStruct;
	TVITEMEX item;
	HTREEITEM hNewItem;

	memset(&item, 0, sizeof(item));
	item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	item.cChildren = tree->getNumChildren(true);
	item.pszText = copyString(tree->getText().c_str());
	item.lParam = (LPARAM)tree;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = TVI_LAST;
	insertStruct.itemex = item;
	hNewItem = TreeView_InsertItem(m_hTreeView, &insertStruct);
	delete item.pszText;
	return hNewItem;
}

void ModelTreeDialog::updateLineChecks(void)
{
	for (int i = LDLLineTypeComment; i <= LDLLineTypeUnknown; i++)
	{
		LDLLineType lineType = (LDLLineType)i;
		bool checked = false;
		BOOL enabled = m_model != NULL;

		if (m_modelTree)
		{
			checked = m_modelTree->getShowLineType(lineType);
		}
		checkSet(m_checkIds[lineType], checked);
		EnableWindow(m_lineChecks[i], enabled);
	}
}

LRESULT ModelTreeDialog::doLineCheck(UINT checkId, LDLLineType lineType)
{
	if (m_modelTree)
	{
		m_modelTree->setShowLineType(lineType, checkGet(checkId));
		refreshTreeView();
	}
	return 0;
}

LRESULT ModelTreeDialog::doHighlightCheck(void)
{
	m_highlight = checkGet(IDC_HIGHLIGHT);
	if (m_highlight)
	{
		highlightItem(TreeView_GetSelection(m_hTreeView));
	}
	else
	{
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	}
	TCUserDefaults::setBoolForKey(m_highlight, MODEL_TREE_HIGHLIGHT_KEY, false);
	return 0;
}

LRESULT ModelTreeDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	switch (commandId)
	{
	case IDC_OPTIONS:
		return doToggleOptions();
	case IDC_HIGHLIGHT:
		return doHighlightCheck();
	default:
		{
			UIntLineTypeMap::const_iterator it = m_checkLineTypes.find(commandId);

			if (it != m_checkLineTypes.end())
			{
				return doLineCheck(it->first, it->second);
			}
		}
		break;
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void ModelTreeDialog::refreshTreeView(void)
{
	SendMessage(m_hTreeView, WM_SETREDRAW, FALSE, 0);
	TreeView_DeleteAllItems(m_hTreeView);
	addChildren(NULL, m_modelTree);
	SendMessage(m_hTreeView, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(m_hTreeView, NULL, NULL, RDW_INVALIDATE);
}

void ModelTreeDialog::fillTreeView(void)
{
	if (m_modelTree == NULL && IsWindowVisible(hWindow))
	{
		if (m_model)
		{
			m_modelTree = new LDModelTree(m_model);
		}
		updateLineChecks();
		refreshTreeView();
	}
}

void ModelTreeDialog::setupLineCheck(UINT checkId, LDLLineType lineType)
{
	m_lineChecks[lineType] = GetDlgItem(hWindow, checkId);
	m_checkLineTypes[checkId] = lineType;
	m_checkIds[lineType] = checkId;
	m_resizer->addSubWindow(checkId, CUIFloatLeft | CUIFloatBottom);
}

void ModelTreeDialog::adjustWindow(int widthDelta)
{
	WINDOWPLACEMENT wp;

	minWidth += widthDelta;
	GetWindowPlacement(hWindow, &wp);
	m_resizer->setOriginalWidth(m_resizer->getOriginalWidth() + widthDelta);
	if (wp.showCmd == SW_MAXIMIZE)
	{
		RECT clientRect;

		GetClientRect(hWindow, &clientRect);
		m_resizer->resize(clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top);
	}
	else
	{
		RECT windowRect;

		GetWindowRect(hWindow, &windowRect);
		windowRect.right += widthDelta;
		MoveWindow(hWindow, windowRect.left, windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top, TRUE);
	}
}

void ModelTreeDialog::swapWindowText(char oldChar, char newChar)
{
	std::string text;

	windowGetText(IDC_OPTIONS, text);
	replaceStringCharacter(&text[0], oldChar, newChar);
	windowSetText(IDC_OPTIONS, text);
}

void ModelTreeDialog::hideOptions(void)
{
	adjustWindow(-m_optionsDelta);
	swapWindowText('<', '>');
	m_optionsShown = false;
}

void ModelTreeDialog::showOptions(void)
{
	adjustWindow(m_optionsDelta);
	swapWindowText('>', '<');
	m_optionsShown = true;
}

LRESULT ModelTreeDialog::doToggleOptions(void)
{
	if (m_optionsShown)
	{
		hideOptions();
	}
	else
	{
		showOptions();
	}
	TCUserDefaults::setBoolForKey(m_optionsShown, MODEL_TREE_OPTIONS_SHOWN_KEY,
		false);
	return 0;
}

BOOL ModelTreeDialog::doInitDialog(HWND hKbControl)
{
	RECT optionsRect;
	RECT clientRect;

	setIcon(IDI_APP_ICON);
	m_hTreeView = GetDlgItem(hWindow, IDC_MODEL_TREE);
	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(IDC_MODEL_TREE,
		CUISizeHorizontal | CUISizeVertical);
	m_resizer->addSubWindow(IDC_SHOW_BOX, CUIFloatLeft | CUIFloatBottom);
	m_resizer->addSubWindow(IDC_OPTIONS, CUIFloatLeft | CUIFloatTop);
	m_resizer->addSubWindow(IDC_HIGHLIGHT, CUIFloatRight | CUIFloatTop);
	m_lineChecks.resize(LDLLineTypeUnknown + 1);
	setupLineCheck(IDC_COMMENT, LDLLineTypeComment);
	setupLineCheck(IDC_MODEL, LDLLineTypeModel);
	setupLineCheck(IDC_LINE, LDLLineTypeLine);
	setupLineCheck(IDC_TRIANGLE, LDLLineTypeTriangle);
	setupLineCheck(IDC_QUAD, LDLLineTypeQuad);
	setupLineCheck(IDC_CONDITIONAL, LDLLineTypeConditionalLine);
	setupLineCheck(IDC_EMPTY, LDLLineTypeEmpty);
	setupLineCheck(IDC_UNKNOWN, LDLLineTypeUnknown);
	GetClientRect(hWindow, &clientRect);
	m_hResizeGrip = GetDlgItem(hWindow, IDC_RESIZEGRIP);
	positionResizeGrip(m_hResizeGrip, clientRect.right, clientRect.bottom);
	GetWindowRect(GetDlgItem(hWindow, IDC_SHOW_BOX), &optionsRect);
	screenToClient(hWindow, &optionsRect);
	m_optionsDelta = clientRect.right - optionsRect.left;
	minWidth = clientRect.right;
	minHeight = clientRect.bottom;
	if (!TCUserDefaults::boolForKey(MODEL_TREE_OPTIONS_SHOWN_KEY, true, false))
	{
		hideOptions();
	}
	checkSet(IDC_HIGHLIGHT, m_highlight);
	setAutosaveName("ModelTreeDialog");
	return CUIDialog::doInitDialog(hKbControl);
}

LRESULT ModelTreeDialog::doClose(void)
{
	if (m_highlight)
	{
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	}
	showWindow(SW_HIDE);
	return 0;
}

LRESULT ModelTreeDialog::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (sizeType != SIZE_MINIMIZED)
	{
		m_resizer->resize(newWidth, newHeight);
		positionResizeGrip(m_hResizeGrip, newWidth, newHeight);
	}
	return CUIDialog::doSize(sizeType, newWidth, newHeight);
}

LRESULT ModelTreeDialog::doTreeCustomDraw(LPNMTVCUSTOMDRAW notification)
{
	if (notification->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		SetWindowLong(hWindow, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW);
		return 0;
	}
	else if (notification->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		LDModelTree *itemTree = (LDModelTree *)notification->nmcd.lItemlParam;

		if (itemTree)
		{
			TCByte r, g, b;

			if ((notification->nmcd.uItemState & CDIS_SELECTED) == 0 &&
				itemTree->getTextRGB(r, g, b))
			{
				notification->clrText = RGB(r, g, b);
			}
			SetWindowLong(hWindow, DWL_MSGRESULT, CDRF_DODEFAULT);
			return 0;
		}
	}
	return 1;
}

LRESULT ModelTreeDialog::doTreeKeyDown(LPNMTVKEYDOWN notification)
{
	if (notification->wVKey == 'C' && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		if (doTreeCopy())
		{
			return 0;
		}
	}
	return 1;
}

BOOL ModelTreeDialog::doTreeCopy(void)
{
	HTREEITEM hItem = TreeView_GetSelection(m_hTreeView);

	if (hItem)
	{
		TVITEMEX item;

		memset(&item, 0, sizeof(item));
		item.mask = TVIF_PARAM;
		item.hItem = hItem;
		if (TreeView_GetItem(m_hTreeView, &item))
		{
			LDModelTree *tree = (LDModelTree *)item.lParam;

			if (tree)
			{
				std::string text = tree->getText();

				if (tree->getLineType() == LDLLineTypeEmpty)
				{
					text = "";
				}
				text += "\r\n";
				if (copyToClipboard(text.c_str()))
				{
					SetWindowLong(hWindow, DWL_MSGRESULT, TRUE);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
