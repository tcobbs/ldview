#include "LDLModelLine.h"
#include <string.h>
#include <stdio.h>
#include "LDLMacros.h"

LDLModelLine::LDLModelLine(LDLModel *mainModel, const char *line, int lineNumber)
	:LDLFileLine(mainModel, line, lineNumber),
	m_highResModel(NULL),
	m_lowResModel(NULL)
{
	m_flags.bfcCertify = BFCUnknownState;
	m_flags.bfcClip = false;
	m_flags.bfcWindingCCW = true;
	m_flags.bfcInvert = false;
}

LDLModelLine::LDLModelLine(const LDLModelLine &other)
	:LDLFileLine(other),
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
	memcpy(m_transformationMatrix, other.m_transformationMatrix,
		sizeof(m_transformationMatrix));
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
		setTransformation(x, y, z, a, b, c, d, e, f, g, h, i);
		m_color = LDLModel::colorForRGBA(red, green, blue, alpha);
		return true;
	}
	else
	{
		setError(LDLEParse, "Error parsing model line.");
		return false;
	}
}

void LDLModelLine::setTransformation(float x, float y, float z,
									 float a, float b, float c,
									 float d, float e, float f,
									 float g, float h, float i)
{
	m_transformationMatrix[3] = 0.0f;
	m_transformationMatrix[7] = 0.0f;
	m_transformationMatrix[11] = 0.0f;
	m_transformationMatrix[15] = 1.0f;
	m_transformationMatrix[0] = a;
	m_transformationMatrix[4] = b;
	m_transformationMatrix[8] = c;
	m_transformationMatrix[1] = d;
	m_transformationMatrix[5] = e;
	m_transformationMatrix[9] = f;
	m_transformationMatrix[2] = g;
	m_transformationMatrix[6] = h;
	m_transformationMatrix[10] = i;
	m_transformationMatrix[12] = x;
	m_transformationMatrix[13] = y;
	m_transformationMatrix[14] = z;
}

void LDLModelLine::print(int indent)
{
	LDLFileLine::print(indent);
	if (getModel())
	{
		getModel()->print(indent);
	}
}

LDLModel *LDLModelLine::getModel(void) const
{
	if (m_parentModel)
	{
		if (m_parentModel->getLowResStuds() && m_lowResModel)
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

void LDLModelLine::setBFCSettings(BFCState bfcCertify, bool bfcClip,
								  bool bfcWindingCCW, bool bfcInvert)
{
	m_flags.bfcCertify = bfcCertify;
	m_flags.bfcClip = bfcClip;
	m_flags.bfcWindingCCW = bfcWindingCCW;
	m_flags.bfcInvert = bfcInvert;
}
