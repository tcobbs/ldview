#include "LDLError.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>

LDLError::LDLError(LDLErrorType type, const char *message, const char *filename,
				   const char *fileLine, int lineNumber,
				   TCStringArray *extraInfo)
		 :type(type),
		  message(copyString(message)),
		  filename(copyString(filename)),
		  fileLine(copyString(fileLine)),
		  lineNumber(lineNumber),
		  extraInfo(extraInfo)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDLError");
#endif
	if (extraInfo)
	{
		extraInfo->retain();
	}
}

LDLError::~LDLError(void)
{
}

void LDLError::dealloc(void)
{
	delete message;
	delete filename;
	delete fileLine;
	if (extraInfo)
	{
		extraInfo->release();
	}
	TCObject::dealloc();
}
