// On the Mac, TREGL.h has to be included prior to anything else that might
// include GL/gl.h, or the wrong version of glext.h gets used, and things don't
// compile.  This is annoying, but it doesn't appear to hurt anything.
#include <TRE/TREGLExtensions.h>
#include <LDLoader/LDLModel.h>
#include <LDLoader/LDLPalette.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include "LDLib/LDUserDefaultsKeys.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCWebClient.h>
#include <QColor>
#include <QInputDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <TCFoundation/TCLocalStrings.h>
#include "ModelViewerWidget.h"
#include "Preferences.h"
#include <LDLib/LDPreferences.h>
#include <LDLib/LDrawModelViewer.h>

#define DEFAULT_PREF_SET TCLocalStrings::get("DefaultPrefSet")

Preferences::Preferences(QWidget *parent, ModelViewerWidget *modelWidget)
	:QDialog(parent),PreferencesPanel(),
	modelWidget(modelWidget),
	modelViewer(modelWidget->getModelViewer()),
	ldPrefs(new LDPreferences(modelViewer)),
	checkAbandon(true),
	proxyPortValidator(new QIntValidator(1,65535,this))
{
	setupUi(this);
    connect( applyButton, SIGNAL( pressed() ), this, SLOT( doApply() ) );
    connect( okButton, SIGNAL( clicked() ), this, SLOT( doOk() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( doCancel() ) );
    connect( primitvesResetButton, SIGNAL( clicked() ), this, SLOT( doResetPrimitives() ) );
    connect( seamWidthButton, SIGNAL( clicked() ), this, SLOT( enableApply() ) );
    connect( seamWidthSpin, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( fieldOfViewSpin, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( memoryUsageBox, SIGNAL( activated(int) ), this, SLOT( enableApply() ) );
    connect( snapshotSaveDirBox, SIGNAL( activated(int) ), this, SLOT( snapshotSaveDirBoxChanged() ) );
    connect( partsListsSaveDirBox, SIGNAL( activated(int) ), this, SLOT( partsListsSaveDirBoxChanged() ) );
    connect( exportsListsSaveDirBox, SIGNAL( activated(int) ), this, SLOT( exportsListsSaveDirBoxChanged() ) );
    connect( snapshotSaveDirButton, SIGNAL( clicked() ), this, SLOT( snapshotSaveDirBrowse() ) );
    connect( partsListsSaveDirButton, SIGNAL( clicked() ), this, SLOT( partsListsSaveDirBrowse() ) );
    connect( exportsSaveDirButton, SIGNAL( clicked() ), this, SLOT( exportsSaveDirBrowse() ) );
	connect( customConfigButton, SIGNAL( clicked() ), this, SLOT( customConfigBrowse() ) );
    connect( transparentButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( wireframeThicknessSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( edgesOnlyButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( conditionalLinesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( conditionalLinesButton, SIGNAL( toggled(bool) ), this, SLOT( doConditionalShow(bool) ) );
    connect( conditionalShowAllButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( conditionalShowControlPtsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( wireframeFogButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( wireframeRemoveHiddenLineButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( highQualityLinesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( alwaysBlackLinesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( edgeThicknessSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( showErrorsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( processLdconfigLdrButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( randomColorsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( frameRateButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( showAxesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( partBoundingBoxOnlyBox, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( aaLinesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( qualityLightingButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( subduedLightingButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( specularLightingButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( alternateLightingButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( crossEyedStereoButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( parallelStereoButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( stereoAmountSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( colorCutawayButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( monochromeCutawayButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( sortTransparencyButton, SIGNAL( toggled(bool) ), this, SLOT( doSortTransparency(bool) ) );
    connect( cutawayOpacitySlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( cutawayThicknessSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( stippleTransparencyButton, SIGNAL( toggled(bool) ), this, SLOT( doStippleTransparency(bool) ) );
    connect( flatShadingButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( smoothCurvesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( nearestFilteringButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( bilinearFilteringButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( trilinearFilteringButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( anisotropicFilteringButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( anisotropicFilteringButton, SIGNAL( toggled(bool) ), this, SLOT( doAnisotropic() ) );
    connect( anisotropicFilteringSlider, SIGNAL( valueChanged(int) ), this, SLOT( doAnisotropicSlider(int) ) );
    connect( anisotropicFilteringSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( curveQualitySlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );
    connect( lowQualityStudsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( hiresPrimitivesButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( generalResetButton, SIGNAL( clicked() ), this, SLOT( doResetGeneral() ) );
    connect( geometryResetButton, SIGNAL( clicked() ), this, SLOT( doResetGeometry() ) );
    connect( effectsResetButton, SIGNAL( clicked() ), this, SLOT( doResetEffects() ) );
    connect( updatesResetButton, SIGNAL( clicked() ), this, SLOT( doResetUpdates() ) );
	connect (updatesResetTimesButton, SIGNAL( clicked () ), this, SLOT (doResetTimesUpdates() ) );
    connect( wireframeButton, SIGNAL( toggled(bool) ), this, SLOT( doWireframe(bool) ) );
    connect( enableBFCButton, SIGNAL( toggled(bool) ), this, SLOT( doBFC(bool) ) );
    connect( enableBFCButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( bfcRedBackFaceButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( bfcGreenFrontFaceButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( bfcBlueNeutralFaceButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( wireframeButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( edgeLinesButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( edgeLinesButton, SIGNAL( toggled(bool) ), this, SLOT( doEdgeLines(bool) ) );
    connect( lightingButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingButton, SIGNAL( toggled(bool) ), this, SLOT( doLighting(bool) ) );
    connect( lightingDir11, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir12, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir13, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir21, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir22, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir23, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir31, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir32, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( lightingDir33, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( effectsUseLIGHTDATButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( effectsUseLIGHTDATButton, SIGNAL( toggled(bool) ), this, SLOT( doDrawLightDats() ) );
    connect( effectsReplaceStandarLightButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( effectsHideLIGHTButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( stereoButton, SIGNAL( toggled(bool) ), this, SLOT( doStereo(bool) ) );
    connect( stereoButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( wireframeCutawayButton, SIGNAL( toggled(bool) ), this, SLOT( doWireframeCutaway(bool) ) );
    connect( wireframeCutawayButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( primitiveSubstitutionButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
    connect( primitiveSubstitutionButton, SIGNAL( toggled(bool) ), this, SLOT( doPrimitiveSubstitution(bool) ) );
    connect( textureStudsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
    connect( textureStudsButton, SIGNAL( toggled(bool) ), this, SLOT( doTextureStuds(bool) ) );
	connect( useTextureMapsButton, SIGNAL( stateChanged(int) ), this, SLOT( enableApply() ) );
	connect( useTextureMapsButton, SIGNAL( toggled(bool) ), this, SLOT( doTextureStuds(bool) ) );
    connect( newPreferenceSetButton, SIGNAL( clicked() ), this, SLOT( doNewPreferenceSet() ) );
    connect( delPreferenceSetButton, SIGNAL( clicked() ), this, SLOT( doDelPreferenceSet() ) );
    connect( hotkeyPreferenceSetButton, SIGNAL( clicked() ), this, SLOT( doHotkeyPreferenceSet() ) );
    connect( preferenceSetList, SIGNAL( currentItemChanged(QListWidgetItem *,QListWidgetItem *) ), this, SLOT( doPrefSetSelected(QListWidgetItem *,QListWidgetItem *) ) );
    connect( backgroundColorButton, SIGNAL( clicked() ), this, SLOT( doBackgroundColor() ) );
    connect( defaultColorButton, SIGNAL( clicked() ), this, SLOT( doDefaultColor() ) );
    connect( updatesNoproxyButton, SIGNAL( toggled(bool) ), this, SLOT( disableProxy() ) );
    connect( updatesProxyButton, SIGNAL( toggled(bool) ), this, SLOT( enableProxy() ) );
    connect( updatesMissingpartsButton, SIGNAL( toggled(bool) ), this, SLOT( doUpdateMissingparts(bool) ) );
    connect( updatesMissingpartsButton, SIGNAL( toggled(bool) ), this, SLOT( enableApply() ) );
	connect( transparentOffsetSlider, SIGNAL( valueChanged(int) ), this, SLOT( enableApply() ) );

	portEdit->setValidator(proxyPortValidator);
	modelViewer = modelWidget->getModelViewer();
	resize(10, 10);
	loadSettings();
	ldPrefs->applySettings();
	reflectSettings();
#ifdef DEBUG
	setDebugLevel((int)TCUserDefaults::longForKey(DEBUG_LEVEL_KEY, 1, false));
#else // DEBUG
	setDebugLevel((int)TCUserDefaults::longForKey(DEBUG_LEVEL_KEY, 0, false));
#endif // DEBUG
	QStyle *style = defaultColorButton->style();
	if (style != NULL)
	{
		QString styleName = style->metaObject()->className();
		if (styleName == "QGtkStyle")
		{
			// QGtkStyle uses an image for the background, and doesn't show
			// the background color at all, so update the color buttons to use
			// the QWindowsStyle instead.
			// In Qt5 no need for changing the style, background color can be
			// set
#if QT_VERSION < 0x50000
			defaultColorButton->setStyle(&qlStyle);
			backgroundColorButton->setStyle(&qlStyle);
#else
			defaultColorButton->setStyle(QStyleFactory::create("Windows"));
			backgroundColorButton->setStyle(QStyleFactory::create("Windows"));
#endif
		}
	}
}

Preferences::~Preferences(void)
{
}

void Preferences::show(void)
{
	applyButton->setEnabled(false);
	QDialog::show();
	raise();
//	setActiveWindow();
}

void Preferences::doPrefSetsApply(void)
{
	TCStringArray *oldPrefSetNames = TCUserDefaults::getAllSessionNames();
	int i;
	int b;
	int count = oldPrefSetNames->getCount();
//	char *prefSetName = NULL;
	const char *sessionName = TCUserDefaults::getSessionName();
	bool changed = false;

	for (i = 0; i < count; i++)
	{
		char *oldPrefSetName = oldPrefSetNames->stringAtIndex(i);
		int index=-1;
		for(b = 0; b < preferenceSetList->count(); b++)
		{
			if (strcmp(oldPrefSetNames->stringAtIndex(i),
						preferenceSetList->item(b)->text().toUtf8().constData()) == 0)
			{
				index = b;
			}	
		}
		if (index == -1)
		{
			TCUserDefaults::removeSession(oldPrefSetName);
		}
	}
	count=preferenceSetList->count();
	for(i = 1; i < count; i++)
	{
		if (oldPrefSetNames->indexOfString(getPrefSet(i)) < 0)
		{
			TCUserDefaults::setSessionName(getPrefSet(i), PREFERENCE_SET_KEY);
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
		const char *selectedPrefSet = getSelectedPrefSet();
		if (!sessionName || !selectedPrefSet ||
			strcmp(sessionName, selectedPrefSet) != 0)
		{
			TCUserDefaults::setSessionName(selectedPrefSet, PREFERENCE_SET_KEY);
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
    	applyButton->setEnabled(false);
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
	int r, g, b;

	ldPrefs->setLineSmoothing(aaLinesButton->checkState());
	ldPrefs->setShowFps(frameRateButton->checkState());
	ldPrefs->setShowAxes(showAxesButton->checkState());
	if (modelWidget)
		modelWidget->setShowFPS(ldPrefs->getShowFps());
	ldPrefs->setShowErrors(showErrorsButton->checkState());
	ldPrefs->setProcessLdConfig(processLdconfigLdrButton->checkState());
	ldPrefs->setRandomColors(randomColorsButton->checkState());
	cTemp = backgroundColorButton->palette().color(QPalette::Button);
	cTemp.getRgb(&r, &g, &b);
	ldPrefs->setBackgroundColor(r, g, b);
	cTemp = defaultColorButton->palette().color(QPalette::Button);
	cTemp.getRgb(&r, &g, &b);
	ldPrefs->setDefaultColor(r, g, b);
	ldPrefs->setFov(fieldOfViewSpin->value());
	ldPrefs->setMemoryUsage(memoryUsageBox->currentIndex());
	ldPrefs->setTransDefaultColor(transparentButton->checkState());
	LDPreferences::DefaultDirMode snapshotDirMode, partsListDirMode, exportDirMode;
	ldPrefs->setSnapshotsDirMode(snapshotDirMode =
				(LDPreferences::DefaultDirMode)snapshotSaveDirBox->currentIndex());
	if (snapshotDirMode == LDPreferences::DDMSpecificDir)
	{
		snapshotDir = snapshotSaveDirEdit->text();
		if(snapshotDir.length()>0)
		{
			ldPrefs->setSnapshotsDir(snapshotDir.toUtf8().constData());
		}
		else
		{
			ldPrefs->setSnapshotsDirMode(LDPreferences::DDMLastDir);
		}
	}
	ldPrefs->setPartsListsDirMode(partsListDirMode =
			(LDPreferences::DefaultDirMode)partsListsSaveDirBox->currentIndex());
	if (partsListDirMode == LDPreferences::DDMSpecificDir)
	{
		partsListDir = partsListsSaveDirEdit->text();
		if (partsListDir.length() > 0)
		{
			ldPrefs->setPartsListsDir(partsListDir.toUtf8().constData());
		}
		else
		{
			ldPrefs->setPartsListsDirMode(LDPreferences::DDMLastDir);
		}
	}
	ldPrefs->setSaveDirMode(LDPreferences::SOExport, exportDirMode =
			(LDPreferences::DefaultDirMode)exportsListsSaveDirBox->currentIndex());
	if (exportDirMode == LDPreferences::DDMSpecificDir)
	{
		exportDir = exportsSaveDirEdit->text();
		if (exportDir.length() > 0)
		{
			ldPrefs->setSaveDir(LDPreferences::SOExport, exportDir.toUtf8().constData());
		}
		else
		{
			ldPrefs->setSaveDirMode(LDPreferences::SOExport,
					LDPreferences::DDMLastDir);
		}
	}
	ldPrefs->setCustomConfigPath(customConfigEdit->text().toUtf8().constData());
	ldPrefs->applyGeneralSettings();
	ldPrefs->commitGeneralSettings();
}

void Preferences::doGeometryApply(void)
{
	ldPrefs->setUseSeams(seamWidthButton->isChecked());
	ldPrefs->setBoundingBoxesOnly(partBoundingBoxOnlyBox->isChecked());
	ldPrefs->setSeamWidth(seamWidthSpin->value());
	ldPrefs->setDrawWireframe(wireframeButton->isChecked());
	if (ldPrefs->getDrawWireframe())
	{
		ldPrefs->setUseWireframeFog(wireframeFogButton->isChecked());
		ldPrefs->setRemoveHiddenLines(
			wireframeRemoveHiddenLineButton->isChecked());
	}
	ldPrefs->setBfc(enableBFCButton->isChecked());
	if (ldPrefs->getBfc())
	{
		ldPrefs->setRedBackFaces(bfcRedBackFaceButton->isChecked());
		ldPrefs->setGreenFrontFaces(bfcGreenFrontFaceButton->isChecked());
		ldPrefs->setBlueNeutralFaces(bfcBlueNeutralFaceButton->isChecked());
	}
	ldPrefs->setWireframeThickness(wireframeThicknessSlider->value());
	ldPrefs->setShowHighlightLines(edgeLinesButton->isChecked());
	if (ldPrefs->getShowHighlightLines())
	{
		ldPrefs->setEdgesOnly(edgesOnlyButton->isChecked());
		ldPrefs->setDrawConditionalHighlights(
			conditionalLinesButton->isChecked());
		if (ldPrefs->getDrawConditionalHighlights())
		{
			ldPrefs->setShowAllConditionalLines(
				conditionalShowAllButton->isChecked());
			ldPrefs->setShowConditionalControlPoints(
				conditionalShowControlPtsButton->isChecked());
		}
		ldPrefs->setUsePolygonOffset(highQualityLinesButton->isChecked());
		ldPrefs->setBlackHighlights(alwaysBlackLinesButton->isChecked());
		ldPrefs->setEdgeThickness(edgeThicknessSlider->value());
	}
	ldPrefs->applyGeometrySettings();
	ldPrefs->commitGeometrySettings();
}

LDPreferences::LightDirection Preferences::getSelectedLightDirection(void)
{
	LDPreferences::LightDirection lightDirection =
		LDPreferences::CustomDirection;
    if(lightingDir11->isChecked())
    {
		lightDirection = LDPreferences::UpperLeft;
    }
    else if (lightingDir12->isChecked())
    {
    lightDirection = LDPreferences::UpperMiddle;
    }
    else if (lightingDir13->isChecked())
    {
		lightDirection = LDPreferences::UpperRight;
    }
    else if (lightingDir21->isChecked())
    {
		lightDirection = LDPreferences::MiddleLeft;
    }
    else if (lightingDir22->isChecked())
    {
		lightDirection = LDPreferences::MiddleMiddle;
    }
    else if (lightingDir23->isChecked())
    {
		lightDirection = LDPreferences::MiddleRight;
    }
    else if (lightingDir31->isChecked())
    {
		lightDirection = LDPreferences::LowerLeft;
    }
    else if (lightingDir32->isChecked())
    {
		lightDirection = LDPreferences::LowerMiddle;
    }
    else if (lightingDir33->isChecked())
    {
		lightDirection = LDPreferences::LowerRight;
    }
	return lightDirection;
}

void Preferences::doEffectsApply(void)
{
	LDVStereoMode smTemp = LDVStereoNone;
	LDVCutawayMode cmTemp = LDVCutawayNormal;

	ldPrefs->setUseLighting(lightingButton->isChecked());
	if (ldPrefs->getUseLighting())
	{
		LDPreferences::LightDirection lightDirection =
			getSelectedLightDirection();
		ldPrefs->setQualityLighting(qualityLightingButton->isChecked());
		ldPrefs->setSubduedLighting(subduedLightingButton->isChecked());
		ldPrefs->setUseSpecular(specularLightingButton->isChecked());
		ldPrefs->setOneLight(alternateLightingButton->isChecked());
		if (lightDirection != LDPreferences::CustomDirection)
		{
			ldPrefs->setLightDirection(lightDirection);
		}
		if (effectsUseLIGHTDATButton->isChecked())
		{
			ldPrefs->setDrawLightDats(true);
			ldPrefs->setOptionalStandardLight(
				effectsReplaceStandarLightButton->isChecked());
		}
		else
		{
			ldPrefs->setDrawLightDats(false);
		}
	}
	ldPrefs->setNoLightGeom(effectsHideLIGHTButton->isChecked());
	if (!stereoButton->isChecked())
	{
		smTemp = LDVStereoNone;
	}
	else if (crossEyedStereoButton->isChecked())
	{
		smTemp = LDVStereoCrossEyed;
	}
	else if (parallelStereoButton->isChecked())
	{
		smTemp = LDVStereoParallel;
	}
	ldPrefs->setStereoMode(smTemp);
	ldPrefs->setStereoEyeSpacing(stereoAmountSlider->value());
	if (!wireframeCutawayButton->isChecked())
	{
		cmTemp = LDVCutawayNormal;
	}
	else if (colorCutawayButton->isChecked())
	{
		cmTemp = LDVCutawayWireframe;
	}
	else if (monochromeCutawayButton->isChecked())
	{
		cmTemp = LDVCutawayStencil;
	}
	ldPrefs->setCutawayMode(cmTemp);
	ldPrefs->setCutawayAlpha(cutawayOpacitySlider->value());
	ldPrefs->setCutawayThickness(cutawayThicknessSlider->value());
	ldPrefs->setSortTransparent(sortTransparencyButton->isChecked());
	ldPrefs->setUseStipple(stippleTransparencyButton->isChecked());
	ldPrefs->setUseFlatShading(flatShadingButton->isChecked());
	ldPrefs->setPerformSmoothing(smoothCurvesButton->isChecked());
	ldPrefs->applyEffectsSettings();
	ldPrefs->commitEffectsSettings();
}

void Preferences::setAniso(int value)
{
    QString s;
	int intLevel = 1 << value;
    s = s.setNum(intLevel);
    s+="x";
	if (value < 1 ) s = "";
    anisotropicLabel->setText(s);
	anisotropicFilteringSlider->setValue(value);
	ldPrefs->setAnisoLevel(intLevel);
}

void Preferences::doPrimitivesApply(void)
{
	int aniso = 0;
	bool aps,texmaps;
	ldPrefs->setTexmaps(texmaps = useTextureMapsButton->isChecked());
	ldPrefs->setAllowPrimitiveSubstitution(aps =
		primitiveSubstitutionButton->isChecked());
	if (ldPrefs->getAllowPrimitiveSubstitution())
	{
		ldPrefs->setTextureStuds(textureStudsButton->isChecked());
		ldPrefs->setCurveQuality(curveQualitySlider->value());
	}
	if (texmaps)
	{
		ldPrefs->setTextureOffsetFactor(transparentOffsetSlider->value()/10.0f);
	}
	if ((aps && ldPrefs->getTextureStuds()) || texmaps)
	{
		int iTemp = GL_NEAREST_MIPMAP_NEAREST;

		if (nearestFilteringButton->isChecked())
		{
			iTemp = GL_NEAREST_MIPMAP_NEAREST;
		}
		else if (bilinearFilteringButton->isChecked())
		{
			iTemp = GL_LINEAR_MIPMAP_NEAREST;
		}
		else if (trilinearFilteringButton->isChecked())
		{
			iTemp = GL_LINEAR_MIPMAP_LINEAR;
		}
		else if (anisotropicFilteringButton->isChecked())
		{
			iTemp = GL_LINEAR_MIPMAP_LINEAR;
			aniso = anisotropicFilteringSlider->value();
		}
		ldPrefs->setTextureFilterType(iTemp);
		setAniso(aniso);
	}
	ldPrefs->setQualityStuds(!lowQualityStudsButton->isChecked());
	ldPrefs->setHiResPrimitives(hiresPrimitivesButton->isChecked());
	ldPrefs->applyPrimitivesSettings();
	ldPrefs->commitPrimitivesSettings();
}

void Preferences::doUpdatesApply()
{
	int  iTemp;
	bool ok;

	ldPrefs->setCheckPartTracker(updatesMissingpartsButton->isChecked());
	if (updatesProxyButton->isChecked())
	{
		ldPrefs->setProxyType(2);
	}
	else
	{
		ldPrefs->setProxyType(0);
	}
	iTemp = portEdit->text().toInt(&ok);
	if (ok)
	{
		ldPrefs->setProxyPort(iTemp);
	}
	iTemp = daymissingpartcheckText->text().toInt(&ok);
	if (ok)
    {
		ldPrefs->setMissingPartWait(iTemp);
	}
	iTemp = dayupdatedpartcheckText->text().toInt(&ok);
	if (ok)
	{
		ldPrefs->setUpdatedPartWait(iTemp);
	}
	ldPrefs->setProxyServer(proxyEdit->text().toLatin1().constData());
	ldPrefs->applyUpdatesSettings();
	ldPrefs->commitUpdatesSettings();
}

void Preferences::doBackgroundColor()
{
	int r,g,b;

	ldPrefs->getBackgroundColor(r, g, b);
	QColor color = QColorDialog::getColor(QColor(r,g,b));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		backgroundColorButton->setPalette(palette);
		applyButton->setEnabled(true);
	}
}

void Preferences::doDefaultColor()
{
	int r, g, b, a, i;
	QRgb old[16];

    for (i = 0 ; i < 16; i++)
    {
#if QT_VERSION < 0x50000
		old[i] = QColorDialog::customColor(i);
#else
		old[i] = QColorDialog::customColor(i).rgb();
#endif
		LDLPalette::getDefaultRGBA(i, r, g, b, a);
		QColorDialog::setCustomColor(i, qRgb(r, g, b));
    }
	ldPrefs->getDefaultColor(r, g, b);
	QColor color = QColorDialog::getColor(QColor(r,g,b));
	if(color.isValid())
	{
		QPalette palette;
		palette.setColor(QPalette::Button, color);
		defaultColorButton->setPalette(palette);
		applyButton->setEnabled(true);
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
	applyButton->setEnabled(false);
	if (modelWidget)
	{
		modelWidget->reflectSettings();
		modelWidget->doApply();
		setupDefaultRotationMatrix();
	}
	checkAbandon = true;
}

void Preferences::doOk(void)
{
	doApply();
	QDialog::close();
}

void Preferences::doCancel(void)
{
	loadSettings();
	reflectSettings();
	QDialog::close();
}

void Preferences::doAnisotropic(void)
{
	bool x = anisotropicFilteringButton->isChecked();
	anisotropicFilteringSlider->setEnabled(x);
	anisotropicLabel->setEnabled(x);
	doAnisotropicSlider(anisotropicFilteringSlider->value());
}

void Preferences::doAnisotropicSlider(int i)
{
	QString s;
	s = s.setNum(1 << i);
	s+="x";
	anisotropicLabel->setText(s);
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
}

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

void Preferences::setUseWireframeFog(bool value)
{
    if (value != ldPrefs->getUseWireframeFog())
	{
		ldPrefs->setUseWireframeFog(value, true, true);
		reflectWireframeSettings();
	}
}

void Preferences::setRemoveHiddenLines(bool value)
{
	if (value != ldPrefs->getRemoveHiddenLines())
	{
		ldPrefs->setRemoveHiddenLines(value, true, true);
		reflectWireframeSettings();
	}
}

void Preferences::setTextureStud(bool value)
{
    if (value != ldPrefs->getTextureStuds())
	{
		ldPrefs->setTextureStuds(value, true, true);
		reflectPrimitivesSettings();
	}
}

void Preferences::setEdgeOnly(bool value)
{
	if (value != ldPrefs->getEdgesOnly())
	{
		ldPrefs->setEdgesOnly(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setConditionalLine(bool value)
{
	if (value != ldPrefs->getDrawConditionalHighlights())
	{
		ldPrefs->setDrawConditionalHighlights(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setHighQuality(bool value)
{
	if (value != ldPrefs->getUsePolygonOffset())
	{
		ldPrefs->setUsePolygonOffset(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setAlwaysBlack(bool value)
{
	if (value != ldPrefs->getBlackHighlights())
	{
		ldPrefs->setBlackHighlights(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setRedBackFaces(bool value)
{
	if (value != ldPrefs->getRedBackFaces())
	{
		ldPrefs->setRedBackFaces(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setGreenFrontFaces(bool value)
{
	if (value != ldPrefs->getGreenFrontFaces())
	{
		ldPrefs->setGreenFrontFaces(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setBlueNeutralFaces(bool value)
{
	if (value != ldPrefs->getBlueNeutralFaces())
	{
		ldPrefs->setBlueNeutralFaces(value, true, true);
		reflectGeometrySettings();
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

void Preferences::setUseBFC(bool value)
{
	if (value != ldPrefs->getBfc())
	{
		ldPrefs->setBfc(value, true, true);
		reflectGeometrySettings();
	}
}

void Preferences::setShowAxes(bool value)
{
    if (value != ldPrefs->getShowAxes())
    {
		ldPrefs->setShowAxes(value, true, true);
		reflectGeneralSettings();
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

	setButtonState(aaLinesButton, ldPrefs->getLineSmoothing());
	setButtonState(frameRateButton, ldPrefs->getShowFps());
	setButtonState(showAxesButton, ldPrefs->getShowAxes());
	setButtonState(showErrorsButton, ldPrefs->getShowErrors());
	setButtonState(processLdconfigLdrButton,
		ldPrefs->getProcessLdConfig());
	setButtonState(randomColorsButton,ldPrefs->getRandomColors());
	ldPrefs->getBackgroundColor(r, g, b);
	QPalette palette0;
	palette0.setColor(QPalette::Button, QColor( r, g, b ));
	backgroundColorButton->setPalette(palette0);
	ldPrefs->getDefaultColor(r, g, b);
	QPalette palette;
	palette.setColor(QPalette::Button, QColor( r, g, b ));
	defaultColorButton->setPalette(palette);
	setRangeValue(fieldOfViewSpin, (int)ldPrefs->getFov());
	setButtonState(transparentButton, ldPrefs->getTransDefaultColor());
	memoryUsageBox->setCurrentIndex(ldPrefs->getMemoryUsage());
	setupSaveDirs();
	customConfigEdit->setText(ldPrefs->getCustomConfigPath().c_str());
}

void Preferences::reflectGeometrySettings(void)
{
	setButtonState(seamWidthButton, ldPrefs->getUseSeams());
	setRangeValue(seamWidthSpin, ldPrefs->getSeamWidth());
	partBoundingBoxOnlyBox->setChecked(ldPrefs->getBoundingBoxesOnly());
//	seamWidthSpin->setValue(seamWidth);
	reflectWireframeSettings();
	reflectBFCSettings();
//	wireframeThicknessSlider->setValue(wireframeThickness);
	edgeLinesButton->setChecked(ldPrefs->getShowHighlightLines());
	if (ldPrefs->getShowHighlightLines())
	{
		enableEdgeLines();
	}
	else
	{
		disableEdgeLines();
	}
	setRangeValue(edgeThicknessSlider, ldPrefs->getEdgeThickness());
//	edgeThicknessSlider->setValue(edgeThickness);
}

void Preferences::reflectWireframeSettings(void)
{
	wireframeButton->setChecked(ldPrefs->getDrawWireframe());
	setRangeValue(wireframeThicknessSlider,
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
	enableBFCButton->setChecked(ldPrefs->getBfc());
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
	lightingButton->setChecked(ldPrefs->getUseLighting());
	if (ldPrefs->getUseLighting())
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
	setButtonState(effectsHideLIGHTButton, ldPrefs->getNoLightGeom());
	if (ldPrefs->getStereoMode() != LDVStereoNone)
	{
		stereoButton->setChecked(true);
		enableStereo();
	}
	else
	{
		stereoButton->setChecked(false);
		disableStereo();
	}
	stereoAmountSlider->setValue(ldPrefs->getStereoEyeSpacing());
	if (ldPrefs->getCutawayMode() != LDVCutawayNormal)
	{
		wireframeCutawayButton->setChecked(true);
		enableWireframeCutaway();
	}
	else
	{
		wireframeCutawayButton->setChecked(false);
		disableWireframeCutaway();
	}
	cutawayOpacitySlider->setValue(ldPrefs->getCutawayAlpha());
	cutawayThicknessSlider->setValue(ldPrefs->getCutawayThickness());
	setButtonState(sortTransparencyButton,
		ldPrefs->getSortTransparent());
	setButtonState(stippleTransparencyButton, ldPrefs->getUseStipple());
	setButtonState(flatShadingButton, ldPrefs->getUseFlatShading());
	setButtonState(smoothCurvesButton, ldPrefs->getPerformSmoothing());
}

void Preferences::reflectPrimitivesSettings(void)
{
	primitiveSubstitutionButton->setChecked(
		ldPrefs->getAllowPrimitiveSubstitution());
	if (ldPrefs->getAllowPrimitiveSubstitution())
	{
		enablePrimitiveSubstitution();
	}
	else
	{
		disablePrimitiveSubstitution();
	}
	curveQualitySlider->setValue(ldPrefs->getCurveQuality());
	setButtonState(lowQualityStudsButton, !ldPrefs->getQualityStuds());
	setButtonState(hiresPrimitivesButton, ldPrefs->getHiResPrimitives());
	useTextureMapsButton->setChecked(ldPrefs->getTexmaps());
	if (ldPrefs->getTexmaps())
	{
		transparentOffsetSlider->setValue(ldPrefs->getTextureOffsetFactor()*10);
	}
}

void Preferences::reflectUpdatesSettings(void)
{
	if (ldPrefs->getProxyType())
	{
		enableProxyServer();
		updatesProxyButton->setChecked(true);
	}
	else
	{
		disableProxyServer();
		updatesNoproxyButton->setChecked(true);
	}
	setButtonState(updatesMissingpartsButton,
		ldPrefs->getCheckPartTracker());
	proxyEdit->setText(ldPrefs->getProxyServer());
	portEdit->setText(QString::number(ldPrefs->getProxyPort()));
	daymissingpartcheckText->setValue(ldPrefs->getMissingPartWait());
	dayupdatedpartcheckText->setValue(ldPrefs->getUpdatedPartWait());
	doUpdateMissingparts(ldPrefs->getCheckPartTracker());
}

void Preferences::doResetGeneral(void)
{
	ldPrefs->loadDefaultGeneralSettings(false);
	reflectGeneralSettings();
}

void Preferences::doResetGeometry(void)
{
	ldPrefs->loadDefaultGeometrySettings(false);
	reflectGeometrySettings();
}

void Preferences::doResetEffects(void)
{
	ldPrefs->loadDefaultEffectsSettings(false);
	reflectEffectsSettings();
}

void Preferences::doResetPrimitives(void)
{
	ldPrefs->loadDefaultPrimitivesSettings(false);
	reflectPrimitivesSettings();
}

void Preferences::doResetUpdates(void)
{
	ldPrefs->loadDefaultUpdatesSettings(false);
	reflectUpdatesSettings();
}

void Preferences::doResetTimesUpdates(void)
{
	LDrawModelViewer::resetUnofficialDownloadTimes();
}

void Preferences::getRGB(int color, int &r, int &g, int &b)
{
	r = color & 0xFF;
	g = (color >> 8) & 0xFF;
	b = (color >> 16) & 0xFF;
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
	const char *constPathKey;

	if (pathKey)
	{
		constPathKey = pathKey;
	}
	else
	{
		constPathKey = LAST_OPEN_PATH_KEY;
	}
	path = TCUserDefaults::stringForKey(constPathKey, NULL, false);
	if (!path)
	{
		path = copyString("/dos/c/ldraw");
	}
	return path;
}

void Preferences::setLastOpenPath(const char *path, char *pathKey)
{
	const char *constPathKey;

	if (pathKey)
	{
		constPathKey = pathKey;
	}
	else
	{
		constPathKey = LAST_OPEN_PATH_KEY;
	}
	TCUserDefaults::setStringForKey(path, constPathKey, false);
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

	key = QString("%1/File%2").arg(RECENT_FILES_KEY).arg(index,2,10,QLatin1Char('0'));
	return key;
}

char *Preferences::getRecentFile(int index)
{
	return TCUserDefaults::stringForKey(getRecentFileKey(index).toLatin1().constData(), NULL, false);
}

void Preferences::setRecentFile(int index, char *filename)
{
	if (filename)
	{
		TCUserDefaults::setStringForKey(filename, getRecentFileKey(index).toLatin1().constData(), false);
	}
	else
	{
		TCUserDefaults::removeValue(getRecentFileKey(index).toLatin1().constData(), false);
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

LDInputHandler::ViewMode Preferences::getViewMode(void)
{
	return (LDInputHandler::ViewMode)TCUserDefaults::longForKey(VIEW_MODE_KEY, 0 , false);
}

void Preferences::setViewMode(LDInputHandler::ViewMode value)
{
	TCUserDefaults::setLongForKey(value, VIEW_MODE_KEY, false);
}

bool Preferences::getLatLongMode(void)
{
	return TCUserDefaults::longForKey(EXAMINE_MODE_KEY, LDrawModelViewer::EMFree ,false);
}

void Preferences::setLatLongMode(bool value)
{
	TCUserDefaults::setLongForKey(value, EXAMINE_MODE_KEY, false);
}

bool Preferences::getKeepRightSideUp(void)
{
	return TCUserDefaults::longForKey(KEEP_RIGHT_SIDE_UP_KEY, false, false);
}

void Preferences::setKeepRightSideUp(bool value)
{
	TCUserDefaults::setLongForKey(value, KEEP_RIGHT_SIDE_UP_KEY, false);
}

bool Preferences::getPovAspectRatio(void)
{
	return TCUserDefaults::boolForKey(POV_CAMERA_ASPECT_KEY, false, false);
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

void Preferences::doUpdateMissingparts(bool value)
{
    daymissingpartcheckText->setEnabled(value);
	daymissingpartcheckLabel->setEnabled(value);
	dayupdatedpartcheckLabel->setEnabled(value);
	dayupdatedpartcheckText->setEnabled(value);
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

void Preferences::doDrawLightDats()
{
	bool checked = effectsUseLIGHTDATButton->isChecked();
	effectsReplaceStandarLightButton->setEnabled(checked);
	bool enabled = ldPrefs->getOptionalStandardLight() ? true : false;
	if (!checked) enabled = false;
	setButtonState(effectsReplaceStandarLightButton , enabled);
}

void Preferences::doSaveDefaultViewAngle()
{
	ldPrefs->saveDefaultView();
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
		setButtonState(stippleTransparencyButton,false);
	}
	applyButton->setEnabled(true);
}

void Preferences::doStippleTransparency(bool value)
{
	if(value)
	{
		setButtonState(sortTransparencyButton,false);
	}
	applyButton->setEnabled(true);
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
	if (value || useTextureMapsButton->isChecked())
	{
		enableTextureStuds();
	}
	else
	{
		disableTextureStuds();
	}
	updateTexmapsEnabled();
}

void Preferences::doNewPreferenceSet()
{
    bool ok;
    QString name = QInputDialog::getText(this,QString("LDView New Preference Set"),
				   QString("Enter name of the new PreferenceSet"), QLineEdit::Normal,QString(),
					&ok);
    if (ok && !name.isEmpty())
	{
		for(int i = 0; i < preferenceSetList->count(); i++)
		{
			if (getPrefSet(i) && strcmp(getPrefSet(i), name.toUtf8().constData())==0)
			{
				QMessageBox::warning(this,
					QString::fromWCharArray(TCLocalStrings::get(L"PrefSetAlreadyExists")),
					QString::fromWCharArray(TCLocalStrings::get(L"DuplicateName")),
					QMessageBox::Ok);
				return;
			}
		}
		if (name.indexOf('/')!=-1)
		{
			QMessageBox::warning(this,
				QString::fromWCharArray(TCLocalStrings::get(L"PrefSetNameBadChars")),
				QString::fromWCharArray(TCLocalStrings::get(L"InvalidName")),
				QMessageBox::Ok);
				return;
		}
		new QListWidgetItem(name,preferenceSetList);
		selectPrefSet(name.toUtf8().constData());
		return;
	}
	if (name.isEmpty() && ok)
	{
		QMessageBox::warning(this,
			QString::fromWCharArray(TCLocalStrings::get(L"PrefSetNameRequired")),
			QString::fromWCharArray(TCLocalStrings::get(L"EmptyName")),
			QMessageBox::Ok);
		return;
	}
}

void Preferences::doDelPreferenceSet()
{
	const char *selectedPrefSet = getSelectedPrefSet();
	if (selectedPrefSet)
	{
		int selectedIndex = preferenceSetList->currentRow();
		if (checkAbandon && applyButton->isEnabled())
		{
			if(QMessageBox::warning(this,
				QString::fromWCharArray(TCLocalStrings::get(L"AbandonChanges")),
				QString::fromWCharArray(TCLocalStrings::get(L"PrefSetAbandonConfirm")),
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
		delete preferenceSetList->currentItem();
		selectedIndex = preferenceSetList->currentRow();
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
	QString res = QInputDialog::getItem(this,getSelectedPrefSet(),
			"Select a hot key to automatically select this Preference Set:\nAlt + ",
			lst, hotKeyIndex, false, &ok);
	if (ok)
	{
		hotKeyIndex = lst.indexOf(res);
		if(hotKeyIndex != -1)
		{
			saveCurrentHotKey();
		}
	}
	
}

char *Preferences::getHotKey(int index)
{
    char key[128];

    snprintf(key, sizeof(key), "%s/Key%d", HOT_KEYS_KEY, index);
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

		snprintf(key, sizeof(key), "%s/Key%d", HOT_KEYS_KEY, currentHotKey);
		TCUserDefaults::removeValue(key, false);
    }
    if (hotKeyIndex > 0)
    {
		char key[128];

		snprintf(key, sizeof(key), "%s/Key%d", HOT_KEYS_KEY, hotKeyIndex % 10);
		TCUserDefaults::setStringForKey(getSelectedPrefSet(), key, false);
    }
}

void Preferences::abandonChanges(void)
{
	applyButton->setEnabled(false);
	loadSettings();
	reflectSettings();
}

const char *Preferences::getPrefSet(int index)
{
	return copyString(preferenceSetList->item(index)->text().toUtf8().constData());
}

const char *Preferences::getSelectedPrefSet(void)
{
    int selectedIndex = preferenceSetList->currentRow();
	if (selectedIndex!=-1)
	{
		return copyString(preferenceSetList->currentItem()->text().toUtf8().constData());
	}
	return NULL;
}
bool Preferences::doPrefSetSelected(bool force)
{
    const char *selectedPrefSet = getSelectedPrefSet();
    bool needToReselect = false;

	if (checkAbandon && applyButton->isEnabled() && !force)
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
			QMessageBox::warning(this,QString::fromWCharArray(TCLocalStrings::get(L"Error")),
				"You have made changes to the current preference set.  You must either apply those changes or abandon them before you can select a new preference set.");
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
		delPreferenceSetButton->setEnabled(enabled);
		delete selectedPrefSet;
    }
	applyButton->setEnabled(true);
	checkAbandon = false;
    return false;
}
	
void Preferences::selectPrefSet(const char *prefSet, bool force)
{
    if (prefSet)
    {
		for (int i=0;i<preferenceSetList->count();i++)
		{
			if (strcmp(prefSet,preferenceSetList->item(i)->text().toUtf8().constData())==0)
			{
				preferenceSetList->setCurrentRow(i);
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
	preferenceSetList->clear();
    new QListWidgetItem(QString(DEFAULT_PREF_SET),preferenceSetList);
    for (i = 0; i < count; i++)
    {
		new QListWidgetItem(sessionNames->stringAtIndex(i),preferenceSetList);
    }
    selectPrefSet();
	sessionNames->release();
}

void Preferences::enableWireframeCutaway(void)
{
	colorCutawayButton->setEnabled(true);
	monochromeCutawayButton->setEnabled(true);
	cutawayOpacitySlider->setEnabled(true);
	cutawayThicknessSlider->setEnabled(true);
	cutawayOpacityLabel->setEnabled(true);
	cutawayThicknessLabel->setEnabled(true);
	setButtonState(colorCutawayButton, false);
	setButtonState(monochromeCutawayButton, false);
	switch (ldPrefs->getCutawayMode())
	{
	case LDVCutawayNormal:
	case LDVCutawayWireframe:
		colorCutawayButton->toggle();
		break;
	case LDVCutawayStencil:
		monochromeCutawayButton->toggle();
		break;
	}
}

void Preferences::selectLightDirection(LDPreferences::LightDirection ld)
{
    switch (ld)
    {
    case LDPreferences::UpperLeft:
		lightingDir11->setChecked(true);
		break;
    case LDPreferences::UpperMiddle:
		lightingDir12->setChecked(true);
		break;
    case LDPreferences::UpperRight:
		lightingDir13->setChecked(true);
		break;
    case LDPreferences::MiddleLeft:
		lightingDir21->setChecked(true);
		break;
    case LDPreferences::MiddleMiddle:
		lightingDir22->setChecked(true);
		break;
    case LDPreferences::MiddleRight:
		lightingDir23->setChecked(true);
		break;
    case LDPreferences::LowerLeft:
		lightingDir31->setChecked(true);
		break;
    case LDPreferences::LowerMiddle:
		lightingDir32->setChecked(true);
		break;
    case LDPreferences::LowerRight:
		lightingDir33->setChecked(true);
		break;
    case LDPreferences::CustomDirection:
		break;
    }
}

void Preferences::enableLighting(void)
{
	qualityLightingButton->setEnabled(true);
	subduedLightingButton->setEnabled(true);
	specularLightingButton->setEnabled(true);
	alternateLightingButton->setEnabled(true);
	effectsUseLIGHTDATButton->setEnabled(true);
	lightingDir11->setEnabled(true);
	lightingDir12->setEnabled(true);
	lightingDir13->setEnabled(true);
	lightingDir21->setEnabled(true);
	lightingDir22->setEnabled(true);
	lightingDir23->setEnabled(true);
	lightingDir31->setEnabled(true);
	lightingDir32->setEnabled(true);
	lightingDir33->setEnabled(true);
	setButtonState(qualityLightingButton, ldPrefs->getQualityLighting());
	setButtonState(subduedLightingButton, ldPrefs->getSubduedLighting());
	setButtonState(specularLightingButton, ldPrefs->getUseSpecular());
	setButtonState(alternateLightingButton, ldPrefs->getOneLight());
	setButtonState(effectsUseLIGHTDATButton, ldPrefs->getDrawLightDats());
	selectLightDirection(ldPrefs->getLightDirection());
	doDrawLightDats();
}

void Preferences::enableStereo(void)
{
	crossEyedStereoButton->setEnabled(true);
	parallelStereoButton->setEnabled(true);
	stereoAmountSlider->setEnabled(true);
	stereoAmountLabel->setEnabled(true);
	setButtonState(crossEyedStereoButton, false);
	setButtonState(parallelStereoButton, false);
	switch (ldPrefs->getStereoMode())
	{
	case LDVStereoNone:
	case LDVStereoCrossEyed:
		crossEyedStereoButton->toggle();
		break;
	case LDVStereoParallel:
		parallelStereoButton->toggle();
		break;
	default:
		break;
	}
}

void Preferences::enableWireframe(void)
{
	wireframeFogButton->setEnabled(true);
	wireframeRemoveHiddenLineButton->setEnabled(true);
	wireframeThicknessSlider->setEnabled(true);
	wireframeThicknessLabel->setEnabled(true);
	setButtonState(wireframeFogButton, ldPrefs->getUseWireframeFog());
    setButtonState(wireframeRemoveHiddenLineButton,
		ldPrefs->getRemoveHiddenLines());
}

void Preferences::enableBFC(void)
{
	bfcRedBackFaceButton->setEnabled(true);
		bfcGreenFrontFaceButton->setEnabled(true);
		bfcBlueNeutralFaceButton->setEnabled(true);
	setButtonState(bfcRedBackFaceButton, ldPrefs->getRedBackFaces());
	setButtonState(bfcGreenFrontFaceButton,
		ldPrefs->getGreenFrontFaces());
	setButtonState(bfcBlueNeutralFaceButton,
		ldPrefs->getBlueNeutralFaces());
}

void Preferences::enableEdgeLines(void)
{
	conditionalLinesButton->setEnabled(true);
	edgesOnlyButton->setEnabled(true);
	highQualityLinesButton->setEnabled(true);
	alwaysBlackLinesButton->setEnabled(true);
	edgeThicknessLabel->setEnabled(true);
	edgeThicknessSlider->setEnabled(true);
	setButtonState(conditionalLinesButton,
		ldPrefs->getDrawConditionalHighlights());
	setButtonState(edgesOnlyButton, ldPrefs->getEdgesOnly());
	setButtonState(highQualityLinesButton,
		ldPrefs->getUsePolygonOffset());
	setButtonState(alwaysBlackLinesButton,
		ldPrefs->getBlackHighlights());
	if (ldPrefs->getDrawConditionalHighlights())
	{
		enableConditionalShow();
	}
	else
	{
		disableConditionalShow();
	}
}

void Preferences::enableConditionalShow(void)
{
	conditionalShowAllButton->setEnabled(true);
	conditionalShowControlPtsButton->setEnabled(true);
	setButtonState(conditionalShowAllButton,
		ldPrefs->getShowAllConditionalLines());
	setButtonState(conditionalShowControlPtsButton,
		ldPrefs->getShowConditionalControlPoints());
}


void Preferences::enablePrimitiveSubstitution(void)
{
	textureStudsButton->setEnabled(true);
	curveQualityLabel->setEnabled(true);
	curveQualitySlider->setEnabled(true);
	setButtonState(textureStudsButton, ldPrefs->getTextureStuds());
	if (ldPrefs->getTextureStuds() || ldPrefs->getTexmaps())
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
	GLfloat maxAniso = TREGLExtensions::getMaxAnisoLevel();
	TCFloat32 anisoLevel = ldPrefs->getAnisoLevel();
	short numAnisoLevels = (short)(log(maxAniso)/log(2.0) + 0.5f);
	if (numAnisoLevels)
		anisotropicFilteringSlider->setMaximum(numAnisoLevels);
	nearestFilteringButton->setEnabled(true);
	bilinearFilteringButton->setEnabled(true);
	trilinearFilteringButton->setEnabled(true);
	anisotropicFilteringButton->setEnabled(
		TREGLExtensions::haveAnisoExtension());
	setButtonState(nearestFilteringButton, false);
	setButtonState(bilinearFilteringButton, false);
	setButtonState(trilinearFilteringButton, false);
	setButtonState(anisotropicFilteringButton, false);
	switch (ldPrefs->getTextureFilterType())
	{
	case GL_NEAREST_MIPMAP_NEAREST:
		nearestFilteringButton->toggle();
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		bilinearFilteringButton->toggle();
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		trilinearFilteringButton->toggle();
		break;
	}
	if (anisoLevel > 1.0)
	{
	 	anisotropicFilteringButton->toggle();
	 	setAniso((int)(log(anisoLevel)/log(2.0)+0.5f));
	}
	if (anisoLevel > maxAniso)
	{
		anisoLevel = (TCFloat32)maxAniso;
	}
	if (anisoLevel < 2.0f)
	{
		anisoLevel = 2.0f;
	}
	doAnisotropic();
}

void Preferences::enableTexmaps(void)
{
	transparentTextureOffsetLabel->setEnabled(true);
	transparentOffsetSlider->setEnabled(true);
	transparentOffsetSlider->setValue(ldPrefs->getTexturesAfterTransparent()*10.0);

}


void Preferences::enableProxyServer(void)
{
	proxyLabel->setEnabled(true);
	proxyEdit->setEnabled(true);
	portLabel->setEnabled(true);
	portEdit->setEnabled(true);
	proxyEdit->setText(ldPrefs->getProxyServer());
}

void Preferences::disableWireframeCutaway(void)
{
	colorCutawayButton->setEnabled(false);
	monochromeCutawayButton->setEnabled(false);
	cutawayOpacitySlider->setEnabled(false);
	cutawayThicknessSlider->setEnabled(false);
	cutawayOpacityLabel->setEnabled(false);
	cutawayThicknessLabel->setEnabled(false);
	setButtonState(colorCutawayButton, false);
	setButtonState(monochromeCutawayButton, false);
}

void Preferences::uncheckLightDirections(void)
{
    lightingDir11->setChecked(false);
    lightingDir12->setChecked(false);
    lightingDir13->setChecked(false);
    lightingDir21->setChecked(false);
    lightingDir22->setChecked(false);
    lightingDir23->setChecked(false);
    lightingDir31->setChecked(false);
    lightingDir32->setChecked(false);
    lightingDir33->setChecked(false);
}

void Preferences::disableLighting(void)
{
	qualityLightingButton->setEnabled(false);
	subduedLightingButton->setEnabled(false);
	specularLightingButton->setEnabled(false);
	alternateLightingButton->setEnabled(false);
    effectsUseLIGHTDATButton->setEnabled(false);
    effectsHideLIGHTButton->setEnabled(true);
	lightingDir11->setEnabled(false);
	lightingDir12->setEnabled(false);
	lightingDir13->setEnabled(false);
	lightingDir21->setEnabled(false);
	lightingDir22->setEnabled(false);
	lightingDir23->setEnabled(false);
	lightingDir31->setEnabled(false);
	lightingDir32->setEnabled(false);
	lightingDir33->setEnabled(false);
	setButtonState(qualityLightingButton, false);
	setButtonState(subduedLightingButton, false);
	setButtonState(specularLightingButton, false);
	setButtonState(alternateLightingButton, false);
    setButtonState(effectsUseLIGHTDATButton, false);

	uncheckLightDirections();
}

void Preferences::disableStereo(void)
{
	crossEyedStereoButton->setEnabled(false);
	parallelStereoButton->setEnabled(false);
	stereoAmountSlider->setEnabled(false);
	stereoAmountLabel->setEnabled(false);
	setButtonState(crossEyedStereoButton, false);
	setButtonState(parallelStereoButton, false);
}

void Preferences::disableWireframe(void)
{
	wireframeFogButton->setEnabled(false);
	wireframeRemoveHiddenLineButton->setEnabled(false);
	wireframeThicknessSlider->setEnabled(false);
	wireframeThicknessLabel->setEnabled(false);
	setButtonState(wireframeFogButton, false);
	setButtonState(wireframeRemoveHiddenLineButton, false);
}

void Preferences::disableBFC(void)
{
	bfcRedBackFaceButton->setEnabled(false);
		bfcGreenFrontFaceButton->setEnabled(false);
	bfcBlueNeutralFaceButton->setEnabled(false);
	setButtonState(bfcRedBackFaceButton, false);
		setButtonState(bfcGreenFrontFaceButton, false);
		setButtonState(bfcBlueNeutralFaceButton, false);
}

void Preferences::disableEdgeLines(void)
{
	conditionalLinesButton->setEnabled(false);
	conditionalShowAllButton->setEnabled(false);
	conditionalShowControlPtsButton->setEnabled(false);
	edgesOnlyButton->setEnabled(false);
	highQualityLinesButton->setEnabled(false);
	alwaysBlackLinesButton->setEnabled(false);
	edgeThicknessLabel->setEnabled(false);
	edgeThicknessSlider->setEnabled(false);
	setButtonState(conditionalLinesButton, false);
	setButtonState(conditionalShowAllButton, false);
	setButtonState(conditionalShowControlPtsButton, false);
	setButtonState(edgesOnlyButton, false);
	setButtonState(highQualityLinesButton, false);
	setButtonState(alwaysBlackLinesButton, false);
}

void Preferences::disableConditionalShow(void)
{
	conditionalShowAllButton->setEnabled(false);
    conditionalShowControlPtsButton->setEnabled(false);
    setButtonState(conditionalShowAllButton, false);
    setButtonState(conditionalShowControlPtsButton, false);
}

void Preferences::disablePrimitiveSubstitution(void)
{
	textureStudsButton->setEnabled(false);
	curveQualityLabel->setEnabled(false);
	curveQualitySlider->setEnabled(false);
	setButtonState(textureStudsButton, false);
	if (!useTextureMapsButton->isChecked()) disableTextureStuds();
}

void Preferences::disableTextureStuds(void)
{
	nearestFilteringButton->setEnabled(false);
	bilinearFilteringButton->setEnabled(false);
	trilinearFilteringButton->setEnabled(false);
	anisotropicFilteringButton->setEnabled(false);
	anisotropicFilteringSlider->setEnabled(false);
	anisotropicLabel->setEnabled(false);
	setButtonState(nearestFilteringButton, false);
	setButtonState(bilinearFilteringButton, false);
	setButtonState(trilinearFilteringButton, false);
	setButtonState(anisotropicFilteringButton, false);
}

void Preferences::disableTexmaps(void)
{
	transparentTextureOffsetLabel->setEnabled(false);
	transparentOffsetSlider->setEnabled(false);
}

void Preferences::updateTexmapsEnabled(void)
{
	if (useTextureMapsButton->isChecked())
	{
		enableTexmaps();
	} else
	{
		disableTexmaps();
	}
}

void Preferences::disableProxyServer(void)
{
	proxyLabel->setEnabled(false);
	proxyEdit->setEnabled(false);
	portLabel->setEnabled(false);
	portEdit->setEnabled(false);
}

char *Preferences::getErrorKey(int errorNumber)
{
	static char key[128];

	snprintf(key, sizeof(key), "%s/Error%02d", SHOW_ERRORS_KEY, errorNumber);
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

			memset(matrix, 0, sizeof(matrix));
			matrix[15] = 1.0f;
			if (sscanf(value, "%f,%f,%f,%f,%f,%f,%f,%f,%f",
				&matrix[0], &matrix[4], &matrix[8],
				&matrix[1], &matrix[5], &matrix[9],
				&matrix[2], &matrix[6], &matrix[10]) == 9)
			{
				modelViewer->setDefaultRotationMatrix(matrix);
			}
			delete value;
		}
    }
}

void Preferences::getBackgroundColor(int &r, int &g, int &b)
{
	ldPrefs->getBackgroundColor(r, g, b);
}

bool Preferences::getShowErrors(void)
{
	return ldPrefs->getShowErrors();
}

bool Preferences::getDrawWireframe(void)
{
	return ldPrefs->getDrawWireframe();
}

bool Preferences::getUseWireframeFog(void)
{
	return ldPrefs->getUseWireframeFog();
}

bool Preferences::getRemoveHiddenLines(void)
{
	return ldPrefs->getRemoveHiddenLines();
}

bool Preferences::getTextureStud(void)
{
	return ldPrefs->getTextureStuds();
}

bool Preferences::getShowsHighlightLines(void)
{
	return ldPrefs->getShowHighlightLines();
}

bool Preferences::getUseLighting(void)
{
	return ldPrefs->getUseLighting();
}

bool Preferences::getUseBFC(void)
{
	return ldPrefs->getBfc();
}

bool Preferences::getShowAxes(void)
{
	return ldPrefs->getShowAxes();
}

bool Preferences::getUseSeams(void)
{
	return ldPrefs->getUseSeams();
}

void Preferences::userDefaultChangedAlertCallback(TCAlert *alert)
{
	const char *key = alert->getMessage();

	if (key)
	{
		if (strcmp(key, CHECK_PART_TRACKER_KEY) == 0)
		{
			reflectUpdatesSettings();
		}
	}
}

void Preferences::checkLightVector(void)
{
	LDPreferences::LightDirection selectedDirection =
		getSelectedLightDirection();
	LDPreferences::LightDirection lightDirection =
		ldPrefs->getLightDirection();
	if (selectedDirection != lightDirection)
	{
		uncheckLightDirections();
		selectLightDirection(lightDirection);
	}
}

void Preferences::updateSaveDir(QLineEdit *textField, QPushButton *button,
								LDPreferences::DefaultDirMode dirMode,
								QString &filename)
{
	bool enable = false;

	if (dirMode == LDPreferences::DDMSpecificDir)
	{
		textField->setText(filename);
		enable = true;
	}
	else
	{
		textField->setText("");
	}
	button->setEnabled(enable);
	textField->setEnabled(enable);
}

void Preferences::setupSaveDir(QComboBox *comboBox, QLineEdit *textField,
							   QPushButton *button,
							   LDPreferences::DefaultDirMode dirMode,
							   QString &filename)
{
	comboBox->setCurrentIndex(dirMode);
	updateSaveDir(textField, button, dirMode, filename);
}

void Preferences::setupSaveDirs()
{
	snapshotDir = ldPrefs->getSaveDir(LDPreferences::SOSnapshot).c_str();
	setupSaveDir(snapshotSaveDirBox, snapshotSaveDirEdit,
				 snapshotSaveDirButton,
				 ldPrefs->getSaveDirMode(LDPreferences::SOSnapshot),
				 snapshotDir);
	partsListDir =ldPrefs->getSaveDir(LDPreferences::SOPartsList).c_str();
	setupSaveDir(partsListsSaveDirBox, partsListsSaveDirEdit,
				 partsListsSaveDirButton,
				 ldPrefs->getSaveDirMode(LDPreferences::SOPartsList),
				 partsListDir);
	exportDir = ldPrefs->getSaveDir(LDPreferences::SOExport).c_str();
	setupSaveDir(exportsListsSaveDirBox, exportsSaveDirEdit,
				 exportsSaveDirButton,
				 ldPrefs->getSaveDirMode(LDPreferences::SOExport),
				 exportDir);
}

void Preferences::snapshotSaveDirBoxChanged()
{
	applyButton->setEnabled(true);
	updateSaveDir(snapshotSaveDirEdit,snapshotSaveDirButton,
		(LDPreferences::DefaultDirMode)snapshotSaveDirBox->currentIndex(),
		snapshotDir);
}

void Preferences::partsListsSaveDirBoxChanged()
{
	updateSaveDir(partsListsSaveDirEdit,partsListsSaveDirButton,
		(LDPreferences::DefaultDirMode)partsListsSaveDirBox->currentIndex(),
		partsListDir);
	applyButton->setEnabled(true);
}

void Preferences::exportsListsSaveDirBoxChanged()
{
	updateSaveDir(exportsSaveDirEdit,exportsSaveDirButton,
			(LDPreferences::DefaultDirMode)exportsListsSaveDirBox->currentIndex(),
			exportDir);
	applyButton->setEnabled(true);
}

void Preferences::snapshotSaveDirBrowse()
{
	browseForDir(QString::fromWCharArray(TCLocalStrings::get(L"BrowseForSnapshotDir")),
		snapshotSaveDirEdit, snapshotDir);
}

void Preferences::partsListsSaveDirBrowse()
{
	browseForDir(QString::fromWCharArray(TCLocalStrings::get(L"BrowseForPartsListDir")),
		partsListsSaveDirEdit, partsListDir);
}

void Preferences::exportsSaveDirBrowse()
{
	browseForDir(QString::fromWCharArray(TCLocalStrings::get(L"BrowseForExportListDir")),
		exportsSaveDirEdit, exportDir);
}

void Preferences::customConfigBrowse()
{
	QString selectedfile=QFileDialog::getOpenFileName(this,"Browse for LDraw file","","LDraw file (*.mpd *.ldr *.dat)");
	if (!selectedfile.isEmpty())
	{
		customConfigEdit->setText(selectedfile);
		applyButton->setEnabled(true);
	}
}

void Preferences::browseForDir(QString prompt, QLineEdit *textField, QString &dir)
{
	QString selectedfile=QFileDialog::getExistingDirectory(this,prompt,dir);
    if (!selectedfile.isEmpty())
    {
		textField->setText(dir = selectedfile);
		applyButton->setEnabled(true);
    }
}

void Preferences::enableApply(void)
{
	applyButton->setEnabled(true);
}

void Preferences::enableProxy(void)
{
	doProxyServer(true);
	enableApply();
}

void Preferences::disableProxy(void)
{
	doProxyServer(false);
	enableApply();
}

