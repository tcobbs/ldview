#include "LDLError.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>

LDLError::LDLError(LDLErrorType type, const char *message, const char *filename,
				   const char *fileLine, int lineNumber,
				   TCStringArray *extraInfo)
	:TCAlert(LDLError::alertClass(), message, extraInfo),
	m_type(type),
	m_filename(copyString(filename)),
	m_fileLine(copyString(fileLine)),
	m_lineNumber(lineNumber)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDLError");
#endif
}

LDLError::~LDLError(void)
{
}

void LDLError::dealloc(void)
{
	delete m_filename;
	delete m_fileLine;
	TCAlert::dealloc();
}
