#include "LDLFindFileAlert.h"
#include <TCFoundation/mystring.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLFindFileAlert::LDLFindFileAlert(const std::string &filename)
	:TCAlert(LDLFindFileAlert::alertClass(), "", NULL),
	m_filename(filename),
	m_fileFound(false),
	m_part(false)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDLFindFileAlert");
#endif
}

LDLFindFileAlert::~LDLFindFileAlert(void)
{
}

void LDLFindFileAlert::setFilename(const std::string &value)
{
	m_filename = value;
}
