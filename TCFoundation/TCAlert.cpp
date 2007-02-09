#include "TCAlert.h"
#include "TCStringArray.h"
#include "mystring.h"

TCAlert::TCAlert(
		const char *alertClass,
		const char *message,
		TCStringArray *extraInfo):
	m_alertClass(copyString(alertClass)),
	m_message(copyString(message)),
	m_extraInfo((TCStringArray *)TCObject::retain(extraInfo))
{
}

TCAlert::TCAlert(
		const char *alertClass,
		const wchar_t *message,
		const WStringList &extraInfo /*= WStringList()*/):
	m_alertClass(copyString(alertClass)),
	m_message(NULL),
	m_wMessage(message),
	m_extraInfo(NULL),
	m_wExtraInfo(extraInfo)
{
}

TCAlert::~TCAlert(void)
{
}

void TCAlert::dealloc(void)
{
	delete m_alertClass;
	delete m_message;
	TCObject::release(m_extraInfo);
	TCObject::dealloc();
}

const char *TCAlert::getMessage(void)
{
	if (!m_message)
	{
		std::string temp;

		wstringtostring(temp, m_wMessage);
		m_message = copyString(temp.c_str());
	}
	return m_message;
}

const wchar_t *TCAlert::getWMessage(void)
{
	if (!m_wMessage.size() && m_message && m_message[0])
	{
		mbstowstring(m_wMessage, m_message);
	}
	return m_wMessage.c_str();
}

TCStringArray *TCAlert::getExtraInfo(void)
{
	if (!m_extraInfo && m_wExtraInfo.size() > 0)
	{
		m_extraInfo = new TCStringArray;
		std::string temp;

		for (WStringList::iterator it = m_wExtraInfo.begin();
			it != m_wExtraInfo.end(); it++)
		{
			wstringtostring(temp, *it);
			m_extraInfo->addString(temp.c_str());
		}
	}
	return m_extraInfo;
}

const WStringList &TCAlert::getWExtraInfo(void)
{
	if (!m_wExtraInfo.size() && m_extraInfo && m_extraInfo->getCount() > 0)
	{
		int i;
		int count = m_extraInfo->getCount();

		for (i = 0; i < count; i++)
		{
			m_wExtraInfo.resize(m_wExtraInfo.size() + 1);
			std::wstring &temp = m_wExtraInfo.back();
			mbstowstring(temp, m_extraInfo->stringAtIndex(i));
		}
	}
	return m_wExtraInfo;
}
