#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include <LDLib/Vector.h>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <GL/gl.h>

TREShapeGroup::TREShapeGroup(void)
	:m_vertices(NULL),
	m_normals(NULL),
	m_indices(NULL),
	m_shapesPresent(0)
{
}

TREShapeGroup::TREShapeGroup(const TREShapeGroup &other)
	:TCObject(other),
	m_vertices((TREVertexArray *)TCObject::copy(other.m_vertices)),
	m_normals((TREVertexArray *)TCObject::copy(other.m_normals)),
	m_indices((TCULongArrayArray *)TCObject::copy(other.m_indices)),
	m_shapesPresent(other.m_shapesPresent)
{
}

TREShapeGroup::~TREShapeGroup(void)
{
}

void TREShapeGroup::dealloc(void)
{
	TCObject::release(m_vertices);
	TCObject::release(m_normals);
	TCObject::release(m_indices);
	TCObject::dealloc();
}

TCULongArray *TREShapeGroup::getIndices(TREShapeType shapeType)
{
	return getIndices(shapeType, false);
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

void TREShapeGroup::addConditionalLine(int index1, int index2, int index3,
									   int index4)
{
	addShape(TRESConditionalLine, index1, index2, index3, index4);
}

void TREShapeGroup::addBFCTriangle(int index1, int index2, int index3)
{
	addShape(TRESBFCTriangle, index1, index2, index3);
}

void TREShapeGroup::addBFCQuad(int index1, int index2, int index3, int index4)
{
	addShape(TRESBFCQuad, index1, index2, index3, index4);
}

void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
}

void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2,
							 int index3)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
	indices->addValue(index3);
}

void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2,
							 int index3, int index4)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
	indices->addValue(index3);
	indices->addValue(index4);
}

GLenum modeForShapeType(TREShapeType shapeType)
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
	default:
		// We shouldn't ever get here.
		return GL_TRIANGLES;
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

void TREShapeGroup::draw(void)
{
	if (m_vertices)
	{
		glVertexPointer(3, GL_FLOAT, 0, m_vertices->getVertices());
		glNormalPointer(GL_FLOAT, 0, m_normals->getVertices());
		drawShapeType(TRESTriangle);
		drawShapeType(TRESQuad);
	}
}

void TREShapeGroup::setup(void)
{
	if (!m_vertices)
	{
		m_vertices = new TREVertexArray;
	}
	if (!m_normals)
	{
		m_normals = new TREVertexArray;
	}
}

int TREShapeGroup::addLine(Vector *vertices)
{
	int index;

	setup();
	index = addVertices(vertices, 3);
	addShape(TRESLine, index, index + 1);
	return index;
}

int TREShapeGroup::addTriangle(Vector *vertices)
{
	int index;

	setup();
	index = addVertices(vertices, 3);
	addShape(TRESTriangle, index, index + 1, index + 2);
	return index;
}

int TREShapeGroup::addQuad(Vector *vertices)
{
	int index;

	setup();
	index = addVertices(vertices, 4);
	addShape(TRESQuad, index, index + 1, index + 2, index + 3);
	return index;
}

int TREShapeGroup::addVertices(Vector *points, int count)
{
	Vector normal = calcNormal(points);
	TREVertex normalVertex;
	int i;

	initVertex(normalVertex, normal);
	for (i = 0; i < count; i++)
	{
		m_normals->addVertex(normalVertex);
	}
	return addVertices(m_vertices, points, count);
}

void TREShapeGroup::initVertex(TREVertex &vertex, Vector &point)
{
	memcpy(vertex.v, (float *)point, sizeof(vertex.v));
}

int TREShapeGroup::addVertices(TREVertexArray *vertices, Vector *points,
							   int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		TREVertex vertex;

		initVertex(vertex, points[i]);
		vertices->addVertex(vertex);
	}
	return vertices->getCount() - count;
}

Vector TREShapeGroup::calcNormal(Vector *points, bool normalize)
{
	Vector normal = (points[1] - points[2]) * (points[1] - points[0]);
//	Vector normal = (points[2] - points[1]) * (points[0] - points[1]);

	if (normalize)
	{
		normal.normalize();
	}
	return normal;
}

