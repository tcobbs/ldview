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

void TRESubModel::drawDefaultColor(void)
{
	if (m_colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawDefaultColor();
	glPopMatrix();
	if (m_colorSet)
	{
		glPopAttrib();
	}
}

void TRESubModel::drawColored(void)
{
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawColored();
	glPopMatrix();
}

void TRESubModel::getMinMax(Vector& min, Vector& max, float* matrix)
{
	float newMatrix[16];

	TREModel::multMatrix(matrix, m_matrix, newMatrix);
	m_model->getMinMax(min, max, newMatrix);
}

void TRESubModel::shrink(float amount)
{
	Vector min;
	Vector max;
	Vector delta;
	float scaleMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f};
	float tempMatrix[16];

	memcpy(tempMatrix, m_matrix, sizeof(tempMatrix));
	m_model->getMinMax(min, max);
	delta = max - min;
	if (delta[0] > amount)
	{
		scaleMatrix[0] = (delta[0] - amount) / delta[0];
	}
	if (delta[1] > amount)
	{
		scaleMatrix[5] = (delta[1] - amount) / delta[1];
	}
	if (delta[2] > amount)
	{
		scaleMatrix[10] = (delta[2] - amount) / delta[2];
	}
	TREModel::multMatrix(tempMatrix, scaleMatrix, m_matrix);
}
