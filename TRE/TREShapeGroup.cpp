#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREModel.h"
#include "TREMainModel.h"
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCMacros.h>
#include <string.h>

PFNGLMULTIDRAWELEMENTSEXTPROC TREShapeGroup::glMultiDrawElementsEXT = NULL;

TREShapeGroup::TREShapeGroup(void)
	:m_vertexStore(NULL),
	m_indices(NULL),
	m_controlPointIndices(NULL),
	m_stripCounts(NULL),
	m_multiDrawIndices(NULL),
	m_shapesPresent(0)
{
}

TREShapeGroup::TREShapeGroup(const TREShapeGroup &other)
	:m_vertexStore(other.m_vertexStore),
	m_indices(NULL),
	m_controlPointIndices((TCULongArray *)TCObject::copy(
		other.m_controlPointIndices)),
	m_stripCounts((TCULongArrayArray *)TCObject::copy(other.m_stripCounts)),
	m_multiDrawIndices(NULL),
	m_shapesPresent(other.m_shapesPresent),
	m_mainModel(other.m_mainModel)
{
	m_vertexStore->retain();
	if (other.m_shapesPresent)
	{
		int i, j;
		int shapeTypeCount = other.m_indices->getCount();
		TREVertexArray *vertices = m_vertexStore->getVertices();
		TREVertexArray *normals = m_vertexStore->getNormals();
		TREVertexArray *textureCoords = m_vertexStore->getTextureCoords();
		TCULongArray *colors = m_vertexStore->getColors();

		m_indices = new TCULongArrayArray(shapeTypeCount);
		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *thoseIndices = (*other.m_indices)[i];
			int indexCount = thoseIndices->getCount();
			TCULongArray *theseIndices = new TCULongArray(indexCount);

			m_indices->addObject(theseIndices);
			for (j = 0; j < indexCount; j++)
			{
				int index = (*thoseIndices)[j];

				theseIndices->addValue(vertices->getCount());
				vertices->addVertex((*vertices)[index]);
				normals->addVertex((*normals)[index]);
				if (textureCoords)
				{
					textureCoords->addVertex((*textureCoords)[index]);
				}
				if (colors)
				{
					colors->addValue((*colors)[index]);
				}
			}
			theseIndices->release();
		}
	}
}

TREShapeGroup::~TREShapeGroup(void)
{
}

TCObject *TREShapeGroup::copy(void)
{
	return new TREShapeGroup(*this);
}

void TREShapeGroup::dealloc(void)
{
	// ************************************************************************
	// The deleteMultiDrawIndices call MUST precede the release calls for
	// m_indices and m_stripCounts.
	deleteMultiDrawIndices();
	// ************************************************************************
	TCObject::release(m_vertexStore);
	TCObject::release(m_indices);
	TCObject::release(m_controlPointIndices);
	TCObject::release(m_stripCounts);
	TCObject::dealloc();
}

void TREShapeGroup::addShapeType(TREShapeType shapeType, int index)
{
	TCULongArray *newIndexArray = new TCULongArray;

	if (!m_indices)
	{
		m_indices = new TCULongArrayArray;
	}
	m_indices->insertObject(newIndexArray, index);
	newIndexArray->release();
	m_shapesPresent |= shapeType;
	if (!m_stripCounts)
	{
		m_stripCounts = new TCULongArrayArray;
	}
	if (shapeType >= TRESFirstStrip)
	{
		TCULongArray *newCountArray = new TCULongArray;

		m_stripCounts->insertObject(newCountArray, index);
		newCountArray->release();
	}
	else
	{
		m_stripCounts->insertObject(NULL);
	}
}

TCULongArray *TREShapeGroup::getControlPointIndices(bool create)
{
	if (create && !m_controlPointIndices)
	{
		m_controlPointIndices = new TCULongArray;
	}
	return m_controlPointIndices;
}

TCULongArray *TREShapeGroup::getIndices(TREShapeType shapeType, bool create)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		if (create)
		{
			addShapeType(shapeType, index);
		}
		else
		{
			return NULL;
		}
	}
	return m_indices->objectAtIndex(index);
}

TCULongArray *TREShapeGroup::getStripCounts(TREShapeType shapeType, bool create)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		if (create)
		{
			addShapeType(shapeType, index);
		}
		else
		{
			return NULL;
		}
	}
	return m_stripCounts->objectAtIndex(index);
}

TCULong TREShapeGroup::getShapeTypeIndex(TREShapeType shapeType)
{
	int bit;
	TCULong index = 0;

	for (bit = 1; bit != shapeType; bit = bit << 1)
	{
		if (m_shapesPresent & bit)
		{
			index++;
		}
	}
	return index;
}

void TREShapeGroup::addIndices(TCULongArray *indices, int firstIndex, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		indices->addValue(firstIndex + i);
	}
}

void TREShapeGroup::addShapeIndices(TREShapeType shapeType, int firstIndex,
									int count)
{
	TCULongArray *indices = getIndices(shapeType, true);

	addIndices(indices, firstIndex, count);
}

void TREShapeGroup::addShapeStripCount(TREShapeType shapeType, int count)
{
	TCULongArray *counts = getStripCounts(shapeType, true);

	counts->addValue(count);
}

GLenum TREShapeGroup::modeForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return GL_LINES;
		break;
	case TRESTriangle:
		return GL_TRIANGLES;
		break;
	case TRESQuad:
		return GL_QUADS;
		break;
	case TRESConditionalLine:
		return GL_LINES;
		break;
	case TRESTriangleStrip:
		return GL_TRIANGLE_STRIP;
		break;
	case TRESQuadStrip:
		return GL_QUAD_STRIP;
		break;
	case TRESTriangleFan:
		return GL_TRIANGLE_FAN;
		break;
	default:
		// We shouldn't ever get here.
		return GL_TRIANGLES;
		break;
	}
}

int TREShapeGroup::numPointsForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return 2;
		break;
	case TRESTriangle:
		return 3;
		break;
	case TRESQuad:
		return 4;
		break;
	case TRESConditionalLine:
		return 2;
		break;
	default:
		// Strips are variable size
		return 0;
		break;
	}
}

void TREShapeGroup::drawShapeType(TREShapeType shapeType)
{
	TCULongArray *indexArray = getIndices(shapeType);

	if (indexArray)
	{
		glDrawElements(modeForShapeType(shapeType), indexArray->getCount(),
			GL_UNSIGNED_INT, indexArray->getValues());
		if (shapeType == TRESLine && m_mainModel->getActiveLineJoinsFlag())
		{
			glDrawElements(GL_POINTS, indexArray->getCount(), GL_UNSIGNED_INT,
				indexArray->getValues());
		}
	}
}

// This is really ugly, but I really need to use dynamic arrays while filling
// in the data.  Since that can't be passed to glMultiDrawElements, I have to
// create a new copy of the data and copy it here.
void TREShapeGroup::initMultiDrawIndices(void)
{
	if (!m_multiDrawIndices)
	{
		int shapeTypeCount = m_indices->getCount();
		int i, j;

		m_multiDrawIndices = new TCULong**[shapeTypeCount];
		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *stripCounts = (*m_stripCounts)[i];

			if (stripCounts)
			{
				TCULongArray *indices = (*m_indices)[i];
				int numStrips = stripCounts->getCount();

				if (numStrips)
				{
					int indexOffset = 0;

					m_multiDrawIndices[i] = new TCULong*[numStrips];
					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*stripCounts)[j];

						if (stripCount)
						{
							m_multiDrawIndices[i][j] = new TCULong[stripCount];
							memcpy(m_multiDrawIndices[i][j],
								indices->getValues() + indexOffset,
								stripCount * sizeof(TCULong));
						}
						else
						{
							m_multiDrawIndices[i][j] = NULL;
						}
						indexOffset += stripCount;
					}
				}
				else
				{
					m_multiDrawIndices[i] = NULL;
				}
			}
			else
			{
				m_multiDrawIndices[i] = NULL;
			}
		}
	}
}

// The m_multiDrawIndices array doesn't have any internal tracking of the size
// of the various nesting levels, so we just need to get that info from the
// other variables the same way we did when we originally created it.
void TREShapeGroup::deleteMultiDrawIndices(void)
{
	if (m_multiDrawIndices)
	{
		int shapeTypeCount = m_indices->getCount();
		int i, j;

		for (i = 0; i < shapeTypeCount; i++)
		{
			TCULongArray *stripCounts = (*m_stripCounts)[i];

			if (stripCounts)
			{
				int numStrips = stripCounts->getCount();

				if (numStrips)
				{
					for (j = 0; j < numStrips; j++)
					{
						if (m_multiDrawIndices[i][j])
						{
							delete m_multiDrawIndices[i][j];
						}
					}
				}
			}
			if (m_multiDrawIndices[i])
			{
				delete m_multiDrawIndices[i];
			}
		}
		delete m_multiDrawIndices;
	}
}

void TREShapeGroup::drawStripShapeType(TREShapeType shapeType)
{
	if (m_shapesPresent & shapeType)
	{
		TCULongArray *indexArray = getIndices(shapeType);
		TCULongArray *countArray = getStripCounts(shapeType);

		if (indexArray && countArray)
		{
			int numStrips = countArray->getCount();

			if (numStrips)
			{
				GLenum glMode = modeForShapeType(shapeType);

				if (glMultiDrawElementsEXT)
				{
					int shapeTypeIndex = getShapeTypeIndex(shapeType);

					if (!m_multiDrawIndices)
					{
						initMultiDrawIndices();
					}
					glMultiDrawElementsEXT(glMode,
						(GLsizei *)countArray->getValues(), GL_UNSIGNED_INT,
						(const void **)m_multiDrawIndices[shapeTypeIndex],
						numStrips);
				}
				else
				{
					TCULong *indices = indexArray->getValues();
					int indexOffset = 0;
					int i;

					for (i = 0; i < numStrips; i++)
					{
						int stripCount = (*countArray)[i];

						glDrawElements(glMode, stripCount, GL_UNSIGNED_INT,
							indices + indexOffset);
						indexOffset += stripCount;
					}
				}
			}
		}
	}
}

void TREShapeGroup::draw(void)
{
	if (m_vertexStore)
	{
		drawShapeType(TRESTriangle);
		drawShapeType(TRESQuad);
		drawStripShapeType(TRESTriangleStrip);
		drawStripShapeType(TRESQuadStrip);
		drawStripShapeType(TRESTriangleFan);
	}
}

void TREShapeGroup::drawLines(void)
{
	if (m_vertexStore)
	{
		drawShapeType(TRESLine);
	}
}

//***********************************************************************
// Return 1 if the v2 bends left of v1, -1 if right, 0 if straight ahead.
int TREShapeGroup::turnVector(float vx1, float vy1, float vx2, float vy2)
{
	// Pos for left bend, 0 = linear
	float vecProduct = (vx1 * vy2) - (vy1 * vx2);

	if (vecProduct > 0.0)
	{
		return 1;
	}
	if (vecProduct < 0.0)
	{
		return -1;
	}
	return 0;
}

void TREShapeGroup::transformPoint(const TCVector &point, const float *matrix,
								   float *tx, float *ty)
{
	float x = point.get(0);
	float y = point.get(1);
	float z = point.get(2);
	float tw;

//	x' = a*x + b*y + c*z + X
//	y' = d*x + e*y + f*z + Y
//	z' = g*x + h*y + i*z + Z
	tw = 1.0f / (matrix[3]*x + matrix[7]*y + matrix[11]*z + matrix[15]);
	*tx = (matrix[0]*x + matrix[4]*y + matrix[8]*z + matrix[12]) * tw;
	*ty = (matrix[1]*x + matrix[5]*y + matrix[9]*z + matrix[13]) * tw;
}

bool TREShapeGroup::shouldDrawConditional(TCULong index1, TCULong index2,
										  TCULong cpIndex1, TCULong cpIndex2,
										  const float *matrix)
{
	// Use matrix--which contains a combination of the projection and the
	// model-view matrix--to calculate coords in the plane of the screen, so
	// we can test optional lines.
	float s1x, s1y;
	float s2x, s2y;
	float s3x, s3y;
	float s4x, s4y;
	TREVertexArray *vertices = m_vertexStore->getVertices();
	TREVertexArray *controlPoints = vertices; //m_vertexStore->getControlPoints();
	const TREVertex &v1 = (*vertices)[index1];
	const TREVertex &v2 = (*vertices)[index2];
	const TREVertex &v3 = (*controlPoints)[cpIndex1];
	const TREVertex &v4 = (*controlPoints)[cpIndex2];
	TCVector p1 = TCVector(v1.v[0], v1.v[1], v1.v[2]);
	TCVector p2 = TCVector(v2.v[0], v2.v[1], v2.v[2]);
	TCVector p3 = TCVector(v3.v[0], v3.v[1], v3.v[2]);
	TCVector p4 = TCVector(v4.v[0], v4.v[1], v4.v[2]);

	// Only draw optional line p1-p2 if p3 and p4 are on the same side of p1-p2.
	// Note that we don't actually adjust for the window size, because it
	// doesn't effect the calculation.  Also, we don't care what the z value is,
	// so we don't bother to compute it.
	transformPoint(p1, matrix, &s1x, &s1y);
	transformPoint(p2, matrix, &s2x, &s2y);
	transformPoint(p3, matrix, &s3x, &s3y);
	transformPoint(p4, matrix, &s4x, &s4y);

	// If we do not turn the same direction \_/ for both test points
	// then they're on opposite sides of segment p1-p2 and we should
	// skip drawing this conditional line.
	if (turnVector(s2x-s1x, s2y-s1y, s3x-s2x, s3y-s2y) == 
		turnVector(s2x-s1x, s2y-s1y, s4x-s2x, s4y-s2y))
	{
		return true;	// Draw it
	}
	else
	{
		return false;	// Skip it.
	}
}

/*
static void printULongArray(TCULongArray *array)
{
	if (array)
	{
		int i;
		int count = array->getCount();

		for (i = 0; i < count; i++)
		{
			printf("%d\n", (*array)[i]);
		}
	}
}

static void printVertices(TREVertexArray *vertices, TCULongArray *indices)
{
	if (vertices && indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			int index = (*indices)[i];
			const TREVertex &vertex = (*vertices)[index];

			printf("%14.10f %14.10f %14.10f\n", vertex.v[0], vertex.v[1],
				vertex.v[2]);
		}
	}
}
*/

void TREShapeGroup::drawConditionalLines(void)
{
	if (m_vertexStore)
	{
		TCULongArray *indices = getIndices(TRESConditionalLine);

		if (indices)
		{
			bool showAllConditional =
				m_vertexStore->getShowAllConditionalFlag();
			bool showConditionalControlPoints =
				m_vertexStore->getConditionalControlPointsFlag();
			TCULongArray *activeIndices;

			if (showAllConditional && !showConditionalControlPoints)
			{
				activeIndices = indices;
				indices->retain();
			}
			else
			{
				int i;
				int count = indices->getCount();
				float modelViewMatrix[16];
				float projectionMatrix[16];
				float matrix[16];

				activeIndices = new TCULongArray;
				glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
				glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
				TCVector::multMatrix(projectionMatrix, modelViewMatrix, matrix);
				for (i = 0; i < count; i += 2)
				{
					TCULong index1 = (*indices)[i];
					TCULong index2 = (*indices)[i + 1];
					TCULong cpIndex1 = (*m_controlPointIndices)[i];
					TCULong cpIndex2 = (*m_controlPointIndices)[i + 1];

					if (showAllConditional ||
						shouldDrawConditional(index1, index2, cpIndex1,
						cpIndex2, matrix))
					{
						activeIndices->addValue(index1);
						activeIndices->addValue(index2);
						if (showConditionalControlPoints)
						{
							activeIndices->addValue(index1);
							activeIndices->addValue(cpIndex1);
							activeIndices->addValue(index1);
							activeIndices->addValue(cpIndex2);
						}
					}
				}
			}
			if (activeIndices->getCount())
			{
				glDrawElements(GL_LINES, activeIndices->getCount(),
					GL_UNSIGNED_INT, activeIndices->getValues());
				if (m_mainModel->getActiveLineJoinsFlag())
				{
					glDrawElements(GL_POINTS, activeIndices->getCount(),
						GL_UNSIGNED_INT, activeIndices->getValues());
				}
			}
			activeIndices->release();
		}
	}
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							int count)
{
	return addShape(shapeType, vertices, NULL, count);
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	return addShape(shapeType, vertices, normals, NULL, count);
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, TCVector *textureCoords,
							int count)
{
	int index;

	if (textureCoords)
	{
		m_vertexStore->setupTextured();
	}
	else
	{
		m_vertexStore->setup();
	}
	if (normals)
	{
		if (textureCoords)
		{
			index = m_vertexStore->addVertices(vertices, normals, textureCoords,
				count);
		}
		else
		{
			index = m_vertexStore->addVertices(vertices, normals, count);
		}
	}
	else
	{
		index = m_vertexStore->addVertices(vertices, count);
	}
	addShapeIndices(shapeType, index, count);
	return index;
}

int TREShapeGroup::addConditionalLine(TCVector *vertices,
									  TCVector *controlPoints)
{
	int index;

	m_vertexStore->setup();
	if (!m_controlPointIndices)
	{
		m_controlPointIndices = new TCULongArray;
	}
	index = m_vertexStore->addVertices(controlPoints, 2);
	addIndices(m_controlPointIndices, index, 2);
	index = m_vertexStore->addVertices(vertices, 2);
	addShapeIndices(TRESConditionalLine, index, 2);
	return index;
}

int TREShapeGroup::addLine(TCVector *vertices)
{
	return addShape(TRESLine, vertices, 2);
}

int TREShapeGroup::addTriangle(TCVector *vertices)
{
	return addShape(TRESTriangle, vertices, 3);
}

int TREShapeGroup::addTriangle(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESTriangle, vertices, normals, 3);
}

int TREShapeGroup::addTriangle(TCVector *vertices, TCVector *normals,
							   TCVector *textureCoords)
{
	return addShape(TRESTriangle, vertices, normals, textureCoords, 3);
}

int TREShapeGroup::addQuad(TCVector *vertices)
{
	return addShape(TRESQuad, vertices, 4);
}

int TREShapeGroup::addQuad(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESQuad, vertices, normals, 4);
}

int TREShapeGroup::addTriangleStrip(TCVector *vertices, TCVector *normals,
									int count)
{
	return addStrip(TRESTriangleStrip, vertices, normals, count);
}

int TREShapeGroup::addQuadStrip(TCVector *vertices, TCVector *normals,
								int count)
{
	return addStrip(TRESQuadStrip, vertices, normals, count);
}

int TREShapeGroup::addTriangleFan(TCVector *vertices, TCVector *normals,
								  int count)
{
	return addStrip(TRESTriangleFan, vertices, normals, count);
}

int TREShapeGroup::addTriangleFan(TCVector *vertices, TCVector *normals,
								  TCVector *textureCoords, int count)
{
	return addStrip(TRESTriangleFan, vertices, normals, textureCoords, count);
}

int TREShapeGroup::addStrip(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	return addStrip(shapeType, vertices, normals, NULL, count);
}

int TREShapeGroup::addStrip(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, TCVector *textureCoords,
							int count)
{
	int index;

	if (textureCoords)
	{
		m_vertexStore->setupTextured();
	}
	else
	{
		m_vertexStore->setup();
	}
	if (textureCoords)
	{
		index = m_vertexStore->addVertices(vertices, normals, textureCoords,
			count);
	}
	else
	{
		index = m_vertexStore->addVertices(vertices, normals, count);
	}
	addShapeStripCount(shapeType, count);
	addShapeIndices(shapeType, index, count);
	return index;
}

void TREShapeGroup::setVertexStore(TREVertexStore *vertexStore)
{
	vertexStore->retain();
	TCObject::release(m_vertexStore);
	m_vertexStore = vertexStore;
}

void TREShapeGroup::scanPoints(TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	int bit;

	for (bit = 1; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		scanPoints(getIndices((TREShapeType)bit), scanner, scanPointCallback,
			matrix);
	}
	for (; (TREShapeType)bit <= TRESLast; bit = bit << 1)
	{
		scanStripPoints(getIndices((TREShapeType)bit),
			getStripCounts((TREShapeType)bit), scanner, scanPointCallback,
			matrix);
	}
}

void TREShapeGroup::scanPoints(TCULongArray *indices, TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	if (indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			scanPoints((*indices)[i], scanner, scanPointCallback, matrix);
		}
	}
}

void TREShapeGroup::scanStripPoints(TCULongArray *indices,
									TCULongArray *stripCounts,
									TCObject *scanner,
									TREScanPointCallback scanPointCallback,
									float* matrix)
{
	if (indices && stripCounts)
	{
		int i, j;
		int numStrips = stripCounts->getCount();
		int indexOffset = 0;

		for (i = 0; i < numStrips; i++)
		{
			int stripCount = (*stripCounts)[i];
			for (j = 0; j < stripCount; j++)
			{
				scanPoints((*indices)[j + indexOffset], scanner,
					scanPointCallback, matrix);
			}
			indexOffset += stripCount;
		}
	}
}

void TREShapeGroup::scanPoints(const TREVertex &vertex, TCObject *scanner,
							   TREScanPointCallback scanPointCallback)
{
	TCVector point = TCVector(vertex.v[0], vertex.v[1], vertex.v[2]);
	((*scanner).*scanPointCallback)(point);
}

void TREShapeGroup::scanPoints(TCULong index, TCObject *scanner,
							   TREScanPointCallback scanPointCallback,
							   float* matrix)
{
	TREVertex vertex = (*m_vertexStore->getVertices())[index];

	transformVertex(vertex, matrix);
	scanPoints(vertex, scanner, scanPointCallback);
}

void TREShapeGroup::unshrinkNormals(float *matrix, float *unshrinkMatrix)
{
	int bit;

	// Skip lines and conditional lines, whose normals don't matter
	for (bit = TRESTriangle; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		unshrinkNormals(getIndices((TREShapeType)bit), matrix, unshrinkMatrix);
	}
	for (; (TREShapeType)bit <= TRESLast; bit = bit << 1)
	{
		unshrinkStripNormals(getIndices((TREShapeType)bit),
			getStripCounts((TREShapeType)bit), matrix, unshrinkMatrix);
	}
}

void TREShapeGroup::unshrinkNormals(TCULongArray *indices, float *matrix,
									float *unshrinkMatrix)
{
	if (indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			unshrinkNormal((*indices)[i], matrix, unshrinkMatrix);
		}
	}
}

void TREShapeGroup::unshrinkStripNormals(TCULongArray *indices,
										 TCULongArray *stripCounts,
										 float *matrix, float *unshrinkMatrix)
{
	if (indices && stripCounts)
	{
		int i, j;
		int numStrips = stripCounts->getCount();
		int indexOffset = 0;

		for (i = 0; i < numStrips; i++)
		{
			int stripCount = (*stripCounts)[i];
			for (j = 0; j < stripCount; j++)
			{
				unshrinkNormal((*indices)[j + indexOffset], matrix,
					unshrinkMatrix);
			}
			indexOffset += stripCount;
		}
	}
}

void TREShapeGroup::unshrinkNormal(TCULong index, float *matrix,
								   float *unshrinkMatrix)
{
	TREVertexArray *normals = m_vertexStore->getNormals();
	TREVertex &normal = normals->vertexAtIndex(index);
	TCVector newNormal = TCVector(normal.v[0], normal.v[1], normal.v[2]);
	float adjust = newNormal.length();

	newNormal = newNormal.transformNormal(matrix);
	newNormal = newNormal.transformNormal(unshrinkMatrix, false);
	adjust = 1.0f / newNormal.length();
	normal.v[0] *= adjust;
	normal.v[1] *= adjust;
	normal.v[2] *= adjust;
}

static void invertULongArray(TCULongArray *array, int start = 0, int end = -1)
{
	int i;
	int count = array->getCount();

	if (end == -1)
	{
		end = count;
	}
	for (i = start; i < (end - start) / 2 + start; i++)
	{
		TCULong temp1 = (*array)[i];
		TCULong temp2 = (*array)[end - i - 1 + start];

		array->replaceValue(temp2, i);
		array->replaceValue(temp1, end - i - 1 + start);
	}
}

int TREShapeGroup::flipNormal(int index)
{
	TREVertexArray *vertices = m_vertexStore->getVertices();
	TREVertexArray *normals = m_vertexStore->getNormals();
	TREVertexArray *textureCoords = m_vertexStore->getTextureCoords();
	TCULongArray *colors = m_vertexStore->getColors();
	TREVertex vertex = (*vertices)[index];
	TREVertex normal = (*normals)[index];

	normal.v[0] = -normal.v[0];
	normal.v[1] = -normal.v[1];
	normal.v[2] = -normal.v[2];
	vertices->addVertex(vertex);
	normals->addVertex(normal);
	if (textureCoords)
	{
		TREVertex textureCoord = (*textureCoords)[index];

		textureCoord.v[0] = 1.0f - textureCoord.v[0];
		textureCoords->addVertex(textureCoord);
	}
	if (colors)
	{
		colors->addValue((*colors)[index]);
	}
	return vertices->getCount() - 1;
}

void TREShapeGroup::invertShapes(TCULongArray *oldIndices,
								 TCULongArray *newIndices)
{
	int i;
	int indexCount = oldIndices->getCount();

	for (i = indexCount - 1; i >= 0; i--)
	{
		int index = (*oldIndices)[i];

		newIndices->addValue(flipNormal(index));
	}
}

void TREShapeGroup::invert(void)
{
	if (m_indices)
	{
		int i, j;
		int shapeTypeCount = m_indices->getCount();
		int firstStripIndex = getShapeTypeIndex(TRESFirstStrip);
		int triangleStripIndex = getShapeTypeIndex(TRESTriangleStrip);
		int quadStripIndex = getShapeTypeIndex(TRESQuadStrip);
		int triangleFanIndex = getShapeTypeIndex(TRESTriangleFan);
		int conditionalLineIndex = getShapeTypeIndex(TRESConditionalLine);

		for (i = 0; i < shapeTypeCount; i++)
		{
			if ((m_shapesPresent & TRESConditionalLine) &&
				i == conditionalLineIndex)
			{
				continue;
			}
			TCULongArray *theseIndices = (*m_indices)[i];
			TCULongArray *newIndices =
				new TCULongArray(theseIndices->getCount());
			TCULongArray *theseStripCounts = (*m_stripCounts)[i];
			int indexCount = theseIndices->getCount();

			if (i < firstStripIndex)
			{
				invertShapes(theseIndices, newIndices);
			}
			else
			{
				if ((m_shapesPresent & TRESTriangleStrip) &&
					i == triangleStripIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						if (stripCount % 2)
						{
							invertULongArray(theseIndices, indexOffset,
								indexOffset + stripCount);
						}
						else
						{
							printf("Cannot invert tri strip with even number "
								"of points.\n");
						}
						indexOffset += stripCount;
					}
					for (j = 0; j < indexCount; j++)
					{
						newIndices->addValue(flipNormal((*theseIndices)[j]));
					}
				}
				else if ((m_shapesPresent & TRESQuadStrip) &&
					i == quadStripIndex)
				{
					for (j = 0; j < indexCount; j += 2)
					{
						TCULong index1 = (*theseIndices)[j];
						TCULong index2 = (*theseIndices)[j + 1];

						newIndices->addValue(flipNormal(index2));
						newIndices->addValue(flipNormal(index1));
					}
				}
				else if ((m_shapesPresent & TRESTriangleFan) &&
					i == triangleFanIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						invertULongArray(theseIndices, indexOffset + 1,
							indexOffset + stripCount);
						indexOffset += stripCount;
					}
					for (j = 0; j < indexCount; j++)
					{
						newIndices->addValue(flipNormal((*theseIndices)[j]));
					}
				}
			}
			m_indices->replaceObject(newIndices, i);
			newIndices->release();
		}
	}
}

void TREShapeGroup::unMirror(void)
{
	if (m_indices)
	{
		int i, j;
		int shapeTypeCount = m_indices->getCount();
		int firstStripIndex = getShapeTypeIndex(TRESFirstStrip);
		int triangleStripIndex = getShapeTypeIndex(TRESTriangleStrip);
		int quadStripIndex = getShapeTypeIndex(TRESQuadStrip);
		int triangleFanIndex = getShapeTypeIndex(TRESTriangleFan);
		int conditionalLineIndex = getShapeTypeIndex(TRESConditionalLine);

		for (i = 0; i < shapeTypeCount; i++)
		{
			if ((m_shapesPresent & TRESConditionalLine) &&
				i == conditionalLineIndex)
			{
				continue;
			}
			TCULongArray *theseIndices = (*m_indices)[i];
			TCULongArray *theseStripCounts = (*m_stripCounts)[i];
			int indexCount = theseIndices->getCount();

			if (i < firstStripIndex)
			{
				invertULongArray(theseIndices);
				mirrorTextureCoords(theseIndices);
			}
			else
			{
				if ((m_shapesPresent & TRESTriangleStrip) &&
					i == triangleStripIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						if (stripCount % 2)
						{
							invertULongArray(theseIndices, indexOffset,
								indexOffset + stripCount);
						}
						else
						{
							printf("Cannot un-mirror tri strip with even "
								"number of points.\n");
						}
						indexOffset += stripCount;
					}
				}
				else if ((m_shapesPresent & TRESQuadStrip) &&
					i == quadStripIndex)
				{
					for (j = 0; j < indexCount; j += 2)
					{
						TCULong temp1 = (*theseIndices)[j];
						TCULong temp2 = (*theseIndices)[j + 1];

						theseIndices->replaceValue(temp2, j);
						theseIndices->replaceValue(temp1, j + 1);
					}
				}
				else if ((m_shapesPresent & TRESTriangleFan) &&
					i == triangleFanIndex)
				{
					int numStrips = theseStripCounts->getCount();
					int indexOffset = 0;

					for (j = 0; j < numStrips; j++)
					{
						int stripCount = (*theseStripCounts)[j];

						invertULongArray(theseIndices, indexOffset + 1,
							indexOffset + stripCount);
						indexOffset += stripCount;
					}
					mirrorTextureCoords(theseIndices);
				}
			}
		}
	}
}

void TREShapeGroup::mirrorTextureCoords(TCULongArray *indices)
{
	TREVertexArray *textureCoords = m_vertexStore->getTextureCoords();

	if (textureCoords)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			int index = (*indices)[i];
			TREVertex &textureCoord = textureCoords->vertexAtIndex(index);

			textureCoord.v[0] = 1.0f - textureCoord.v[0];
		}
	}
}

void TREShapeGroup::transferTriangle(TCULong color, TCULong index0,
									 TCULong index1, TCULong index2,
									 const float *matrix)
{
	TREVertexArray *oldVertices = m_vertexStore->getVertices();
	TREVertexArray *oldNormals = m_vertexStore->getNormals();
	TREVertex vertex = (*oldVertices)[index0];
	TREVertex normal = (*oldNormals)[index0];
	TCVector vertices[3];
	TCVector normals[3];
	bool mirrored = TCVector::determinant(matrix) < 0.0 ? true : false;

	vertices[0] =
		TCVector(vertex.v[0], vertex.v[1], vertex.v[2]).transformPoint(matrix);
	normals[0] =
		TCVector(normal.v[0], normal.v[1], normal.v[2]).transformNormal(matrix);
	if (mirrored)
	{
		normals[0] *= -1.0f;
	}
	vertex = (*oldVertices)[index1];
	normal = (*oldNormals)[index1];
	vertices[1] =
		TCVector(vertex.v[0], vertex.v[1], vertex.v[2]).transformPoint(matrix);
	normals[1] =
		TCVector(normal.v[0], normal.v[1], normal.v[2]).transformNormal(matrix);
	if (mirrored)
	{
		normals[1] *= -1.0f;
	}
	vertex = (*oldVertices)[index2];
	normal = (*oldNormals)[index2];
	vertices[2] =
		TCVector(vertex.v[0], vertex.v[1], vertex.v[2]).transformPoint(matrix);
	normals[2] =
		TCVector(normal.v[0], normal.v[1], normal.v[2]).transformNormal(matrix);
	if (mirrored)
	{
		normals[2] *= -1.0f;
	}
	m_mainModel->addTransparentTriangle(color, vertices, normals);
}

void TREShapeGroup::transferQuadStrip(int shapeTypeIndex, TCULong color,
									  int offset, int stripCount,
									  const float *matrix, bool remove)
{
	int i;
	TCULongArray *indices = (*m_indices)[shapeTypeIndex];

	for (i = offset; i < offset + stripCount - 2; i++)
	{
		if ((i - offset) % 2)
		{
			transferTriangle(color, (*indices)[i], (*indices)[i + 2],
				(*indices)[i + 1], matrix);
		}
		else
		{
			transferTriangle(color, (*indices)[i], (*indices)[i + 1],
				(*indices)[i + 2], matrix);
		}
	}
	if (remove)
	{
		indices->removeValues(offset, stripCount);
	}
}

void TREShapeGroup::transferTriangleStrip(int shapeTypeIndex, TCULong color,
										  int offset, int stripCount,
										  const float *matrix, bool remove)
{
	int i;
	TCULongArray *indices = (*m_indices)[shapeTypeIndex];

	for (i = offset; i < offset + stripCount - 2; i++)
	{
		transferTriangle(color, (*indices)[i], (*indices)[i + 1],
			(*indices)[i + 2], matrix);
/*
		if ((i - offset) % 2)
		{
			transferTriangle(color, (*indices)[i], (*indices)[i + 2],
				(*indices)[i + 1], matrix);
		}
*/
	}
	if (remove)
	{
		indices->removeValues(offset, stripCount);
	}
}

void TREShapeGroup::transferTriangleFan(int shapeTypeIndex, TCULong color,
										int offset, int stripCount,
										const float *matrix, bool remove)
{
	int i;
	TCULongArray *indices = (*m_indices)[shapeTypeIndex];

	for (i = offset; i < offset + stripCount - 2; i++)
	{
		transferTriangle(color, (*indices)[offset], (*indices)[i + 1],
			(*indices)[i + 2], matrix);
	}
	if (remove)
	{
		indices->removeValues(offset, stripCount);
	}
}

bool TREShapeGroup::isTransparent(TCULong color, bool hostFormat)
{
	if (hostFormat)
	{
		return (htonl(color) & 0xFF) < 0xFF;
	}
	else
	{
		return (color & 0xFF) < 0xFF;
	}
}

void TREShapeGroup::transferTransparent(TCULong color, const float *matrix)
{
	if (m_indices && isTransparent(color, true))
	{
		int bit;

		for (bit = TRESFirst; (TREShapeType)bit <= TRESLast; bit = bit << 1)
		{
			TREShapeType shapeType = (TREShapeType)bit;
			transferTransparent(htonl(color), shapeType, getIndices(shapeType),
				matrix);
		}
	}
}

void TREShapeGroup::transferTransparent(TCULong color, TREShapeType shapeType,
										TCULongArray *indices,
										const float *matrix)
{
	TREVertexArray *oldVertices = m_vertexStore->getVertices();
	TREVertexArray *oldNormals = m_vertexStore->getNormals();

	if (indices && oldVertices && oldNormals)
	{
		int i;
		int count = indices->getCount();

		if (shapeType == TRESTriangle || shapeType == TRESQuad)
		{
			int shapeSize = numPointsForShapeType(shapeType);

			if (shapeSize > 2)
			{
				// Start at the end and work backward.  Otherwise our index will
				// have to change every time items are removed.
				for (i = count - shapeSize; i >= 0; i -= shapeSize)
				{
					TCULong index = (*indices)[i];
//					TCVector vertices[3];
//					TCVector normals[3];

					transferTriangle(color, index, (*indices)[i + 1],
						(*indices)[i + 2], matrix);
					if (shapeSize == 4)
					{
						transferTriangle(color, index, (*indices)[i + 2],
							(*indices)[i + 3], matrix);
					}
				}
			}
		}
		else if (shapeType >= TRESFirstStrip && shapeType <= TRESLast)
		{
			int shapeTypeIndex = getShapeTypeIndex(shapeType);
			TCULongArray *stripCounts = (*m_stripCounts)[shapeTypeIndex];
			int numStrips = stripCounts->getCount();
			int offset = count;

			for (i = numStrips - 1; i >= 0; i--)
			{
				int stripCount = (*stripCounts)[i];
				int index;

				offset -= stripCount;
				index = (*indices)[offset];
				switch (shapeType)
				{
				case TRESTriangleStrip:
					transferTriangleStrip(shapeTypeIndex, color, offset,
						stripCount, matrix, false);
					break;
				case TRESQuadStrip:
					transferQuadStrip(shapeTypeIndex, color, offset, stripCount,
						matrix, false);
					break;
				case TRESTriangleFan:
					transferTriangleFan(shapeTypeIndex, color, offset,
						stripCount, matrix, false);
					break;
				default:
					break;
				}
			}
		}
	}
}

void TREShapeGroup::flatten(TREShapeGroup *srcShapes, float *matrix,
							TCULong color, bool colorSet)
{
	TREVertexStore *srcVertexStore = NULL;

	if (srcShapes && (srcVertexStore = srcShapes->getVertexStore()) != NULL &&
		m_vertexStore != NULL)
	{
		TCULong bit;

		for (bit = TRESFirst; bit <= TRESLast; bit = bit << 1)
		{
			TCULongArray *srcIndices = srcShapes->getIndices((TREShapeType)bit);
			
			if (srcIndices)
			{
				TREVertexArray *srcVertices = srcVertexStore->getVertices();
				TREVertexArray *srcNormals = srcVertexStore->getNormals();
				TREVertexArray *srcTextureCoords =
					srcVertexStore->getTextureCoords();
				TCULongArray *srcColors = srcVertexStore->getColors();
				TCULongArray *dstIndices = getIndices((TREShapeType)bit, true);
				TCULongArray *srcCPIndices = NULL;
				TCULongArray *dstCPIndices = NULL;

				if ((TREShapeType)bit == TRESConditionalLine)
				{
					srcCPIndices = srcShapes->getControlPointIndices();
					dstCPIndices = getControlPointIndices(true);
				}
				if (srcVertices)
				{
					TREVertexArray *dstVertices = m_vertexStore->getVertices();
					TREVertexArray *dstNormals = m_vertexStore->getNormals();
					TREVertexArray *dstTextureCoords =
						m_vertexStore->getTextureCoords();
					TCULongArray *dstColors = m_vertexStore->getColors();
					TREShapeType shapeType = (TREShapeType)bit;

					if (shapeType < TRESFirstStrip)
					{
						flattenShapes(dstVertices, dstNormals, dstTextureCoords,
							dstColors, dstIndices, dstCPIndices, srcVertices,
							srcNormals, srcTextureCoords, srcColors, srcIndices,
							srcCPIndices, matrix, color, colorSet);
					}
					else
					{
						TCULongArray *dstStripCounts =
							getStripCounts((TREShapeType)bit, true);
						TCULongArray *srcStripCounts =
							srcShapes->getStripCounts((TREShapeType)bit);

						flattenStrips(dstVertices, dstNormals, dstTextureCoords,
							dstColors, dstIndices, dstStripCounts, srcVertices,
							srcNormals, srcTextureCoords, srcColors, srcIndices,
							srcStripCounts, matrix, color, colorSet);
					}
				}
			}
		}
	}
}

void TREShapeGroup::flattenShapes(TREVertexArray *dstVertices,
								  TREVertexArray *dstNormals,
								  TREVertexArray *dstTextureCoords,
								  TCULongArray *dstColors,
								  TCULongArray *dstIndices,
								  TCULongArray *dstCPIndices,
								  TREVertexArray *srcVertices,
								  TREVertexArray *srcNormals,
								  TREVertexArray *srcTextureCoords,
								  TCULongArray *srcColors,
								  TCULongArray *srcIndices,
								  TCULongArray *srcCPIndices,
								  float *matrix,
								  TCULong color,
								  bool colorSet)
{
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
		if (srcTextureCoords)
		{
			dstTextureCoords->addVertex((*srcTextureCoords)[index]);
		}
		if (colorSet)
		{
			dstColors->addValue(color);
		}
		else if (srcColors)
		{
			dstColors->addValue((*srcColors)[index]);
		}
		if (srcCPIndices && dstCPIndices)
		{
			index = (*srcCPIndices)[i];
			vertex = (*srcVertices)[index];
			dstCPIndices->addValue(dstVertices->getCount());
			transformVertex(vertex, matrix);
			dstVertices->addVertex(vertex);
			if (srcNormals)
			{
				dstNormals->addVertex((*srcNormals)[index]);
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

void TREShapeGroup::flattenStrips(TREVertexArray *dstVertices,
								  TREVertexArray *dstNormals,
								  TREVertexArray *dstTextureCoords,
								  TCULongArray *dstColors,
								  TCULongArray *dstIndices,
								  TCULongArray *dstStripCounts,
								  TREVertexArray *srcVertices,
								  TREVertexArray *srcNormals,
								  TREVertexArray *srcTextureCoords,
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
			if (srcTextureCoords)
			{
				dstTextureCoords->addVertex((*srcTextureCoords)[index]);
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

void TREShapeGroup::transformVertex(TREVertex &vertex, float *matrix)
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

void TREShapeGroup::transformNormal(TREVertex &normal, float *matrix)
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

