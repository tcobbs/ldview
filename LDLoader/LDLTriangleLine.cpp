#include "LDLTriangleLine.h"

LDLTriangleLine::LDLTriangleLine(LDLModel *parentModel, const char *line,
							   int lineNumber)
	:LDLShapeLine(parentModel, line, lineNumber)
{
}

LDLTriangleLine::LDLTriangleLine(const LDLTriangleLine &other)
	:LDLShapeLine(other)
{
}

bool LDLTriangleLine::parse(void)
{
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	int lineType;

	if (sscanf(m_line, "%d %i %f %f %f %f %f %f %f %f %f", &lineType,
		&m_colorNumber, &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3) == 11)
	{
		m_points = new TCVector[3];
		m_points[0] = TCVector(x1, y1, z1);
		m_points[1] = TCVector(x2, y2, z2);
		m_points[2] = TCVector(x3, y3, z3);
		return true;
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, "Error parsing triangle line.");
		return false;
	}
}

TCObject *LDLTriangleLine::copy(void)
{
	return new LDLTriangleLine(*this);
}
