#include "LDLUnknownLine.h"

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
	setError(LDLEParse, "Couldn't parse line.");
	return false;
}

TCObject *LDLUnknownLine::copy(void)
{
	return new LDLUnknownLine(*this);
}
