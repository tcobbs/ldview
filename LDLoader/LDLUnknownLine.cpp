#include "LDLUnknownLine.h"

LDLUnknownLine::LDLUnknownLine(LDLModel *mainModel, const char *line,
							   int lineNumber)
	:LDLFileLine(mainModel, line, lineNumber)
{
}

LDLUnknownLine::LDLUnknownLine(const LDLUnknownLine &other)
	:LDLFileLine(other)
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
