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
	addShape(TRESTriangle, index, index + 1, index + 2);
	return index;
}

int TREColoredShapeGroup::addQuad(TCULong color, Vector *vertices)
{
	int index;

	m_vertexStore->setupColored();
	index = m_vertexStore->addVertices(htonl(color), vertices, 4);
	addShape(TRESQuad, index, index + 1, index + 2, index + 3);
	return index;
}
