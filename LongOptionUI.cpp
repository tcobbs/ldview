#include "LongOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

LongOptionUI::LongOptionUI(HWND hParentWnd, LDExporterSetting &setting):
NumberOptionUI(hParentWnd, setting, ltoucstr(setting.getLongValue()).c_str())
{
}

bool LongOptionUI::validate(ucstring &error)
{
	ucstring text;
	long value;

	CUIWindow::windowGetText(m_hEdit, text);
	if (sucscanf(text.c_str(), _UC("%ld"), &value) == 1)
	{
		if (m_setting->hasMin() && value < m_setting->getMinLongValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMinLongValue()).c_str());
			error = _UC("Value too small");
		}
		else if (m_setting->hasMax() && value > m_setting->getMaxLongValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMaxLongValue()).c_str());
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
		error = _UC("Integer required");
	}
	SendMessage(m_hEdit, EM_SETSEL, 0, -1);
	SetFocus(m_hEdit);
	return false;
}

void LongOptionUI::commit(void)
{
	m_setting->setValue(m_value, true);
}
