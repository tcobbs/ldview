#include "TRETransShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include <TCFoundation/TCMacros.h>

TRETransShapeGroup::TRETransShapeGroup(void)
	:m_sortedTriangles(NULL)
{
}

TRETransShapeGroup::TRETransShapeGroup(const TRETransShapeGroup &other)
	:TREColoredShapeGroup(other),
	m_sortedTriangles((TRESortedTriangleArray *)TCObject::copy(
		other.m_sortedTriangles))
{
}

TRETransShapeGroup::~TRETransShapeGroup(void)
{
}

void TRETransShapeGroup::dealloc(void)
{
	TCObject::release(m_sortedTriangles);
	TREColoredShapeGroup::dealloc();
}

void TRETransShapeGroup::draw(bool sort)
{
	if (sort)
	{
		sortShapes();
	}
	TREColoredShapeGroup::draw();
}

static int triangleCompareFunc(const void *left, const void *right)
{
	TRESortedTriangle *leftTriangle = *(TRESortedTriangle **)left;
	TRESortedTriangle *rightTriangle = *(TRESortedTriangle **)right;

	if (leftTriangle->depth > rightTriangle->depth)
	{
		return -1;
	}
	else if (leftTriangle->depth < rightTriangle->depth)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void TRETransShapeGroup::sortShapes(void)
{
	int i, j;
	int count;
	TREVertexArray *vertices = m_vertexStore->getVertices();
	const float oneThird = 1.0f / 3.0f;
	TCULong *values;
	TCULongArray *indices = getIndices(TRESTriangle);
	int offset = 0;
	float matrix[16];

	if (!m_sortedTriangles)
	{
		if (indices)
		{
			count = indices->getCount();
			m_sortedTriangles = new TRESortedTriangleArray;
			for (i = 0; i < count; i += 3)
			{
				TRESortedTriangle *sortedTriangle = new TRESortedTriangle;
				float midX = 0.0f;
				float midY = 0.0f;
				float midZ = 0.0f;

				sortedTriangle->indices[0] = (*indices)[i];
				sortedTriangle->indices[1] = (*indices)[i + 1];
				sortedTriangle->indices[2] = (*indices)[i + 2];
				for (j = 0; j < 3; j++)
				{
					const TREVertex &vertex =
						(*vertices)[sortedTriangle->indices[j]];

					midX += vertex.v[0];
					midY += vertex.v[1];
					midZ += vertex.v[2];
				}
				sortedTriangle->center = TCVector(midX * oneThird,
					midY * oneThird, midZ * oneThird);
				m_sortedTriangles->addObject(sortedTriangle);
				sortedTriangle->release();
			}
		}
		else
		{
			return;
		}
	}
	count = m_sortedTriangles->getCount();
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	for (i = 0; i < count; i++)
	{
		TRESortedTriangle *sortedTriangle = (*m_sortedTriangles)[i];
		sortedTriangle->depth =
			sortedTriangle->center.transformPoint(matrix).lengthSquared();
	}
	qsort(m_sortedTriangles->getItems(), m_sortedTriangles->getCount(),
		sizeof(void *), triangleCompareFunc);
	values = indices->getValues();
	for (i = 0; i < count; i++, offset += 3)
	{
		TRESortedTriangle *sortedTriangle = (*m_sortedTriangles)[i];

		values[offset] = sortedTriangle->indices[0];
		values[offset + 1] = sortedTriangle->indices[1];
		values[offset + 2] = sortedTriangle->indices[2];
	}
}
