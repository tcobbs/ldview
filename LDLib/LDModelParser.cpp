#include "LDModelParser.h"

#include <string.h>

#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLShapeLine.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLConditionalLineLine.h>
#include <LDLoader/LDLPalette.h>
#include <TRE/TREMainModel.h>
#include <TRE/TRESubModel.h>
#include <TRE/TREVertexArray.h>
#include <TRE/TREShapeGroup.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCVector.h>

static const int LO_NUM_SEGMENTS = 8;
static const int HI_NUM_SEGMENTS = 16;

LDModelParser::LDModelParser(void)
	:m_mainLDLModel(NULL),
	m_mainTREModel(NULL),
	m_curveQuality(2),
	m_seamWidth(0.0f)
{
	m_flags.flattenParts = true;	// Supporting false here could take a lot
									// of work.
	m_flags.primitiveSubstitution = true;
	m_flags.seams = false;
	m_flags.edgeLines = false;
	m_flags.bfc = false;
	m_flags.compileParts = true;
	m_flags.compileAll = true;
	m_flags.lighting = false;
	m_flags.twoSidedLighting = false;
	m_flags.aaLines = false;
	m_flags.sortTransparent = false;
	m_flags.stipple = false;
	m_flags.wireframe = false;
	m_flags.conditionalLines = false;
	m_flags.showAllConditional = false;
	m_flags.conditionalControlPoints = false;
	m_flags.defaultColorSet = false;
	m_flags.defaultColorNumberSet = false;
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
	if (subModel)
	{
		if (m_flags.seams)
		{
			subModel->shrink(m_seamWidth);
		}
	}
}

void LDModelParser::setDefaultRGB(TCByte r, TCByte g, TCByte b)
{
	defaultR = r;
	defaultG = g;
	defaultB = b;
	m_flags.defaultColorSet = true;
}

void LDModelParser::setDefaultColorNumber(int colorNumber)
{
	defaultColorNumber = colorNumber;
	m_flags.defaultColorNumberSet = true;
}

bool LDModelParser::parseMainModel(LDLMainModel *mainLDLModel)
{
	TCULong colorNumber = 7;
	TCULong edgeColorNumber;
	LDLPalette *palette = mainLDLModel->getPalette();

	m_mainLDLModel = (LDLMainModel *)mainLDLModel->retain();
	m_mainTREModel = new TREMainModel;
	m_mainTREModel->setPartFlag(mainLDLModel->isPart());
	m_mainTREModel->setEdgeLinesFlag(getEdgeLinesFlag());
	m_mainTREModel->setEdgesOnlyFlag(getEdgesOnlyFlag());
	m_mainTREModel->setLightingFlag(getLightingFlag());
	m_mainTREModel->setTwoSidedLightingFlag(getTwoSidedLightingFlag());
	m_mainTREModel->setBFCFlag(getBFCFlag());
	m_mainTREModel->setCompilePartsFlag(getCompilePartsFlag());
	m_mainTREModel->setCompileAllFlag(getCompileAllFlag());
	m_mainTREModel->setPolygonOffsetFlag(getPolygonOffsetFlag());
	if (m_flags.defaultColorNumberSet)
	{
		colorNumber = defaultColorNumber;
	}
	else if (m_flags.defaultColorSet)
	{
		colorNumber = palette->getColorNumberForRGB(defaultR, defaultG,
			defaultB);
	}
	edgeColorNumber = mainLDLModel->getEdgeColorNumber(colorNumber);
	m_mainTREModel->setColor(mainLDLModel->getPackedRGBA(colorNumber),
		mainLDLModel->getPackedRGBA(edgeColorNumber));
	m_mainTREModel->setAALinesFlag(getAALinesFlag());
	m_mainTREModel->setSortTransparentFlag(getSortTransparentFlag());
	m_mainTREModel->setStippleFlag(getStippleFlag());
	m_mainTREModel->setWireframeFlag(getWireframeFlag());
	m_mainTREModel->setConditionalLinesFlag(getConditionalLinesFlag());
	m_mainTREModel->setShowAllConditionalFlag(getShowAllConditionalFlag());
	m_mainTREModel->setConditionalControlPointsFlag(
		getConditionalControlPointsFlag());
	if (parseModel(m_mainLDLModel, m_mainTREModel, getBFCFlag()))
	{
		if (m_mainTREModel->isPart() || getFileIsPartFlag())
		{
			finishPart(m_mainTREModel);
		}
		m_mainTREModel->postProcess();
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelParser::addSubModel(LDLModelLine *modelLine,
								TREModel *treParentModel,
								TREModel *treModel, bool invert)
{
	TCULong colorNumber = modelLine->getColorNumber();
	TRESubModel *treSubModel = NULL;

	if (colorNumber == 16)
	{
		treSubModel = treParentModel->addSubModel(modelLine->getMatrix(),
			treModel, invert);
	}
	else
	{
		LDLModel *parentModel = modelLine->getParentModel();
		TCULong edgeColorNumber = parentModel->getEdgeColorNumber(colorNumber);

		treSubModel = treParentModel->addSubModel(
			parentModel->getPackedRGBA(colorNumber),
			parentModel->getPackedRGBA(edgeColorNumber),
			modelLine->getMatrix(), treModel, invert);
	}
	if (treModel->isPart() && !treParentModel->isPart())
	{
		finishPart(treModel, treSubModel);
	}
	if (TCVector::determinant(treSubModel->getMatrix()) < 0.0f)
	{
		// Generate an inverted version of the child model, because this
		// reference to it involves a mirror.
		treModel->getUnMirroredModel();
	}
	return true;
}

bool LDModelParser::parseModel(LDLModelLine *modelLine, TREModel *treModel,
							   bool bfc)
{
	LDLModel *ldlModel = modelLine->getModel();
	bool invert = modelLine->getBFCInvert();

	if (ldlModel)
	{
		const char *name = ldlModel->getName();
		TREModel *model = m_mainTREModel->modelNamed(name, bfc);

		if (model)
		{
			return addSubModel(modelLine, treModel, model, bfc && invert);
		}
		else
		{
			model = new TREModel;
			model->setMainModel(treModel->getMainModel());
			model->setName(name);
			model->setPartFlag(ldlModel->isPart());
			if (parseModel(ldlModel, model, bfc))
			{
				m_mainTREModel->registerModel(model, bfc);
				model->release();
				return addSubModel(modelLine, treModel, model, bfc && invert);
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

bool LDModelParser::isCyls(const char *filename)
{
	return isPrimitive(filename, "cyls.dat");
}

bool LDModelParser::isCyls2(const char *filename)
{
	return isPrimitive(filename, "cyls2.dat");
}

bool LDModelParser::isDisc(const char *filename)
{
	return isPrimitive(filename, "disc.dat");
}

bool LDModelParser::isNdis(const char *filename)
{
	return isPrimitive(filename, "ndis.dat");
}

bool LDModelParser::isEdge(const char *filename)
{
	return isPrimitive(filename, "edge.dat");
}

bool LDModelParser::isCcyli(const char *filename)
{
	return isPrimitive(filename, "ccyli.dat");
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

bool LDModelParser::isOldRing(const char *filename)
{
	int len = strlen(filename);

	if (len >= 9 && len <= 12 &&
		stringHasCaseInsensitivePrefix(filename, "ring") &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		int i;

		for (i = 4; i < len - 5; i++)
		{
			if (!isdigit(filename[i]))
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isRing(const char *filename)
{
	return strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "ring") &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::isRin(const char *filename)
{
	return strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "rin") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::isTorusO(const char *filename)
{
	return strlen(filename) == 12 && toupper(filename[0]) == 'T' &&
		isdigit(filename[1]) && isdigit(filename[2]) &&
		toupper(filename[3]) == 'O' && isdigit(filename[4]) &&
		isdigit(filename[7]) && isdigit(filename[6]) &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::isTorusI(const char *filename)
{
	return strlen(filename) == 12 && toupper(filename[0]) == 'T' &&
		isdigit(filename[1]) && isdigit(filename[2]) &&
		toupper(filename[3]) == 'I' && isdigit(filename[4]) &&
		isdigit(filename[7]) && isdigit(filename[6]) &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat");
}

bool LDModelParser::substituteStud(TREModel *treModel, int numSegments)
{
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, m_flags.bfc);
	treModel->addDisc(TCVector(0.0f, -4.0f, 0.0f), 6.0f, numSegments,
		numSegments, m_flags.bfc);
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

bool LDModelParser::substituteStu22(TREModel *treModel, bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, 0.0f,
		numSegments, numSegments, bfc);
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

bool LDModelParser::substituteStu23(TREModel *treModel, bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addDisc(TCVector(0.0f, -4.0f, 0.0f), 4.0f, numSegments,
		numSegments, bfc);
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

bool LDModelParser::substituteStu24(TREModel *treModel, bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 6.0f, 0.0f,
		numSegments, numSegments, bfc);
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

bool LDModelParser::substituteCylinder(TREModel *treModel, float fraction,
									   bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, numSegments,
		(int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder(TREModel *treModel, float fraction,
											 bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addSlopedCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, (int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder2(TREModel *treModel,
											  float fraction, bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addSlopedCylinder2(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, (int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteDisc(TREModel *treModel, float fraction, bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		(int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteNotDisc(TREModel *treModel, float fraction,
									  bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addNotDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		(int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteCircularEdge(TREModel *treModel, float fraction)
{
	if (m_flags.edgeLines)
	{
		int numSegments = getNumCircleSegments(fraction);

		treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
			(int)(numSegments * fraction));
	}
	return true;
}

bool LDModelParser::substituteCone(TREModel *treModel, float fraction, int size,
								   bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addOpenCone(TCVector(0.0f, 0.0f, 0.0f), (float)size + 1.0f,
		(float)size, 1.0f, numSegments, (int)(numSegments * fraction), bfc);
	return true;
}

bool LDModelParser::substituteRing(TREModel *treModel, float fraction, int size,
								   bool bfc)
{
	int numSegments = getNumCircleSegments(fraction);

	treModel->addRing(TCVector(0.0f, 0.0f, 0.0f), (float)size + 1.0f,
		(float)size, numSegments, (int)(numSegments * fraction), bfc);
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
												 TREModel *treModel, bool bfc)
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
			return substituteStu22(treModel, false, bfc);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu22a.dat") == 0)
		{
			return substituteStu22(treModel, true, bfc);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu23.dat") == 0)
		{
			return substituteStu23(treModel, false, bfc);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu23a.dat") == 0)
		{
			return substituteStu23(treModel, true, bfc);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu24.dat") == 0)
		{
			return substituteStu24(treModel, false, bfc);
		}
		else if (strcasecmp(modelName, "LDL-LOWRES:stu24a.dat") == 0)
		{
			return substituteStu24(treModel, true, bfc);
		}
		else if (strcasecmp(modelName, "stud.dat") == 0)
		{
			return substituteStud(treModel);
		}
		else if (strcasecmp(modelName, "1-8sphe.dat") == 0)
		{
			// Need to do eighth sphere substitution
		}
		else if (isCyli(modelName))
		{
			return substituteCylinder(treModel, startingFraction(modelName),
				bfc);
		}
		else if (isCyls(modelName))
		{
			return substituteSlopedCylinder(treModel,
				startingFraction(modelName), bfc);
		}
		else if (isCyls2(modelName))
		{
			return substituteSlopedCylinder2(treModel,
				startingFraction(modelName), bfc);
		}
		else if (isDisc(modelName))
		{
			return substituteDisc(treModel, startingFraction(modelName), bfc);
		}
		else if (isNdis(modelName))
		{
			return substituteNotDisc(treModel, startingFraction(modelName),
				bfc);
		}
		else if (isEdge(modelName))
		{
			return substituteCircularEdge(treModel,
				startingFraction(modelName));
		}
		else if (isCcyli(modelName))
		{
			// Need to do old-style torus substitution
		}
		else if (isCon(modelName))
		{
			int size;

			sscanf(modelName + 6, "%d", &size);
			return substituteCone(treModel, startingFraction(modelName), size,
				bfc);
		}
		else if (isOldRing(modelName))
		{
			int size;

			sscanf(modelName + 4, "%d", &size);
			return substituteRing(treModel, 1.0f, size, bfc);
		}
		else if (isRing(modelName))
		{
			int size;

			sscanf(modelName + 7, "%d", &size);
			return substituteRing(treModel, startingFraction(modelName), size,
				bfc);
		}
		else if (isRin(modelName))
		{
			int size;

			sscanf(modelName + 6, "%d", &size);
			return substituteRing(treModel, startingFraction(modelName), size,
				bfc);
		}
		else if (isTorusO(modelName))
		{
			// Need to do outer torus substitution
		}
		else if (isTorusI(modelName))
		{
			// Need to do inner torus substitution
		}
	}
	return false;
}

bool LDModelParser::parseModel(LDLModel *ldlModel, TREModel *treModel, bool bfc)
{
	BFCState newState = ldlModel->getBFCState();

	bfc = ((bfc && (newState == BFCOnState)) || newState == BFCForcedOnState)
		&& getBFCFlag();
	if (ldlModel && !performPrimitiveSubstitution(ldlModel, treModel, bfc))
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
						parseModel((LDLModelLine *)fileLine, treModel, bfc);
						break;
					case LDLLineTypeLine:
						parseLine((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeTriangle:
						parseTriangle((LDLShapeLine *)fileLine, treModel,
							bfc, false);
						break;
					case LDLLineTypeQuad:
						parseQuad((LDLShapeLine *)fileLine, treModel, bfc,
							false);
						break;
					case LDLLineTypeConditionalLine:
						parseConditionalLine((LDLConditionalLineLine *)fileLine,
							treModel);
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
		if (!getEdgesOnlyFlag())
		{
			treModel->addLine(shapeLine->getPoints());
		}
	}
	else if (colorNumber == 24)
	{
		if (getEdgeLinesFlag())
		{
			treModel->addEdgeLine(shapeLine->getPoints());
		}
	}
	else if (!getEdgesOnlyFlag())
	{
		treModel->addLine(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

void LDModelParser::parseConditionalLine(LDLConditionalLineLine
										 *conditionalLine,
										 TREModel *treModel)
{
	if (getConditionalLinesFlag())
	{
		treModel->addConditionalLine(conditionalLine->getPoints(),
			conditionalLine->getControlPoints());
	}
}

bool LDModelParser::shouldFlipWinding(bool invert, bool windingCCW)
{
	return (invert && windingCCW) || (!invert && !windingCCW);
}

void LDModelParser::parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel,
								  bool bfc, bool invert)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		if (bfc && shapeLine->getBFCOn())
		{
			if (shouldFlipWinding(invert, shapeLine->getBFCWindingCCW()))
			{
				TCVector points[3];

				points[0] = shapeLine->getPoints()[2];
				points[1] = shapeLine->getPoints()[1];
				points[2] = shapeLine->getPoints()[0];
				treModel->addBFCTriangle(points);
			}
			else
			{
				treModel->addBFCTriangle(shapeLine->getPoints());
			}
		}
		else
		{
			treModel->addTriangle(shapeLine->getPoints());
		}
	}
	else
	{
		if (bfc)
		{
			if (bfc && shapeLine->getBFCOn())
			{
				if (shouldFlipWinding(invert, shapeLine->getBFCWindingCCW()))
				{
					TCVector points[3];

					points[0] = shapeLine->getPoints()[2];
					points[1] = shapeLine->getPoints()[1];
					points[2] = shapeLine->getPoints()[0];
					treModel->addBFCTriangle(shapeLine->getParentModel()->
						getPackedRGBA(colorNumber), points);
				}
				else
				{
					treModel->addBFCTriangle(shapeLine->getParentModel()->
						getPackedRGBA(colorNumber), shapeLine->getPoints());
				}
			}
		}
		else
		{
			treModel->addTriangle(shapeLine->getParentModel()->
				getPackedRGBA(colorNumber), shapeLine->getPoints());
		}
	}
}

void LDModelParser::parseQuad(LDLShapeLine *shapeLine, TREModel *treModel,
							  bool bfc, bool invert)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		if (bfc && shapeLine->getBFCOn())
		{
			if (shouldFlipWinding(invert, shapeLine->getBFCWindingCCW()))
			{
				TCVector points[4];

				points[0] = shapeLine->getPoints()[3];
				points[1] = shapeLine->getPoints()[2];
				points[2] = shapeLine->getPoints()[1];
				points[3] = shapeLine->getPoints()[0];
				treModel->addBFCQuad(points);
			}
			else
			{
				treModel->addBFCQuad(shapeLine->getPoints());
			}
		}
		else
		{
			treModel->addQuad(shapeLine->getPoints());
		}
	}
	else
	{
		if (bfc)
		{
			if (bfc && shapeLine->getBFCOn())
			{
				if (shouldFlipWinding(invert, shapeLine->getBFCWindingCCW()))
				{
					TCVector points[4];

					points[0] = shapeLine->getPoints()[3];
					points[1] = shapeLine->getPoints()[2];
					points[2] = shapeLine->getPoints()[1];
					points[3] = shapeLine->getPoints()[0];
					treModel->addBFCQuad(shapeLine->getParentModel()->
						getPackedRGBA(colorNumber), points);
				}
				else
				{
					treModel->addBFCQuad(shapeLine->getParentModel()->
						getPackedRGBA(colorNumber), shapeLine->getPoints());
				}
			}
		}
		else
		{
			treModel->addQuad(shapeLine->getParentModel()->
				getPackedRGBA(colorNumber), shapeLine->getPoints());
		}
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

