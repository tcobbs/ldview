#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include <LDLib/Vector.h>

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
		// We shouldn't ever get here.
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

void TREShapeGroup::draw(void)
{
	if (m_vertexStore)
	{
//		m_vertexStore->activate();
		drawShapeType(TRESTriangle);
		drawShapeType(TRESQuad);
	}
}

int TREShapeGroup::addLine(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 3);
	addShape(TRESLine, index, index + 1);
	return index;
}

int TREShapeGroup::addTriangle(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 3);
	addShape(TRESTriangle, index, index + 1, index + 2);
	return index;
}

int TREShapeGroup::addQuad(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 4);
	addShape(TRESQuad, index, index + 1, index + 2, index + 3);
	return index;
}

void TREShapeGroup::setVertexStore(TREVertexStore *vertexStore)
{
	vertexStore->retain();
	TCObject::release(m_vertexStore);
	m_vertexStore = vertexStore;
}
