#include "LDLShapeLine.h"

LDLShapeLine::LDLShapeLine(LDLModel *mainModel, const char *line, int lineNumber)
	:LDLFileLine(mainModel, line, lineNumber),
	m_points(NULL)
{
}

LDLShapeLine::LDLShapeLine(const LDLShapeLine &other)
	:LDLFileLine(other),
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
