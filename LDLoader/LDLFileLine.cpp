#include "LDLFileLine.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>

#include "LDLModel.h"
#include "LDLCommentLine.h"
#include "LDLModelLine.h"
#include "LDLLineLine.h"
#include "LDLTriangleLine.h"
#include "LDLQuadLine.h"
#include "LDLConditionalLineLine.h"
#include "LDLEmptyLine.h"
#include "LDLUnknownLine.h"

LDLFileLine::LDLFileLine(LDLModel *parentModel, const char *line, int lineNumber)
	:m_parentModel(parentModel),
	m_line(copyString(line)),
	m_lineNumber(lineNumber),
	m_error(NULL)
{
}

LDLFileLine::LDLFileLine(const LDLFileLine &other)
	:m_parentModel(other.m_parentModel),
	m_line(copyString(other.m_line)),
	m_lineNumber(other.m_lineNumber),
	m_error(NULL)
{
	if (other.m_error)
	{
		m_error = (LDLError*)other.m_error->retain();
	}
}

LDLFileLine::~LDLFileLine(void)
{
}

void LDLFileLine::dealloc(void)
{
	delete m_line;
	if (m_error)
	{
		m_error->release();
		m_error = NULL;
	}
	TCObject::dealloc();
}

void LDLFileLine::setError(LDLErrorType type, const char* format, ...)
{
	va_list argPtr;

	if (m_error)
	{
		m_error->release();
	}
	va_start(argPtr, type);
	m_error = m_parentModel->newError(type, *this, format, argPtr);
	va_end(argPtr);
}

int LDLFileLine::scanLineType(const char *line)
{
	int num;

	if (sscanf(line, "%d", &num) == 1)
	{
		return num;
	}
	return -1;
}

bool LDLFileLine::lineIsEmpty(const char *line)
{
	while (line[0])
	{
		char thisChar = line[0];

		if (thisChar != ' ' || thisChar != '\t')
		{
			return false;
		}
		line++;
	}
	return true;
}

LDLFileLine *LDLFileLine::initFileLine(LDLModel *parentModel, const char *line,
									   int lineNumber)
{
	if (strlen(line))
	{
		switch (scanLineType(line))
		{
		case 0:
			return new LDLCommentLine(parentModel, line, lineNumber);
			break;
		case 1:
			return new LDLModelLine(parentModel, line, lineNumber);
			break;
		case 2:
			return new LDLLineLine(parentModel, line, lineNumber);
			break;
		case 3:
			return new LDLTriangleLine(parentModel, line, lineNumber);
			break;
		case 4:
			return new LDLQuadLine(parentModel, line, lineNumber);
			break;
		case 5:
			return new LDLConditionalLineLine(parentModel, line, lineNumber);
			break;
		}
	}
	if (lineIsEmpty(line))
	{
		return new LDLEmptyLine(parentModel, line, lineNumber);
	}
	else
	{
		return new LDLUnknownLine(parentModel, line, lineNumber);
	}
}

void LDLFileLine::print(int /*indent*/) const
{
//	printf("%d: %s\n", m_lineNumber, m_line);
}
