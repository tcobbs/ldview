#include "TREMainModel.h"
#include "TREVertexStore.h"
#include "TRETransShapeGroup.h"
#include "TREGL.h"
#include <math.h>

#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCProgressAlert.h>

const float POLYGON_OFFSET_FACTOR = 0.85f;
const float POLYGON_OFFSET_UNITS = 0.0f;

TREMainModel::TREMainModel(void)
	:m_loadedModels(NULL),
	m_loadedBFCModels(NULL),
	m_vertexStore(new TREVertexStore),
	m_coloredVertexStore(new TREVertexStore),
	m_transVertexStore(NULL),
	m_color(htonl(0x999999FF)),
	m_edgeColor(htonl(0x666658FF)),
	m_maxRadiusSquared(0.0f),
	m_edgeLineWidth(1.0f),
	m_abort(false)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREMainModel");
#endif // _LEAK_DEBUG
	m_mainModel = this;
	m_mainFlags.compiled = false;
	m_mainFlags.compiling = false;
	m_mainFlags.removingHiddenLines = false;
	m_mainFlags.cutawayDraw = false;

	m_mainFlags.compileParts = false;
	m_mainFlags.compileAll = false;
	m_mainFlags.edgeLines = false;
	m_mainFlags.twoSidedLighting = false;
	m_mainFlags.lighting = false;
	m_mainFlags.useStrips = true;
	m_mainFlags.useFlatStrips = false;
	m_mainFlags.bfc = false;
	m_mainFlags.aaLines = false;
	m_mainFlags.sortTransparent = false;
	m_mainFlags.stipple = false;
	m_mainFlags.wireframe = false;
	m_mainFlags.conditionalLines = false;
	m_mainFlags.smoothCurves = false;
	m_mainFlags.showAllConditional = false;
	m_mainFlags.conditionalControlPoints = false;
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
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor)
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

/*
void TREMainModel::compileTransparent(void)
{
	if (!m_coloredListIDs[TREMTransparent] &&
		m_coloredShapes[TREMTransparent])
	{
		int listID = glGenLists(1);

		glNewList(listID, GL_COMPILE);
		((TRETransShapeGroup *)m_coloredShapes[TREMTransparent])->
			draw(false);
		glEndList();
		m_coloredListIDs[TREMTransparent] = listID;
	}
}
*/

void TREMainModel::compile(void)
{
	if (!m_mainFlags.compiled)
	{
		TCProgressAlert::send("TREMainModel", "Compiling...", 0.0f, &m_abort);
		if (!m_abort)
		{
			m_mainFlags.compiling = true;
			m_vertexStore->activate(true);
			compileDefaultColor();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.2f,
				&m_abort);
		}
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				compileBFC();
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.3f,
				&m_abort);
		}
		if (!m_abort)
		{
			compileDefaultColorLines();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.35f,
				&m_abort);
		}
		if (!m_abort)
		{
			compileEdgeLines();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.45f,
				&m_abort);
		}
		if (!m_abort)
		{
			m_coloredVertexStore->activate(true);
			compileColored();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.55f,
				&m_abort);
		}
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				compileColoredBFC();
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.65f,
				&m_abort);
		}
		if (!m_abort)
		{
			compileColoredLines();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.7f,
				&m_abort);
		}
		if (!m_abort)
		{
			compileColoredEdgeLines();
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.8f,
				&m_abort);
		}
		if (!m_abort)
		{
			if (m_transVertexStore)
			{
				m_transVertexStore->activate(true);
			}
			if (!getSortTransparentFlag())
			{
				compileTransparent();
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.9f,
				&m_abort);
		}
		if (!m_abort)
		{
			m_mainFlags.compiled = true;
			m_mainFlags.compiling = false;
			TCProgressAlert::send("TREMainModel", "Compiling...", 1.0f,
				&m_abort);
		}
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
	float normalSpecular[4];

	glGetLightfv(GL_LIGHT0, GL_SPECULAR, normalSpecular);
	if (m_mainFlags.compileParts || m_mainFlags.compileAll)
	{
		compile();
	}
	if (getEdgeLinesFlag() && !getWireframeFlag() && getPolygonOffsetFlag())
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(POLYGON_OFFSET_FACTOR, POLYGON_OFFSET_UNITS);
	}
	if (getWireframeFlag())
	{
		enableLineSmooth();
	}
	if (getEdgesOnlyFlag())
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	if (!getEdgesOnlyFlag() || !getWireframeFlag())
	{
		drawSolid();
	}
	if (getEdgesOnlyFlag())
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	// Next, disable lighting and draw lines.  First draw default colored lines,
	// which probably don't exist, since color number 16 doesn't often get used
	// for lines.
	drawLines();
	if (getAALinesFlag() && getWireframeFlag())
	{
		// We use glPushAttrib() when we enable line smoothing.
		glPopAttrib();
	}
	if (!getEdgesOnlyFlag() && !getRemovingHiddenLines())
	{
		drawTransparent();
	}
}

void TREMainModel::enableLineSmooth(void)
{
	if (getAALinesFlag())
	{
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void TREMainModel::drawSolid(void)
{
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
}

void TREMainModel::drawLines(void)
{
	if (getLightingFlag())
	{
		glDisable(GL_LIGHTING);
	}
	if (getAALinesFlag() && !getWireframeFlag())
	{
		// Note that if we're in wireframe mode, smoothing is already enabled.
		enableLineSmooth();
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
	glLineWidth(m_edgeLineWidth);
	drawEdgeLines();
	m_vertexStore->deactivate();
	m_vertexStore->activate(false);
	drawConditionalLines();
	// Next, draw the specific colored lines.  As with the specific colored
	// triangles and quads, every point in the vertex store specifies a color.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColoredLines();
	// Next draw the specific colored edge lines.  Note that if it weren't for
	// the fact that edge lines can be turned off, these could simply be added
	// to the colored lines list.
	drawColoredEdgeLines();
	m_coloredVertexStore->deactivate();
	m_coloredVertexStore->activate(false);
	drawColoredConditionalLines();
	if (getAALinesFlag() && !getWireframeFlag())
	{
		// Note that if we're in wireframe mode, smoothing was enabled
		// elsewhere, and will therefore be disabled eleswhere.
		glDepthMask(TRUE);
		// We use glPushAttrib() when we enable line smoothing.
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

void TREMainModel::setShowAllConditionalFlag(bool value)
{
	m_mainFlags.showAllConditional = value;
	m_vertexStore->setShowAllConditionalFlag(value);
	m_coloredVertexStore->setShowAllConditionalFlag(value);
	if (m_transVertexStore)
	{
		m_transVertexStore->setShowAllConditionalFlag(value);
	}
}

void TREMainModel::setConditionalControlPointsFlag(bool value)
{
	m_mainFlags.conditionalControlPoints = value;
	m_vertexStore->setConditionalControlPointsFlag(value);
	m_coloredVertexStore->setConditionalControlPointsFlag(value);
	if (m_transVertexStore)
	{
		m_transVertexStore->setConditionalControlPointsFlag(value);
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

bool TREMainModel::postProcess(void)
{
	TCProgressAlert::send("TREMainModel", "Processing...", 0.0f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	transferTransparent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.2f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkDefaultColorPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.3f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkBFCPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.4f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkDefaultColorLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.45f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkEdgeLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.55f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkConditionalLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.65f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.7f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredBFCPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.75f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.8f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredEdgeLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.9f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkColoredConditionalLinesPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 1.0f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	if (getCompilePartsFlag() || getCompileAllFlag())
	{
		compile();
	}
	return !m_abort;
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

// This should really be modified to work in the sub-models, so that if sorting
// isn't enabled, transparent bits get drawn as just another standard pass,
// instead of all being thrown into the main model.
void TREMainModel::addTransparentTriangle(TCULong color,
										  const TCVector vertices[],
										  const TCVector normals[])
{
	if (!m_transVertexStore)
	{
		m_transVertexStore = new TREVertexStore;
		m_transVertexStore->setLightingFlag(getLightingFlag());
		m_transVertexStore->setTwoSidedLightingFlag(getTwoSidedLightingFlag());
		m_transVertexStore->setShowAllConditionalFlag(
			getShowAllConditionalFlag());
		m_transVertexStore->setConditionalControlPointsFlag(
			getConditionalControlPointsFlag());
	}
	if (!m_coloredShapes[TREMTransparent])
	{
		m_coloredShapes[TREMTransparent] = new TRETransShapeGroup;
		m_coloredShapes[TREMTransparent]->setVertexStore(m_transVertexStore);
	}
	m_coloredShapes[TREMTransparent]->addTriangle(color, vertices, normals);
}

void TREMainModel::drawTransparent(void)
{
	if (m_coloredShapes[TREMTransparent])
	{
		float specular[] = {0.75f, 0.75f, 0.75f, 1.0f};
		float oldSpecular[4];
		float oldShininess;

		glGetMaterialfv(GL_FRONT, GL_SHININESS, &oldShininess);
		glGetMaterialfv(GL_FRONT, GL_SPECULAR, oldSpecular);
		m_transVertexStore->activate(!m_mainFlags.sortTransparent);
		if (getStippleFlag())
		{
			glEnable(GL_POLYGON_STIPPLE);
		}
		else if (!getCutawayDrawFlag())
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (!getWireframeFlag())
			{
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
			}
			glDepthMask(FALSE);
		}
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		if (!getWireframeFlag() && getPolygonOffsetFlag())
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
		if (m_coloredListIDs[TREMTransparent])
		{
			glCallList(m_coloredListIDs[TREMTransparent]);
		}
		else
		{
			((TRETransShapeGroup *)m_coloredShapes[TREMTransparent])->
				draw(getSortTransparentFlag() && !getCutawayDrawFlag());
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, oldSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, oldShininess);
		if (getStippleFlag())
		{
			glDisable(GL_POLYGON_STIPPLE);
		}
		else if (!getCutawayDrawFlag())
		{
			glDisable(GL_BLEND);
			if (!getWireframeFlag())
			{
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}
			glDepthMask(TRUE);
		}
	}
}

bool TREMainModel::shouldLoadConditionalLines(void)
{
	return (m_mainFlags.edgeLines && m_mainFlags.conditionalLines) ||
		m_mainFlags.smoothCurves;
}
