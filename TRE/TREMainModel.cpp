#include "TREMainModel.h"
#include "TREVertexStore.h"
#include "TRETransShapeGroup.h"
#include "TREGL.h"
#include <math.h>
#include <string.h>

#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCProgressAlert.h>

//const float POLYGON_OFFSET_FACTOR = 0.85f;
//const float POLYGON_OFFSET_UNITS = 0.0f;
const float POLYGON_OFFSET_FACTOR = 1.0f;
const float POLYGON_OFFSET_UNITS = 1.0f;

TCImageArray *TREMainModel::sm_studTextures = NULL;
unsigned TREMainModel::sm_studTextureID = 0;
TREMainModel::TREMainModelCleanup TREMainModel::sm_mainModelCleanup;

TREMainModel::TREMainModelCleanup::~TREMainModelCleanup(void)
{
	TCObject::release(TREMainModel::sm_studTextures);
	TREMainModel::sm_studTextures = NULL;
	if (TREMainModel::sm_studTextureID)
	{
		glDeleteTextures(1, &TREMainModel::sm_studTextureID);
	}
}

TREMainModel::TREMainModel(void)
	:m_loadedModels(NULL),
	m_loadedBFCModels(NULL),
	m_vertexStore(new TREVertexStore),
	m_studVertexStore(new TREVertexStore),
	m_coloredVertexStore(new TREVertexStore),
	m_coloredStudVertexStore(new TREVertexStore),
	m_transVertexStore(new TREVertexStore),
	m_color(htonl(0x999999FF)),
	m_edgeColor(htonl(0x666658FF)),
	m_maxRadiusSquared(0.0f),
	m_edgeLineWidth(1.0f),
	m_abort(false),
	m_studTextureFilter(GL_LINEAR_MIPMAP_LINEAR)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TREMainModel");
#endif // _LEAK_DEBUG
	m_mainModel = this;
	m_mainFlags.compiled = false;
	m_mainFlags.compiling = false;
	m_mainFlags.removingHiddenLines = false;
	m_mainFlags.cutawayDraw = false;
	m_mainFlags.activeLineJoins = false;

	m_mainFlags.compileParts = false;
	m_mainFlags.compileAll = false;
	m_mainFlags.edgeLines = false;
	m_mainFlags.edgesOnly = false;
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
	m_mainFlags.smoothCurves = true;
	m_mainFlags.showAllConditional = false;
	m_mainFlags.conditionalControlPoints = false;
	m_mainFlags.studLogo = true;
	m_mainFlags.redBackFaces = false;
	m_mainFlags.lineJoins = false;	// Doesn't work right
}

TREMainModel::TREMainModel(const TREMainModel &other)
	:TREModel(other),
	m_loadedModels((TCDictionary *)TCObject::copy(other.m_loadedModels)),
	m_loadedBFCModels((TCDictionary *)TCObject::copy(other.m_loadedBFCModels)),
	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
	m_studVertexStore((TREVertexStore *)TCObject::copy(
		other.m_studVertexStore)),
	m_coloredVertexStore((TREVertexStore *)TCObject::copy(
		other.m_coloredVertexStore)),
	m_coloredStudVertexStore((TREVertexStore *)TCObject::copy(
		other.m_coloredStudVertexStore)),
	m_transVertexStore((TREVertexStore *)TCObject::copy(
		other.m_transVertexStore)),
	m_color(other.m_color),
	m_edgeColor(other.m_edgeColor),
	m_maxRadiusSquared(other.m_maxRadiusSquared),
	m_edgeLineWidth(other.m_edgeLineWidth),
	m_abort(false),
	m_studTextureFilter(other.m_studTextureFilter),
	m_mainFlags(other.m_mainFlags)
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
	TCObject::release(m_studVertexStore);
	TCObject::release(m_coloredVertexStore);
	TCObject::release(m_coloredStudVertexStore);
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
	if (getRedBackFacesFlag())
	{
		float mRed[] = {1.0f, 0.0f, 0.0f, 1.0f};

		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
//		glMaterialfv(GL_BACK, GL_AMBIENT, mRed);
		glMaterialfv(GL_BACK, GL_DIFFUSE, mRed);
	}
	else
	{
		// Note that GL_BACK is the default face to cull, and GL_CCW is the
		// default polygon winding.
		glEnable(GL_CULL_FACE);
		if (getTwoSidedLightingFlag() && getLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
		}
	}
}

void TREMainModel::deactivateBFC(void)
{
	if (getRedBackFacesFlag())
	{
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	}
	else
	{
		if (getTwoSidedLightingFlag() && getLightingFlag())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
		}
		glDisable(GL_CULL_FACE);
	}
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

bool TREMainModel::shouldCompileSection(TREMSection section)
{
	return section != TREMConditionalLines && section != TREMTransparent;
}

void TREMainModel::compile(void)
{
	if (!m_mainFlags.compiled)
	{
		int i;
		float numSections = (float)(TREMLast - TREMFirst + 1);

//		TCProgressAlert::send("TREMainModel", "Compiling...", 0.0f, &m_abort);
		if (!m_abort)
		{
			m_mainFlags.compiling = true;
			for (i = TREMFirst; i <= TREMLast && !m_abort; i++)
			{
				TREMSection section = (TREMSection)i;

				if (shouldCompileSection(section))
				{
					float progress = (float)i / (numSections * 2.0f);

					TCProgressAlert::send("TREMainModel", "Compiling...",
						progress, &m_abort);
					if (!m_abort && isSectionPresent(section, false))
					{
						if (isStudSection(section))
						{
							m_studVertexStore->activate(true);
						}
						else
						{
							m_vertexStore->activate(true);
						}
						if (section == TREMEdgeLines)
						{
							if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
							{
//								glPointSize(m_edgeLineWidth);
								m_mainFlags.activeLineJoins = true;
//								glEnable(GL_POINT_SMOOTH);
//								glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
							}
						}
						TREModel::compile(section, false);
						m_mainFlags.activeLineJoins = false;
					}
				}
			}
/*
			TREModel::compile(TREMStandard, false);
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.2f,
				&m_abort);
*/
		}
/*
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				TREModel::compile(TREMBFC, false);
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.3f,
				&m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMLines, false);
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.35f,
				&m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMEdgeLines, false);
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.45f,
				&m_abort);
		}
*/
		if (!m_abort)
		{
			for (i = TREMFirst; i <= TREMLast && !m_abort; i++)
			{
				TREMSection section = (TREMSection)i;

				if (shouldCompileSection(section))
				{
					float progress = (float)i / (numSections * 2.0f) + 0.5f;

					TCProgressAlert::send("TREMainModel", "Compiling...",
						progress, &m_abort);
					if (!m_abort && isSectionPresent(section, true))
					{
						if (isStudSection(section))
						{
							m_coloredStudVertexStore->activate(true);
						}
						else
						{
							m_coloredVertexStore->activate(true);
						}
						TREModel::compile(section, true);
					}
				}
			}
/*
			TREModel::compile(TREMStandard, true);
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.55f,
				&m_abort);
*/
		}
/*
		if (!m_abort)
		{
			if (getBFCFlag())
			{
				TREModel::compile(TREMBFC, true);
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.65f,
				&m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMBFC, true);
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.7f,
				&m_abort);
		}
		if (!m_abort)
		{
			TREModel::compile(TREMEdgeLines, true);
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
				TREModel::compile(TREMTransparent, true);
			}
			TCProgressAlert::send("TREMainModel", "Compiling...", 0.9f,
				&m_abort);
		}
*/
		if (!m_abort)
		{
			m_mainFlags.compiled = true;
			m_mainFlags.compiling = false;
			TCProgressAlert::send("TREMainModel", "Compiling...", 1.0f,
				&m_abort);
//			TCProgressAlert::send("LDrawModelViewer", "Done.", 2.0f);
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
	bool subModelsOnly = false;

	if (TREShapeGroup::isTransparent(m_color, true))
	{
		subModelsOnly = true;
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
	TREModel::draw(TREMStandard, false, subModelsOnly);
	if (getStudLogoFlag())
	{
		glEnable(GL_TEXTURE_2D);
		bindStudTexture();
		configureStudTexture();
		m_studVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		TREModel::draw(TREMStud, false, subModelsOnly);
	}
	if (getBFCFlag())
	{
		activateBFC();
		if (getStudLogoFlag())
		{
			TREModel::draw(TREMStudBFC, false, subModelsOnly);
			glDisable(GL_TEXTURE_2D);
			m_vertexStore->activate(m_mainFlags.compileAll ||
				m_mainFlags.compileParts);
		}
		TREModel::draw(TREMBFC, false, subModelsOnly);
	}
	else if (getStudLogoFlag())
	{
		glDisable(GL_TEXTURE_2D);
	}
	// Next draw all opaque triangles and quads that were specified with a color
	// number other than 16.  Note that the colored vertex store includes color
	// information for every vertex.
	if (getBFCFlag())
	{
		m_coloredVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		drawColored(TREMBFC);
		if (getStudLogoFlag())
		{
			glEnable(GL_TEXTURE_2D);
			m_coloredStudVertexStore->activate(m_mainFlags.compileAll ||
				m_mainFlags.compileParts);
			drawColored(TREMStudBFC);
			glDisable(GL_TEXTURE_2D);
		}
		deactivateBFC();
	}
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColored(TREMStandard);
	if (getStudLogoFlag())
	{
		glEnable(GL_TEXTURE_2D);
		m_coloredStudVertexStore->activate(m_mainFlags.compileAll ||
			m_mainFlags.compileParts);
		drawColored(TREMStud);
		glDisable(GL_TEXTURE_2D);
	}
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
		glDepthMask(GL_FALSE);
	}
	glColor4ubv((GLubyte*)&m_color);
	m_vertexStore->activate(m_mainFlags.compileAll || m_mainFlags.compileParts);
	TREModel::draw(TREMLines);
	// Next, switch to the default edge color, and draw the edge lines.  By
	// definition, edge lines in the original files use the default edge color.
	// However, if parts are flattened, they can contain sub-models of a
	// different color, which can lead to non-default colored edge lines.
	glColor4ubv((GLubyte*)&m_edgeColor);
	glLineWidth(m_edgeLineWidth);
	if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
	{
		glPointSize(m_edgeLineWidth);
		m_mainFlags.activeLineJoins = true;
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	TREModel::draw(TREMEdgeLines);
	m_vertexStore->deactivate();
	m_vertexStore->activate(false);
	if (getConditionalLinesFlag())
	{
		TREModel::draw(TREMConditionalLines);
	}
	if (getLineJoinsFlag() && m_edgeLineWidth > 1.0f)
	{
		glPointSize(1.0f);
		m_mainFlags.activeLineJoins = false;
	}
	// Next, draw the specific colored lines.  As with the specific colored
	// triangles and quads, every point in the vertex store specifies a color.
	m_coloredVertexStore->activate(m_mainFlags.compileAll ||
		m_mainFlags.compileParts);
	drawColored(TREMLines);
	// Next draw the specific colored edge lines.  Note that if it weren't for
	// the fact that edge lines can be turned off, these could simply be added
	// to the colored lines list.
	drawColored(TREMEdgeLines);
	m_coloredVertexStore->deactivate();
	m_coloredVertexStore->activate(false);
	if (getConditionalLinesFlag())
	{
		drawColored(TREMConditionalLines);
	}
	if (getAALinesFlag() && !getWireframeFlag())
	{
		// Note that if we're in wireframe mode, smoothing was enabled
		// elsewhere, and will therefore be disabled eleswhere.
		glDepthMask(GL_TRUE);
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
	m_studVertexStore->setLightingFlag(value);
	m_coloredVertexStore->setLightingFlag(value);
	m_coloredStudVertexStore->setLightingFlag(value);
	m_transVertexStore->setLightingFlag(value);
}

void TREMainModel::setTwoSidedLightingFlag(bool value)
{
	m_mainFlags.twoSidedLighting = value;
	m_vertexStore->setTwoSidedLightingFlag(value);
	m_studVertexStore->setTwoSidedLightingFlag(value);
	m_coloredVertexStore->setTwoSidedLightingFlag(value);
	m_coloredStudVertexStore->setTwoSidedLightingFlag(value);
	m_transVertexStore->setTwoSidedLightingFlag(value);
}

void TREMainModel::setShowAllConditionalFlag(bool value)
{
	m_mainFlags.showAllConditional = value;
	m_vertexStore->setShowAllConditionalFlag(value);
	m_studVertexStore->setShowAllConditionalFlag(value);
	m_coloredVertexStore->setShowAllConditionalFlag(value);
	m_coloredStudVertexStore->setShowAllConditionalFlag(value);
	m_transVertexStore->setShowAllConditionalFlag(value);
}

void TREMainModel::setConditionalControlPointsFlag(bool value)
{
	m_mainFlags.conditionalControlPoints = value;
	m_vertexStore->setConditionalControlPointsFlag(value);
	m_studVertexStore->setConditionalControlPointsFlag(value);
	m_coloredVertexStore->setConditionalControlPointsFlag(value);
	m_coloredStudVertexStore->setConditionalControlPointsFlag(value);
	m_transVertexStore->setConditionalControlPointsFlag(value);
}

float TREMainModel::getMaxRadiusSquared(const TCVector &center)
{
	if (!m_maxRadiusSquared)
	{
		float identityMatrix[16];
		TCVector::initIdentityMatrix(identityMatrix);

		m_center = center;
		scanPoints(this,
			(TREScanPointCallback)&TREMainModel::scanMaxRadiusSquaredPoint,
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
	int i;
	float numSections = (float)(TREMTransparent - TREMStandard);

	TCProgressAlert::send("TREMainModel", "Processing...", 0.0f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	transferTransparent();
	// Note: I DON'T want to check if the transparent section is present.
	// That's why I'm using < below, instead of <=.
	for (i = TREMFirst; i < TREMTransparent && !m_abort; i++)
	{
		float progress = (float)i / numSections * 0.8f + 0.2f;

		TCProgressAlert::send("TREMainModel", "Processing...", progress,
			&m_abort);
		if (!m_abort)
		{
			checkSectionPresent((TREMSection)i);
			checkSectionPresent((TREMSection)i, true);
		}
	}
	if (m_abort)
	{
		return false;
	}
/*
	checkDefaultColorPresent();
	TCProgressAlert::send("TREMainModel", "Processing...", 0.3f, &m_abort);
	if (m_abort)
	{
		return false;
	}
	checkStudsPresent();
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
*/
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
	TREModel::transferTransparent(m_color, TREMStandard, identityMatrix);
	TREModel::transferTransparent(m_color, TREMStud, identityMatrix);
	transferColoredTransparent(TREMStandard, identityMatrix);
	transferColoredTransparent(TREMStud, identityMatrix);
	if (getBFCFlag())
	{
		TREModel::transferTransparent(m_color, TREMBFC, identityMatrix);
		TREModel::transferTransparent(m_color, TREMStudBFC, identityMatrix);
		transferColoredTransparent(TREMBFC, identityMatrix);
		transferColoredTransparent(TREMStudBFC, identityMatrix);
	}
}

// This should really be modified to work in the sub-models, so that if sorting
// isn't enabled, transparent bits get drawn as just another standard pass,
// instead of all being thrown into the main model.
void TREMainModel::addTransparentTriangle(TCULong color,
										  const TCVector vertices[],
										  const TCVector normals[])
{
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
			glDepthMask(GL_FALSE);
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
			glDepthMask(GL_TRUE);
		}
	}
}

bool TREMainModel::shouldLoadConditionalLines(void)
{
	return (m_mainFlags.edgeLines && m_mainFlags.conditionalLines) ||
		m_mainFlags.smoothCurves;
}

void TREMainModel::bindStudTexture(void)
{
	if (!sm_studTextureID && sm_studTextures)
	{
		int i;
		int count = sm_studTextures->getCount();

		glGenTextures(1, &sm_studTextureID);
		glBindTexture(GL_TEXTURE_2D, sm_studTextureID);
		for (i = 0; i < count; i++)
		{
			TCImage *texture = (*sm_studTextures)[i];

			if (texture)
			{
				int textureSize = texture->getWidth();

				glTexImage2D(GL_TEXTURE_2D, i, 4, textureSize, textureSize, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, texture->getImageData());
			}
		}
	}
}

void TREMainModel::configureStudTexture(void)
{
	if (sm_studTextureID)
	{
		glBindTexture(GL_TEXTURE_2D, sm_studTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		if (m_studTextureFilter == GL_NEAREST_MIPMAP_NEAREST ||
			m_studTextureFilter == GL_NEAREST_MIPMAP_LINEAR ||
			m_studTextureFilter == GL_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			m_studTextureFilter);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
}

// NOTE: static function
void TREMainModel::loadStudTexture(const char *filename)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		if (mainImage->loadFile(filename))
		{
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}

// NOTE: static function
void TREMainModel::loadStudMipTextures(TCImage *mainImage)
{
	int i;
	int yPosition = 0;
	int thisSize = mainImage->getWidth();
	int imageHeight = mainImage->getHeight();

	TCObject::release(sm_studTextures);
	sm_studTextures = new TCImageArray(8);
	for (i = 0; thisSize > 0 && yPosition + thisSize <= imageHeight; i++)
	{
		TCImage *texture = mainImage->createSubImage(0, yPosition,
			thisSize, thisSize);
		sm_studTextures->addObject(texture);
		texture->release();
		yPosition += thisSize;
		thisSize /= 2;
	}
}

// NOTE: static function
void TREMainModel::setStudTextureData(TCByte *data, long length)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		if (mainImage->loadData(data, length))
		{
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}

// NOTE: static function
void TREMainModel::setRawStudTextureData(TCByte *data, long length)
{
	if (!sm_studTextures)
	{
		TCImage *mainImage = new TCImage;
		int rowSize;

		mainImage->setFlipped(true);
		mainImage->setLineAlignment(4);
		mainImage->setDataFormat(TCRgba8);
		mainImage->setSize(128, 255);
		mainImage->allocateImageData();
		rowSize = mainImage->getRowSize();
		if (length == rowSize * 255)
		{
			TCByte *imageData = mainImage->getImageData();
			int i;

			for (i = 0; i < 255; i++)
			{
				memcpy(imageData + rowSize * (254 - i), data + rowSize * i,
					rowSize);
			}
			loadStudMipTextures(mainImage);
		}
		mainImage->release();
	}
}
