#include "LDLTriangleLine.h"
#include "LDLLineLine.h"
#include "LDLMainModel.h"
#include <TCFoundation/TCLocalStrings.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLTriangleLine::LDLTriangleLine(LDLModel *parentModel, const char *line,
							   int lineNumber, const char *originalLine)
	:LDLShapeLine(parentModel, line, lineNumber, originalLine),
	m_colinearIndex(-1),
	m_matchingIndex(-1)
{
}

LDLTriangleLine::LDLTriangleLine(const LDLTriangleLine &other)
	:LDLShapeLine(other),
	m_colinearIndex(other.m_colinearIndex),
	m_matchingIndex(other.m_matchingIndex)
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
		if (!getMainModel()->getSkipValidation())
		{
			// Note that we don't care what the second matching index is,
			// because we only need to throw out one of the two points, so don't
			// bother to even read it.
			if (getMatchingPoints(&m_matchingIndex))
			{
				m_valid = false;
			}
			else
			{
				checkForColinearPoints();
			}
		}
		return true;
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, TCLocalStrings::get(_UC("LDLTriLineParse")));
		return false;
	}
}

TCObject *LDLTriangleLine::copy(void) const
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
		if (m_matchingIndex >= 0)
		{
			return removeMatchingPoint();
		}
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

LDLFileLineArray *LDLTriangleLine::removePoint(int index)
{
	LDLFileLineArray *fileLineArray = NULL;
	LDLLineLine *lineLine = NULL;
/*
	TCVector &p1 = m_points[0];
	TCVector &p2 = m_points[1];
	TCVector &p3 = m_points[2];
*/

	switch (index)
	{
	case 0:
		lineLine = newLineLine(1, 2);
		break;
	case 1:
		lineLine = newLineLine(0, 2);
		break;
	case 2:
		lineLine = newLineLine(0, 1);
		break;
	default:
		break;
	}
	if (lineLine)
	{
		fileLineArray = new LDLFileLineArray(1);
		fileLineArray->addObject(lineLine);
		lineLine->release();
	}
	return fileLineArray;
}

LDLFileLineArray *LDLTriangleLine::removeMatchingPoint(void)
{
	LDLFileLineArray *fileLineArray = removePoint(m_matchingIndex);

	if (fileLineArray)
	{
		UCCHAR pointBuf[64] = _UC("");

		printPoint(m_matchingIndex, pointBuf);
		setWarning(LDLEMatchingPoints,
			TCLocalStrings::get(_UC("LDLTriLineIdentical")),
			m_matchingIndex + 1, pointBuf);
	}
	else
	{
		setError(LDLEGeneral,
			TCLocalStrings::get(_UC("LDLTriLineIdenticalError")));
	}
	return fileLineArray;
}

LDLFileLineArray *LDLTriangleLine::removeColinearPoint(void)
{
	LDLFileLineArray *fileLineArray = removePoint(m_colinearIndex);

	if (fileLineArray)
	{
		UCCHAR pointBuf[64] = _UC("");

		printPoint(m_colinearIndex, pointBuf);
		setWarning(LDLEColinear, TCLocalStrings::get(_UC("LDLTriLineCoLinear")),
			m_colinearIndex + 1, pointBuf);
	}
	else
	{
		setError(LDLEGeneral, TCLocalStrings::get(_UC("LDLTriLineCoLinearError")));
	}
	return fileLineArray;
}

LDLLineLine *LDLTriangleLine::newLineLine(int p1, int p2)
{
	UCCHAR pointBuf1[64] = _UC("");
	UCCHAR pointBuf2[64] = _UC("");
	UCCHAR ucNewLine[1024];
	char *newLine;
	LDLLineLine *retValue;

	printPoint(p1, pointBuf1);
	printPoint(p2, pointBuf2);
	sucprintf(ucNewLine, COUNT_OF(ucNewLine), _UC("3 %ld %s %s"), m_colorNumber,
		pointBuf1, pointBuf2);
	newLine = ucstringtombs(ucNewLine);
	retValue = new LDLLineLine(m_parentModel, newLine, m_lineNumber, m_line);
	delete newLine;
	return retValue;
}
