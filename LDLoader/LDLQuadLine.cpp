#include "LDLQuadLine.h"

LDLQuadLine::LDLQuadLine(LDLModel *mainModel, const char *line, int lineNumber)
	:LDLShapeLine(mainModel, line, lineNumber)
{
}

LDLQuadLine::LDLQuadLine(const LDLQuadLine &other)
	:LDLShapeLine(other)
{
}

bool LDLQuadLine::parse(void)
{
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;
	int lineType;

	if (sscanf(m_line, "%d %i %f %f %f %f %f %f %f %f %f %f %f %f", &lineType,
		&m_colorNumber, &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3,
		&x4, &y4, &z4) == 14)
	{
		m_points = new Vector[4];
		m_points[0] = Vector(x1, y1, z1);
		m_points[1] = Vector(x2, y2, z2);
		m_points[2] = Vector(x3, y3, z3);
		m_points[3] = Vector(x4, y4, z4);
		return true;
	}
	else
	{
		setError(LDLEParse, "Error parsing quad line.");
		return false;
	}
}

TCObject *LDLQuadLine::copy(void)
{
	return new LDLQuadLine(*this);
}
