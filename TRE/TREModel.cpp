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
	m_shapes(NULL),
	m_coloredShapes(NULL),
	m_edgeShapes(NULL),
	m_coloredEdgeShapes(NULL),
	m_invertedModel(NULL),
	m_defaultColorListID(0),
	m_coloredListID(0),
	m_defaultColorLinesListID(0),
	m_coloredLinesListID(0),
	m_edgeLinesListID(0),
	m_coloredEdgeLinesListID(0)
{
	m_flags.part = false;
	m_flags.boundingBox = false;
	m_flags.unshrunkNormals = false;
	m_flags.inverted = false;
}

TREModel::TREModel(const TREModel &other)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_shapes((TREShapeGroup *)TCObject::copy(other.m_shapes)),
	m_coloredShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredShapes)),
	m_edgeShapes((TREShapeGroup *)TCObject::copy(other.m_edgeShapes)),
	m_coloredEdgeShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredEdgeShapes)),
	m_invertedModel((TREModel *)TCObject::copy(other.m_invertedModel)),
	m_defaultColorListID(0),
	m_coloredListID(0),
	m_defaultColorLinesListID(0),
	m_coloredLinesListID(0),
	m_edgeLinesListID(0),
	m_coloredEdgeLinesListID(0),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
}

TREModel::TREModel(const TREModel &other, bool shallow)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels(shallow ? NULL :
		(TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_shapes((TREShapeGroup *)TCObject::copy(other.m_shapes)),
	m_coloredShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredShapes)),
	m_edgeShapes((TREShapeGroup *)TCObject::copy(other.m_edgeShapes)),
	m_coloredEdgeShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredEdgeShapes)),
	m_invertedModel(shallow ? NULL :
		(TREModel *)TCObject::copy(other.m_invertedModel)),
	m_defaultColorListID(0),
	m_coloredListID(0),
	m_defaultColorLinesListID(0),
	m_coloredLinesListID(0),
	m_edgeLinesListID(0),
	m_coloredEdgeLinesListID(0),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
}

TREModel::~TREModel(void)
{
}

void TREModel::dealloc(void)
{
	delete m_name;
	// Don't release m_mainModel
	TCObject::release(m_subModels);
	TCObject::release(m_shapes);
	TCObject::release(m_coloredShapes);
	TCObject::release(m_edgeShapes);
	TCObject::release(m_coloredEdgeShapes);
	if (!m_flags.inverted)
	{
		TCObject::release(m_invertedModel);
	}
	if (m_defaultColorListID)
	{
		glDeleteLists(m_defaultColorListID, 1);
	}
	if (m_coloredListID)
	{
		glDeleteLists(m_coloredListID, 1);
	}
	if (m_defaultColorLinesListID)
	{
		glDeleteLists(m_defaultColorLinesListID, 1);
	}
	if (m_coloredLinesListID)
	{
		glDeleteLists(m_coloredLinesListID, 1);
	}
	if (m_edgeLinesListID)
	{
		glDeleteLists(m_edgeLinesListID, 1);
	}
	if (m_coloredEdgeLinesListID)
	{
		glDeleteLists(m_coloredEdgeLinesListID, 1);
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

TREModel *TREModel::getInvertedModel(void)
{
	if (!m_invertedModel)
	{
		m_invertedModel = shallowCopy();
		m_invertedModel->invert(this);
	}
	return m_invertedModel;
}

void TREModel::invert(TREModel *originalModel)
{
	m_invertedModel = originalModel;
	m_flags.inverted = true;
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
//			subModel->release();
		}
	}
	if (m_shapes)
	{
		m_shapes->invert();
	}
	if (m_coloredShapes)
	{
		m_coloredShapes->invert();
	}
}

void TREModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

/*
void TREModel::draw(void)
{
	if (m_flags.part)
	{
		setGlNormalize(false);
	}
	else
	{
		setGlNormalize(true);
	}
	if (m_shapes)
	{
		m_shapes->draw();
	}
	if (m_coloredShapes)
	{
		glPushAttrib(GL_CURRENT_BIT);
		m_coloredShapes->draw();
		glPopAttrib();
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->draw();
		}
	}
}
*/

void TREModel::compileDefaultColor(void)
{
	if (!m_defaultColorListID)
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileDefaultColor();
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);
			float matrix[16];

			TCVector::initIdentityMatrix(matrix);
			glNewList(listID, GL_COMPILE);
			drawDefaultColor();
			glEndList();
			m_defaultColorListID = listID;
		}
	}
}

void TREModel::compileDefaultColorLines(void)
{
	if (!m_defaultColorLinesListID)
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
			m_defaultColorLinesListID = listID;
		}
	}
}

void TREModel::compileEdgeLines(void)
{
	if (!m_edgeLinesListID)
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
			m_edgeLinesListID = listID;
		}
	}
}

void TREModel::compileColored(void)
{
	if (!m_coloredListID)
	{
		int listID = glGenLists(1);

		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileColored();
			}
		}
		glNewList(listID, GL_COMPILE);
		drawColored();
		glEndList();
		m_coloredListID = listID;
	}
}

void TREModel::compileColoredLines(void)
{
	if (!m_coloredLinesListID)
	{
		int listID = glGenLists(1);

		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileColoredLines();
			}
		}
		glNewList(listID, GL_COMPILE);
		drawColoredLines();
		glEndList();
		m_coloredLinesListID = listID;
	}
}

void TREModel::compileColoredEdgeLines(void)
{
	if (!m_coloredEdgeLinesListID)
	{
		int listID = glGenLists(1);

		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->getModel()->compileColoredEdgeLines();
			}
		}
		glNewList(listID, GL_COMPILE);
		drawColoredEdgeLines();
		glEndList();
		m_coloredEdgeLinesListID = listID;
	}
}

void TREModel::drawDefaultColor(void)
{
	float identityMatrix[16];

	TCVector::initIdentityMatrix(identityMatrix);
	drawDefaultColor(identityMatrix);
}

void TREModel::drawDefaultColor(const float *matrix)
{
	if (m_defaultColorListID)
	{
		glCallList(m_defaultColorListID);
	}
	else
	{
		if (m_flags.part)
		{
			setGlNormalize(false);
		}
		else
		{
			setGlNormalize(true);
		}
		if (m_shapes)
		{
			m_shapes->draw();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawDefaultColor(matrix);
			}
		}
	}
}

void TREModel::drawDefaultColorLines(void)
{
	if (m_defaultColorLinesListID)
	{
		glCallList(m_defaultColorLinesListID);
	}
	else
	{
		if (m_shapes)
		{
			m_shapes->drawLines();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawDefaultColorLines();
			}
		}
	}
}

void TREModel::drawColored(void)
{
	if (m_coloredListID)
	{
		glCallList(m_coloredListID);
	}
	else
	{
		if (m_flags.part)
		{
			setGlNormalize(false);
		}
		else
		{
			setGlNormalize(true);
		}
		if (m_coloredShapes)
		{
			m_coloredShapes->draw();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawColored();
			}
		}
	}
}

void TREModel::drawColoredLines(void)
{
	if (m_coloredLinesListID)
	{
		glCallList(m_coloredLinesListID);
	}
	else
	{
		if (m_coloredShapes)
		{
			m_coloredShapes->drawLines();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawColoredLines();
			}
		}
	}
}

void TREModel::drawEdgeLines(void)
{
	if (m_edgeLinesListID)
	{
		glCallList(m_edgeLinesListID);
	}
	else
	{
		if (m_edgeShapes)
		{
			m_edgeShapes->drawLines();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawEdgeLines();
			}
		}
	}
}

void TREModel::drawColoredEdgeLines(void)
{
	if (m_coloredEdgeLinesListID)
	{
		glCallList(m_coloredEdgeLinesListID);
	}
	else
	{
		if (m_coloredEdgeShapes)
		{
			m_coloredEdgeShapes->drawLines();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawColoredEdgeLines();
			}
		}
	}
}

void TREModel::setup(void)
{
	if (!m_shapes)
	{
		m_shapes = new TREShapeGroup;
		m_shapes->setVertexStore(m_mainModel->getVertexStore());
	}
}

void TREModel::setupEdges(void)
{
	if (!m_edgeShapes)
	{
		m_edgeShapes = new TREShapeGroup;
		m_edgeShapes->setVertexStore(m_mainModel->getVertexStore());
	}
}

void TREModel::setupColored(void)
{
	if (!m_coloredShapes)
	{
		m_coloredShapes = new TREColoredShapeGroup;
		m_coloredShapes->setVertexStore(m_mainModel->getColoredVertexStore());
	}
}

void TREModel::setupColoredEdges(void)
{
	if (!m_coloredEdgeShapes)
	{
		m_coloredEdgeShapes = new TREColoredShapeGroup;
		m_coloredEdgeShapes->setVertexStore(
			m_mainModel->getColoredVertexStore());
	}
}

void TREModel::addLine(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes->addLine(color, vertices);
}

void TREModel::addLine(TCVector *vertices)
{
	setup();
	m_shapes->addLine(vertices);
}

void TREModel::addEdgeLine(TCVector *vertices)
{
	setupEdges();
	m_edgeShapes->addLine(vertices);
}

void TREModel::addTriangle(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes->addTriangle(color, vertices);
}

void TREModel::addTriangle(TCVector *vertices)
{
	setup();
	m_shapes->addTriangle(vertices);
}

void TREModel::addTriangle(TCVector *vertices, TCVector *normals)
{
	setup();
	m_shapes->addTriangle(vertices, normals);
}

void TREModel::addQuad(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes->addQuad(color, vertices);
}

void TREModel::addQuad(TCVector *vertices)
{
	setup();
	m_shapes->addQuad(vertices);
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
	setup();
	if (!flat || m_mainModel->getUseFlatStripsFlag())
	{
		m_shapes->addQuadStrip(vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			m_shapes->addQuad(quadVertices, quadNormals);
		}
	}
}

void TREModel::addQuadStrip(TCULong color, TCVector *vertices,
							TCVector *normals, int count, bool flat)
{
	setupColored();
	if (!flat || m_mainModel->getUseFlatStripsFlag())
	{
		m_coloredShapes->addQuadStrip(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			m_coloredShapes->addQuad(color, quadVertices, quadNormals);
		}
	}
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
	setup();
	if (!flat || m_mainModel->getUseFlatStripsFlag())
	{
		m_shapes->addTriangleFan(vertices, normals, count);
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
			m_shapes->addTriangle(triangleVertices, triangleNormals);
		}
	}
}

void TREModel::addTriangleFan(TCULong color, TCVector *vertices,
							  TCVector *normals, int count, bool flat)
{
	setupColored();
	if (!flat || m_mainModel->getUseFlatStripsFlag())
	{
		m_coloredShapes->addTriangleFan(color, vertices, normals, count);
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
			m_coloredShapes->addTriangle(color, triangleVertices,
				triangleNormals);
		}
	}
}

TRESubModel *TREModel::addSubModel(float *matrix, TREModel *model)
{
	TRESubModel *subModel = new TRESubModel;

	if (!m_subModels)
	{
		m_subModels = new TRESubModelArray;
	}
	subModel->setMatrix(matrix);
	subModel->setModel(model);
	m_subModels->addObject(subModel);
	subModel->release();
	return subModel;
}

TRESubModel *TREModel::addSubModel(TCULong color, TCULong edgeColor,
								   float *matrix, TREModel *model)
{
	TRESubModel *subModel = addSubModel(matrix, model);

	subModel->setColor(color, edgeColor);
	return subModel;
}

void TREModel::flatten(void)
{
	if (m_subModels && m_subModels->getCount())
	{
		float identityMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f};

		flatten(this, identityMatrix, 0, false, 0, false, false);
		if (m_subModels)
		{
			m_subModels->removeAll();
		}
	}
}

void TREModel::flatten(TREModel *model, float *matrix, TCULong color,
					   bool colorSet, TCULong edgeColor, bool edgeColorSet,
					   bool includeShapes)
{
	TRESubModelArray *subModels = model->m_subModels;

	if (includeShapes)
	{
		if (model->m_shapes)
		{
			if (colorSet)
			{
				setupColored();
				m_coloredShapes->getVertexStore()->setupColored();
				flattenShapes(m_coloredShapes, model->m_shapes, matrix, color,
					true);
			}
			else
			{
				setup();
				flattenShapes(m_shapes, model->m_shapes, matrix, 0, false);
			}
		}
		if (model->m_edgeShapes)
		{
			if (edgeColorSet)
			{
				setupColoredEdges();
				m_coloredEdgeShapes->getVertexStore()->setupColored();
				flattenShapes(m_coloredEdgeShapes, model->m_edgeShapes, matrix,
					edgeColor, true);
			}
			else
			{
				setupEdges();
				flattenShapes(m_edgeShapes, model->m_edgeShapes, matrix,
					edgeColor, false);
			}
		}
		if (model->m_coloredShapes)
		{
			setupColored();
			flattenShapes(m_coloredShapes, model->m_coloredShapes, matrix,
				0, false);
		}
		if (model->m_coloredEdgeShapes)
		{
			setupColoredEdges();
			flattenShapes(m_coloredEdgeShapes, model->m_coloredEdgeShapes,
				matrix, 0, false);
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
				flatten(subModel->getModel(), newMatrix,
					htonl(subModel->getColor()), true,
					htonl(subModel->getEdgeColor()), true, true);
			}
			else
			{
				flatten(subModel->getModel(), newMatrix, color, colorSet,
					edgeColor, edgeColorSet, true);
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
	if (det < 0)
	{
		newNormal *= -1.0f;
	}
	TREVertexStore::initVertex(normal, newNormal);
}

/*
float TREModel::invertMatrix(float* matrix, float* inverseMatrix)
{
	float det = TCVector::determinant(matrix);

	if (fEq(det, 0.0f))
	{
		debugPrintf("Error: Matrix inversion failed.\n");
	}
	else
	{
		float oneOverDet = 1.0f / det;

		inverseMatrix[0]  =  (matrix[5] * matrix[10] - matrix[6] * matrix[9]) *
			oneOverDet;
		inverseMatrix[1]  = -(matrix[1] * matrix[10] - matrix[2] * matrix[9]) *
			oneOverDet;
		inverseMatrix[2]  =  (matrix[1] * matrix[6]  - matrix[2] * matrix[5]) *
			oneOverDet;
		inverseMatrix[4]  = -(matrix[4] * matrix[10] - matrix[6] * matrix[8]) *
			oneOverDet;
		inverseMatrix[5]  =  (matrix[0] * matrix[10] - matrix[2] * matrix[8]) *
			oneOverDet;
		inverseMatrix[6]  = -(matrix[0] * matrix[6]  - matrix[2] * matrix[4]) *
			oneOverDet;
		inverseMatrix[8]  =  (matrix[4] * matrix[9]  - matrix[5] * matrix[8]) *
			oneOverDet;
		inverseMatrix[9]  = -(matrix[0] * matrix[9]  - matrix[1] * matrix[8]) *
			oneOverDet;
		inverseMatrix[10] =  (matrix[0] * matrix[5]  - matrix[1] * matrix[4]) *
			oneOverDet;

		inverseMatrix[12] = -(matrix[12] * matrix[0] + matrix[13] * matrix[4] +
							matrix[14] * matrix[8]);
		inverseMatrix[13] = -(matrix[12] * matrix[1] + matrix[13] * matrix[5] +
							matrix[14] * matrix[9]);
		inverseMatrix[14] = -(matrix[12] * matrix[2] + matrix[13] * matrix[6] +
							matrix[14] * matrix[10]);

		inverseMatrix[3] = inverseMatrix[7] = inverseMatrix[11] = 0.0;
		inverseMatrix[15] = 1.0;
	}
	return det;
}

void TREModel::multMatrix(float* left, float* right, float* result)
{
	result[0] = left[0] * right[0] + left[4] * right[1] +
		left[8] * right[2] + left[12] * right[3];
	result[1] = left[1] * right[0] + left[5] * right[1] +
		left[9] * right[2] + left[13] * right[3];
	result[2] = left[2] * right[0] + left[6] * right[1] +
		left[10] * right[2] + left[14] * right[3];
	result[3] = left[3] * right[0] + left[7] * right[1] +
		left[11] * right[2] + left[15] * right[3];
	result[4] = left[0] * right[4] + left[4] * right[5] +
		left[8] * right[6] + left[12] * right[7];
	result[5] = left[1] * right[4] + left[5] * right[5] +
		left[9] * right[6] + left[13] * right[7];
	result[6] = left[2] * right[4] + left[6] * right[5] +
		left[10] * right[6] + left[14] * right[7];
	result[7] = left[3] * right[4] + left[7] * right[5] +
		left[11] * right[6] + left[15] * right[7];
	result[8] = left[0] * right[8] + left[4] * right[9] +
		left[8] * right[10] + left[12] * right[11];
	result[9] = left[1] * right[8] + left[5] * right[9] +
		left[9] * right[10] + left[13] * right[11];
	result[10] = left[2] * right[8] + left[6] * right[9] +
		left[10] * right[10] + left[14] * right[11];
	result[11] = left[3] * right[8] + left[7] * right[9] +
		left[11] * right[10] + left[15] * right[11];
	result[12] = left[0] * right[12] + left[4] * right[13] +
		left[8] * right[14] + left[12] * right[15];
	result[13] = left[1] * right[12] + left[5] * right[13] +
		left[9] * right[14] + left[13] * right[15];
	result[14] = left[2] * right[12] + left[6] * right[13] +
		left[10] * right[14] + left[14] * right[15];
	result[15] = left[3] * right[12] + left[7] * right[13] +
		left[11] * right[14] + left[15] * right[15];
}
*/

void TREModel::setGlNormalize(bool value)
{
//	if (value != sm_normalizeOn)
	{
		if (value)
		{
			glEnable(GL_NORMALIZE);
			sm_normalizeOn = true;
		}
		else
		{
			glDisable(GL_NORMALIZE);
			sm_normalizeOn = false;
		}
	}
}

void TREModel::addSlopedCylinder(const TCVector& center, float radius,
								 float height, int numSegments,
								 int usedSegments)
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
	addQuadStrip(points, normals, vertexCount);
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
								  int usedSegments)
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
	addQuadStrip(points, normals, vertexCount);
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
						   int numSegments, int usedSegments)
{
	addOpenCone(center, radius, radius, height, numSegments, usedSegments);
}

void TREModel::addDisc(const TCVector &center, float radius, int numSegments,
					   int usedSegments)
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
	addTriangleFan(points, normals, vertexCount, true);
	delete[] points;
	delete[] normals;
}

void TREModel::addNotDisc(const TCVector &center, float radius, int numSegments,
						  int usedSegments)
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
		addTriangleFan(points, normals, vertexCount, true);
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
					   int numSegments, int usedSegments)
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
		addTriangle(points, normals);
		delete[] points;
		delete[] normals;
	}
}

void TREModel::addOpenCone(const TCVector& center, float radius1, float radius2,
						   float height, int numSegments, int usedSegments)
{
	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	if (radius1 == 0.0f)
	{
		addCone(center, radius2, height, numSegments, usedSegments);
	}
	else if (radius2 == 0.0f)
	{
		addCone(center, radius1, height, numSegments, usedSegments);
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
		addQuadStrip(points, normals, vertexCount, height == 0.0f);
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
					   int numSegments, int usedSegments)
{
	addOpenCone(center, radius1, radius2, 0.0f, numSegments, usedSegments);
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
	if (m_shapes)
	{
		m_shapes->scanPoints(scanner, scanPointCallback, matrix);
	}
	if (m_coloredShapes)
	{
		m_coloredShapes->scanPoints(scanner, scanPointCallback, matrix);
	}
	if (m_edgeShapes)
	{
		m_edgeShapes->scanPoints(scanner, scanPointCallback, matrix);
	}
	if (m_coloredEdgeShapes)
	{
		m_coloredEdgeShapes->scanPoints(scanner, scanPointCallback, matrix);
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
	if (m_shapes)
	{
		m_shapes->unshrinkNormals(matrix, unshrinkMatrix);
	}
	if (m_coloredShapes)
	{
		m_coloredShapes->unshrinkNormals(matrix, unshrinkMatrix);
	}
	if (m_edgeShapes)
	{
		m_edgeShapes->unshrinkNormals(matrix, unshrinkMatrix);
	}
	if (m_coloredEdgeShapes)
	{
		m_coloredEdgeShapes->unshrinkNormals(matrix, unshrinkMatrix);
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
