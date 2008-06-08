#include "LongOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

LongOptionUI::LongOptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
NumberOptionUI(parent, setting, setting.getStringValue().c_str())
{
}

bool LongOptionUI::validate(ucstring &error)
{
	ucstring text;
	long value;

	CUIWindow::windowGetText(m_hEdit, text);
	if (sucscanf(text.c_str(), _UC("%ld"), &value) == 1)
	{
		ucstring format;
		ucstring buf;

		if (m_setting->hasMin() && value < m_setting->getMinLongValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMinLongValue()).c_str());
		}
		else if (m_setting->hasMax() && value > m_setting->getMaxLongValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMaxLongValue()).c_str());
		}
		else
		{
			m_value = value;
			return true;
		}
		format = TCObject::ls(_UC("ValueOutOfRange"));
		buf.reserve(format.size() + 128);
		sucprintf(&buf[0], buf.capacity() + 1, format.c_str(),
			ltoucstr(m_setting->getMinLongValue()).c_str(),
			ltoucstr(m_setting->getMaxLongValue()).c_str());
		error = buf.c_str();
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
