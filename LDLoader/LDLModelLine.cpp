#include "LDLModelLine.h"
#include <string.h>
#include <stdio.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCVector.h>

LDLModelLine::LDLModelLine(LDLModel *parentModel, const char *line,
						   int lineNumber, const char *originalLine)
	:LDLActionLine(parentModel, line, lineNumber, originalLine),
	m_highResModel(NULL),
	m_lowResModel(NULL)
{
}

LDLModelLine::LDLModelLine(const LDLModelLine &other)
	:LDLActionLine(other),
	m_highResModel(NULL),
	m_lowResModel(NULL),
	m_color(other.m_color),
	m_colorNumber(other.m_colorNumber)
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
	if (m_highResModel)
	{
		m_highResModel->release();
	}
	if (m_lowResModel)
	{
		m_lowResModel->release();
	}
	LDLFileLine::dealloc();
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

		m_highResModel = m_parentModel->subModelNamed(subModelName);
		if (m_highResModel)
		{
			m_highResModel->retain();
		}
		else
		{
			m_valid = false;
			setError(LDLEFileNotFound, "Error loading submodel %s",
				subModelName);
			return false;
		}
		m_lowResModel = m_parentModel->subModelNamed(subModelName, true);
		if (m_lowResModel)
		{
			m_lowResModel->retain();
		}
		m_parentModel->getRGBA(m_colorNumber, red, green, blue, alpha);
		m_color = LDLModel::colorForRGBA(red, green, blue, alpha);
		if (setTransformation(x, y, z, a, b, c, d, e, f, g, h, i))
		{
			if (m_highResModel->isPart())
			{
				float determinant = TCVector::determinant(m_matrix);

				if (!fEq2(determinant, 1.0f, 0.05f) &&
					!fEq2(determinant, -1.0f, 0.05f))
				{
					// If the determinant is not either 1 or -1, they applied a
					// non-uniform scale.  Note that we are being EXTREMELY
					//  loose with this "equality" check (within 0.05).
					setError(LDLEPartDeterminant,
						"Part transformed non-uniformly.");
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, "Error parsing model line.");
		return false;
	}
}

bool LDLModelLine::setTransformation(float x, float y, float z,
									 float a, float b, float c,
									 float d, float e, float f,
									 float g, float h, float i)
{
	bool retValue = true;

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
/*
	if (m_matrix[0] == 0.0f && m_matrix[1] == 0.0f && m_matrix[2] == 0.0f)
	{
		setError(LDLEMatrix, "Degenerate matrix column 0.");
		m_matrix[0] = 1.0f;
		retValue = false;
	}
	if (m_matrix[4] == 0.0f && m_matrix[5] == 0.0f && m_matrix[6] == 0.0f)
	{
//		setError(LDLEMatrix, "Degenerate matrix column 1.");
		m_matrix[5] = 1.0f;
//		retValue = false;
	}
	if (m_matrix[8] == 0.0f && m_matrix[9] == 0.0f && m_matrix[10] == 0.0f)
	{
		setError(LDLEMatrix, "Degenerate matrix column 2.");
		m_matrix[10] = 1.0f;
		retValue = false;
	}
*/
	return retValue;
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
