#include "TRETransShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include <TCFoundation/TCMacros.h>
#include <stdlib.h>

#ifdef WIN32
#pragma warning(push, 3)
#endif // WIN32

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

TRETransShapeGroup::TRETransShapeGroup(void)
	:m_sortedTriangles(NULL),
	m_useSortThread(false),
	m_sortThread(NULL)
{
}

TRETransShapeGroup::TRETransShapeGroup(const TRETransShapeGroup &other)
	:TREColoredShapeGroup(other),
	m_sortedTriangles((TRESortedTriangleArray *)TCObject::copy(
		other.m_sortedTriangles)),
	m_useSortThread(false),
	m_sortThread(NULL)
{
}

TRETransShapeGroup::~TRETransShapeGroup(void)
{
}

void TRETransShapeGroup::dealloc(void)
{
	TCObject::release(m_sortedTriangles);
	if (m_sortThread)
	{
		m_sortThread->join();
		delete m_sortThread;
		m_sortThread = NULL;
	}
	TREColoredShapeGroup::dealloc();
}

void TRETransShapeGroup::draw(bool sort)
{
	if (sort)
	{
		if (m_useSortThread)
		{
			m_sortThread->join();
			delete m_sortThread;
			m_sortThread = NULL;
		}
		else
		{
			treGlGetFloatv(GL_MODELVIEW_MATRIX, m_sortMatrix);
			sortShapes();
		}
	}
	drawShapeType(TRESTriangle);
//	TREColoredShapeGroup::draw();
}

void TRETransShapeGroup::backgroundSort(void)
{
	sortShapes();
}

void TRETransShapeGroup::sortInBackground(bool sort)
{
	if (sort)
	{
		initSortedTriangles();
		m_useSortThread = true;
		treGlGetFloatv(GL_MODELVIEW_MATRIX, m_sortMatrix);
		m_sortThread = new boost::thread(
			boost::bind(&TRETransShapeGroup::backgroundSort, this));
	}
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

void TRETransShapeGroup::initSortedTriangles(void)
{
	if (!m_sortedTriangles)
	{
		TCULongArray *indices = getIndices(TRESTriangle);

		if (indices)
		{
			int i, j;
			int count = indices->getCount();
			TREVertexArray *vertices = m_vertexStore->getVertices();
			const TCFloat oneThird = 1.0f / 3.0f;

			m_sortedTriangles = new TRESortedTriangleArray;
			for (i = 0; i < count; i += 3)
			{
				TRESortedTriangle *sortedTriangle = new TRESortedTriangle;
				TCFloat midX = 0.0f;
				TCFloat midY = 0.0f;
				TCFloat midZ = 0.0f;

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
}

void TRETransShapeGroup::sortShapes(void)
{
	int i;
	int count;
	TCULong *values;
	TCULongArray *indices = getIndices(TRESTriangle);
	int offset = 0;

	initSortedTriangles();
	count = m_sortedTriangles->getCount();
	for (i = 0; i < count; i++)
	{
		TRESortedTriangle *sortedTriangle = (*m_sortedTriangles)[i];
		sortedTriangle->depth =
			sortedTriangle->center.transformPoint(m_sortMatrix).
			lengthSquared();
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
