#include "TREModel.h"
#include "TRESubModel.h"
#include "TREMainModel.h"
#include "TREShapeGroup.h"
#include "TREColoredShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREGL.h"

#include <TCFoundation/mystring.h>
#include <LDLoader/LDLMacros.h>		// TODO: remove dependency
#include <LDLib/Vector.h>			// TODO: remove dependency

bool TREModel::sm_normalizeOn = false;

TREModel::TREModel(void)
	:m_name(NULL),
	m_mainModel(NULL),
	m_subModels(NULL),
	m_shapes(NULL),
	m_coloredShapes(NULL)
{
	m_flags.part = false;
}

TREModel::TREModel(const TREModel &other)
	:TCObject(other),
	m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_shapes((TREShapeGroup *)TCObject::copy(other.m_shapes)),
	m_coloredShapes((TREColoredShapeGroup *)TCObject::copy(
		other.m_coloredShapes)),
	m_flags(other.m_flags)
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

void TREModel::draw(void)
{
	if (m_flags.part)
	{
		glNormalize(false);
	}
	else
	{
		glNormalize(true);
	}
	if (m_shapes)
	{
		m_shapes->draw();
	}
	if (m_coloredShapes)
	{
		glPushAttrib(GL_CURRENT_BIT);
		m_coloredShapes->draw();
		glPopAttrib();
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
		m_shapes->setVertexStore(m_mainModel->getVertexStore());
	}
}

void TREModel::setupColored(void)
{
	if (!m_coloredShapes)
	{
		m_coloredShapes = new TREColoredShapeGroup;
		m_coloredShapes->setVertexStore(m_mainModel->getColoredVertexStore());
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

void TREModel::flatten(void)
{
	float identityMatrix[16] = {1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f};

	flatten(this, identityMatrix, 0, false, false);
	if (m_subModels)
	{
		m_subModels->removeAll();
	}
}

void TREModel::flatten(TREModel *model, float *matrix, TCULong color,
					   bool colorSet, bool includeShapes)
{
	TRESubModelArray *subModels = model->m_subModels;

	if (includeShapes)
	{
		if (model->m_shapes)
		{
			if (colorSet)
			{
				setupColored();
				m_coloredShapes->getVertexStore()->setupColored();
				flattenShapes(m_coloredShapes, model->m_shapes, matrix, color,
					true);
			}
			else
			{
				setup();
				flattenShapes(m_shapes, model->m_shapes, matrix, color, false);
			}
		}
		if (model->m_coloredShapes)
		{
			setupColored();
			flattenShapes(m_coloredShapes, model->m_coloredShapes, matrix,
				0, false);
		}
	}
	if (subModels)
	{
		int i;
		int count = subModels->getCount();
		float newMatrix[16];

		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel = (*subModels)[i];

			multMatrix(matrix, subModel->getMatrix(), newMatrix);
			if (subModel->isColorSet())
			{
				flatten(subModel->getModel(), newMatrix,
					htonl(subModel->getColor()), true, true);
			}
			else
			{
				flatten(subModel->getModel(), newMatrix, color, colorSet, true);
			}
		}
	}
}

void TREModel::flattenShapes(TREShapeGroup *dstShapes, TREShapeGroup *srcShapes,
							 float *matrix, TCULong color, bool colorSet)
{
	TREVertexStore *srcVertexStore = NULL;
	TREVertexStore *dstVertexStore = NULL;

	if (srcShapes && (srcVertexStore = srcShapes->getVertexStore()) != NULL &&
		dstShapes && (dstVertexStore = dstShapes->getVertexStore()) != NULL)
	{
		TCULong bit;

		for (bit = TRESFirst; bit <= TRESLast; bit = bit << 1)
		{
			TCULongArray *srcIndices = srcShapes->getIndices((TREShapeType)bit);

			if (srcIndices)
			{
				int i;
				int count = srcIndices->getCount();
				TREVertexArray *srcVertices = srcVertexStore->getVertices();
				TREVertexArray *srcNormals = srcVertexStore->getNormals();
				TCULongArray *srcColors = srcVertexStore->getColors();
				TCULongArray *dstIndices =
					dstShapes->getIndices((TREShapeType)bit, true);

				if (srcVertices)
				{
					TREVertexArray *dstVertices = dstVertexStore->getVertices();
					TREVertexArray *dstNormals = dstVertexStore->getNormals();
					TCULongArray *dstColors = dstVertexStore->getColors();
					int shapeSize =
						TREShapeGroup::numPointsForShapeType((TREShapeType)bit);
					Vector normal;
					Vector points[3];
					TREVertex normalVertex;

					for (i = 0; i < count; i++)
					{
						int index = (*srcIndices)[i];
						TREVertex vertex = (*srcVertices)[index];

						dstIndices->addValue(dstVertices->getCount());
						transformVertex(vertex, matrix);
						dstVertices->addVertex(vertex);
						if (shapeSize > 2)
						{
							if (i % shapeSize == 0)
							{
								int j;

								for (j = 0; j < 3; j++)
								{
									TREVertex v =
										(*srcVertices)[(*srcIndices)[i + j]];

									transformVertex(v, matrix);
									memcpy((float *)points[j], v.v,
										sizeof(v.v));
								}
								normal = TREVertexStore::calcNormal(points);
								TREVertexStore::initVertex(normalVertex,
									normal);
							}
							dstNormals->addVertex(normalVertex);
						}
						else if (srcNormals)
						{
							TREVertex normal = (*srcNormals)[index];

							transformNormal(normal, matrix);
							dstNormals->addVertex(normal);
						}
						if (colorSet)
						{
							dstColors->addValue(color);
						}
						else if (srcColors)
						{
							dstColors->addValue((*srcColors)[index]);
						}
					}
				}
			}
		}
	}
}

void TREModel::transformVertex(TREVertex &vertex, float *matrix)
{
	Vector newVertex;
	float x = vertex.v[0];
	float y = vertex.v[1];
	float z = vertex.v[2];

//	x' = a*x + b*y + c*z + X
//	y' = d*x + e*y + f*z + Y
//	z' = g*x + h*y + i*z + Z
	newVertex[0] = matrix[0]*x + matrix[4]*y + matrix[8]*z + matrix[12];
	newVertex[1] = matrix[1]*x + matrix[5]*y + matrix[9]*z + matrix[13];
	newVertex[2] = matrix[2]*x + matrix[6]*y + matrix[10]*z + matrix[14];
	TREVertexStore::initVertex(vertex, newVertex);
}

void TREModel::transformNormal(TREVertex &normal, float *matrix)
{
	Vector newNormal;
	float inverseMatrix[16];
	float x = normal.v[0];
	float y = normal.v[1];
	float z = normal.v[2];

	invertMatrix(matrix, inverseMatrix);
//	x' = a*x + b*y + c*z + X
//	y' = d*x + e*y + f*z + Y
//	z' = g*x + h*y + i*z + Z
	newNormal[0] = inverseMatrix[0]*x + inverseMatrix[1]*y +
		inverseMatrix[2]*z;
	newNormal[1] = inverseMatrix[4]*x + inverseMatrix[5]*y + inverseMatrix[6]*z;
	newNormal[2] = inverseMatrix[8]*x + inverseMatrix[9]*y +
		inverseMatrix[10]*z;
	newNormal.normalize();
	TREVertexStore::initVertex(normal, newNormal);
}

float TREModel::determinant(float* matrix)
{
	float det;

	det = matrix[0] * (matrix[5] * matrix[10] - matrix[6] * matrix[9]);
	det -= matrix[4] * (matrix[1] * matrix[10] - matrix[2] * matrix[9]);
	det += matrix[8] * (matrix[1] * matrix[6] - matrix[2] * matrix[5]);
	return det;
}

void TREModel::invertMatrix(float* matrix, float* inverseMatrix)
{
	float det = determinant(matrix);

	if (fEq(det, 0.0f))
	{
		debugPrintf("Error: Matrix inversion failed.\n");
	}
	else
	{
		det = 1.0f / det;

		inverseMatrix[0]  =  (matrix[5] * matrix[10] - matrix[6] * matrix[9]) *
			det;
		inverseMatrix[1]  = -(matrix[1] * matrix[10] - matrix[2] * matrix[9]) *
			det;
		inverseMatrix[2]  =  (matrix[1] * matrix[6]  - matrix[2] * matrix[5]) *
			det;
		inverseMatrix[4]  = -(matrix[4] * matrix[10] - matrix[6] * matrix[8]) *
			det;
		inverseMatrix[5]  =  (matrix[0] * matrix[10] - matrix[2] * matrix[8]) *
			det;
		inverseMatrix[6]  = -(matrix[0] * matrix[6]  - matrix[2] * matrix[4]) *
			det;
		inverseMatrix[8]  =  (matrix[4] * matrix[9]  - matrix[5] * matrix[8]) *
			det;
		inverseMatrix[9]  = -(matrix[0] * matrix[9]  - matrix[1] * matrix[8]) *
			det;
		inverseMatrix[10] =  (matrix[0] * matrix[5]  - matrix[1] * matrix[4]) *
			det;

		inverseMatrix[12] = -(matrix[12] * matrix[0] + matrix[13] * matrix[4] +
							matrix[14] * matrix[8]);
		inverseMatrix[13] = -(matrix[12] * matrix[1] + matrix[13] * matrix[5] +
							matrix[14] * matrix[9]);
		inverseMatrix[14] = -(matrix[12] * matrix[2] + matrix[13] * matrix[6] +
							matrix[14] * matrix[10]);

		inverseMatrix[3] = inverseMatrix[7] = inverseMatrix[11] = 0.0;
		inverseMatrix[15] = 1.0;
	}
}

void TREModel::multMatrix(float* left, float* right, float* result)
{
	result[0] = left[0] * right[0] + left[4] * right[1] +
		left[8] * right[2] + left[12] * right[3];
	result[1] = left[1] * right[0] + left[5] * right[1] +
		left[9] * right[2] + left[13] * right[3];
	result[2] = left[2] * right[0] + left[6] * right[1] +
		left[10] * right[2] + left[14] * right[3];
	result[3] = left[3] * right[0] + left[7] * right[1] +
		left[11] * right[2] + left[15] * right[3];
	result[4] = left[0] * right[4] + left[4] * right[5] +
		left[8] * right[6] + left[12] * right[7];
	result[5] = left[1] * right[4] + left[5] * right[5] +
		left[9] * right[6] + left[13] * right[7];
	result[6] = left[2] * right[4] + left[6] * right[5] +
		left[10] * right[6] + left[14] * right[7];
	result[7] = left[3] * right[4] + left[7] * right[5] +
		left[11] * right[6] + left[15] * right[7];
	result[8] = left[0] * right[8] + left[4] * right[9] +
		left[8] * right[10] + left[12] * right[11];
	result[9] = left[1] * right[8] + left[5] * right[9] +
		left[9] * right[10] + left[13] * right[11];
	result[10] = left[2] * right[8] + left[6] * right[9] +
		left[10] * right[10] + left[14] * right[11];
	result[11] = left[3] * right[8] + left[7] * right[9] +
		left[11] * right[10] + left[15] * right[11];
	result[12] = left[0] * right[12] + left[4] * right[13] +
		left[8] * right[14] + left[12] * right[15];
	result[13] = left[1] * right[12] + left[5] * right[13] +
		left[9] * right[14] + left[13] * right[15];
	result[14] = left[2] * right[12] + left[6] * right[13] +
		left[10] * right[14] + left[14] * right[15];
	result[15] = left[3] * right[12] + left[7] * right[13] +
		left[11] * right[14] + left[15] * right[15];
}

void TREModel::glNormalize(bool value)
{
	if (value != sm_normalizeOn)
	{
		if (value)
		{
			glEnable(GL_NORMALIZE);
			sm_normalizeOn = true;
		}
		else
		{
			glDisable(GL_NORMALIZE);
			sm_normalizeOn = false;
		}
	}
}
