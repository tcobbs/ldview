#include "LDLUnknownLine.h"
#include <TCFoundation/TCLocalStrings.h>

LDLUnknownLine::LDLUnknownLine(LDLModel *parentModel, const char *line,
							   int lineNumber, const char *originalLine)
	:LDLFileLine(parentModel, line, lineNumber, originalLine)
{
}

LDLUnknownLine::LDLUnknownLine(const LDLUnknownLine &other)
	:LDLFileLine(other)
{
}

LDLUnknownLine::~LDLUnknownLine(void)
{
}

bool LDLUnknownLine::parse(void)
{
	setError(LDLEParse, TCLocalStrings::get("LDLUnknownLineParse"));
	return false;
}

TCObject *LDLUnknownLine::copy(void)
{
	return new LDLUnknownLine(*this);
}
