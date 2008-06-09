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
			case LDExporterSetting::TString:
				m_canvas->addStringSetting(*it);
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
	CUIWindow::windowGetText(hWindow, titleBase);
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
