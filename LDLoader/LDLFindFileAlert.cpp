#include "LDLFindFileAlert.h"
#include <TCFoundation/mystring.h>

LDLFindFileAlert::LDLFindFileAlert(const char *filename)
	:TCAlert(LDLFindFileAlert::alertClass(), "", NULL),
	m_filename(copyString(filename))
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDLFindFileAlert");
#endif
}

LDLFindFileAlert::~LDLFindFileAlert(void)
{
}

void LDLFindFileAlert::dealloc(void)
{
	delete m_filename;
	TCAlert::dealloc();
}

void LDLFindFileAlert::setFilename(const char *value)
{
	delete m_filename;
	m_filename = copyString(value);
}
