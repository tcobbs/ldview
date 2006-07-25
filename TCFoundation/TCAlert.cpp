#include "TCAlert.h"
#include "TCStringArray.h"
#include "mystring.h"

TCAlert::TCAlert(const char *alertClass, const char *message,
				 TCStringArray *extraInfo)
	:m_alertClass(copyString(alertClass)),
	m_message(copyString(message)),
	m_extraInfo((TCStringArray *)TCObject::retain(extraInfo))
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
