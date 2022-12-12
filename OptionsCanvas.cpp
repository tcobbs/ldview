#include "OptionsCanvas.h"
#include "OptionsScroller.h"
#include "ExportOptionsDialog.h"
#include "Resource.h"
#include "BoolOptionUI.h"
#include "GroupOptionUI.h"
#include "LongOptionUI.h"
#include "FloatOptionUI.h"
#include "StringOptionUI.h"
#include "PathOptionUI.h"
#include "EnumOptionUI.h"
#include <TCFoundation/TCLocalStrings.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

OptionsCanvas::OptionsCanvas(HINSTANCE hInstance)
	: CUIDialog(hInstance)
	, m_parent(NULL)
	, m_spacing(0)
	, m_margin(0)
{
}

OptionsCanvas::~OptionsCanvas(void)
{
}

void OptionsCanvas::dealloc(void)
{
	for (OptionUIList::iterator it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		delete *it;
	}
	CUIDialog::dealloc();
}

void OptionsCanvas::create(OptionsScroller *parent)
{
	m_parent = parent;
	// Note: as mentioned in the class description, this dialog resource is for
	// an empty window.  All controls are added programmatically.
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OPTIONS_CANVAS),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

void OptionsCanvas::addGroup(LDExporterSetting &setting)
{
	// Note: groups are only used for top-level groupings.  Nested groupings
	// just use a check box (BoolOptionUI).
	m_optionUIs.push_back(new GroupOptionUI(this, setting, m_spacing));
}

void OptionsCanvas::addBoolSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new BoolOptionUI(this, setting));
}

void OptionsCanvas::addFloatSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new FloatOptionUI(this, setting));
}

void OptionsCanvas::addLongSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new LongOptionUI(this, setting));
}

void OptionsCanvas::addStringSetting(LDExporterSetting &setting)
{
	if (setting.isPath())
	{
		// Paths go to TCUserDefault via different functions, and they also have
		// a browse button, which strings lack.
		m_optionUIs.push_back(new PathOptionUI(this, setting));
	}
	else
	{
		m_optionUIs.push_back(new StringOptionUI(this, setting));
	}
}

void OptionsCanvas::addEnumSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new EnumOptionUI(this, setting));
}

BOOL OptionsCanvas::doInitDialog(HWND /*hKbControl*/)
{
	RECT spacingRect = { 4, 3, 0, 0 };

	// Try to make it look right no matter what DPI setting is set in Windows.
	MapDialogRect(hWindow, &spacingRect);
	m_spacing = spacingRect.top;
	m_margin = spacingRect.left;
	return TRUE;
}

// Calculates the height of a single option, or a group of options.  If the
// setting has a group size > 0, this calculates the heights of all settings in
// the group (recursively calling itself for each one).  This should allow for
// arbitrarily deep group nesting but has only been tested with one level of
// groups nested inside other groups.
void OptionsCanvas::calcOptionHeight(
	HDC lhdc,
	OptionUIList::iterator &it,
	int &ly,
	int lwidth,
	int leftMargin,
	int rightMargin,
	int numberWidth,
	int &optimalWidth,
	bool update,
	bool enabled)
{
	OptionUI *optionUI = *it;
	LDExporterSetting *setting = optionUI->getSetting();
	LDExporterSetting::Type type = setting->getType();
	int otherWidth = 0;

	// Add the height of this option to our running total y value.  If update is
	// true, this will also move the controls inside the option.
	if (type == LDExporterSetting::TLong ||
		type == LDExporterSetting::TFloat)
	{
		// Float and long settings have an edit control to the right of static
		// text.  We want all these edit controls to line up, so optimalWidth is
		// used here, and we do two passes at control layout.  The first pass is
		// essentially just to figure out the optimalWidth value, but it's much
		// easier to re-run the same code with one setting changed than to
		// reimplement a large portion of it.  At the end of the first pass,
		// optimalWidth will contain the width of the widest float/long label
		// text.  During the second pass, all these labels will be made to be
		// this width, and the edit controls will be just to the right of that.
		ly += optionUI->updateLayout(lhdc, m_margin + leftMargin, ly,
			numberWidth - leftMargin - rightMargin, update, optimalWidth) +
			m_spacing;
	}
	else
	{
		// We don't actually care about otherWidth here, but we have to pass an
		// int ref in, and it has to be a different variable from optimalWidth.
		ly += optionUI->updateLayout(lhdc, m_margin + leftMargin, ly,
			lwidth - leftMargin - rightMargin, update, otherWidth) +
			m_spacing;
	}
	// If we're in a group, and the group has a check box that is unchecked,
	// enabled will be false.  Otherwise it will be true.
	optionUI->setEnabled(enabled);
	if (setting->getGroupSize() > 0)
	{
		int groupLeftMargin;
		int groupRightMargin = optionUI->getRightGroupMargin();

		// Note that enabled below is only used for children of this group.
		// Yes, we're updating a passed in parameter, but it's not passed by
		// reference.
		enabled = optionUI->getEnabled();
		groupLeftMargin = optionUI->getLeftGroupMargin();
		leftMargin += groupLeftMargin;
		rightMargin += groupRightMargin;
		// Since we've indented, we now have less space available for the label
		// text.
		optimalWidth -= groupLeftMargin + groupRightMargin;
		for (int i = 0; i < setting->getGroupSize(); i++)
		{
			// Note: the it parameter is a ref to an iterator, so we're actually
			// updating the iterator used by our caller.  This is very strange,
			// but does what we want, since we want to process this whole group
			// and have our caller not process it.  The strangeness is brought
			// on by the fact that we're storing a nested data structure in a
			// linear list.
			it++;
			if (it == m_optionUIs.end())
			{
				// Needless to say, this shouldn't ever happen.
				throw "Group size bigger than total number of settings left.";
			}
			// Recursive call to this same function.  Note that everything that
			// is passed by reference (it, ly, and optimalWidth) may be updated
			// in the recursive call.
			calcOptionHeight(lhdc, it, ly, lwidth, leftMargin, rightMargin,
				numberWidth, optimalWidth, update, enabled);
		}
		if (optionUI->getBottomGroupMargin() > 0)
		{
			// The bottom margin is the space needed for the line on the bottom
			// of the group box.  Nested groups don't have that line, so have
			// a bottom margin of 0.  Additionally, since nested groups don't
			// have a box around them, there's no need for them to know how tall
			// the group as a whole is.
			if (update)
			{
				// This sizes the group box to enclose all its options.
				((GroupOptionUI *)optionUI)->close(ly - m_spacing);
			}
			// This inserts the space for the bottom of the group box.
			ly += optionUI->getBottomGroupMargin();
		}
		// We're done with this indent level, so adjust optimalWidth to be
		// appropriate for the calling indent level.
		optimalWidth += groupLeftMargin + groupRightMargin;
	}
}

// If update is false, this calculates the height of all the child controls, as
// well as the optimal width for the labels of float/long options.
int OptionsCanvas::calcHeight(
	int newWidth,
	int &optimalWidth,
	bool update /*= false*/)
{
	// Make space for the top margin.
	int ly = m_margin;
	// Make space for the left and right margins.
	int lwidth = newWidth - m_margin * 2;
	int numberWidth = lwidth;
	HDC lhdc = GetDC(hWindow);
	HFONT hNewFont = (HFONT)SendMessage(hWindow, WM_GETFONT, 0, 0);
	HFONT hOldFont = (HFONT)SelectObject(lhdc, hNewFont);

	if (update)
	{
		// On the second pass (update==true), we know how much space the
		// float/long options will take: optimalWidth.  So that's what we're
		// giving them.
		numberWidth = optimalWidth;
	}
	for (OptionUIList::iterator it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		// Note that if the given option is a group, calcOptionHeight will
		// recursively process all items in that group.
		calcOptionHeight(lhdc, it, ly, lwidth, 0, 0, numberWidth, optimalWidth,
			update, true);
	}
	SelectObject(lhdc, hOldFont);
	ReleaseDC(hWindow, lhdc);
	return ly;
}

LRESULT OptionsCanvas::doSize(
	WPARAM /*sizeType*/,
	int newWidth,
	int /*newHeight*/)
{
	int optimalWidth = 0;
	// Pass 1: calculate the appropriate value for optimalWidth, the needed
	// widht for the widest float/long option.
	calcHeight(newWidth, optimalWidth, false);
	// Pass 2: update the locations of all of the controls.
	calcHeight(newWidth, optimalWidth, true);
	return 0;
}

bool OptionsCanvas::commitSettings(void)
{
	ucstring error;
	OptionUIList::iterator it;

	// First, walk through all settings and validate them.  If any of the
	// validations fails, stop and return false.  That means that if there are
	// any validation failures, the settings before the failure won't have their
	// values updated.
	for (it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		OptionUI *optionUI = *it;

		if (!optionUI->validate(error))
		{
			if (error.size() > 0)
			{
				messageBoxUC(hWindow, error.c_str(), ls(_UC("Error")), MB_OK);
			}
			return false;
		}
	}
	// If we get here, validation succeeded, so save all the option values.
	for (it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		OptionUI *optionUI = *it;

		optionUI->commit();
	}
	return true;
}

void OptionsCanvas::updateEnabled(void)
{
	RECT rect;

	GetClientRect(hWindow, &rect);
	// We're not actually resizing, but since the resize code properly handles
	// the enable/disable, that's the easiest way to update it.  No, it's not
	// very efficient, but it's not slow enough for the user to notice.
	doSize(SIZE_RESTORED, rect.right, rect.bottom);
	// We didn't actually resize, and the resize code doesn't update the child
	// windows as it goes, so force a redraw (erase background not needed).
	RedrawWindow(hWindow, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

LRESULT OptionsCanvas::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	switch (notifyCode)
	{
	case EN_SETFOCUS:
	case BN_SETFOCUS:
	case CBN_SETFOCUS:
		m_parent->scrollControlToVisible(control);
		break;
	case BN_CLICKED:
		{
			OptionUI *optionUI =
				(OptionUI *)GetWindowLongPtr(control, GWLP_USERDATA);

			if (optionUI != NULL)
			{
				optionUI->doClick(control);
			}
		}
		break;
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void OptionsCanvas::resetSettings(void)
{
	for (OptionUIList::iterator it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		(*it)->reset();
	}
	updateEnabled();
}
