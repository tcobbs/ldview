#include "TREColoredShapeGroup.h"

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <GL/gl.h>

TREColoredShapeGroup::TREColoredShapeGroup(void)
	:m_colors(NULL)
{
}

TREColoredShapeGroup::TREColoredShapeGroup(const TREColoredShapeGroup &other)
	:TREShapeGroup(other),
	m_colors((TCULongArrayArray *)TCObject::copy(other.m_colors))
{
}

TREColoredShapeGroup::~TREColoredShapeGroup(void)
{
}

void TREColoredShapeGroup::dealloc(void)
{
	TCObject::release(m_colors);
	TREShapeGroup::dealloc();
}

int TREColoredShapeGroup::addTriangle(TCULong color, Vector *vertices)
{
	int index = TREShapeGroup::addTriangle(vertices);
	addColor(TRESTriangle, htonl(color), 3);
	return index;
}

int TREColoredShapeGroup::addQuad(TCULong color, Vector *vertices)
{
	int index = TREShapeGroup::addQuad(vertices);
	addColor(TRESQuad, htonl(color), 4);
	return index;
}

void TREColoredShapeGroup::addColor(TREShapeType shapeType, TCULong color,
									int count)
{
	int i;
	TCULongArray *colorArray = getColors(shapeType);

	for (i = 0; i < count; i++)
	{
		colorArray->addValue(color);
	}
}

void TREColoredShapeGroup::setup(void)
{
	TREShapeGroup::setup();
}

void TREColoredShapeGroup::addShapeType(TREShapeType shapeType, int index)
{
	TCULongArray *newULongArray = new TCULongArray;

	if (!m_colors)
	{
		m_colors = new TCULongArrayArray;
	}
	m_colors->insertObject(newULongArray, index);
	newULongArray->release();
	TREShapeGroup::addShapeType(shapeType, index);
}

void TREColoredShapeGroup::drawShapeType(TREShapeType shapeType)
{
	TCULongArray *colorArray = getColors(shapeType);

	if (colorArray)
	{
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorArray->getValues());
		glEnableClientState(GL_COLOR_ARRAY);
		TREShapeGroup::drawShapeType(shapeType);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	else
	{
		TREShapeGroup::drawShapeType(shapeType);
	}
}

TCULongArray *TREColoredShapeGroup::getColors(TREShapeType shapeType)
{
	if (!(m_shapesPresent & shapeType))
	{
		return NULL;
	}
	else
	{
		TCULong index = getIndex(shapeType);

		return m_colors->objectAtIndex(index);
	}
}

