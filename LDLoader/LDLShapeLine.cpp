#include "LDLShapeLine.h"

LDLShapeLine::LDLShapeLine(LDLModel *parentModel, const char *line,
						   int lineNumber, const char *originalLine)
	:LDLActionLine(parentModel, line, lineNumber, originalLine),
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

		m_points = new TCVector[count];
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

int LDLShapeLine::middleIndex(const TCVector &p1, const TCVector &p2,
							 const TCVector &p3) const
{
	TCVector min = p1;
	TCVector max = p1;
	int i;

	for (i = 0; i < 3; i++)
	{
		if (p2.get(i) < min.get(i))
		{
			min[i] = p2.get(i);
		}
		if (p2.get(i) > max.get(i))
		{
			max[i] = p2.get(i);
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (p3.get(i) < min.get(i))
		{
			min[i] = p3.get(i);
		}
		if (p3.get(i) > max.get(i))
		{
			max[i] = p3.get(i);
		}
	}
	if (p1 != min && p1 != max)
	{
		return 0;
	}
	else if (p2 != min && p2 != max)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

bool LDLShapeLine::isXZPlanar(void) const
{
	// This returns true if the all the points in this file line have Y == 0.
	int i;
	int count = getNumPoints();

	for (i = 0; i < count; i++)
	{
		if (m_points[i][1] != 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool LDLShapeLine::isXZPlanar(const float *matrix) const
{
	// This returns true if the all the points in this file line have Y == 0,
	// after being transformed by matrix.
	int i;
	int count = getNumPoints();

	for (i = 0; i < count; i++)
	{
		TCVector newPoint = m_points[i].transformPoint(matrix);

		if (newPoint[1] != 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool LDLShapeLine::getMatchingPoints(int *index1, int *index2)
{
	int i, j;
	int count = getNumPoints();

	for (i = 0; i < count - 1; i++)
	{
		for (j = i + 1; j < count; j++)
		{
			if (m_points[i].exactlyEquals(m_points[j]))
			{
				if (index1)
				{
					*index1 = i;
				}
				if (index2)
				{
					*index2 = j;
				}
				setWarning(LDLEMatchingPoints,
					"Vertices %d and %d are the same.", i + 1, j + 1);
				return true;
			}
		}
	}
	return false;
}
