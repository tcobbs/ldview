#include "TREMainModel.h"
#include "TREVertexStore.h"
#include "TRETransShapeGroup.h"
#include "TREGL.h"
#include <math.h>

#include <TCFoundation/TCDictionary.h>

static const float POLYGON_OFFSET_FACTOR = 0.85f;
static const float POLYGON_OFFSET_UNITS = 0.0f;

TREMainModel::TREMainModel(void)
	:m_loadedModels(NULL),
	m_loadedBFCModels(NULL),
	m_vertexStore(new TREVertexStore),
	m_coloredVertexStore(new TREVertexStore),
	m_transVertexStore(NULL),
	m_transShapes(NULL),
	m_color(htonl(0x999999FF)),
	m_edgeColor(htonl(0x666658FF)),
	m_maxRadiusSquared(0.0f),
	m_transListID(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREMainModel");
#endif // _LEAK_DEBUG
	m_mainModel = this;
	m_mainFlags.compileParts = false;
	m_mainFlags.compileAll = false;
	m_mainFlags.compiled = false;
	m_mainFlags.edgeLines = false;
	m_mainFlags.twoSidedLighting = false;
	m_mainFlags.lighting = false;
	m_mainFlags.useStrips = true;
	m_mainFlags.useFlatStrips = false;
	m_mainFlags.bfc = false;
	m_mainFlags.aaLines = false;
	m_mainFlags.sortTransparent = false;
}

TREMainModel::TREMainModel(const TREMainModel &other)
	:TREModel(other),
	m_loadedModels((TCDictionary *)TCObject::copy(other.m_loadedModels)),
	m_loadedBFCModels((TCDictionary *)TCObject::copy(other.m_loadedBFCModels)),
	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
	m_coloredVertexStore((TREVertexStore *)TCObject::copy(
		other.m_coloredVertexStore)),
	m_transVertexStore((TREVertexStore *)TCObject::copy(
		other.m_transVertexStore)),
	m_mainFlags(other.m_mainFlags),
	m_transShapes((TRETransShapeGroup *)TCObject::copy(other.m_transShapes)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_transListID(0)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREMainModel");
#endif // _LEAK_DEBUG
	m_mainModel = this;
}

TREMainModel::~TREMainModel(void)
{
}

void TREMainModel::dealloc(void)
{
	TCObject::release(m_loadedModels);
	TCObject::release(m_loadedBFCModels);
	TCObject::release(m_vertexStore);
	TCObject::release(m_coloredVertexStore);
	TCObject::release(m_transVertexStore);
	TCObject::release(m_transShapes);
	if (m_transListID)
	{
		glDeleteLists(m_transListID, 1);
	}
	TREModel::dealloc();
}

TCObject *TREMainModel::copy(void)
{
	return new TREMainModel(*this);
}

TCDictionary *TREMainModel::getLoadedModels(bool bfc)
{
	if (bfc)
	{
		if (!m_loadedBFCModels)
		{
			m_loadedBFCModels = new TCDictionary(0);
		}
		return m_loadedBFCModels;
	}
	else
	{
		if (!m_loadedModels)
		{
			m_loadedModels = new TCDictionary(0);
		}
		return m_loadedModels;
	}
}

void TREMainModel::activateBFC(void)
{
	// Note that GL_BACK is the default face to cull, and GL_CCW is the
	// default polygon winding.
	glEnable(GL_CULL_FACE);
	if (getTwoSidedLightingFlag() && getLightingFlag())
	{
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	}
}

void TREMainModel::deactivateBFC(void)
{
	if (getTwoSidedLightingFlag() && getLightingFlag())
	{
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	}
	glDisable(GL_CULL_FACE);
}

void TREMainModel::compileTransparent(void)
{
	if (!m_transListID && m_transShapes)
	{
		int listID = glGenLists(1);

		glNewList(listID, GL_COMPILE);
		m_transShapes->draw(false);
		glEndList();
		m_transListID = listID;
	}
}

void TREMainModel::compile(void)
{
	if (!m_mainFlags.compiled)
	{
		m_vertexStore->activate(true);
		compileDefaultColor();
		if (getBFCFlag())
		{
			compileBFC();
		}
		compileDefaultColorLines();
		compileEdgeLines();
		m_coloredVertexStore->activate(true);
		compileColored();
		if (getBFCFlag())
		{
			compileColoredBFC();
		}
		compileColoredLines();
		compileColoredEdgeLines();
		m_transVertexStore->activate(true);
		if (!getSortTransparentFlag())
		{
			compileTransparent();
		}
		m_mainFlags.compiled = true;
	}
}

void TREMainModel::recompile(void)
{
	if (m_mainFlags.compiled)
	{
		uncompile();
		m_mainFlags.compiled = false;
	}
	compile();
}

void TREMainModel::draw(void)
{
	if (m_mainFlags.compileParts || m_mainFlags.compileAll)
	{
		compile();
	}
	if (getEdgeLinesFlag())
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(POLYGON_OFFSET_FACTOR, POLYGON_OFFSET_UNITS);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	// I admit, this is a mess.  But I'm not sure how to make it less of a mess.
	// The various things do need to be drawn separately, and they have to get
	// drawn in a specific order.
	//
	// First, draw all opaque triangles and quads that are color number 16 (the
	// default color inherited from above.  Note that the actual drawing color
	// will generally be changed before each part, since you don't usually use
	// color number 16 when you use a part in your model.
	glColor4ubv((GLubyte*)&m_color);
	m_vertexStore->activate(m_mainFlags.compileAll || m_mainFlags.compileParts);
	drawDefaultColor();
	if (getBFCFlag())
	{
		activateBFC();
		drawBFC();
	}
	// Next draw all opaque triangles and quads that were specified with a color
	// number other than 16.  Note that the colored vertex store includes color
	// information for every vertex.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	if (getBFCFlag())
	{
		drawColoredBFC();
		deactivateBFC();
	}
	drawColored();
	// Next, disable lighting and draw lines.  First draw default colored lines,
	// which probably don't exist, since color number 16 doesn't often get used
	// for lines.
	drawLines();
	drawTransparent();
}

void TREMainModel::drawLines(void)
{
	if (getLightingFlag())
	{
		glDisable(GL_LIGHTING);
	}
	if (getAALinesFlag())
	{
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Smooth lines produce odd effects on the edge of transparent surfaces
		// when depth writing is enabled, so disable.
		glDepthMask(FALSE);
	}
	glColor4ubv((GLubyte*)&m_color);
	m_vertexStore->activate(m_mainFlags.compileAll || m_mainFlags.compileParts);
	drawDefaultColorLines();
	// Next, switch to the default edge color, and draw the edge lines.  By
	// definition, edge lines in the original files use the default edge color.
	// However, if parts are flattened, they can contain sub-models of a
	// different color, which can lead to non-default colored edge lines.
	glColor4ubv((GLubyte*)&m_edgeColor);
	drawEdgeLines();
	// Next, draw the specific colored lines.  As with the specific colored
	// triangles and quads, every point in the vertex store specifies a color.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColoredLines();
	// Next draw the specific colored edge lines.  Note that if it weren't for
	// the fact that edge lines can be turned off, these could simply be added
	// to the colored lines list.
	drawColoredEdgeLines();
	if (getAALinesFlag())
	{
		glDepthMask(TRUE);
		glPopAttrib();
	}
	if (getLightingFlag())
	{
		glEnable(GL_LIGHTING);
	}
}

TREModel *TREMainModel::modelNamed(const char *name, bool bfc)
{
	return (TREMainModel *)getLoadedModels(bfc)->objectForKey(name);
}

void TREMainModel::registerModel(TREModel *model, bool bfc)
{
	getLoadedModels(bfc)->setObjectForKey(model, model->getName());
}

void TREMainModel::setColor(TCULong color, TCULong edgeColor)
{
	m_color = htonl(color);
	m_edgeColor = htonl(edgeColor);
}

TCULong TREMainModel::getColor(void)
{
	return htonl(m_color);
}

TCULong TREMainModel::getEdgeColor(void)
{
	return htonl(m_edgeColor);
}

void TREMainModel::setLightingFlag(bool value)
{
	m_mainFlags.lighting = value;
	m_vertexStore->setLightingFlag(value);
	m_coloredVertexStore->setLightingFlag(value);
	if (m_transVertexStore)
	{
		m_transVertexStore->setLightingFlag(value);
	}
}

void TREMainModel::setTwoSidedLightingFlag(bool value)
{
	m_mainFlags.twoSidedLighting = value;
	m_vertexStore->setTwoSidedLightingFlag(value);
	m_coloredVertexStore->setTwoSidedLightingFlag(value);
	if (m_transVertexStore)
	{
		m_transVertexStore->setTwoSidedLightingFlag(value);
	}
}

float TREMainModel::getMaxRadiusSquared(const TCVector &center)
{
	if (!m_maxRadiusSquared)
	{
		float identityMatrix[16];
		TCVector::initIdentityMatrix(identityMatrix);

		m_center = center;
		scanPoints(this, (TREScanPointCallback)scanMaxRadiusSquaredPoint,
			identityMatrix);
	}
	return m_maxRadiusSquared;
}

void TREMainModel::scanMaxRadiusSquaredPoint(const TCVector &point)
{
	float rSquared = (point - m_center).lengthSquared();

	if (rSquared > m_maxRadiusSquared)
	{
		m_maxRadiusSquared = rSquared;
	}
}

// By asking for the maximum radius squared, and then returning the square root
// of that, we only have to do one square root for the whole radius calculation.
// Otherwise, we would have to do one for every point.
float TREMainModel::getMaxRadius(const TCVector &center)
{
	return (float)sqrt(getMaxRadiusSquared(center));
}

void TREMainModel::postProcess(void)
{
	transferTransparent();
	checkDefaultColorPresent();
	checkBFCPresent();
	checkDefaultColorLinesPresent();
	checkEdgeLinesPresent();
	checkColoredPresent();
	checkColoredBFCPresent();
	checkColoredLinesPresent();
	checkColoredEdgeLinesPresent();
	if (getCompilePartsFlag() || getCompileAllFlag())
	{
		compile();
	}
}

// We have to remove all the transparent objects from the whole tree after we've
// completely built the tree.  Before it is completely built, we don't know
// which items are transparent.  The actual removal is complicated.
//
// First, any geometry that has a specific color specified (and thus is in a
// TREColoredShapeGroup at the point this method is called) is simply removed
// from its parent model completely, and then added to the transparent triangle
// list (after being transformed into its coordinate system).  Strips, fans,
// and quads are all split into triangles.
//
// Next, any "default color" geometry that will in the end by transparent will
// be transferred to the triangle list (with coordinate transformation and
// strip/quad splitting).  When the non-transparent portions are drawn, they
// will completely skip default colored geomtry when the default color is
// transparent.
void TREMainModel::transferTransparent(void)
{
	float identityMatrix[16];

	TCVector::initIdentityMatrix(identityMatrix);
	transferColoredTransparent(TREMStandard, identityMatrix);
	TREModel::transferTransparent(m_color, TREMStandard, identityMatrix);
	if (getBFCFlag())
	{
		transferColoredTransparent(TREMBFC, identityMatrix);
		TREModel::transferTransparent(m_color, TREMBFC, identityMatrix);
	}
}

void TREMainModel::addTransparentTriangle(TCULong color,
										  const TCVector vertices[],
										  const TCVector normals[])
{
	if (!m_transVertexStore)
	{
		m_transVertexStore = new TREVertexStore;
		m_transVertexStore->setLightingFlag(getLightingFlag());
		m_transVertexStore->setTwoSidedLightingFlag(getTwoSidedLightingFlag());
	}
	if (!m_transShapes)
	{
		m_transShapes = new TRETransShapeGroup;
		m_transShapes->setVertexStore(m_transVertexStore);
	}
	m_transShapes->addTriangle(color, vertices, normals);
}

void TREMainModel::drawTransparent(void)
{
	if (m_transShapes)
	{
//		float specular[] = {0.75f, 0.75f, 0.75f, 1.0f};

		m_transVertexStore->activate(!m_mainFlags.sortTransparent);
		if (false /*stipple*/)
		{
			glEnable(GL_POLYGON_STIPPLE);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (true /*!drawWireframe*/)
			{
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
			}
			glDepthMask(FALSE);
		}
//		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		if (true /*!wireframe*/)
		{
			if (getEdgeLinesFlag())
			{
				glPolygonOffset(0.0f, 0.0f);
			}
			else
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(-POLYGON_OFFSET_FACTOR, -POLYGON_OFFSET_UNITS);
			}
		}
		if (m_transListID)
		{
			glCallList(m_transListID);
		}
		else
		{
			m_transShapes->draw(m_mainFlags.sortTransparent);
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, TGLShape::getNormalSpecular());
//		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
//			TGLShape::getNormalShininess());
		if (false /*stipple*/)
		{
			glDisable(GL_POLYGON_STIPPLE);
		}
		else if (true /*!cutawayDraw*/)
		{
			glDisable(GL_BLEND);
			if (true /*!drawWireframe*/)
			{
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}
			glDepthMask(TRUE);
		}
	}
}
