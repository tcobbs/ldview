#include "ModelTreeDialog.h"
#include "Resource.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <LDLoader/LDLMainModel.h>
#include <LDLib/LDModelTree.h>
#include <CUI/CUIWindowResizer.h>

ModelTreeDialog::ModelTreeDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIDialog(hInstance, hParentWindow),
m_model(NULL),
m_modelTree(NULL),
m_resizer(NULL)
{
}

ModelTreeDialog::~ModelTreeDialog(void)
{
}

void ModelTreeDialog::dealloc(void)
{
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	TCObject::release(m_modelTree);
	TCObject::release(m_model);
	TCObject::release(m_resizer);
	CUIDialog::dealloc();
}

void ModelTreeDialog::show(LDLMainModel *model, HWND hParentWnd /*= NULL*/)
{
	if (model != m_model)
	{
		TCObject::release(m_model);
		TCObject::release(m_modelTree);
		m_modelTree = NULL;
		m_model = model;
		m_model->retain();
	}
	if (hWindow == NULL)
	{
		createDialog(IDD_MODELTREE, hParentWnd);
	}
	fillTreeView();
	ShowWindow(hWindow, SW_SHOW);
}

LRESULT ModelTreeDialog::doItemExpanding(LPNMTREEVIEW notification)
{
	if (notification->action == TVE_EXPAND)
	{
		LDModelTree *tree = (LDModelTree *)notification->itemNew.lParam;

		if (!tree->getChildrenLoaded())
		{
			addChildren(notification->itemNew.hItem, tree);
		}
	}
	return 1;
}

LRESULT ModelTreeDialog::doNotify(int controlId, LPNMHDR notification)
{
	if (controlId == IDC_MODEL_TREE)
	{
		switch (notification->code)
		{
		case TVN_ITEMEXPANDING:
			doItemExpanding((LPNMTREEVIEW)notification);
		}
	}
	return 0;
}

void ModelTreeDialog::addChildren(HTREEITEM parent, const LDModelTree *tree)
{
	if (tree->hasChildren())
	{
		const LDModelTreeArray *children = tree->getChildren();
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
	item.cChildren = tree->getNumChildren();
	item.pszText = copyString(tree->getText().c_str());
	item.lParam = (LPARAM)tree;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = TVI_LAST;
	insertStruct.itemex = item;
	hNewItem = TreeView_InsertItem(m_hTreeView, &insertStruct);
	delete item.pszText;
	return hNewItem;
}

void ModelTreeDialog::fillTreeView(void)
{
	SendMessage(m_hTreeView, WM_SETREDRAW, FALSE, 0);
	TreeView_DeleteAllItems(m_hTreeView);
	m_modelTree = new LDModelTree(m_model);
	addChildren(NULL, m_modelTree);
	SendMessage(m_hTreeView, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(m_hTreeView, NULL, NULL, RDW_INVALIDATE);
}

BOOL ModelTreeDialog::doInitDialog(HWND /*hKbControl*/)
{
	setIcon(IDI_APP_ICON);
	m_hTreeView = GetDlgItem(hWindow, IDC_MODEL_TREE);
	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(IDC_MODEL_TREE,
		CUISizeHorizontal | CUISizeVertical);
	return TRUE;
}

LRESULT ModelTreeDialog::doClose(void)
{
	showWindow(SW_HIDE);
	return 0;
}

LRESULT ModelTreeDialog::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (sizeType != SIZE_MINIMIZED)
	{
		m_resizer->resize(newWidth, newHeight);
	}
	return 0;
}
