#include "TREModel.h"
#include "TRESubModel.h"
#include "TREMainModel.h"
#include "TREShapeGroup.h"
#include "TREColoredShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREGL.h"

#include <TCFoundation/mystring.h>
#include <LDLoader/LDLMacros.h>		// TODO: remove dependency

bool TREModel::sm_normalizeOn = false;

TREModel::TREModel(void)
	:m_name(NULL),
	m_mainModel(NULL),
	m_subModels(NULL),
	m_shapes(NULL),
	m_coloredShapes(NULL),
	m_highlightShapes(NULL),
	m_defaultColorListID(0),
	m_coloredListID(0),
	m_defaultColorLinesListID(0),
	m_coloredLinesListID(0),
	m_highlightLinesListID(0)
{
	m_flags.part = false;
	m_flags.boundingBox = false;
}

TREModel::TREModel(const TREModel &other)
	:TCObject(other),
	m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_shapes((TREShapeGroup *)TCObject::copy(other.m_shapes)),
	m_coloredShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredShapes)),
	m_defaultColorListID(0),
	m_coloredListID(0),
	m_defaultColorLinesListID(0),
	m_coloredLinesListID(0),
	m_highlightLinesListID(0),
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
	if (m_highlightLinesListID)
	{
		glDeleteLists(m_highlightLinesListID, 1);
	}
	TCObject::dealloc();
}

TCObject *TREModel::copy(void)
{
	return new TREModel(*this);
}

void TREModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

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
		if (m_mainModel->getCompileAll() ||
			(m_flags.part && m_mainModel->getCompileParts()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			drawDefaultColor();
			glEndList();
			m_defaultColorListID = listID;
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

void TREModel::drawDefaultColor(void)
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
				(*m_subModels)[i]->drawDefaultColor();
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

void TREModel::drawHighlightLines(void)
{
	if (m_highlightLinesListID)
	{
		glCallList(m_highlightLinesListID);
	}
	else
	{
		if (m_highlightShapes)
		{
			m_highlightShapes->drawLines();
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->drawHighlightLines();
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

void TREModel::setupHighlight(void)
{
	if (!m_highlightShapes)
	{
		m_highlightShapes = new TREShapeGroup;
		m_highlightShapes->setVertexStore(m_mainModel->getVertexStore());
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

void TREModel::addHighlightLine(TCVector *vertices)
{
	setupHighlight();
	m_highlightShapes->addLine(vertices);
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

void TREModel::addQuadStrip(TCVector *vertices, TCVector *normals, int count)
{
	setup();
	m_shapes->addQuadStrip(vertices, normals, count);
}

void TREModel::addQuadStrip(TCULong color, TCVector *vertices, TCVector *normals,
							int count)
{
	setupColored();
	m_coloredShapes->addQuadStrip(color, vertices, normals, count);
}

void TREModel::addTriangleFan(TCVector *vertices, TCVector *normals, int count)
{
	setup();
	m_shapes->addTriangleFan(vertices, normals, count);
}

void TREModel::addTriangleFan(TCULong color, TCVector *vertices, TCVector *normals,
							  int count)
{
	setupColored();
	m_coloredShapes->addTriangleFan(color, vertices, normals, count);
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

TRESubModel *TREModel::addSubModel(TCULong color, TCULong highlightColor,
								   float *matrix, TREModel *model)
{
	TRESubModel *subModel = addSubModel(matrix, model);

	subModel->setColor(color, highlightColor);
	return subModel;
}

void TREModel::flatten(void)
{
	float identityMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f};

	flatten(this, identityMatrix, 0, false, false);
	if (m_subModels)
	{
		m_subModels->removeAll();
	}
}

void TREModel::flatten(TREModel *model, float *matrix, TCULong color,
					   bool colorSet, bool includeShapes)
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
				flattenShapes(m_shapes, model->m_shapes, matrix, color, false);
			}
		}
		if (model->m_coloredShapes)
		{
			setupColored();
			flattenShapes(m_coloredShapes, model->m_coloredShapes, matrix,
				0, false);
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

			multMatrix(matrix, subModel->getMatrix(), newMatrix);
			if (subModel->isColorSet())
			{
				flatten(subModel->getModel(), newMatrix,
					htonl(subModel->getColor()), true, true);
			}
			else
			{
				flatten(subModel->getModel(), newMatrix, color, colorSet, true);
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
		if (shapeSize > 2)
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
		else if (srcNormals)
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
}

void TREModel::flattenStrips(TREShapeType shapeType,
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
	int i, j;
	int count = srcIndices->getCount();
	int stripCount = 0;

	for (i = 0; i < count; i += stripCount)
	{
		stripCount = (*srcIndices)[i];

		dstIndices->addValue(stripCount);
		i++;
		for (j = 0; j < stripCount; j++)
		{
			int index = (*srcIndices)[i + j];
			TREVertex vertex = (*srcVertices)[index];

			dstIndices->addValue(dstVertices->getCount());
			transformVertex(vertex, matrix);
			dstVertices->addVertex(vertex);
			if (shapeSize > 2)
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
			else if (srcNormals)
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
						flattenStrips(shapeType, dstVertices, dstNormals,
							dstColors, dstIndices, srcVertices, srcNormals,
							srcColors, srcIndices, matrix, color, colorSet);
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

	det = invertMatrix(matrix, inverseMatrix);
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

void TREModel::setGlNormalize(bool value)
{
	if (value != sm_normalizeOn)
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

void TREModel::addCylinder(const TCVector& center, float radius, float height,
						   int numSegments, int usedSegments)
{
	addOpenCone(center, radius, radius, height, numSegments, usedSegments);
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
		TCVector p1, p2;
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
		addQuadStrip(points, normals, vertexCount);
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

void TREModel::addDisk(const TCVector& center, float radius, int numSegments,
					   int usedSegments)
{
	int i;
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	TCVector normal = TCVector(0.0f, 1.0f, 0.0f);

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

		angle = -2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i + 1]);
		normals[i + 1] = normal;
	}
	addTriangleFan(points, normals, vertexCount);
	delete[] points;
	delete[] normals;
}

void TREModel::calculateBoundingBox(void)
{
	if (!m_flags.boundingBox)
	{
		getMinMax(m_boundingMin, m_boundingMax);
		m_flags.boundingBox = true;
	}
}

void TREModel::getMinMax(TCVector& min, TCVector& max, float* matrix)
{
	if (m_shapes)
	{
		m_shapes->getMinMax(min, max, matrix);
	}
	if (m_coloredShapes)
	{
		m_coloredShapes->getMinMax(min, max, matrix);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->getMinMax(min, max, matrix);
		}
	}
}

void TREModel::getMinMax(TCVector& min, TCVector& max)
{
	if (m_flags.boundingBox)
	{
		min = m_boundingMin;
		max = m_boundingMax;
	}
	else
	{
		float identityMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f};
		min[0] = 9999999.0f;
		min[1] = 9999999.0f;
		min[2] = 9999999.0f;
		max[0] = -9999999.0f;
		max[1] = -9999999.0f;
		max[2] = -9999999.0f;
		getMinMax(min, max, identityMatrix);
	}
}
