#include "LDLLineLine.h"

LDLLineLine::LDLLineLine(LDLModel *mainModel, const char *line, int lineNumber)
	:LDLShapeLine(mainModel, line, lineNumber)
{
}

LDLLineLine::LDLLineLine(const LDLLineLine &other)
	:LDLShapeLine(other)
{
}

bool LDLLineLine::parse(void)
{
	float x1, y1, z1;
	float x2, y2, z2;
	int lineType;

	if (sscanf(m_line, "%d %i %f %f %f %f %f %f", &lineType, &m_colorNumber,
		&x1, &y1, &z1, &x2, &y2, &z2) == 8)
	{
		m_points = new Vector[2];
		m_points[0] = Vector(x1, y1, z1);
		m_points[1] = Vector(x2, y2, z2);
		return true;
	}
	else
	{
		setError(LDLEParse, "Error parsing line line.");
		return false;
	}
}

TCObject *LDLLineLine::copy(void)
{
	return new LDLLineLine(*this);
}
