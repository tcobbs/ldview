#include "TRESubModel.h"
#include "TREModel.h"
#include "TREGL.h"

#include <string.h>

TRESubModel::TRESubModel(void)
	:m_model(NULL),
	m_invertedSubModel(NULL),
	m_color(0),
	m_edgeColor(0)
{
	m_flags.colorSet = false;
	m_flags.inverted = false;
	m_flags.mirrorMatrix = false;
}

TRESubModel::TRESubModel(const TRESubModel &other)
	:m_model((TREModel *)TCObject::copy(other.m_model)),
	m_invertedSubModel((TRESubModel *)TCObject::copy(other.m_invertedSubModel)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_flags(other.m_flags)
{
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

TRESubModel::TRESubModel(const TRESubModel &other, bool shallow)
	:m_model(shallow ? NULL : (TREModel *)TCObject::copy(other.m_model)),
	m_invertedSubModel(shallow ? NULL :
		(TRESubModel *)TCObject::copy(other.m_invertedSubModel)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_flags(other.m_flags)
{
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

TRESubModel::~TRESubModel(void)
{
}

void TRESubModel::dealloc(void)
{
	TCObject::release(m_model);
	if (!m_flags.inverted)
	{
		TCObject::release(m_invertedSubModel);
	}
	TCObject::dealloc();
}

TCObject *TRESubModel::copy(void)
{
	return new TRESubModel(*this);
}

TRESubModel *TRESubModel::shallowCopy(void)
{
	return new TRESubModel(*this, true);
}

TRESubModel *TRESubModel::getInvertedSubModel(void)
{
	if (!m_invertedSubModel)
	{
		m_invertedSubModel = shallowCopy();
		m_invertedSubModel->invert(this);
	}
	return m_invertedSubModel;
}

void TRESubModel::invert(TRESubModel *originalSubModel)
{
	m_invertedSubModel = originalSubModel;
	m_flags.inverted = true;
	m_model = originalSubModel->m_model->getInvertedModel();
	m_model->retain();
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
	if (TCVector::determinant(m_matrix) < 0.0f)
	{
		m_flags.mirrorMatrix = true;
	}
	else
	{
		m_flags.mirrorMatrix = false;
	}
}

void TRESubModel::setColor(TCULong color, TCULong edgeColor)
{
	m_color = htonl(color);
	m_edgeColor = htonl(edgeColor);
	m_flags.colorSet = true;
}

TCULong TRESubModel::getColor(void)
{
	return htonl(m_color);
}

TCULong TRESubModel::getEdgeColor(void)
{
	return htonl(m_edgeColor);
}

/*
void TRESubModel::draw(void)
{
	if (m_flags.colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->draw();
	glPopMatrix();
	if (m_flags.colorSet)
	{
		glPopAttrib();
	}
}
*/

void TRESubModel::compileDefaultColor(void)
{
	if (m_flags.mirrorMatrix)
	{
		m_model->getInvertedModel()->compileDefaultColor();
	}
	else
	{
		m_model->compileDefaultColor();
	}
}

void TRESubModel::drawDefaultColor(void)
{
	if (m_flags.colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
//	TCVector::multMatrix(matrix, m_matrix, newMatrix);
//	if (TCVector::determinant(newMatrix) < 0.0f)
	if (m_flags.mirrorMatrix)
	{
		m_model->getInvertedModel()->drawDefaultColor();
	}
	else
	{
		m_model->drawDefaultColor();
	}
	glPopMatrix();
	if (m_flags.colorSet)
	{
		glPopAttrib();
	}
}

void TRESubModel::drawDefaultColorLines(void)
{
	if (m_flags.colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawDefaultColorLines();
	glPopMatrix();
	if (m_flags.colorSet)
	{
		glPopAttrib();
	}
}

void TRESubModel::drawEdgeLines(void)
{
	if (m_flags.colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_edgeColor);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawEdgeLines();
	glPopMatrix();
	if (m_flags.colorSet)
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

void TRESubModel::drawColoredLines(void)
{
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawColoredLines();
	glPopMatrix();
}

void TRESubModel::drawColoredEdgeLines(void)
{
	glPushMatrix();
	glMultMatrixf(m_matrix);
	m_model->drawColoredEdgeLines();
	glPopMatrix();
}

void TRESubModel::scanPoints(TCObject *scanner,
							 TREScanPointCallback scanPointCallback,
							 float *matrix)
{
	float newMatrix[16];

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	m_model->scanPoints(scanner, scanPointCallback, newMatrix);
}

void TRESubModel::unshrinkNormals(float *matrix, float *unshrinkMatrix)
{
	float newMatrix[16];

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	m_model->unshrinkNormals(newMatrix, unshrinkMatrix);
}

void TRESubModel::shrink(float amount)
{
	TCVector boundingMin;
	TCVector boundingMax;
	TCVector delta;
	float scaleMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f};
	float tempMatrix[16];

	memcpy(tempMatrix, m_matrix, sizeof(tempMatrix));
	m_model->getBoundingBox(boundingMin, boundingMax);
	delta = boundingMax - boundingMin;
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
	TCVector::multMatrix(tempMatrix, scaleMatrix, m_matrix);
	m_model->unshrinkNormals(scaleMatrix);
}
