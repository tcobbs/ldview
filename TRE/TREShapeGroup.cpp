#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREModel.h"
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCMacros.h>

TREShapeGroup::TREShapeGroup(void)
	:m_vertexStore(NULL),
	m_indices(NULL),
	m_shapesPresent(0)
{
}

TREShapeGroup::TREShapeGroup(const TREShapeGroup &other)
	:TCObject(other),
	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
	m_indices((TCULongArrayArray *)TCObject::copy(other.m_indices)),
	m_shapesPresent(other.m_shapesPresent)
{
}

TREShapeGroup::~TREShapeGroup(void)
{
}

void TREShapeGroup::dealloc(void)
{
	TCObject::release(m_vertexStore);
	TCObject::release(m_indices);
	TCObject::dealloc();
}

void TREShapeGroup::addShapeType(TREShapeType shapeType, int index)
{
	TCULongArray *newIntArray = new TCULongArray;

	if (!m_indices)
	{
		m_indices = new TCULongArrayArray;
	}
	m_indices->insertObject(newIntArray, index);
	newIntArray->release();
	m_shapesPresent |= shapeType;
}

TCULongArray *TREShapeGroup::getIndices(TREShapeType shapeType, bool create)
{
	TCULong index = getIndex(shapeType);

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

TCULong TREShapeGroup::getIndex(TREShapeType shapeType)
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

/*
void TREShapeGroup::addConditionalLine(int index1, int index2, int index3,
									   int index4)
{
	addShape(TRESConditionalLine, index1, index2, index3, index4);
}
*/

void TREShapeGroup::addShapeIndices(TREShapeType shapeType, int firstIndex,
									int count)
{
	TCULongArray *indices = getIndices(shapeType, true);
	int i;

	for (i = 0; i < count; i++)
	{
		indices->addValue(firstIndex + i);
	}
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
	case TRESBFCTriangle:
		return GL_TRIANGLES;
		break;
	case TRESBFCQuad:
		return GL_QUADS;
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
		return 4;
		break;
	case TRESBFCTriangle:
		return 3;
		break;
	case TRESBFCQuad:
		return 4;
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
	}
}

void TREShapeGroup::drawStripShapeType(TREShapeType shapeType)
{
	TCULongArray *indexArray = getIndices(shapeType);

	if (indexArray)
	{
		TCULong *indices = indexArray->getValues();
		int totalIndices = indexArray->getCount();
		int stripCount = 0;
		int i;
		GLenum glMode = modeForShapeType(shapeType);

		for (i = 0; i < totalIndices; i += stripCount)
		{
			stripCount = indices[i];
			i++;
			glDrawElements(glMode, stripCount, GL_UNSIGNED_INT, indices + i);
		}
	}
}

void TREShapeGroup::draw(void)
{
	drawNonBFC();
	drawBFC();
}

void TREShapeGroup::drawBFC(void)
{
	if (m_vertexStore && (m_shapesPresent & (TRESBFCTriangle | TRESBFCQuad)))
	{
		// Note that GL_BACK is the default face to cull, and GL_CCW is the
		// default polygon winding.
		glEnable(GL_CULL_FACE);
		if (m_vertexStore->getTwoSidedLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		}
		drawShapeType(TRESBFCTriangle);
		drawShapeType(TRESBFCQuad);
		if (m_vertexStore->getTwoSidedLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		}
		glDisable(GL_CULL_FACE);
	}
}

void TREShapeGroup::drawNonBFC(void)
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

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							int count)
{
	return addShape(shapeType, vertices, NULL, count);
}

int TREShapeGroup::addShape(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	if (normals)
	{
		index = m_vertexStore->addVertices(vertices, normals, count);
	}
	else
	{
		index = m_vertexStore->addVertices(vertices, count);
	}
	addShapeIndices(shapeType, index, count);
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

int TREShapeGroup::addQuad(TCVector *vertices)
{
	return addShape(TRESQuad, vertices, 4);
}

int TREShapeGroup::addQuad(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESQuad, vertices, normals, 4);
}

int TREShapeGroup::addBFCTriangle(TCVector *vertices)
{
	return addShape(TRESBFCTriangle, vertices, 3);
}

int TREShapeGroup::addBFCTriangle(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESBFCTriangle, vertices, normals, 3);
}

int TREShapeGroup::addBFCQuad(TCVector *vertices)
{
	return addShape(TRESBFCQuad, vertices, 4);
}

int TREShapeGroup::addBFCQuad(TCVector *vertices, TCVector *normals)
{
	return addShape(TRESBFCQuad, vertices, normals, 4);
}

int TREShapeGroup::addQuadStrip(TCVector *vertices, TCVector *normals, int count)
{
	return addStrip(TRESQuadStrip, vertices, normals, count);
}

int TREShapeGroup::addTriangleFan(TCVector *vertices, TCVector *normals, int count)
{
	return addStrip(TRESTriangleFan, vertices, normals, count);
}

int TREShapeGroup::addStrip(TREShapeType shapeType, TCVector *vertices,
							TCVector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, normals, count);
	addShapeIndices(shapeType, count, 1);
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
		scanStripPoints(getIndices((TREShapeType)bit), scanner,
			scanPointCallback, matrix);
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

void TREShapeGroup::scanStripPoints(TCULongArray *indices, TCObject *scanner,
									TREScanPointCallback scanPointCallback,
									float* matrix)
{
	if (indices)
	{
		int i, j;
		int count = indices->getCount();
		int stripCount = 0;

		for (i = 0; i < count; i += stripCount)
		{
			stripCount = (*indices)[i];
			i++;
			for (j = 0; j < stripCount; j++)
			{
				scanPoints((*indices)[i + j], scanner, scanPointCallback,
					matrix);
			}
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

	TREModel::transformVertex(vertex, matrix);
	scanPoints(vertex, scanner, scanPointCallback);
}

void TREShapeGroup::unshrinkNormals(float *matrix, float *unshrinkMatrix)
{
	int bit;

	for (bit = 1; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		unshrinkNormals(getIndices((TREShapeType)bit), matrix, unshrinkMatrix);
	}
	for (; (TREShapeType)bit <= TRESLast; bit = bit << 1)
	{
		unshrinkStripNormals(getIndices((TREShapeType)bit), matrix,
			unshrinkMatrix);
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

void TREShapeGroup::unshrinkStripNormals(TCULongArray *indices, float *matrix,
										 float *unshrinkMatrix)
{
	if (indices)
	{
		int i, j;
		int count = indices->getCount();
		int stripCount = 0;

		for (i = 0; i < count; i += stripCount)
		{
			stripCount = (*indices)[i];
			i++;
			for (j = 0; j < stripCount; j++)
			{
				unshrinkNormal((*indices)[i + j], matrix, unshrinkMatrix);
			}
		}
	}
}

void TREShapeGroup::unshrinkNormal(TCULong index, float *matrix,
								   float *unshrinkMatrix)
{
	TREVertexArray *normals = m_vertexStore->getNormals();
	TREVertex normal = (*normals)[index];
	TCVector newNormal = TCVector(normal.v[0], normal.v[1], normal.v[2]);
	float adjust;

	if (!fEq(newNormal.length(), 1.0f))
	{
		printf("Huh?\n");
	}
	newNormal = newNormal.transformNormal(matrix);
	newNormal = newNormal.transformNormal(unshrinkMatrix, false);
	adjust = 1.0f / newNormal.length();
	if (adjust > 1.0f)
	{
		printf("adjust: %f\n", adjust);
	}
	normal.v[0] *= adjust;
	normal.v[1] *= adjust;
	normal.v[2] *= adjust;
	normals->replaceVertex(normal, index);
}
