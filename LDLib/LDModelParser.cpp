#include "LDModelParser.h"
#include "Vector.h"

#include <string.h>

#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLShapeLine.h>
#include <LDLoader/LDLModelLine.h>
#include <TRE/TREMainModel.h>
#include <TRE/TREVertexArray.h>
#include <TRE/TREShapeGroup.h>

LDModelParser::LDModelParser(void)
	:m_mainLDLModel(NULL),
	m_mainTREModel(NULL)
{
	m_flags.flattenParts = true;
}

LDModelParser::~LDModelParser(void)
{
}

void LDModelParser::dealloc(void)
{
	TCObject::release(m_mainLDLModel);
	TCObject::release(m_mainTREModel);
	TCObject::dealloc();
}

bool LDModelParser::parseMainModel(LDLMainModel *mainLDLModel)
{
	m_mainLDLModel = (LDLMainModel *)mainLDLModel->retain();
	m_mainTREModel = new TREMainModel;
	m_mainTREModel->setPart(mainLDLModel->isPart());
	if (parseModel(m_mainLDLModel, m_mainTREModel))
	{
		if (m_flags.flattenParts && m_mainTREModel->isPart())
		{
			m_mainTREModel->flatten();
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool LDModelParser::addSubModel(LDLModelLine *modelLine, TREModel *treModel,
								TREModel *subModel)
{
	TCULong colorNumber = modelLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addSubModel(modelLine->getMatrix(), subModel);
	}
	else
	{
		treModel->addSubModel(
			modelLine->getParentModel()->getPackedRGBA(colorNumber),
			modelLine->getMatrix(), subModel);
	}
	if (m_flags.flattenParts && subModel->isPart() && !treModel->isPart())
	{
		subModel->flatten();
	}
	return true;
}

bool LDModelParser::parseModel(LDLModelLine *modelLine, TREModel *treModel)
{
	LDLModel *ldlModel = modelLine->getModel();

	if (ldlModel)
	{
		const char *name = ldlModel->getName();
		TREModel *model = m_mainTREModel->modelNamed(name);

		if (model)
		{
			return addSubModel(modelLine, treModel, model);
		}
		else
		{
			model = new TREModel;
			model->setMainModel(treModel->getMainModel());
			model->setName(name);
			model->setPart(ldlModel->isPart());
			if (parseModel(ldlModel, model))
			{
				m_mainTREModel->registerModel(model);
				model->release();
				return addSubModel(modelLine, treModel, model);
			}
			else
			{
				model->release();
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

bool LDModelParser::parseModel(LDLModel *ldlModel, TREModel *treModel)
{
	if (ldlModel)
	{
		LDLFileLineArray *fileLines = ldlModel->getFileLines();

		if (fileLines)
		{
			int i;
			int count = ldlModel->getActiveLineCount();

			treModel->setName(ldlModel->getName());
			for (i = 0; i < count; i++)
			{
				LDLFileLine *fileLine = (*fileLines)[i];

				if (fileLine->isActionLine() && fileLine->isValid())
				{
					switch (fileLine->getLineType())
					{
					case LDLLineTypeModel:
						parseModel((LDLModelLine *)fileLine, treModel);
						break;
					case LDLLineTypeLine:
						break;
					case LDLLineTypeTriangle:
						parseTriangle((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeQuad:
						parseQuad((LDLShapeLine *)fileLine, treModel);
						break;
					case LDLLineTypeConditionalLine:
						break;
					}
				}
			}
		}
	}
	return true;
}

void LDModelParser::parseTriangle(LDLShapeLine *shapeLine, TREModel *treModel)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addTriangle(shapeLine->getPoints());
	}
	else
	{
		treModel->addTriangle(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

void LDModelParser::parseQuad(LDLShapeLine *shapeLine, TREModel *treModel)
{
	TCULong colorNumber = shapeLine->getColorNumber();

	if (colorNumber == 16)
	{
		treModel->addQuad(shapeLine->getPoints());
	}
	else
	{
		treModel->addQuad(shapeLine->getParentModel()->
			getPackedRGBA(colorNumber), shapeLine->getPoints());
	}
}

/*
int LDModelParser::parseShapeVertices(LDLShapeLine *shapeLine,
									   TREModel *treModel)
{
	Vector *points = shapeLine->getPoints();
	TREVertexArray *vertices = treModel->getVertices();
	int i;
	int count = shapeLine->getNumPoints();

	for (i = 0; i < count; i++)
	{
		TREVertex vertex;

		memcpy(vertex.v, (float *)points[i], sizeof(vertex.v));
		vertices->addVertex(vertex);
	}
	return vertices->getCount() - count;
}
*/