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

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   TCVector *vertices, int count)
{
	return addShape(shapeType, color, vertices, NULL, count);
}

int TREColoredShapeGroup::addShape(TREShapeType shapeType, TCULong color,
								   TCVector *vertices, TCVector *normals,
								   int count)
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

int TREColoredShapeGroup::addTriangle(TCULong color, TCVector *vertices,
									  TCVector *normals)
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

