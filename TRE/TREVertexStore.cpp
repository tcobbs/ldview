#include "TREVertexStore.h"
#include "TREVertexArray.h"
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCMacros.h>
#include <string.h>

TREVertexStore *TREVertexStore::sm_activeVertexStore = NULL;
PFNWGLALLOCATEMEMORYNVPROC TREVertexStore::wglAllocateMemoryNV = NULL;
PFNWGLFREEMEMORYNVPROC TREVertexStore::wglFreeMemoryNV = NULL;
PFNGLVERTEXARRAYRANGENVPROC TREVertexStore::glVertexArrayRangeNV = NULL;
PFNGLBINDBUFFERARBPROC TREVertexStore::glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC TREVertexStore::glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC TREVertexStore::glGenBuffersARB = NULL;
PFNGLBUFFERDATAARBPROC TREVertexStore::glBufferDataARB = NULL;
TCByte *TREVertexStore::sm_varBuffer = NULL;
int TREVertexStore::sm_varSize = 0;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

TREVertexStore::TREVertexStore(void)
	:m_vertices(NULL),
	m_normals(NULL),
	m_textureCoords(NULL),
	m_colors(NULL),
	m_edgeFlags(NULL),
	m_verticesOffset(0),
	m_normalsOffset(0),
	m_textureCoordsOffset(0),
	m_colorsOffset(0),
	m_edgeFlagsOffset(0),
	m_vbo(0)
{
	m_flags.varTried = false;
	m_flags.varFailed = false;
	m_flags.vboTried = false;
	m_flags.vboFailed = false;
	m_flags.lighting = false;
	m_flags.twoSidedLighting = false;
	m_flags.showAllConditional = false;
	m_flags.conditionalControlPoints = false;
	m_flags.conditionals = false;
}

TREVertexStore::TREVertexStore(const TREVertexStore &other)
	:m_vertices((TREVertexArray *)TCObject::copy(other.m_vertices)),
	m_normals((TREVertexArray *)TCObject::copy(other.m_normals)),
	m_textureCoords((TREVertexArray *)TCObject::copy(other.m_textureCoords)),
	m_colors((TCULongArray *)TCObject::copy(other.m_colors)),
	m_edgeFlags((GLbooleanArray *)TCObject::copy(other.m_edgeFlags)),
	m_verticesOffset(0),
	m_normalsOffset(0),
	m_textureCoordsOffset(0),
	m_colorsOffset(0),
	m_edgeFlagsOffset(0),
	m_vbo(0),
	m_flags(other.m_flags)
{
	m_flags.varTried = false;
	m_flags.varFailed = false;
	m_flags.vboTried = false;
	m_flags.vboFailed = false;
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
	TCObject::release(m_textureCoords);
	TCObject::release(m_colors);
	TCObject::release(m_edgeFlags);
	if (sm_varBuffer && wglFreeMemoryNV)
	{
		wglFreeMemoryNV(sm_varBuffer);
		sm_varBuffer = NULL;
		sm_varSize = 0;
	}
	if (m_vbo && glDeleteBuffersARB)
	{
		glDeleteBuffersARB(1, &m_vbo);
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

int TREVertexStore::addVertices(const TCVector *points, int count,
								GLboolean edgeFlag)
{
	TCVector normal;
	TREVertex normalVertex;
	int i;

	if (count > 2)
	{
		normal = calcNormal(points);
	}
	initVertex(normalVertex, normal);
	for (i = 0; i < count; i++)
	{
		m_normals->addVertex(normalVertex);
	}
	if (getConditionalsFlag())
	{
		if (!m_edgeFlags)
		{
			m_edgeFlags = new GLbooleanArray;
		}
		for (i = m_edgeFlags->getCount(); i < m_vertices->getCount(); i++)
		{
			m_edgeFlags->addValue(GL_TRUE);
		}
		for (i = 0; i < count; i++)
		{
			m_edgeFlags->addValue(edgeFlag);
		}
	}
	return addVertices(m_vertices, points, count);
}

int TREVertexStore::addVertices(const TCVector *points, const TCVector *normals,
								int count)
{
	addVertices(m_normals, normals, count);
	return addVertices(m_vertices, points, count);
}

int TREVertexStore::addVertices(const TCVector *points, const TCVector *normals,
								const TCVector *textureCoords, int count)
{
	addVertices(m_textureCoords, textureCoords, count);
	return addVertices(points, normals, count);
}

int TREVertexStore::addVertices(TCULong color, const TCVector *points,
								int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, count);
}

int TREVertexStore::addVertices(TCULong color, const TCVector *points,
								const TCVector *normals, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, normals, count);
}

int TREVertexStore::addVertices(TCULong color, const TCVector *points,
								const TCVector *normals,
								const TCVector *textureCoords, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		m_colors->addValue(color);
	}
	return addVertices(points, normals, textureCoords, count);
}

void TREVertexStore::initVertex(TREVertex &vertex, const TCVector &point)
{
	memcpy(vertex.v, (const TCFloat *)point, sizeof(vertex.v));
}

int TREVertexStore::addVertices(TREVertexArray *vertices,
								const TCVector *points, int count)
{
	int i;
	TREVertex vertex;

	for (i = 0; i < count; i++)
	{
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
//		GLfloat priority = 1.0f;
		GLfloat priority = 0.0f;
		int offset = sm_varSize;
		TCByte *oldBuffer = NULL;
		int verticesSize = count * sizeof(TREVertex);
		int verticesAllocatedSize = (verticesSize + 31) / 32 * 32;
		int normalsSize = 0;
		int normalsAllocatedSize = 0;
		int textureCoordsSize = 0;
		int textureCoordsAllocatedSize = 0;
		int colorsSize = 0;
		int colorsAllocatedSize = 0;
		int edgeFlagsSize = 0;
		int edgeFlagsAllocatedSize = 0;

		m_flags.varTried = true;
		sm_varSize += count * sizeof(TREVertex);
		if (m_normals)
		{
			normalsSize = verticesSize;
			normalsAllocatedSize = (normalsSize + 31) / 32 * 32;
			sm_varSize += normalsAllocatedSize;
		}
		if (m_textureCoords)
		{
			textureCoordsSize = verticesSize;
			textureCoordsAllocatedSize = (textureCoordsSize + 31) / 32 * 32;
			sm_varSize += textureCoordsAllocatedSize;
		}
		if (m_colors)
		{
			colorsSize = count * sizeof(TCULong);
			colorsAllocatedSize = (colorsSize + 31) / 32 * 32;
			sm_varSize += colorsAllocatedSize;
		}
		if (m_edgeFlags)
		{
			edgeFlagsSize = m_edgeFlags->getCount() * 4;
			edgeFlagsAllocatedSize = (edgeFlagsSize + 31) / 32 * 32;
			sm_varSize += edgeFlagsAllocatedSize;
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
			m_verticesOffset = offset;
			memcpy(sm_varBuffer + m_verticesOffset,
				m_vertices->getVertices(), verticesSize);
			if (m_normals)
			{
				m_normalsOffset = offset + verticesAllocatedSize;
				memcpy(sm_varBuffer + m_normalsOffset,
					m_normals->getVertices(), normalsSize);
			}
			if (m_textureCoords)
			{
				m_textureCoordsOffset = offset + verticesAllocatedSize
					+ normalsAllocatedSize;
				memcpy(sm_varBuffer + m_textureCoordsOffset,
					m_textureCoords->getVertices(), textureCoordsSize);
			}
			if (m_colors)
			{
				m_colorsOffset = offset + verticesAllocatedSize +
					normalsAllocatedSize + textureCoordsAllocatedSize;
				memcpy(sm_varBuffer + m_colorsOffset, m_colors->getItems(),
					colorsSize);
			}
			if (m_edgeFlags)
			{
				m_edgeFlagsOffset = offset + verticesAllocatedSize +
					normalsAllocatedSize + textureCoordsAllocatedSize +
					colorsAllocatedSize;
				memcpy(sm_varBuffer + m_edgeFlagsOffset,
					m_edgeFlags->getItems(), edgeFlagsSize);
			}
			glVertexArrayRangeNV(sm_varSize, sm_varBuffer);
			glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
		}
		else
		{
			sm_varSize = 0;
			m_flags.varFailed = true;
		}
		if (oldBuffer)
		{
			delete oldBuffer;
		}
	}
	else
	{
		m_flags.varFailed = true;
	}
}

void TREVertexStore::setupVBO(void)
{
	if (m_vertices && glBindBufferARB && glDeleteBuffersARB &&
		glGenBuffersARB && glBufferDataARB)
	{
		m_flags.vboTried = true;
		glGenBuffersARB(1, &m_vbo);
		if (m_vbo)
		{
			int count = m_vertices->getCount();
			int verticesSize = count * sizeof(TREVertex);
			int verticesAllocatedSize = (verticesSize + 31) / 32 * 32;
			int normalsSize = 0;
			int normalsAllocatedSize = 0;
			int textureCoordsSize = 0;
			int textureCoordsAllocatedSize = 0;
			int colorsSize = 0;
			int colorsAllocatedSize = 0;
			int edgeFlagsSize = 0;
			int edgeFlagsAllocatedSize = 0;
			TCByte *vboBuffer;
			GLsizeiptrARB vboSize;

			vboSize = verticesAllocatedSize;
			if (m_normals)
			{
				normalsSize = verticesSize;
				normalsAllocatedSize = (normalsSize + 31) / 32 * 32;
				vboSize += normalsAllocatedSize;
			}
			if (m_textureCoords)
			{
				textureCoordsSize = verticesSize;
				textureCoordsAllocatedSize = (textureCoordsSize + 31) / 32 * 32;
				vboSize += textureCoordsAllocatedSize;
			}
			if (m_colors)
			{
				colorsSize = count * sizeof(TCULong);
				colorsAllocatedSize = (colorsSize + 31) / 32 * 32;
				vboSize += colorsAllocatedSize;
			}
			if (m_edgeFlags)
			{
				edgeFlagsSize = m_edgeFlags->getCount() * 4;
				edgeFlagsAllocatedSize = (edgeFlagsSize + 31) / 32 * 32;
				vboSize += edgeFlagsAllocatedSize;
			}
			vboBuffer = new TCByte[vboSize];
			if (vboBuffer)
			{
				memcpy(vboBuffer, m_vertices->getVertices(), verticesSize);
				if (m_normals)
				{
					m_normalsOffset = verticesAllocatedSize;
					memcpy(vboBuffer + m_normalsOffset,
						m_normals->getVertices(), normalsSize);
				}
				if (m_textureCoords)
				{
					m_textureCoordsOffset = verticesAllocatedSize +
						normalsAllocatedSize;
					memcpy(vboBuffer + m_textureCoordsOffset,
						m_textureCoords->getVertices(), textureCoordsSize);
				}
				if (m_colors)
				{
					m_colorsOffset = verticesAllocatedSize +
						normalsAllocatedSize + textureCoordsAllocatedSize;
					memcpy(vboBuffer + m_colorsOffset,
						m_colors->getItems(), colorsSize);
				}
				if (m_edgeFlags)
				{
					m_edgeFlagsOffset = verticesAllocatedSize +
						normalsAllocatedSize + textureCoordsAllocatedSize +
						colorsAllocatedSize;
					memcpy(vboBuffer + m_edgeFlagsOffset,
						m_edgeFlags->getItems(), edgeFlagsSize);
				}
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, vboSize, vboBuffer,
					GL_STATIC_DRAW_ARB);
				if (glGetError())
				{
					m_flags.vboFailed = true;
					vboBuffer = NULL;
				}
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
				if (m_flags.vboFailed)
				{
					glDeleteBuffersARB(1, &m_vbo);
					m_vbo = 0;
				}
				delete vboBuffer;
				vboBuffer = NULL;
			}
			else
			{
				m_flags.vboFailed = true;
			}
		}
		else
		{
			m_flags.vboFailed = true;
		}
	}
	else
	{
		m_flags.vboFailed = true;
	}
}

void TREVertexStore::deactivate(void)
{
	if (sm_activeVertexStore == this)
	{
		sm_activeVertexStore = NULL;
	}
}

bool TREVertexStore::activate(bool displayLists)
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
			if (!m_flags.varTried && !m_flags.varFailed)
			{
				setupVAR();
			}
			if (!displayLists && !m_flags.vboTried && !m_flags.vboFailed &&
				!sm_varBuffer)
			{
				setupVBO();
			}
			if (!displayLists && m_vbo)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
				glVertexPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
					BUFFER_OFFSET(0));
			}
			else
			{
				if (glBindBufferARB)
				{
					glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
				}
				if (sm_varBuffer)
				{
					glVertexPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
						sm_varBuffer + m_verticesOffset);
				}
				else
				{
					glVertexPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
						m_vertices->getVertices());
				}
			}
		}
		else
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (m_normals && getLightingFlag())
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			if (!displayLists && m_vbo)
			{
				glNormalPointer(TRE_GL_FLOAT, sizeof(TREVertex),
					BUFFER_OFFSET(m_normalsOffset));
			}
			else if (sm_varBuffer)
			{
				glNormalPointer(TRE_GL_FLOAT, sizeof(TREVertex),
					sm_varBuffer + m_normalsOffset);
			}
			else
			{
				glNormalPointer(TRE_GL_FLOAT, sizeof(TREVertex),
					m_normals->getVertices());
			}
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if (m_textureCoords)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			if (!displayLists && m_vbo)
			{
				glTexCoordPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
					BUFFER_OFFSET(m_textureCoordsOffset));
			}
			else if (sm_varBuffer)
			{
				glTexCoordPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
					sm_varBuffer + m_textureCoordsOffset);
			}
			else
			{
				glTexCoordPointer(3, TRE_GL_FLOAT, sizeof(TREVertex),
					m_textureCoords->getVertices());
			}
		}
		else
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (m_colors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			if (!displayLists && m_vbo)
			{
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(TCULong),
					BUFFER_OFFSET(m_colorsOffset));
			}
			else if (sm_varBuffer)
			{
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(TCULong),
					sm_varBuffer + m_colorsOffset);
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
		if (m_edgeFlags)
		{
			glEnableClientState(GL_EDGE_FLAG_ARRAY);
			if (!displayLists && m_vbo)
			{
				glEdgeFlagPointer(4, BUFFER_OFFSET(m_edgeFlagsOffset));
			}
			else if (sm_varBuffer)
			{
				glEdgeFlagPointer(4, sm_varBuffer + m_edgeFlagsOffset);
			}
			else
			{
				glEdgeFlagPointer(4, m_edgeFlags->getValues());
			}
		}
		else
		{
			glDisableClientState(GL_EDGE_FLAG_ARRAY);
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

void TREVertexStore::setupTextured(void)
{
	setup();
	if (!m_textureCoords)
	{
		m_textureCoords = new TREVertexArray;
	}
}

TCVector TREVertexStore::calcNormal(const TCVector *points, bool normalize)
{
	TCVector normal = (points[1] - points[2]) * (points[1] - points[0]);
//	TCVector normal = (points[2] - points[1]) * (points[0] - points[1]);

	if (normalize)
	{
		normal.normalize();
	}
	return normal;
}

void TREVertexStore::setLightingFlag(bool value)
{
	m_flags.lighting = value;
	sm_activeVertexStore = NULL;
}

void TREVertexStore::openGlWillEnd(void)
{
	deactivate();
	if (sm_varBuffer && wglFreeMemoryNV)
	{
		wglFreeMemoryNV(sm_varBuffer);
		sm_varBuffer = NULL;
		sm_varSize = 0;
		m_flags.varTried = false;
	}
	if (m_vbo && glDeleteBuffersARB)
	{
		glDeleteBuffersARB(1, &m_vbo);
		m_vbo = 0;
		m_flags.vboTried = false;
	}
}
