#include "TRESubModel.h"
#include "TREModel.h"
#include "TREGL.h"

#include <string.h>

TRESubModel::TRESubModel(void)
	:m_model(NULL),
	m_color(0),
	m_colorSet(false)
{
}

TRESubModel::TRESubModel(const TRESubModel &other)
	:TCObject(other),
	m_model((TREModel *)TCObject::copy(other.m_model)),
	m_color(other.m_color),
	m_colorSet(other.m_colorSet)
{
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

TRESubModel::~TRESubModel(void)
{
}

void TRESubModel::dealloc(void)
{
	TCObject::release(m_model);
	TCObject::dealloc();
}

TCObject *TRESubModel::copy(void)
{
	return new TRESubModel(*this);
}

void TRESubModel::setModel(TREModel *model)
{
	model->retain();
	TCObject::release(m_model);
	m_model = model;
}

void TRESubModel::setMatrix(float *matrix)
{
	memcpy(m_matrix, matrix, sizeof(m_matrix));
}

void TRESubModel::setColor(TCULong color)
{
	m_color = htonl(color);
	m_colorSet = true;
}

TCULong TRESubModel::getColor(void)
{
	return htonl(m_color);
}

void TRESubModel::draw(void)
{
	if (m_colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->draw();
	glPopMatrix();
	if (m_colorSet)
	{
		glPopAttrib();
	}
}
