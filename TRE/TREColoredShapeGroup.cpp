#include "TREColoredShapeGroup.h"
#include "TREVertexStore.h"
#include "TREGL.h"

TREColoredShapeGroup::TREColoredShapeGroup(void)
	:m_transparentIndices(NULL),
	m_transparentStripCounts(NULL)
{
}

TREColoredShapeGroup::TREColoredShapeGroup(const TREColoredShapeGroup &other)
	:TREShapeGroup(other),
	m_transparentIndices((TCULongArrayArray *)TCObject::copy(
		other.m_transparentIndices)),
	m_transparentStripCounts((TCULongArrayArray *)TCObject::copy(
		other.m_transparentStripCounts))
{
}

TREColoredShapeGroup::~TREColoredShapeGroup(void)
{
}

void TREColoredShapeGroup::dealloc(void)
{
	TCObject::release(m_transparentIndices);
	TCObject::release(m_transparentStripCounts);
	TREShapeGroup::dealloc();
}

TCObject *TREColoredShapeGroup::copy(void)
{
	return new TREColoredShapeGroup(*this);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   const TCVector *vertices, int count)
{
	return addShape(shapeType, color, vertices, NULL, count);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   const TCVector *vertices,
								   const TCVector *normals, int count)
{
	return addShape(shapeType, color, vertices, normals, NULL, count);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   const TCVector *vertices,
								   const TCVector *normals,
								   const TCVector *textureCoords, int count)
{
	int index;

	m_vertexStore->setupColored();
	if (textureCoords)
	{
		m_vertexStore->setupTextured();
	}
	if (normals)
	{
		if (textureCoords)
		{
			index = m_vertexStore->addVertices(htonl(color), vertices, normals,
				textureCoords, count);
		}
		else
		{
			index = m_vertexStore->addVertices(htonl(color), vertices, normals,
				count);
		}
	}
	else
	{
		index = m_vertexStore->addVertices(htonl(color), vertices, count);
	}
	addShapeIndices(shapeType, index, count);
	return index;
}

int TREColoredShapeGroup::addLine(TCULong color, const TCVector *vertices)
{
	return addShape(TRESLine, color, vertices, 2);
}

int TREColoredShapeGroup::addTriangle(TCULong color, const TCVector *vertices)
{
	return addShape(TRESTriangle, color, vertices, 3);
}

int TREColoredShapeGroup::addTriangle(TCULong color, const TCVector *vertices,
									  const TCVector *normals)
{
	return addShape(TRESTriangle, color, vertices, normals, 3);
}

int TREColoredShapeGroup::addTriangle(TCULong color, const TCVector *vertices,
									  const TCVector *normals,
									  const TCVector *textureCoords)
{
	return addShape(TRESTriangle, color, vertices, normals, textureCoords, 3);
}

int TREColoredShapeGroup::addQuad(TCULong color, const TCVector *vertices)
{
	return addShape(TRESQuad, color, vertices, 4);
}

int TREColoredShapeGroup::addQuad(TCULong color, const TCVector *vertices,
								  const TCVector *normals)
{
	return addShape(TRESQuad, color, vertices, normals, 4);
}

int TREColoredShapeGroup::addQuadStrip(TCULong color, const TCVector *vertices,
									   const TCVector *normals, int count)
{
	return addStrip(color, TRESQuadStrip, vertices, normals, count);
}

int TREColoredShapeGroup::addTriangleFan(TCULong color,
										 const TCVector *vertices,
										 const TCVector *normals, int count)
{
	return addStrip(color, TRESTriangleFan, vertices, normals, count);
}

int TREColoredShapeGroup::addStrip(TCULong color, TREShapeType shapeType,
								   const TCVector *vertices,
								   const TCVector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(htonl(color), vertices, normals, count);
	addShapeStripCount(shapeType, count);
	addShapeIndices(shapeType, index, count);
	return index;
}

void TREColoredShapeGroup::transferColoredTransparent(const TCFloat *matrix)
{
	if (m_indices)
	{
		int bit;

		for (bit = TRESFirst; (TREShapeType)bit <= TRESLast; bit = bit << 1)
		{
			TCULongArray *transparentIndices =
				getTransparentIndices((TREShapeType)bit);

			if (transparentIndices && transparentIndices->getCount())
			{
				// If we already have transparent indices, then we've
				// already processed this model and recorded which indices
				// are transparent.  If that is the case, we don't want to
				// re-record the indices.
				transparentIndices = NULL;
			}
			TREShapeType shapeType = (TREShapeType)bit;
			transferColoredTransparent(shapeType, getIndices(shapeType),
				transparentIndices, matrix);
		}
	}
}

void TREColoredShapeGroup::transferColoredTransparent(TREShapeType shapeType,
	TCULongArray *indices, TCULongArray *transparentIndices,
	const TCFloat *matrix)
{
	TCULongArray *colors = m_vertexStore->getColors();
	TREVertexArray *oldVertices = m_vertexStore->getVertices();
	TREVertexArray *oldNormals = m_vertexStore->getNormals();

	if (indices && colors && oldVertices && oldNormals)
	{
		int i, j;
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
					TCULong color = htonl((*colors)[index]);

					if (isTransparent(color, false))
					{
//						TCVector vertices[3];
//						TCVector normals[3];

						transferTriangle(color, index, (*indices)[i + 1],
							(*indices)[i + 2], matrix);
						if (shapeSize == 4)
						{
							transferTriangle(color, index, (*indices)[i + 2],
								(*indices)[i + 3], matrix);
						}
						if (transparentIndices)
						{
							for (j = shapeSize - 1; j >= 0; j--)
							{
								transparentIndices->addValue(i + j);
							}
						}
					}
				}
			}
		}
		else if (shapeType >= TRESFirstStrip && shapeType <= TRESLast)
		{
			int shapeTypeIndex = getShapeTypeIndex(shapeType);
			TCULongArray *stripCounts = (*m_stripCounts)[shapeTypeIndex];
			TCULongArray *transparentStripCounts =
				getTransparentStripCounts(shapeType);
			int numStrips = stripCounts->getCount();
			int offset = count;

			if (transparentStripCounts && transparentStripCounts->getCount())
			{
				transparentStripCounts = NULL;
			}
			for (i = numStrips - 1; i >= 0; i--)
			{
				int stripCount = (*stripCounts)[i];
				int index;
				int color;

				offset -= stripCount;
				index = (*indices)[offset];
				color = htonl((*colors)[index]);
				if (isTransparent(color, false))
				{
					switch (shapeType)
					{
					case TRESTriangleStrip:
						transferTriangleStrip(shapeTypeIndex, color, offset,
							stripCount, matrix);
						break;
					case TRESQuadStrip:
						transferQuadStrip(shapeTypeIndex, color, offset,
							stripCount, matrix);
						break;
					case TRESTriangleFan:
						transferTriangleFan(shapeTypeIndex, color, offset,
							stripCount, matrix);
						break;
					default:
						break;
					}
					if (transparentIndices)
					{
						for (j = stripCount - 1; j >= 0; j--)
						{
							transparentIndices->addValue(offset + j);
						}
					}
					if (transparentStripCounts)
					{
						transparentStripCounts->addValue(i);
					}
				}
			}
		}
	}
}

TCULongArray *TREColoredShapeGroup::getTransparentIndices(
	TREShapeType shapeType)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		return NULL;
	}
	if (!m_transparentIndices)
	{
		int i;
		int count = m_indices->getCount();

		m_transparentIndices = new TCULongArrayArray;
		for (i = 0; i < count; i++)
		{
			TCULongArray *indices = new TCULongArray;

			m_transparentIndices->addObject(indices);
			indices->release();
		}
	}
	return (*m_transparentIndices)[index];
}

TCULongArray *TREColoredShapeGroup::getTransparentStripCounts(
	TREShapeType shapeType)
{
	TCULong index = getShapeTypeIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		return NULL;
	}
	if (!m_transparentStripCounts)
	{
		int i;
		int count = m_indices->getCount();

		m_transparentStripCounts = new TCULongArrayArray;
		for (i = 0; i < count; i++)
		{
			TCULongArray *stripCounts = new TCULongArray;

			m_transparentStripCounts->addObject(stripCounts);
			stripCounts->release();
		}
	}
	return (*m_transparentStripCounts)[index];
}

void TREColoredShapeGroup::cleanupTransparent(void)
{
	int i, j;

	if (m_transparentIndices)
	{
		int arrayCount = m_transparentIndices->getCount();

		for (i = 0; i < arrayCount; i++)
		{
			TCULongArray *transparentIndices = (*m_transparentIndices)[i];
			TCULongArray *indices = (*m_indices)[i];
			int indexCount = transparentIndices->getCount();

			for (j = 0; j < indexCount; j++)
			{
				indices->removeValue((*transparentIndices)[j]);
			}
		}
		m_transparentIndices->release();
		m_transparentIndices = NULL;
	}
	if (m_transparentStripCounts)
	{
		int arrayCount = m_transparentStripCounts->getCount();

		for (i = 0; i < arrayCount; i++)
		{
			TCULongArray *transparentStripCounts =
				(*m_transparentStripCounts)[i];
			TCULongArray *stripCounts = (*m_stripCounts)[i];
			int stripCountCount = transparentStripCounts->getCount();

			for (j = 0; j < stripCountCount; j++)
			{
				stripCounts->removeValue((*transparentStripCounts)[j]);
			}
		}
		m_transparentStripCounts->release();
		m_transparentStripCounts = NULL;
	}
}
