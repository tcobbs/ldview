#include "LDLTriangleLine.h"
#include "LDLLineLine.h"

LDLTriangleLine::LDLTriangleLine(LDLModel *parentModel, const char *line,
							   int lineNumber, const char *originalLine)
	:LDLShapeLine(parentModel, line, lineNumber, originalLine)
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
		checkForColinearPoints();
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

void LDLTriangleLine::checkForColinearPoints(void)
{
	m_colinearIndex = getColinearIndex();
	if (m_colinearIndex >= 0)
	{
		m_valid = false;
	}
}

int LDLTriangleLine::getColinearIndex(void)
{
	TCVector &p1 = m_points[0];
	TCVector &p2 = m_points[1];
	TCVector &p3 = m_points[2];
	TCVector normal = (p1 - p3) * (p1 - p2);

	if (normal.lengthSquared() == 0.0f)
	{
		return middleIndex(p1, p2, p3);
	}
	return -1;
}

LDLFileLineArray *LDLTriangleLine::getReplacementLines(void)
{
	if (m_valid)
	{
		return NULL;
	}
	else
	{
		if (m_colinearIndex >= 0)
		{
			return removeColinearPoint();
		}
		else
		{
			return NULL;
		}
	}
}

LDLFileLineArray *LDLTriangleLine::removeColinearPoint(void)
{
	LDLFileLineArray *fileLineArray = NULL;
	LDLLineLine *lineLine = NULL;
	TCVector &p1 = m_points[0];
	TCVector &p2 = m_points[1];
	TCVector &p3 = m_points[2];
	char pointBuf[64] = "";

	switch (m_colinearIndex)
	{
	case 0:
		p1.print(pointBuf);
		lineLine = newLineLine(p2, p3);
		break;
	case 1:
		p2.print(pointBuf);
		lineLine = newLineLine(p1, p3);
		break;
	case 2:
		p3.print(pointBuf);
		lineLine = newLineLine(p1, p2);
		break;
	default:
		break;
	}
	if (lineLine)
	{
		fileLineArray = new LDLFileLineArray(1);
		fileLineArray->addObject(lineLine);
		lineLine->release();
		setError(LDLEColinear, "Triangle contains co-linear points.\n"
			"Point %d <%s> removed.\n", m_colinearIndex + 1, pointBuf);
	}
	else
	{
		setError(LDLEGeneral, "Unexpected error removing co-linear points from "
			"triangle.\n");
	}
	return fileLineArray;
}

LDLLineLine *LDLTriangleLine::newLineLine(const TCVector &p1,
										  const TCVector &p2)
{
	char pointBuf1[64] = "";
	char pointBuf2[64] = "";
	char newLine[1024];

	p1.print(pointBuf1, 8);
	p2.print(pointBuf2, 8);
	sprintf(newLine, "3 %d %s %s", m_colorNumber, pointBuf1, pointBuf2);
	return new LDLLineLine(m_parentModel, newLine, m_lineNumber, m_line);
}
