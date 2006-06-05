#include "qt4wrapper.h"
#include <LDLoader/LDLModel.h>
#include <LDLoader/LDLPalette.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include "UserDefaultsKeys.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCWebClient.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qcolor.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <TCFoundation/TCLocalStrings.h>
#include <netinet/in.h>
#include <qtextedit.h>
#include "ModelViewerWidget.h"
#include "Preferences.h"
#include "PreferencesPanel.h"
#include <LDLib/LDPreferences.h>
#include <LDLib/LDrawModelViewer.h>

#define DEFAULT_PREF_SET TCLocalStrings::get("DefaultPrefSet")

Preferences::Preferences(ModelViewerWidget *modelWidget)
	:modelWidget(modelWidget),
	modelViewer(modelWidget->getModelViewer()),
	ldPrefs(new LDPreferences(modelViewer)),
	panel(new PreferencesPanel),
	checkAbandon(true)
{
	modelViewer = modelWidget->getModelViewer();
	panel->setPreferences(this);
	panel->resize(10, 10);
	loadSettings();
	ldPrefs->applySettings();
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

void Preferences::doPrefSetsApply(void)
{
	TCStringArray *oldPrefSetNames = TCUserDefaults::getAllSessionNames();
	int i;
	uint b;
	int count = oldPrefSetNames->getCount();
//	char *prefSetName = NULL;
	const char *sessionName = TCUserDefaults::getSessionName();
	bool changed = false;

	for (i = 0; i < count; i++)
	{
		char *oldPrefSetName = oldPrefSetNames->stringAtIndex(i);
		int index=-1;
		for(b = 0; b < panel->preferenceSetList->count(); b++)
		{
			if (strcmp(oldPrefSetNames->stringAtIndex(i),
						panel->preferenceSetList->text(b).ascii()) == 0) 
			{
				index = b;
			}	
		}
		if (index == -1)
		{
			TCUserDefaults::removeSession(oldPrefSetName);
		}
	}
	count=panel->preferenceSetList->count();
	for(i = 1; i < count; i++)
	{
		if (oldPrefSetNames->indexOfString(getPrefSet(i)) < 0)
		{
			TCUserDefaults::setSessionName(getPrefSet(i));
		}
	}
	oldPrefSetNames->release();
	if (getSelectedPrefSet() && 
		(strcmp(getSelectedPrefSet(), DEFAULT_PREF_SET) == 0))
	{
		if (sessionName && sessionName[0])
        {
			TCUserDefaults::setSessionName(NULL, PREFERENCE_SET_KEY);
			changed = true;
		}
	}
	else
	{
		if (!sessionName || strcmp(sessionName, getSelectedPrefSet()) != 0)
		{
			TCUserDefaults::setSessionName(getSelectedPrefSet(),
				PREFERENCE_SET_KEY);
			changed = true;
		}
	}
	if (changed)
	{
		loadSettings();
		reflectSettings();

		doGeneralApply();
    	doGeometryApply();
    	doEffectsApply();
    	doPrimitivesApply();
    	panel->applyButton->setEnabled(false);
    	if (modelWidget)
    	{
        	modelWidget->reflectSettings();
        	modelWidget->doApply();
        	setupDefaultRotationMatrix();
    	}
    	checkAbandon = true;
	
	}
}

void Preferences::doGeneralApply(void)
{
	QColor cTemp;
	int iTemp;
	int r, g, b;

	ldPrefs->setLineSmoothing(panel->aaLinesButton->state());
	ldPrefs->setShowFps(panel->frameRateButton->state());
	if (modelWidget)
		modelWidget->setShowFPS(ldPrefs->getShowFps());
	ldPrefs->setShowErrors(panel->showErrorsButton->state());
	ldPrefs->setProcessLdConfig(panel->processLdconfigLdrButton->state());
	cTemp = panel->backgroundColorButton->backgroundColor();
	cTemp.rgb(&r, &g, &b);
	iTemp = htonl(LDLPalette::colorForRGBA(r, g, b, 0));
	ldPrefs->setBackgroundColor((TCULong)iTemp);
	cTemp = panel->defaultColorButton->backgroundColor();
	cTemp.rgb(&r, &g, &b);
	iTemp = htonl(LDLPalette::colorForRGBA(r, g, b, 0));
	ldPrefs->setDefaultColor((TCULong)iTemp);
	ldPrefs->setFov(panel->fieldOfViewSpin->value());
	ldPrefs->setMemoryUsage(panel->memoryUsageBox->currentItem());
	ldPrefs->setTransDefaultColor(panel->transparentButton->state());
	ldPrefs->applyGeneralSettings();
	ldPrefs->commitGeneralSettings();

/*
	bool bTemp;

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
	cTemp = panel->backgroundColorButton->backgroundColor();
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
	cTemp = panel->defaultColorButton->backgroundColor();
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
	iTemp = panel->memoryUsageBox->currentItem();
	if (iTemp != memoryUsage)
	{
		memoryUsage = iTemp;
		TCUserDefaults::setLongForKey(iTemp, MEMORY_USAGE_KEY);
	}
	bTemp = panel->transparentButton->state();
	if (bTemp != transDefaultColor)
	{
		transDefaultColor = bTemp;
		TCUserDefaults::setLongForKey(bTemp ? 1 : 0,
			TRANS_DEFAULT_COLOR_KEY, false);
	}
	if (modelWidget)
	{
		modelWidget->setShowFPS(showFPS);
	}
	if (modelViewer)
	{
		modelViewer->setLineSmoothing(lineSmoothing);
		modelViewer->setBackgroundRGB(br, bg, bb);
		modelViewer->setDefaultRGB((TCByte)dr, (TCByte)dg, (TCByte)db,
			transDefaultColor);
		modelViewer->setDefaultColorNumber(defaultColorNumber);
		modelViewer->setFov((TCFloat)fieldOfView);
		modelViewer->setProcessLDConfig(processLdconfigLdr);
		modelViewer->setMemoryUsage(memoryUsage);
	}
*/
}

void Preferences::doGeometryApply(void)
{
	ldPrefs->setUseSeams(panel->seamWidthButton->state());
	ldPrefs->setSeamWidth(panel->seamWidthSpin->value());
	ldPrefs->setDrawWireframe(panel->wireframeButton->state());
	if (ldPrefs->getDrawWireframe())
	{
		ldPrefs->setUseWireframeFog(panel->wireframeFogButton->state());
		ldPrefs->setRemoveHiddenLines(
			panel->wireframeRemoveHiddenLineButton->state());
	}
	ldPrefs->setBfc(panel->enableBFCButton->state());
	if (ldPrefs->getBfc())
	{
		ldPrefs->setRedBackFaces(panel->bfcRedBackFaceButton->state());
		ldPrefs->setGreenFrontFaces(panel->bfcGreenFrontFaceButton->state());
	}
	ldPrefs->setWireframeThickness(panel->wireframeThicknessSlider->value());
	ldPrefs->setShowHighlightLines(panel->edgeLinesButton->state());
	if (ldPrefs->getShowHighlightLines())
	{
		ldPrefs->setEdgesOnly(panel->edgesOnlyButton->state());
		ldPrefs->setDrawConditionalHighlights(
			panel->conditionalLinesButton->state());
		if (ldPrefs->getDrawConditionalHighlights())
		{
			ldPrefs->setShowAllConditionalLines(
				panel->conditionalShowAllButton->state());
			ldPrefs->setShowConditionalControlPoints(
				panel->conditionalShowControlPtsButton->state());
		}
		ldPrefs->setUsePolygonOffset(panel->highQualityLinesButton->state());
		ldPrefs->setBlackHighlights(panel->alwaysBlackLinesButton->state());
		ldPrefs->setEdgeThickness(panel->edgeThicknessSlider->value());
	}
	ldPrefs->applyGeometrySettings();
	ldPrefs->commitGeometrySettings();
}

void Preferences::doEffectsApply(void)
{
	LDVStereoMode smTemp = LDVStereoNone;
	LDVCutawayMode cmTemp = LDVCutawayNormal;

	ldPrefs->setUseLighting(panel->lightingButton->state());
	if (ldPrefs->getUseLighting())
	{
		ldPrefs->setQualityLighting(panel->qualityLightingButton->state());
		ldPrefs->setSubduedLighting(panel->subduedLightingButton->state());
		ldPrefs->setUseSpecular(panel->specularLightingButton->state());
		ldPrefs->setOneLight(panel->alternateLightingButton->state());
	}
	if (!panel->stereoButton->state())
	{
		smTemp = LDVStereoNone;
	}
	else if (panel->crossEyedStereoButton->isChecked())
	{
		smTemp = LDVStereoCrossEyed;
	}
	else if (panel->parallelStereoButton->isChecked())
	{
		smTemp = LDVStereoParallel;
	}
	ldPrefs->setStereoMode(smTemp);
	ldPrefs->setStereoEyeSpacing(panel->stereoAmountSlider->value());
	if (!panel->wireframeCutawayButton->state())
	{
		cmTemp = LDVCutawayNormal;
	}
	else if (panel->colorCutawayButton->isChecked())
	{
		cmTemp = LDVCutawayWireframe;
	}
	else if (panel->monochromeCutawayButton->isChecked())
	{
		cmTemp = LDVCutawayStencil;
	}
	ldPrefs->setCutawayMode(cmTemp);
	ldPrefs->setCutawayAlpha(panel->cutawayOpacitySlider->value());
	ldPrefs->setCutawayThickness(panel->cutawayThicknessSlider->value());
	ldPrefs->setSortTransparent(panel->sortTransparencyButton->state());
	ldPrefs->setUseStipple(panel->stippleTransparencyButton->state());
	ldPrefs->setUseFlatShading(panel->flatShadingButton->state());
	ldPrefs->setPerformSmoothing(panel->smoothCurvesButton->state());
	ldPrefs->applyEffectsSettings();
	ldPrefs->commitEffectsSettings();
}

void Preferences::doPrimitivesApply(void)
{
	ldPrefs->setAllowPrimitiveSubstitution(
		panel->primitiveSubstitutionButton->state());
	if (ldPrefs->getAllowPrimitiveSubstitution())
	{
		ldPrefs->setTextureStuds(panel->textureStudsButton->state());
		if (ldPrefs->getTextureStuds())
		{
			int iTemp = GL_NEAREST_MIPMAP_NEAREST;

			if (panel->nearestFilteringButton->isChecked())
			{
				iTemp = GL_NEAREST_MIPMAP_NEAREST;
			}
			else if (panel->bilinearFilteringButton->isChecked())
			{
				iTemp = GL_LINEAR_MIPMAP_NEAREST;
			}
			else if (panel->trilinearFilteringButton->isChecked())
			{
				iTemp = GL_LINEAR_MIPMAP_LINEAR;
			}
			ldPrefs->setTextureFilterType(iTemp);
		}
		ldPrefs->setCurveQuality(panel->curveQualitySlider->value());
	}
	ldPrefs->setQualityStuds(!panel->lowQualityStudsButton->state());
	ldPrefs->setHiResPrimitives(panel->hiresPrimitivesButton->state());
	ldPrefs->applyPrimitivesSettings();
	ldPrefs->commitPrimitivesSettings();
}

void Preferences::doUpdatesApply()
{
	int  iTemp;

	ldPrefs->setCheckPartTracker(panel->updatesMissingpartsButton->state());
	if (panel->updatesProxyButton->isChecked())
	{
		ldPrefs->setProxyType(2);
	}
	else
	{
		ldPrefs->setProxyType(0);
	}
	if (sscanf(panel->portEdit->text().ascii(),"%i",&iTemp) == 1)
	{
		ldPrefs->setProxyPort(iTemp);
	}
	ldPrefs->setProxyServer(panel->proxyEdit->text().ascii());
	ldPrefs->applyUpdatesSettings();
	ldPrefs->commitUpdatesSettings();
}

void Preferences::doBackgroundColor()
{
	int r,g,b;
	getRGB((int)ldPrefs->getBackgroundColor(), r ,g, b);
	QColor color = QColorDialog::getColor(QColor(r,g,b));
	if(color.isValid())
	{
		panel->backgroundColorButton->setPaletteBackgroundColor(color);
		panel->applyButton->setEnabled(true);
	}
}

void Preferences::doDefaultColor()
{
	int r, g, b, a, i;
	QRgb old[16];

    for (i = 0 ; i < 16; i++)
    {
		old[i] = QColorDialog::customColor(i);
        LDLPalette::getDefaultRGBA(i, r, g, b, a);
        QColorDialog::setCustomColor(i, qRgb(r, g, b));
    }
	getRGB((int)ldPrefs->getDefaultColor(), r ,g, b);
	QColor color = QColorDialog::getColor(QColor(r,g,b));
	if(color.isValid())
	{
		panel->defaultColorButton->setPaletteBackgroundColor(color);
		panel->applyButton->setEnabled(true);
	}
	for (i = 0 ; i <16 ; i++)
		QColorDialog::setCustomColor(i, old[i]);
}

void Preferences::doApply(void)
{
	doGeneralApply();
	doGeometryApply();
	doEffectsApply();
	doPrimitivesApply();
	doUpdatesApply();
	doPrefSetsApply();
	panel->applyButton->setEnabled(false);
	if (modelWidget)
	{
		modelWidget->reflectSettings();
		modelWidget->doApply();
		setupDefaultRotationMatrix();
	}
	checkAbandon = true;
}

void Preferences::doCancel(void)
{
	loadSettings();
	reflectSettings();
}

bool Preferences::getAllowPrimitiveSubstitution(void)
{
	return ldPrefs->getAllowPrimitiveSubstitution();
}

void Preferences::setButtonState(QCheckBox *button, bool state)
{
	bool buttonState = button->isChecked();

	if (state != buttonState)
	{
		button->toggle();
	}
}

void Preferences::setButtonState(QRadioButton *button, bool state)
{
	bool buttonState = button->isChecked();

	if (state != buttonState)
	{
		button->toggle();
	}
}

void Preferences::setRangeValue(QSpinBox *rangeControl, int value)
{
	int rangeValue = rangeControl->value();

	if (value != rangeValue)
	{
		rangeControl->setValue(value);
	}
}

void Preferences::setRangeValue(QSlider *rangeControl, int value)
{
	int rangeValue = rangeControl->value();

	if (value != rangeValue)
	{
		rangeControl->setValue(value);
	}
}

void Preferences::loadSettings(void)
{
	ldPrefs->loadSettings();
	loadOtherSettings();
/*
	loadGeneralSettings();
	loadGeometrySettings();
	loadEffectsSettings();
	loadUpdatesSettings();
	loadPrimitivesSettings();
*/
}

/*
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
	transDefaultColor = TCUserDefaults::longForKey(TRANS_DEFAULT_COLOR_KEY,
        transDefaultColor) != 0;
	memoryUsage = TCUserDefaults::longForKey(MEMORY_USAGE_KEY,
		(long)memoryUsage);
	if (memoryUsage < 0 || memoryUsage > 2)
	{
		memoryUsage = 2;
		TCUserDefaults::setLongForKey(2, MEMORY_USAGE_KEY);
	}
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
	bfc = TCUserDefaults::longForKey(BFC_KEY, (long)bfc) != 0;
	bfcRedBackFace = TCUserDefaults::longForKey(RED_BACK_FACES_KEY, 
		(long)bfcRedBackFace) !=0;
        bfcGreenFrontFace = TCUserDefaults::longForKey(GREEN_FRONT_FACES_KEY,
                (long)bfcGreenFrontFace) !=0;
	edgeLines = TCUserDefaults::longForKey(SHOW_HIGHLIGHT_LINES_KEY,
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

void Preferences::loadUpdatesSettings(void)
{
	loadDefaultUpdatesSettings();
	proxyType = TCUserDefaults::longForKey(PROXY_TYPE_KEY, proxyType, false);
	proxyServer = TCUserDefaults::stringForKey(PROXY_SERVER_KEY, NULL, false);
    proxyPort = TCUserDefaults::longForKey(PROXY_PORT_KEY, proxyPort, false);
    checkPartTracker = TCUserDefaults::longForKey(CHECK_PART_TRACKER_KEY,
        (long)checkPartTracker, false) != 0;
}
*/

void Preferences::loadOtherSettings(void)
{
	loadDefaultOtherSettings();
	statusBar = TCUserDefaults::longForKey(STATUS_BAR_KEY, (long)statusBar,
		false) != 0;
	toolBar  = TCUserDefaults::longForKey(TOOLBAR_KEY, (long)toolBar,
        false) != 0;
	windowWidth = TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, 640, false);
	windowHeight = TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, 480, false);
}

/*
void Preferences::loadDefaultGeneralSettings(void)
{
	lineSmoothing = false;
	showFPS = false;
	showErrors = true;
	processLdconfigLdr = true;
	backgroundColor = 0;
	defaultColor = LDLPalette::colorForRGBA(0x99, 0x99, 0x99, 255);
	defaultColorNumber = -1;
	fieldOfView = 45;
	memoryUsage = 2;
	transDefaultColor = false;
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
	bfc = false;
	bfcRedBackFace = false;
        bfcGreenFrontFace = false;
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

void Preferences::loadDefaultUpdatesSettings(void)
{
	proxyType = 0;
	if (proxyServer) delete proxyServer;
	proxyServer = NULL;
	proxyPort = 80;
	checkPartTracker = true;
}
*/

void Preferences::loadDefaultOtherSettings(void)
{
	statusBar = true;
	toolBar = true;
}

void Preferences::setDrawWireframe(bool value)
{
    if (value != ldPrefs->getDrawWireframe())
    {
		ldPrefs->setDrawWireframe(value, true, true);
		reflectWireframeSettings();
	}
}

void Preferences::setShowsHighlightLines(bool value)
{
	if (value != ldPrefs->getShowHighlightLines())
	{
		ldPrefs->setShowHighlightLines(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setAllowPrimitiveSubstitution(bool value)
{
	if (value != ldPrefs->getAllowPrimitiveSubstitution())
	{
		ldPrefs->setAllowPrimitiveSubstitution(value, true, true);
		reflectPrimitivesSettings();
	}
}

void Preferences::setUseLighting(bool value)
{
    if (value != ldPrefs->getUseLighting())
	{
		ldPrefs->setUseLighting(value, true, true);
		reflectEffectsSettings();
	}
}

void Preferences::setUseSeams(bool value)
{
    if (value != ldPrefs->getUseSeams())
	{
		ldPrefs->setUseSeams(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::reflectSettings(void)
{
	reflectGeneralSettings();
	reflectGeometrySettings();
	reflectEffectsSettings();
	reflectPrimitivesSettings();
	reflectUpdatesSettings();
	setupPrefSetsList();
}

void Preferences::reflectGeneralSettings(void)
{
	int r, g, b;
	setButtonState(panel->aaLinesButton, ldPrefs->getLineSmoothing());
	setButtonState(panel->frameRateButton, ldPrefs->getShowFps());
	setButtonState(panel->showErrorsButton, ldPrefs->getShowErrors());
	setButtonState(panel->processLdconfigLdrButton,
		ldPrefs->getProcessLdConfig());
	getRGB((int)ldPrefs->getBackgroundColor(),r,g,b);
	panel->backgroundColorButton->setPaletteBackgroundColor(QColor( r, g, b));
	getRGB(ldPrefs->getDefaultColor(),r,g,b);
	panel->defaultColorButton->setPaletteBackgroundColor(QColor( r, g, b ));
	setRangeValue(panel->fieldOfViewSpin, (int)ldPrefs->getFov());
	setButtonState(panel->transparentButton, ldPrefs->getTransDefaultColor());
	panel->memoryUsageBox->setCurrentItem(ldPrefs->getMemoryUsage());
}

void Preferences::reflectGeometrySettings(void)
{
	setButtonState(panel->seamWidthButton, ldPrefs->getUseSeams());
	setRangeValue(panel->seamWidthSpin, ldPrefs->getSeamWidth());
//	panel->seamWidthSpin->setValue(seamWidth);
	reflectWireframeSettings();
	reflectBFCSettings();
//	panel->wireframeThicknessSlider->setValue(wireframeThickness);
	setButtonState(panel->edgeLinesButton, ldPrefs->getShowHighlightLines());
	if (ldPrefs->getShowHighlightLines())
	{
		enableEdgeLines();
	}
	else
	{
		disableEdgeLines();
	}
	setRangeValue(panel->edgeThicknessSlider, ldPrefs->getEdgeThickness());
//	panel->edgeThicknessSlider->setValue(edgeThickness);
}

void Preferences::reflectWireframeSettings(void)
{
	setButtonState(panel->wireframeButton, ldPrefs->getDrawWireframe());
	setRangeValue(panel->wireframeThicknessSlider,
		ldPrefs->getWireframeThickness());
	if (ldPrefs->getDrawWireframe())
	{
		enableWireframe();
	}
	else
	{
		disableWireframe();
	}
}

void Preferences::reflectBFCSettings()
{
	setButtonState(panel->enableBFCButton, ldPrefs->getBfc());
	if (ldPrefs->getBfc())
	{
		enableBFC();
	}
	else
	{
		disableBFC();
	}
}

void Preferences::reflectEffectsSettings(void)
{
	setButtonState(panel->lightingButton, ldPrefs->getUseLighting());
	if (ldPrefs->getUseLighting())
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
	if (ldPrefs->getStereoMode() != LDVStereoNone)
	{
		setButtonState(panel->stereoButton, true);
		enableStereo();
	}
	else
	{
		setButtonState(panel->stereoButton, false);
		disableStereo();
	}
	panel->stereoAmountSlider->setValue(ldPrefs->getStereoEyeSpacing());
	if (ldPrefs->getCutawayMode() != LDVCutawayNormal)
	{
		setButtonState(panel->wireframeCutawayButton, true);
		enableWireframeCutaway();
	}
	else
	{
		setButtonState(panel->wireframeCutawayButton, false);
		disableWireframeCutaway();
	}
	panel->cutawayOpacitySlider->setValue(ldPrefs->getCutawayAlpha());
	panel->cutawayThicknessSlider->setValue(ldPrefs->getCutawayThickness());
	setButtonState(panel->sortTransparencyButton,
		ldPrefs->getSortTransparent());
	setButtonState(panel->stippleTransparencyButton, ldPrefs->getUseStipple());
	setButtonState(panel->flatShadingButton, ldPrefs->getUseFlatShading());
	setButtonState(panel->smoothCurvesButton, ldPrefs->getPerformSmoothing());
}

void Preferences::reflectPrimitivesSettings(void)
{
	setButtonState(panel->primitiveSubstitutionButton,
		ldPrefs->getAllowPrimitiveSubstitution());
	if (ldPrefs->getAllowPrimitiveSubstitution())
	{
		enablePrimitiveSubstitution();
	}
	else
	{
		disablePrimitiveSubstitution();
	}
	panel->curveQualitySlider->setValue(ldPrefs->getCurveQuality());
	setButtonState(panel->lowQualityStudsButton, !ldPrefs->getQualityStuds());
	setButtonState(panel->hiresPrimitivesButton, ldPrefs->getHiResPrimitives());
}

void Preferences::reflectUpdatesSettings(void)
{
	char s[16];
	if (ldPrefs->getProxyType())
	{
		enableProxyServer();
		panel->updatesProxyButton->setChecked(true);
	}
	else
	{
		disableProxyServer();
		panel->updatesNoproxyButton->setChecked(true);
	}
	setButtonState(panel->updatesMissingpartsButton,
		ldPrefs->getCheckPartTracker());
	panel->proxyEdit->setText(ldPrefs->getProxyServer());
	sprintf(s,"%u",ldPrefs->getProxyPort());
	panel->portEdit->setText(s);
}

void Preferences::doResetGeneral(void)
{
	ldPrefs->loadDefaultGeneralSettings();
	reflectGeneralSettings();
}

void Preferences::doResetGeometry(void)
{
	ldPrefs->loadDefaultGeometrySettings();
	reflectGeometrySettings();
}

void Preferences::doResetEffects(void)
{
	ldPrefs->loadDefaultEffectsSettings();
	reflectEffectsSettings();
}

void Preferences::doResetPrimitives(void)
{
	ldPrefs->loadDefaultPrimitivesSettings();
	reflectPrimitivesSettings();
}

void Preferences::doResetUpdates(void)
{
	ldPrefs->loadDefaultUpdatesSettings();
	reflectUpdatesSettings();
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

void Preferences::setToolBar(bool value)
{
    if (value != toolBar)
	{
		toolBar = value;
		 TCUserDefaults::setLongForKey(toolBar ? 1 : 0, TOOLBAR_KEY, false);
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

void Preferences::doProxyServer(bool value)
{
	if (value)
		enableProxyServer();
	else
		disableProxyServer();
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

void Preferences::doSortTransparency(bool value)
{
	if(value)
	{
		setButtonState(panel->stippleTransparencyButton,false);
	}
}

void Preferences::doStippleTransparency(bool value)
{
	if(value)
	{
		setButtonState(panel->sortTransparencyButton,false);
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

void Preferences::doBFC(bool value)
{
	if (value)
	{
		enableBFC();
	}
	else
	{
		disableBFC();
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

void Preferences::doNewPreferenceSet()
{
    bool ok;
    QString name = QInputDialog::getText("LDView New Preference Set", 
                   "Enter name of the new PreferenceSet", QLineEdit::Normal,
                   QString::null, &ok, panel);
    if (ok && !name.isEmpty())
	{
		for(uint i = 0; i < panel->preferenceSetList->count(); i++)
		{
			if (getPrefSet(i) && strcmp(getPrefSet(i), name.ascii())==0)
			{
				QMessageBox::warning(panel,
					TCLocalStrings::get("PrefSetAlreadyExists"),
					TCLocalStrings::get("DuplicateName"),
					QMessageBox::Ok,0);
				return;
			}
		}
		if (name.find('/')!=-1)
		{
			QMessageBox::warning(panel,
				TCLocalStrings::get("PrefSetNameBadChars"),
				TCLocalStrings::get("InvalidName"),
				QMessageBox::Ok,0);
				return;
		}
		panel->preferenceSetList->insertItem(name);
		selectPrefSet(name.ascii());
		return;
	}
	if (name.isEmpty() && ok);
	{
		QMessageBox::warning(panel,
			TCLocalStrings::get("PrefSetNameRequired"),
			TCLocalStrings::get("EmptyName"),
			QMessageBox::Ok,0);
		return;
	}
}

void Preferences::doDelPreferenceSet()
{
	const char *selectedPrefSet = getSelectedPrefSet();
	if (selectedPrefSet)
	{
		int selectedIndex = panel->preferenceSetList->currentItem();
		if (checkAbandon && panel->applyButton->isEnabled())
		{
			if(QMessageBox::warning(panel,
				TCLocalStrings::get("PrefSetAbandonConfirm"),
				TCLocalStrings::get("AbandonChanges"),
				 QMessageBox::Yes, QMessageBox::No)== QMessageBox::Yes)
			{
				abandonChanges();
			}
			else
			{
				return;
			}
		}
		checkAbandon = false;
		panel->preferenceSetList->removeItem(selectedIndex);
		selectedIndex = panel->preferenceSetList->currentItem();
		selectedPrefSet = getPrefSet(selectedIndex);
		selectPrefSet(selectedPrefSet, true);
	}
}

void Preferences::doHotkeyPreferenceSet()
{
	hotKeyIndex = getCurrentHotKey();

	if (hotKeyIndex == -1)
	{
		hotKeyIndex++;
	}
	else if (hotKeyIndex == 0)
	{
		hotKeyIndex = 10;
	}
	QStringList lst;
	lst << TCLocalStrings::get("<None>") << "1" << "2" << "3" << 
	"4" << "5" << "6" << "7" << "8" << "9" << "0";
	bool ok;
	QString res = QInputDialog::getItem(getSelectedPrefSet(), 
			"Select a hot key to automatically select this Preference Set:\nAlt + ",
			lst, hotKeyIndex, FALSE, &ok, panel);
	if (ok)
	{
		hotKeyIndex = lst.findIndex(res.ascii());
		if(hotKeyIndex != -1)
		{
			saveCurrentHotKey();
		}
	}
	
}

char *Preferences::getHotKey(int index)
{
    char key[128];
                                                                                
    sprintf(key, "%s/Key%d", HOT_KEYS_KEY, index);
    return TCUserDefaults::stringForKey(key, NULL, false);
}

int Preferences::getHotKey(const char *currentPrefSetName)
{
    int i;
    int retValue = -1;
                                                                                
    for (i = 0; i < 10 && retValue == -1; i++)
    {
        char *prefSetName = getHotKey(i);
                                                                                
        if (prefSetName)
        {
            if (strcmp(prefSetName, currentPrefSetName) == 0)
            {
                retValue = i;
            }
            delete prefSetName;
        }
    }
    return retValue;
}

void Preferences::performHotKey(int hotKeyIndex)
{
    char *hotKeyPrefSetName = getHotKey(hotKeyIndex);
    bool retValue = false;
    if (hotKeyPrefSetName)
    {
        const char *currentSessionName = TCUserDefaults::getSessionName();
        bool hotKeyIsDefault = strcmp(hotKeyPrefSetName, DEFAULT_PREF_SET) == 0;

        if (currentSessionName)
        {
            if (strcmp(currentSessionName, hotKeyPrefSetName) == 0)
            {
                retValue = true;
            }
        }
        else if (hotKeyIsDefault)
        {
            retValue = true;
        }
        if (!retValue)
        {
            bool changed = false;

            if (hotKeyIsDefault)
            {
                TCUserDefaults::setSessionName(NULL, PREFERENCE_SET_KEY);
                changed = true;
            }
            else
            {
                TCStringArray *sessionNames =
                    TCUserDefaults::getAllSessionNames();

                if (sessionNames->indexOfString(hotKeyPrefSetName) != -1)
                {
                    TCUserDefaults::setSessionName(hotKeyPrefSetName,
                        PREFERENCE_SET_KEY);
                    {
                        changed = true;
                    }
                }
                sessionNames->release();
            }
            if (changed)
            {
                loadSettings();
				reflectSettings();
                doApply();
                retValue = true;
            }
        }
    }
	delete hotKeyPrefSetName;
}


int Preferences::getCurrentHotKey(void)
{
	int retValue = -1;
                                                                                
	if (getSelectedPrefSet())
	{
		return getHotKey(getSelectedPrefSet());
	}
	return retValue;
}

void Preferences::saveCurrentHotKey(void)
{
    int currentHotKey = getCurrentHotKey();
                                                                                
    if (currentHotKey >= 0)
    {
        char key[128];
                                                                                
        sprintf(key, "%s/Key%d", HOT_KEYS_KEY, currentHotKey);
        TCUserDefaults::removeValue(key, false);
    }
    if (hotKeyIndex > 0)
    {
        char key[128];
                                                                                
        sprintf(key, "%s/Key%d", HOT_KEYS_KEY, hotKeyIndex % 10);
        TCUserDefaults::setStringForKey(getSelectedPrefSet(), key, false);
    }
}

void Preferences::abandonChanges(void)
{
	panel->applyButton->setEnabled(false);
	loadSettings();
	reflectSettings();
}

const char *Preferences::getPrefSet(int index)
{
	return panel->preferenceSetList->text(index).ascii();
}

const char *Preferences::getSelectedPrefSet(void)
{
    int selectedIndex = panel->preferenceSetList->currentItem();
	if (selectedIndex!=-1)
	{
		return panel->preferenceSetList->currentText().ascii();
	}
	return NULL;
}
bool Preferences::doPrefSetSelected(bool force)
{
    const char *selectedPrefSet = getSelectedPrefSet();
    bool needToReselect = false;

	if (checkAbandon && panel->applyButton->isEnabled() && !force)
    {
        char *savedSession =
            TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);

        if (!savedSession || !savedSession[0])
        {
            delete savedSession;
            savedSession = copyString(DEFAULT_PREF_SET);
        }
        if (selectedPrefSet && (strcmp(savedSession, selectedPrefSet) != 0))
        {
            needToReselect = true;
            selectPrefSet(NULL, true);
		}
		delete savedSession;
	}
	if (selectedPrefSet)
    {
        bool enabled = true;

        if (needToReselect)
        {
            selectPrefSet(selectedPrefSet);
        }
		if (strcmp(selectedPrefSet, DEFAULT_PREF_SET) == 0)
        {
            enabled = false;
        }
		panel->delPreferenceSetButton->setEnabled(enabled);
    }
	panel->applyButton->setEnabled(true);
	checkAbandon = false;
    return FALSE;
}
	
void Preferences::selectPrefSet(const char *prefSet, bool force)
{
    if (prefSet)
    {
		for (uint i=0;i<panel->preferenceSetList->count();i++)
		{
			if (strcmp(prefSet,panel->preferenceSetList->text(i).ascii())==0)
			{
				panel->preferenceSetList->setCurrentItem(i);
			}
		}
		doPrefSetSelected(force);
	}
    else
    {
        char *savedSession =
            TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);

        if (savedSession && savedSession[0])
        {
            selectPrefSet(savedSession, force);
        }
        else
        {
            selectPrefSet(DEFAULT_PREF_SET, force);
        }
        delete savedSession;
    }
}
void Preferences::setupPrefSetsList(void)
{
    TCStringArray *sessionNames = TCUserDefaults::getAllSessionNames();
    int i;
    int count = sessionNames->getCount();
	panel->preferenceSetList->clear();
    panel->preferenceSetList->insertItem(DEFAULT_PREF_SET);
    for (i = 0; i < count; i++)
    {
        panel->preferenceSetList->insertItem(sessionNames->stringAtIndex(i));
    }
    selectPrefSet();
	sessionNames->release();
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
	switch (ldPrefs->getCutawayMode())
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
	setButtonState(panel->qualityLightingButton, ldPrefs->getQualityLighting());
	setButtonState(panel->subduedLightingButton, ldPrefs->getSubduedLighting());
	setButtonState(panel->specularLightingButton, ldPrefs->getUseSpecular());
	setButtonState(panel->alternateLightingButton, ldPrefs->getOneLight());
}

void Preferences::enableStereo(void)
{
	panel->crossEyedStereoButton->setEnabled(true);
	panel->parallelStereoButton->setEnabled(true);
	panel->stereoAmountSlider->setEnabled(true);
	panel->stereoAmountLabel->setEnabled(true);
	setButtonState(panel->crossEyedStereoButton, false);
	setButtonState(panel->parallelStereoButton, false);
	switch (ldPrefs->getStereoMode())
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
	setButtonState(panel->wireframeFogButton, ldPrefs->getUseWireframeFog());
    setButtonState(panel->wireframeRemoveHiddenLineButton,
		ldPrefs->getRemoveHiddenLines());
}

void Preferences::enableBFC(void)
{
	panel->bfcRedBackFaceButton->setEnabled(true);
        panel->bfcGreenFrontFaceButton->setEnabled(true);
	setButtonState(panel->bfcRedBackFaceButton, ldPrefs->getRedBackFaces());
	setButtonState(panel->bfcGreenFrontFaceButton,
		ldPrefs->getGreenFrontFaces());
}

void Preferences::enableEdgeLines(void)
{
	panel->conditionalLinesButton->setEnabled(true);
	panel->edgesOnlyButton->setEnabled(true);
	panel->highQualityLinesButton->setEnabled(true);
	panel->alwaysBlackLinesButton->setEnabled(true);
	panel->edgeThicknessLabel->setEnabled(true);
	panel->edgeThicknessSlider->setEnabled(true);
	setButtonState(panel->conditionalLinesButton,
		ldPrefs->getDrawConditionalHighlights());
	setButtonState(panel->edgesOnlyButton, ldPrefs->getEdgesOnly());
	setButtonState(panel->highQualityLinesButton,
		ldPrefs->getUsePolygonOffset());
	setButtonState(panel->alwaysBlackLinesButton,
		ldPrefs->getBlackHighlights());
	if (ldPrefs->getDrawConditionalHighlights())
	{
		enableConditionalShow();
	}
}

void Preferences::enableConditionalShow(void)
{
	panel->conditionalShowAllButton->setEnabled(true);
	panel->conditionalShowControlPtsButton->setEnabled(true);
	setButtonState(panel->conditionalShowAllButton,
		ldPrefs->getShowAllConditionalLines());
	setButtonState(panel->conditionalShowControlPtsButton,
		ldPrefs->getShowConditionalControlPoints());
}


void Preferences::enablePrimitiveSubstitution(void)
{
	panel->textureStudsButton->setEnabled(true);
	panel->curveQualityLabel->setEnabled(true);
	panel->curveQualitySlider->setEnabled(true);
	setButtonState(panel->textureStudsButton, ldPrefs->getTextureStuds());
	if (ldPrefs->getTextureStuds())
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
	switch (ldPrefs->getTextureFilterType())
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

void Preferences::enableProxyServer(void)
{
	panel->proxyLabel->setEnabled(true);
	panel->proxyEdit->setEnabled(true);
	panel->portLabel->setEnabled(true);
	panel->portEdit->setEnabled(true);
	panel->proxyEdit->setText(ldPrefs->getProxyServer());
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

void Preferences::disableBFC(void)
{
	panel->bfcRedBackFaceButton->setEnabled(false);
        panel->bfcGreenFrontFaceButton->setEnabled(false);
	setButtonState(panel->bfcRedBackFaceButton, false);
        setButtonState(panel->bfcGreenFrontFaceButton, false);
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

void Preferences::disableProxyServer(void)
{
	panel->proxyLabel->setEnabled(false);
	panel->proxyEdit->setEnabled(false);
	panel->portLabel->setEnabled(false);
	panel->portEdit->setEnabled(false);
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
        TCFloat latitude;
        TCFloat longitude;
                                                                                                                                                             
        if (sscanf(value, "%f,%f", &latitude, &longitude) == 2)
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
                                                                                                                                                             
            modelViewer->setDefaultRotationMatrix(resultMatrix);
        }
        delete value;
    }
    else
    {
        value = TCUserDefaults::stringForKey(DEFAULT_MATRIX_KEY);
        if (value)
        {
            TCFloat matrix[16];
                                                                                                                                                             
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

int Preferences::getBackgroundColor(void)
{
	return (int)ldPrefs->getBackgroundColor();
}

bool Preferences::getShowErrors(void)
{
	return ldPrefs->getShowErrors();
}

bool Preferences::getDrawWireframe(void)
{
	return ldPrefs->getDrawWireframe();
}

bool Preferences::getShowsHighlightLines(void)
{
	return ldPrefs->getShowHighlightLines();
}

bool Preferences::getUseLighting(void)
{
	return ldPrefs->getUseLighting();
}

bool Preferences::getUseSeams(void)
{
	return ldPrefs->getUseSeams();
}

