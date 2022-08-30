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
#include <CUI/CUIColorButton.h>
#include <CommCtrl.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#define SEARCH_TIMER 1

ModelTreeDialog::ModelTreeDialog(HINSTANCE hInstance, HWND hParentWindow):
CUIDialog(hInstance, hParentWindow),
m_modelWindow(NULL),
m_model(NULL),
m_modelTree(NULL),
m_resizer(NULL),
m_optionsShown(true),
m_highlight(TCUserDefaults::boolForKey(MODEL_TREE_HIGHLIGHT_KEY, false, false)),
m_clearing(false)
{
	COLORREF defHighlightColor = RGB(160, 224, 255);
	long highlightColor =
		TCUserDefaults::longForKey(MODEL_TREE_HIGHLIGHT_COLOR_KEY,
		defHighlightColor, false);
	LongVector customColors;
	
	customColors = TCUserDefaults::longVectorForKey(MODEL_TREE_CUST_COLORS_KEY,
		customColors, false);
	if (customColors.size() == 0)
	{
		// Put the default highlight color into the list of custom colors.
		customColors.resize(16);
		customColors[0] = defHighlightColor;
		TCUserDefaults::setLongVectorForKey(customColors,
			MODEL_TREE_CUST_COLORS_KEY, false);
	}
	m_highlightR = GetRValue(highlightColor);
	m_highlightG = GetGValue(highlightColor);
	m_highlightB = GetBValue(highlightColor);
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
	TCObject::release(m_colorButton);
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
		clearTreeView();
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
	LDModelTree *tree = getItemTree(hItem);

	if (tree != NULL)
	{
		m_modelWindow->getModelViewer()->setHighlightColor(m_highlightR,
			m_highlightG, m_highlightB, false);
		m_modelWindow->getModelViewer()->setHighlightPaths(
			tree->getTreePath());
	}
	else
	{
		m_modelWindow->getModelViewer()->setHighlightPaths("");
	}
}

LRESULT ModelTreeDialog::doTreeSelChanged(LPNMTREEVIEW notification)
{
	if (!m_clearing)
	{
		HTREEITEM hItem = notification->itemNew.hItem;
		if (m_highlight)
		{
			highlightItem(hItem);
		}
		LDModelTree* tree = getItemTree(hItem);
		if (tree != NULL)
		{
			m_searchPath = tree->getTreePath();
		}
		else
		{
			m_searchPath = "";
		}
		updateStatusText();
	}
	return 1;	// Ignored
}

LRESULT ModelTreeDialog::doHighlightColorChanged(void)
{
	LDrawModelViewer *modelViewer = m_modelWindow->getModelViewer();
	m_colorButton->getColor(m_highlightR, m_highlightG, m_highlightB);
	modelViewer->setHighlightColor(m_highlightR, m_highlightG, m_highlightB);
	TCUserDefaults::setLongForKey(RGB(m_highlightR, m_highlightG, m_highlightB),
		MODEL_TREE_HIGHLIGHT_COLOR_KEY, false);
	return 0;
}

LRESULT ModelTreeDialog::doNotify(int controlId, LPNMHDR notification)
{
	if (controlId == IDC_MODEL_TREE)
	{
		switch (notification->code)
		{
		case TVN_ITEMEXPANDINGUC:
			return doTreeItemExpanding((LPNMTREEVIEW)notification);
		case TVN_KEYDOWN:
			return doTreeKeyDown((LPNMTVKEYDOWN)notification);
		case TVN_SELCHANGEDUC:
			return doTreeSelChanged((LPNMTREEVIEW)notification);
		case NM_CUSTOMDRAW:
			return doTreeCustomDraw((LPNMTVCUSTOMDRAW)notification);
		}
	}
	else if (controlId == IDC_HIGHLIGHT_COLOR)
	{
		if (notification->code == CCBN_COLOR_CHANGED)
		{
			return doHighlightColorChanged();
		}
	}
	return 1;
}

void ModelTreeDialog::addChildren(HTREEITEM parent, const LDModelTree *tree)
{
	if (tree != NULL && tree->hasChildren(true))
	{
		const LDModelTreeArray *lchildren = tree->getChildren(true);
		int count = lchildren->getCount();

		for (int i = 0; i < count; i++)
		{
			const LDModelTree *child = (*lchildren)[i];
			addLine(parent, child);
		}
	}
}

HTREEITEM ModelTreeDialog::addLine(HTREEITEM parent, const LDModelTree *tree)
{
	TVINSERTSTRUCTUC insertStruct;
	TVITEMEXUC item;
	HTREEITEM hNewItem;

	memset(&item, 0, sizeof(item));
	item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	item.cChildren = tree->getNumChildren(true);
	item.pszText = copyString(tree->getTextUC().c_str());
	item.lParam = (LPARAM)tree;
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = TVI_LAST;
	insertStruct.itemex = item;
	hNewItem = (HTREEITEM)::SendMessage(m_hTreeView, TVM_INSERTITEMUC, 0,
		(LPARAM)&insertStruct);
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

LDModelTree* ModelTreeDialog::getItemTree(HTREEITEM hItem)
{
	if (!hItem)
	{
		return NULL;
	}
	TVITEMEX item;

	memset(&item, 0, sizeof(item));
	item.mask = TVIF_PARAM;
	item.hItem = hItem;
	if (TreeView_GetItem(m_hTreeView, &item))
	{
		return (LDModelTree *)item.lParam;
	}
	return NULL;
}

LRESULT ModelTreeDialog::doSearch(LDModelTree::SearchMode mode, bool updateFocus)
{
	ucstring searchString;
	windowGetText(IDC_SEARCH, searchString);
	std::string pathString = m_searchPath;
	if (m_modelTree->search(searchString, pathString, mode))
	{
		selectFromHighlightPath(pathString);
		if (updateFocus)
		{
			SetFocus(GetDlgItem(hWindow, IDC_MODEL_TREE));
		}
	}
	else
	{
		TreeView_SelectItem(m_hTreeView, NULL);
	}
	return 0;
}

LRESULT ModelTreeDialog::doTimer(UINT_PTR timerID)
{
	switch (timerID)
	{
	case SEARCH_TIMER:
		killTimer(SEARCH_TIMER);
		doSearch(LDModelTree::SearchMode::SMType, false);
		break;
	}
	return 0;
}

LRESULT ModelTreeDialog::doSearchChange(void)
{
	killTimer(SEARCH_TIMER);
	setTimer(SEARCH_TIMER, 500);
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
	case IDC_PREV:
		return doSearch(LDModelTree::SearchMode::SMPrevious, true);
	case IDC_NEXT:
		return doSearch(LDModelTree::SearchMode::SMNext, true);
	case IDC_SEARCH:
		if (notifyCode == EN_CHANGE)
		{
			return doSearchChange();
		}
		break;
	case IDOK:
		if (GetFocus() == GetDlgItem(hWindow, IDC_SEARCH))
		{
			killTimer(SEARCH_TIMER);
			return doSearch(LDModelTree::SearchMode::SMType, true);
		}
		break;
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

void ModelTreeDialog::clearTreeView(void)
{
	m_clearing = true;
	SendMessage(m_hTreeView, WM_SETREDRAW, FALSE, 0);
	TreeView_DeleteAllItems(m_hTreeView);
	SendMessage(m_hTreeView, WM_SETREDRAW, TRUE, 0);
	TreeView_SelectItem(m_hTreeView, NULL);
	m_clearing = false;
	updateStatusText();
}

HTREEITEM ModelTreeDialog::getChild(HTREEITEM hParent, int index)
{
	HTREEITEM hItem;

	hItem = TreeView_GetChild(m_hTreeView, hParent);
	for (int i = 1; i <= index; i++)
	{
		hItem = TreeView_GetNextSibling(m_hTreeView, hItem);
	}
	return hItem;
}

void ModelTreeDialog::selectFromHighlightPath(std::string path)
{
	HTREEITEM hItem = NULL;

	path = m_modelTree->adjustHighlightPath(path);
	while (path.size() > 0)
	{
		int lineNumber = atoi(&path[1]) - 1;

		hItem = getChild(hItem, lineNumber);
		if (hItem != NULL)
		{
			size_t index = path.find('/', 1);
			if (index < path.size())
			{
				TreeView_Expand(m_hTreeView, hItem, TVE_EXPAND);
				path = path.substr(index);
			}
			else
			{
				LDModelTree *tree = getItemTree(hItem);
				if (tree != NULL)
				{
					m_searchPath = tree->getTreePath();
				}
				TreeView_SelectItem(m_hTreeView, hItem);
				path = "";
			}
		}
		else
		{
			return;
		}
	}
}

void ModelTreeDialog::refreshTreeView(void)
{
	// The following has to be a copy, because the selection triggers the
	// list to disappear out from under us.
	StringList paths = m_modelWindow->getModelViewer()->getHighlightPaths();

	clearTreeView();
	SendMessage(m_hTreeView, WM_SETREDRAW, FALSE, 0);
	addChildren(NULL, m_modelTree);
	SendMessage(m_hTreeView, WM_SETREDRAW, TRUE, 0);
	RedrawWindow(m_hTreeView, NULL, NULL, RDW_INVALIDATE);
	if (m_modelTree != NULL)
	{
		for (StringList::const_iterator it = paths.begin(); it != paths.end();
			it++)
		{
			selectFromHighlightPath(*it);
		}
	}
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
	int showCommand;

	if (widthDelta > 0)
	{
		showCommand = SW_SHOW;
	}
	else
	{
		showCommand = SW_HIDE;
	}
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
	for (size_t i = 0; i < m_lineChecks.size(); i++)
	{
		ShowWindow(m_lineChecks[i], showCommand);
	}
	//positionResizeGrip(m_hResizeGrip);
}

void ModelTreeDialog::swapWindowText(UCCHAR oldChar, UCCHAR newChar)
{
	ucstring text;

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

void ModelTreeDialog::addToolToTooltip(int controlId, const ucstring& tooltipText)
{
	HWND hControl = GetDlgItem(hWindow, controlId);
	TOOLINFOUC ti;

	memset(&ti, 0, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = hWindow;
	ti.uId = (UINT_PTR)hControl;
	ucstring tooltipTextCopy = tooltipText;
	ti.lpszText = &tooltipTextCopy[0];
	::SendMessage(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL ModelTreeDialog::doInitDialog(HWND hKbControl)
{
	RECT optionsRect;
	RECT clientRect;

	m_colorButton = new CUIColorButton(GetDlgItem(hWindow,
		IDC_HIGHLIGHT_COLOR), MODEL_TREE_CUST_COLORS_KEY, m_highlightR,
		m_highlightG, m_highlightB);
	setIcon(IDI_APP_ICON);
	m_hTreeView = GetDlgItem(hWindow, IDC_MODEL_TREE);

	m_hTooltip = CUIWindow::createWindowExUC(0, TOOLTIPS_CLASSUC,
		NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /*| TTS_BALLOON*/,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hWindow, NULL, hInstance, NULL);
	SetWindowPos(m_hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	if (haveWindowsVistaOrLater())
	{
		std::wstring magnifier;
		if (haveWindows7OrLater())
		{
			// Windows 7 is the first version to support emojis.
			// Note: EM_SETCUEBANNER always expects Unicode, so having a hard-coded Unicode
			// magnifier works as long as the Windows version support emojis.
			magnifier = L"\U0001F50D ";
		}
		std::wstring cue = magnifier + ls(L"Search");
		SendDlgItemMessage(hWindow, IDC_SEARCH, EM_SETCUEBANNER, TRUE, (LPARAM)cue.c_str());
	}
	addToolToTooltip(IDC_PREV, ls(_UC("Previous")));
	addToolToTooltip(IDC_NEXT, ls(_UC("Next")));

	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(IDC_SEARCH, CUISizeHorizontal | CUIFloatBottom);
	m_resizer->addSubWindow(IDC_PREV, CUIFloatLeft | CUIFloatBottom);
	m_resizer->addSubWindow(IDC_NEXT, CUIFloatLeft | CUIFloatBottom);
	m_resizer->addSubWindow(IDC_MODEL_TREE,
		CUISizeHorizontal | CUISizeVertical);
	m_resizer->addSubWindow(IDC_SHOW_BOX, CUIFloatLeft | CUIFloatBottom);
	m_resizer->addSubWindow(IDC_OPTIONS, CUIFloatLeft | CUIFloatTop);
	m_resizer->addSubWindow(IDC_HIGHLIGHT, CUIFloatRight | CUIFloatTop);
	m_resizer->addSubWindow(IDC_HIGHLIGHT_COLOR, CUIFloatRight | CUIFloatTop);
	m_lineChecks.resize(LDLLineTypeUnknown + 1);
	setupLineCheck(IDC_COMMENT, LDLLineTypeComment);
	setupLineCheck(IDC_MODEL, LDLLineTypeModel);
	setupLineCheck(IDC_LINE, LDLLineTypeLine);
	setupLineCheck(IDC_TRIANGLE, LDLLineTypeTriangle);
	setupLineCheck(IDC_QUAD, LDLLineTypeQuad);
	setupLineCheck(IDC_CONDITIONAL, LDLLineTypeConditionalLine);
	setupLineCheck(IDC_EMPTY, LDLLineTypeEmpty);
	setupLineCheck(IDC_UNKNOWN, LDLLineTypeUnknown);
	GetWindowRect(GetDlgItem(hWindow, IDC_SHOW_BOX), &optionsRect);
	screenToClient(hWindow, &optionsRect);
	GetClientRect(hWindow, &clientRect);
	m_optionsDelta = clientRect.right - optionsRect.left;
	setMinSize(clientRect.right, clientRect.bottom, true);
	if (!TCUserDefaults::boolForKey(MODEL_TREE_OPTIONS_SHOWN_KEY, true, false))
	{
		hideOptions();
	}
	checkSet(IDC_HIGHLIGHT, m_highlight);
	initStatusBar();
	setAutosaveName("ModelTreeDialog");
	return CUIDialog::doInitDialog(hKbControl);
}

void ModelTreeDialog::updateStatusText(void)
{
	HTREEITEM hItem = TreeView_GetSelection(m_hTreeView);
	LDModelTree *tree = getItemTree(hItem);

	if (tree != NULL)
	{
		statusBarSetText(m_hStatus, 0, tree->getStatusText());
		return;
	}
	statusBarSetText(m_hStatus, 0, ls(_UC("NoSelection")));
}

void ModelTreeDialog::initStatusBar(void)
{
	int parts[] = {-1};

	m_hStatus = ::CreateWindow(STATUSCLASSNAME, _UC(""),
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 10, 10, hWindow, NULL,
		hInstance, 0);
	statusBarSetParts(m_hStatus, 1, parts);
	updateStatusText();
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
		//positionResizeGrip(m_hResizeGrip, newWidth, newHeight);
		SendMessage(m_hStatus, WM_SIZE, sizeType,
			MAKELPARAM(newWidth, newHeight));
	}
	return CUIDialog::doSize(sizeType, newWidth, newHeight);
}

LRESULT ModelTreeDialog::doTreeCustomDraw(LPNMTVCUSTOMDRAW notification)
{
	if (notification->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		SetWindowLongPtr(hWindow, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
		return TRUE;
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
			SetWindowLongPtr(hWindow, DWLP_MSGRESULT, CDRF_DODEFAULT);
			return TRUE;
		}
	}
	return FALSE;
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
	LDModelTree *tree = getItemTree(hItem);

	if (tree != NULL)
	{
		std::string text = tree->getText();

		if (tree->getLineType() == LDLLineTypeEmpty)
		{
			text = "";
		}
		text += "\r\n";
		if (copyToClipboard(text.c_str()))
		{
			SetWindowLongPtr(hWindow, DWLP_MSGRESULT, TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

void ModelTreeDialog::doOK(void)
{
	// Don't do anything.
}
