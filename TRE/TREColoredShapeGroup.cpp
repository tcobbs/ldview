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

int TREColoredShapeGroup::addTriangle(TCULong color, Vector *vertices)
{
	int index;

	m_vertexStore->setupColored();
	index = m_vertexStore->addVertices(htonl(color), vertices, 3);
	addShape(TRESTriangle, index, 3);
	return index;
}

int TREColoredShapeGroup::addQuad(TCULong color, Vector *vertices)
{
	int index;

	m_vertexStore->setupColored();
	index = m_vertexStore->addVertices(htonl(color), vertices, 4);
	addShape(TRESQuad, index, 4);
	return index;
}

int TREColoredShapeGroup::addQuadStrip(TCULong color, Vector *vertices,
									   Vector *normals, int count)
{
	return addStrip(color, TRESQuadStrip, vertices, normals, count);
}

int TREColoredShapeGroup::addTriangleFan(TCULong color, Vector *vertices,
										 Vector *normals, int count)
{
	return addStrip(color, TRESTriangleFan, vertices, normals, count);
}

int TREColoredShapeGroup::addStrip(TCULong color, TREShapeType shapeType,
								   Vector *vertices, Vector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(htonl(color), vertices, normals, count);
	addShape(shapeType, count, 1);
	addShape(shapeType, index, count);
	return index;
}

