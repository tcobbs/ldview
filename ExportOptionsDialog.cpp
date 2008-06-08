#include "ExportOptionsDialog.h"
#include "OptionsCanvas.h"
#include "OptionsScroller.h"
#include "Resource.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

ExportOptionsDialog::ExportOptionsDialog(
	HINSTANCE hInstance,
	LDExporter *exporter):
CUIDialog(hInstance, NULL),
m_exporter(exporter),
m_resizer(NULL),
m_scroller(NULL),
m_canvas(NULL)
{
}

ExportOptionsDialog::~ExportOptionsDialog(void)
{
}

void ExportOptionsDialog::dealloc(void)
{
	TCObject::release(m_resizer);
	TCObject::release(m_scroller);
	TCObject::release(m_canvas);
	CUIDialog::dealloc();
}

INT_PTR ExportOptionsDialog::doModal(HWND hWndParent)
{
	return CUIDialog::doModal(IDD_EXPORT_OPTIONS, hWndParent);
}

//void ExportOptionsDialog::addRow(LDExporterSetting &setting, LVITEM &item)
//{
//	const ucstring &ucname = setting.getName();
//
//	memset(&item, 0, sizeof(item));
//	item.mask = TVIF_TEXT;
//	item.pszText = ucstringtombs(ucname.c_str(), ucname.size());
//	item.iItem = ListView_GetItemCount(hOptionsList);
//	item.iSubItem = 0;
//	ListView_InsertItem(hOptionsList, &item);
//	delete item.pszText;
//	item.iSubItem = 0;
//	item.mask = TVIF_IMAGE;
//	item.iImage = -1;
//	ListView_SetItem(hOptionsList, &item);
//}
//
//void ExportOptionsDialog::addBoolSetting(LDExporterSetting &setting)
//{
//	LVITEM item;
//
//	addRow(setting, item);
//	item.iSubItem = 1;
//	item.mask = TVIF_IMAGE;
//	item.iImage = setting.getBoolValue() ? 2 : 1;
//	ListView_SetItem(hOptionsList, &item);
//	ListView_SetCheckState(hOptionsList, item.iItem,
//		setting.getBoolValue() ? TRUE : FALSE);
//}
//
//void ExportOptionsDialog::addFloatSetting(LDExporterSetting &setting)
//{
//	LVITEM item;
//
//	addRow(setting, item);
//	ListView_SetItemState(hOptionsList, item.iItem, 0, LVIS_STATEIMAGEMASK);
//}
//
//void ExportOptionsDialog::addLongSetting(LDExporterSetting &setting)
//{
//	LVITEM item;
//
//	addRow(setting, item);
//}
//
//void ExportOptionsDialog::initOptionsList(void)
//{
//	LDExporterSettingList::iterator it;
//	LDExporterSettingList &settings = m_exporter->getSettings();
//	LVCOLUMN column;
//	DWORD exStyle = LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES;
//	RECT rect;
//	int valueWidth = 100;
//	HIMAGELIST hStateImageList = ImageList_LoadImage(getLanguageModule(),
//		MAKEINTRESOURCE(IDB_TOOLBAR), 16, 0, RGB(255, 0, 254), IMAGE_BITMAP,
//		LR_DEFAULTCOLOR);
//
//	SendMessage(hOptionsList, LVM_SETEXTENDEDLISTVIEWSTYLE, exStyle, exStyle);
//	ListView_SetImageList(hOptionsList, hStateImageList, LVSIL_SMALL);
//	GetClientRect(hOptionsList, &rect);
//	memset(&column, 0, sizeof(LVCOLUMN));
//	column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
//	column.fmt = LVCFMT_RIGHT | LVCFMT_BITMAP_ON_RIGHT;
//	column.cx = rect.right - valueWidth;
//	column.pszText = copyString(ls("Option"));
//	// Columns inserted at index 0 are always left-aligned.  So, as per the
//	// help file, insert a dummy column here, then delete it after inserting
//	// all columns.
//	ListView_InsertColumn(hOptionsList, 0, &column);
//	//ListView_InsertColumn(hOptionsList, 1, &column);
//	//ListView_DeleteColumn(hOptionsList, 0);
//	delete column.pszText;
//	column.mask |= LVCF_SUBITEM;
//	column.fmt = LVCFMT_LEFT;
//	column.cx = valueWidth;
//	column.iSubItem = 1;
//	column.pszText = copyString(ls("Value"));
//	ListView_InsertColumn(hOptionsList, 1, &column);
//	delete column.pszText;
//	for (it = settings.begin(); it != settings.end(); it++)
//	{
//		switch (it->getType())
//		{
//		case LDExporterSetting::TBool:
//			addBoolSetting(*it);
//			break;
//		case LDExporterSetting::TFloat:
//			addFloatSetting(*it);
//			break;
//		case LDExporterSetting::TLong:
//			addLongSetting(*it);
//			break;
//		default:
//			throw "not implemented";
//		}
//	}
//}

void ExportOptionsDialog::initCanvasOptions(void)
{
	LDExporterSettingList &settings = m_exporter->getSettings();
	LDExporterSettingList::iterator it;

	for (it = settings.begin(); it != settings.end(); it++)
	{
		if (it->getGroupSize() > 0)
		{
			m_canvas->addGroup(*it);
		}
		else
		{
			switch (it->getType())
			{
			case LDExporterSetting::TBool:
				m_canvas->addBoolSetting(*it);
				break;
			case LDExporterSetting::TFloat:
				m_canvas->addFloatSetting(*it);
				break;
			case LDExporterSetting::TLong:
				m_canvas->addLongSetting(*it);
				break;
			default:
				throw "not implemented";
			}
		}
	}
}

void ExportOptionsDialog::initCanvas(void)
{
	HWND hCanvasLoc = GetDlgItem(hWindow, IDC_OPTIONS_CANVAS);
	HWND hScroller;
	RECT canvasRect;

	GetWindowRect(hCanvasLoc, &canvasRect);
	DestroyWindow(hCanvasLoc);
	screenToClient(hWindow, &canvasRect);
	m_scroller = new OptionsScroller(hInstance);
	m_scroller->create(this);
	hScroller = m_scroller->getHWindow();
	m_canvas = m_scroller->getCanvas();
	m_canvas->retain();
	initCanvasOptions();
	MoveWindow(hScroller, canvasRect.left, canvasRect.top,
		canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top,
		TRUE);
	ShowWindow(hScroller, SW_SHOW);
}

BOOL ExportOptionsDialog::doInitDialog(HWND hKbControl)
{
	std::string titleBase;
	std::string extension = m_exporter->getExtension();
	std::string title;
	RECT rect;
	HWND hScroller;

	GetClientRect(hWindow, &rect);
	minWidth = rect.right / 2;
	minHeight = rect.bottom / 2;
	setIcon(IDI_APP_ICON);
	titleBase.resize(SendMessageA(hWindow, WM_GETTEXTLENGTH, 0, 0));
	SendMessageA(hWindow, WM_GETTEXT, (WPARAM)titleBase.size() + 1,
		(LPARAM)&titleBase[0]);
	convertStringToUpper(&extension[0]);
	title = extension + " " + titleBase;
	SendMessageA(hWindow, WM_SETTEXT, 0, (LPARAM)&title[0]);
	initCanvas();
	hScroller = m_scroller->getHWindow();
	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(hScroller, CUISizeHorizontal | CUISizeVertical);
	m_resizer->addSubWindow(IDOK, CUIFloatLeft | CUIFloatTop);
	m_resizer->addSubWindow(IDCANCEL, CUIFloatLeft | CUIFloatTop);
	attachResizeGrip(IDC_RESIZEGRIP, m_resizer);
	initialized = TRUE;
	setAutosaveName((extension + "ExportOptions").c_str());
	return CUIDialog::doInitDialog(hKbControl);
}

void ExportOptionsDialog::doOK(void)
{
	if (m_canvas->commitSettings())
	{
		CUIDialog::doOK();
	}
}

LRESULT ExportOptionsDialog::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (sizeType != SIZE_MINIMIZED)
	{
		m_resizer->resize(newWidth, newHeight);
	}
	return CUIDialog::doSize(sizeType, newWidth, newHeight);
}

LRESULT ExportOptionsDialog::doBeginLabelEdit(NMLVDISPINFO * /*notification*/)
{
	return FALSE;
}

LRESULT ExportOptionsDialog::doEndLabelEdit(NMLVDISPINFO * /*notification*/)
{
	return FALSE;
}

//LRESULT ExportOptionsDialog::doNotify(int controlId, LPNMHDR notification)
//{
//	if (controlId == IDC_OPTIONS_LIST)
//	{
//		switch (notification->code)
//		{
//		case LVN_BEGINLABELEDIT:
//			return doBeginLabelEdit((NMLVDISPINFO *)notification);
//		case LVN_ENDLABELEDIT:
//			return doEndLabelEdit((NMLVDISPINFO *)notification);
//		}
//	}
//	return 1;
//}

LRESULT ExportOptionsDialog::doMouseWheel(
	short keyFlags,
	short zDelta,
	int xPos,
	int yPos)
{
	RECT rect;
	POINT point = { xPos, yPos };

	GetWindowRect(m_scroller->getHWindow(), &rect);
	if (PtInRect(&rect, point))
	{
		m_scroller->doMouseWheel(keyFlags, zDelta, xPos, yPos);
	}
	return 0;
}
