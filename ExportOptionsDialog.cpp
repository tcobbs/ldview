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
	// All the groupSize stuff here is done to prevent group settings from
	// being nested.  Nested groups get bool settings.
	std::stack<int> groupSizes;
	int groupSize = 0;

	for (it = settings.begin(); it != settings.end(); it++)
	{
		// Track whether we were inside a group at the beginning of this
		// iteration of the loop.
		bool inGroup = groupSize > 0;

		if (groupSize > 0)
		{
			groupSize--;
			if (groupSize == 0)
			{
				// We just got to the end of a group, so pop the previous
				// groupSize value off the groupSizes stack.
				groupSize = groupSizes.top();
				groupSizes.pop();
			}
		}
		if (it->getGroupSize() > 0)
		{
			// This item is the start of a group.
			if (inGroup)
			{
				// At the beginning of this iteration we were in a group, so
				// use a bool setting instead of a group setting.
				m_canvas->addBoolSetting(*it);
			}
			else
			{
				// Top level group; use a group setting.
				m_canvas->addGroup(*it);
			}
			// We're now in a new group, so push the current groupSize onto
			// the groupSizes stack.
			groupSizes.push(groupSize);
			// Update groupSize based on the new group's size.
			groupSize = it->getGroupSize();
		}
		else
		{
			// This setting isn't the start of a group; add the appropriate type
			// of option UI to the canvas.
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
			case LDExporterSetting::TEnum:
				m_canvas->addEnumSetting(*it);
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
	ucstring titleBase;
	std::string extension = m_exporter->getExtension();
	ucstring title;
	RECT rect;
	HWND hScroller;

	GetClientRect(hWindow, &rect);
	setMinSize(rect.right, rect.bottom, true);
	setIcon(IDI_APP_ICON);
	CUIWindow::windowGetText(hWindow, titleBase);
	convertStringToUpper(&extension[0]);
	utf8toucstring(title, extension);
	title += _UC(" ");
	title += titleBase;
	windowSetText(hWindow, title);
	initCanvas();
	hScroller = m_scroller->getHWindow();
	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(hScroller, CUISizeHorizontal | CUISizeVertical);
	m_resizer->addSubWindow(IDC_RESET, CUIFloatRight | CUIFloatTop);
	m_resizer->addSubWindow(IDOK, CUIFloatLeft | CUIFloatTop);
	m_resizer->addSubWindow(IDCANCEL, CUIFloatLeft | CUIFloatTop);
	m_resizer->addResizeGrip();
	initialized = TRUE;
	// Keep a different autosave setting for each export file type.
	setAutosaveName((extension + "ExportOptions").c_str());
	return CUIDialog::doInitDialog(hKbControl);
}

void ExportOptionsDialog::doOK(void)
{
	// If there is a validation error, commitSettings will show the error to the
	// user and return false.
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
		// I don't know WHY we get the mouse wheel event istead of the scroller
		// window (which is set up to handle it), but we do.  It's probably
		// related to the dialog event forwarding for keyboard navigation.
		m_scroller->doMouseWheel(keyFlags, zDelta, xPos, yPos);
	}
	return 0;
}

LRESULT ExportOptionsDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (notifyCode == BN_SETFOCUS)
	{
		UCCHAR className[128];

		GetClassName(control, className, COUNT_OF(className));
		if (ucstrcmp(className, WC_BUTTON) == 0)
		{
			// For some reason, when the focus changes betwen the OK and Cancel
			// buttons, the group box borders in the canvas get redrawn without
			// redrawing the labels or check boxes that are over the top of
			// them.  This causes there to be a line through their titles.  I
			// don't have any idea why this happens, but redrawing the canvas
			// and all its children fixes the problem.
			RedrawWindow(m_canvas->getHWindow(), NULL, NULL,
				RDW_INVALIDATE | RDW_ALLCHILDREN);
		}
	}
	else if (notifyCode == BN_CLICKED)
	{
		if (commandId == IDC_RESET)
		{
			return doReset();
		}
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

LRESULT ExportOptionsDialog::doReset(void)
{
	m_canvas->resetSettings();
	return 0;
}
