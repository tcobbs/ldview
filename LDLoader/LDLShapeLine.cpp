#include "LDLShapeLine.h"

LDLShapeLine::LDLShapeLine(LDLModel *parentModel, const char *line,
						   int lineNumber)
	:LDLActionLine(parentModel, line, lineNumber),
	m_points(NULL)
{
}

LDLShapeLine::LDLShapeLine(const LDLShapeLine &other)
	:LDLActionLine(other),
	m_points(NULL),
	m_colorNumber(other.m_colorNumber)
{
	if (other.m_points)
	{
		int i;
		int count = other.getNumPoints();

		m_points = new Vector[count];
		for (i = 0; i < count; i++)
		{
			m_points[i] = other.m_points[i];
		}
	}
}

bool LDLShapeLine::parse(void)
{
	return true;
}

void LDLShapeLine::dealloc(void)
{
	delete[] m_points;
	LDLFileLine::dealloc();
}
