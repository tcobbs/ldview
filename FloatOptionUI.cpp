#include "FloatOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

FloatOptionUI::FloatOptionUI(HWND hParentWnd, LDExporterSetting &setting):
NumberOptionUI(hParentWnd, setting, ftoucstr(setting.getFloatValue()).c_str())
{
}

bool FloatOptionUI::validate(ucstring &error)
{
	ucstring text;
	TCFloat value;

	CUIWindow::windowGetText(m_hEdit, text);
	if (sucscanf(text.c_str(), _UC("%f"), &value) == 1)
	{
		if (m_setting->hasMin() && value < m_setting->getMinFloatValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMinFloatValue()).c_str());
			error = _UC("Value too small");
		}
		else if (m_setting->hasMax() && value > m_setting->getMaxFloatValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMaxFloatValue()).c_str());
			error = _UC("Value too large");
		}
		else
		{
			m_value = value;
			return true;
		}
	}
	else
	{
		error = _UC("Decimal number required");
	}
	SendMessage(m_hEdit, EM_SETSEL, 0, -1);
	SetFocus(m_hEdit);
	return false;
}

void FloatOptionUI::commit(void)
{
	m_setting->setValue(m_value, true);
}
