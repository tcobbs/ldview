#include "TREVertexStore.h"
#include "TREVertexArray.h"
#include <LDLib/Vector.h>
#include <string.h>

TREVertexStore *TREVertexStore::sm_activeVertexStore = NULL;
PFNWGLALLOCATEMEMORYNVPROC TREVertexStore::wglAllocateMemoryNV = NULL;
PFNWGLFREEMEMORYNVPROC TREVertexStore::wglFreeMemoryNV = NULL;
PFNGLVERTEXARRAYRANGENVPROC TREVertexStore::glVertexArrayRangeNV = NULL;
TCByte *TREVertexStore::sm_varBuffer = NULL;
int TREVertexStore::sm_varSize = 0;

TREVertexStore::TREVertexStore(void)
	:m_vertices(NULL),
	m_normals(NULL),
	m_colors(NULL),
	m_varVerticesOffset(0),
	m_varNormalsOffset(0),
	m_varColorsOffset(0),
	m_varTried(false),
	m_varFailed(false)
{
}

TREVertexStore::TREVertexStore(const TREVertexStore &other)
	:TCObject(other),
	m_vertices((TREVertexArray *)TCObject::copy(other.m_vertices)),
	m_normals((TREVertexArray *)TCObject::copy(other.m_normals)),
	m_colors((TCULongArray *)TCObject::copy(other.m_colors)),
	m_varVerticesOffset(0),
	m_varNormalsOffset(0),
	m_varColorsOffset(0),
	m_varTried(false),
	m_varFailed(false)
{
}

TREVertexStore::~TREVertexStore(void)
{
}

void TREVertexStore::dealloc(void)
{
	if (sm_activeVertexStore == this)
	{
		sm_activeVertexStore = NULL;
	}
	TCObject::release(m_vertices);
	TCObject::release(m_normals);
	TCObject::release(m_colors);
	if (sm_varBuffer && wglFreeMemoryNV)
	{
		wglFreeMemoryNV(sm_varBuffer);
		sm_varBuffer = NULL;
		sm_varSize = 0;
	}
/*
	if (m_varVertices)
	{
		wglFreeMemoryNV(m_varVertices);
		if (m_varNormals)
		{
			wglFreeMemoryNV(m_varNormals);
		}
		if (m_varColors)
		{
			wglFreeMemoryNV(m_varColors);
		}
	}
*/
	TCObject::dealloc();
}

TCObject *TREVertexStore::copy(void)
{
	return new TREVertexStore(*this);
}

int TREVertexStore::addVertices(Vector *points, int count)
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

int TREVertexStore::addVertices(Vector *points, Vector *normals, int count)
{
	addVertices(m_normals, normals, count);
	return addVertices(m_vertices, points, count);
}

int TREVertexStore::addVertices(TCULong color, Vector *points, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, count);
}

int TREVertexStore::addVertices(TCULong color, Vector *points, Vector *normals,
								int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, normals, count);
}

void TREVertexStore::initVertex(TREVertex &vertex, Vector &point)
{
	memcpy(vertex.v, (float *)point, sizeof(vertex.v));
}

int TREVertexStore::addVertices(TREVertexArray *vertices, Vector *points,
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

void TREVertexStore::setupVAR(void)
{
	if (m_vertices && wglAllocateMemoryNV && wglFreeMemoryNV &&
		glVertexArrayRangeNV)
	{
		int count = m_vertices->getCount();
//		float priority = 1.0f;
		float priority = 0.0f;
		int offset = sm_varSize;
		TCByte *oldBuffer = NULL;
		int verticesSize = count * sizeof(TREVertex);
		int verticesAllocatedSize = (verticesSize + 31) / 32 * 32;
		int normalsSize = 0;
		int normalsAllocatedSize = 0;
		int colorsSize = 0;
		int colorsAllocatedSize = 0;

		m_varTried = true;
		sm_varSize += count * sizeof(TREVertex);
		if (m_normals)
		{
			normalsSize = verticesSize;
			normalsAllocatedSize = (normalsSize + 31) / 32 * 32;
			sm_varSize += normalsAllocatedSize;
		}
		if (m_colors)
		{
			colorsSize = count * sizeof(TCULong);
			colorsAllocatedSize = (colorsSize + 31) / 32 * 32;
			sm_varSize += colorsAllocatedSize;
		}
		if (offset)
		{
			oldBuffer = new TCByte[offset];
			memcpy(oldBuffer, sm_varBuffer, offset);
			wglFreeMemoryNV(sm_varBuffer);
		}
		sm_varBuffer = (TCByte *)wglAllocateMemoryNV(sm_varSize, 0.0f, 0.0f,
			priority);
		if (sm_varBuffer)
		{
			if (oldBuffer)
			{
				memcpy(sm_varBuffer, oldBuffer, offset);
			}
			m_varVerticesOffset = offset;
			memcpy(sm_varBuffer + m_varVerticesOffset,
				m_vertices->getVertices(), verticesSize);
			if (m_normals)
			{
				m_varNormalsOffset = offset + verticesAllocatedSize;
				memcpy(sm_varBuffer + m_varNormalsOffset,
					m_normals->getVertices(), normalsSize);
			}
			if (m_colors)
			{
				m_varColorsOffset = offset + verticesAllocatedSize +
					normalsAllocatedSize;
				memcpy(sm_varBuffer + m_varColorsOffset, m_colors->getItems(),
					colorsSize);
			}
			glVertexArrayRangeNV(sm_varSize, sm_varBuffer);
			glEnableClientState(/*0x8533*/GL_VERTEX_ARRAY_RANGE_NV);
		}
		else
		{
			sm_varSize = 0;
			m_varFailed = true;
		}
		if (oldBuffer)
		{
			delete oldBuffer;
		}
	}
	else
	{
		m_varFailed = true;
	}
}

bool TREVertexStore::activate(void)
{
	if (sm_activeVertexStore == this)
	{
		// Already active
		return false;
	}
	else
	{
		if (m_vertices)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			if (!m_varTried && !m_varFailed)
			{
				setupVAR();
			}
			if (sm_varBuffer)
			{
				glVertexPointer(3, GL_FLOAT, sizeof(TREVertex),
					sm_varBuffer + m_varVerticesOffset);
			}
			else
			{
				glVertexPointer(3, GL_FLOAT, sizeof(TREVertex),
					m_vertices->getVertices());
			}
		}
		else
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (m_normals)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			if (sm_varBuffer)
			{
				glNormalPointer(GL_FLOAT, sizeof(TREVertex),
					sm_varBuffer + m_varNormalsOffset);
			}
			else
			{
				glNormalPointer(GL_FLOAT, sizeof(TREVertex),
					m_normals->getVertices());
			}
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if (m_colors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			if (sm_varBuffer)
			{
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(TCULong),
					sm_varBuffer + m_varColorsOffset);
			}
			else
			{
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(TCULong),
					m_colors->getValues());
			}
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}
		sm_activeVertexStore = this;
		return true;
	}
}

void TREVertexStore::setup(void)
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

void TREVertexStore::setupColored(void)
{
	setup();
	if (!m_colors)
	{
		m_colors = new TCULongArray;
	}
}

Vector TREVertexStore::calcNormal(Vector *points, bool normalize)
{
	Vector normal = (points[1] - points[2]) * (points[1] - points[0]);
//	Vector normal = (points[2] - points[1]) * (points[0] - points[1]);

	if (normalize)
	{
		normal.normalize();
	}
	return normal;
}

