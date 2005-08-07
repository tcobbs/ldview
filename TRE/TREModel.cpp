#include "TREModel.h"
#include "TRESubModel.h"
#include "TREMainModel.h"
#include "TREShapeGroup.h"
#include "TREColoredShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREGL.h"

#include <TCFoundation/mystring.h>
#include <TCFoundation/TCMacros.h>

bool TREModel::sm_normalizeOn = false;

TREModel::TREModel(void)
	:m_name(NULL),
	m_mainModel(NULL),
	m_subModels(NULL),
	m_unMirroredModel(NULL),
	m_invertedModel(NULL),
	m_sectionsPresent(0),
	m_coloredSectionsPresent(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = NULL;
		m_coloredShapes[i] = NULL;
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
	m_flags.part = false;
	m_flags.boundingBox = false;
	m_flags.unshrunkNormals = false;
	m_flags.unMirrored = false;
	m_flags.inverted = false;
	m_flags.flattened = false;
}

TREModel::TREModel(const TREModel &other)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels((TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_unMirroredModel((TREModel *)TCObject::copy(other.m_unMirroredModel)),
	m_invertedModel((TREModel *)TCObject::copy(other.m_invertedModel)),
	m_sectionsPresent(other.m_sectionsPresent),
	m_coloredSectionsPresent(other.m_coloredSectionsPresent),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = (TREShapeGroup *)TCObject::copy(other.m_shapes[i]);
		m_coloredShapes[i] =
			(TREColoredShapeGroup *)TCObject::copy(other.m_coloredShapes[i]);
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
}

TREModel::TREModel(const TREModel &other, bool shallow)
	:m_name(copyString(other.m_name)),
	m_mainModel(other.m_mainModel),
	m_subModels(shallow ? NULL :
		(TRESubModelArray *)TCObject::copy(other.m_subModels)),
	m_unMirroredModel(shallow ? NULL :
		(TREModel *)TCObject::copy(other.m_unMirroredModel)),
	m_invertedModel(shallow ? NULL :
		(TREModel *)TCObject::copy(other.m_invertedModel)),
	m_sectionsPresent(other.m_sectionsPresent),
	m_coloredSectionsPresent(other.m_coloredSectionsPresent),
	m_boundingMin(other.m_boundingMin),
	m_boundingMax(other.m_boundingMax),
	m_flags(other.m_flags)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREModel");
#endif // _LEAK_DEBUG
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		m_shapes[i] = (TREShapeGroup *)TCObject::copy(other.m_shapes[i]);
		m_coloredShapes[i] =
			(TREColoredShapeGroup *)TCObject::copy(other.m_coloredShapes[i]);
		m_listIDs[i] = 0;
		m_coloredListIDs[i] = 0;
	}
}

TREModel::~TREModel(void)
{
}

void TREModel::dealloc(void)
{
	int i;

	delete m_name;
	// Don't release m_mainModel
	TCObject::release(m_subModels);
	if (m_unMirroredModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_unMirroredModel->m_unMirroredModel = NULL;
	}
	if (m_invertedModel)
	{
		// The following points back to us, and since we're being deallocated
		// right now, we don't want it to deallocate us.
		m_invertedModel->m_invertedModel = NULL;
	}
	if (!m_flags.unMirrored)
	{
		TCObject::release(m_unMirroredModel);
	}
	m_unMirroredModel = NULL;
	if (!m_flags.inverted)
	{
		TCObject::release(m_invertedModel);
	}
	m_invertedModel = NULL;
	for (i = 0; i <= TREMLast; i++)
	{
		TCULong listID = m_listIDs[i];

		if (listID)
		{
			glDeleteLists(listID, 1);
		}
		listID = m_coloredListIDs[i];
		if (listID)
		{
			glDeleteLists(listID, 1);
		}
		TCObject::release(m_shapes[i]);
		TCObject::release(m_coloredShapes[i]);
	}
	TCObject::dealloc();
}

TCObject *TREModel::copy(void)
{
	return new TREModel(*this);
}

TREModel *TREModel::shallowCopy(void)
{
	return new TREModel(*this, true);
}

TREModel *TREModel::getUnMirroredModel(void)
{
	if (!m_unMirroredModel)
	{
		m_unMirroredModel = shallowCopy();
		m_unMirroredModel->unMirror(this);
	}
	return m_unMirroredModel;
}

TREModel *TREModel::getInvertedModel(void)
{
	if (!m_invertedModel)
	{
		m_invertedModel = shallowCopy();
		m_invertedModel->invert(this);
	}
	return m_invertedModel;
}

void TREModel::unMirror(TREModel *originalModel)
{
	int i;

	m_unMirroredModel = originalModel;
	m_flags.unMirrored = true;
	if (m_unMirroredModel->m_invertedModel)
	{
		m_invertedModel = m_unMirroredModel->m_invertedModel->m_unMirroredModel;
		if (m_invertedModel)
		{
			m_invertedModel->m_invertedModel = this;
		}
	}
	if (originalModel->m_subModels)
	{
		int i;
		int count;

		count = originalModel->m_subModels->getCount();
		m_subModels = new TRESubModelArray(count);
		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel =
				(*originalModel->m_subModels)[i]->getUnMirroredSubModel();

			m_subModels->addObject(subModel);
		}
	}
	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];
			TREColoredShapeGroup *coloredShapeGroup = m_coloredShapes[i];

			if (shapeGroup)
			{
				shapeGroup->unMirror();
			}
			if (coloredShapeGroup)
			{
				coloredShapeGroup->unMirror();
			}
		}
	}
}

void TREModel::invert(TREModel *originalModel)
{
	int i;

	m_invertedModel = originalModel;
	m_flags.inverted = true;
	if (m_invertedModel->m_unMirroredModel)
	{
		m_unMirroredModel = m_invertedModel->m_unMirroredModel->m_invertedModel;
		if (m_unMirroredModel)
		{
			m_unMirroredModel->m_unMirroredModel = this;
		}
	}
	if (originalModel->m_subModels)
	{
		int i;
		int count;

		count = originalModel->m_subModels->getCount();
		m_subModels = new TRESubModelArray(count);
		for (i = 0; i < count; i++)
		{
			TRESubModel *subModel =
				(*originalModel->m_subModels)[i]->getInvertedSubModel();

			m_subModels->addObject(subModel);
		}
	}
	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];
			TREColoredShapeGroup *coloredShapeGroup = m_coloredShapes[i];

			if (shapeGroup)
			{
				shapeGroup->invert();
			}
			if (coloredShapeGroup)
			{
				coloredShapeGroup->invert();
			}
		}
	}
}

void TREModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

void TREModel::compile(TREMSection section, bool colored, bool nonUniform)
{
	int *listIDs;

	if (colored)
	{
		listIDs = m_coloredListIDs;
	}
	else
	{
		listIDs = m_listIDs;
	}
	if (!listIDs[section] && isSectionPresent(section, colored))
	{
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				TRESubModel *subModel = (*m_subModels)[i];
				subModel->getEffectiveModel()->compile(section, colored,
					nonUniform | subModel->getNonUniformFlag());
			}
		}
		if (m_mainModel->getCompileAllFlag() ||
			(m_flags.part && m_mainModel->getCompilePartsFlag()))
		{
			int listID = glGenLists(1);

			glNewList(listID, GL_COMPILE);
			draw(section, colored, false, nonUniform);
			glEndList();
			listIDs[section] = listID;
//			debugPrintf("U Standard <<%s>> %d %d\n", m_name, m_flags.unMirrored,
//				m_flags.inverted);
		}
	}
}

void TREModel::draw(TREMSection section)
{
	draw(section, false);
}

#ifdef _DEBUG
void TREModel::checkGLError(char *msg)
#else // _DEBUG
void TREModel::checkGLError(char *)
#endif // _DEBUG
{
	GLenum errorCode;
	
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		const char* errorString;// = interpretGLError(errorCode);

		switch (errorCode)
		{
			case GL_INVALID_ENUM:
				errorString = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				errorString = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				errorString = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				errorString = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				errorString = "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				errorString = "GL_OUT_OF_MEMORY";
				break;
			default:
				errorString = "Unknown Error";
				break;
		}
#ifdef _DEBUG
		debugPrintf("%s: %s\n", msg, errorString);
#endif // _DEBUG
//		reportError("OpenGL error:\n%s: %s\n", LDMEOpenGL, msg, errorString);
	}
}

void TREModel::draw(TREMSection section, bool colored, bool subModelsOnly,
					bool nonUniform)
{
	TCULong listID;

	if (colored)
	{
		listID = m_coloredListIDs[section];
	}
	else
	{
		listID = m_listIDs[section];
	}
	if (listID && !subModelsOnly)
	{
		// Note that subModelsOnly gets set when the current color is
		// transparent.  In that case, we don't want to draw our geometry,
		// because transparent geometry gets drawn elsewhere.  However, we do
		// want to draw any sub-models, because some of them not be
		// transparent.
//		checkGLError("Before glCallList");
		glCallList(listID);
//		checkGLError("After glCallList");
	}
	else if (isSectionPresent(section, colored))
	{
		if (!subModelsOnly)
		{
			TREShapeGroup *shapeGroup;

			if (colored)
			{
				shapeGroup = m_coloredShapes[section];
			}
			else
			{
				shapeGroup = m_shapes[section];
			}
			if (isLineSection(section))
			{
				if (shapeGroup)
				{
					shapeGroup->drawLines();
				}
			}
			else if (section == TREMConditionalLines)
			{
				if (shapeGroup)
				{
					shapeGroup->drawConditionalLines();
				}
			}
			else
			{
/*
				if (m_flags.part && isFlattened())
				{
					setGlNormalize(false | nonUniform);
				}
				else
				{
					setGlNormalize(true);
				}
*/
				if (shapeGroup)
				{
					shapeGroup->draw();
				}
			}
		}
		if (m_subModels)
		{
			int i;
			int count = m_subModels->getCount();

			for (i = 0; i < count; i++)
			{
				(*m_subModels)[i]->draw(section, colored, subModelsOnly,
					nonUniform);
			}
		}
	}
}

void TREModel::drawColored(TREMSection section)
{
	draw(section, true);
}

void TREModel::setup(TREMSection section)
{
	if (!m_shapes[section])
	{
		TREShapeGroup *shapeGroup = new TREShapeGroup;

		shapeGroup->setMainModel(m_mainModel);
		if (section == TREMStud || section == TREMStudBFC)
		{
			shapeGroup->setVertexStore(m_mainModel->getStudVertexStore());
		}
		else
		{
			shapeGroup->setVertexStore(m_mainModel->getVertexStore());
		}
		// No need to release previous, since we determined it is NULL.
		m_shapes[section] = shapeGroup;
		// Don't release shapeGroup, becase m_shapes isn't a TCObjectArray.
	}
}

void TREModel::setupColored(TREMSection section)
{
	if (!m_coloredShapes[section])
	{
		TREColoredShapeGroup *shapeGroup = new TREColoredShapeGroup;

		shapeGroup->setMainModel(m_mainModel);
		if (section == TREMStud || section == TREMStudBFC)
		{
			shapeGroup->setVertexStore(
				m_mainModel->getColoredStudVertexStore());
		}
		else
		{
			shapeGroup->setVertexStore(m_mainModel->getColoredVertexStore());
		}
		// No need to release previous, since we determined it is NULL.
		m_coloredShapes[section] = shapeGroup;
		// Don't release shapeGroup, becase m_shapes isn't a TCObjectArray.
	}
}

void TREModel::setupStandard(void)
{
	setup(TREMStandard);
}

void TREModel::setupStud(void)
{
	setup(TREMStud);
}

void TREModel::setupStudBFC(void)
{
	setup(TREMStudBFC);
}

void TREModel::setupLines(void)
{
	setup(TREMLines);
}

void TREModel::setupBFC(void)
{
	setup(TREMBFC);
}

void TREModel::setupEdges(void)
{
	setup(TREMEdgeLines);
}

void TREModel::setupConditional(void)
{
	setup(TREMConditionalLines);
}

void TREModel::setupColored(void)
{
	setupColored(TREMStandard);
}

void TREModel::setupColoredStud(void)
{
	setupColored(TREMStud);
}

void TREModel::setupColoredStudBFC(void)
{
	setupColored(TREMStudBFC);
}

void TREModel::setupColoredLines(void)
{
	setupColored(TREMLines);
}

void TREModel::setupColoredBFC(void)
{
	setupColored(TREMBFC);
}

void TREModel::setupColoredEdges(void)
{
	setupColored(TREMEdgeLines);
}

void TREModel::setupColoredConditional(void)
{
	setupColored(TREMConditionalLines);
}

void TREModel::addLine(TCULong color, TCVector *vertices)
{
	setupColoredLines();
	m_coloredShapes[TREMLines]->addLine(color, vertices);
}

void TREModel::addLine(TCVector *vertices)
{
	setupLines();
	m_shapes[TREMLines]->addLine(vertices);
}

void TREModel::addConditionalLine(TCVector *vertices, TCVector *controlPoints)
{
	setup(TREMConditionalLines);
	m_shapes[TREMConditionalLines]->addConditionalLine(vertices, controlPoints);
}

void TREModel::addEdgeLine(TCVector *vertices)
{
	setupEdges();
	m_shapes[TREMEdgeLines]->addLine(vertices);
}

void TREModel::addTriangle(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes[TREMStandard]->addTriangle(color, vertices);
}

void TREModel::addTriangle(TCVector *vertices)
{
	setupStandard();
	m_shapes[TREMStandard]->addTriangle(vertices);
}

void TREModel::addBFCTriangle(TCULong color, TCVector *vertices)
{
	setupColoredBFC();
	m_coloredShapes[TREMBFC]->addTriangle(color, vertices);
}

void TREModel::addBFCTriangle(TCVector *vertices)
{
	setupBFC();
	m_shapes[TREMBFC]->addTriangle(vertices);
}

void TREModel::addTriangle(TCVector *vertices, TCVector *normals)
{
	setupStandard();
	m_shapes[TREMStandard]->addTriangle(vertices, normals);
}

void TREModel::addBFCTriangle(TCVector *vertices, TCVector *normals)
{
	setupBFC();
	m_shapes[TREMBFC]->addTriangle(vertices, normals);
}

void TREModel::addQuad(TCULong color, TCVector *vertices)
{
	setupColored();
	m_coloredShapes[TREMStandard]->addQuad(color, vertices);
}

void TREModel::addQuad(TCVector *vertices)
{
	setupStandard();
	m_shapes[TREMStandard]->addQuad(vertices);
}

void TREModel::addBFCQuad(TCVector *vertices)
{
	setupBFC();
	m_shapes[TREMBFC]->addQuad(vertices);
}

void TREModel::addBFCQuad(TCULong color, TCVector *vertices)
{
	setupColoredBFC();
	m_coloredShapes[TREMBFC]->addQuad(color, vertices);
}

void TREModel::triangleStripToTriangle(int index, TCVector *stripVertices,
									   TCVector *stripNormals,
									   TCVector *triangleVertices,
									   TCVector *triangleNormals)
{
	int ofs1 = 1;
	int ofs2 = 2;

	if (index % 2)
	{
		ofs1 = 2;
		ofs2 = 1;
	}
	triangleVertices[0] = stripVertices[index];
	triangleVertices[1] = stripVertices[index + ofs1];
	triangleVertices[2] = stripVertices[index + ofs2];
	triangleNormals[0] = stripNormals[index];
	triangleNormals[1] = stripNormals[index + ofs1];
	triangleNormals[2] = stripNormals[index + ofs2];
}

void TREModel::quadStripToQuad(int index, TCVector *stripVertices,
							   TCVector *stripNormals, TCVector *quadVertices,
							   TCVector *quadNormals)
{
	quadVertices[0] = stripVertices[index];
	quadVertices[1] = stripVertices[index + 1];
	quadVertices[2] = stripVertices[index + 3];
	quadVertices[3] = stripVertices[index + 2];
	quadNormals[0] = stripNormals[index];
	quadNormals[1] = stripNormals[index + 1];
	quadNormals[2] = stripNormals[index + 3];
	quadNormals[3] = stripNormals[index + 2];
}

void TREModel::addQuadStrip(TCVector *vertices, TCVector *normals, int count,
							bool flat)
{
	setupStandard();
	addQuadStrip(m_shapes[TREMStandard], vertices, normals, count, flat);
}

void TREModel::addTriangleStrip(TCVector *vertices, TCVector *normals,
								int count, bool flat)
{
	setupStandard();
	addTriangleStrip(m_shapes[TREMStandard], vertices, normals, count, flat);
}

void TREModel::addQuadStrip(TREShapeGroup *shapeGroup, TCVector *vertices,
							TCVector *normals, int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addQuadStrip(vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			shapeGroup->addQuad(quadVertices, quadNormals);
		}
	}
}

void TREModel::addBFCQuadStrip(TCVector *vertices, TCVector *normals, int count,
							   bool flat)
{
	setupBFC();
	addQuadStrip(m_shapes[TREMBFC], vertices, normals, count, flat);
}

void TREModel::addQuadStrip(TCULong color, TCVector *vertices,
							TCVector *normals, int count, bool flat)
{
	setupColored();
	addQuadStrip(m_coloredShapes[TREMStandard], color, vertices, normals,
		count, flat);
}

void TREModel::addTriangleStrip(TREShapeGroup *shapeGroup, TCVector *vertices,
								TCVector *normals, int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addTriangleStrip(vertices, normals, count);
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];

		for (i = 0; i < count - 2; i++)
		{
			triangleStripToTriangle(i, vertices, normals, triangleVertices,
				triangleNormals);
			shapeGroup->addTriangle(triangleVertices, triangleNormals);
		}
	}
}

/*
void TREModel::addTriangleStrip(TREColoredShapeGroup *shapeGroup, TCULong color,
								TCVector *vertices, TCVector *normals,
								int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
//		shapeGroup->addTriangleStrip(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];

		for (i = 0; i < count - 2; i++)
		{
			triangleStripToTriangle(i, vertices, normals, triangleVertices,
				triangleNormals);
			shapeGroup->addTriangle(color, triangleVertices, triangleNormals);
		}
	}
}
*/

void TREModel::addQuadStrip(TREColoredShapeGroup *shapeGroup, TCULong color,
							TCVector *vertices, TCVector *normals, int count,
							bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addQuadStrip(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector quadVertices[4];
		TCVector quadNormals[4];

		for (i = 0; i < count - 3; i += 2)
		{
			quadStripToQuad(i, vertices, normals, quadVertices, quadNormals);
			shapeGroup->addQuad(color, quadVertices, quadNormals);
		}
	}
}

void TREModel::addBFCQuadStrip(TCULong color, TCVector *vertices,
							   TCVector *normals, int count, bool flat)
{
	setupColoredBFC();
	addQuadStrip(m_coloredShapes[TREMBFC], color, vertices, normals, count,
		flat);
}

/*
void TREModel::addBFCTriangleStrip(TCULong color, TCVector *vertices,
								   TCVector *normals, int count, bool flat)
{
	setupColoredBFC();
	addTriangleStrip(m_coloredShapes[TREMBFC], color, vertices, normals, count,
		flat);
}
*/

void TREModel::addBFCTriangleStrip(TCVector *vertices, TCVector *normals,
								   int count, bool flat)
{
	setupBFC();
	addTriangleStrip(m_shapes[TREMBFC], vertices, normals, count, flat);
}

void TREModel::triangleFanToTriangle(int index, TCVector *stripVertices,
									 TCVector *stripNormals,
									 TCVector *stripTextureCoords,
									 TCVector *triangleVertices,
									 TCVector *triangleNormals,
									 TCVector *triangleTextureCoords)
{
	triangleVertices[0] = stripVertices[0];
	triangleVertices[1] = stripVertices[index];
	triangleVertices[2] = stripVertices[index + 1];
	triangleNormals[0] = stripNormals[0];
	triangleNormals[1] = stripNormals[index];
	triangleNormals[2] = stripNormals[index + 1];
	if (stripTextureCoords)
	{
		triangleTextureCoords[0] = stripTextureCoords[0];
		triangleTextureCoords[1] = stripTextureCoords[index];
		triangleTextureCoords[2] = stripTextureCoords[index + 1];
	}
}

void TREModel::addTriangleFan(TCVector *vertices, TCVector *normals,
							  TCVector *textureCoords, int count, bool flat)
{
	TREShapeGroup *shapeGroup;

	if (textureCoords)
	{
		setupStud();
		shapeGroup = m_shapes[TREMStud];
	}
	else
	{
		setupStandard();
		shapeGroup = m_shapes[TREMStandard];
	}
	addTriangleFan(shapeGroup, vertices, normals, textureCoords, count, flat);
}

void TREModel::addTriangleFan(TREShapeGroup *shapeGroup, TCVector *vertices,
							  TCVector *normals, TCVector *textureCoords,
							  int count, bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		if (textureCoords)
		{
			shapeGroup->addTriangleFan(vertices, normals, textureCoords, count);
		}
		else
		{
			shapeGroup->addTriangleFan(vertices, normals, count);
		}
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];
		TCVector triangleTextureCoords[3];

		for (i = 1; i < count - 1; i++)
		{
			triangleFanToTriangle(i, vertices, normals, textureCoords,
				triangleVertices, triangleNormals, triangleTextureCoords);
			if (textureCoords)
			{
				shapeGroup->addTriangle(triangleVertices, triangleNormals,
					triangleTextureCoords);
			}
			else
			{
				shapeGroup->addTriangle(triangleVertices, triangleNormals);
			}
		}
	}
}

void TREModel::addBFCTriangleFan(TCVector *vertices, TCVector *normals,
								 TCVector *textureCoords, int count, bool flat)
{
	TREShapeGroup *shapeGroup;

	if (textureCoords)
	{
		setupStudBFC();
		shapeGroup = m_shapes[TREMStudBFC];
	}
	else
	{
		setupBFC();
		shapeGroup = m_shapes[TREMBFC];
	}
	addTriangleFan(shapeGroup, vertices, normals, textureCoords, count, flat);
}

void TREModel::addTriangleFan(TCULong color, TCVector *vertices,
							  TCVector *normals, int count, bool flat)
{
	setupColored();
	addTriangleFan(m_coloredShapes[TREMStandard], color, vertices, normals,
		count, flat);
}

void TREModel::addTriangleFan(TREColoredShapeGroup *shapeGroup, TCULong color,
							  TCVector *vertices, TCVector *normals, int count,
							  bool flat)
{
	if (m_mainModel->getUseStripsFlag() && (!flat ||
		m_mainModel->getUseFlatStripsFlag()))
	{
		shapeGroup->addTriangleFan(color, vertices, normals, count);
	}
	else
	{
		int i;
		TCVector triangleVertices[3];
		TCVector triangleNormals[3];

		for (i = 1; i < count - 1; i++)
		{
			triangleFanToTriangle(i, vertices, normals, NULL, triangleVertices,
				triangleNormals, NULL);
			shapeGroup->addTriangle(color, triangleVertices, triangleNormals);
		}
	}
}

void TREModel::addBFCTriangleFan(TCULong color, TCVector *vertices,
								 TCVector *normals, int count, bool flat)
{
	setupColoredBFC();
	addTriangleFan(m_coloredShapes[TREMBFC], color, vertices, normals, count,
		flat);
}

TRESubModel *TREModel::addSubModel(float *matrix, TREModel *model, bool invert)
{
	TRESubModel *subModel = new TRESubModel;

	if (!m_subModels)
	{
		m_subModels = new TRESubModelArray;
	}
	subModel->setMatrix(matrix);
	subModel->setModel(model);
	subModel->setBFCInvertFlag(invert);
	m_subModels->addObject(subModel);
	subModel->release();
	return subModel;
}

TRESubModel *TREModel::addSubModel(TCULong color, TCULong edgeColor,
								   float *matrix, TREModel *model, bool invert)
{
	TRESubModel *subModel = addSubModel(matrix, model, invert);

	subModel->setColor(color, edgeColor);
	return subModel;
}

void TREModel::smooth(void)
{
	TREConditionalMap conditionalMap;
	TRENormalInfoArray *normalInfos = new TRENormalInfoArray;

	fillConditionalMap(conditionalMap);
	calcShapeNormals(conditionalMap, normalInfos, TRESTriangle);
	calcShapeNormals(conditionalMap, normalInfos, TRESQuad);
	finishShapeNormals(conditionalMap);
	applyShapeNormals(normalInfos);
	normalInfos->release();
}

void TREModel::finishShapeNormals(TREConditionalMap &conditionalMap)
{
	TREConditionalMap::iterator it = conditionalMap.begin();

	while (it != conditionalMap.end())
	{
		TRESmoother &smoother0 = it->second;

		smoother0.finish();
/*
		int i, j;
		int count0 = smoother0.getVertexCount();
		const TREVertex &startVertex0 = smoother0.getStartVertex();

		for (i = 0; i < count0; i++)
		{
			const TREVertex &vertex0 = smoother0.getVertex(i);
			TRESmoother &smoother1 = conditionalMap[TREVertexKey(vertex0)];
			int count1 = smoother1.getVertexCount();
//			const TREVertex &startVertex1 = smoother1.getStartVertex();

			for (j = 0; j < count1; j++)
			{
				const TREVertex &vertex1 = smoother1.getVertex(j);

				if (vertex1.v[0] == startVertex0.v[0] &&
					vertex1.v[1] == startVertex0.v[1] &&
					vertex1.v[2] == startVertex0.v[2])
				{
					TCVector &normal0 = smoother0.getNormal(i);
					TCVector &normal1 = smoother1.getNormal(j);

					if (TRESmoother::shouldFlipNormal(normal0, normal1))
					{
						normal0 -= normal1;
					}
					else
					{
						normal0 += normal1;
					}
					normal1 = normal0.normalize();
					break;
				}
			}
		}
*/
		it++;
	}
}

void TREModel::applyShapeNormals(TRENormalInfoArray *normalInfos)
{
	int i;
	int infoCount = normalInfos->getCount();
	TCVector normal;
	TCVector vertexNormal;

	for (i = 0; i < infoCount; i++)
	{
		TRENormalInfo *normalInfo = (*normalInfos)[i];
		TREVertexArray *normals = normalInfo->m_normals;
		TRESmoother *smoother = normalInfo->m_smoother;
		TREVertex &vertex = normals->vertexAtIndex(normalInfo->m_normalIndex);
		
		vertexNormal = TCVector(vertex.v);
		normal = smoother->getNormal(normalInfo->m_smootherIndex);
		if (TRESmoother::shouldFlipNormal(normal, TCVector(vertex.v)))
		{
			normal *= -1.0f;
		}
		// The following number is the cos of 25 degrees.  I don't want to
		// calculate it on the fly.  We only want to apply this normal if the
		// difference between it an the original normal is less than 25 degrees.
		// If the normal only applies to two faces, then the faces have to be
		// more than 50 degrees apart for this to happen.  Note that low-res
		// studs have 45-degree angles between the faces, so 50 gives a little
		// leeway.
		if (normal.dot(vertexNormal) > 0.906307787f)
		{
			memcpy(vertex.v, (const float *)normal, 3 * sizeof(float));
		}
	}
}

void TREModel::calcShapeNormals(TREConditionalMap &conditionalMap,
								TRENormalInfoArray *normalInfos,
								TREShapeType shapeType)
{
	calcShapeNormals(conditionalMap, normalInfos, TREMStandard, shapeType);
	calcShapeNormals(conditionalMap, normalInfos, TREMBFC, shapeType);
}

void TREModel::calcShapeNormals(TREConditionalMap &conditionalMap,
								TRENormalInfoArray *normalInfos,
								TREMSection section, TREShapeType shapeType)
{
	calcShapeNormals(conditionalMap, normalInfos, m_shapes[section], shapeType);
	calcShapeNormals(conditionalMap, normalInfos, m_coloredShapes[section],
		shapeType);
}

/*
This function causes all the smoothed normals to be calculated.  It doesn't
actually apply them, but calculates their values, and records them so that they
can be later applied to the actual geometry.
*/
void TREModel::calcShapeNormals(TREConditionalMap &conditionalMap,
								TRENormalInfoArray *normalInfos,
								TREShapeGroup *shapeGroup,
								TREShapeType shapeType)
{
	if (shapeGroup)
	{
		TCULongArray *indices = shapeGroup->getIndices(shapeType);

		if (indices)
		{
			int i, j;
			int count = indices->getCount();
			int shapeSize = 3;
			TREVertexArray *vertices =
				shapeGroup->getVertexStore()->getVertices();
			TREVertexArray *normals =
				shapeGroup->getVertexStore()->getNormals();

			if (shapeType == TRESQuad)
			{
				shapeSize = 4;
			}
			for (i = 0; i < count; i += shapeSize)
			{
				for (j = 0; j < shapeSize; j++)
				{
					// Process the smooth edge between points i + j and
					// i + j + 1 (with wrap-around based on shapeSize).  Pass
					// i + j + 2 (with wrap-around) in to allow multiple
					// continuous shapes that all share one point to all get
					// smoothed together.
					processSmoothEdge(conditionalMap, normalInfos, vertices,
						normals, (*indices)[i + j],
						(*indices)[i + ((j + 1) % shapeSize)],
						(*indices)[i + ((j + 2) % shapeSize)]);
				}
			}
		}
	}
}

/*
This function looks for a conditional line between the points at index0 and
index1, and also between index1 and index2.  If it finds one between index0 and
index1, it considers that edge to be smooth, and adds the surface normal
associated with the point at index0 to the surface normal stored in the
TRESmoother associated with each of the two points at the ends of the
conditional line.  If it finds the second conditional line, it adds that line to
the TGLSmoother associated with index1.  Smoothers that are associated with
each other will later be smoothed together.
*/
void TREModel::processSmoothEdge(TREConditionalMap &conditionalMap,
								TRENormalInfoArray *normalInfos,
								 const TREVertexArray *vertices,
								 TREVertexArray *normals, int index0,
								 int index1, int index2)
{
	TRESmoother *smoother0 = NULL;
	int line0Index = getConditionalLine(conditionalMap,
		vertices->constVertexAtIndex(index0),
		vertices->constVertexAtIndex(index1), smoother0);

	if (line0Index >= 0)
	{
		// line0Index is the index in smoother0 of the condtional line that goes
		// between the points at index0 and index1.  Note that smoother has
		// been initialized to point to the smoother associated with the point
		// at index0.
		TRESmoother *smoother1 = NULL;
		int line1Index = getConditionalLine(conditionalMap,
			vertices->constVertexAtIndex(index1),
			vertices->constVertexAtIndex(index0), smoother1);

		if (smoother1)
		{
			// If we found the first conditional line (line0Index >= 0), then we
			// should always get smoother1, since smoother1 just corresponds to
			// the point at the other end of the conditional line.  Since we
			// found a conditional line, and all of them are inserted keyed off
			// of both ends, smoother1 should always be non-NULL if line0Index
			// >= 0.
			const TCVector &normal0 =
				TCVector(normals->constVertexAtIndex(index0).v);
			TRENormalInfo *normalInfo;
			// Check to see if there is conditional line between the points at
			// index1 and index2.  Note that by passing smoother1 into
			// getConditionalLine with a value already set, it uses that
			// smoother, and skips the lookup.  If line2Index comes out >= 0,
			// the point will be marked as shared between the two conditionals,
			// so that it will be smoothed betwen them.
			int line2Index = getConditionalLine(conditionalMap,
				vertices->constVertexAtIndex(index1),
				vertices->constVertexAtIndex(index2), smoother1);

			// Check to see if the normal for index0 is more than 90 degrees
			// away from the running total normal stored in smoother0.  Note
			// that each conditional line gets its own normal unless two are
			// merged together below.
			if (TRESmoother::shouldFlipNormal(smoother0->getNormal(line0Index),
				normal0))
			{
				// Subtract shape's normal from the running total, since it is
				// more than 90 degrees off.
				smoother0->getNormal(line0Index) -= normal0;
			}
			else
			{
				// Add shape's normal to the running total.
				smoother0->getNormal(line0Index) += normal0;
			}
			normalInfo = new TRENormalInfo;
			normalInfo->m_normals = normals;
			normalInfo->m_normalIndex = index0;
			normalInfo->m_smoother = smoother0;
			normalInfo->m_smootherIndex = line0Index;
			normalInfos->addObject(normalInfo);
			normalInfo->release();
			if (line1Index >= 0)
			{
				// This should always be the case.
				const TCVector &normal1 =
					TCVector(normals->constVertexAtIndex(index1).v);

				// Check to see if the normal for index1 is more than 90 degrees
				// away from the running total normal stored in smoother1.  Note
				// that each conditional line gets its own normal unless two are
				// merged together below.
				if (TRESmoother::shouldFlipNormal(smoother1->getNormal(
					line1Index), normal1))
				{
					// Subtract shape's normal from the running total, since it is
					// more than 90 degrees off.
					smoother1->getNormal(line1Index) -= normal1;
				}
				else
				{
					// Add shape's normal to the running total.
					smoother1->getNormal(line1Index) += normal1;
				}
				normalInfo = new TRENormalInfo;
				normalInfo->m_normals = normals;
				normalInfo->m_normalIndex = index1;
				normalInfo->m_smoother = smoother1;
				normalInfo->m_smootherIndex = line1Index;
				normalInfos->addObject(normalInfo);
				normalInfo->release();
			}
			if (line2Index >= 0)
			{
				smoother1->markShared(line1Index, line2Index);
			}
		}
	}
}

/*
This functions finds a conditional line in conditionalMap that goes from vertex0
to vertex1, and returns its index if it exists.  Returns -1 otherwise.  Also,
sets smoother equal to the TRESmoother that goes with point0 in conditionalMap
in order to speed up processing in the function that calls us.
*/
int TREModel::getConditionalLine(TREConditionalMap &conditionalMap,
								 const TREVertex point0, const TREVertex point1,
								 TRESmoother *&smoother)
{
	int i;
	int count;

	if (!smoother)
	{
		TREVertexKey pointKey(point0);
		TREConditionalMap::iterator it = conditionalMap.find(pointKey);

//		debugPrintf("%f %f %f -> ", point0.v[0], point0.v[1], point0.v[2]);
//		debugPrintf("%f %f %f\n", point1.v[0], point1.v[1], point1.v[2]);
		if (it == conditionalMap.end())
		{
			return -1;
		}
		else
		{
			smoother = &(*it).second;
		}
	}
	count = smoother->getVertexCount();
	for (i = 0; i < count; i++)
	{
		// Note that by definition point0 will match the start point of the
		// smoother, so there's no need to check.
		if (smoother->getVertex(i).approxEquals(point1, 0.01f))
		{
//			debugPrintf("*\n");
			return i;
		}
	}
	return -1;
}

/*
This function fills conditionalMap with all the conditional lines, using the
points at each end of each conditional line as keys in the tree.  This means
that each conditional line will go into the tree twice.
*/
void TREModel::fillConditionalMap(TREConditionalMap &conditionalMap)
{
	fillConditionalMap(conditionalMap, m_shapes[TREMConditionalLines]);
	fillConditionalMap(conditionalMap, m_coloredShapes[TREMConditionalLines]);
}

void TREModel::fillConditionalMap(TREConditionalMap &conditionalMap,
								  TREShapeGroup *shapeGroup)
{
	if (shapeGroup)
	{
		TCULongArray *indices = shapeGroup->getIndices(TRESConditionalLine);

		if (indices)
		{
			int i;
			int count = indices->getCount();
			TREVertexArray *vertices =
				shapeGroup->getVertexStore()->getVertices();

			// Note there are 2 indices per conditional line; hence the += 2.
			for (i = 0; i < count; i += 2)
			{
				int index0 = (*indices)[i];
				int index1 = (*indices)[i + 1];
				const TREVertex &vertex0 = vertices->constVertexAtIndex(index0);
				const TREVertex &vertex1 = vertices->constVertexAtIndex(index1);
				TREVertexKey vertex0Key(vertex0);
				TREVertexKey vertex1Key(vertex1);

				if (vertex0Key < vertex1Key || vertex1Key < vertex0Key)
				{
					// Add the conditional line to the map using its first point
					// as the key in the map.
					addConditionalPoint(conditionalMap, vertices, index0,
						index1, vertex0Key);
					// Add the conditional line to the map using its second point
					// as the key in the map.
					addConditionalPoint(conditionalMap, vertices, index1,
						index0, vertex1Key);
				}
				else
				{
					TCVector length = TCVector(vertex0.v) - TCVector(vertex1.v);

					debugPrintf(2, "Conditional too short to map: %f.\n",
						length.length());
				}
			}
		}
	}
}

/*
This function adds the line between index0 and index1 to conditionalMap, using
the point at index0 as its key in the map.
*/
void TREModel::addConditionalPoint(TREConditionalMap &conditionalMap,
								   const TREVertexArray *vertices, int index0,
								   int index1, const TREVertexKey &vertexKey)
{
//	TREVertexKey vertexKey(vertices->constVertexAtIndex(index0));
	TREConditionalMap::iterator it = conditionalMap.find(vertexKey);

	if (it == conditionalMap.end())
	{
		// Note that this would probably be more clear if we used the []
		// operator of map.  However, that would require extra lookups, and
		// we're trying to keep the lookups to a minimum.
		TREConditionalMap::value_type newValue(vertexKey,
			TRESmoother(vertices->constVertexAtIndex(index0)));

		// The insert function returns a pair, where the first of the pair is
		// an iterator pointing to the newly inserted item, and the second of
		// the pair is a boolean saying whether or not an insertion occurred.
		// Since we got here, we know the item doesn't already exist, so we
		// really don't care about the second of the pair.
		it = conditionalMap.insert(newValue).first;
	}
	// If the item didn't originally exist, it gets set during insertion above.
	// If the item did exist, it's set in the find() call.  Note that not all
	// the vertices that match are necessarily equal.  However, this won't
	// effect the results.
	(*it).second.addVertex(vertices->constVertexAtIndex(index1));
}

void TREModel::flatten(void)
{
	if (m_subModels && m_subModels->getCount())
	{
		flatten(this, TCVector::getIdentityMatrix(), 0, false, 0, false, false);
		if (m_subModels)
		{
			m_subModels->removeAll();
		}
		m_flags.flattened = true;
	}
}

void TREModel::flatten(TREModel *model, const float *matrix, TCULong color,
					   bool colorSet, TCULong edgeColor, bool edgeColorSet,
					   bool includeShapes)
{
	TRESubModelArray *subModels = model->m_subModels;

	if (includeShapes)
	{
		int i;

		for (i = 0; i <= TREMLast; i++)
		{
			TREShapeGroup *otherShapeGroup = model->m_shapes[i];
			TREColoredShapeGroup *otherColoredShapeGroup =
				model->m_coloredShapes[i];

			if (otherShapeGroup)
			{
				bool actualColorSet = colorSet;
				TCULong actualColor = color;

				if (i == TREMEdgeLines || i == TREMConditionalLines)
				{
					actualColorSet = edgeColorSet;
					actualColor = edgeColor;
				}
				if (actualColorSet)
				{
					TREColoredShapeGroup *coloredShapeGroup;
					
					setupColored((TREMSection)i);
					coloredShapeGroup = m_coloredShapes[i];
					coloredShapeGroup->getVertexStore()->setupColored();
					if (i == TREMStud || i == TREMStudBFC)
					{
						coloredShapeGroup->getVertexStore()->setupTextured();
					}
					coloredShapeGroup->flatten(otherShapeGroup, matrix,
						actualColor, true);
//					flattenShapes(coloredShapeGroup, otherShapeGroup,
//						matrix, actualColor, true);
				}
				else
				{
					TREShapeGroup *shapeGroup;

					setup((TREMSection)i);
					shapeGroup = m_shapes[i];
					if (i == TREMStud || i == TREMStudBFC)
					{
						shapeGroup->getVertexStore()->setupTextured();
					}
					shapeGroup->flatten(otherShapeGroup, matrix, 0, false);
//					flattenShapes(shapeGroup, otherShapeGroup, matrix, 0,
//						false);
				}
			}
			if (otherColoredShapeGroup)
			{
				setupColored((TREMSection)i);
				m_coloredShapes[i]->flatten(otherColoredShapeGroup, matrix, 0,
					false);
//				flattenShapes(m_coloredShapes[i], otherColoredShapeGroup,
//					matrix, 0, false);
			}
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

			TCVector::multMatrix(matrix, subModel->getMatrix(), newMatrix);
			if (subModel->isColorSet())
			{
				flatten(subModel->getEffectiveModel(), newMatrix,
					htonl(subModel->getColor()), true,
					htonl(subModel->getEdgeColor()), true, true);
			}
			else
			{
				flatten(subModel->getEffectiveModel(), newMatrix, color,
					colorSet, edgeColor, edgeColorSet, true);
			}
		}
	}
}

/*
void TREModel::flattenShapes(TREVertexArray *dstVertices,
							 TREVertexArray *dstNormals,
							 TCULongArray *dstColors,
							 TCULongArray *dstIndices,
							 TCULongArray *dstCPIndices,
							 TREVertexArray *srcVertices,
							 TREVertexArray *srcNormals,
							 TCULongArray *srcColors,
							 TCULongArray *srcIndices,
							 TCULongArray *srcCPIndices,
							 float *matrix,
							 TCULong color,
							 bool colorSet)
{
	int i;
	int count = srcIndices->getCount();

	for (i = 0; i < count; i++)
	{
		int index = (*srcIndices)[i];
		TREVertex vertex = (*srcVertices)[index];

		dstIndices->addValue(dstVertices->getCount());
		transformVertex(vertex, matrix);
		dstVertices->addVertex(vertex);
		if (srcNormals)
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
		if (srcCPIndices && dstCPIndices)
		{
			index = (*srcCPIndices)[i];
			vertex = (*srcVertices)[index];
			dstCPIndices->addValue(dstVertices->getCount());
			transformVertex(vertex, matrix);
			dstVertices->addVertex(vertex);
			if (srcNormals)
			{
				TREVertex normal = (*srcNormals)[index];

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

void TREModel::flattenStrips(TREVertexArray *dstVertices,
							 TREVertexArray *dstNormals,
							 TCULongArray *dstColors,
							 TCULongArray *dstIndices,
							 TCULongArray *dstStripCounts,
							 TREVertexArray *srcVertices,
							 TREVertexArray *srcNormals,
							 TCULongArray *srcColors,
							 TCULongArray *srcIndices,
							 TCULongArray *srcStripCounts, float *matrix,
							 TCULong color, bool colorSet)
{
	int i, j;
	int numStrips = srcStripCounts->getCount();
	int indexOffset = 0;

	for (i = 0; i < numStrips; i++)
	{
		int stripCount = (*srcStripCounts)[i];

		dstStripCounts->addValue(stripCount);
		for (j = 0; j < stripCount; j++)
		{
			int index = (*srcIndices)[j + indexOffset];
			TREVertex vertex = (*srcVertices)[index];

			dstIndices->addValue(dstVertices->getCount());
			transformVertex(vertex, matrix);
			dstVertices->addVertex(vertex);
			if (srcNormals)
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
		indexOffset += stripCount;
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
				TREVertexArray *srcVertices = srcVertexStore->getVertices();
				TREVertexArray *srcNormals = srcVertexStore->getNormals();
				TCULongArray *srcColors = srcVertexStore->getColors();
				TCULongArray *dstIndices =
					dstShapes->getIndices((TREShapeType)bit, true);
				TCULongArray *srcCPIndices = NULL;
				TCULongArray *dstCPIndices = NULL;

				if ((TREShapeType)bit == TRESConditionalLine)
				{
					srcCPIndices = srcShapes->getControlPointIndices();
					dstCPIndices = dstShapes->getControlPointIndices(true);
				}
				if (srcVertices)
				{
					TREVertexArray *dstVertices = dstVertexStore->getVertices();
					TREVertexArray *dstNormals = dstVertexStore->getNormals();
					TCULongArray *dstColors = dstVertexStore->getColors();
					TREShapeType shapeType = (TREShapeType)bit;

					if (shapeType < TRESFirstStrip)
					{
						flattenShapes(dstVertices, dstNormals, dstColors,
							dstIndices, dstCPIndices, srcVertices, srcNormals,
							srcColors, srcIndices, srcCPIndices, matrix, color,
							colorSet);
					}
					else
					{
						TCULongArray *dstStripCounts =
							dstShapes->getStripCounts((TREShapeType)bit, true);
						TCULongArray *srcStripCounts =
							srcShapes->getStripCounts((TREShapeType)bit);

						flattenStrips(dstVertices, dstNormals, dstColors,
							dstIndices, dstStripCounts, srcVertices, srcNormals,
							srcColors, srcIndices, srcStripCounts, matrix,
							color, colorSet);
					}
				}
			}
		}
	}
}
*/

/*
void TREModel::setGlNormalize(bool value)
{
//	if (value != sm_normalizeOn)
	{
		if (value)
		{
//			glEnable(GL_RESCALE_NORMAL);
			glEnable(GL_NORMALIZE);
//			sm_normalizeOn = true;
		}
		else
		{
//			glDisable(GL_RESCALE_NORMAL);
			glDisable(GL_NORMALIZE);
//			sm_normalizeOn = false;
		}
	}
}
*/

void TREModel::addSlopedCylinder(const TCVector& center, float radius,
								 float height, int numSegments,
								 int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector top = center;
	TCVector normal = TCVector(0.0f, 1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments * 2 + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i * 2]);
		top[1] =
			center.get(1) + height - ((height / radius) * points[i * 2][0]);
		setCirclePoint(angle, radius, top, points[i * 2 + 1]);
		if (height == 0.0f)
		{
			normals[i * 2] = normal;
			normals[i * 2 + 1] = normal;
		}
		else
		{
			normals[i * 2] = (points[i * 2] - center).normalize();
			normals[i * 2 + 1] =
				(points[i * 2 + 1] - top).normalize();
		}
	}
	if (bfc)
	{
		addBFCQuadStrip(points, normals, vertexCount);
	}
	else
	{
		addQuadStrip(points, normals, vertexCount);
	}
	if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
	{
		addOpenConeConditionals(points, numSegments, usedSegments);
	}
	delete[] points;
	delete[] normals;
}

void TREModel::addSlopedCylinder2(const TCVector& center, float radius,
								  float height, int numSegments,
								  int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector top = center;
	TCVector normal = TCVector(0.0f, 1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments * 2 + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i + (float)M_PI / 2.0f;
		setCirclePoint(angle, radius, center, points[i * 2]);
		top[1] = myabs(points[i * 2][0]);
		setCirclePoint(angle, radius, top, points[i * 2 + 1]);
		if (height == 0.0f)
		{
			normals[i * 2] = normal;
			normals[i * 2 + 1] = normal;
		}
		else
		{
			normals[i * 2] = (points[i * 2] - center).normalize();
			normals[i * 2 + 1] =
				(points[i * 2 + 1] - top).normalize();
		}
	}
	if (bfc)
	{
		addBFCQuadStrip(points, normals, vertexCount);
	}
	else
	{
		addQuadStrip(points, normals, vertexCount);
	}
	if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
	{
		addSlopedCylinder2Conditionals(points, numSegments, usedSegments);
	}
	delete[] points;
	delete[] normals;
}

void TREModel::addCylinder(const TCVector& center, float radius, float height,
						   int numSegments, int usedSegments, bool bfc)
{
	addOpenCone(center, radius, radius, height, numSegments, usedSegments, bfc);
}

void TREModel::addStudDisc(const TCVector &center, float radius,
						   int numSegments, int usedSegments, bool bfc)
{
	addDisc(center, radius, numSegments, usedSegments, bfc,
		m_mainModel->getStudLogoFlag());
}

void TREModel::genStudTextureCoords(TCVector *textureCoords, int vertexCount)
{
	int i;
	TCVector p1;
	TCVector offset = TCVector(0.5f, 0.5f, 0.0f);
	int numSegments = vertexCount - 2;

	textureCoords[0] = TCVector(0.5f, 0.5f, 0.0f);
	for (i = 1; i < vertexCount; i++)
	{
		float x, z;
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * (i - 1);
		x = (float)cos(angle) * 0.5f;
		z = (float)sin(angle) * 0.5f;
		p1[0] = z;
		p1[1] = x;
		p1 += offset;
		p1[1] = 1.0f - p1[1];
		textureCoords[i] = p1;
	}
}

void TREModel::addChrd(const TCVector &center, float radius, int numSegments,
					   int usedSegments, bool bfc)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	int i;
	TCVector normal = TCVector(0.0f, -1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments + 1;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
//	points[0] = center + TCVector(1.0f, 0.0f, 0.0f);
//	normals[0] = normal;
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i]);
		normals[i] = normal;
	}
	if (bfc)
	{
		addBFCTriangleFan(points, normals, NULL, vertexCount, true);
	}
	else
	{
		addTriangleFan(points, normals, NULL, vertexCount, true);
	}
	delete[] points;
	delete[] normals;
}

void TREModel::addDisc(const TCVector &center, float radius, int numSegments,
					   int usedSegments, bool bfc, bool stud)
{
	int vertexCount;
	TCVector *points;
	TCVector *normals;
	TCVector *textureCoords = NULL;
	int i;
	TCVector normal = TCVector(0.0f, -1.0f, 0.0f);

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	vertexCount = usedSegments + 2;
	points = new TCVector[vertexCount];
	normals = new TCVector[vertexCount];
	points[0] = center;
	normals[0] = normal;
	for (i = 0; i <= usedSegments; i++)
	{
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(angle, radius, center, points[i + 1]);
		normals[i + 1] = normal;
	}
	if (stud && TREMainModel::getStudTextures())
	{
		textureCoords = new TCVector[vertexCount];
		genStudTextureCoords(textureCoords, vertexCount);
	}
	if (bfc)
	{
		addBFCTriangleFan(points, normals, textureCoords, vertexCount, true);
	}
	else
	{
		addTriangleFan(points, normals, textureCoords, vertexCount, true);
	}
	delete[] points;
	delete[] normals;
	delete[] textureCoords;
}

void TREModel::addNotDisc(const TCVector &center, float radius, int numSegments,
						  int usedSegments, bool bfc)
{
	int quarter = numSegments / 4;
	int numQuarters;
	int i, j;
	TCVector normal = TCVector(0.0f, -1.0f, 0.0f);
	TCVector p1;

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	numQuarters = (usedSegments + quarter - 1) / quarter;
	for (i = 0; i < numQuarters; i++)
	{
		TCVector *points;
		TCVector *normals;
		int vertexCount;
		int quarterSegments = quarter;
		float zMult = 1.0f;
		float xMult = 1.0f;

		if (i >= 2)
		{
			zMult = -1.0f;
		}
		if (i == 1 || i == 2)
		{
			xMult = -1.0f;
		}
		if (i == numQuarters - 1)
		{
			quarterSegments = usedSegments % quarter;
			if (!quarterSegments)
			{
				quarterSegments = quarter;
			}
		}
		vertexCount = quarterSegments + 2;
		points = new TCVector[vertexCount];
		normals = new TCVector[vertexCount];
		points[0] = center + TCVector(xMult * radius, 0.0f, zMult * radius);
		normals[0] = normal;
		for (j = 0; j <= quarterSegments; j++)
		{
			float x, z;
			float angle;

			angle = 2.0f * (float)M_PI / numSegments *
				(j + i * quarterSegments);
			x = radius * (float)cos(angle);
			z = radius * (float)sin(angle);
			p1[0] = center.get(0) + x;
			p1[2] = center.get(2) + z;
			p1[1] = center.get(1);
			points[quarterSegments - j + 1] = p1;
			normals[quarterSegments - j + 1] = normal;
		}
		if (bfc)
		{
			addBFCTriangleFan(points, normals, NULL, vertexCount, true);
		}
		else
		{
			addTriangleFan(points, normals, NULL, vertexCount, true);
		}
		delete[] points;
		delete[] normals;
	}
}

void TREModel::setCirclePoint(float angle, float radius, const TCVector& center,
							  TCVector& point)
{
	float x1, z1;

	x1 = radius * (float)cos(angle);
	z1 = radius * (float)sin(angle);
	point[0] = center.get(0) + x1;
	point[1] = center.get(1);
	point[2] = center.get(2) + z1;
}

void TREModel::addCone(const TCVector &center, float radius, float height,
					   int numSegments, int usedSegments, bool bfc)
{
	int i;
	TCVector top = center;
	TCVector p1, p2, p3;
	TCVector linePoints[2];
	TCVector controlPoints[2];
	TCVector *points = new TCVector[3];
	TCVector *normals = new TCVector[3];
	TCVector tri0Cross;
	TCVector tri1Cross;
	TCVector tri2Cross;
	int axis1 = 2;
	int axis2 = 0;

	if (numSegments == usedSegments * 2)
	{
		axis2 = 2;
	}
	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	top[1] += height;
	points[2] = top;
	for (i = 0; i < usedSegments; i++)
	{
		float angle0, angle1, angle2, angle3;

		angle0 = 2.0f * (float)M_PI / numSegments * (i - 1);
		angle1 = 2.0f * (float)M_PI / numSegments * i;
		angle2 = 2.0f * (float)M_PI / numSegments * (i + 1);
		angle3 = 2.0f * (float)M_PI / numSegments * (i + 2);
		setCirclePoint(angle1, radius, center, p1);
		setCirclePoint(angle2, radius, center, p2);
		points[0] = p2;
		points[1] = p1;
		if (i == 0)
		{
			setCirclePoint(angle0, radius, center, p3);
			tri0Cross = (p3 - top) * (p3 - p1);
			tri1Cross = (p1 - top) * (p1 - p2);
			if (shouldLoadConditionalLines())
			{
				linePoints[0] = p1;
				linePoints[1] = top;
				controlPoints[0] = p1;
				controlPoints[0][axis1] -= 1.0f;
				controlPoints[1] = p2;
				addConditionalLine(linePoints, controlPoints);
			}
		}
		else
		{
			tri0Cross = tri1Cross;
			tri1Cross = tri2Cross;
		}
		normals[1] = tri1Cross + tri0Cross;
		normals[1].normalize();
		setCirclePoint(angle3, radius, center, p3);
		if (shouldLoadConditionalLines())
		{
			linePoints[0] = p2;
			linePoints[1] = top;
			controlPoints[0] = p1;
			if (i == usedSegments - 1)
			{
				controlPoints[1] = p2;
				if (usedSegments * 8 == numSegments)
				{
					controlPoints[1][0] -= 1.0f;
					controlPoints[1][2] += 1.0f;
				}
				else if (usedSegments * 8 == numSegments * 3)
				{
					controlPoints[1][0] -= 1.0f;
					controlPoints[1][2] -= 1.0f;
				}
				else if (usedSegments * 4 == numSegments * 3)
				{
					controlPoints[1][0] += 1.0f;
				}
				else
				{
					controlPoints[1][axis2] -= 1.0f;
				}
			}
			else
			{
				controlPoints[1] = p3;
			}
			addConditionalLine(linePoints, controlPoints);
		}
		tri2Cross = (p2 - top) * (p2 - p3);
		normals[0] = tri2Cross + tri1Cross;
		normals[0].normalize();
		normals[2] = tri1Cross;
		normals[2].normalize();
		if (bfc)
		{
			addBFCTriangle(points, normals);
		}
		else
		{
			addTriangle(points, normals);
		}
	}
	delete[] points;
	delete[] normals;
}

TCVector TREModel::calcIntersection(int i, int j, int num,
									TCVector* zeroXPoints,
									TCVector* zeroYPoints,
									TCVector* zeroZPoints)
{
	TCVector temp1, temp2, temp3, temp4, temp5, temp6;
	TCVector p1, p2, p3;

	if (i + j == num)
	{
		return zeroXPoints[j];
	}
	else if (i == 0)
	{
		return zeroZPoints[num - j];
	}
	else if (j == 0)
	{
		return zeroYPoints[i];
	}
	temp1 = zeroYPoints[i];
	temp2 = zeroXPoints[num - i];
	temp3 = zeroZPoints[num - j];
	temp4 = zeroXPoints[j];
	temp5 = zeroYPoints[i + j];
	temp6 = zeroZPoints[num - i - j];
	return (temp1 + temp2 + temp3 + temp4 + temp5 + temp6 -
		zeroXPoints[0] - zeroYPoints[0] - zeroZPoints[0]) / 9.0f;
}

void TREModel::addTorusIO(bool inner, const TCVector& center, float yRadius,
						  float xzRadius, int numSegments, int usedSegments,
						  bool bfc)
{
	int i, j;
	TCVector p1, p2;
	TCVector top = center;
	int ySegments = numSegments / 4;
	TCVector *points;
	TCVector *stripPoints;
	TCVector *stripNormals;
	int spot;
	int stripSize = (ySegments + 1) * 2;

	points = new TCVector[(ySegments + 1) * (usedSegments + 1)];
	stripPoints = new TCVector[stripSize];
	stripNormals = new TCVector[stripSize];
	for (i = 0; i <= usedSegments; i++)
	{
		float xzAngle;	// Angle in the xz plane

		xzAngle = 2.0f * (float)M_PI / numSegments * i;
		for (j = 0; j <= ySegments; j++)
		{
			float yAngle; // Angle above the xz plane
			float currentRadius;

			if (inner)
			{
				yAngle = (float)M_PI - 2.0f * (float)M_PI / numSegments * j;
			}
			else
			{
				yAngle = 2.0f * (float)M_PI / numSegments * j;
			}
			top[1] = xzRadius * (float)sin(yAngle) + center.get(1);
			currentRadius = xzRadius * (float)cos(yAngle) + yRadius;
			setCirclePoint(xzAngle, currentRadius, top, p1);
			points[i * (ySegments + 1) + j] = p1;
		}
	}
	top = center;
	for (i = 0; i < usedSegments; i++)
	{
		float xzAngle;	// Angle in the xz plane
		int ofs1 = 1;
		int ofs2 = 0;

		if (inner)
		{
			ofs1 = 0;
			ofs2 = 1;
		}

		xzAngle = 2.0f * (float)M_PI / numSegments * (i + ofs2);
		setCirclePoint(xzAngle, yRadius, top, p1);
		xzAngle = 2.0f * (float)M_PI / numSegments * (i + ofs1);
		setCirclePoint(xzAngle, yRadius, top, p2);
		spot = 0;
		for (j = 0; j <= ySegments; j++)
		{
			stripPoints[spot] = points[(i + ofs1) * (ySegments + 1) + j];
			stripNormals[spot] = (stripPoints[spot] - p2).normalize();
			spot++;
			stripPoints[spot] = points[(i + ofs2) * (ySegments + 1) + j];
			stripNormals[spot] = (stripPoints[spot] - p1).normalize();
			spot++;
		}
		if (bfc)
		{
			addBFCQuadStrip(stripPoints, stripNormals, stripSize);
		}
		else
		{
			addQuadStrip(stripPoints, stripNormals, stripSize);
		}
	}
	if (shouldLoadConditionalLines())
	{
		addTorusIOConditionals(inner, points, numSegments, usedSegments, center,
			yRadius, xzRadius);
	}
	delete[] stripPoints;
	delete[] stripNormals;
	delete[] points;
}

/*
void TREModel::addTorusO(const TCVector& center, float yRadius,
						 float xzRadius, int numSegments, int usedSegments,
						 bool bfc)
{
	int i, j;
	TCVector p1, p2;
	TCVector top = center;
	int ySegments = numSegments / 4;
	TCVector *points;
	TCVector *stripPoints;
	TCVector *stripNormals;
	int spot;
	int stripSize = (ySegments + 1) * 2;

	points = new TCVector[(ySegments + 1) * (usedSegments + 1)];
	stripPoints = new TCVector[stripSize];
	stripNormals = new TCVector[stripSize];
	for (i = 0; i <= usedSegments; i++)
	{
		float xzAngle;	// Angle in the xz plane

		xzAngle = 2.0f * (float)M_PI / numSegments * i;
		for (j = 0; j <= ySegments; j++)
		{
			float yAngle; // Angle above the xz plane
			float currentRadius;

			yAngle = 2.0f * (float)M_PI / numSegments * j;
			top[1] = xzRadius * (float)sin(yAngle) + center.get(1);
			currentRadius = xzRadius * (float)cos(yAngle) + yRadius;
			setCirclePoint(xzAngle, currentRadius, top, p1);
			points[i * (ySegments + 1) + j] = p1;
		}
	}
	top = center;
	for (i = 0; i < usedSegments; i++)
	{
		float xzAngle;	// Angle in the xz plane

		xzAngle = 2.0f * (float)M_PI / numSegments * i;
		setCirclePoint(xzAngle, yRadius, top, p1);
		xzAngle = 2.0f * (float)M_PI / numSegments * (i + 1);
		setCirclePoint(xzAngle, yRadius, top, p2);
		spot = 0;
		for (j = 0; j <= ySegments; j++)
		{
			stripPoints[spot] = points[(i + 1) * (ySegments + 1) + j];
			stripNormals[spot] = (stripPoints[spot] - p2).normalize();
			spot++;
			stripPoints[spot] = points[i * (ySegments + 1) + j];
			stripNormals[spot] = (stripPoints[spot] - p1).normalize();
			spot++;
		}
		if (bfc)
		{
			addBFCQuadStrip(stripPoints, stripNormals, stripSize);
		}
		else
		{
			addQuadStrip(stripPoints, stripNormals, stripSize);
		}
	}
	if (shouldLoadConditionalLines())
	{
		addTorusQConditionals(points, numSegments, usedSegments, center,
			yRadius - 0.1f, xzRadius);
	}
	delete[] stripPoints;
	delete[] stripNormals;
	delete[] points;
}
*/

void TREModel::addTorusIOConditionals(bool inner, TCVector *points,
									  int numSegments, int usedSegments,
									  const TCVector& center, float radius,
									  float height)
{
	int i, j;
	TCVector p1, p2, p3, p4;
	TCVector top = center;
	top[1] = height;
	int ySegments = numSegments / 4;
	int axis = 0;

	if ((inner && height > 0.0f) || (!inner && height < 0.0f))
	{
		radius += 0.1f;
	}
	else
	{
		radius -= 0.1f;
	}
	if (numSegments == usedSegments * 2)
	{
		axis = 2;
	}
	for (i = 0; i <= usedSegments; i++)
	{
		for (j = 0; j < ySegments; j++)
		{
			p1 = points[i * (ySegments + 1) + j];
			p2 = points[i * (ySegments + 1) + j + 1];
			if (i == 0)
			{
				p3 = p1;
				p3[2] -= 0.1f;
			}
			else
			{
				p3 = points[(i - 1) * (ySegments + 1) + j];
			}
			if (i == usedSegments && numSegments != usedSegments)
			{
				p4 = p1;
				if (usedSegments * 8 == numSegments)
				{
					p4[0] -= 0.1f;
					p4[2] += 0.1f;
				}
				else if (usedSegments * 8 == numSegments * 3)
				{
					p4[0] -= 0.1f;
					p4[2] -= 0.1f;
				}
				else if (usedSegments * 4 == numSegments * 3)
				{
					p4[0] += 0.1f;
				}
				else
				{
					p4[axis] -= 0.1f;
				}
			}
			else
			{
				p4 = points[(i + 1) * (ySegments + 1) + j];
			}
			addConditionalLine(p1, p2, p3, p4);
		}
	}
	for (i = 0; i < usedSegments; i++)
	{
		for (j = 0; j <= ySegments; j++)
		{
			p1 = points[i * (ySegments + 1) + j];
			p2 = points[(i + 1) * (ySegments + 1) + j];
			if (j == 0)
			{
				p3 = p1;
				if (height > 0.0f)
				{
					p3[1] -= 0.1f;
				}
				else
				{
					p3[1] += 0.1f;
				}
			}
			else
			{
				p3 = points[i * (ySegments + 1) + (j - 1)];
			}
			if (j == ySegments)
			{
				float angle = 2.0f * (float)M_PI / numSegments * i;

				setCirclePoint(angle, radius, top, p4);
				p4[1] = height;
			}
			else
			{
				p4 = points[i * (ySegments + 1) + (j + 1)];
			}
			addConditionalLine(p1, p2, p3, p4);
		}
	}
}

void TREModel::addEighthSphere(const TCVector& center, float radius,
							   int numSegments, bool bfc)
{
	TCVector* zeroXPoints;
	TCVector* zeroYPoints;
	TCVector* zeroZPoints;
	int usedSegments = numSegments / 4;
	int i, j;
	TCVector p1, p2, p3;
	TCVector *spherePoints = NULL;
	int numMainPoints = (usedSegments + 1) * (usedSegments + 1) - 1;
	int mainSpot = 0;

	if (shouldLoadConditionalLines())
	{
		spherePoints = new TCVector[numMainPoints];
	}
	zeroXPoints = new TCVector[usedSegments + 1];
	zeroYPoints = new TCVector[usedSegments + 1];
	zeroZPoints = new TCVector[usedSegments + 1];
	for (i = 0; i <= usedSegments; i++)
	{
		float angle = 2.0f * (float)M_PI / numSegments * i;

		zeroYPoints[i][0] = 1.0f / ((float)tan(angle) + 1);
		zeroYPoints[i][1] = 0.0f;
		zeroYPoints[i][2] = 1.0f - zeroYPoints[i][0];
		zeroZPoints[i] = zeroYPoints[i].rearrange(2, 0, 1);
		zeroXPoints[i] = zeroYPoints[i].rearrange(1, 2, 0);
		zeroXPoints[i] += center;
		zeroYPoints[i] += center;
		zeroZPoints[i] += center;
	}
	for (j = 0; j < usedSegments; j++)
	{
		int stripCount = usedSegments - j;
		int stripSpot = 0;
		TCVector *points = new TCVector[stripCount * 2 + 1];
		TCVector *normals = new TCVector[stripCount * 2 + 1];

		for (i = 0; i < stripCount; i++)
		{
			if (i == 0)
			{
				p1 = calcIntersection(i, j, usedSegments, zeroXPoints,
					zeroYPoints, zeroZPoints);
				p1 *= radius / p1.length();
				normals[stripSpot] = (p1 - center).normalize();
				points[stripSpot++] = p1;
				if (shouldLoadConditionalLines())
				{
					spherePoints[mainSpot++] = p1;
				}
			}
			p2 = calcIntersection(i, j + 1, usedSegments, zeroXPoints,
				zeroYPoints, zeroZPoints);
			p2 *= radius / p2.length();
			p3 = calcIntersection(i + 1, j, usedSegments, zeroXPoints,
				zeroYPoints, zeroZPoints);
			p3 *= radius / p3.length();
			normals[stripSpot] = (p2 - center).normalize();
			points[stripSpot++] = p2;
			normals[stripSpot] = (p3 - center).normalize();
			points[stripSpot++] = p3;
			if (shouldLoadConditionalLines())
			{
				spherePoints[mainSpot++] = p2;
				spherePoints[mainSpot++] = p3;
			}
		}
		if (bfc)
		{
			addBFCTriangleStrip(points, normals, stripSpot);
		}
		else
		{
			addTriangleStrip(points, normals, stripSpot);
		}
		delete[] points;
		delete[] normals;
	}
	if (shouldLoadConditionalLines())
	{
		addEighthSphereConditionals(spherePoints, numSegments);
	}
	delete[] spherePoints;
	delete[] zeroXPoints;
	delete[] zeroYPoints;
	delete[] zeroZPoints;
}

void TREModel::addConditionalLine(const TCVector &p1, const TCVector &p2,
								  const TCVector &c1, const TCVector &c2)
{
	TCVector points[2];
	TCVector conditionalPoints[2];

	points[0] = p1;
	points[1] = p2;
	conditionalPoints[0] = c1;
	conditionalPoints[1] = c2;
	addConditionalLine(points, conditionalPoints);
}

void TREModel::addEighthSphereConditionals(TCVector *points, int numSegments)
{
	int usedSegments = numSegments / 4;
	int i, j;
	TCVector p1, p2, p3, p4;
	TCVector circlePoint;
	int mainSpot = 0;

	for (j = 0; j < usedSegments; j++)
	{
		int stripCount = usedSegments - j;
		
		for (i = 0; i < stripCount; i++)
		{
			if (i > 0)
			{
				p3 = points[mainSpot - 1];
			}
			else
			{
				if (j > 0)
				{
					p3 = points[mainSpot];
					p3[2] -= 0.1f;
				}
				else
				{
					p3 = points[mainSpot];
					p3[2] -= 0.1f;
				}
			}
			p4 = points[mainSpot + 2];
			p1 = points[mainSpot];
			p2 = points[mainSpot + 1];
			addConditionalLine(p1, p2, p3, p4);
			p3 = p1;
			p1 = p2;
			p2 = p4;
			if (i < stripCount - 1)
			{
				p4 = points[mainSpot + 3];
			}
			else
			{
				p4 = points[mainSpot + 1];
				p4[0] -= 0.1f;
			}
			addConditionalLine(p1, p2, p3, p4);
			p1 = points[mainSpot];
			p2 = points[mainSpot + 2];
			p3 = points[mainSpot + 1];
			if (j == 0)
			{
				p4 = points[mainSpot];
				p4[1] -= 0.1f;
			}
			else
			{
				p4 = points[sphereIndex(i * 2 + 2, j - 1, usedSegments)];
			}
			addConditionalLine(p1, p2, p3, p4);
			mainSpot += 2;
		}
		mainSpot++;
	}
}

int TREModel::sphereIndex(int i, int j, int usedSegments)
{
	int retVal = 0;
	int k;

	for (k = 0; k < j; k++)
	{
		int rowSize = usedSegments - k;

		retVal += rowSize * 2 + 1;
	}
	return retVal + i;
}

void TREModel::addOpenCone(const TCVector& center, float radius1, float radius2,
						   float height, int numSegments, int usedSegments,
						   bool bfc)
{
	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	if (radius1 == 0.0f)
	{
		addCone(center, radius2, height, numSegments, usedSegments, bfc);
	}
	else if (radius2 == 0.0f)
	{
		addCone(center, radius1, height, numSegments, usedSegments, bfc);
	}
	else
	{
		int vertexCount = usedSegments * 2 + 2;
		TCVector *points = new TCVector[vertexCount];
		TCVector *normals = new TCVector[vertexCount];
		int i;
		TCVector top = center;
		TCVector normal = TCVector(0.0f, -1.0f, 0.0f);
		TCVector topNormalPoint;
		TCVector normalPoint;
		float normalAdjust = 1.0f;

		if (height < 0.0f)
		{
			normalAdjust = -1.0f;
		}
		top[1] += height;
		if (height)
		{
			topNormalPoint = top - normal * radius2 * (radius2 - radius1) /
				height;
			normalPoint = center - normal * radius1 * (radius2 - radius1) /
				height;
		}
		for (i = 0; i <= usedSegments; i++)
		{
			float angle;

			angle = 2.0f * (float)M_PI / numSegments * i;
			setCirclePoint(angle, radius1, center, points[i * 2]);
			setCirclePoint(angle, radius2, top, points[i * 2 + 1]);
			if (height == 0.0f)
			{
				normals[i * 2] = normal;
				normals[i * 2 + 1] = normal;
			}
			else
			{
				normals[i * 2] = (points[i * 2] - normalPoint).normalize() *
					normalAdjust;
				normals[i * 2 + 1] =
					(points[i * 2 + 1] - topNormalPoint).normalize() *
					normalAdjust;
			}
		}
		if (bfc)
		{
			addBFCQuadStrip(points, normals, vertexCount, height == 0.0f);
		}
		else
		{
			addQuadStrip(points, normals, vertexCount, height == 0.0f);
		}
		if (shouldLoadConditionalLines() && !fEq(height, 0.0f))
		{
			addOpenConeConditionals(points, numSegments, usedSegments);
		}
		delete[] points;
		delete[] normals;
	}
}

void TREModel::addOpenConeConditionals(TCVector *points, int numSegments,
									   int usedSegments)
{
	int i;
	TCVector controlPoints[2];
	int axis1 = 2;
	int axis2 = 0;
	TCVector *p1;
	TCVector *p2;

	if (numSegments == usedSegments * 2)
	{
		axis2 = 2;
	}
	for (i = 0; i <= usedSegments; i++)
	{
		p1 = &points[i * 2];
		p2 = &points[i * 2 + 1];
		if (p1 == p2)
		{
			continue;
		}
		if (i == 0)
		{
			if (numSegments == usedSegments)
			{
				controlPoints[0] = points[numSegments * 2 - 2];
			}
			else
			{
				controlPoints[0] = *p1;
				controlPoints[0][axis1] -= 1.0f;
			}
		}
		else
		{
			controlPoints[0] = points[(i - 1) * 2];
		}
		if (i == usedSegments)
		{
			if (numSegments == usedSegments)
			{
				// No need to repeat the last one if it's a closed surface.
				return;
			}
			else
			{
				controlPoints[1] = *p1;
				if (usedSegments * 16 == numSegments)
				{
					controlPoints[1][0] -= (float)tan(deg2rad(22.5f));
					controlPoints[1][2] += 1.0f;
				}
				else if (usedSegments * 8 == numSegments)
				{
					controlPoints[1][0] -= 1.0f;
					controlPoints[1][2] += 1.0f;
				}
				else if (usedSegments * 8 == numSegments * 3)
				{
					controlPoints[1][0] -= 1.0f;
					controlPoints[1][2] -= 1.0f;
				}
				else if (usedSegments * 4 == numSegments * 3)
				{
					controlPoints[1][0] += 1.0f;
				}
				else
				{
					controlPoints[1][axis2] -= 1.0f;
				}
			}
		}
		else
		{
			controlPoints[1] = points[(i + 1) * 2];
		}
		addConditionalLine(p1, controlPoints);
	}
}

void TREModel::addSlopedCylinder2Conditionals(TCVector *points,
											  int numSegments, int usedSegments)
{
	int i;
	TCVector linePoints[2];
	TCVector controlPoints[2];
	int axis2 = 2;
	TCVector *p1;
	TCVector *p2;

	if (numSegments == usedSegments * 2)
	{
		axis2 = 0;
	}
	for (i = 1; i <= usedSegments; i++)
	{
		linePoints[0] = points[i * 2];
		linePoints[1] = points[i * 2 + 1];
		p1 = &linePoints[0];
		p2 = &linePoints[1];
		if (p1 == p2)
		{
			continue;
		}
		controlPoints[0] = points[(i - 1) * 2];
		if (i == usedSegments)
		{
			if (numSegments == usedSegments)
			{
				controlPoints[1] = points[2];
			}
			else
			{
				controlPoints[1] = *p1;
				if (usedSegments * 8 == numSegments)
				{
					controlPoints[1][0] += 1.0f;
					controlPoints[1][2] -= 1.0f;
				}
				else if (usedSegments * 8 == numSegments * 3)
				{
					controlPoints[1][0] -= 1.0f;
					controlPoints[1][2] -= 1.0f;
				}
				else if (usedSegments * 4 == numSegments * 3)
				{
					controlPoints[1][2] += 1.0f;
				}
				else
				{
					controlPoints[1][axis2] -= 1.0f;
				}
			}
		}
		else
		{
			controlPoints[1] = points[(i + 1) * 2];
		}
		addConditionalLine(linePoints, controlPoints);
	}
}

void TREModel::addCircularEdge(const TCVector& center, float radius,
							   int numSegments, int usedSegments)
{
	int i;
	TCVector p1;
	TCVector *allPoints;
	TCVector points[2];

	if (usedSegments == -1)
	{
		usedSegments = numSegments;
	}
	allPoints = new TCVector[usedSegments + 1];
	for (i = 0; i <= usedSegments; i++)
	{
		float x, z;
		float angle;

		angle = 2.0f * (float)M_PI / numSegments * i;
		x = radius * (float)cos(angle);
		z = radius * (float)sin(angle);
		p1[0] = center.get(0) + x;
		p1[2] = center.get(2) + z;
		p1[1] = center.get(1);
		allPoints[i] = p1;
	}
	for (i = 0; i < usedSegments; i++)
	{
		points[0] = allPoints[i];
		points[1] = allPoints[i + 1];
		addEdgeLine(points);
	}
	delete[] allPoints;
}

void TREModel::addRing(const TCVector& center, float radius1, float radius2,
					   int numSegments, int usedSegments, bool bfc)
{
	addOpenCone(center, radius1, radius2, 0.0f, numSegments, usedSegments,
		bfc);
}

void TREModel::calculateBoundingBox(void)
{
	if (!m_flags.boundingBox)
	{
		m_boundingMin[0] = 1e10f;
		m_boundingMin[1] = 1e10f;
		m_boundingMin[2] = 1e10f;
		m_boundingMax[0] = -1e10f;
		m_boundingMax[1] = -1e10f;
		m_boundingMax[2] = -1e10f;
		scanPoints(this,
			(TREScanPointCallback)&TREModel::scanBoundingBoxPoint,
			TCVector::getIdentityMatrix());
		m_flags.boundingBox = true;
	}
}

void TREModel::scanPoints(TCObject *scanner,
						  TREScanPointCallback scanPointCallback,
						  const float *matrix)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		TREShapeGroup *shapeGroup = m_shapes[i];

		if (shapeGroup)
		{
			shapeGroup->scanPoints(scanner, scanPointCallback, matrix);
		}
		shapeGroup = m_coloredShapes[i];
		if (shapeGroup)
		{
			shapeGroup->scanPoints(scanner, scanPointCallback, matrix);
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->scanPoints(scanner, scanPointCallback, matrix);
		}
	}
}

void TREModel::unshrinkNormals(const float *matrix, const float *unshrinkMatrix)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		if (!isLineSection(i))
		{
			TREShapeGroup *shapeGroup = m_shapes[i];

			if (shapeGroup)
			{
				shapeGroup->unshrinkNormals(matrix, unshrinkMatrix);
			}
			shapeGroup = m_coloredShapes[i];
			if (shapeGroup)
			{
				shapeGroup->unshrinkNormals(matrix, unshrinkMatrix);
			}
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->unshrinkNormals(matrix, unshrinkMatrix);
		}
	}
}

void TREModel::getBoundingBox(TCVector& min, TCVector& max)
{
	if (!m_flags.boundingBox)
	{
		calculateBoundingBox();
	}
	min = m_boundingMin;
	max = m_boundingMax;
}

void TREModel::scanBoundingBoxPoint(const TCVector &point)
{
	if (point.get(0) < m_boundingMin[0])
	{
		m_boundingMin[0] = point.get(0);
	}
	if (point.get(1) < m_boundingMin[1])
	{
		m_boundingMin[1] = point.get(1);
	}
	if (point.get(2) < m_boundingMin[2])
	{
		m_boundingMin[2] = point.get(2);
	}
	if (point.get(0) > m_boundingMax[0])
	{
		m_boundingMax[0] = point.get(0);
	}
	if (point.get(1) > m_boundingMax[1])
	{
		m_boundingMax[1] = point.get(1);
	}
	if (point.get(2) > m_boundingMax[2])
	{
		m_boundingMax[2] = point.get(2);
	}
}

// When you shrink a part, all the normals end up getting lengthened by an
// amount that is based on their direction and the magnitude of the shrinkage
// matrix.  If that isn't adjusted for, then all normals have to be normalized
// by OpenGL, which slows things down.  This functions shortens all the normals
// in a part by the appropriate amount based on the shrinkage matrix.  Then when
// the part is drawn with the shrinkage matrix, they automatically get adjusted
// back to being unit length, and we don't have to force OpenGL to normalize
// them.
//
// Note: At first glance, this would appear to mess up parts that are mirror
// images of each other.  Since one part will reference the other with a mirror
// matrix, it makes it possible to shrink the normals twice.  However, since
// all parts get flattenned, and the flatenning process re-normalizes the
// normals to be unit lenght, everything is fine.  If it ever becomes desirable
// to allow parts not to be flattened, things will get more complicated.
void TREModel::unshrinkNormals(const float *scaleMatrix)
{
	// If the same part is referenced twice in a model, we'll get here twice.
	// We only want to adjust the normals once, or we'll be in trouble, so
	// record the fact that the normals have been adjusted.
	if (!m_flags.unshrunkNormals)
	{
		unshrinkNormals(TCVector::getIdentityMatrix(), scaleMatrix);
		m_flags.unshrunkNormals = true;
	}
}

void TREModel::setSectionPresent(TREMSection section, bool colored)
{
	TCULong bit = 1 << section;

	if (colored)
	{
		m_coloredSectionsPresent |= bit;
	}
	else
	{
		m_sectionsPresent |= bit;
	}
}

bool TREModel::isSectionPresent(TREMSection section, bool colored)
{
	TCULong bit = 1 << section;

	if (colored)
	{
		return (m_coloredSectionsPresent & bit) != 0;
	}
	else
	{
		return (m_sectionsPresent & bit) != 0;
	}
}

bool TREModel::checkShapeGroupPresent(TREShapeGroup *shapeGroup,
									  TREMSection section, bool colored)
{
	if (shapeGroup)
	{
		setSectionPresent(section, colored);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			if ((*m_subModels)[i]->getEffectiveModel()->
				checkSectionPresent(section, colored))
			{
				setSectionPresent(section, colored);
			}
		}
	}
	return isSectionPresent(section, colored);
}

bool TREModel::checkSectionPresent(TREMSection section, bool colored)
{
	if (colored)
	{
		return checkShapeGroupPresent(m_coloredShapes[section], section,
			true);
	}
	else
	{
		return checkShapeGroupPresent(m_shapes[section], section, false);
	}
}

bool TREModel::checkSectionPresent(TREMSection section)
{
	return checkSectionPresent(section, false);
}

bool TREModel::checkColoredSectionPresent(TREMSection section)
{
	return checkSectionPresent(section, true);
}

/*
bool TREModel::checkDefaultColorPresent(void)
{
	return checkSectionPresent(TREMStandard);
}

bool TREModel::checkDefaultColorLinesPresent(void)
{
	return checkSectionPresent(TREMLines);
}

bool TREModel::checkEdgeLinesPresent(void)
{
	return checkSectionPresent(TREMEdgeLines);
}

bool TREModel::checkConditionalLinesPresent(void)
{
	return checkSectionPresent(TREMConditionalLines);
}

bool TREModel::checkStudsPresent(void)
{
	return checkSectionPresent(TREMStud);
}

bool TREModel::checkBFCPresent(void)
{
	return checkSectionPresent(TREMBFC);
}

bool TREModel::checkColoredPresent(void)
{
	return checkColoredSectionPresent(TREMStandard);
}

bool TREModel::checkColoredBFCPresent(void)
{
	return checkColoredSectionPresent(TREMBFC);
}

bool TREModel::checkColoredLinesPresent(void)
{
	return checkColoredSectionPresent(TREMLines);
}

bool TREModel::checkColoredEdgeLinesPresent(void)
{
	return checkColoredSectionPresent(TREMEdgeLines);
}

bool TREModel::checkColoredConditionalLinesPresent(void)
{
	return checkColoredSectionPresent(TREMConditionalLines);
}
*/

void TREModel::uncompile(void)
{
	int i;

	for (i = 0; i <= TREMLast; i++)
	{
		TCULong listID = m_listIDs[i];

		if (listID)
		{
			glDeleteLists(listID, 1);
			m_listIDs[i] = 0;
		}
		listID = m_coloredListIDs[i];
		if (listID)
		{
			glDeleteLists(listID, 1);
			m_coloredListIDs[i] = 0;
		}
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->getEffectiveModel()->uncompile();
		}
	}
}

void TREModel::cleanupTransparent(TREMSection section)
{
	TREColoredShapeGroup *shapeGroup = m_coloredShapes[section];

	if (shapeGroup)
	{
		shapeGroup->cleanupTransparent();
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->getModel()->cleanupTransparent(section);
		}
	}
}

void TREModel::transferColoredTransparent(TREMSection section,
										  const float *matrix)
{
	TREColoredShapeGroup *shapeGroup = m_coloredShapes[section];

	if (shapeGroup)
	{
		shapeGroup->transferColoredTransparent(matrix);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->transferColoredTransparent(section, matrix);
		}
	}
}

void TREModel::transferTransparent(TCULong color, TREMSection section,
								   const float *matrix)
{
	TREShapeGroup *shapeGroup = m_shapes[section];

	if (shapeGroup)
	{
		shapeGroup->transferTransparent(color, matrix);
	}
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_subModels)[i]->transferTransparent(color, section, matrix);
		}
	}
}

bool TREModel::shouldLoadConditionalLines(void)
{
	return m_mainModel->shouldLoadConditionalLines();
}

//void TREModel::flattenNonUniform(TCULong color, bool colorSet, TCULong edgeColor,
//								 bool edgeColorSet)
void TREModel::flattenNonUniform(void)
{
	if (m_subModels)
	{
		int i;
		int count = m_subModels->getCount();
		float determinant;

		for (i = count - 1; i >= 0; i--)
		{
			TRESubModel *subModel = (*m_subModels)[i];
			TREModel *newModel = subModel->getEffectiveModel();

			determinant = TCVector::determinant(subModel->getOriginalMatrix());
			if (!fEq(determinant, 1.0) && !fEq(determinant, -1.0))
			{
				if (subModel->isColorSet())
				{
					flatten(newModel, subModel->getMatrix(),
						htonl(subModel->getColor()), true,
						htonl(subModel->getEdgeColor()), true, true);
				}
				else
				{
					flatten(newModel, subModel->getMatrix(), 0, false,
						0, false, true);
//					flatten(newModel, subModel->getMatrix(), color, colorSet,
//						edgeColor, edgeColorSet, true);
				}
				m_subModels->removeObject(i);
				debugPrintf("Flattened non-uniform sub-model: %g.\n",
					determinant);
			}
			else
			{
				if (subModel->isColorSet())
				{
					newModel->flattenNonUniform();
//					newModel->flattenNonUniform(htonl(subModel->getColor()),
//						true, htonl(subModel->getEdgeColor()), true);
				}
				else
				{
					newModel->flattenNonUniform();
//					newModel->flattenNonUniform(color, colorSet, edgeColor,
//						edgeColorSet);
				}
			}
		}
	}
}
