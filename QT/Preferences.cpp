#include "Preferences.h"
#include "PreferencesPanel.h"
#include "ModelViewerWidget.h"
#include <LDLoader/LDLModel.h>
#include <LDLoader/LDLPalette.h>
#include "TCColorButton.h"
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include "UserDefaultsKeys.h"
#include <TCFoundation/TCMacros.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qcolor.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <netinet/in.h>

Preferences::Preferences(ModelViewerWidget *modelWidget)
	:modelWidget(modelWidget),
	panel(new PreferencesPanel)
{
	modelViewer = modelWidget->getModelViewer();
	panel->setPreferences(this);
	panel->resize(10, 10);
	loadSettings();
	reflectSettings();
	setDebugLevel((int)TCUserDefaults::longForKey(DEBUG_LEVEL_KEY));
}

Preferences::~Preferences(void)
{
}

void Preferences::show(void)
{
	panel->applyButton->setEnabled(false);
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}

void Preferences::doGeneralApply(void)
{
	bool bTemp;
	int iTemp;
	QColor cTemp;
	int br, bg, bb;
	int dr, dg, db;

	bTemp = panel->aaLinesButton->state();
	if (bTemp != lineSmoothing)
	{
		lineSmoothing = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, LINE_SMOOTHING_KEY);
	}
	bTemp = panel->frameRateButton->state();
	if (bTemp != showFPS)
	{
		showFPS = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SHOW_FPS_KEY);
	}
	bTemp = panel->showErrorsButton->state();
	if (bTemp != showErrors)
	{
		showErrors = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SHOW_ERRORS_KEY, false);
	}
	bTemp = panel->processLdconfigLdrButton->state();
    if (bTemp !=  processLdconfigLdr)
	{
		processLdconfigLdr = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, 
				PROCESS_LDCONFIG_KEY, false);
	}
	cTemp = panel->backgroundColorButton->color();
	cTemp.rgb(&br, &bg, &bb);
	iTemp = LDLPalette::colorForRGBA(br, bg, bb, 255);
	if (iTemp != backgroundColor)
	{
		backgroundColor = iTemp;
		TCUserDefaults::setLongForKey((long)backgroundColor,
			BACKGROUND_COLOR_KEY);
	}
	else
	{
		getRGB(backgroundColor, br, bg, bb);
	}
	cTemp = panel->defaultColorButton->color();
	cTemp.rgb(&dr, &dg, &db);
	iTemp = LDLPalette::colorForRGBA(dr, dg, db, 255);
	if (iTemp != defaultColor)
	{
		defaultColor = iTemp;
		TCUserDefaults::setLongForKey((long)defaultColor, DEFAULT_COLOR_KEY);
	}
	else
	{
		getRGB(defaultColor, dr, dg, db);
	}
	iTemp = panel->fieldOfViewSpin->value();
	if (iTemp != fieldOfView)
	{
		fieldOfView = iTemp;
		TCUserDefaults::setLongForKey(iTemp, FOV_KEY);
	}
	if (modelWidget)
	{
		modelWidget->setShowFPS(showFPS);
	}
	if (modelViewer)
	{
		modelViewer->setLineSmoothing(lineSmoothing);
		modelViewer->setBackgroundRGB(br, bg, bb);
		// TC TODO: last param should be user-setable flag that decides
		// whether or not the default color is transparent.
		modelViewer->setDefaultRGB((TCByte)dr, (TCByte)dg, (TCByte)db,
			false);
		modelViewer->setDefaultColorNumber(defaultColorNumber);
		modelViewer->setFov((float)fieldOfView);
		modelViewer->setProcessLDConfig(processLdconfigLdr);
	}
}

void Preferences::doGeometryApply(void)
{
	bool bTemp;
	int iTemp;

	bTemp = panel->seamWidthButton->state();
	if (bTemp != seams)
	{
		seams = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SEAMS_KEY);
	}
	iTemp = panel->seamWidthSpin->value();
	if (iTemp != seamWidth)
	{
		seamWidth = iTemp;
		TCUserDefaults::setLongForKey(iTemp, SEAM_WIDTH_KEY);
	}
	bTemp = panel->wireframeButton->state();
	if (bTemp != wireframe)
	{
		wireframe = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, WIREFRAME_KEY);
	}
	if (wireframe)
	{
		bTemp = panel->wireframeFogButton->state();
		if (bTemp != wireframeFog)
		{
			wireframeFog = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, WIREFRAME_FOG_KEY);
		}
		bTemp = panel->wireframeRemoveHiddenLineButton->state();
		if (bTemp != wireframeRemoveHiddenLines)
		{
			wireframeRemoveHiddenLines = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, REMOVE_HIDDEN_LINES_KEY);
		}
	}
	iTemp = panel->wireframeThicknessSlider->value();
	if (iTemp != wireframeThickness)
	{
		wireframeThickness = iTemp;
		TCUserDefaults::setLongForKey(iTemp, WIREFRAME_THICKNESS_KEY);
	}
	bTemp = panel->edgeLinesButton->state();
	if (bTemp != edgeLines)
	{
		edgeLines = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SHOWS_HIGHLIGHT_LINES_KEY);
	}
	if (edgeLines)
	{
		bTemp = panel->conditionalLinesButton->state();
		if (bTemp != conditionalLines)
		{
			conditionalLines = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
				CONDITIONAL_HIGHLIGHTS_KEY);
		}
		bTemp = panel->edgesOnlyButton->state();
		if (bTemp != edgesOnly)
		{
			edgesOnly = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
				EDGES_ONLY_KEY);
		}
		bTemp = panel->highQualityLinesButton->state();
		if (bTemp != polygonOffset)
		{
			polygonOffset = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, POLYGON_OFFSET_KEY);
		}
		bTemp = panel->alwaysBlackLinesButton->state();
		if (bTemp != blackEdgeLines)
		{
			blackEdgeLines = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, BLACK_HIGHLIGHTS_KEY);
		}
		iTemp = panel->edgeThicknessSlider->value();
		if (iTemp != edgeThickness)
		{
			edgeThickness = iTemp;
			TCUserDefaults::setLongForKey(iTemp, EDGE_THICKNESS_KEY);
		}
		if (conditionalLines)
		{
			bTemp = panel->conditionalShowAllButton->state();
			if (bTemp != conditionalShowAll)
			{
				conditionalShowAll = bTemp;
				TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
					SHOW_ALL_TYPE5_KEY);
			}
			bTemp = panel->conditionalShowControlPtsButton->state();
			if (bTemp != conditionalShowControlPts)
			{
				conditionalShowControlPts = bTemp;
				TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
					SHOW_TYPE5_CONTROL_POINTS_KEY);
			}
		}
	}
	if (modelViewer)
	{
		if (seams)
		{
			modelViewer->setSeamWidth(seamWidth / 100.0f);
		}
		else
		{
			modelViewer->setSeamWidth(0.0f);
		}
		modelViewer->setDrawWireframe(wireframe);
		modelViewer->setUseWireframeFog(wireframeFog);
		modelViewer->setRemoveHiddenLines(wireframeRemoveHiddenLines);
		modelViewer->setWireframeLineWidth(wireframeThickness);
		modelViewer->setShowsHighlightLines(edgeLines);
		modelViewer->setDrawConditionalHighlights(conditionalLines);
		modelViewer->setShowAllConditionalLines(conditionalShowAll);
		modelViewer->setShowConditionalControlPoints(
			conditionalShowControlPts);
		modelViewer->setEdgesOnly(edgesOnly);
		modelViewer->setUsePolygonOffset(polygonOffset);
		modelViewer->setBlackHighlights(blackEdgeLines);
		modelViewer->setHighlightLineWidth(edgeThickness);
	}
}

void Preferences::doEffectsApply(void)
{
	bool bTemp;
	int iTemp;
	LDVStereoMode smTemp = LDVStereoNone;
	LDVCutawayMode cmTemp = LDVCutawayNormal;

	bTemp = panel->lightingButton->state();
	if (bTemp != lighting)
	{
		lighting = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, LIGHTING_KEY);
	}
	if (lighting)
	{
		bTemp = panel->qualityLightingButton->state();
		if (bTemp != qualityLighting)
		{
			qualityLighting = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, QUALITY_LIGHTING_KEY);
		}
		bTemp = panel->subduedLightingButton->state();
		if (bTemp != subduedLighting)
		{
			subduedLighting = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SUBDUED_LIGHTING_KEY);
		}
		bTemp = panel->specularLightingButton->state();
		if (bTemp != specular)
		{
			specular = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SPECULAR_KEY);
		}
		bTemp = panel->alternateLightingButton->state();
		if (bTemp != alternateLighting)
		{
			alternateLighting = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, ONE_LIGHT_KEY);
		}
	}
	if (!panel->stereoButton->state())
	{
		smTemp = LDVStereoNone;
	}
	else if (panel->crossEyedStereoButton->state())
	{
		smTemp = LDVStereoCrossEyed;
	}
	else if (panel->parallelStereoButton->state())
	{
		smTemp = LDVStereoParallel;
	}
	if (smTemp != stereoMode)
	{
		stereoMode = smTemp;
		TCUserDefaults::setLongForKey(smTemp, STEREO_MODE_KEY);
	}
	iTemp = panel->stereoAmountSlider->value();
	if (iTemp != stereoEyeSpacing)
	{
		stereoEyeSpacing = iTemp;
		TCUserDefaults::setLongForKey(iTemp, STEREO_SPACING_KEY);
	}
	if (!panel->wireframeCutawayButton->state())
	{
		cmTemp = LDVCutawayNormal;
	}
	else if (panel->colorCutawayButton->state())
	{
		cmTemp = LDVCutawayWireframe;
	}
	else if (panel->monochromeCutawayButton->state())
	{
		cmTemp = LDVCutawayStencil;
	}
	if (cmTemp != cutawayMode)
	{
		cutawayMode = cmTemp;
		TCUserDefaults::setLongForKey(cmTemp, CUTAWAY_MODE_KEY);
	}
	iTemp = panel->cutawayOpacitySlider->value();
	if (iTemp != cutawayAlpha)
	{
		cutawayAlpha = iTemp;
		TCUserDefaults::setLongForKey(iTemp, CUTAWAY_ALPHA_KEY);
	}
	iTemp = panel->cutawayThicknessSlider->value();
	if (iTemp != cutawayThickness)
	{
		cutawayThickness = iTemp;
		TCUserDefaults::setLongForKey(iTemp, CUTAWAY_THICKNESS_KEY);
	}
	bTemp = panel->sortTransparencyButton->state();
	if (bTemp != sortTransparent)
	{
		sortTransparent = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, SORT_KEY);
	}
	bTemp = panel->stippleTransparencyButton->state();
	if (bTemp != stipple)
	{
		stipple = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, STIPPLE_KEY);
	}
	bTemp = panel->flatShadingButton->state();
	if (bTemp != flatShading)
	{
		flatShading = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, FLAT_SHADING_KEY);
	}
	bTemp = panel->smoothCurvesButton->state();
	if (bTemp != smoothing)
	{
		smoothing = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, PERFORM_SMOOTHING_KEY);
	}
	if (modelViewer)
	{
		modelViewer->setUseLighting(lighting);
		modelViewer->setQualityLighting(qualityLighting);
		modelViewer->setSubduedLighting(subduedLighting);
		modelViewer->setUsesSpecular(specular);
		modelViewer->setOneLight(alternateLighting);
		modelViewer->setStereoMode(stereoMode);
		modelViewer->setStereoEyeSpacing((GLfloat)stereoEyeSpacing);
		modelViewer->setCutawayMode(cutawayMode);
		modelViewer->setCutawayAlpha((float)cutawayAlpha / 100.0f);
		modelViewer->setCutawayLineWidth((float)cutawayThickness);
		modelViewer->setSortTransparent(sortTransparent);
		modelViewer->setUseStipple(stipple);
		modelViewer->setUsesFlatShading(flatShading);
		modelViewer->setPerformSmoothing(smoothing);
	}
}

void Preferences::doPrimitivesApply(void)
{
	bool bTemp;
	int iTemp;

	bTemp = panel->primitiveSubstitutionButton->state();
	if (bTemp != allowPrimitiveSubstitution)
	{
		allowPrimitiveSubstitution = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
			PRIMITIVE_SUBSTITUTION_KEY);
	}
	if (allowPrimitiveSubstitution)
	{
		bTemp = panel->textureStudsButton->state();
		if (bTemp != textureStuds)
		{
			textureStuds = bTemp;
			TCUserDefaults::setLongForKey(bTemp ? 1 : 0, TEXTURE_STUDS_KEY);
		}
		if (textureStuds)
		{
			iTemp = GL_NEAREST_MIPMAP_NEAREST;
			if (panel->nearestFilteringButton->state())
			{
				iTemp = GL_NEAREST_MIPMAP_NEAREST;
			}
			else if (panel->bilinearFilteringButton->state())
			{
				iTemp = GL_LINEAR_MIPMAP_NEAREST;
			}
			else if (panel->trilinearFilteringButton->state())
			{
				iTemp = GL_LINEAR_MIPMAP_LINEAR;
			}
			if (iTemp != textureFilterType)
			{
				textureFilterType = iTemp;
				TCUserDefaults::setLongForKey(iTemp, TEXTURE_FILTER_TYPE_KEY);
			}
		}
		iTemp = panel->curveQualitySlider->value();
		if (iTemp != curveQuality)
		{
			curveQuality = iTemp;
			TCUserDefaults::setLongForKey(iTemp, CURVE_QUALITY_KEY);
		}
	}
	bTemp = !panel->lowQualityStudsButton->state();
	if (bTemp != qualityStuds)
	{
		qualityStuds = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0, QUALITY_STUDS_KEY);
	}
	bTemp = ! panel->hiresPrimitivesButton->state();
	if (bTemp != hiresPrimitives)
	{
		hiresPrimitives = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0,  HI_RES_PRIMITIVES_KEY);
	}
	if (modelViewer)
	{
		modelViewer->setAllowPrimitiveSubstitution(allowPrimitiveSubstitution);
		modelViewer->setTextureStuds(textureStuds);
		modelViewer->setTextureFilterType(textureFilterType);
		modelViewer->setCurveQuality(curveQuality);
		modelViewer->setQualityStuds(qualityStuds);
		modelViewer->setHiResPrimitives(hiresPrimitives);
	}
}

void Preferences::doApply(void)
{
	doGeneralApply();
	doGeometryApply();
	doEffectsApply();
	doPrimitivesApply();
	panel->applyButton->setEnabled(false);
	if (modelWidget)
	{
		modelWidget->doApply();
		setupDefaultRotationMatrix();
	}
}

void Preferences::doCancel(void)
{
	loadSettings();
	reflectSettings();
}

bool Preferences::getAllowPrimitiveSubstitution(void)
{
	return allowPrimitiveSubstitution;
}

void Preferences::setButtonState(QButton *button, bool state)
{
	bool buttonState = button->state() == QButton::On ? true : false;

	if (state != buttonState)
	{
		button->toggle();
	}
}

void Preferences::setRangeValue(QRangeControl *rangeControl, int value)
{
	int rangeValue = rangeControl->value();

	if (value != rangeValue)
	{
		rangeControl->setValue(value);
	}
}

void Preferences::setupColorButton(TCColorButton *button, int colorNumber)
{
	int r, g, b;

	getRGB(colorNumber, r, g, b);
	button->setColor(QColor(r, g, b));
}

void Preferences::loadSettings(void)
{
	loadGeneralSettings();
	loadGeometrySettings();
	loadEffectsSettings();
	loadPrimitivesSettings();
	loadOtherSettings();
}

void Preferences::loadGeneralSettings(void)
{
	loadDefaultGeneralSettings();
	lineSmoothing = TCUserDefaults::longForKey(LINE_SMOOTHING_KEY,
		lineSmoothing) != 0;
	showFPS = TCUserDefaults::longForKey(SHOW_FPS_KEY, (long)showFPS) != 0;
	showErrors = TCUserDefaults::longForKey(SHOW_ERRORS_KEY, (long)showErrors,
		false) != 0;
	processLdconfigLdr = TCUserDefaults::longForKey(PROCESS_LDCONFIG_KEY,
		(long)processLdconfigLdr, false) != 0;
	backgroundColor = (int)TCUserDefaults::longForKey(BACKGROUND_COLOR_KEY,
		(long)backgroundColor);
	defaultColor = (int)TCUserDefaults::longForKey(DEFAULT_COLOR_KEY,
		(long)defaultColor);
	defaultColorNumber =
		(int)TCUserDefaults::longForKey(DEFAULT_COLOR_NUMBER_KEY,
		defaultColorNumber);
	fieldOfView = TCUserDefaults::longForKey(FOV_KEY, fieldOfView);
}

void Preferences::loadGeometrySettings(void)
{
	loadDefaultGeometrySettings();
	seams = TCUserDefaults::longForKey(SEAMS_KEY, seams);
	seamWidth = TCUserDefaults::longForKey(SEAM_WIDTH_KEY, seamWidth);
	wireframe = TCUserDefaults::longForKey(WIREFRAME_KEY, (long)wireframe) != 0;
	wireframeFog = TCUserDefaults::longForKey(WIREFRAME_FOG_KEY,
		(long)wireframeFog) != 0;
        wireframeRemoveHiddenLines=TCUserDefaults::longForKey(REMOVE_HIDDEN_LINES_KEY, 
		(long)wireframeRemoveHiddenLines) !=0;
	wireframeThickness = TCUserDefaults::longForKey(WIREFRAME_THICKNESS_KEY,
		wireframeThickness);
	edgeLines = TCUserDefaults::longForKey(SHOWS_HIGHLIGHT_LINES_KEY,
		(long)edgeLines) != 0;
	conditionalLines =
		TCUserDefaults::longForKey(CONDITIONAL_HIGHLIGHTS_KEY,
		(long)conditionalLines) != 0;
	conditionalShowAll =
		TCUserDefaults::longForKey(SHOW_ALL_TYPE5_KEY,
		(long)conditionalShowAll) != 0;
	conditionalShowControlPts =
		TCUserDefaults::longForKey(SHOW_TYPE5_CONTROL_POINTS_KEY,
		(long)conditionalShowControlPts) != 0;
	edgesOnly = TCUserDefaults::longForKey(EDGES_ONLY_KEY,
		(long)edgesOnly) != 0;
	polygonOffset = TCUserDefaults::longForKey(POLYGON_OFFSET_KEY,
		(long)polygonOffset) != 0;
	blackEdgeLines = TCUserDefaults::longForKey(BLACK_HIGHLIGHTS_KEY,
		(long)blackEdgeLines) != 0;
	edgeThickness = TCUserDefaults::longForKey(EDGE_THICKNESS_KEY,
		edgeThickness);
}

void Preferences::loadEffectsSettings(void)
{
	loadDefaultEffectsSettings();
	lighting = TCUserDefaults::longForKey(LIGHTING_KEY, (long)lighting)
		!= 0;
	qualityLighting = TCUserDefaults::longForKey(QUALITY_LIGHTING_KEY,
		(long)qualityLighting) != 0;
	subduedLighting = TCUserDefaults::longForKey(SUBDUED_LIGHTING_KEY,
		(long)subduedLighting) != 0;
	specular = TCUserDefaults::longForKey(SPECULAR_KEY, (long)specular)
		!= 0;
	alternateLighting = TCUserDefaults::longForKey(ONE_LIGHT_KEY,
		(long)alternateLighting) != 0;
	stereoMode = (LDVStereoMode)TCUserDefaults::longForKey(STEREO_MODE_KEY,
		(long)stereoMode);
	stereoEyeSpacing = TCUserDefaults::longForKey(STEREO_SPACING_KEY,
		stereoEyeSpacing);
	cutawayMode = (LDVCutawayMode)TCUserDefaults::longForKey(CUTAWAY_MODE_KEY,
		(long)cutawayMode);
	cutawayAlpha = TCUserDefaults::longForKey(CUTAWAY_ALPHA_KEY, cutawayAlpha);
	cutawayThickness = TCUserDefaults::longForKey(CUTAWAY_THICKNESS_KEY,
		cutawayThickness);
	sortTransparent = TCUserDefaults::longForKey(SORT_KEY,
		(long)sortTransparent) != 0;
	stipple = TCUserDefaults::longForKey(STIPPLE_KEY, (long)stipple) != 0;
	flatShading = TCUserDefaults::longForKey(FLAT_SHADING_KEY,
		(long)flatShading) != 0;
	smoothing = TCUserDefaults::longForKey(PERFORM_SMOOTHING_KEY,
		(long)smoothing) != 0;
}

void Preferences::loadPrimitivesSettings(void)
{
	loadDefaultPrimitivesSettings();
	allowPrimitiveSubstitution = TCUserDefaults::longForKey(
		PRIMITIVE_SUBSTITUTION_KEY, (long)allowPrimitiveSubstitution) != 0;
	textureStuds = TCUserDefaults::longForKey(TEXTURE_STUDS_KEY,
		(long)textureStuds) != 0;
	textureFilterType = TCUserDefaults::longForKey(TEXTURE_FILTER_TYPE_KEY,
		textureFilterType);
	curveQuality = TCUserDefaults::longForKey(CURVE_QUALITY_KEY, curveQuality);
	qualityStuds = TCUserDefaults::longForKey(QUALITY_STUDS_KEY,
		(long)qualityStuds) != 0;
	hiresPrimitives = TCUserDefaults::longForKey(HI_RES_PRIMITIVES_KEY,
		(long)hiresPrimitives) !=0;
}

void Preferences::loadOtherSettings(void)
{
	loadDefaultOtherSettings();
	statusBar = TCUserDefaults::longForKey(STATUS_BAR_KEY, (long)statusBar,
		false) != 0;
	windowWidth = TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, 640, false);
	windowHeight = TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, 480, false);
}

void Preferences::loadDefaultGeneralSettings(void)
{
	lineSmoothing = false;
	showFPS = false;
	showErrors = true;
	processLdconfigLdr = true;
	backgroundColor = 0;
	defaultColor = 0x999999;
	defaultColorNumber = -1;
	fieldOfView = 45;
	// User colors?
}

void Preferences::loadDefaultGeometrySettings(void)
{
	seams = true;
	seamWidth = 50;
	wireframe = false;
	wireframeFog = false;
	wireframeRemoveHiddenLines=false;
	wireframeThickness = 1;
	edgeLines = false;
	conditionalLines = false;
	conditionalShowAll = false;
	conditionalShowControlPts = false;
	edgesOnly = false;
	polygonOffset = true;
	blackEdgeLines = false;
	edgeThickness = 1;
}

void Preferences::loadDefaultEffectsSettings(void)
{
	lighting = true;
	qualityLighting = false;
	subduedLighting = false;
	specular = true;
	alternateLighting = false;
	stereoMode = LDVStereoNone;
	stereoEyeSpacing = 50;
	cutawayMode = LDVCutawayNormal;
	cutawayAlpha = 100;
	cutawayThickness = 1;
	sortTransparent = true;
	stipple = false;
	flatShading = false;
	smoothing = true;
}

void Preferences::loadDefaultPrimitivesSettings(void)
{
	allowPrimitiveSubstitution = true;
	textureStuds = true;
	textureFilterType = GL_LINEAR_MIPMAP_LINEAR;
	curveQuality = 2;
	qualityStuds = false;
    hiresPrimitives = false;
}

void Preferences::loadDefaultOtherSettings(void)
{
	statusBar = true;
}

void Preferences::reflectSettings(void)
{
	reflectGeneralSettings();
	reflectGeometrySettings();
	reflectEffectsSettings();
	reflectPrimitivesSettings();
}

void Preferences::reflectGeneralSettings(void)
{
	setButtonState(panel->aaLinesButton, lineSmoothing);
	setButtonState(panel->frameRateButton, showFPS);
	setButtonState(panel->showErrorsButton, showErrors);
	setButtonState(panel->processLdconfigLdrButton, processLdconfigLdr);
	setupColorButton(panel->backgroundColorButton, backgroundColor);
	setupColorButton(panel->defaultColorButton, defaultColor);
	setRangeValue(panel->fieldOfViewSpin, fieldOfView);
}

void Preferences::reflectGeometrySettings(void)
{
	setButtonState(panel->seamWidthButton, seams);
	setRangeValue(panel->seamWidthSpin, seamWidth);
//	panel->seamWidthSpin->setValue(seamWidth);
	reflectWireframeSettings();
//	panel->wireframeThicknessSlider->setValue(wireframeThickness);
	setButtonState(panel->edgeLinesButton, edgeLines);
	if (edgeLines)
	{
		enableEdgeLines();
	}
	else
	{
		disableEdgeLines();
	}
	setRangeValue(panel->edgeThicknessSlider, edgeThickness);
//	panel->edgeThicknessSlider->setValue(edgeThickness);
}

void Preferences::reflectWireframeSettings(void)
{
	setButtonState(panel->wireframeButton, wireframe);
	setRangeValue(panel->wireframeThicknessSlider, wireframeThickness);
	if (wireframe)
	{
		enableWireframe();
	}
	else
	{
		disableWireframe();
	}
}

void Preferences::reflectEffectsSettings(void)
{
	setButtonState(panel->lightingButton, lighting);
	if (lighting)
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
	if (stereoMode != LDVStereoNone)
	{
		setButtonState(panel->stereoButton, true);
		enableStereo();
	}
	else
	{
		setButtonState(panel->stereoButton, false);
		disableStereo();
	}
	panel->stereoAmountSlider->setValue(stereoEyeSpacing);
	if (cutawayMode != LDVCutawayNormal)
	{
		setButtonState(panel->wireframeCutawayButton, true);
		enableWireframeCutaway();
	}
	else
	{
		setButtonState(panel->wireframeCutawayButton, false);
		disableWireframeCutaway();
	}
	panel->cutawayOpacitySlider->setValue(cutawayAlpha);
	panel->cutawayThicknessSlider->setValue(cutawayThickness);
	setButtonState(panel->sortTransparencyButton, sortTransparent);
	setButtonState(panel->stippleTransparencyButton, stipple);
	setButtonState(panel->flatShadingButton, flatShading);
	setButtonState(panel->smoothCurvesButton, smoothing);
}

void Preferences::reflectPrimitivesSettings(void)
{
	setButtonState(panel->primitiveSubstitutionButton,
		allowPrimitiveSubstitution);
	if (allowPrimitiveSubstitution)
	{
		enablePrimitiveSubstitution();
	}
	else
	{
		disablePrimitiveSubstitution();
	}
	panel->curveQualitySlider->setValue(curveQuality);
	setButtonState(panel->lowQualityStudsButton, !qualityStuds);
	setButtonState(panel->hiresPrimitivesButton, !hiresPrimitives);
}

void Preferences::doResetGeneral(void)
{
	loadDefaultGeneralSettings();
	reflectGeneralSettings();
}

void Preferences::doResetGeometry(void)
{
	loadDefaultGeometrySettings();
	reflectGeometrySettings();
}

void Preferences::doResetEffects(void)
{
	loadDefaultEffectsSettings();
	reflectEffectsSettings();
}

void Preferences::doResetPrimitives(void)
{
	loadDefaultPrimitivesSettings();
	reflectPrimitivesSettings();
}

void Preferences::getRGB(int color, int &r, int &g, int &b)
{
	color = htonl(color);
	r = (color >> 24) & 0xFF;
	g = (color >> 16) & 0xFF;
	b = (color >> 8) & 0xFF;
}

void Preferences::setStatusBar(bool value)
{
	if (value != statusBar)
	{
		statusBar = value;
		TCUserDefaults::setLongForKey(statusBar ? 1 : 0, STATUS_BAR_KEY, false);
	}
}

char *Preferences::getLastOpenPath(char *pathKey)
{
	char *path;

	if (!pathKey)
	{
		pathKey = LAST_OPEN_PATH_KEY;
	}
	path = TCUserDefaults::stringForKey(pathKey, NULL, false);
	if (!path)
	{
		path = "/dos/c/ldraw";
	}
	return path;
}

void Preferences::setLastOpenPath(const char *path, char *pathKey)
{
	if (!pathKey)
	{
		pathKey = LAST_OPEN_PATH_KEY;
	}
	TCUserDefaults::setStringForKey(path, pathKey, false);
}

char *Preferences::getLDrawDir(void)
{
	return TCUserDefaults::stringForKey(LDRAWDIR_KEY, NULL, false);
}

void Preferences::setLDrawDir(const char *path)
{
	TCUserDefaults::setStringForKey(path, LDRAWDIR_KEY, false);
}

long Preferences::getMaxRecentFiles(void)
{
	return TCUserDefaults::longForKey(MAX_RECENT_FILES_KEY, 10, false);
}

const QString &Preferences::getRecentFileKey(int index)
{
	static QString key;

	key.sprintf("%s/File%02d", RECENT_FILES_KEY, index);
	return key;
}

char *Preferences::getRecentFile(int index)
{
	return TCUserDefaults::stringForKey(getRecentFileKey(index), NULL, false);
}

void Preferences::setRecentFile(int index, char *filename)
{
	if (filename)
	{
		TCUserDefaults::setStringForKey(filename, getRecentFileKey(index), false);
	}
	else
	{
		TCUserDefaults::removeValue(getRecentFileKey(index), false);
	}
}

LDVPollMode Preferences::getPollMode(void)
{
	return (LDVPollMode)TCUserDefaults::longForKey(POLL_KEY, 0, false);
}

void Preferences::setPollMode(LDVPollMode value)
{
	TCUserDefaults::setLongForKey(value, POLL_KEY, false);
}

LDVViewMode Preferences::getViewMode(void)
{
	return (LDVViewMode)TCUserDefaults::longForKey(VIEW_MODE_KEY, 0 , false);
}

void Preferences::setViewMode(LDVViewMode value)
{
	TCUserDefaults::setLongForKey(value, VIEW_MODE_KEY, false);
}

void Preferences::setWindowSize(int width, int height)
{
	if (width != windowWidth)
	{
		windowWidth = width;
		TCUserDefaults::setLongForKey(width, WINDOW_WIDTH_KEY, false);
	}
	if (height != windowHeight)
	{
		windowHeight = height;
		TCUserDefaults::setLongForKey(height, WINDOW_HEIGHT_KEY, false);
	}
}

int Preferences::getWindowWidth(void)
{
	return windowWidth;
}

int Preferences::getWindowHeight(void)
{
	return windowHeight;
}

void Preferences::doWireframeCutaway(bool value)
{
	if (value)
	{
		enableWireframeCutaway();
	}
	else
	{
		disableWireframeCutaway();
	}
}

void Preferences::doLighting(bool value)
{
	if (value)
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
}

void Preferences::doStereo(bool value)
{
	if (value)
	{
		enableStereo();
	}
	else
	{
		disableStereo();
	}
}

void Preferences::doWireframe(bool value)
{
	if (value)
	{
		enableWireframe();
	}
	else
	{
		disableWireframe();
	}
}

void Preferences::doEdgeLines(bool value)
{
	if (value)
	{
		enableEdgeLines();
	}
	else
	{
		disableEdgeLines();
	}
}

void Preferences::doConditionalShow(bool value)
{
    if (value)
    {
        enableConditionalShow();
	}
	else
	{
		disableConditionalShow();
	}
}

void Preferences::doPrimitiveSubstitution(bool value)
{
	if (value)
	{
		enablePrimitiveSubstitution();
	}
	else
	{
		disablePrimitiveSubstitution();
	}
}

void Preferences::doTextureStuds(bool value)
{
	if (value)
	{
		enableTextureStuds();
	}
	else
	{
		disableTextureStuds();
	}
}

void Preferences::enableWireframeCutaway(void)
{
	panel->colorCutawayButton->setEnabled(true);
	panel->monochromeCutawayButton->setEnabled(true);
	panel->cutawayOpacitySlider->setEnabled(true);
	panel->cutawayThicknessSlider->setEnabled(true);
	panel->cutawayOpacityLabel->setEnabled(true);
	panel->cutawayThicknessLabel->setEnabled(true);
	setButtonState(panel->colorCutawayButton, false);
	setButtonState(panel->monochromeCutawayButton, false);
	switch (cutawayMode)
	{
	case LDVCutawayNormal:
	case LDVCutawayWireframe:
		panel->colorCutawayButton->toggle();
		break;
	case LDVCutawayStencil:
		panel->monochromeCutawayButton->toggle();
		break;
	}
}

void Preferences::enableLighting(void)
{
	panel->qualityLightingButton->setEnabled(true);
	panel->subduedLightingButton->setEnabled(true);
	panel->specularLightingButton->setEnabled(true);
	panel->alternateLightingButton->setEnabled(true);
	setButtonState(panel->qualityLightingButton, qualityLighting);
	setButtonState(panel->subduedLightingButton, subduedLighting);
	setButtonState(panel->specularLightingButton, specular);
	setButtonState(panel->alternateLightingButton, alternateLighting);
}

void Preferences::enableStereo(void)
{
	panel->crossEyedStereoButton->setEnabled(true);
	panel->parallelStereoButton->setEnabled(true);
	panel->stereoAmountSlider->setEnabled(true);
	panel->stereoAmountLabel->setEnabled(true);
	setButtonState(panel->crossEyedStereoButton, false);
	setButtonState(panel->parallelStereoButton, false);
	switch (stereoMode)
	{
	case LDVStereoNone:
	case LDVStereoCrossEyed:
		panel->crossEyedStereoButton->toggle();
		break;
	case LDVStereoParallel:
		panel->parallelStereoButton->toggle();
		break;
	default:
		break;
	}
}

void Preferences::enableWireframe(void)
{
	panel->wireframeFogButton->setEnabled(true);
	panel->wireframeRemoveHiddenLineButton->setEnabled(true);
	panel->wireframeThicknessSlider->setEnabled(true);
	panel->wireframeThicknessLabel->setEnabled(true);
	setButtonState(panel->wireframeFogButton, wireframeFog);
    setButtonState(panel->wireframeRemoveHiddenLineButton, wireframeRemoveHiddenLines);
}

void Preferences::enableEdgeLines(void)
{
	panel->conditionalLinesButton->setEnabled(true);
	panel->edgesOnlyButton->setEnabled(true);
	panel->highQualityLinesButton->setEnabled(true);
	panel->alwaysBlackLinesButton->setEnabled(true);
	panel->edgeThicknessLabel->setEnabled(true);
	panel->edgeThicknessSlider->setEnabled(true);
	setButtonState(panel->conditionalLinesButton, conditionalLines);
	setButtonState(panel->edgesOnlyButton, edgesOnly);
	setButtonState(panel->highQualityLinesButton, polygonOffset);
	setButtonState(panel->alwaysBlackLinesButton, blackEdgeLines);
	if (conditionalLines)
	{
		panel->conditionalShowAllButton->setEnabled(true);
		panel->conditionalShowControlPtsButton->setEnabled(true);
		setButtonState(panel->conditionalShowAllButton, conditionalShowAll);
		setButtonState(panel->conditionalShowControlPtsButton, conditionalShowControlPts);
	}
}

void Preferences::enableConditionalShow(void)
{
	panel->conditionalShowAllButton->setEnabled(true);
	panel->conditionalShowControlPtsButton->setEnabled(true);
	setButtonState(panel->conditionalShowAllButton, conditionalShowAll);
	setButtonState(panel->conditionalShowControlPtsButton, conditionalShowControlPts);
}


void Preferences::enablePrimitiveSubstitution(void)
{
	panel->textureStudsButton->setEnabled(true);
	panel->curveQualityLabel->setEnabled(true);
	panel->curveQualitySlider->setEnabled(true);
	setButtonState(panel->textureStudsButton, textureStuds);
	if (textureStuds)
	{
		enableTextureStuds();
	}
	else
	{
		disableTextureStuds();
	}
}

void Preferences::enableTextureStuds(void)
{
	panel->nearestFilteringButton->setEnabled(true);
	panel->bilinearFilteringButton->setEnabled(true);
	panel->trilinearFilteringButton->setEnabled(true);
	setButtonState(panel->nearestFilteringButton, false);
	setButtonState(panel->bilinearFilteringButton, false);
	setButtonState(panel->trilinearFilteringButton, false);
	switch (textureFilterType)
	{
	case GL_NEAREST_MIPMAP_NEAREST:
		panel->nearestFilteringButton->toggle();
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		panel->bilinearFilteringButton->toggle();
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		panel->trilinearFilteringButton->toggle();
		break;
	}
}

void Preferences::disableWireframeCutaway(void)
{
	panel->colorCutawayButton->setEnabled(false);
	panel->monochromeCutawayButton->setEnabled(false);
	panel->cutawayOpacitySlider->setEnabled(false);
	panel->cutawayThicknessSlider->setEnabled(false);
	panel->cutawayOpacityLabel->setEnabled(false);
	panel->cutawayThicknessLabel->setEnabled(false);
	setButtonState(panel->colorCutawayButton, false);
	setButtonState(panel->monochromeCutawayButton, false);
}

void Preferences::disableLighting(void)
{
	panel->qualityLightingButton->setEnabled(false);
	panel->subduedLightingButton->setEnabled(false);
	panel->specularLightingButton->setEnabled(false);
	panel->alternateLightingButton->setEnabled(false);
	setButtonState(panel->qualityLightingButton, false);
	setButtonState(panel->subduedLightingButton, false);
	setButtonState(panel->specularLightingButton, false);
	setButtonState(panel->alternateLightingButton, false);
}

void Preferences::disableStereo(void)
{
	panel->crossEyedStereoButton->setEnabled(false);
	panel->parallelStereoButton->setEnabled(false);
	panel->stereoAmountSlider->setEnabled(false);
	panel->stereoAmountLabel->setEnabled(false);
	setButtonState(panel->crossEyedStereoButton, false);
	setButtonState(panel->parallelStereoButton, false);
}

void Preferences::disableWireframe(void)
{
	panel->wireframeFogButton->setEnabled(false);
	panel->wireframeRemoveHiddenLineButton->setEnabled(false);
	panel->wireframeThicknessSlider->setEnabled(false);
	panel->wireframeThicknessLabel->setEnabled(false);
	setButtonState(panel->wireframeFogButton, false);
	setButtonState(panel->wireframeRemoveHiddenLineButton, false);
}

void Preferences::disableEdgeLines(void)
{
	panel->conditionalLinesButton->setEnabled(false);
	panel->conditionalShowAllButton->setEnabled(false);
	panel->conditionalShowControlPtsButton->setEnabled(false);
	panel->edgesOnlyButton->setEnabled(false);
	panel->highQualityLinesButton->setEnabled(false);
	panel->alwaysBlackLinesButton->setEnabled(false);
	panel->edgeThicknessLabel->setEnabled(false);
	panel->edgeThicknessSlider->setEnabled(false);
	setButtonState(panel->conditionalLinesButton, false);
	setButtonState(panel->conditionalShowAllButton, false);
	setButtonState(panel->conditionalShowControlPtsButton, false);
	setButtonState(panel->edgesOnlyButton, false);
	setButtonState(panel->highQualityLinesButton, false);
	setButtonState(panel->alwaysBlackLinesButton, false);
}

void Preferences::disableConditionalShow(void)
{
	panel->conditionalShowAllButton->setEnabled(false);
    panel->conditionalShowControlPtsButton->setEnabled(false);
    setButtonState(panel->conditionalShowAllButton, false);
    setButtonState(panel->conditionalShowControlPtsButton, false);
}

void Preferences::disablePrimitiveSubstitution(void)
{
	panel->textureStudsButton->setEnabled(false);
	panel->curveQualityLabel->setEnabled(false);
	panel->curveQualitySlider->setEnabled(false);
	setButtonState(panel->textureStudsButton, false);
	disableTextureStuds();
}

void Preferences::disableTextureStuds(void)
{
	panel->nearestFilteringButton->setEnabled(false);
	panel->bilinearFilteringButton->setEnabled(false);
	panel->trilinearFilteringButton->setEnabled(false);
	setButtonState(panel->nearestFilteringButton, false);
	setButtonState(panel->bilinearFilteringButton, false);
	setButtonState(panel->trilinearFilteringButton, false);
}

char *Preferences::getErrorKey(int errorNumber)
{
	static char key[128];

	sprintf(key, "%s/Error%02d", SHOW_ERRORS_KEY, errorNumber);
	return key;
}

void Preferences::setShowError(int errorNumber, bool value)
{
	TCUserDefaults::setLongForKey(value ? 1 : 0, getErrorKey(errorNumber),
		false);
}

bool Preferences::getShowError(int errorNumber)
{
	return TCUserDefaults::longForKey(getErrorKey(errorNumber), 1, false) != 0;
}

void Preferences::setupDefaultRotationMatrix(void)
{
    char *value = TCUserDefaults::stringForKey(DEFAULT_LAT_LONG_KEY);
                                                                                                                                                             
    if (value)
    {
        float latitude;
        float longitude;
                                                                                                                                                             
        if (sscanf(value, "%f,%f", &latitude, &longitude) == 2)
        {
            float leftMatrix[16];
            float rightMatrix[16];
            float resultMatrix[16];
            float cosTheta;
            float sinTheta;
                                                                                                                                                             
            TCVector::initIdentityMatrix(leftMatrix);
            TCVector::initIdentityMatrix(rightMatrix);
            latitude = (float)deg2rad(latitude);
            longitude = (float)deg2rad(longitude);
                                                                                                                                                             
            // First, apply latitude by rotating around X.
            cosTheta = (float)cos(latitude);
            sinTheta = (float)sin(latitude);
            rightMatrix[5] = cosTheta;
            rightMatrix[6] = sinTheta;
            rightMatrix[9] = -sinTheta;
            rightMatrix[10] = cosTheta;
            TCVector::multMatrix(leftMatrix, rightMatrix, resultMatrix);
                                                                                                                                                             
            memcpy(leftMatrix, resultMatrix, sizeof(leftMatrix));
            TCVector::initIdentityMatrix(rightMatrix);
                                                                                                                                                             
            // Next, apply longitude by rotating around Y.
            cosTheta = (float)cos(longitude);
            sinTheta = (float)sin(longitude);
            rightMatrix[0] = cosTheta;
            rightMatrix[2] = -sinTheta;
            rightMatrix[8] = sinTheta;
            rightMatrix[10] = cosTheta;
            TCVector::multMatrix(leftMatrix, rightMatrix, resultMatrix);
                                                                                                                                                             
            modelViewer->setDefaultRotationMatrix(resultMatrix);
        }
        delete value;
    }
    else
    {
        value = TCUserDefaults::stringForKey(DEFAULT_MATRIX_KEY);
        if (value)
        {
            float matrix[16];
                                                                                                                                                             
/*
            if (sscanf(value, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &matrix[0], &matrix[4], &matrix[8], &matrix[12],
                &matrix[1], &matrix[5], &matrix[9], &matrix[13],
                &matrix[2], &matrix[6], &matrix[10], &matrix[14],
                &matrix[3], &matrix[7], &matrix[11], &matrix[15]) == 16)
            {
                modelViewer->setDefaultRotationMatrix(matrix);
            }
            else
            {
*/
            memset(matrix, 0, sizeof(matrix));
            matrix[15] = 1.0f;
            if (sscanf(value, "%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &matrix[0], &matrix[4], &matrix[8],
                &matrix[1], &matrix[5], &matrix[9],
                &matrix[2], &matrix[6], &matrix[10]) == 9)
            {
                modelViewer->setDefaultRotationMatrix(matrix);
            }
//          }
            delete value;
        }
    }
}

