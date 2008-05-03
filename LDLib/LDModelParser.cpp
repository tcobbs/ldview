#include "LDModelParser.h"
#include "LDrawModelViewer.h"
#include "LDObiInfo.h"

#include <string.h>

#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLShapeLine.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLCommentLine.h>
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

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

static const int LO_NUM_SEGMENTS = 8;
static const int HI_NUM_SEGMENTS = 16;


LDModelParser::LDModelParser(LDrawModelViewer *modelViewer)
	:m_modelViewer(modelViewer),
	m_mainLDLModel(NULL),
	m_mainTREModel(NULL),
	m_seamWidth(0.0f),
	m_obiInfo(NULL),
	m_obiUniqueId(0),
	m_abort(false)
{
	TCByte defaultR, defaultG, defaultB;
	bool defaultTrans;
	int defaultColorNumber;

	m_flags.flattenParts = true;	// Supporting false here could take a lot
									// of work.
	m_flags.seams = false;
	m_flags.defaultColorSet = false;
	m_flags.defaultColorNumberSet = false;
	setCurveQuality(m_modelViewer->getCurveQuality());
	setNoLightGeomFlag(m_modelViewer->getNoLightGeom());
	setPrimitiveSubstitutionFlag(
		m_modelViewer->getAllowPrimitiveSubstitution());
	setSeamWidth(m_modelViewer->getSeamWidth());
	m_modelViewer->getDefaultRGB(defaultR, defaultG, defaultB, defaultTrans);
	setDefaultRGB(defaultR, defaultG, defaultB, defaultTrans);
	defaultColorNumber = m_modelViewer->getDefaultColorNumber();
	if (defaultColorNumber == -1)
	{
		TCByte r, g, b;
		bool transparent;

		modelViewer->getDefaultRGB(r, g, b, transparent);
		LDLMainModel *mainModel = modelViewer->getMainModel();

		if (mainModel)
		{
			LDLPalette *palette = mainModel->getPalette();

			setDefaultColorNumber(palette->getColorNumberForRGB(r, g, b,
				transparent));
		}
	}
	else
	{
		setDefaultColorNumber(defaultColorNumber);
	}
	m_flags.boundingBoxesOnly = m_modelViewer->getBoundingBoxesOnly();
	m_flags.obi = m_modelViewer->getObi();
}

LDModelParser::~LDModelParser(void)
{
}

void LDModelParser::dealloc(void)
{
	TCObject::release(m_mainLDLModel);
	TCObject::release(m_mainTREModel);
	LDLPrimitiveCheck::dealloc();
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
	int colorNumber = 7;
	int edgeColorNumber;
	LDLPalette *palette = mainLDLModel->getPalette();

	m_mainLDLModel = (LDLMainModel *)mainLDLModel->retain();
	m_mainTREModel = new TREMainModel;
	m_mainTREModel->setAlertSender(m_alertSender);
	m_mainTREModel->setMultiThreadedFlag(getMultiThreadedFlag());
	m_mainTREModel->setPartFlag(mainLDLModel->isPart());
	m_mainTREModel->setEdgeLinesFlag(getEdgeLinesFlag());
	m_mainTREModel->setEdgesOnlyFlag(getEdgesOnlyFlag());
	m_mainTREModel->setLightingFlag(getLightingFlag());
	m_mainTREModel->setTwoSidedLightingFlag(getTwoSidedLightingFlag());
	m_mainTREModel->setBFCFlag(getBFCFlag());
	m_mainTREModel->setRedBackFacesFlag(getRedBackFacesFlag());
	m_mainTREModel->setGreenFrontFacesFlag(getGreenFrontFacesFlag());
	switch (m_modelViewer->getMemoryUsage())
	{
	case 0:
		m_mainTREModel->setCompilePartsFlag(false);
		m_mainTREModel->setCompileAllFlag(false);
		m_mainTREModel->setFlattenConditionalsFlag(false);
		break;
	case 1:
		m_mainTREModel->setCompilePartsFlag(true);
		m_mainTREModel->setCompileAllFlag(false);
		m_mainTREModel->setFlattenConditionalsFlag(false);
		break;
	case 2:
		m_mainTREModel->setCompilePartsFlag(true);
		m_mainTREModel->setCompileAllFlag(true);
		m_mainTREModel->setFlattenConditionalsFlag(true);
		break;
	}
	m_mainTREModel->setPolygonOffsetFlag(getPolygonOffsetFlag());
	m_mainTREModel->setEdgeLineWidth(m_modelViewer->getHighlightLineWidth());
	m_mainTREModel->setStudAnisoLevel(m_modelViewer->getAnisoLevel());
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
	m_mainTREModel->setStudTextureFilter(m_modelViewer->getTextureFilterType());
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
	m_obiTokens.clear();
	if (parseModel(m_mainLDLModel, m_mainTREModel, getBFCFlag(),
		m_defaultColorNumber))
	{
		if (m_mainTREModel->isPart() || getFileIsPartFlag())
		{
			m_mainTREModel->setPartFlag(true);
			finishPart(m_mainTREModel);
		}
		m_mainTREModel->finish();
		TCProgressAlert::send("LDModelParser",
			TCLocalStrings::get(_UC("ParsingStatus")), 1.0f, &m_abort, this);
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

bool LDModelParser::getFileIsPartFlag(void) const
{
	return m_modelViewer->getFileIsPart();
}

bool LDModelParser::getEdgeLinesFlag(void) const
{
	return m_modelViewer->getShowsHighlightLines();
}

bool LDModelParser::getEdgesOnlyFlag(void) const
{
	return m_modelViewer->getEdgesOnly();
}

bool LDModelParser::getLightingFlag(void) const
{
	return m_modelViewer->getUseLighting();
}

bool LDModelParser::getTwoSidedLightingFlag(void) const
{
	return m_modelViewer->forceOneLight();
}

bool LDModelParser::getBFCFlag(void) const
{
	return m_modelViewer->getBfc();
}

bool LDModelParser::getAALinesFlag(void) const
{
	return m_modelViewer->getLineSmoothing();
}

bool LDModelParser::getSortTransparentFlag(void) const
{
	return m_modelViewer->getSortTransparent();
}

bool LDModelParser::getStippleFlag(void) const
{
	return m_modelViewer->getUseStipple();
}

bool LDModelParser::getWireframeFlag(void) const
{
	return m_modelViewer->getDrawWireframe();
}

bool LDModelParser::getConditionalLinesFlag(void) const
{
	return m_modelViewer->getDrawConditionalHighlights();
}

bool LDModelParser::getSmoothCurvesFlag(void) const
{
	return m_modelViewer->getPerformSmoothing();
}

bool LDModelParser::getShowAllConditionalFlag(void) const
{
	return m_modelViewer->getShowAllConditionalLines();
}

bool LDModelParser::getConditionalControlPointsFlag(void) const
{
	return m_modelViewer->getShowConditionalControlPoints();
}

bool LDModelParser::getPolygonOffsetFlag(void) const
{
	return m_modelViewer->getUsePolygonOffset();
}

bool LDModelParser::getStudLogoFlag(void) const
{
	return m_modelViewer->getTextureStuds();
}

bool LDModelParser::getRedBackFacesFlag(void) const
{
	return m_modelViewer->getRedBackFaces();
}

bool LDModelParser::getGreenFrontFacesFlag(void) const
{
	return m_modelViewer->getGreenFrontFaces();
}

bool LDModelParser::getMultiThreadedFlag(void) const
{
	return m_modelViewer->getMultiThreaded();
}

bool LDModelParser::addSubModel(LDLModelLine *modelLine,
								TREModel *treParentModel,
								TREModel *treModel, bool invert)
{
	int colorNumber = actualColorNumber(modelLine);
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
		if (strcasecmp(treModel->getName(), "light.dat") == 0)
		{
			treSubModel->setLightFlag(true);
		}
	}
	if (TCVector::determinant(treSubModel->getMatrix()) < 0.0f)
	{
		// Generate an inverted version of the child model, because this
		// reference to it involves a mirror.
		treModel->getUnMirroredModel();
	}
	return true;
}

void LDModelParser::addBoundingQuad(
	TREModel *model,
	const TCVector *minMax,
	int face)
{
	TCVector quad[4];
	int faceIndices[][4][3] =
	{
		{
			{ 0, 0, 0 },
			{ 0, 1, 0 },
			{ 1, 1, 0 },
			{ 1, 0, 0 }
		},
		{
			{ 1, 1, 1 },
			{ 0, 1, 1 },
			{ 0, 0, 1 },
			{ 1, 0, 1 }
		},
		{
			{ 0, 0, 0 },
			{ 0, 0, 1 },
			{ 0, 1, 1 },
			{ 0, 1, 0 }
		},
		{
			{ 1, 1, 1 },
			{ 1, 0, 1 },
			{ 1, 0, 0 },
			{ 1, 1, 0 }
		},
		{
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 1, 0, 1 },
			{ 0, 0, 1 }
		},
		{
			{ 1, 1, 1 },
			{ 1, 1, 0 },
			{ 0, 1, 0 },
			{ 0, 1, 1 }
		},
	};
	for (int q = 0; q < 4; q++)
	{
		for (int i = 0; i < 3; i++)
		{
			quad[q][i] = minMax[faceIndices[face][q][i]][i];
		}
	}
	model->addBFCQuad(quad);
}

int LDModelParser::getActiveColorNumber(
	LDLModelLine *modelLine,
	int activeColorNumber)
{
	if (modelLine)
	{
		int modelLineColorNumber = modelLine->getColorNumber();

		if (modelLineColorNumber != 16)
		{
			if (modelLineColorNumber == 24)
			{
				LDLModel *ldlModel = modelLine->getModel();

				if (ldlModel)
				{
					return ldlModel->getEdgeColorNumber(activeColorNumber);
				}
			}
			else
			{
				return modelLineColorNumber;
			}
		}
	}
	return activeColorNumber;
}

std::string LDModelParser::modelNameKey(LDLModel *model, int activeColorNumber)
{
	const char *name = model->getName();

	if (model->colorNumberIsTransparent(activeColorNumber))
	{
		std::string nameKey;

		m_obiInfo->end();
		nameKey = "Trans:";
		nameKey += name;
		return nameKey;
	}
	else if (m_obiTokens.size() > 0)
	{
		std::string nameKey;
		char num[32];

		sprintf(num, "%lX:", m_obiUniqueId++);
		nameKey = num;
		nameKey += name;
		return nameKey;
	}
	else
	{
		return name;
	}
}

bool LDModelParser::parseModel(
	LDLModelLine *modelLine,
	TREModel *treModel,
	bool bfc,
	int activeColorNumber)
{
	LDLModel *ldlModel = modelLine->getModel();
	bool invert = modelLine->getBFCInvert();

	activeColorNumber = getActiveColorNumber(modelLine, activeColorNumber);
	if (ldlModel)
	{
		TREModel *model = NULL;
		std::string nameKey = modelNameKey(ldlModel, activeColorNumber);

		model = m_mainTREModel->modelNamed(nameKey.c_str(), bfc);
		if (model)
		{
			return addSubModel(modelLine, treModel, model, bfc && invert);
		}
		else
		{
			model = new TREModel;
			model->setMainModel(treModel->getMainModel());
			model->setName(nameKey.c_str());
			model->setPartFlag(ldlModel->isPart());
			model->setNoShrinkFlag(ldlModel->getNoShrinkFlag());
			if (m_flags.boundingBoxesOnly && ldlModel->isPart())
			{
				TCVector minMax[2];

				m_mainTREModel->registerModel(model, bfc);
				model->release();
				ldlModel->getBoundingBox(minMax[0], minMax[1]);
				for (int i = 0; i < 6; i++)
				{
					addBoundingQuad(model, minMax, i);
				}
				return addSubModel(modelLine, treModel, model, bfc && invert);
			}
			else if (parseModel(ldlModel, model, bfc, activeColorNumber))
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

bool LDModelParser::substituteStud(int numSegments)
{
	TCULong blackColor = 0;

	if (m_flags.obi &&
		!m_mainLDLModel->colorNumberIsTransparent(m_currentColorNumber))
	{
		blackColor = m_mainLDLModel->getPackedRGBA(0);
	}
	m_currentTREModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f,
		numSegments, numSegments, getBFCFlag(), blackColor, blackColor);
	m_currentTREModel->addStudDisc(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
		numSegments, numSegments, getBFCFlag());
	if (getEdgeLinesFlag())
	{
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments, -1, blackColor);
		m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
			numSegments, -1, blackColor);
	}
	return true;
}

bool LDModelParser::substituteStud(void)
{
	return substituteStud(getNumCircleSegments());
}

bool LDModelParser::substituteStu2(void)
{
	return substituteStud(LO_NUM_SEGMENTS);
}

bool LDModelParser::substituteStu22(bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	m_currentTREModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 4.0f, -4.0f, numSegments,
		numSegments, bfc);
	m_currentTREModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 4.0f, numSegments,
		numSegments, bfc);
	m_currentTREModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 6.0f, 0.0f,
		numSegments, numSegments, bfc);
	if (getEdgeLinesFlag())
	{
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 4.0f,
			numSegments);
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments);
		if (!isA)
		{
			m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 4.0f,
				numSegments);
			m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteStu23(bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	m_currentTREModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 4.0f, 4.0f, numSegments,
		numSegments, bfc);
	m_currentTREModel->addDisc(TCVector(0.0f, -4.0f, 0.0f), 4.0f, numSegments,
		numSegments, bfc);
	if (getEdgeLinesFlag())
	{
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 4.0f,
			numSegments);
		if (!isA)
		{
			m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 4.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteStu24(bool isA, bool bfc)
{
	int numSegments = LO_NUM_SEGMENTS;

	m_currentTREModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 6.0f, -4.0f, numSegments,
		numSegments, bfc);
	m_currentTREModel->addCylinder(TCVector(0.0f, -4.0f, 0.0f), 8.0f, 4.0f, numSegments,
		numSegments, bfc);
	m_currentTREModel->addOpenCone(TCVector(0.0f, -4.0f, 0.0f), 6.0f, 8.0f, 0.0f,
		numSegments, numSegments, bfc);
	if (getEdgeLinesFlag())
	{
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 6.0f,
			numSegments);
		m_currentTREModel->addCircularEdge(TCVector(0.0f, -4.0f, 0.0f), 8.0f,
			numSegments);
		if (!isA)
		{
			m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 6.0f,
				numSegments);
			m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 8.0f,
				numSegments);
		}
	}
	return true;
}

bool LDModelParser::substituteTorusQ(TCFloat fraction, int size, bool bfc,
									 bool is48)
{
	int numSegments;

	numSegments = getNumCircleSegments(fraction, is48);
	m_currentTREModel->addTorusIO(true, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	m_currentTREModel->addTorusIO(false, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	m_currentTREModel->addTorusIO(true, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		-getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	m_currentTREModel->addTorusIO(false, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		-getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteTorusIO(bool inner, TCFloat fraction, int size,
									  bool bfc, bool is48)
{
	int numSegments;
	//int size;
	//const char *modelName = m_currentTREModel->getName();
	//TCFloat fraction;
	//int offset = 0;

	//if (is48)
	//{
	//	offset = 3;
	//}
	//sscanf(modelName + 1 + offset, "%d", &numSegments);
	//sscanf(modelName + 4 + offset, "%d", &size);
	//fraction = (TCFloat)numSegments / 16.0f;
	numSegments = getNumCircleSegments(fraction, is48);
	m_currentTREModel->addTorusIO(inner, TCVector(0.0f, 0.0f, 0.0f), 1.0f,
		getTorusFraction(size), numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteEighthSphere(bool bfc,
										   bool is48)
{
	int numSegments = getNumCircleSegments(1.0, is48);

	m_currentTREModel->addEighthSphere(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		bfc);
	return true;
}

bool LDModelParser::substituteCylinder(TCFloat fraction,
									   bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder(TCFloat fraction,
											 bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addSlopedCylinder(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteSlopedCylinder2(TCFloat fraction, bool bfc,
											  bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addSlopedCylinder2(TCVector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f,
		numSegments, getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteChrd(TCFloat fraction, bool bfc,
								   bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addChrd(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteDisc(TCFloat fraction, bool bfc,
								   bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteNotDisc(TCFloat fraction,
									  bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addNotDisc(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteCircularEdge(TCFloat fraction,
										   bool is48)
{
	if (getEdgeLinesFlag())
	{
		int numSegments = getNumCircleSegments(fraction, is48);

		m_currentTREModel->addCircularEdge(TCVector(0.0f, 0.0f, 0.0f), 1.0f, numSegments,
			getUsedCircleSegments(numSegments, fraction));
	}
	return true;
}

bool LDModelParser::substituteCone(TCFloat fraction, int size,
								   bool bfc, bool is48)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addOpenCone(TCVector(0.0f, 0.0f, 0.0f), (TCFloat)size + 1.0f,
		(TCFloat)size, 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::substituteRing(TCFloat fraction, int size,
								   bool bfc, bool is48, bool /*isOld*/)
{
	int numSegments = getNumCircleSegments(fraction, is48);

	m_currentTREModel->addRing(TCVector(0.0f, 0.0f, 0.0f), (TCFloat)size,
		(TCFloat)size + 1.0f, numSegments,
		getUsedCircleSegments(numSegments, fraction), bfc);
	return true;
}

bool LDModelParser::performPrimitiveSubstitution(
	LDLModel *ldlModel,
	TREModel *treModel,
	int activeColorNumber,
	bool bfc)
{
	m_currentTREModel = treModel;
	m_currentColorNumber = activeColorNumber;
	return LDLPrimitiveCheck::performPrimitiveSubstitution(ldlModel, bfc);
//	const char *modelName = ldlModel->getName();
//
//	if (getPrimitiveSubstitutionFlag())
//	{
//		bool is48;
//
//		if (!modelName)
//		{
//			return false;
//		}
//		if (strcasecmp(modelName, "LDL-LOWRES:stu2.dat") == 0)
//		{
//			return substituteStu2(treModel);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu22.dat") == 0)
//		{
//			return substituteStu22(treModel, false, bfc);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu22a.dat") == 0)
//		{
//			return substituteStu22(treModel, true, bfc);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu23.dat") == 0)
//		{
//			return substituteStu23(treModel, false, bfc);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu23a.dat") == 0)
//		{
//			return substituteStu23(treModel, true, bfc);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu24.dat") == 0)
//		{
//			return substituteStu24(treModel, false, bfc);
//		}
//		else if (strcasecmp(modelName, "LDL-LOWRES:stu24a.dat") == 0)
//		{
//			return substituteStu24(treModel, true, bfc);
//		}
//		else if (strcasecmp(modelName, "stud.dat") == 0)
//		{
//			return substituteStud(treModel);
//		}
//		else if (strcasecmp(modelName, "1-8sphe.dat") == 0)
//		{
//			return substituteEighthSphere(treModel, bfc);
//		}
//		else if (strcasecmp(modelName, "48/1-8sphe.dat") == 0 ||
//			strcasecmp(modelName, "48\\1-8sphe.dat") == 0)
//		{
//			return substituteEighthSphere(treModel, bfc, true);
//		}
//		else if (isCyli(modelName, &is48))
//		{
//			return substituteCylinder(treModel, startingFraction(modelName),
//				bfc, is48);
//		}
//		else if (isCyls(modelName, &is48))
//		{
//			return substituteSlopedCylinder(treModel,
//				startingFraction(modelName), bfc, is48);
//		}
//		else if (isCyls2(modelName, &is48))
//		{
//			return substituteSlopedCylinder2(treModel,
//				startingFraction(modelName), bfc, is48);
//		}
//		else if (isChrd(modelName, &is48))
//		{
//			return substituteChrd(treModel, startingFraction(modelName), bfc,
//				is48);
//		}
//		else if (isDisc(modelName, &is48))
//		{
//			return substituteDisc(treModel, startingFraction(modelName), bfc,
//				is48);
//		}
//		else if (isNdis(modelName, &is48))
//		{
//			return substituteNotDisc(treModel, startingFraction(modelName),
//				bfc, is48);
//		}
//		else if (isEdge(modelName, &is48))
//		{
//			return substituteCircularEdge(treModel,
//				startingFraction(modelName), is48);
//		}
///*
//		else if (isCcyli(modelName, &is48))
//		{
//			// The file now simply refers to a new torus.
//			// Need to do old-style torus substitution
//		}
//*/
//		else if (isCon(modelName, &is48))
//		{
//			int size;
//			int offset = 0;
//
//			if (is48)
//			{
//				offset = 3;
//			}
//			sscanf(modelName + 6 + offset, "%d", &size);
//			return substituteCone(treModel, startingFraction(modelName), size,
//				bfc, is48);
//		}
//		else if (isOldRing(modelName, &is48))
//		{
//			int size;
//			int offset = 0;
//
//			if (is48)
//			{
//				offset = 3;
//			}
//			sscanf(modelName + 4 + offset, "%d", &size);
//			return substituteRing(treModel, 1.0f, size, bfc, is48);
//		}
//		else if (isRing(modelName, &is48))
//		{
//			int size;
//			int offset = 0;
//
//			if (is48)
//			{
//				offset = 3;
//			}
//			sscanf(modelName + 7 + offset, "%d", &size);
//			return substituteRing(treModel, startingFraction(modelName), size,
//				bfc, is48);
//		}
//		else if (isRin(modelName, &is48))
//		{
//			int size;
//			int offset = 0;
//
//			if (is48)
//			{
//				offset = 3;
//			}
//			sscanf(modelName + 6 + offset, "%d", &size);
//			return substituteRing(treModel, startingFraction(modelName), size,
//				bfc, is48);
//		}
//		else if (isTorusO(modelName, &is48))
//		{
//			return substituteTorusIO(treModel, false, bfc, is48);
//		}
//		else if (isTorusI(modelName, &is48))
//		{
//			return substituteTorusIO(treModel, true, bfc, is48);
//		}
//		else if (isTorusQ(modelName, &is48))
//		{
//			return substituteTorusQ(treModel, bfc, is48);
//		}
//	}
//	if (getNoLightGeomFlag())
//	{
//		if (modelName && strcasecmp(modelName, "light.dat") == 0)
//		{
//			// Don't draw any geometry for light.dat.
//			return true;
//		}
//	}
//	return false;
}

bool LDModelParser::parseModel(
	LDLModel *ldlModel,
	TREModel *treModel,
	bool bfc,
	int activeColorNumber)
{
	BFCState newState = ldlModel->getBFCState();
	LDObiInfo obiInfo;
	LDObiInfo *origObiInfo = m_obiInfo;

	if (m_obiInfo != NULL && m_obiInfo->isActive() &&
		!ldlModel->colorNumberIsTransparent(activeColorNumber))
	{
		obiInfo.start(m_obiInfo->getColor(), m_obiInfo->getEdgeColor(), true);
	}
	m_obiInfo = &obiInfo;
	bfc = ((bfc && (newState == BFCOnState)) || newState == BFCForcedOnState)
		&& getBFCFlag();
	if (ldlModel && !performPrimitiveSubstitution(ldlModel, treModel,
		activeColorNumber, bfc))
	{
		LDLFileLineArray *fileLines = ldlModel->getFileLines();

		if (fileLines)
		{
			int i;
			int count = ldlModel->getActiveLineCount();
			StringIntMap biLocalTokens;

			for (i = 0; i < count && !m_abort; i++)
			{
				LDLFileLine *fileLine = (*fileLines)[i];

				if (fileLine->isValid())
				{
					if (fileLine->isActionLine())
					{
						//if (m_flags.obi)
						//{
						//	((LDLActionLine *)fileLine)->setObiOverrideActive(
						//		!ldlModel->colorNumberIsTransparent(
						//		activeColorNumber));
						//}
						switch (fileLine->getLineType())
						{
						case LDLLineTypeModel:
							parseModel((LDLModelLine *)fileLine, treModel, bfc,
								activeColorNumber);
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
							parseConditionalLine(
								(LDLConditionalLineLine *)fileLine, treModel);
							break;
						default:
							break;
						}
						m_obiInfo->actionHappened();
					}
					else if (fileLine->getLineType() == LDLLineTypeComment)
					{
						parseCommentLine((LDLCommentLine *)fileLine, treModel,
							biLocalTokens);
					}
				}
				if (ldlModel->isMainModel())
				{
					TCProgressAlert::send("LDLModelParser",
						TCLocalStrings::get(_UC("ParsingStatus")),
						(float)(i + 1) / (float)(count + 1), &m_abort, this);
				}
			}
			for (StringIntMap::iterator it = biLocalTokens.begin();
				it != biLocalTokens.end(); it++)
			{
				unsetToken(m_obiTokens, it->first.c_str(), it->second);
			}
		}
	}
	m_obiInfo = origObiInfo;
	return !m_abort;
}

// Note: static method
bool LDModelParser::unsetToken(
	StringIntMap &tokens,
	const char *token,
	int count /*= 1*/)
{
	StringIntMap::iterator it = tokens.find(token);

	if (it != tokens.end())
	{
		it->second -= count;
		if (it->second <= 0)
		{
			tokens.erase(it);
		}
		return true;
	}
	return false;
}

void LDModelParser::parseCommentLine(
	LDLCommentLine *commentLine,
	TREModel *treModel,
	StringIntMap &biLocalTokens)
{
	if (commentLine->isStepMeta())
	{
		treModel->nextStep();
	}
	else if (commentLine->isOBIMeta() && m_flags.obi)
	{
		switch (commentLine->getOBICommand())
		{
		case LDLCommentLine::OBICSet:
			if (commentLine->hasOBIToken())
			{
				std::string token = commentLine->getOBIToken();

				convertStringToLower(&token[0]);
				m_obiTokens[token]++;
				biLocalTokens[token]++;
			}
			break;
		case LDLCommentLine::OBICUnset:
			if (commentLine->hasOBIToken())
			{
				std::string token = commentLine->getOBIToken();

				convertStringToLower(&token[0]);
				if (unsetToken(biLocalTokens, token.c_str()))
				{
					// Only unset from the all list if the token was found in
					// the local list.  Models aren't allowed to unset tokens
					// from their parent model.
					unsetToken(m_obiTokens, token.c_str());
				}
			}
			break;
		case LDLCommentLine::OBICNext:
		case LDLCommentLine::OBICStart:
			m_obiInfo->start(commentLine, m_obiTokens);
			break;
		case LDLCommentLine::OBICEnd:
			m_obiInfo->end();
			break;
		default:
			// Gets rid of warning.
			break;
		}
	}
}

void LDModelParser::parseLine(LDLShapeLine *shapeLine, TREModel *treModel)
{
	int colorNumber = actualColorNumber(shapeLine);
	//TCULong colorNumber = shapeLine->getColorNumber();

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
		int colorNumber = conditionalLine->getColorNumber();
		TCULong color = 0;

		if (colorNumber != 24)
		{
			color = conditionalLine->getParentModel()->getPackedRGBA(
				colorNumber);
		}
		treModel->addConditionalLine(conditionalLine->getPoints(),
			conditionalLine->getControlPoints(), color);
	}
}

bool LDModelParser::shouldFlipWinding(bool invert, bool windingCCW)
{
	return (invert && windingCCW) || (!invert && !windingCCW);
}

void LDModelParser::parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel,
								  bool bfc, bool invert)
{
	int colorNumber = actualColorNumber(shapeLine);
	//TCULong colorNumber = shapeLine->getColorNumber();

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

int LDModelParser::actualColorNumber(LDLActionLine *actionLine)
{
	int colorNumber = actionLine->getColorNumber();
	TCULong color;

	switch (colorNumber)
	{
	case 16:
		color = m_obiInfo->getColor();
		break;
	case 24:
		color = m_obiInfo->getEdgeColor();
		break;
	default:
		color = 0;
		break;
	}
	if (color == 0)
	{
		return colorNumber;
	}
	else
	{
		return LDLPalette::colorNumberForPackedRGBA(color);
	}
}

void LDModelParser::parseQuad(LDLShapeLine *shapeLine, TREModel *treModel,
							  bool bfc, bool invert)
{
	int colorNumber = actualColorNumber(shapeLine);

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

