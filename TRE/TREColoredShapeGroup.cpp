#include "TREColoredShapeGroup.h"
#include "TREVertexStore.h"
#include "TREGL.h"

TREColoredShapeGroup::TREColoredShapeGroup(void)
{
}

TREColoredShapeGroup::TREColoredShapeGroup(const TREColoredShapeGroup &other)
	:TREShapeGroup(other)
{
}

TREColoredShapeGroup::~TREColoredShapeGroup(void)
{
}

void TREColoredShapeGroup::dealloc(void)
{
	TREShapeGroup::dealloc();
}

TCObject *TREColoredShapeGroup::copy(void)
{
	return new TREColoredShapeGroup(*this);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   TCVector *vertices, int count)
{
	return addShape(shapeType, color, vertices, NULL, count);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   const TCVector *vertices,
								   const TCVector *normals, int count)
{
	int index;

	m_vertexStore->setupColored();
	if (normals)
	{
		index = m_vertexStore->addVertices(htonl(color), vertices, normals,
			count);
	}
	else
	{
		index = m_vertexStore->addVertices(htonl(color), vertices, count);
	}
	addShapeIndices(shapeType, index, count);
	return index;
}

int TREColoredShapeGroup::addLine(TCULong color, TCVector *vertices)
{
	return addShape(TRESLine, color, vertices, 2);
}

int TREColoredShapeGroup::addTriangle(TCULong color, TCVector *vertices)
{
	return addShape(TRESTriangle, color, vertices, 3);
}

int TREColoredShapeGroup::addTriangle(TCULong color, const TCVector *vertices,
									  const TCVector *normals)
{
	return addShape(TRESTriangle, color, vertices, normals, 3);
}

int TREColoredShapeGroup::addQuad(TCULong color, TCVector *vertices)
{
	return addShape(TRESQuad, color, vertices, 4);
}

int TREColoredShapeGroup::addQuad(TCULong color, TCVector *vertices,
								  TCVector *normals)
{
	return addShape(TRESQuad, color, vertices, normals, 4);
}

int TREColoredShapeGroup::addQuadStrip(TCULong color, TCVector *vertices,
									   TCVector *normals, int count)
{
	return addStrip(color, TRESQuadStrip, vertices, normals, count);
}

int TREColoredShapeGroup::addTriangleFan(TCULong color, TCVector *vertices,
										 TCVector *normals, int count)
{
	return addStrip(color, TRESTriangleFan, vertices, normals, count);
}

int TREColoredShapeGroup::addStrip(TCULong color, TREShapeType shapeType,
								   TCVector *vertices, TCVector *normals,
								   int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(htonl(color), vertices, normals, count);
	addShapeStripCount(shapeType, count);
	addShapeIndices(shapeType, index, count);
	return index;
}

void TREColoredShapeGroup::transferColoredTransparent(const float *matrix)
{
	if (m_indices)
	{
		int bit;

		for (bit = TRESFirst; (TREShapeType)bit <= TRESLast; bit = bit << 1)
		{
			TREShapeType shapeType = (TREShapeType)bit;
			transferColoredTransparent(shapeType, getIndices(shapeType),
				matrix);
		}
	}
}

void TREColoredShapeGroup::transferColoredTransparent(TREShapeType shapeType,
													  TCULongArray *indices,
													  const float *matrix)
{
	TCULongArray *colors = m_vertexStore->getColors();
	TREVertexArray *oldVertices = m_vertexStore->getVertices();
	TREVertexArray *oldNormals = m_vertexStore->getNormals();

	if (indices && colors && oldVertices && oldNormals)
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
						indices->removeValues(i, shapeSize);
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
							stripCount, matrix, true);
						break;
					case TRESQuadStrip:
						transferQuadStrip(shapeTypeIndex, color, offset,
							stripCount, matrix, true);
						break;
					case TRESTriangleFan:
						transferTriangleFan(shapeTypeIndex, color, offset,
							stripCount, matrix, true);
						break;
					default:
						break;
					}
					stripCounts->removeValue(i);
				}
			}
		}
	}
}
