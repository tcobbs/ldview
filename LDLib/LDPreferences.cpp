#include "LDPreferences.h"
#include "LDUserDefaultsKeys.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCWebClient.h>
#include <LDLoader/LDLPalette.h>

LDPreferences::LDPreferences(LDrawModelViewer* modelViewer)
	:modelViewer(modelViewer ? ((LDrawModelViewer*)modelViewer->retain()) :
	NULL)
{
	int i;

/*
	if (modelViewer == NULL)
	{
		throw "modelViewer must not be NULL";
	}
*/
	m_globalSettings[ZOOM_MAX_KEY] = true;
	m_globalSettings[SHOW_ERRORS_KEY] = true;
	m_globalSettings[PROXY_TYPE_KEY] = true;
	m_globalSettings[PROXY_SERVER_KEY] = true;
	m_globalSettings[PROXY_PORT_KEY] = true;
	m_globalSettings[CHECK_PART_TRACKER_KEY] = true;
	m_globalSettings[CHECK_PART_WAIT_KEY] = true;
	m_globalSettings[CHECK_PART_UPDATE_WAIT_KEY] = true;
	m_globalSettings[CAMERA_GLOBE_KEY] = true;
	m_globalSettings[INV_SHOW_MODEL_KEY] = true;
	m_globalSettings[INV_EXTERNAL_CSS_KEY] = true;
	m_globalSettings[INV_PART_IMAGES_KEY] = true;
	m_globalSettings[INV_COLUMN_ORDER_KEY] = true;
	m_globalSettings[INV_LAST_SAVE_PATH_KEY] = true;
	m_defaultColorNumber = -1;
	for (i = 0; i < 16; i++)
	{
		char key[128];

		sprintf(key, "%s/Color%02d", CUSTOM_COLORS_KEY, i);
		m_globalSettings[key] = true;
	}
	if (modelViewer)
	{
		modelViewer->setPreferences(this);
	}
}

LDPreferences::~LDPreferences(void)
{
}

void LDPreferences::dealloc(void)
{
	TCObject::release(modelViewer);
	TCObject::dealloc();
}

int LDPreferences::getColor(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

void LDPreferences::getRGB(int color, int &r, int &g, int &b)
{
	// Colors are stored in RGB format.
	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
}

void LDPreferences::getColorSetting(const char *key, int &r, int &g, int &b,
	TCULong defaultColor)
{
	long value = getLongSetting(key, defaultColor);

	getRGB(value, r, g, b);
}

void LDPreferences::applySettings(void)
{
	applyGeneralSettings();
	applyGeometrySettings();
	applyEffectsSettings();
	applyPrimitivesSettings();
	applyUpdatesSettings();
	if (modelViewer)
	{
		modelViewer->setZoomMax(m_zoomMax);
		modelViewer->setLightVector(m_lightVector);
		modelViewer->setDistanceMultiplier(1.0f / m_defaultZoom);
	}
	setupDefaultRotationMatrix();
	setupModelCenter();
	setupModelSize();
}

void LDPreferences::applyGeneralSettings(void)
{
	if (modelViewer)
	{
		int r, g, b;

		// FSAA taken care of automatically.
		getRGB(m_backgroundColor, r, g, b);
		modelViewer->setBackgroundRGBA(r, g, b, 0);
		getRGB(m_defaultColor, r, g, b);
		modelViewer->setProcessLDConfig(m_processLdConfig);
		modelViewer->setSkipValidation(m_skipValidation);
		// showFrameRate taken care of automatically.
		// showErrors taken care of automatically.
		// fullScreenRefresh taken care of automatically.
		modelViewer->setFov(m_fov);
		modelViewer->setDefaultRGB((TCByte)r, (TCByte)g, (TCByte)b,
			m_transDefaultColor);
		modelViewer->setDefaultColorNumber(m_defaultColorNumber);
		modelViewer->setLineSmoothing(m_lineSmoothing);
		modelViewer->setMemoryUsage(m_memoryUsage);
	}
}

void LDPreferences::applyGeometrySettings(void)
{
	if (modelViewer)
	{
		if (m_useSeams)
		{
			modelViewer->setSeamWidth(m_seamWidth / 100.0f);
		}
		else
		{
			modelViewer->setSeamWidth(0.0f);
		}
		modelViewer->setDrawWireframe(m_drawWireframe);
		modelViewer->setUseWireframeFog(m_useWireframeFog);
		modelViewer->setRemoveHiddenLines(m_removeHiddenLines);
		modelViewer->setWireframeLineWidth((GLfloat)m_wireframeThickness);
		modelViewer->setBfc(m_bfc);
		modelViewer->setRedBackFaces(m_redBackFaces);
		modelViewer->setGreenFrontFaces(m_greenFrontFaces);
		modelViewer->setShowsHighlightLines(m_showHighlightLines);
		modelViewer->setEdgesOnly(m_edgesOnly);
		modelViewer->setDrawConditionalHighlights(m_drawConditionalHighlights);
		modelViewer->setShowAllConditionalLines(m_showAllConditionalLines);
		modelViewer->setShowConditionalControlPoints(
			m_showConditionalControlPoints);
		modelViewer->setUsePolygonOffset(m_usePolygonOffset);
		modelViewer->setBlackHighlights(m_blackHighlights);
		modelViewer->setHighlightLineWidth((GLfloat)m_edgeThickness);
	}
}

void LDPreferences::applyEffectsSettings(void)
{
	if (modelViewer)
	{
		modelViewer->setUseLighting(m_useLighting);
		modelViewer->setQualityLighting(m_qualityLighting);
		modelViewer->setSubduedLighting(m_subduedLighting);
		modelViewer->setUsesSpecular(m_useSpecular);
		modelViewer->setOneLight(m_oneLight);
		modelViewer->setStereoMode(m_stereoMode);
		modelViewer->setStereoEyeSpacing((GLfloat)m_stereoEyeSpacing);
		modelViewer->setCutawayMode(m_cutawayMode);
		modelViewer->setCutawayAlpha((TCFloat32)m_cutawayAlpha / 100.0f);
		modelViewer->setCutawayLineWidth((TCFloat32)m_cutawayThickness);
		modelViewer->setSortTransparent(m_sortTransparent);
		modelViewer->setUseStipple(m_useStipple);
		modelViewer->setUsesFlatShading(m_useFlatShading);
		modelViewer->setPerformSmoothing(m_performSmoothing);
	}
}

void LDPreferences::applyPrimitivesSettings(void)
{
	if (modelViewer)
	{
		modelViewer->setAllowPrimitiveSubstitution(m_allowPrimitiveSubstitution);
		modelViewer->setTextureStuds(m_textureStuds);
		modelViewer->setTextureFilterType(m_textureFilterType);
		modelViewer->setCurveQuality(m_curveQuality);
		modelViewer->setQualityStuds(m_qualityStuds);
		modelViewer->setHiResPrimitives(m_hiResPrimitives);
	}
}

void LDPreferences::applyUpdatesSettings(void)
{
	if (modelViewer)
	{
		modelViewer->setCheckPartTracker(m_checkPartTracker);
		modelViewer->setMissingPartWait(m_missingPartWait);
		modelViewer->setUpdatedPartWait(m_updatedPartWait);
		if (m_proxyType == 2)
		{
			TCWebClient::setProxyServer(m_proxyServer.c_str());
			TCWebClient::setProxyPort(m_proxyPort);
		}
#ifdef WIN32
		else if (m_proxyType == 1)
		{
			std::string appName = TCUserDefaults::getAppName();

			// This is sort of cheating, but since I know exactly how TCUserDefaults
			// works, I know that this will get the job done.  Note that if the
			// actual type of the data in the registry doesn't match the requested
			// type, TCUserDefaults treats the registry value as if it doesn't
			// exist.
			TCUserDefaults::setAppName(
				"Microsoft/Windows/CurrentVersion/Internet Settings");
			if (TCUserDefaults::longForKey("ProxyEnable", 0, false))
			{
				char *proxyServer = TCUserDefaults::stringForKey("ProxyServer",
					NULL, false);

				if (proxyServer)
				{
					char *colon = (char *)strchr(proxyServer, ':');

					if (colon)
					{
						int proxyPort;

						*colon = 0;
						if (sscanf(&colon[1], "%d", &proxyPort) == 1)
						{
							TCWebClient::setProxyServer(proxyServer);
							TCWebClient::setProxyPort(proxyPort);
						}
					}
					delete proxyServer;
				}
			}
			TCUserDefaults::setAppName(appName.c_str());
		}
#endif
		else
		{
			TCWebClient::setProxyServer(NULL);
		}
	}
}

void LDPreferences::loadSettings(void)
{
	std::string lightVectorString;
	loadGeneralSettings();
	loadGeometrySettings();
	loadEffectsSettings();
	loadPrimitivesSettings();
	loadUpdatesSettings();
	loadInventorySettings();
	m_changedSettings.clear();

	m_skipValidation = false;
	m_zoomMax = getLongSetting(ZOOM_MAX_KEY, 199) / 100.0f;
	lightVectorString = getStringSetting(LIGHT_VECTOR_KEY);
	if (lightVectorString.length())
	{
		TCFloat lx, ly, lz;

		// ToDo: how to deal with 64-bit float scanf?
		if (sscanf(lightVectorString.c_str(), "%f,%f,%f", &lx, &ly, &lz) == 3)
		{
			m_lightVector = TCVector(lx, ly, lz);
		}
	}
	else
	{
		m_lightVector = TCVector(0.0f, 0.0f, 1.0f);
	}
	m_defaultZoom = getFloatSetting(DEFAULT_ZOOM_KEY, 1.0f);
}

void LDPreferences::loadDefaultGeneralSettings(void)
{
	int i;

	m_initializing = true;
	setFsaaMode(0);
	setLineSmoothing(false);
	setBackgroundColor(0, 0, 0);
	setDefaultColor(0x99, 0x99, 0x99);
	setTransDefaultColor(false);
	setProcessLdConfig(true);
	setShowFps(false);
	setShowErrors(true);
	setFullScreenRefresh(0);
	setFov(45.0f);
	setMemoryUsage(2);
	for (i = 0; i < 16; i++)
	{
		int r, g, b, a;

		LDLPalette::getDefaultRGBA(i, r, g, b, a);
		setCustomColor(i, r, g, b);
	}
	m_initializing = false;
}

void LDPreferences::loadDefaultGeometrySettings(void)
{
	m_initializing = true;
	setUseSeams(false);
	setSeamWidth(50);
	setDrawWireframe(false);
	setUseWireframeFog(false);
	setRemoveHiddenLines(false);
	setWireframeThickness(1);
	setBfc(true);
	setRedBackFaces(false);
	//setGreenFrontFaces(false);
	setGreenFrontFaces(false);
	setShowHighlightLines(false);
	setEdgesOnly(false);
	setDrawConditionalHighlights(false);
	setShowAllConditionalLines(false);
	setShowConditionalControlPoints(false);
	setUsePolygonOffset(true);
	setBlackHighlights(false);
	setEdgeThickness(1);
	m_initializing = false;
}

void LDPreferences::loadDefaultEffectsSettings(void)
{
	m_initializing = true;
	setUseLighting(true);
	setQualityLighting(false);
	setSubduedLighting(false);
	setUseSpecular(true);
	setOneLight(false);
	setStereoMode(LDVStereoNone);
	setStereoEyeSpacing(50);
	setCutawayMode(LDVCutawayNormal);
	setCutawayAlpha(100);
	setCutawayThickness(1);
	setSortTransparent(true);
	setUseStipple(false);
	setUseFlatShading(false);
	setPerformSmoothing(true);
	m_initializing = false;
}

void LDPreferences::loadDefaultPrimitivesSettings(void)
{
	m_initializing = true;
	setAllowPrimitiveSubstitution(true);
	setTextureStuds(true);
	setTextureFilterType(GL_LINEAR_MIPMAP_LINEAR);
	setCurveQuality(2);
	setQualityStuds(false);
	setHiResPrimitives(false);
	m_initializing = false;
}

void LDPreferences::loadDefaultUpdatesSettings(void)
{
	m_initializing = true;
	setProxyType(0);
	setProxyServer("");
	setProxyPort(80);
	setCheckPartTracker(true);
	setMissingPartWait(7);
	setUpdatedPartWait(7);
	m_initializing = false;
}

void LDPreferences::loadDefaultInventorySettings(void)
{
	m_initializing = true;
	setInvShowModel(false);
	setInvExternalCss(false);
	setInvPartImages(true);
	setInvShowFile(true);
	LongVector columnOrder;
	columnOrder.push_back(1);	// Part
	columnOrder.push_back(3);	// Color
	columnOrder.push_back(4);	// Quantity
	setInvColumnOrder(columnOrder);
	setInvLastSavePath("");
	m_initializing = false;
}

void LDPreferences::loadGeneralSettings(void)
{
	int i;

	loadDefaultGeneralSettings();
	m_fsaaMode = getIntSetting(FSAA_MODE_KEY, m_fsaaMode);
	m_lineSmoothing = getBoolSetting(LINE_SMOOTHING_KEY, m_lineSmoothing);
	m_backgroundColor = getLongSetting(BACKGROUND_COLOR_KEY,
		m_backgroundColor);
	m_defaultColor = getLongSetting(DEFAULT_COLOR_KEY, m_defaultColor);
	m_transDefaultColor = getBoolSetting(TRANS_DEFAULT_COLOR_KEY,
		m_transDefaultColor);
	m_defaultColorNumber = getIntSetting(DEFAULT_COLOR_NUMBER_KEY,
		m_defaultColorNumber);
	m_processLdConfig = getBoolSetting(PROCESS_LDCONFIG_KEY, m_processLdConfig);
	m_skipValidation = getBoolSetting(SKIP_VALIDATION_KEY, m_skipValidation);
	m_showFps = getBoolSetting(SHOW_FPS_KEY, m_showFps);
	m_showErrors = getBoolSetting(SHOW_ERRORS_KEY, m_showErrors);
	m_fullScreenRefresh = getIntSetting(FULLSCREEN_REFRESH_KEY,
		m_fullScreenRefresh);
	m_fov = getFloatSetting(FOV_KEY, (TCFloat32)m_fov);
	m_memoryUsage = getIntSetting(MEMORY_USAGE_KEY, m_memoryUsage);
	if (m_memoryUsage < 0 || m_memoryUsage > 2)
	{
		setMemoryUsage(2, true);
	}
	for (i = 0; i < 16; i++)
	{
		char key[128];

		sprintf(key, "%s/Color%02d", CUSTOM_COLORS_KEY, i);
		// Windows XP doesn't like the upper bits to be set, so mask those out.
		m_customColors[i] = getLongSetting(key, m_customColors[i]) & 0xFFFFFF;
	}
}

void LDPreferences::loadGeometrySettings(void)
{
	int useSeams;

	loadDefaultGeometrySettings();
	useSeams = getIntSetting(SEAMS_KEY, -1);
	m_seamWidth = getIntSetting(SEAM_WIDTH_KEY, m_seamWidth);
	if (useSeams == -1)
	{
		if (m_seamWidth)
		{
			m_useSeams = true;
		}
		else
		{
			m_useSeams = false;
		}
	}
	else
	{
		m_useSeams = useSeams != 0;
	}
	m_drawWireframe = getBoolSetting(WIREFRAME_KEY, m_drawWireframe);
	m_useWireframeFog = getBoolSetting(WIREFRAME_FOG_KEY, m_useWireframeFog);
	m_removeHiddenLines = getBoolSetting(REMOVE_HIDDEN_LINES_KEY,
		m_removeHiddenLines);
	m_wireframeThickness = getIntSetting(WIREFRAME_THICKNESS_KEY,
		m_wireframeThickness);
	m_bfc = getBoolSetting(BFC_KEY, m_bfc);
	m_redBackFaces = getBoolSetting(RED_BACK_FACES_KEY, m_redBackFaces);
	m_greenFrontFaces = getBoolSetting(GREEN_FRONT_FACES_KEY,
		m_greenFrontFaces);
	m_showHighlightLines = getBoolSetting(SHOW_HIGHLIGHT_LINES_KEY,
		m_showHighlightLines);
	m_edgesOnly = getBoolSetting(EDGES_ONLY_KEY, m_edgesOnly);
	m_drawConditionalHighlights = getBoolSetting(CONDITIONAL_HIGHLIGHTS_KEY,
		m_drawConditionalHighlights);
	m_showAllConditionalLines = getBoolSetting(SHOW_ALL_TYPE5_KEY,
		m_showAllConditionalLines);
	m_showConditionalControlPoints =
		getBoolSetting(SHOW_TYPE5_CONTROL_POINTS_KEY,
		m_showConditionalControlPoints);
	m_usePolygonOffset = getBoolSetting(POLYGON_OFFSET_KEY, m_usePolygonOffset);
	m_blackHighlights = getBoolSetting(BLACK_HIGHLIGHTS_KEY, m_blackHighlights);
	m_edgeThickness = getIntSetting(EDGE_THICKNESS_KEY, m_edgeThickness);
}

void LDPreferences::loadEffectsSettings(void)
{
	loadDefaultEffectsSettings();
	m_useLighting = getBoolSetting(LIGHTING_KEY, m_useLighting);
	m_qualityLighting = getBoolSetting(QUALITY_LIGHTING_KEY, m_qualityLighting);
	m_subduedLighting = getBoolSetting(SUBDUED_LIGHTING_KEY, m_subduedLighting);
	m_useSpecular = getBoolSetting(SPECULAR_KEY, m_useSpecular);
	m_oneLight = getBoolSetting(ONE_LIGHT_KEY, m_oneLight);
	m_stereoMode = (LDVStereoMode)getLongSetting(STEREO_MODE_KEY, m_stereoMode);
	m_stereoEyeSpacing = getIntSetting(STEREO_SPACING_KEY, m_stereoEyeSpacing);
	m_cutawayMode = (LDVCutawayMode)getLongSetting(CUTAWAY_MODE_KEY,
		m_cutawayMode);
	m_cutawayAlpha = getIntSetting(CUTAWAY_ALPHA_KEY, m_cutawayAlpha);
	m_cutawayThickness = getIntSetting(CUTAWAY_THICKNESS_KEY,
		m_cutawayThickness);
	m_sortTransparent = getBoolSetting(SORT_KEY, m_sortTransparent);
	m_useStipple = getBoolSetting(STIPPLE_KEY, m_useStipple);
	m_useFlatShading = getBoolSetting(FLAT_SHADING_KEY, m_useFlatShading);
	m_performSmoothing = getBoolSetting(PERFORM_SMOOTHING_KEY,
		m_performSmoothing);
}

void LDPreferences::loadPrimitivesSettings(void)
{
	loadDefaultPrimitivesSettings();
	m_allowPrimitiveSubstitution =
		getBoolSetting(PRIMITIVE_SUBSTITUTION_KEY,
		m_allowPrimitiveSubstitution);
	m_textureStuds = getBoolSetting(TEXTURE_STUDS_KEY, m_textureStuds);
	m_textureFilterType = getIntSetting(TEXTURE_FILTER_TYPE_KEY,
		m_textureFilterType);
	m_curveQuality = getIntSetting(CURVE_QUALITY_KEY, m_curveQuality);
	m_qualityStuds = getBoolSetting(QUALITY_STUDS_KEY, m_qualityStuds);
	m_hiResPrimitives = getBoolSetting(HI_RES_PRIMITIVES_KEY,
		m_hiResPrimitives);
}

void LDPreferences::loadUpdatesSettings(void)
{
	loadDefaultUpdatesSettings();
	m_proxyType = getIntSetting(PROXY_TYPE_KEY, m_proxyType);
	m_proxyServer = getStringSetting(PROXY_SERVER_KEY, NULL);
	m_proxyPort = getIntSetting(PROXY_PORT_KEY, m_proxyPort);
	m_checkPartTracker = getBoolSetting(CHECK_PART_TRACKER_KEY,
		m_checkPartTracker);
	m_missingPartWait = getIntSetting(CHECK_PART_WAIT_KEY, m_missingPartWait);
	m_updatedPartWait = getIntSetting(CHECK_PART_UPDATE_WAIT_KEY,
		m_updatedPartWait);
}

void LDPreferences::loadInventorySettings(void)
{
	loadDefaultInventorySettings();
	m_invShowModel = getBoolSetting(INV_SHOW_MODEL_KEY, m_invShowModel);
	m_invExternalCss = getBoolSetting(INV_EXTERNAL_CSS_KEY, m_invExternalCss);
	m_invPartImages = getBoolSetting(INV_PART_IMAGES_KEY, m_invPartImages);
	m_invShowFile = getBoolSetting(INV_SHOW_FILE_KEY, m_invShowFile);
	m_invColumnOrder = getLongVectorSetting(INV_COLUMN_ORDER_KEY,
		m_invColumnOrder);
	m_invLastSavePath = getStringSetting(INV_LAST_SAVE_PATH_KEY,
		m_invLastSavePath.c_str());
}

void LDPreferences::commitSettings(void)
{
	commitGeneralSettings();
	commitGeometrySettings();
	commitEffectsSettings();
	commitPrimitivesSettings();
	commitUpdatesSettings();
	commitInventorySettings();
	//commitPrefSetsSettings();
}

void LDPreferences::commitGeneralSettings(void)
{
	int i;
	int r, g, b;

	setFsaaMode(m_fsaaMode, true);
	setLineSmoothing(m_lineSmoothing, true);
	getRGB(m_backgroundColor, r, g, b);
	setBackgroundColor(r, g, b, true);
	getRGB(m_defaultColor, r, g, b);
	setDefaultColor(r, g, b, true);
	setTransDefaultColor(m_transDefaultColor, true);
	for (i = 0; i < 16; i++)
	{
		getRGB(m_customColors[i], r, g, b);
		setCustomColor(i, r, g, b, true);
	}
	setProcessLdConfig(m_processLdConfig, true);
	setShowFps(m_showFps, true);
	setShowErrors(m_showErrors, true);
	setFullScreenRefresh(m_fullScreenRefresh, true);
	setFov(m_fov, true);
	setMemoryUsage(m_memoryUsage, true);
}

void LDPreferences::commitGeometrySettings(void)
{
	setSetting(m_useSeams, m_useSeams, SEAMS_KEY, true);
	if (getUseSeams())
	{
		setSeamWidth(m_seamWidth, true);
	}
	setDrawWireframe(m_drawWireframe, true);
	setUseWireframeFog(m_useWireframeFog, true);
	setRemoveHiddenLines(m_removeHiddenLines, true);
	setWireframeThickness(m_wireframeThickness, true);
	setBfc(m_bfc, true);
	setRedBackFaces(m_redBackFaces, true);
	setGreenFrontFaces(m_greenFrontFaces, true);
	setShowHighlightLines(m_showHighlightLines, true);
	if (m_showHighlightLines)
	{
		setEdgesOnly(m_edgesOnly, true);
		setDrawConditionalHighlights(m_drawConditionalHighlights, true);
		if (m_drawConditionalHighlights)
		{
			setShowAllConditionalLines(m_showAllConditionalLines, true);
			setShowConditionalControlPoints(m_showConditionalControlPoints,
				true);
		}
		setUsePolygonOffset(m_usePolygonOffset, true);
		setBlackHighlights(m_blackHighlights, true);
	}
	setEdgeThickness(m_edgeThickness, true);
}

void LDPreferences::commitEffectsSettings(void)
{
	setUseLighting(m_useLighting, true);
	if (m_useLighting)
	{
		setQualityLighting(m_qualityLighting, true);
		setSubduedLighting(m_subduedLighting, true);
		setUseSpecular(m_useSpecular, true);
		setOneLight(m_oneLight, true);
	}
	setStereoMode(m_stereoMode, true);
	setStereoEyeSpacing(m_stereoEyeSpacing, true);
	setCutawayMode(m_cutawayMode, true);
	setCutawayAlpha(m_cutawayAlpha, true);
	setCutawayThickness(m_cutawayThickness, true);
	setUseStipple(m_useStipple, true);
	setSortTransparent(m_sortTransparent, true);
	setUseFlatShading(m_useFlatShading, true);
	setPerformSmoothing(m_performSmoothing, true);
}

void LDPreferences::commitPrimitivesSettings(void)
{
	setAllowPrimitiveSubstitution(m_allowPrimitiveSubstitution, true);
	if (m_allowPrimitiveSubstitution)
	{
		setTextureStuds(m_textureStuds, true);
		setTextureFilterType(m_textureFilterType, true);
		setCurveQuality(m_curveQuality, true);
	}
	setQualityStuds(m_qualityStuds, true);
	setHiResPrimitives(m_hiResPrimitives, true);
}

void LDPreferences::commitUpdatesSettings(void)
{
	setProxyType(m_proxyType, true);
	if (m_proxyType == 2)
	{
		setProxyServer(m_proxyServer.c_str(), true);
		setProxyPort(m_proxyPort, true);
	}
	setCheckPartTracker(m_checkPartTracker, true);
	setMissingPartWait(m_missingPartWait, true);
	setUpdatedPartWait(m_updatedPartWait, true);
}

void LDPreferences::commitInventorySettings(void)
{
	setInvShowModel(m_invShowModel, true);
	setInvExternalCss(m_invExternalCss, true);
	setInvPartImages(m_invPartImages, true);
	setInvShowFile(m_invShowFile, true);
	setInvColumnOrder(m_invColumnOrder, true);
	setInvLastSavePath(m_invLastSavePath.c_str(), true);
}

void LDPreferences::setupDefaultRotationMatrix(void)
{
	std::string value = getStringSetting(CAMERA_GLOBE_KEY);

	if (!value.length())
	{
		value = getStringSetting(DEFAULT_LAT_LONG_KEY);
	}
	if (value.length())
	{
		TCFloat latitude;
		TCFloat longitude;

		if (sscanf(value.c_str(), "%f,%f", &latitude, &longitude) == 2)
		{
			TCFloat leftMatrix[16];
			TCFloat rightMatrix[16];
			TCFloat resultMatrix[16];
			TCFloat cosTheta;
			TCFloat sinTheta;

			TCVector::initIdentityMatrix(leftMatrix);
			TCVector::initIdentityMatrix(rightMatrix);
			latitude = (TCFloat)deg2rad(latitude);
			longitude = (TCFloat)deg2rad(longitude);

			// First, apply latitude by rotating around X.
			cosTheta = (TCFloat)cos(latitude);
			sinTheta = (TCFloat)sin(latitude);
			rightMatrix[5] = cosTheta;
			rightMatrix[6] = sinTheta;
			rightMatrix[9] = -sinTheta;
			rightMatrix[10] = cosTheta;
			TCVector::multMatrix(leftMatrix, rightMatrix, resultMatrix);

			memcpy(leftMatrix, resultMatrix, sizeof(leftMatrix));
			TCVector::initIdentityMatrix(rightMatrix);

			// Next, apply longitude by rotating around Y.
			cosTheta = (TCFloat)cos(longitude);
			sinTheta = (TCFloat)sin(longitude);
			rightMatrix[0] = cosTheta;
			rightMatrix[2] = -sinTheta;
			rightMatrix[8] = sinTheta;
			rightMatrix[10] = cosTheta;
			TCVector::multMatrix(leftMatrix, rightMatrix, resultMatrix);

			if (modelViewer)
			{
				modelViewer->setDefaultRotationMatrix(resultMatrix);
			}
		}
	}
	else
	{
		value = getStringSetting(DEFAULT_MATRIX_KEY);
		if (value.length())
		{
			TCFloat matrix[16];

			memset(matrix, 0, sizeof(matrix));
			matrix[15] = 1.0f;
			// ToDo: how to deal with 64-bit float scanf?
			if (sscanf(value.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f,%f",
				&matrix[0], &matrix[4], &matrix[8],
				&matrix[1], &matrix[5], &matrix[9],
				&matrix[2], &matrix[6], &matrix[10]) == 9)
			{
				if (modelViewer)
				{
					modelViewer->setDefaultRotationMatrix(matrix);
				}
			}
		}
	}
}

void LDPreferences::setupModelCenter(void)
{
	std::string value = getStringSetting(MODEL_CENTER_KEY);

	if (value.length())
	{
		TCFloat center[3];
		// ToDo: how to deal with 64-bit float scanf?
		if (sscanf(value.c_str(), "%f,%f,%f", &center[0], &center[1],&center[2])
			== 3)
		{
			if (modelViewer)
			{
				modelViewer->setModelCenter(center);
			}
		}
	}
}

void LDPreferences::setupModelSize(void)
{
	std::string value = getStringSetting(MODEL_SIZE_KEY);

	if (value.length())
	{
		TCFloat size;
		// ToDo: how to deal with 64-bit float scanf?
		if (sscanf(value.c_str(), "%f", &size) == 1)
		{
			if (modelViewer)
			{
				modelViewer->setModelSize(size);
			}
		}
	}
}

void LDPreferences::setColorSetting(TCULong &setting, int r, int g, int b,
									 const char *key, bool commit)
{
	TCULong value = getColor(r, g, b);
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setLongForKey(setting, key, !m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(LongVector &setting, const LongVector &value,
							   const char *key, bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setLongVectorForKey(value, key,
				!m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(bool &setting, bool value, const char *key,
								bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setLongForKey(value ? 1 : 0, key,
				!m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(int &setting, int value, const char *key,
								bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setLongForKey(value, key, !m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(TCULong &setting, TCULong value, const char *key,
								bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setLongForKey((long)value, key,
				!m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(TCFloat &setting, TCFloat value, const char *key,
								bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setFloatForKey(value, key, !m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

void LDPreferences::setSetting(std::string &setting, const std::string &value,
								const char *key, bool commit)
{
	if (setting != value || (m_changedSettings[key] && commit))
	{
		setting = value;
		if (commit)
		{
			TCUserDefaults::setStringForKey(value.c_str(), key,
				!m_globalSettings[key]);
			m_changedSettings.erase(key);
		}
		else if (!m_initializing)
		{
			m_changedSettings[key] = true;
		}
	}
}

bool LDPreferences::getBoolSetting(const char *key, bool defaultValue)
{
	return TCUserDefaults::longForKey(key, (long)defaultValue,
		!m_globalSettings[key]) != 0;
}

LongVector LDPreferences::getLongVectorSetting(const char *key,
										 const LongVector &defaultValue)
{
	return TCUserDefaults::longVectorForKey(key, defaultValue,
		!m_globalSettings[key]);
}

long LDPreferences::getLongSetting(const char *key, long defaultValue)
{
	return TCUserDefaults::longForKey(key, defaultValue, !m_globalSettings[key]);
}

int LDPreferences::getIntSetting(const char *key, int defaultValue)
{
	return (int)TCUserDefaults::longForKey(key, defaultValue,
		!m_globalSettings[key]);
}

float LDPreferences::getFloatSetting(const char *key, float defaultValue)
{
	return TCUserDefaults::floatForKey(key, defaultValue, !m_globalSettings[key]);
}

std::string LDPreferences::getStringSetting(const char *key,
											const char *defaultValue)
{
	char *tmpString = TCUserDefaults::stringForKey(key, defaultValue,
		!m_globalSettings[key]);
	std::string result;

	if (tmpString)
	{
		result = tmpString;
		delete tmpString;
	}
	return result;
}

// General settings
void LDPreferences::setFsaaMode(int value, bool commit)
{
	setSetting(m_fsaaMode, value, FSAA_MODE_KEY, commit);
}

void LDPreferences::setLineSmoothing(bool value, bool commit)
{
	setSetting(m_lineSmoothing, value, LINE_SMOOTHING_KEY, commit);
}

void LDPreferences::setBackgroundColor(int r, int g, int b, bool commit)
{
	setColorSetting(m_backgroundColor, r, g, b, BACKGROUND_COLOR_KEY, commit);
}

void LDPreferences::setDefaultColor(int r, int g, int b, bool commit)
{
	setColorSetting(m_defaultColor, r, g, b, DEFAULT_COLOR_KEY, commit);
}

void LDPreferences::setTransDefaultColor(bool value, bool commit)
{
	setSetting(m_transDefaultColor, value, TRANS_DEFAULT_COLOR_KEY, commit);
}

void LDPreferences::setProcessLdConfig(bool value, bool commit)
{
	setSetting(m_processLdConfig, value, PROCESS_LDCONFIG_KEY, commit);
}

void LDPreferences::setShowFps(bool value, bool commit)
{
	setSetting(m_showFps, value, SHOW_FPS_KEY, commit);
}

void LDPreferences::setShowErrors(bool value, bool commit)
{
	setSetting(m_showErrors, value, SHOW_ERRORS_KEY, commit);
}

void LDPreferences::setFullScreenRefresh(int value, bool commit)
{
	setSetting(m_fullScreenRefresh, value, FULLSCREEN_REFRESH_KEY, commit);
}

void LDPreferences::setFov(TCFloat value, bool commit)
{
	setSetting(m_fov, value, FOV_KEY, commit);
}

void LDPreferences::setMemoryUsage(int value, bool commit)
{
	setSetting(m_memoryUsage, value, MEMORY_USAGE_KEY, commit);
}

void LDPreferences::setCustomColor(int index, int r, int g, int b, bool commit)
{
	char key[128];

	sprintf(key, "%s/Color%02d", CUSTOM_COLORS_KEY, index);
	setColorSetting(m_customColors[index], r, g, b, key, commit);
}

// Geometry settings
void LDPreferences::setUseSeams(bool value, bool commit, bool apply)
{
	setSetting(m_useSeams, value, SEAMS_KEY, commit);
	if (apply)
	{
		if (m_useSeams)
		{
			if (modelViewer)
			{
				modelViewer->setSeamWidth(m_seamWidth / 100.0f);
			}
		}
		else
		{
			if (modelViewer)
			{
				modelViewer->setSeamWidth(0.0f);
			}
		}
	}
}

void LDPreferences::setSeamWidth(int value, bool commit)
{
	setSetting(m_seamWidth, value, SEAM_WIDTH_KEY, commit);
}

void LDPreferences::setDrawWireframe(bool value, bool commit, bool apply)
{
	setSetting(m_drawWireframe, value, WIREFRAME_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setDrawWireframe(m_drawWireframe);
		}
	}
}

void LDPreferences::setUseWireframeFog(bool value, bool commit, bool apply)
{
	setSetting(m_useWireframeFog, value, WIREFRAME_FOG_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setUseWireframeFog(m_useWireframeFog);
		}
	}
}

void LDPreferences::setRemoveHiddenLines(bool value, bool commit, bool apply)
{
	setSetting(m_removeHiddenLines, value, REMOVE_HIDDEN_LINES_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setRemoveHiddenLines(m_removeHiddenLines);
		}
	}
}

void LDPreferences::setWireframeThickness(int value, bool commit)
{
	setSetting(m_wireframeThickness, value, WIREFRAME_THICKNESS_KEY, commit);
}

void LDPreferences::setBfc(bool value, bool commit, bool apply)
{
	setSetting(m_bfc, value, BFC_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setBfc(m_bfc);
		}
	}
}

void LDPreferences::setRedBackFaces(bool value, bool commit, bool apply)
{
	setSetting(m_redBackFaces, value, RED_BACK_FACES_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setRedBackFaces(m_redBackFaces);
		}
	}
}

void LDPreferences::setGreenFrontFaces(bool value, bool commit, bool apply)
{
	setSetting(m_greenFrontFaces, value, GREEN_FRONT_FACES_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setGreenFrontFaces(m_greenFrontFaces);
		}
	}
}

void LDPreferences::setShowHighlightLines(bool value, bool commit, bool apply)
{
	setSetting(m_showHighlightLines, value, SHOW_HIGHLIGHT_LINES_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setShowsHighlightLines(m_showHighlightLines);
		}
	}
}

void LDPreferences::setDrawConditionalHighlights(bool value, bool commit,
												 bool apply)
{
	setSetting(m_drawConditionalHighlights, value, CONDITIONAL_HIGHLIGHTS_KEY,
		commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setDrawConditionalHighlights(
				m_drawConditionalHighlights);
		}
	}
}

void LDPreferences::setShowAllConditionalLines(bool value, bool commit)
{
	setSetting(m_showAllConditionalLines, value, SHOW_ALL_TYPE5_KEY, commit);
}

void LDPreferences::setShowConditionalControlPoints(bool value, bool commit)
{
	setSetting(m_showConditionalControlPoints, value,
		SHOW_TYPE5_CONTROL_POINTS_KEY, commit);
}

void LDPreferences::setEdgesOnly(bool value, bool commit, bool apply)
{
	setSetting(m_edgesOnly, value, EDGES_ONLY_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setEdgesOnly(m_edgesOnly);
		}
	}
}

void LDPreferences::setUsePolygonOffset(bool value, bool commit, bool apply)
{
	setSetting(m_usePolygonOffset, value, POLYGON_OFFSET_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setUsePolygonOffset(m_usePolygonOffset);
		}
	}
}

void LDPreferences::setBlackHighlights(bool value, bool commit, bool apply)
{
	setSetting(m_blackHighlights, value, BLACK_HIGHLIGHTS_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setBlackHighlights(m_blackHighlights);
		}
	}
}

void LDPreferences::setEdgeThickness(int value, bool commit)
{
	setSetting(m_edgeThickness, value, EDGE_THICKNESS_KEY, commit);
}


// Effects settings
void LDPreferences::setUseLighting(bool value, bool commit, bool apply)
{
	setSetting(m_useLighting, value, LIGHTING_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setUseLighting(m_useLighting);
		}
	}
}

void LDPreferences::setQualityLighting(bool value, bool commit, bool apply)
{
	setSetting(m_qualityLighting, value, QUALITY_LIGHTING_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setQualityLighting(m_qualityLighting);
		}
	}
}

void LDPreferences::setSubduedLighting(bool value, bool commit, bool apply)
{
	setSetting(m_subduedLighting, value, SUBDUED_LIGHTING_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setSubduedLighting(m_subduedLighting);
		}
	}
}

void LDPreferences::setUseSpecular(bool value, bool commit, bool apply)
{
	setSetting(m_useSpecular, value, SPECULAR_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setUsesSpecular(m_useSpecular);
		}
	}
}

void LDPreferences::setOneLight(bool value, bool commit, bool apply)
{
	setSetting(m_oneLight, value, ONE_LIGHT_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setOneLight(m_oneLight);
		}
	}
}

void LDPreferences::setStereoMode(LDVStereoMode value, bool commit)
{
	setSetting((int &)m_stereoMode, value, STEREO_MODE_KEY, commit);
}

void LDPreferences::setStereoEyeSpacing(int value, bool commit)
{
	setSetting(m_stereoEyeSpacing, value, STEREO_SPACING_KEY, commit);
}

void LDPreferences::setCutawayMode(LDVCutawayMode value, bool commit)
{
	setSetting((int &)m_cutawayMode, value, CUTAWAY_MODE_KEY, commit);
}

void LDPreferences::setCutawayAlpha(int value, bool commit)
{
	setSetting(m_cutawayAlpha, value, CUTAWAY_ALPHA_KEY, commit);
}

void LDPreferences::setCutawayThickness(int value, bool commit)
{
	setSetting(m_cutawayThickness, value, CUTAWAY_THICKNESS_KEY, commit);
}

void LDPreferences::setSortTransparent(bool value, bool commit)
{
	setSetting(m_sortTransparent, value, SORT_KEY, commit);
}

void LDPreferences::setPerformSmoothing(bool value, bool commit)
{
	setSetting(m_performSmoothing, value, PERFORM_SMOOTHING_KEY, commit);
}

void LDPreferences::setUseStipple(bool value, bool commit)
{
	setSetting(m_useStipple, value, STIPPLE_KEY, commit);
}

void LDPreferences::setUseFlatShading(bool value, bool commit)
{
	setSetting(m_useFlatShading, value, FLAT_SHADING_KEY, commit);
}


// Primitives settings
void LDPreferences::setAllowPrimitiveSubstitution(bool value, bool commit,
												  bool apply)
{
	setSetting(m_allowPrimitiveSubstitution, value, PRIMITIVE_SUBSTITUTION_KEY,
		commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setAllowPrimitiveSubstitution(
				m_allowPrimitiveSubstitution);
		}
	}
}

void LDPreferences::setTextureStuds(bool value, bool commit, bool apply)
{
	setSetting(m_textureStuds, value, TEXTURE_STUDS_KEY, commit);
	if (apply)
	{
		if (modelViewer)
		{
			modelViewer->setTextureStuds(m_textureStuds);
		}
	}
}

void LDPreferences::setTextureFilterType(int value, bool commit)
{
	setSetting(m_textureFilterType, value, TEXTURE_FILTER_TYPE_KEY, commit);
}

void LDPreferences::setCurveQuality(int value, bool commit)
{
	setSetting(m_curveQuality, value, CURVE_QUALITY_KEY, commit);
}

void LDPreferences::setQualityStuds(bool value, bool commit)
{
	setSetting(m_qualityStuds, value, QUALITY_STUDS_KEY, commit);
}

void LDPreferences::setHiResPrimitives(bool value, bool commit)
{
	setSetting(m_hiResPrimitives, value, HI_RES_PRIMITIVES_KEY, commit);
}


// Update settings
void LDPreferences::setProxyType(int value, bool commit)
{
	setSetting(m_proxyType, value, PROXY_TYPE_KEY, commit);
}

void LDPreferences::setProxyServer(const char *value, bool commit)
{
	setSetting(m_proxyServer, value, PROXY_SERVER_KEY, commit);
}

void LDPreferences::setProxyPort(int value, bool commit)
{
	setSetting(m_proxyPort, value, PROXY_PORT_KEY, commit);
}

void LDPreferences::setCheckPartTracker(bool value, bool commit)
{
	setSetting(m_checkPartTracker, value, CHECK_PART_TRACKER_KEY, commit);
}

void LDPreferences::setMissingPartWait(int value, bool commit)
{
	setSetting(m_missingPartWait, value, CHECK_PART_WAIT_KEY, commit);
}

void LDPreferences::setUpdatedPartWait(int value, bool commit)
{
	setSetting(m_updatedPartWait, value, CHECK_PART_UPDATE_WAIT_KEY, commit);
}

void LDPreferences::setInvShowModel(bool value, bool commit)
{
	setSetting(m_invShowModel, value, INV_SHOW_MODEL_KEY, commit);
}

void LDPreferences::setInvExternalCss(bool value, bool commit)
{
	setSetting(m_invExternalCss, value, INV_EXTERNAL_CSS_KEY, commit);
}

void LDPreferences::setInvPartImages(bool value, bool commit)
{
	setSetting(m_invPartImages, value, INV_PART_IMAGES_KEY, commit);
}

void LDPreferences::setInvShowFile(bool value, bool commit)
{
	setSetting(m_invShowFile, value, INV_SHOW_FILE_KEY, commit);
}

void LDPreferences::setInvColumnOrder(const LongVector &value, bool commit)
{
	setSetting(m_invColumnOrder, value, INV_COLUMN_ORDER_KEY, commit);
}

void LDPreferences::setInvLastSavePath(const char *value, bool commit)
{
	setSetting(m_invLastSavePath, value, INV_LAST_SAVE_PATH_KEY, commit);
}

void LDPreferences::setDefaultZoom(TCFloat value, bool commit)
{
	setSetting(m_defaultZoom, value, DEFAULT_ZOOM_KEY, commit);
}

void LDPreferences::saveDefaultView(void)
{
	if (modelViewer)
	{
		TCFloat matrix[16];
		TCFloat rotationMatrix[16];
		TCFloat otherMatrix[16] = {1,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,1};
		char matrixString[1024];

		memcpy(rotationMatrix, modelViewer->getRotationMatrix(),
			sizeof(rotationMatrix));
		TCVector::multMatrix(otherMatrix, rotationMatrix, matrix);
		matrix[12] = 0.0f;
		matrix[13] = 0.0f;
		matrix[14] = 0.0f;
		sprintf(matrixString, "%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g",
			matrix[0], matrix[4], matrix[8],
			matrix[1], matrix[5], matrix[9],
			matrix[2], matrix[6], matrix[10]);
		TCUserDefaults::setStringForKey(matrixString, DEFAULT_MATRIX_KEY);
		modelViewer->setDefaultRotationMatrix(matrix);
	}
}

void LDPreferences::resetDefaultView(void)
{
	TCUserDefaults::removeValue(DEFAULT_MATRIX_KEY);
	if (modelViewer)
	{
		modelViewer->setDefaultRotationMatrix(NULL);
	}
}

void LDPreferences::getBackgroundColor(int &r, int &g, int &b)
{
	getRGB(m_backgroundColor, r, g, b);
}

void LDPreferences::getDefaultColor(int &r, int &g, int &b)
{
	getRGB(m_defaultColor, r, g, b);
}

void LDPreferences::getCustomColor(int index, int &r, int &g, int &b)
{
	getRGB(m_customColors[index], r, g, b);
}
