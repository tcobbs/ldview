#include "LDLEmptyLine.h"

LDLEmptyLine::LDLEmptyLine(LDLModel *parentModel, const char *line,
						   int lineNumber, const char *originalLine)
	:LDLFileLine(parentModel, line, lineNumber, originalLine)
{
}

LDLEmptyLine::LDLEmptyLine(const LDLEmptyLine &other)
	:LDLFileLine(other)
{
}

LDLEmptyLine::~LDLEmptyLine(void)
{
}

bool LDLEmptyLine::parse(void)
{
	return true;
}

TCObject *LDLEmptyLine::copy(void)
{
	return new LDLEmptyLine(*this);
}
