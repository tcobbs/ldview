#include "OptionsCanvas.h"
#include "ExportOptionsDialog.h"
#include "Resource.h"
#include "BoolOptionUI.h"
#include "LongOptionUI.h"
#include "FloatOptionUI.h"
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

void OptionsCanvas::create(CUIWindow *parent)
{
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_OPTIONS_CANVAS),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

void OptionsCanvas::addBoolSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new BoolOptionUI(hWindow, setting));
}

void OptionsCanvas::addFloatSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new FloatOptionUI(hWindow, setting));
}

void OptionsCanvas::addLongSetting(LDExporterSetting &setting)
{
	m_optionUIs.push_back(new LongOptionUI(hWindow, setting));
}

BOOL OptionsCanvas::doInitDialog(HWND /*hKbControl*/)
{
	return TRUE;
}

int OptionsCanvas::calcHeight(int newWidth, bool update /*= false*/)
{
	int margin = 6;
	int spacing = 4;
	int y = margin;
	HDC hdc = GetDC(hWindow);
	int width = newWidth - margin * 2;
	HFONT hNewFont = (HFONT)SendMessage(hWindow, WM_GETFONT, 0, 0);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hNewFont);

	for (OptionUIList::iterator it = m_optionUIs.begin();
		it != m_optionUIs.end(); it++)
	{
		OptionUI *optionUI = *it;

		y += optionUI->updateLayout(hdc, margin, y, width, update) + spacing;
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
	calcHeight(newWidth, true);
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
