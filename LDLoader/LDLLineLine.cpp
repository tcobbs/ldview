#include "LDLLineLine.h"
#include "LDLMainModel.h"
#include <TCFoundation/TCLocalStrings.h>

LDLLineLine::LDLLineLine(LDLModel *parentModel, const char *line,
						 int lineNumber, const char *originalLine)
	:LDLShapeLine(parentModel, line, lineNumber, originalLine)
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
		m_points = new TCVector[2];
		m_points[0] = TCVector(x1, y1, z1);
		m_points[1] = TCVector(x2, y2, z2);
		if (!getMainModel()->getSkipValidation())
		{
			getMatchingPoints();
		}
		return true;
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, TCLocalStrings::get(_UC("LDLLineParse")));
		return false;
	}
}

TCObject *LDLLineLine::copy(void)
{
	return new LDLLineLine(*this);
}
