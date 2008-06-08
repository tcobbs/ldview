#include "FloatOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

FloatOptionUI::FloatOptionUI(OptionsCanvas *parent, LDExporterSetting &setting):
NumberOptionUI(parent, setting, setting.getStringValue().c_str())
{
}

bool FloatOptionUI::validate(ucstring &error)
{
	ucstring text;
	TCFloat value;

	CUIWindow::windowGetText(m_hEdit, text);
	if (sucscanf(text.c_str(), _UC("%f"), &value) == 1)
	{
		ucstring format;
		ucstring buf;

		if (m_setting->hasMin() && value < m_setting->getMinFloatValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMinFloatValue()).c_str());
		}
		else if (m_setting->hasMax() && value > m_setting->getMaxFloatValue())
		{
			SetWindowText(m_hEdit,
				ftostr(m_setting->getMaxFloatValue()).c_str());
		}
		else
		{
			m_value = value;
			return true;
		}
		format = TCObject::ls(_UC("ValueOutOfRange"));
		buf.reserve(format.size() + 128);
		sucprintf(&buf[0], buf.capacity() + 1, format.c_str(),
			ftoucstr(m_setting->getMinFloatValue()).c_str(),
			ftoucstr(m_setting->getMaxFloatValue()).c_str());
		error = buf.c_str();
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
