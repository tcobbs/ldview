#include "LDLUnknownLine.h"

LDLUnknownLine::LDLUnknownLine(LDLModel *parentModel, const char *line,
							   int lineNumber)
	:LDLFileLine(parentModel, line, lineNumber)
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
