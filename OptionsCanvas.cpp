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
#include <TCFoundation/TCLocalStrings.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

OptionsCanvas::OptionsCanvas(HINSTANCE hInstance):
CUIDialog(hInstance)
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
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OPTIONS_CANVAS),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

void OptionsCanvas::addGroup(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new GroupOptionUI(this, setting));
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
		m_optionUIs.push_back(new PathOptionUI(this, setting));
	}
	else
	{
		m_optionUIs.push_back(new StringOptionUI(this, setting));
	}
}

BOOL OptionsCanvas::doInitDialog(HWND /*hKbControl*/)
{
	RECT spacingRect = { 4, 3, 0, 0 };

	MapDialogRect(hWindow, &spacingRect);
	m_spacing = spacingRect.top;
	m_margin = spacingRect.left;
	return TRUE;
}

void OptionsCanvas::closeGroup(
	GroupOptionUI *&currentGroup,
	int &y,
	int &optimalWidth,
	int &leftMargin,
	int &rightMargin,
	int &numberWidth,
	int spacing,
	bool &enabled,
	bool update)
{
	if (update)
	{
		currentGroup->close(y - spacing);
	}
	y += currentGroup->getBottomMargin();
	optimalWidth += leftMargin;
	numberWidth -= leftMargin;
	leftMargin = 0;
	rightMargin = 0;
	enabled = true;
	currentGroup = NULL;
}

int OptionsCanvas::calcHeight(
	int newWidth,
	int &optimalWidth,
	bool update /*= false*/)
{
	int y = m_margin;
	HDC hdc = GetDC(hWindow);
	int width = newWidth - m_margin * 2;
	HFONT hNewFont = (HFONT)SendMessage(hWindow, WM_GETFONT, 0, 0);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hNewFont);
	int otherWidth = 0;
	int numberWidth = width;
	GroupOptionUI *currentGroup = NULL;
	int groupCount = 0;
	int leftMargin = 0;
	int rightMargin = 0;
	bool enabled = true;

	if (update)
	{
		numberWidth = optimalWidth;
	}
	for (OptionUIList::iterator it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		OptionUI *optionUI = *it;
		LDExporterSetting *setting = optionUI->getSetting();
		LDExporterSetting::Type type = setting->getType();

		if (groupCount > 0 && setting->getGroupSize() > 0)
		{
			// Groups can't be nested, so if we see a group when we're not done
			// with a previous group, just finish the previous one anyway.
			closeGroup(currentGroup, y, optimalWidth, leftMargin,
				rightMargin, numberWidth, m_spacing, enabled, update);
			groupCount = 0;
		}
		if (type == LDExporterSetting::TLong ||
			type == LDExporterSetting::TFloat)
		{
			y += optionUI->updateLayout(hdc, m_margin + leftMargin, y,
				numberWidth - leftMargin - rightMargin, update, optimalWidth) +
				m_spacing;
		}
		else
		{
			y += optionUI->updateLayout(hdc, m_margin + leftMargin, y,
				width - leftMargin - rightMargin, update, otherWidth) +
				m_spacing;
		}
		optionUI->setEnabled(enabled);
		if (currentGroup)
		{
			groupCount--;
			if (groupCount == 0)
			{
				closeGroup(currentGroup, y, optimalWidth, leftMargin,
					rightMargin, numberWidth, m_spacing, enabled, update);
			}
		}
		if (setting->getGroupSize() > 0)
		{
			currentGroup = (GroupOptionUI *)optionUI;
			groupCount = setting->getGroupSize();
			leftMargin = currentGroup->getLeftMargin();
			rightMargin = currentGroup->getRightMargin();
			optimalWidth -= leftMargin;
			numberWidth += leftMargin;
			enabled = currentGroup->getEnabled();
		}
	}
	SelectObject(hdc, hOldFont);
	ReleaseDC(hWindow, hdc);
	return y;
}

LRESULT OptionsCanvas::doSize(
	WPARAM /*sizeType*/,
	int newWidth,
	int /*newHeight*/)
{
	int optimalWidth = 0;
	calcHeight(newWidth, optimalWidth, false);
	calcHeight(newWidth, optimalWidth, true);
	return 0;
}

bool OptionsCanvas::commitSettings(void)
{
	ucstring error;
	OptionUIList::iterator it;

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
	doSize(SIZE_RESTORED, rect.right, rect.bottom);
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
