#include "TREModel.h"
#include "TRESubModel.h"
#include "TREMainModel.h"
#include "TREShapeGroup.h"
#include "TREColoredShapeGroup.h"

#include <TCFoundation/mystring.h>
#include <LDLib/Vector.h>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <GL/gl.h>

TREModel::TREModel(void)
	:m_name(NULL),
	m_mainModel(NULL),
	m_subModels(NULL),
	m_shapes(NULL),
	m_coloredShapes(NULL)
{
}

TREModel::TREModel(const TREModel &other)
	:TCObject(other),
	m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_shapes((TREShapeGroup *)TCObject::copy(other.m_shapes)),
	m_coloredShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredShapes))
{
}

TREModel::~TREModel(void)
{
}

void TREModel::dealloc(void)
{
	delete m_name;
	// Don't release m_mainModel
	TCObject::release(m_subModels);
	TCObject::release(m_shapes);
	TCObject::release(m_coloredShapes);
	TCObject::dealloc();
}

TCObject *TREModel::copy(void)
{
	return new TREModel(*this);
}

void TREModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

void TREModel::draw(float * /*matrix*/)
{
	if (m_shapes)
	{
		m_shapes->draw();
	}
	if (m_coloredShapes)
	{
		m_coloredShapes->draw();
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->draw();
		}
	}
}

void TREModel::setup(void)
{
	if (!m_shapes)
	{
		m_shapes = new TREShapeGroup;
	}
}

void TREModel::setupColored(void)
{
	if (!m_coloredShapes)
	{
		m_coloredShapes = new TREColoredShapeGroup;
	}
}

void TREModel::addTriangle(TCULong color, Vector *vertices)
{
	setupColored();
	m_coloredShapes->addTriangle(color, vertices);
}

void TREModel::addTriangle(Vector *vertices)
{
	setup();
	m_shapes->addTriangle(vertices);
}

void TREModel::addQuad(TCULong color, Vector *vertices)
{
	setupColored();
	m_coloredShapes->addQuad(color, vertices);
}

void TREModel::addQuad(Vector *vertices)
{
	setup();
	m_shapes->addQuad(vertices);
}

void TREModel::addSubModel(float *matrix, TREModel *model)
{
	TRESubModel *subModel = new TRESubModel;

	if (!m_subModels)
	{
		m_subModels = new TRESubModelArray;
	}
	subModel->setMatrix(matrix);
	subModel->setModel(model);
	m_subModels->addObject(subModel);
	subModel->release();
}

void TREModel::addSubModel(TCULong color, float *matrix, TREModel *model)
{
	addSubModel(matrix, model);
	(*m_subModels)[m_subModels->getCount() - 1]->setColor(color);
}
