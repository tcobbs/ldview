#include "LDLEmptyLine.h"

LDLEmptyLine::LDLEmptyLine(LDLModel *mainModel, const char *line, int lineNumber)
	:LDLFileLine(mainModel, line, lineNumber)
{
}

LDLEmptyLine::LDLEmptyLine(const LDLEmptyLine &other)
	:LDLFileLine(other)
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
