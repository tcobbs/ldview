#include "TRESubModel.h"
#include "TREModel.h"
#include "TREGL.h"
#include "TREMainModel.h"

#include <string.h>

TRESubModel::TRESubModel(void)
	:m_model(NULL),
	m_unMirroredSubModel(NULL),
	m_invertedSubModel(NULL),
	m_color(0),
	m_edgeColor(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TRESubModel");
#endif // _LEAK_DEBUG
	m_flags.colorSet = false;
	m_flags.unMirrored = false;
	m_flags.mirrorMatrix = false;
	m_flags.bfcInvert = false;
	m_flags.inverted = false;
}

TRESubModel::TRESubModel(const TRESubModel &other)
	:m_model((TREModel *)TCObject::copy(other.m_model)),
	m_unMirroredSubModel((TRESubModel *)TCObject::copy(
		other.m_unMirroredSubModel)),
	m_invertedSubModel((TRESubModel *)TCObject::copy(
		other.m_invertedSubModel)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TRESubModel");
#endif // _LEAK_DEBUG
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

TRESubModel::TRESubModel(const TRESubModel &other, bool shallow)
	:m_model(shallow ? NULL : (TREModel *)TCObject::copy(other.m_model)),
	m_unMirroredSubModel(shallow ? NULL :
		(TRESubModel *)TCObject::copy(other.m_unMirroredSubModel)),
	m_invertedSubModel(shallow ? NULL :
		(TRESubModel *)TCObject::copy(other.m_invertedSubModel)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TRESubModel");
#endif // _LEAK_DEBUG
	memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
}

TRESubModel::~TRESubModel(void)
{
}

void TRESubModel::dealloc(void)
{
	TCObject::release(m_model);
	if (m_unMirroredSubModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_unMirroredSubModel->m_unMirroredSubModel = NULL;
	}
	if (m_invertedSubModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_invertedSubModel->m_invertedSubModel = NULL;
	}
	if (!m_flags.unMirrored)
	{
		TCObject::release(m_unMirroredSubModel);
	}
	m_unMirroredSubModel = NULL;
	if (!m_flags.inverted)
	{
		TCObject::release(m_invertedSubModel);
	}
	m_invertedSubModel = NULL;
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

TRESubModel *TRESubModel::getUnMirroredSubModel(void)
{
	if (!m_unMirroredSubModel)
	{
		m_unMirroredSubModel = shallowCopy();
		m_unMirroredSubModel->unMirror(this);
	}
	return m_unMirroredSubModel;
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

void TRESubModel::unMirror(TRESubModel *originalSubModel)
{
	m_unMirroredSubModel = originalSubModel;
	m_flags.unMirrored = !originalSubModel->m_flags.unMirrored;
	if (m_unMirroredSubModel->m_invertedSubModel)
	{
		m_invertedSubModel =
			m_unMirroredSubModel->m_invertedSubModel->m_unMirroredSubModel;
		if (m_invertedSubModel)
		{
			m_invertedSubModel->m_invertedSubModel = this;
		}
	}
	m_model = originalSubModel->m_model->getUnMirroredModel();
	m_model->retain();
}

void TRESubModel::invert(TRESubModel *originalSubModel)
{
	m_invertedSubModel = originalSubModel;
	m_flags.inverted = !originalSubModel->m_flags.inverted;
	if (m_invertedSubModel->m_unMirroredSubModel)
	{
		m_unMirroredSubModel =
			m_invertedSubModel->m_unMirroredSubModel->m_invertedSubModel;
		if (m_unMirroredSubModel)
		{
			m_unMirroredSubModel->m_unMirroredSubModel = this;
		}
	}
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

TREModel *TRESubModel::getEffectiveModel(void) const
{
	if (m_flags.mirrorMatrix)
	{
		if (m_flags.bfcInvert)
		{
			return m_model->getUnMirroredModel()->getInvertedModel();
		}
		else
		{
			return m_model->getUnMirroredModel();
		}
	}
	else if (m_flags.bfcInvert)
	{
		return m_model->getInvertedModel();
	}
	else
	{
		return m_model;
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

void TRESubModel::draw(TREMSection section, bool colored)
{
	if (!colored)
	{
		// I know this looks backwards.  However, the colored geometry has the
		// color set for each primitive.  Consequently, it doesn't need the
		// OpenGL color to be set.
		if (m_flags.colorSet)
		{
			if (section == TREMEdgeLines || section == TREMConditionalLines)
			{
				glPushAttrib(GL_CURRENT_BIT);
				glColor4ubv((GLubyte*)&m_edgeColor);
			}
			else
			{
				if (section != TREMLines)
				{
					if (TREShapeGroup::isTransparent(m_color, true))
					{
						// Don't draw transparent shapes here.
						return;
					}
				}
				glPushAttrib(GL_CURRENT_BIT);
				glColor4ubv((GLubyte*)&m_color);
			}
		}
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	getEffectiveModel()->draw(section, colored);
	glPopMatrix();
	if (!colored)
	{
		if (m_flags.colorSet)
		{
			glPopAttrib();
		}
	}
}

/*
void TRESubModel::compileDefaultColor(void)
{
	getEffectiveModel()->compileDefaultColor();
}

void TRESubModel::compileColored(void)
{
	getEffectiveModel()->compileColored();
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
	getEffectiveModel()->drawDefaultColor();
	glPopMatrix();
	if (m_flags.colorSet)
	{
		glPopAttrib();
	}
}

void TRESubModel::drawBFC(void)
{
	if (m_flags.colorSet)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor4ubv((GLubyte*)&m_color);
	}
	glPushMatrix();
	glMultMatrixf(m_matrix);
	getEffectiveModel()->drawBFC();
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
	getEffectiveModel()->drawColored();
	glPopMatrix();
}

void TRESubModel::drawColoredBFC(void)
{
	glPushMatrix();
	glMultMatrixf(m_matrix);
	getEffectiveModel()->drawColoredBFC();
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
*/

void TRESubModel::scanPoints(TCObject *scanner,
							 TREScanPointCallback scanPointCallback,
							 float *matrix)
{
	float newMatrix[16];

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	getEffectiveModel()->scanPoints(scanner, scanPointCallback, newMatrix);
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

void TRESubModel::transferColoredTransparent(TREMSection section,
											 const float *matrix)
{
	float newMatrix[16];

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	m_model->transferColoredTransparent(section, newMatrix);
}

void TRESubModel::transferTransparent(TCULong color, TREMSection section,
									  const float *matrix)
{
	float newMatrix[16];

	TCVector::multMatrix(matrix, m_matrix, newMatrix);
	if (m_flags.colorSet)
	{
		color = m_color;
	}
	m_model->transferTransparent(color, section, newMatrix);
}
