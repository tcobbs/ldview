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
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <ctype.h>

static const int LO_NUM_SEGMENTS = 8;
static const int HI_NUM_SEGMENTS = 16;

LDModelParser::LDModelParser(void)
	:m_mainLDLModel(NULL),
	m_mainTREModel(NULL),
	m_curveQuality(2),
	m_seamWidth(0.0f),
	m_edgeLineWidth(0.0f),
	m_abort(false),
	m_studTextureFilter(GL_LINEAR_MIPMAP_LINEAR)
{
	m_flags.flattenParts = true;	// Supporting false here could take a lot
									// of work.
	m_flags.primitiveSubstitution = true;
	m_flags.seams = false;
	m_flags.edgeLines = false;
	m_flags.edgesOnly = false;
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
	m_flags.smoothCurves = true;
	m_flags.showAllConditional = false;
	m_flags.conditionalControlPoints = false;
	m_flags.defaultColorSet = false;
	m_flags.defaultColorNumberSet = false;
	m_flags.studLogo = false;
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

bool LDModelParser::shouldLoadConditionalLines(void)
{
	return (getEdgeLinesFlag() && getConditionalLinesFlag()) ||
		getSmoothCurvesFlag();
}

void LDModelParser::finishPart(TREModel *treModel, TRESubModel *subModel)
{
	if (getFlattenPartsFlag())
	{
		treModel->flatten();
	}
	if (getSmoothCurvesFlag())
	{
		treModel->smooth();
	}
	if (subModel)
	{
		if (getSeamsFlag() && !treModel->getNoShrinkFlag())
		{
			subModel->shrink(m_seamWidth);
		}
	}
}

void LDModelParser::setDefaultRGB(TCByte r, TCByte g, TCByte b,
								  bool transparent)
{
	m_defaultR = r;
	m_defaultG = g;
	m_defaultB = b;
	m_flags.defaultTrans = transparent;
	setDefaultColorSetFlag(true);
}

void LDModelParser::setDefaultColorNumber(int colorNumber)
{
	m_defaultColorNumber = colorNumber;
	setDefaultColorNumberSetFlag(true);
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
	m_mainTREModel->setRedBackFacesFlag(getRedBackFacesFlag());
	m_mainTREModel->setGreenFrontFacesFlag(getGreenFrontFacesFlag());
	m_mainTREModel->setCompilePartsFlag(getCompilePartsFlag());
	m_mainTREModel->setCompileAllFlag(getCompileAllFlag());
	m_mainTREModel->setPolygonOffsetFlag(getPolygonOffsetFlag());
	m_mainTREModel->setEdgeLineWidth(m_edgeLineWidth);
	m_mainTREModel->setAALinesFlag(getAALinesFlag());
	m_mainTREModel->setSortTransparentFlag(getSortTransparentFlag());
	m_mainTREModel->setStippleFlag(getStippleFlag());
	m_mainTREModel->setWireframeFlag(getWireframeFlag());
	m_mainTREModel->setConditionalLinesFlag(getConditionalLinesFlag());
	m_mainTREModel->setSmoothCurvesFlag(getSmoothCurvesFlag());
	m_mainTREModel->setShowAllConditionalFlag(getShowAllConditionalFlag());
	m_mainTREModel->setConditionalControlPointsFlag(
		getConditionalControlPointsFlag());
	m_mainTREModel->setStudLogoFlag(getStudLogoFlag());
	m_mainTREModel->setStudTextureFilter(getStudTextureFilter());
	if (getDefaultColorNumberSetFlag())
	{
		colorNumber = m_defaultColorNumber;
	}
	else if (getDefaultColorSetFlag())
	{
		colorNumber = palette->getColorNumberForRGB(m_defaultR, m_defaultG,
			m_defaultB, m_flags.defaultTrans);
	}
	edgeColorNumber = mainLDLModel->getEdgeColorNumber(colorNumber);
	m_mainTREModel->setColor(mainLDLModel->getPackedRGBA(colorNumber),
		mainLDLModel->getPackedRGBA(edgeColorNumber));
	if (parseModel(m_mainLDLModel, m_mainTREModel, getBFCFlag()))
	{
		if (m_mainTREModel->isPart() || getFileIsPartFlag())
		{
			m_mainTREModel->setPartFlag(true);
			finishPart(m_mainTREModel);
		}
		m_mainTREModel->finish();
		TCProgressAlert::send("LDModelParser",
			TCLocalStrings::get("ParsingStatus"), 1.0f, &m_abort);
		if (m_abort)
		{
			return false;
		}
		else
		{
			return m_mainTREModel->postProcess();
		}
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

	if (colorNumber == 16 || colorNumber == 24)
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
		treSubModel->setNonUniformFlag(modelLine->getNonUniformFlag());
		if (parentModel->hasSpecular(colorNumber))
		{
			GLfloat specular[4];

			parentModel->getSpecular(colorNumber, specular);
			treSubModel->setSpecular(specular);
		}
		if (parentModel->hasShininess(colorNumber))
		{
			GLfloat shininess;

			parentModel->getShininess(colorNumber, shininess);
			treSubModel->setShininess(shininess);
		}
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
			model->setNoShrinkFlag(ldlModel->getNoShrinkFlag());
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

TCFloat LDModelParser::startingFraction(const char *filename)
{
	int top;
	int bottom;

	if (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\"))
	{
		filename += 3;
	}
	sscanf(filename, "%d", &top);
	sscanf(filename + 2, "%d", &bottom);
	return (TCFloat)top / (TCFloat)bottom;
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

bool LDModelParser::isPrimitive(const char *filename, const char *suffix,
								bool *is48)
{
	int fileLen = strlen(filename);
	int suffixLen = strlen(suffix);

	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (((fileLen == suffixLen + 3 && startsWithFraction(filename)) ||
		(suffixLen <= 8 && fileLen == suffixLen + 4 &&
		startsWithFraction2(filename))) &&
		stringHasCaseInsensitiveSuffix(filename, suffix))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isPrimitive(filename + 3, suffix, NULL);
	}
	return false;
}

bool LDModelParser::isCyli(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyli.dat", is48);
}

bool LDModelParser::isCyls(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyls.dat", is48);
}

bool LDModelParser::isCyls2(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyls2.dat", is48);
}

bool LDModelParser::isChrd(const char *filename, bool *is48)
{
	return isPrimitive(filename, "chrd.dat", is48);
}

bool LDModelParser::isDisc(const char *filename, bool *is48)
{
	return isPrimitive(filename, "disc.dat", is48);
}

bool LDModelParser::isNdis(const char *filename, bool *is48)
{
	return isPrimitive(filename, "ndis.dat", is48);
}

bool LDModelParser::isEdge(const char *filename, bool *is48)
{
	return isPrimitive(filename, "edge.dat", is48);
}

/*
bool LDModelParser::isCcyli(const char *filename, bool *is48)
{
	return isPrimitive(filename, "ccyli.dat", is48);
}
*/

bool LDModelParser::is1DigitCon(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 11 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return is1DigitCon(filename + 3, NULL);
	}
	return false;
}

bool LDModelParser::is2DigitCon(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return is1DigitCon(filename + 3, NULL);
	}
	return false;
}

bool LDModelParser::isCon(const char *filename, bool *is48)
{
	return is1DigitCon(filename, is48) || is2DigitCon(filename, is48);
}

bool LDModelParser::isOldRing(const char *filename, bool *is48)
{
	int len = strlen(filename);

	if (is48 != NULL)
	{
		*is48 = false;
	}
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
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isOldRing(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isRing(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "ring") &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isRing(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isRin(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "rin") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isRin(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isTorus(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && toupper(filename[0]) == 'T' &&
		isdigit(filename[1]) && isdigit(filename[2]) &&
		isdigit(filename[4]) && isdigit(filename[7]) && isdigit(filename[6]) &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isTorus(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isTorusO(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'O';
		}
		else
		{
			return toupper(filename[3]) == 'O';
		}
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isTorusI(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'I';
		}
		else
		{
			return toupper(filename[3]) == 'I';
		}
	}
	else
	{
		return false;
	}
}

bool LDModelParser::isTorusQ(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'Q';
		}
		else
		{
			return toupper(filename[3]) == 'Q';
		}
	}
	else
	{
		return false;
	}
}

bool LDModelParser::substituteStud(TREModel *treModel, int numSegments)
{
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, getBFCFlag());
	treModel->addStudDisc(TCVector(0.0f, -4.0f, 0.0f), 6.0f, numSegments,
		numSegments, getBFCFlag());
	if (getEdgeLinesFlag())
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

	treModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 4.0f, -4.0f, numSegments,
		numSegments, bfc);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 6.0f, 0.0f,
		numSegments, numSegments, bfc);
	if (getEdgeLinesFlag())
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
	if (getEdgeLinesFlag())
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

	treModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 6.0f, -4.0f, numSegments,
		numSegments, bfc);
	treModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 4.0f, numSegments,
		numSegments, bfc);
	treModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 8.0f, 0.0f,
		numSegments, numSegments, bfc);
	if (getEdgeLinesFlag())
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

TCFloat LDModelParser::getTorusFraction(int size)
{
	int i;

	for (i = 0; i < 10; i++)
	{
		if (size == i + i * 10 + i * 100 + i * 1000)
		{
			return (TCFloat)i / 9.0f;
		}
	}
	return (TCFloat)size / 10000.0f;
}

bool LDModelParser::substituteTorusQ(TREModel *treModel, bool bfc, bool is48)
{
	int numSegments;
	int size;
	const char *modelName = treModel->getName();
	TCFloat fraction;
	int offset = 0;

	if (is48)
	{
		offset = 3;
	}
	sscanf(modelName + 1 + offset, "%d", &numSegments);
	sscanf(modelName + 4 + offset, "%d", &size);
	fraction = (TCFloat)numSegments / 16.0f;
	numSegments = getNumCircleSegments(fraction, is48);
	treModel->addTorusIO(true, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	treModel->addTorusIO(false, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	treModel->addTorusIO(true, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		-getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	treModel->addTorusIO(false, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		-getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteTorusIO(TREModel *treModel, bool inner, bool bfc,
									  bool is48)
{
	int numSegments;
	int size;
	const char *modelName = treModel->getName();
	TCFloat fraction;
	int offset = 0;

	if (is48)
	{
		offset = 3;
	}
	sscanf(modelName + 1 + offset, "%d", &numSegments);
	sscanf(modelName + 4 + offset, "%d", &size);
	fraction = (TCFloat)numSegments / 16.0f;
	numSegments = getNumCircleSegments(fraction, is48);
	treModel->addTorusIO(inner, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteEighthSphere(TREModel *treModel, bool bfc,
										   bool is48)
{
	int numSegments = getNumCircleSegments(1.0, is48);

	treModel->addEighthSphere(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		bfc);
	return true;
}

bool LDModelParser::substituteCylinder(TREModel *treModel, TCFloat fraction,
									   bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder(TREModel *treModel, TCFloat fraction,
											 bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addSlopedCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder2(TREModel *treModel,
											  TCFloat fraction, bool bfc,
											  bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addSlopedCylinder2(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteChrd(TREModel *treModel, TCFloat fraction, bool bfc,
								   bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addChrd(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteDisc(TREModel *treModel, TCFloat fraction, bool bfc,
								   bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteNotDisc(TREModel *treModel, TCFloat fraction,
									  bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addNotDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteCircularEdge(TREModel *treModel, TCFloat fraction,
										   bool is48)
{
	if (getEdgeLinesFlag())
	{
		int numSegments = getNumCircleSegments(fraction, is48);

		treModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
			getUsedCircleSegments(numSegments, fraction));
	}
	return true;
}

bool LDModelParser::substituteCone(TREModel *treModel, TCFloat fraction, int size,
								   bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addOpenCone(TCVector(0.0f, 0.0f, 0.0f), (TCFloat)size + 1.0f,
		(TCFloat)size, 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteRing(TREModel *treModel, TCFloat fraction, int size,
								   bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	treModel->addRing(TCVector(0.0f, 0.0f, 0.0f), (TCFloat)size,
		(TCFloat)size + 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

int LDModelParser::getUsedCircleSegments(int numSegments, TCFloat fraction)
{
	return (int)(numSegments * fraction + 0.000001);
}

int LDModelParser::getNumCircleSegments(TCFloat fraction, bool is48)
{
	int retValue = m_curveQuality * LO_NUM_SEGMENTS;

	if (is48 && retValue < 48)
	{
		retValue = 48;
	}
	if (fraction != 0.0f)
	{
		int i;
		
		for (i = m_curveQuality; !fEq(fraction * retValue,
			(TCFloat)getUsedCircleSegments(retValue, fraction)) && i < 12; i++)
		{
			int newValue = (i + 1) * LO_NUM_SEGMENTS;

			if (newValue > retValue)
			{
				retValue = newValue;
			}
		}
	}
	return retValue;
}

bool LDModelParser::performPrimitiveSubstitution(LDLModel *ldlModel,
												 TREModel *treModel, bool bfc)
{
	if (getPrimitiveSubstitutionFlag())
	{
		const char *modelName = ldlModel->getName();
		bool is48;

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
			return substituteEighthSphere(treModel, bfc);
		}
		else if (strcasecmp(modelName, "48/1-8sphe.dat") == 0 ||
			strcasecmp(modelName, "48\\1-8sphe.dat") == 0)
		{
			return substituteEighthSphere(treModel, bfc, true);
		}
		else if (isCyli(modelName, &is48))
		{
			return substituteCylinder(treModel, startingFraction(modelName),
				bfc, is48);
		}
		else if (isCyls(modelName, &is48))
		{
			return substituteSlopedCylinder(treModel,
				startingFraction(modelName), bfc, is48);
		}
		else if (isCyls2(modelName, &is48))
		{
			return substituteSlopedCylinder2(treModel,
				startingFraction(modelName), bfc, is48);
		}
		else if (isChrd(modelName, &is48))
		{
			return substituteChrd(treModel, startingFraction(modelName), bfc,
				is48);
		}
		else if (isDisc(modelName, &is48))
		{
			return substituteDisc(treModel, startingFraction(modelName), bfc,
				is48);
		}
		else if (isNdis(modelName, &is48))
		{
			return substituteNotDisc(treModel, startingFraction(modelName),
				bfc, is48);
		}
		else if (isEdge(modelName, &is48))
		{
			return substituteCircularEdge(treModel,
				startingFraction(modelName), is48);
		}
/*
		else if (isCcyli(modelName, &is48))
		{
			// The file now simply refers to a new torus.
			// Need to do old-style torus substitution
		}
*/
		else if (isCon(modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(modelName + 6 + offset, "%d", &size);
			return substituteCone(treModel, startingFraction(modelName), size,
				bfc, is48);
		}
		else if (isOldRing(modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(modelName + 4 + offset, "%d", &size);
			return substituteRing(treModel, 1.0f, size, bfc, is48);
		}
		else if (isRing(modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(modelName + 7 + offset, "%d", &size);
			return substituteRing(treModel, startingFraction(modelName), size,
				bfc, is48);
		}
		else if (isRin(modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(modelName + 6 + offset, "%d", &size);
			return substituteRing(treModel, startingFraction(modelName), size,
				bfc, is48);
		}
		else if (isTorusO(modelName, &is48))
		{
			return substituteTorusIO(treModel, false, bfc, is48);
		}
		else if (isTorusI(modelName, &is48))
		{
			return substituteTorusIO(treModel, true, bfc, is48);
		}
		else if (isTorusQ(modelName, &is48))
		{
			return substituteTorusQ(treModel, bfc, is48);
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

			//treModel->setName(ldlModel->getName()); Already done.
			for (i = 0; i < count && !m_abort; i++)
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
					default:
						break;
					}
				}
				if (ldlModel->isMainModel())
				{
					TCProgressAlert::send("LDLModelParser",
						TCLocalStrings::get("ParsingStatus"),
						(float)(i + 1) / (float)(count + 1), &m_abort);
				}
			}
		}
	}
	return !m_abort;
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
	if (shouldLoadConditionalLines())
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

void LDModelParser::setSeamWidth(TCFloat seamWidth)
{
	m_seamWidth = seamWidth;
	if (m_seamWidth)
	{
		setSeamsFlag(true);
	}
	else
	{
		setSeamsFlag(false);
	}
}

TCFloat LDModelParser::getSeamWidth(void)
{
	if (getSeamsFlag())
	{
		return m_seamWidth;
	}
	else
	{
		return 0.0f;
	}
}

