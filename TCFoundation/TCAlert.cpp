#include "TCAlert.h"
#include "TCStringArray.h"
#include "mystring.h"

TCAlert::TCAlert(TCULong alertClass, const char *message,
				 TCStringArray *extraInfo)
	:m_alertClass(alertClass),
	m_message(copyString(message)),
	m_extraInfo((TCStringArray *)TCObject::retain(extraInfo))
{
}

TCAlert::~TCAlert(void)
{
}

void TCAlert::dealloc(void)
{
	delete m_message;
	TCObject::release(m_extraInfo);
	TCObject::dealloc();
}
