#include "LDLModelLine.h"
#include "LDLPalette.h"
#include <string.h>
#include <stdio.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCLocalStrings.h>

LDLModelLine::LDLModelLine(LDLModel *parentModel, const char *line,
						   int lineNumber, const char *originalLine)
	:LDLActionLine(parentModel, line, lineNumber, originalLine),
	m_highResModel(NULL),
	m_lowResModel(NULL)
{
	m_flags.nonUniform = true;
}

LDLModelLine::LDLModelLine(const LDLModelLine &other)
	:LDLActionLine(other),
	m_highResModel(NULL),
	m_lowResModel(NULL),
	m_color(other.m_color),
	m_colorNumber(other.m_colorNumber),
	m_flags(other.m_flags)
{
	if (other.m_highResModel)
	{
		m_highResModel = (LDLModel *)other.m_highResModel->copy();
	}
	if (other.m_lowResModel)
	{
		m_lowResModel = (LDLModel *)other.m_lowResModel->copy();
	}
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

LDLModelLine::~LDLModelLine(void)
{
}

void LDLModelLine::dealloc(void)
{
	TCObject::release(m_highResModel);
	TCObject::release(m_lowResModel);
	LDLActionLine::dealloc();
}

TCObject *LDLModelLine::copy(void)
{
	return new LDLModelLine(*this);
}

bool LDLModelLine::parse(void)
{
	float x, y, z;
	float a, b, c, d, e, f, g, h, i;
	char subModelName[1024];
	int lineType;

	if (sscanf(m_line, "%d %i %f %f %f %f %f %f %f %f %f %f %f %f %s",
		&lineType, &m_colorNumber, &x, &y, &z, &a, &b, &c, &d, &e, &f,
		&g, &h, &i, subModelName) == 15)
	{
		int red, green, blue, alpha;
		float determinant;

		m_highResModel = m_parentModel->subModelNamed(subModelName);
		if (m_highResModel)
		{
			m_highResModel->retain();
		}
		else
		{
			m_valid = false;
			setError(LDLEFileNotFound, TCLocalStrings::get("LDLModelLineFNF"),
				subModelName);
			return false;
		}
		m_lowResModel = m_parentModel->subModelNamed(subModelName, true);
		if (m_lowResModel)
		{
			m_lowResModel->retain();
		}
		m_parentModel->getRGBA(m_colorNumber, red, green, blue, alpha);
		m_color = LDLPalette::colorForRGBA(red, green, blue, alpha);
		setTransformation(x, y, z, a, b, c, d, e, f, g, h, i);
		determinant = TCVector::determinant(m_matrix);
		if (determinant == 0.0f)
		{
			// The determinant is zero.  We will try to fix the
			// transformation matrix if the part only contains points in an
			// XZ-plane with Y == 0.0. Many part authors are lazy or
			// ignorant and specify zero Y values.
			if (isXZPlanar())
			{
				// This is an XZ-planar model, so try to fix the singular
				// matrix.
				determinant = tryToFixPlanarMatrix();
				if (determinant == 0.0f)
				{
					// If it's still zero, we failed to fix it.
					setError(LDLEMatrix,
						TCLocalStrings::get("LDLModelLineSingular"));
				}
			}
			else
			{
				// We don't want to even try to fix if the sub-model isn't
				// XZ-planar at Y == 0.
				setError(LDLEMatrix,
					TCLocalStrings::get("LDLModelLineSingularNonFlat"));
			}
			if (determinant == 0.0f)
			{
				m_valid = false;
				return false;
			}
		}
		// Note that if we get this far, determinant still holds the current
		// determinant of the matrix, even if the matrix got adjusted above.
		if (m_highResModel->isPart() && !m_parentModel->isPart())
		{
			if (!fEq2(determinant, 1.0f, 0.05f) &&
				!fEq2(determinant, -1.0f, 0.05f))
			{
				// If the determinant is not either 1 or -1, they applied a
				// non-uniform scale.  Note that we are being EXTREMELY
				//  loose with this "equality" check (within 0.05).
				setWarning(LDLEPartDeterminant,
					TCLocalStrings::get("LDLModelLineNonUniformPart"));
				m_flags.nonUniform = true;
			}
		}
		return true;
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, TCLocalStrings::get("LDLModelLineParse"));
		return false;
	}
}

void LDLModelLine::setTransformation(float x, float y, float z,
									 float a, float b, float c,
									 float d, float e, float f,
									 float g, float h, float i)
{
	m_matrix[3] = 0.0f;
	m_matrix[7] = 0.0f;
	m_matrix[11] = 0.0f;
	m_matrix[15] = 1.0f;
	m_matrix[0] = a;
	m_matrix[4] = b;
	m_matrix[8] = c;
	m_matrix[1] = d;
	m_matrix[5] = e;
	m_matrix[9] = f;
	m_matrix[2] = g;
	m_matrix[6] = h;
	m_matrix[10] = i;
	m_matrix[12] = x;
	m_matrix[13] = y;
	m_matrix[14] = z;
}

void LDLModelLine::print(int indent) const
{
	LDLActionLine::print(indent);
	if (getModel())
	{
		getModel()->print(indent);
	}
}

LDLModel *LDLModelLine::getModel(void) const
{
	if (m_parentModel)
	{
		if (m_lowResModel && m_parentModel->getLowResStuds())
		{
			return m_lowResModel;
		}
		else
		{
			return m_highResModel;
		}
	}
	else
	{
		return NULL;
	}
}

bool LDLModelLine::isXZPlanar(const float *matrix) const
{
	// This returns true if the all the points in this file line have Y == 0,
	// after being transformed by matrix.
	float newMatrix[16];
	int i;
	int count = m_highResModel->getActiveLineCount();
	LDLFileLineArray *fileLines = m_highResModel->getFileLines();

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	for (i = 0; i < count; i++)
	{
		LDLFileLine *fileLine = (*fileLines)[i];

		if (!fileLine->isXZPlanar(newMatrix))
		{
			return false;
		}
	}
	return true;
}

bool LDLModelLine::isXZPlanar(void) const
{
	// This returns true if the all the points in this file line have Y == 0.
	int i;
	int count = m_highResModel->getActiveLineCount();
	LDLFileLineArray *fileLines = m_highResModel->getFileLines();

	for (i = 0; i < count; i++)
	{
		LDLFileLine *fileLine = (*fileLines)[i];

		if (fileLine->getLineType() == LDLLineTypeModel)
		{
			LDLModelLine *modelLine = (LDLModelLine *)fileLine;
			float identityMatrix[16];

			TCVector::initIdentityMatrix(identityMatrix);
			if (!modelLine->isXZPlanar(identityMatrix))
			{
				return false;
			}
			else if (!fileLine->isXZPlanar())
			{
				return false;
			}
		}
	}
	return true;
}

// This code is inspired by an old (2000) version of L3Input that Lars Hassing
// wrote and then released for use in LDGLite.
//
// This method attempts to fix a singular matrix.  It's only called if the model
// is XZ-planar at 0.0 (all Y values equal 0.0).  Typically one of the rows are
// all zeros, and it doesn't affect the transformation to set the Y value to
// 1.0 (what the part author should have done).  Also, the second column may be
// all zeros.  Here we may set one of the Y values to 1.0 at a time.  Setting
// all three Y values to 1.0 at the same time may give two rows that are
// linear dependent (Travis Cobbs: not sure what that means).
float LDLModelLine::tryToFixPlanarMatrix(void)
{
	float determinant = 0.0f;
	int i;

	// First, check the rows.
	for (i = 0; i < 3; i++)
	{
		if (m_matrix[i] == 0.0f && m_matrix[4 + i] == 0.0f &&
			m_matrix[8 + i] == 0.0f)
		{
			// Row i all zeros, try fixing by setting the Y value
			m_matrix[4 + i] = 1.0f;
			determinant = TCVector::determinant(m_matrix);
			if (determinant != 0.0f)
			{
				setWarning(LDLEMatrix,
					TCLocalStrings::get("LDLModelLineZeroMatrixRow"), i);
				return determinant;
			}
		}
	}
	// That didn't work, so check the Y column.
	if (m_matrix[4] == 0.0f && m_matrix[5] == 0.0f && m_matrix[6] == 0.0f)
	{
		for (i = 0; i < 3; i++)
		{
			// Y column all zeros, try fixing one Y at a time.
			m_matrix[4 + i] = 1.0f;
			determinant = TCVector::determinant(m_matrix);
			if (determinant != 0.0f)
			{
				setWarning(LDLEMatrix,
					TCLocalStrings::get("LDLModelLineZeroMatrixCol"), i);
				return determinant;
			}
		}
	}
	return determinant;
}
