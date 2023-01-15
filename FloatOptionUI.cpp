#include "FloatOptionUI.h"
#include <CUI/CUIWindow.h>
#include <windowsx.h>
#include <LDExporter/LDExporterSetting.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

FloatOptionUI::FloatOptionUI(OptionsCanvas *parent, LDExporterSetting &setting)
// Note that setting.getStringValue() automatically converts the setting's float
// value into a string.
	: NumberOptionUI(parent, setting, setting.getStringValue().c_str())
	, m_value(0.0f)
{
	// All the work is done in NumberOptionUI's constructor.
}

// This is called to determine if the value in the setting is valid.
bool FloatOptionUI::validate(ucstring &error)
{
	ucstring text;
	TCFloat value;

	CUIWindow::windowGetText(m_hEdit, text);
	if (sucscanf(text.c_str(), _UC("%f"), &value) == 1)
	{
		ucstring format;
		ucstring buf;

		// We got a float value; now check to see if it's in an acceptable
		// range.
		if (m_setting->hasMin() && value < m_setting->getMinFloatValue())
		{
			// The value is too small.  Set the edit control to the minimum
			// allowed value; an error will be presented to the user based on
			// code below.
			SetWindowText(m_hEdit,
				ftoucstr(m_setting->getMinFloatValue()).c_str());
		}
		else if (m_setting->hasMax() && value > m_setting->getMaxFloatValue())
		{
			// The value is too large.  Set the edit control to the maximum
			// allowed value; an error will be presented to the user based on
			// code below.
			SetWindowText(m_hEdit,
				ftoucstr(m_setting->getMaxFloatValue()).c_str());
		}
		else
		{
			// The value isn't out of the acceptable range, so we're good.
			m_value = value;
			// Validation succeeded.
			return true;
		}
		// If we get here, the value was either too small or too large.  Set
		// error to contain an error listing the valid range.
		format = TCObject::ls(_UC("ValueOutOfRange"));
		buf.reserve(format.size() + 128);
		sucprintf(&buf[0], buf.capacity() + 1, format.c_str(),
			ftoucstr(m_setting->getMinFloatValue()).c_str(),
			ftoucstr(m_setting->getMaxFloatValue()).c_str());
		error = buf.c_str();
	}
	else
	{
		// The scanf didn't find a float, so tell the user what is expected.
		error = TCObject::ls(_UC("DecimalNumberRequired"));
	}
	// If we get this far, validation failed.
	// Select the text in this edit control and set the focus to it, so that
	// when the user dismisses the error message box, they'll know which value
	// had the error.
	SendMessage(m_hEdit, EM_SETSEL, 0, -1);
	SetFocus(m_hEdit);
	// Validation failed.
	return false;
}

// Write the value to the setting.  Note that this requires validate to be
// called before commit is called, but that does happen.
void FloatOptionUI::commit(void)
{
	m_setting->setValue(m_value, true);
}
