#include "TREVertexStore.h"
#include "TREVertexArray.h"
#include "TREGL.h"
#include <LDLib/Vector.h>
#include <string.h>

TREVertexStore *TREVertexStore::sm_activeVertexStore = NULL;

TREVertexStore::TREVertexStore(void)
	:m_vertices(NULL),
	m_normals(NULL),
	m_colors(NULL)
{
}

TREVertexStore::TREVertexStore(const TREVertexStore &other)
	:TCObject(other),
	m_vertices((TREVertexArray *)TCObject::copy(other.m_vertices)),
	m_normals((TREVertexArray *)TCObject::copy(other.m_normals)),
	m_colors((TCULongArray *)TCObject::copy(other.m_colors))
{
}

TREVertexStore::~TREVertexStore(void)
{
}

void TREVertexStore::dealloc(void)
{
	TCObject::release(m_vertices);
	TCObject::release(m_normals);
	TCObject::release(m_colors);
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

int TREVertexStore::addVertices(TCULong color, Vector *points, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, count);
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
			glVertexPointer(3, GL_FLOAT, 0, m_vertices->getVertices());
		}
		else
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (m_normals)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, m_normals->getVertices());
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if (m_colors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_colors->getValues());
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

