#include "LDModelParser.h"

#include <string.h>

#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLShapeLine.h>
#include <LDLoader/LDLModelLine.h>
#include <TRE/TREMainModel.h>
#include <TRE/TRESubModel.h>
#include <TRE/TREVertexArray.h>
#include <TRE/TREShapeGroup.h>
#include <TCFoundation/mystring.h>
#include <LDLoader/LDLMacros.h>
#include <TCFoundation/TCVector.h>

static const int LO_NUM_SEGMENTS = 8;
static const int HI_NUM_SEGMENTS = 16;

LDModelParser::LDModelParser(void)
	:m_mainLDLModel(NULL),
	m_mainTREModel(NULL),
	m_curveQuality(2),
	m_seamWidth(0.0f)
{
	m_flags.flattenParts = true;
	m_flags.primitiveSubstitution = false;
	m_flags.seams = false;
	m_flags.edgeLines = false;
}

LDModelParser::~LDModelParser(void)
{
}

void LDModelParser::dealloc(void)
{
	TCObject::release(m_mainLDLModel);
	TCObject::release(m_mainTREModel);
	TCObject::dealloc();
}

void LDModelParser::finishPart(TREModel *treModel, TRESubModel *subModel)
{
	if (m_flags.flattenParts)
	{
		treModel->flatten();
	}
	treModel->calculateBoundingBox();
	if (subModel)
	{
		if (m_flags.seams)
		{
			subModel->shrink(m_seamWidth);
		}
	}
}

bool LDModelParser::parseMainModel(LDLMainModel *mainLDLModel)
{
	TCULong colorNumber = 7;
	TCULong edgeColorNumber = mainLDLModel->getEdgeColorNumber(colorNumber);

	m_mainLDLModel = (LDLMainModel *)mainLDLModel->retain();
	m_mainTREModel = new TREMainModel;
	m_mainTREModel->setPartFlag(mainLDLModel->isPart());
	m_mainTREModel->setEdgeLinesFlag(getEdgeLinesFlag());
	m_mainTREModel->setTwoSidedLightingFlag(getTwoSidedLightingFlag());
	m_mainTREModel->setColor(mainLDLModel->getPackedRGBA(colorNumber),
		mainLDLModel->getPackedRGBA(edgeColorNumber));
	if (parseModel(m_mainLDLModel, m_mainTREModel))
	{
		if (m_mainTREModel->isPart())
		{
			finishPart(m_mainTREModel);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelParser::addSubModel(LDLModelLine *modelLine,
								TREModel *treParentModel,
								TREModel *treModel)
{
	TCULong colorNumber = modelLine->getColorNumber();
	TRESubModel *treSubModel = NULL;

	if (colorNumber == 16)
	{
		treSubModel = treParentModel->addSubModel(modelLine->getMatrix(),
			treModel);
	}
	else
	{
		LDLModel *parentModel = modelLine->getParentModel();
		TCULong edgeColorNumber = parentModel->getEdgeColorNumber(colorNumber);

		treSubModel = treParentModel->addSubModel(
			parentModel->getPackedRGBA(colorNumber),
			parentModel->getPackedRGBA(edgeColorNumber),
			modelLine->getMatrix(), treModel);
	}
	if (treModel->isPart() && !treParentModel->isPart())
	{
		finishPart(treModel, treSubModel);
	}
	return true;
}

bool LDModelParser::parseModel(LDLModelLine *modelLine, TREModel *treModel)
{
	LDLModel *ldlModel = modelLine->getModel();

	if (ldlModel)
	{
		const char *name = ldlModel->getName();
		TREModel *model = m_mainTREModel->modelNamed(name);

		if (model)
		{
			return addSubModel(modelLine, treModel, model);
		}
		else
		{
			model = new TREModel;
			model->setMainModel(treModel->getMainModel());
			model->setName(name);
			model->setPartFlag(ldlModel->isPart());
			if (parseModel(ldlModel, model))
			{
				m_mainTREModel->registerModel(model);
				model->release();
				return addSubModel(modelLine, treModel, model);
			}
			else
			{
				model->release();
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

float LDModelParser::startingFraction(const char *filename)
{
	int top;
	int bottom;

	sscanf(filename, "%d", &top);
	sscanf(filename + 2, "%d", &bottom);
	return (float)top / (float)bottom;
}

bool LDModelParser::startsWithFraction(const char *filename)
{
	return isdigit(filename[0]) && filename[1] == '-' && isdigit(filename[2]) &&
		!isdigit(filename[3]);
}

bool LDModelParser::startsWithFraction2(const char *filename)
{
	return isdigit(filename[0]) && filename[1] == '-' && isdigit(filename[2]) &&
		isdigit(filename[3]) && !isdigit(filename[4]);
}

bool LDModelParser::isPrimitive(const char *filename, const char *suffix)
{
	int fileLen = strlen(filename);
	int suffixLen = strlen(suffix);

	return ((fileLen == suffixLen + 3 && startsWithFraction(filename)) ||
		(suffixLen <= 8 && fileLen == suffixLen + 4 &&
		startsWithFraction2(filename))) &&
		stringHasCaseInsensitiveSuffix(filename, suffix);
}

bool LDModelParser::isCyli(const char *filename)
{
	return isPrimitive(filename, "cyli.dat");
}

bool LDModelParser::is1DigitCon(const char *filename)
{
	return strlen(filename) == 11 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::is2DigitCon(const char *filename)
{
	return strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::isCon(const char *filename)
{
	return is1DigitCon(filename) || is2DigitCon(filename);
}

bool LDModelParser::substituteStud(TREModel *treModel, int numSegments)
{
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments);
	treModel->addDisk(TCVector(0.0f, -4.0f, 0.0f), 6.0f, numSegments);
	if (m_flags.edgeLines)
	{
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments);
		treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
			numSegments);
	}
	return true;
}

bool LDModelParser::substituteStud(TREModel *treModel)
{
	return substituteStud(treModel, getNumCircleSegments());
}

bool LDModelParser::substituteStu2(TREModel *treModel)
{
	return substituteStud(treModel, LO_NUM_SEGMENTS);
}

bool LDModelParser::substituteStu22(TREModel *treModel, bool isA)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 4.0f, numSegments);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, 0.0f,
		numSegments);
	if (m_flags.edgeLines)
	{
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 4.0f,
			numSegments);
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments);
		if (!isA)
		{
			treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 4.0f,
				numSegments);
			treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteStu23(TREModel *treModel, bool isA)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 4.0f, numSegments);
	treModel->addDisk(TCVector(0.0f, -4.0f, 0.0f), 4.0f, numSegments);
	if (m_flags.edgeLines)
	{
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 4.0f,
			numSegments);
		if (!isA)
		{
			treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 4.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteStu24(TREModel *treModel, bool isA)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 4.0f, numSegments);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 6.0f, 0.0f,
		numSegments);
	if (m_flags.edgeLines)
	{
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments);
		treModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 8.0f,
			numSegments);
		if (!isA)
		{
			treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
				numSegments);
			treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 8.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteCylinder(TREModel *treModel, float fraction)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, numSegments,
		(int)(numSegments * fraction));
	return true;
}

bool LDModelParser::substituteCone(TREModel *treModel, float fraction, int size)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addOpenCone(TCVector(0.0f, 0.0f, 0.0f), (float)size + 1.0f,
		(float)size, 1.0f, numSegments, (int)(numSegments * fraction));
	return true;
}

int LDModelParser::getNumCircleSegments(float fraction)
{
	int retValue = m_curveQuality * LO_NUM_SEGMENTS;

	if (fraction != 0.0f)
	{
		int i;
		
		for (i = m_curveQuality; !fEq(fraction * retValue,
			(float)(int)(fraction * retValue)) && i < 12; i++)
		{
			retValue = (i + 1) * LO_NUM_SEGMENTS;
		}
	}
	return retValue;
}

bool LDModelParser::performPrimitiveSubstitution(LDLModel *ldlModel,
												 TREModel *treModel)
{
	if (m_flags.primitiveSubstitution)
	{
		const char *modelName = ldlModel->getName();

		if (!modelName)
		{
			return false;
		}
		if (strcasecmp(modelName, "LDL-LOWRES:stu2.dat") == 0)
		{
			return substituteStu2(treModel);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu22.dat") == 0)
		{
			return substituteStu22(treModel);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu22a.dat") == 0)
		{
			return substituteStu22(treModel, true);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu23.dat") == 0)
		{
			return substituteStu23(treModel);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu23a.dat") == 0)
		{
			return substituteStu23(treModel, true);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu24.dat") == 0)
		{
			return substituteStu24(treModel);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu24a.dat") == 0)
		{
			return substituteStu24(treModel, true);
		}
		else if (strcmp(modelName, "stud.dat") == 0)
		{
			return substituteStud(treModel);
		}
		else if (isCyli(modelName))
		{
			return substituteCylinder(treModel, startingFraction(modelName));
		}
		else if (isCon(modelName))
		{
			int size;

			sscanf(modelName + 6, "%d", &size);
			return substituteCone(treModel, startingFraction(modelName), size);
		}
	}
	return false;
}

bool LDModelParser::parseModel(LDLModel *ldlModel, TREModel *treModel)
{
	if (ldlModel && !performPrimitiveSubstitution(ldlModel, treModel))
	{
		LDLFileLineArray *fileLines = ldlModel->getFileLines();

		if (fileLines)
		{
			int i;
			int count = ldlModel->getActiveLineCount();

			treModel->setName(ldlModel->getName());
			for (i = 0; i < count; i++)
			{
				LDLFileLine *fileLine = (*fileLines)[i];

				if (fileLine->isActionLine() && fileLine->isValid())
				{
					switch (fileLine->getLineType())
					{
					case LDLLineTypeModel:
						parseModel((LDLModelLine *)fileLine, treModel);
						break;
					case LDLLineTypeLine:
						parseLine((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeTriangle:
						parseTriangle((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeQuad:
						parseQuad((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeConditionalLine:
						break;
					}
				}
			}
		}
	}
	return true;
}

void LDModelParser::parseLine(LDLShapeLine *shapeLine, TREModel *treModel)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addLine(shapeLine->getPoints());
	}
	else if (colorNumber == 24)
	{
		if (m_flags.edgeLines)
		{
			treModel->addEdgeLine(shapeLine->getPoints());
		}
	}
	else
	{
		treModel->addLine(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

void LDModelParser::parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addTriangle(shapeLine->getPoints());
	}
	else
	{
		treModel->addTriangle(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

void LDModelParser::parseQuad(LDLShapeLine *shapeLine, TREModel *treModel)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addQuad(shapeLine->getPoints());
	}
	else
	{
		treModel->addQuad(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

void LDModelParser::setSeamWidth(float seamWidth)
{
	m_seamWidth = seamWidth;
	if (m_seamWidth)
	{
		m_flags.seams = true;
	}
	else
	{
		m_flags.seams = false;
	}
}

float LDModelParser::getSeamWidth(void)
{
	if (m_flags.seams)
	{
		return m_seamWidth;
	}
	else
	{
		return 0.0f;
	}
}

/*
int LDModelParser::parseShapeVertices(LDLShapeLine *shapeLine,
									   TREModel *treModel)
{
	TCVector *points = shapeLine->getPoints();
	TREVertexArray *vertices = treModel->getVertices();
	int i;
	int count = shapeLine->getNumPoints();

	for (i = 0; i < count; i++)
	{
		TREVertex vertex;

		memcpy(vertex.v, (float *)points[i], sizeof(vertex.v));
		vertices->addVertex(vertex);
	}
	return vertices->getCount() - count;
}
*/