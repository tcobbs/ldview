#include "TREModel.h"
#include "TRESubModel.h"
#include "TREMainModel.h"
#include "TREShapeGroup.h"
#include "TREColoredShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREGL.h"

#include <TCFoundation/mystring.h>
#include <TCFoundation/TCMacros.h>

bool TREModel::sm_normalizeOn = false;

TREModel::TREModel(void)
	:m_name(NULL),
	m_mainModel(NULL),
	m_subModels(NULL),
	m_unMirroredModel(NULL),
	m_invertedModel(NULL),
	m_sectionsPresent(0),
	m_coloredSectionsPresent(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = NULL;
		m_coloredShapes[i] = NULL;
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
	m_flags.part = false;
	m_flags.boundingBox = false;
	m_flags.unshrunkNormals = false;
	m_flags.unMirrored = false;
	m_flags.inverted = false;
	m_flags.flattened = false;
}

TREModel::TREModel(const TREModel &other)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_unMirroredModel((TREModel *)TCObject::copy(other.m_unMirroredModel)),
	m_invertedModel((TREModel *)TCObject::copy(other.m_invertedModel)),
	m_sectionsPresent(other.m_sectionsPresent),
	m_coloredSectionsPresent(other.m_coloredSectionsPresent),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = (TREShapeGroup *)TCObject::copy(other.m_shapes[i]);
		m_coloredShapes[i] =
			(TREColoredShapeGroup *)TCObject::copy(other.m_coloredShapes[i]);
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
}

TREModel::TREModel(const TREModel &other, bool shallow)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels(shallow ? NULL :
		(TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_unMirroredModel(shallow ? NULL :
		(TREModel *)TCObject::copy(other.m_unMirroredModel)),
	m_invertedModel(shallow ? NULL :
		(TREModel *)TCObject::copy(other.m_invertedModel)),
	m_sectionsPresent(other.m_sectionsPresent),
	m_coloredSectionsPresent(other.m_coloredSectionsPresent),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = (TREShapeGroup *)TCObject::copy(other.m_shapes[i]);
		m_coloredShapes[i] =
			(TREColoredShapeGroup *)TCObject::copy(other.m_coloredShapes[i]);
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
}

TREModel::~TREModel(void)
{
}

void TREModel::dealloc(void)
{
	int i;

	delete m_name;
	// Don't release m_mainModel
	TCObject::release(m_subModels);
	if (m_unMirroredModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_unMirroredModel->m_unMirroredModel = NULL;
	}
	if (m_invertedModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_invertedModel->m_invertedModel = NULL;
	}
	if (!m_flags.unMirrored)
	{
		TCObject::release(m_unMirroredModel);
	}
	m_unMirroredModel = NULL;
	if (!m_flags.inverted)
	{
		TCObject::release(m_invertedModel);
	}
	m_invertedModel = NULL;
	for (i = 0; i <= TREMLast; i++)
	{
		TCULong listID = m_listIDs[i];

		if (listID)
		{
			glDeleteLists(listID, 1);
		}
		listID = m_coloredListIDs[i];
		if (listID)
		{
			glDeleteLists(listID, 1);
		}
		TCObject::release(m_shapes[i]);
		TCObject::release(m_coloredShapes[i]);
	}
	TCObject::dealloc();
}

TCObject *TREModel::copy(void)
{
	return new TREModel(*this);
}

TREModel *TREModel::shallowCopy(void)
{
	return new TREModel(*this, true);
}

TREModel *TREModel::getUnMirroredModel(void)
{
	if (!m_unMirroredModel)
	{
		m_unMirroredModel = shallowCopy();
		m_unMirroredModel->unMirror(this);
	}
	return m_unMirroredModel;
}

TREModel *TREModel::getInvertedModel(void)
{
	if (!m_invertedModel)
	{
		m_invertedModel = shallowCopy();
		m_invertedModel->invert(this);
	}
	return m_invertedModel;
}

void TREModel::unMirror(TREModel *originalModel)
{
	int i;

	m_unMirroredModel = originalModel;
	m_flags.unMirrored = true;
	if (m_unMirroredModel->m_invertedModel)
	{
		m_invertedModel = m_unMirroredModel->m_invertedModel->m_unMirroredModel;
		if (m_invertedModel)
		{
			m_invertedModel->m_invertedModel = this;
		}
	}
	if (originalModel->m_subModels)
	{
		int i;
		int count;

		count = originalModel->m_subModels->getCount();
		m_subModels = new TRESubModelArray(count);
		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel =
				(*originalModel->m_subModels)[i]->getUnMirroredSubModel();

			m_subModels->addObject(subModel);
		}
	}
	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];
			TREColoredShapeGroup *coloredShapeGroup = m_coloredShapes[i];

			if (shapeGroup)
			{
				shapeGroup->unMirror();
			}
			if (coloredShapeGroup)
			{
				coloredShapeGroup->unMirror();
			}
		}
	}
}

void TREModel::invert(TREModel *originalModel)
{
	int i;

	m_invertedModel = originalModel;
	m_flags.inverted = true;
	if (m_invertedModel->m_unMirroredModel)
	{
		m_unMirroredModel = m_invertedModel->m_unMirroredModel->m_invertedModel;
		if (m_unMirroredModel)
		{
			m_unMirroredModel->m_unMirroredModel = this;
		}
	}
	if (originalModel->m_subModels)
	{
		int i;
		int count;

		count = originalModel->m_subModels->getCount();
		m_subModels = new TRESubModelArray(count);
		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel =
				(*originalModel->m_subModels)[i]->getInvertedSubModel();

			m_subModels->addObject(subModel);
		}
	}
	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];
			TREColoredShapeGroup *coloredShapeGroup = m_coloredShapes[i];

			if (shapeGroup)
			{
				shapeGroup->invert();
			}
			if (coloredShapeGroup)
			{
				coloredShapeGroup->invert();
			}
		}
	}
}

void TREModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

void TREModel::compile(TREMSection section, bool colored)
{
	int *listIDs;

	if (colored)
	{
		listIDs = m_coloredListIDs;
	}
	else
	{
		listIDs = m_listIDs;
	}
	if (!listIDs[section] && isSectionPresent(section, colored))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getEffectiveModel()->compile(section,
					colored);
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			draw(section, colored);
			glEndList();
			listIDs[section] = listID;
//			debugPrintf("U Standard <<%s>> %d %d\n", m_name, m_flags.unMirrored,
//				m_flags.inverted);
		}
	}
}

void TREModel::compileDefaultColor(void)
{
	compile(TREMStandard, false);
/*
	if (!m_listIDs[TREMStandard] && isSectionPresent(TREMStandard, false))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getEffectiveModel()->compileDefaultColor();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawDefaultColor();
			glEndList();
			m_listIDs[TREMStandard] = listID;
			debugPrintf("U Standard <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileBFC(void)
{
	compile(TREMBFC, false);
/*
	if (!m_listIDs[TREMBFC] && isSectionPresent(TREMBFC, false))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getEffectiveModel()->compileBFC();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawBFC();
			glEndList();
			m_listIDs[TREMBFC] = listID;
			debugPrintf("U BFC      <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileDefaultColorLines(void)
{
	compile(TREMLines, false);
/*
	if (!m_listIDs[TREMLines] && isSectionPresent(TREMLines, false))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileDefaultColorLines();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawDefaultColorLines();
			glEndList();
			m_listIDs[TREMLines] = listID;
			debugPrintf("U Lines    <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileEdgeLines(void)
{
	compile(TREMEdgeLines, false);
/*
	if (!m_listIDs[TREMEdgeLines] && isSectionPresent(TREMEdgeLines, false))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileEdgeLines();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawEdgeLines();
			glEndList();
			m_listIDs[TREMEdgeLines] = listID;
			debugPrintf("U Edges    <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileColored(void)
{
	compile(TREMStandard, true);
/*
	if (!m_coloredListIDs[TREMStandard] &&
		isSectionPresent(TREMStandard, true))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getEffectiveModel()->compileColored();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawColored();
			glEndList();
			m_coloredListIDs[TREMStandard] = listID;
			debugPrintf("C Standard <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileColoredBFC(void)
{
	compile(TREMBFC, true);
/*
	if (!m_coloredListIDs[TREMBFC] && isSectionPresent(TREMBFC, true))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getEffectiveModel()->compileColoredBFC();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawColoredBFC();
			glEndList();
			m_coloredListIDs[TREMBFC] = listID;
			debugPrintf("C BFC      <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileColoredLines(void)
{
	compile(TREMLines, true);
/*
	if (!m_coloredListIDs[TREMLines] && isSectionPresent(TREMLines, true))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileColoredLines();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawColoredLines();
			glEndList();
			m_coloredListIDs[TREMLines] = listID;
			debugPrintf("C Lines    <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
*/
}

void TREModel::compileColoredEdgeLines(void)
{
	compile(TREMEdgeLines, true);
/*
	if (!m_coloredListIDs[TREMEdgeLines] &&
		isSectionPresent(TREMEdgeLines, true))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileColoredEdgeLines();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawColoredEdgeLines();
			glEndList();
			m_coloredListIDs[TREMEdgeLines] = listID;
			debugPrintf("C Edges    <<%s>> %d %d\n", m_name, m_flags.unMirrored,
				m_flags.inverted);
		}
	}
	if (!isSectionPresent(TREMEdgeLines, true))
	{
		printf("good\n");
	}
*/
}

void TREModel::draw(TREMSection section)
{
	draw(section, false);
}

void TREModel::draw(TREMSection section, bool colored)
{
	TCULong listID;

	if (colored)
	{
		listID = m_coloredListIDs[section];
	}
	else
	{
		listID = m_listIDs[section];
	}
	if (listID)
	{
		glCallList(listID);
	}
	else if (isSectionPresent(section, colored))
	{
		TREShapeGroup *shapeGroup;

		if (colored)
		{
			shapeGroup = m_coloredShapes[section];
		}
		else
		{
			shapeGroup = m_shapes[section];
		}
		if (isLineSection(section))
		{
			if (shapeGroup)
			{
				shapeGroup->drawLines();
			}
		}
		else
		{
			if (m_flags.part && isFlattened())
			{
				setGlNormalize(false);
			}
			else
			{
				setGlNormalize(true);
			}
			if (shapeGroup)
			{
				shapeGroup->draw();
			}
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->draw(section, colored);
			}
		}
	}
}

void TREModel::drawColored(TREMSection section)
{
	draw(section, true);
}

void TREModel::drawDefaultColor(void)
{
	draw(TREMStandard);
}

void TREModel::drawBFC(void)
{
	draw(TREMBFC);
}

void TREModel::drawDefaultColorLines(void)
{
	draw(TREMLines);
}

void TREModel::drawEdgeLines(void)
{
	draw(TREMEdgeLines);
}

void TREModel::drawColored(void)
{
	drawColored(TREMStandard);
}

void TREModel::drawColoredBFC(void)
{
	drawColored(TREMBFC);
}

void TREModel::drawColoredLines(void)
{
	drawColored(TREMLines);
}

void TREModel::drawColoredEdgeLines(void)
{
	drawColored(TREMEdgeLines);
}

void TREModel::setup(TREMSection section)
{
	if (!m_shapes[section])
	{
		TREShapeGroup *shapeGroup = new TREShapeGroup;

		shapeGroup->setVertexStore(m_mainModel->getVertexStore());
		// No need to release previous, since we determined it is NULL.
		m_shapes[section] = shapeGroup;
		// Don't release shapeGroup, becase m_shapes isn't a TCObjectArray.
	}
}

void TREModel::setupColored(TREMSection section)
{
	if (!m_coloredShapes[section])
	{
		TREColoredShapeGroup *shapeGroup = new TREColoredShapeGroup;

		shapeGroup->setVertexStore(m_mainModel->getColoredVertexStore());
		// No need to release previous, since we determined it is NULL.
		m_coloredShapes[section] = shapeGroup;
		// Don't release shapeGroup, becase m_shapes isn't a TCObjectArray.
	}
}

void TREModel::setupStandard(void)
{
	setup(TREMStandard);
}

void TREModel::setupLines(void)
{
	setup(TREMLines);
}

void TREModel::setupBFC(void)
{
	setup(TREMBFC);
}

void TREModel::setupEdges(void)
{
	setup(TREMEdgeLines);
}

void TREModel::setupColored(void)
{
	setupColored(TREMStandard);
}

void TREModel::setupColoredLines(void)
{
	setupColored(TREMLines);
}

void TREModel::setupColoredBFC(void)
{
	setupColored(TREMBFC);
}

void TREModel::setupColoredEdges(void)
{
	setupColored(TREMEdgeLines);
}

void TREModel::addLine(TCULong color, TCVector *vertices)
{
	setupColoredLines();
	m_coloredShapes[TREMLines]->addLine(color, vertices);
}

void TREModel::addLine(TCVector *vertices)
{
	setupLines();
	m_shapes[TREMLines]->addLine(vertices);
}

void TREModel::addEdgeLine(TCVector *vertices)
{
	setupEdges();
	m_shapes[TREMEdgeLines]->addLine(vertices);
}

void TREModel::addTriangle(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes[TREMStandard]->addTriangle(color, vertices);
}

void TREModel::addTriangle(TCVector *vertices)
{
	setupStandard();
	m_shapes[TREMStandard]->addTriangle(vertices);
}

void TREModel::addBFCTriangle(TCULong color, TCVector *vertices)
{
	setupColoredBFC();
	m_coloredShapes[TREMBFC]->addTriangle(color, vertices);
}

void TREModel::addBFCTriangle(TCVector *vertices)
{
	setupBFC();
	m_shapes[TREMBFC]->addTriangle(vertices);
}

void TREModel::addTriangle(TCVector *vertices, TCVector *normals)
{
	setupStandard();
	m_shapes[TREMStandard]->addTriangle(vertices, normals);
}

void TREModel::addBFCTriangle(TCVector *vertices, TCVector *normals)
{
	setupBFC();
	m_shapes[TREMBFC]->addTriangle(vertices, normals);
}

void TREModel::addQuad(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes[TREMStandard]->addQuad(color, vertices);
}

void TREModel::addQuad(TCVector *vertices)
{
	setupStandard();
	m_shapes[TREMStandard]->addQuad(vertices);
}

void TREModel::addBFCQuad(TCVector *vertices)
{
	setupBFC();
	m_shapes[TREMBFC]->addQuad(vertices);
}

void TREModel::addBFCQuad(TCULong color, TCVector *vertices)
{
	setupColoredBFC();
	m_coloredShapes[TREMBFC]->addQuad(color, vertices);
}

void TREModel::quadStripToQuad(int index, TCVector *stripVertices,
							   TCVector *stripNormals, TCVector *quadVertices,
							   TCVector *quadNormals)
{
	quadVertices[0] = stripVertices[index];
	quadVertices[1] = stripVertices[index + 1];
	quadVertices[2] = stripVertices[index + 3];
	quadVertices[3] = stripVertices[index + 2];
	quadNormals[0] = stripNormals[index];
	quadNormals[1] = stripNormals[index + 1];
	quadNormals[2] = stripNormals[index + 3];
	quadNormals[3] = stripNormals[index + 2];
}

void TREModel::addQuadStrip(TCVector *vertices, TCVector *normals, int count,
							bool flat)
{
	setupStandard();
	addQuadStrip(m_shapes[TREMStandard], vertices, normals, count, flat);
}

void TREModel::addQuadStrip(TREShapeGroup *shapeGroup, TCVector *vertices,
							TCVector *normals, int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addQuadStrip(vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			shapeGroup->addQuad(quadVertices, quadNormals);
		}
	}
}

void TREModel::addBFCQuadStrip(TCVector *vertices, TCVector *normals, int count,
							   bool flat)
{
	setupBFC();
	addQuadStrip(m_shapes[TREMBFC], vertices, normals, count, flat);
}

void TREModel::addQuadStrip(TCULong color, TCVector *vertices,
							TCVector *normals, int count, bool flat)
{
	setupColored();
	addQuadStrip(m_coloredShapes[TREMStandard], color, vertices, normals,
		count, flat);
}

void TREModel::addQuadStrip(TREColoredShapeGroup *shapeGroup, TCULong color,
							TCVector *vertices, TCVector *normals, int count,
							bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addQuadStrip(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			shapeGroup->addQuad(color, quadVertices, quadNormals);
		}
	}
}

void TREModel::addBFCQuadStrip(TCULong color, TCVector *vertices,
							   TCVector *normals, int count, bool flat)
{
	setupColoredBFC();
	addQuadStrip(m_coloredShapes[TREMBFC], color, vertices, normals, count,
		flat);
}

void TREModel::triangleFanToTriangle(int index, TCVector *stripVertices,
									 TCVector *stripNormals,
									 TCVector *triangleVertices,
									 TCVector *triangleNormals)
{
	triangleVertices[0] = stripVertices[0];
	triangleVertices[1] = stripVertices[index];
	triangleVertices[2] = stripVertices[index + 1];
	triangleNormals[0] = stripNormals[0];
	triangleNormals[1] = stripNormals[index];
	triangleNormals[2] = stripNormals[index + 1];
}

void TREModel::addTriangleFan(TCVector *vertices, TCVector *normals, int count,
							  bool flat)
{
	setupStandard();
	addTriangleFan(m_shapes[TREMStandard], vertices, normals, count, flat);
}

void TREModel::addTriangleFan(TREShapeGroup *shapeGroup, TCVector *vertices,
							  TCVector *normals, int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addTriangleFan(vertices, normals, count);
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];

		for (i = 1; i < count - 1; i++)
		{
			triangleFanToTriangle(i, vertices, normals, triangleVertices,
				triangleNormals);
			shapeGroup->addTriangle(triangleVertices, triangleNormals);
		}
	}
}

void TREModel::addBFCTriangleFan(TCVector *vertices, TCVector *normals,
								 int count, bool flat)
{
	setupBFC();
	addTriangleFan(m_shapes[TREMBFC], vertices, normals, count, flat);
}

void TREModel::addTriangleFan(TCULong color, TCVector *vertices,
							  TCVector *normals, int count, bool flat)
{
	setupColored();
	addTriangleFan(m_coloredShapes[TREMStandard], color, vertices, normals,
		count, flat);
}

void TREModel::addTriangleFan(TREColoredShapeGroup *shapeGroup, TCULong color,
							  TCVector *vertices, TCVector *normals, int count,
							  bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addTriangleFan(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];

		for (i = 1; i < count - 1; i++)
		{
			triangleFanToTriangle(i, vertices, normals, triangleVertices,
				triangleNormals);
			shapeGroup->addTriangle(color, triangleVertices, triangleNormals);
		}
	}
}

void TREModel::addBFCTriangleFan(TCULong color, TCVector *vertices,
								 TCVector *normals, int count, bool flat)
{
	setupColoredBFC();
	addTriangleFan(m_coloredShapes[TREMBFC], color, vertices, normals, count,
		flat);
}

TRESubModel *TREModel::addSubModel(float *matrix, TREModel *model, bool invert)
{
	TRESubModel *subModel = new TRESubModel;

	if (!m_subModels)
	{
		m_subModels = new TRESubModelArray;
	}
	subModel->setMatrix(matrix);
	subModel->setModel(model);
	subModel->setBFCInvertFlag(invert);
	m_subModels->addObject(subModel);
	subModel->release();
	return subModel;
}

TRESubModel *TREModel::addSubModel(TCULong color, TCULong edgeColor,
								   float *matrix, TREModel *model, bool invert)
{
	TRESubModel *subModel = addSubModel(matrix, model, invert);

	subModel->setColor(color, edgeColor);
	return subModel;
}

void TREModel::flatten(void)
{
	if (m_subModels && m_subModels->getCount())
	{
		float identityMatrix[16];

		TCVector::initIdentityMatrix(identityMatrix);
		flatten(this, identityMatrix, 0, false, 0, false, false);
		if (m_subModels)
		{
			m_subModels->removeAll();
		}
		m_flags.flattened = true;
	}
}

void TREModel::flatten(TREModel *model, float *matrix, TCULong color,
					   bool colorSet, TCULong edgeColor, bool edgeColorSet,
					   bool includeShapes)
{
	TRESubModelArray *subModels = model->m_subModels;

	if (includeShapes)
	{
		int i;

		for (i = 0; i <= TREMLast; i++)
		{
			TREShapeGroup *otherShapeGroup = model->m_shapes[i];
			TREColoredShapeGroup *otherColoredShapeGroup =
				model->m_coloredShapes[i];

			if (otherShapeGroup)
			{
				bool actualColorSet = colorSet;
				TCULong actualColor = color;

				if (i == TREMEdgeLines)
				{
					actualColorSet = edgeColorSet;
					actualColor = edgeColor;
				}
				if (actualColorSet)
				{
					TREColoredShapeGroup *coloredShapeGroup;
					
					setupColored((TREMSection)i);
					coloredShapeGroup = m_coloredShapes[i];
					coloredShapeGroup->getVertexStore()->setupColored();
					flattenShapes(coloredShapeGroup, otherShapeGroup,
						matrix, actualColor, true);
				}
				else
				{
					setup((TREMSection)i);
					flattenShapes(m_shapes[i], otherShapeGroup, matrix, 0,
						false);
				}
			}
			if (otherColoredShapeGroup)
			{
				setupColored((TREMSection)i);
				flattenShapes(m_coloredShapes[i], otherColoredShapeGroup,
					matrix, 0, false);
			}
		}
	}
	if (subModels)
	{
		int i;
		int count = subModels->getCount();
		float newMatrix[16];

		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel = (*subModels)[i];

			TCVector::multMatrix(matrix, subModel->getMatrix(), newMatrix);
			if (subModel->isColorSet())
			{
				flatten(subModel->getEffectiveModel(), newMatrix,
					htonl(subModel->getColor()), true,
					htonl(subModel->getEdgeColor()), true, true);
			}
			else
			{
				flatten(subModel->getEffectiveModel(), newMatrix, color,
					colorSet, edgeColor, edgeColorSet, true);
			}
		}
	}
}

void TREModel::flattenShapes(TREShapeType shapeType,
							 TREVertexArray *dstVertices,
							 TREVertexArray *dstNormals,
							 TCULongArray *dstColors,
							 TCULongArray *dstIndices,
							 TREVertexArray *srcVertices,
							 TREVertexArray *srcNormals,
							 TCULongArray *srcColors,
							 TCULongArray *srcIndices, float *matrix,
							 TCULong color, bool colorSet)
{
	int shapeSize = TREShapeGroup::numPointsForShapeType(shapeType);
	TCVector normal;
	TCVector points[3];
	TREVertex normalVertex;
	int i;
	int count = srcIndices->getCount();

	for (i = 0; i < count; i++)
	{
		int index = (*srcIndices)[i];
		TREVertex vertex = (*srcVertices)[index];

		dstIndices->addValue(dstVertices->getCount());
		transformVertex(vertex, matrix);
		dstVertices->addVertex(vertex);
		if (srcNormals)
		{
			TREVertex normal = (*srcNormals)[index];

			transformNormal(normal, matrix);
			dstNormals->addVertex(normal);
		}
		else if (shapeSize > 2)
		{
			if (i % shapeSize == 0)
			{
				int j;

				for (j = 0; j < 3; j++)
				{
					TREVertex v = (*srcVertices)[(*srcIndices)[i + j]];

					transformVertex(v, matrix);
					memcpy((float *)points[j], v.v, sizeof(v.v));
				}
				normal = TREVertexStore::calcNormal(points);
				TREVertexStore::initVertex(normalVertex, normal);
			}
			dstNormals->addVertex(normalVertex);
		}
		if (colorSet)
		{
			dstColors->addValue(color);
		}
		else if (srcColors)
		{
			dstColors->addValue((*srcColors)[index]);
		}
	}
}

void TREModel::flattenStrips(TREVertexArray *dstVertices,
							 TREVertexArray *dstNormals,
							 TCULongArray *dstColors,
							 TCULongArray *dstIndices,
							 TCULongArray *dstStripCounts,
							 TREVertexArray *srcVertices,
							 TREVertexArray *srcNormals,
							 TCULongArray *srcColors,
							 TCULongArray *srcIndices,
							 TCULongArray *srcStripCounts, float *matrix,
							 TCULong color, bool colorSet)
{
	int i, j;
	int numStrips = srcStripCounts->getCount();
	int indexOffset = 0;

	for (i = 0; i < numStrips; i++)
	{
		int stripCount = (*srcStripCounts)[i];

		dstStripCounts->addValue(stripCount);
		for (j = 0; j < stripCount; j++)
		{
			int index = (*srcIndices)[j + indexOffset];
			TREVertex vertex = (*srcVertices)[index];

			dstIndices->addValue(dstVertices->getCount());
			transformVertex(vertex, matrix);
			dstVertices->addVertex(vertex);
			if (srcNormals)
			{
				TREVertex normal = (*srcNormals)[index];

				transformNormal(normal, matrix);
				dstNormals->addVertex(normal);
			}
			if (colorSet)
			{
				dstColors->addValue(color);
			}
			else if (srcColors)
			{
				dstColors->addValue((*srcColors)[index]);
			}
		}
		indexOffset += stripCount;
	}
}

void TREModel::flattenShapes(TREShapeGroup *dstShapes, TREShapeGroup *srcShapes,
							 float *matrix, TCULong color, bool colorSet)
{
	TREVertexStore *srcVertexStore = NULL;
	TREVertexStore *dstVertexStore = NULL;

	if (srcShapes && (srcVertexStore = srcShapes->getVertexStore()) != NULL &&
		dstShapes && (dstVertexStore = dstShapes->getVertexStore()) != NULL)
	{
		TCULong bit;

		for (bit = TRESFirst; bit <= TRESLast; bit = bit << 1)
		{
			TCULongArray *srcIndices = srcShapes->getIndices((TREShapeType)bit);

			if (srcIndices)
			{
				TREVertexArray *srcVertices = srcVertexStore->getVertices();
				TREVertexArray *srcNormals = srcVertexStore->getNormals();
				TCULongArray *srcColors = srcVertexStore->getColors();
				TCULongArray *dstIndices =
					dstShapes->getIndices((TREShapeType)bit, true);

				if (srcVertices)
				{
					TREVertexArray *dstVertices = dstVertexStore->getVertices();
					TREVertexArray *dstNormals = dstVertexStore->getNormals();
					TCULongArray *dstColors = dstVertexStore->getColors();
					TREShapeType shapeType = (TREShapeType)bit;

					if (shapeType < TRESFirstStrip)
					{
						flattenShapes(shapeType, dstVertices, dstNormals,
							dstColors, dstIndices, srcVertices, srcNormals,
							srcColors, srcIndices, matrix, color, colorSet);
					}
					else
					{
						TCULongArray *dstStripCounts =
							dstShapes->getStripCounts((TREShapeType)bit, true);
						TCULongArray *srcStripCounts =
							srcShapes->getStripCounts((TREShapeType)bit);

						flattenStrips(dstVertices, dstNormals, dstColors,
							dstIndices, dstStripCounts, srcVertices, srcNormals,
							srcColors, srcIndices, srcStripCounts, matrix,
							color, colorSet);
					}
				}
			}
		}
	}
}

void TREModel::transformVertex(TREVertex &vertex, float *matrix)
{
	TCVector newVertex;
	float x = vertex.v[0];
	float y = vertex.v[1];
	float z = vertex.v[2];

//	x' = a*x + b*y + c*z + X
//	y' = d*x + e*y + f*z + Y
//	z' = g*x + h*y + i*z + Z
	newVertex[0] = matrix[0]*x + matrix[4]*y + matrix[8]*z + matrix[12];
	newVertex[1] = matrix[1]*x + matrix[5]*y + matrix[9]*z + matrix[13];
	newVertex[2] = matrix[2]*x + matrix[6]*y + matrix[10]*z + matrix[14];
	TREVertexStore::initVertex(vertex, newVertex);
}

void TREModel::transformNormal(TREVertex &normal, float *matrix)
{
	TCVector newNormal;
	float inverseMatrix[16];
	float x = normal.v[0];
	float y = normal.v[1];
	float z = normal.v[2];
	float det;

	det = TCVector::invertMatrix(matrix, inverseMatrix);
//	x' = a*x + b*y + c*z + X
//	y' = d*x + e*y + f*z + Y
//	z' = g*x + h*y + i*z + Z
	newNormal[0] = inverseMatrix[0]*x + inverseMatrix[1]*y +
		inverseMatrix[2]*z;
	newNormal[1] = inverseMatrix[4]*x + inverseMatrix[5]*y + inverseMatrix[6]*z;
	newNormal[2] = inverseMatrix[8]*x + inverseMatrix[9]*y +
		inverseMatrix[10]*z;
	newNormal.normalize();
	TREVertexStore::initVertex(normal, newNormal);
}

void TREModel::setGlNormalize(bool value)
{
//	if (value != sm_normalizeOn)
	{
		if (value)
		{
			glEnable(GL_NORMALIZE);
//			sm_normalizeOn = true;
		}
		else
		{
			glDisable(GL_NORMALIZE);
//			sm_normalizeOn = false;
		}
	}
}

void TREModel::addSlopedCylinder(const TCVector& center, float radius,
								 float height, int numSegments,
								 int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector top = center;
	TCVector normal = TCVector(0.0f, 1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments * 2 + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i * 2]);
		top[1] =
			center.get(1) + height - ((height / radius) * points[i * 2][0]);
		setCirclePoint(angle, radius, top, points[i * 2 + 1]);
		if (height == 0.0f)
		{
			normals[i * 2] = normal;
			normals[i * 2 + 1] = normal;
		}
		else
		{
			normals[i * 2] = (points[i * 2] - center).normalize();
			normals[i * 2 + 1] =
				(points[i * 2 + 1] - top).normalize();
		}
	}
	if (bfc)
	{
		addBFCQuadStrip(points, normals, vertexCount);
	}
	else
	{
		addQuadStrip(points, normals, vertexCount);
	}
	delete[] points;
	delete[] normals;
/*
	if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
	{
		genSlopedCylinderConditionals(cone, numSegments, usedSegments);
	}
*/
}

void TREModel::addSlopedCylinder2(const TCVector& center, float radius,
								  float height, int numSegments,
								  int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector top = center;
	TCVector normal = TCVector(0.0f, 1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments * 2 + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i + (float)M_PI / 2.0f;
		setCirclePoint(angle, radius, center, points[i * 2]);
		top[1] = abs(points[i * 2][0]);
		setCirclePoint(angle, radius, top, points[i * 2 + 1]);
		if (height == 0.0f)
		{
			normals[i * 2] = normal;
			normals[i * 2 + 1] = normal;
		}
		else
		{
			normals[i * 2] = (points[i * 2] - center).normalize();
			normals[i * 2 + 1] =
				(points[i * 2 + 1] - top).normalize();
		}
	}
	if (bfc)
	{
		addBFCQuadStrip(points, normals, vertexCount);
	}
	else
	{
		addQuadStrip(points, normals, vertexCount);
	}
	delete[] points;
	delete[] normals;
/*
	if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
	{
		genSlopedCylinder2Conditionals(cone, numSegments, usedSegments);
	}
*/
}

void TREModel::addCylinder(const TCVector& center, float radius, float height,
						   int numSegments, int usedSegments, bool bfc)
{
	addOpenCone(center, radius, radius, height, numSegments, usedSegments, bfc);
}

void TREModel::addDisc(const TCVector &center, float radius, int numSegments,
					   int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector normal = TCVector(0.0f, -1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	points[0] = center;
	normals[0] = normal;
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i + 1]);
		normals[i + 1] = normal;
	}
	if (bfc)
	{
		addBFCTriangleFan(points, normals, vertexCount, true);
	}
	else
	{
		addTriangleFan(points, normals, vertexCount, true);
	}
	delete[] points;
	delete[] normals;
}

void TREModel::addNotDisc(const TCVector &center, float radius, int numSegments,
						  int usedSegments, bool bfc)
{
	int quarter = numSegments / 4;
	int numQuarters;
	int i, j;
	TCVector normal = TCVector(0.0f, -1.0f, 0.0f);
	TCVector p1;

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	numQuarters = (usedSegments + quarter - 1) / quarter;
	for (i = 0; i < numQuarters; i++)
	{
		TCVector *points;
		TCVector *normals;
		int vertexCount;
		int quarterSegments = quarter;
		float zMult = 1.0f;
		float xMult = 1.0f;

		if (i >= 2)
		{
			zMult = -1.0f;
		}
		if (i == 1 || i == 2)
		{
			xMult = -1.0f;
		}
		if (i == numQuarters - 1)
		{
			quarterSegments = usedSegments % quarter;
			if (!quarterSegments)
			{
				quarterSegments = quarter;
			}
		}
		vertexCount = quarterSegments + 2;
		points = new TCVector[vertexCount];
		normals = new TCVector[vertexCount];
		points[0] = center + TCVector(xMult * radius, 0.0f, zMult * radius);
		normals[0] = normal;
		for (j = 0; j <= quarterSegments; j++)
		{
			float x, z;
			float angle;

			angle = 2.0f * (float)M_PI / numSegments *
				(j + i * quarterSegments);
			x = radius * (float)cos(angle);
			z = radius * (float)sin(angle);
			p1[0] = center.get(0) + x;
			p1[2] = center.get(2) + z;
			p1[1] = center.get(1);
			points[quarterSegments - j + 1] = p1;
			normals[quarterSegments - j + 1] = normal;
		}
		if (bfc)
		{
			addBFCTriangleFan(points, normals, vertexCount, true);
		}
		else
		{
			addTriangleFan(points, normals, vertexCount, true);
		}
		delete[] points;
		delete[] normals;
	}
}

void TREModel::setCirclePoint(float angle, float radius, const TCVector& center,
							  TCVector& point)
{
	float x1, z1;

	x1 = radius * (float)cos(angle);
	z1 = radius * (float)sin(angle);
	point[0] = center.get(0) + x1;
	point[1] = center.get(1);
	point[2] = center.get(2) + z1;
}

void TREModel::addCone(const TCVector &center, float radius, float height,
					   int numSegments, int usedSegments, bool bfc)
{
	int i;
	TCVector top = center;
	TCVector p1, p2, p3;

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	top[1] += height;
	for (i = 0; i < usedSegments; i++)
	{
		TCVector *points = new TCVector[3];
		TCVector *normals = new TCVector[3];
		float angle0, angle1, angle2, angle3;

		angle0 = 2.0f * (float)M_PI / numSegments * (i - 1);
		angle1 = 2.0f * (float)M_PI / numSegments * i;
		angle2 = 2.0f * (float)M_PI / numSegments * (i + 1);
		angle3 = 2.0f * (float)M_PI / numSegments * (i + 2);
		setCirclePoint(angle1, radius, center, p1);
		setCirclePoint(angle2, radius, center, p2);
		points[0] = p2;
		points[1] = p1;
		points[2] = top;
		setCirclePoint(angle0, radius, center, p3);
/*
		if (i == 0)
		{
			newConditionalLine(24, p1, top, p3, p2);
		}
*/
		normals[0] = ((p1 - top) * (p1 - p2)) + ((p3 - top) * (p3 - p1));
		normals[0].normalize();
		setCirclePoint(angle3, radius, center, p3);
//		newConditionalLine(24, p2, top, p1, p3);
		normals[1] = ((p2 - top) * (p2 - p3)) + ((p1 - top) * (p1 - p2));
		normals[1].normalize();
		normals[2] = (p1 - top) * (p1 - p2);
		normals[2].normalize();
		if (bfc)
		{
			addBFCTriangle(points, normals);
		}
		else
		{
			addTriangle(points, normals);
		}
		delete[] points;
		delete[] normals;
	}
}

void TREModel::addOpenCone(const TCVector& center, float radius1, float radius2,
						   float height, int numSegments, int usedSegments,
						   bool bfc)
{
	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	if (radius1 == 0.0f)
	{
		addCone(center, radius2, height, numSegments, usedSegments, bfc);
	}
	else if (radius2 == 0.0f)
	{
		addCone(center, radius1, height, numSegments, usedSegments, bfc);
	}
	else
	{
		int vertexCount = usedSegments * 2 + 2;
		TCVector *points = new TCVector[vertexCount];
		TCVector *normals = new TCVector[vertexCount];
		int i;
		TCVector top = center;
		TCVector normal = TCVector(0.0f, 1.0f, 0.0f);
		TCVector topNormalPoint;
		TCVector normalPoint;

		top[1] += height;
		if (height)
		{
			topNormalPoint = top + normal * radius2 * (radius2 - radius1) /
				height;
			normalPoint = center + normal * radius1 * (radius2 - radius1) /
				height;
		}
		for (i = 0; i <= usedSegments; i++)
		{
			float angle;

			angle = 2.0f * (float)M_PI / numSegments * i;
			setCirclePoint(angle, radius1, center, points[i * 2]);
			setCirclePoint(angle, radius2, top, points[i * 2 + 1]);
			if (height == 0.0f)
			{
				normals[i * 2] = normal;
				normals[i * 2 + 1] = normal;
			}
			else
			{
				normals[i * 2] = (points[i * 2] - normalPoint).normalize();
				normals[i * 2 + 1] =
					(points[i * 2 + 1] - topNormalPoint).normalize();
			}
		}
		if (bfc)
		{
			addBFCQuadStrip(points, normals, vertexCount, height == 0.0f);
		}
		else
		{
			addQuadStrip(points, normals, vertexCount, height == 0.0f);
		}
		delete[] points;
		delete[] normals;
/*
		if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
		{
			genConeConditionals(cone, numSegments, usedSegments);
		}
*/
	}
}

void TREModel::addCircularEdge(const TCVector& center, float radius,
							   int numSegments, int usedSegments)
{
	int i;
	TCVector p1;
	TCVector *allPoints;
	TCVector points[2];

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	allPoints = new TCVector[usedSegments + 1];
	for (i = 0; i <= usedSegments; i++)
	{
		float x, z;
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		x = radius * (float)cos(angle);
		z = radius * (float)sin(angle);
		p1[0] = center.get(0) + x;
		p1[2] = center.get(2) + z;
		p1[1] = center.get(1);
		allPoints[i] = p1;
	}
	for (i = 0; i < usedSegments; i++)
	{
		points[0] = allPoints[i];
		points[1] = allPoints[i + 1];
		addEdgeLine(points);
	}
	delete[] allPoints;
}

void TREModel::addRing(const TCVector& center, float radius1, float radius2,
					   int numSegments, int usedSegments, bool bfc)
{
	addOpenCone(center, radius1, radius2, 0.0f, numSegments, usedSegments,
		bfc);
}

void TREModel::calculateBoundingBox(void)
{
	if (!m_flags.boundingBox)
	{
		float identityMatrix[16];

		TCVector::initIdentityMatrix(identityMatrix);
		m_boundingMin[0] = 1e10f;
		m_boundingMin[1] = 1e10f;
		m_boundingMin[2] = 1e10f;
		m_boundingMax[0] = -1e10f;
		m_boundingMax[1] = -1e10f;
		m_boundingMax[2] = -1e10f;
		scanPoints(this, (TREScanPointCallback)scanBoundingBoxPoint,
			identityMatrix);
		m_flags.boundingBox = true;
	}
}

void TREModel::scanPoints(TCObject *scanner,
						  TREScanPointCallback scanPointCallback, float *matrix)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		TREShapeGroup *shapeGroup = m_shapes[i];

		if (shapeGroup)
		{
			shapeGroup->scanPoints(scanner, scanPointCallback, matrix);
		}
		shapeGroup = m_coloredShapes[i];
		if (shapeGroup)
		{
			shapeGroup->scanPoints(scanner, scanPointCallback, matrix);
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->scanPoints(scanner, scanPointCallback, matrix);
		}
	}
}

void TREModel::unshrinkNormals(float *matrix, float *unshrinkMatrix)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];

			if (shapeGroup)
			{
				shapeGroup->unshrinkNormals(matrix, unshrinkMatrix);
			}
			shapeGroup = m_coloredShapes[i];
			if (shapeGroup)
			{
				shapeGroup->unshrinkNormals(matrix, unshrinkMatrix);
			}
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->unshrinkNormals(matrix, unshrinkMatrix);
		}
	}
}

void TREModel::getBoundingBox(TCVector& min, TCVector& max)
{
	if (!m_flags.boundingBox)
	{
		calculateBoundingBox();
	}
	min = m_boundingMin;
	max = m_boundingMax;
}

void TREModel::scanBoundingBoxPoint(const TCVector &point)
{
	if (point.get(0) < m_boundingMin[0])
	{
		m_boundingMin[0] = point.get(0);
	}
	if (point.get(1) < m_boundingMin[1])
	{
		m_boundingMin[1] = point.get(1);
	}
	if (point.get(2) < m_boundingMin[2])
	{
		m_boundingMin[2] = point.get(2);
	}
	if (point.get(0) > m_boundingMax[0])
	{
		m_boundingMax[0] = point.get(0);
	}
	if (point.get(1) > m_boundingMax[1])
	{
		m_boundingMax[1] = point.get(1);
	}
	if (point.get(2) > m_boundingMax[2])
	{
		m_boundingMax[2] = point.get(2);
	}
}

// When you shrink a part, all the normals end up getting lengthened by an
// amount that is based on their direction and the magnitude of the shrinkage
// matrix.  If that isn't adjusted for, then all normals have to be normalized
// by OpenGL, which slows things down.  This functions shortens all the normals
// in a part by the appropriate amount based on the shrinkage matrix.  Then when
// the part is drawn with the shrinkage matrix, they automatically get adjusted
// back to being unit length, and we don't have to force OpenGL to normalize
// them.
//
// Note: At first glance, this would appear to mess up parts that are mirror
// images of each other.  Since one part will reference the other with a mirror
// matrix, it makes it possible to shrink the normals twice.  However, since
// all parts get flattenned, and the flatenning process re-normalizes the
// normals to be unit lenght, everything is fine.  If it ever becomes desirable
// to allow parts not to be flattened, things will get more complicated.
void TREModel::unshrinkNormals(float *scaleMatrix)
{
	// If the same part is referenced twice in a model, we'll get here twice.
	// We only want to adjust the normals once, or we'll be in trouble, so
	// record the fact that the normals have been adjusted.
	if (!m_flags.unshrunkNormals)
	{
		float identityMatrix[16];

		TCVector::initIdentityMatrix(identityMatrix);
		unshrinkNormals(identityMatrix, scaleMatrix);
		m_flags.unshrunkNormals = true;
	}
}

void TREModel::setSectionPresent(TREMSection section, bool colored)
{
	TCULong bit = 1 << section;

	if (colored)
	{
		m_coloredSectionsPresent |= bit;
	}
	else
	{
		m_sectionsPresent |= bit;
	}
}

bool TREModel::isSectionPresent(TREMSection section, bool colored)
{
	TCULong bit = 1 << section;

	if (colored)
	{
		return (m_coloredSectionsPresent & bit) != 0;
	}
	else
	{
		return (m_sectionsPresent & bit) != 0;
	}
}

bool TREModel::checkShapeGroupPresent(TREShapeGroup *shapeGroup,
									  TREMSection section, bool colored)
{
	if (shapeGroup)
	{
		setSectionPresent(section, colored);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			if ((*m_subModels)[i]->getEffectiveModel()->
				checkSectionPresent(section, colored))
			{
				setSectionPresent(section, colored);
			}
		}
	}
	return isSectionPresent(section, colored);
}

bool TREModel::checkSectionPresent(TREMSection section, bool colored)
{
	if (colored)
	{
		return checkShapeGroupPresent(m_coloredShapes[section], section,
			true);
	}
	else
	{
		return checkShapeGroupPresent(m_shapes[section], section, false);
	}
}

bool TREModel::checkSectionPresent(TREMSection section)
{
	return checkSectionPresent(section, false);
}

bool TREModel::checkColoredSectionPresent(TREMSection section)
{
	return checkSectionPresent(section, true);
}

bool TREModel::checkDefaultColorPresent(void)
{
	return checkSectionPresent(TREMStandard);
}

bool TREModel::checkBFCPresent(void)
{
	return checkSectionPresent(TREMBFC);
}

bool TREModel::checkDefaultColorLinesPresent(void)
{
	return checkSectionPresent(TREMLines);
}

bool TREModel::checkEdgeLinesPresent(void)
{
	return checkSectionPresent(TREMEdgeLines);
}

bool TREModel::checkColoredPresent(void)
{
	return checkColoredSectionPresent(TREMStandard);
}

bool TREModel::checkColoredBFCPresent(void)
{
	return checkColoredSectionPresent(TREMBFC);
}

bool TREModel::checkColoredLinesPresent(void)
{
	return checkColoredSectionPresent(TREMLines);
}

bool TREModel::checkColoredEdgeLinesPresent(void)
{
	return checkColoredSectionPresent(TREMEdgeLines);
}

void TREModel::uncompile(void)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		TCULong listID = m_listIDs[i];

		if (listID)
		{
			glDeleteLists(listID, 1);
			m_listIDs[i] = 0;
		}
		listID = m_coloredListIDs[i];
		if (listID)
		{
			glDeleteLists(listID, 1);
			m_coloredListIDs[i] = 0;
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->getEffectiveModel()->uncompile();
		}
	}
}

void TREModel::transferColoredTransparent(TREMSection section,
										  const float *matrix)
{
	TREColoredShapeGroup *shapeGroup = m_coloredShapes[section];

	if (shapeGroup)
	{
		shapeGroup->transferColoredTransparent(m_mainModel, matrix);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->transferColoredTransparent(section, matrix);
		}
	}
}

void TREModel::transferTransparent(TCULong color, TREMSection section,
								   const float *matrix)
{
	TREShapeGroup *shapeGroup = m_shapes[section];

	if (shapeGroup)
	{
		shapeGroup->transferTransparent(color, m_mainModel, matrix);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->transferTransparent(color, section, matrix);
		}
	}
}

