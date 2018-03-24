#include "LDViewPreferences.h"
#include <CUI/CUIDialog.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <shlobj.h>
#include <shlwapi.h>
#pragma warning(pop)
#include <LDLib/LDrawModelViewer.h>
#include <LDLib/LDPreferences.h>
#include <LDLoader/LDLPalette.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCWebClient.h>
#include "LDVExtensionsSetup.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include "ModelWindow.h"
#include "LDViewWindow.h"
#include <CUI/CUIScaler.h>

#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <Commctrl.h>
#include <stdio.h>
//#include <tmschema.h>
#include <TRE/TREGLExtensions.h>
#include <HtmlHelp.h>
#include <VersionHelpers.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

//#define WILLY_DEBUG

#ifdef WILLY_DEBUG
void WillyMessage(const char *message)
{
	FILE *debugFile = ucfopen("C:\\LDView-Debug.txt", "a");
	if (debugFile)
	{
		fprintf(debugFile, "%s\n", message);
		fclose(debugFile);
	}
}
#else // WILLY_DEBUG
void WillyMessage(const char *)
{
}
#endif //WILLY_DEBUG

#ifndef IDC_HARDWARE_STEREO
#define IDC_HARDWARE_STEREO 1030
#endif

#if (_WIN32_WINNT < 0x0500)
#define ODS_NOFOCUSRECT 0x0200
#endif // (_WIN32_WINNT < 0x0500)

#define DEFAULT_PREF_SET ls(_UC("DefaultPrefSet"))

ucstring LDViewPreferences::ldviewPath;

LDViewPreferences::LDViewPreferences(HINSTANCE hInstance,
									 LDrawModelViewer* modelViewer)
	:CUIPropertySheet(ls(_UC("LDViewPreferences")), hInstance),
	modelViewer(modelViewer ? ((LDrawModelViewer*)modelViewer->retain()) :
		NULL),
	ldPrefs(new LDPreferences(modelViewer)),
	generalPageNumber(0),
	geometryPageNumber(1),
	effectsPageNumber(2),
	primitivesPageNumber(3),
	updatesPageNumber(4),
	prefSetsPageNumber(5),
	hGeneralPage(NULL),
	hBackgroundColorBitmap(NULL),
	hBackgroundColorButton(NULL),
	hDefaultColorBitmap(NULL),
	hDefaultColorButton(NULL),
	hMouseOverButton(NULL),
	origButtonWindowProc(NULL),
	hButtonColorDC(NULL),
	hGeometryPage(NULL),
	hEffectsPage(NULL),
	hPrimitivesPage(NULL),
	hUpdatesPage(NULL),
	hPrefSetsPage(NULL),
	setActiveWarned(false),
	checkAbandon(true),
	hButtonTheme(NULL)
//	hTabTheme(NULL)
{
	WillyMessage("\n\n\n");
	CUIThemes::init();
	loadSettings();
	applySettings();
	TCAlertManager::registerHandler(TCUserDefaults::alertClass(), this,
		(TCAlertCallback)&LDViewPreferences::userDefaultChangedAlertCallback);
	TCAlertManager::registerHandler(
		LDPreferences::lightVectorChangedAlertClass(), this,
		(TCAlertCallback)&LDViewPreferences::lightVectorChangedCallback);
	lightDirIndexToId[0] = IDR_LIGHT_ANGLE_UL;
	lightDirIndexToId[1] = IDR_LIGHT_ANGLE_UM;
	lightDirIndexToId[2] = IDR_LIGHT_ANGLE_UR;
	lightDirIndexToId[3] = IDR_LIGHT_ANGLE_ML;
	lightDirIndexToId[4] = IDR_LIGHT_ANGLE_MM;
	lightDirIndexToId[5] = IDR_LIGHT_ANGLE_MR;
	lightDirIndexToId[6] = IDR_LIGHT_ANGLE_LL;
	lightDirIndexToId[7] = IDR_LIGHT_ANGLE_LM;
	lightDirIndexToId[8] = IDR_LIGHT_ANGLE_LR;
	for (IntIntMap::const_iterator it = lightDirIndexToId.begin()
		; it != lightDirIndexToId.end(); it++)
	{
		lightDirIdToIndex[it->second] = it->first;
	}
}

LDViewPreferences::~LDViewPreferences(void)
{
}

void LDViewPreferences::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::release(modelViewer);
	TCObject::release(ldPrefs);
	if (hButtonTheme)
	{
		CUIThemes::closeThemeData(hButtonTheme);
		hButtonTheme = NULL;
	}
/*
	if (hTabTheme)
	{
		CUIThemes::closeThemeData(hTabTheme);
		hTabTheme = NULL;
	}
*/
	CUIPropertySheet::dealloc();
}

void LDViewPreferences::lightVectorChangedCallback(TCAlert * /*alert*/)
{
	checkLightVector();
}

void LDViewPreferences::userDefaultChangedAlertCallback(TCAlert *alert)
{
	const char *key = alert->getMessage();

	if (key)
	{
		if (strcmp(key, CHECK_PART_TRACKER_KEY) == 0)
		{
			if (hUpdatesPage)
			{
				setCheck(hUpdatesPage, IDC_CHECK_PART_TRACKER,
					ldPrefs->getCheckPartTracker());
			}
		}
	}
}

void LDViewPreferences::applySettings(void)
{
	ldPrefs->applySettings();
}

void LDViewPreferences::loadSettings(void)
{
	ldPrefs->loadSettings();
}

COLORREF LDViewPreferences::getColor(const char *key, COLORREF defaultColor)
{
	return (COLORREF)htonl(TCUserDefaults::longForKey(key,
		(long)(htonl(defaultColor) >> 8)) << 8);
}

void LDViewPreferences::setColor(const char *key, COLORREF color)
{
	TCUserDefaults::setLongForKey((long)(htonl(color) >> 8), key);
}

void LDViewPreferences::setUseSeams(bool value)
{
	if (value != ldPrefs->getUseSeams())
	{
		ldPrefs->setUseSeams(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_SEAMS, value);
			if (value)
			{
				enableSeams();
			}
			else
			{
				disableSeams();
			}
		}
	}
}

void LDViewPreferences::setDrawWireframe(bool value)
{
	if (value != ldPrefs->getDrawWireframe())
	{
		ldPrefs->setDrawWireframe(value, true, true);
		if (hGeometryPage)
		{
			setupGroupCheckButton(hGeometryPage, IDC_WIREFRAME, value);
			if (value)
			{
				enableWireframe();
			}
			else
			{
				disableWireframe();
			}
		}
	}
}

void LDViewPreferences::setUseWireframeFog(bool value)
{
	if (value != ldPrefs->getUseWireframeFog())
	{
		ldPrefs->setUseWireframeFog(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_WIREFRAME_FOG, value);
		}
	}
}

void LDViewPreferences::setRemoveHiddenLines(bool value)
{
	if (value != ldPrefs->getRemoveHiddenLines())
	{
		ldPrefs->setRemoveHiddenLines(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_REMOVE_HIDDEN_LINES, value);
		}
	}
}

void LDViewPreferences::setEdgesOnly(bool value)
{
	if (value != ldPrefs->getEdgesOnly())
	{
		ldPrefs->setEdgesOnly(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_EDGES_ONLY, value);
		}
	}
}

int LDViewPreferences::getFullScreenRefresh(void)
{
	return ldPrefs->getFullScreenRefresh();
}

int LDViewPreferences::getSeamWidth(void)
{
	return ldPrefs->getSeamWidth();
}

bool LDViewPreferences::getQualityLighting(void)
{
	return ldPrefs->getQualityLighting();
}

bool LDViewPreferences::getUsesFlatShading(void)
{
	return ldPrefs->getUseFlatShading();
}

LDVCutawayMode LDViewPreferences::getCutawayMode(void)
{
	return ldPrefs->getCutawayMode();
}

bool LDViewPreferences::getUsesSpecular(void)
{
	return ldPrefs->getUseSpecular();
}

bool LDViewPreferences::getOneLight(void)
{
	return ldPrefs->getOneLight();
}

bool LDViewPreferences::getDrawLightDats(void)
{
	return ldPrefs->getDrawLightDats();
}

bool LDViewPreferences::getOptionalStandardLight(void)
{
	return ldPrefs->getOptionalStandardLight();
}

bool LDViewPreferences::getPerformSmoothing(void)
{
	return ldPrefs->getPerformSmoothing();
}

bool LDViewPreferences::getShowsFPS(void)
{
	return ldPrefs->getShowFps();
}

bool LDViewPreferences::getLineSmoothing(void)
{
	return ldPrefs->getLineSmoothing();
}

bool LDViewPreferences::getQualityStuds(void)
{
	return ldPrefs->getQualityStuds();
}

bool LDViewPreferences::getAllowPrimitiveSubstitution(void)
{
	return ldPrefs->getAllowPrimitiveSubstitution();
}

bool LDViewPreferences::getShowsHighlightLines(void)
{
	return ldPrefs->getShowHighlightLines();
}

bool LDViewPreferences::getEdgesOnly(void)
{
	return ldPrefs->getEdgesOnly();
}

bool LDViewPreferences::getDrawConditionalHighlights(void)
{
	return ldPrefs->getDrawConditionalHighlights();
}

bool LDViewPreferences::getShowAllConditionalLines(void)
{
	return ldPrefs->getShowAllConditionalLines();
}

bool LDViewPreferences::getShowConditionalControlPoints(void)
{
	return ldPrefs->getShowConditionalControlPoints();
}

bool LDViewPreferences::getUseFlatShading(void)
{
	return ldPrefs->getUseFlatShading();
}

bool LDViewPreferences::getBoundingBoxesOnly(void)
{
	return ldPrefs->getBoundingBoxesOnly();
}

bool LDViewPreferences::getTransDefaultColor(void)
{
	return ldPrefs->getTransDefaultColor();
}

bool LDViewPreferences::getUseSeams(void)
{
	return ldPrefs->getUseSeams();
}

bool LDViewPreferences::getDrawWireframe(void)
{
	return ldPrefs->getDrawWireframe();
}

bool LDViewPreferences::getBfc(void)
{
	return ldPrefs->getBfc();
}

bool LDViewPreferences::getShowAxes(void)
{
	return ldPrefs->getShowAxes();
}

bool LDViewPreferences::getRandomColors(void)
{
	return ldPrefs->getRandomColors();
}

bool LDViewPreferences::getRedBackFaces(void)
{
	return ldPrefs->getRedBackFaces();
}

bool LDViewPreferences::getGreenFrontFaces(void)
{
	return ldPrefs->getGreenFrontFaces();
}

bool LDViewPreferences::getBlueNeutralFaces(void)
{
	return ldPrefs->getBlueNeutralFaces();
}

bool LDViewPreferences::getUseWireframeFog(void)
{
	return ldPrefs->getUseWireframeFog();
}

bool LDViewPreferences::getRemoveHiddenLines(void)
{
	return ldPrefs->getRemoveHiddenLines();
}

bool LDViewPreferences::getUsePolygonOffset(void)
{
	return ldPrefs->getUsePolygonOffset();
}

bool LDViewPreferences::getBlackHighlights(void)
{
	return ldPrefs->getBlackHighlights();
}

bool LDViewPreferences::getUseLighting(void)
{
	return ldPrefs->getUseLighting();
}

bool LDViewPreferences::getSubduedLighting(void)
{
	return ldPrefs->getSubduedLighting();
}

bool LDViewPreferences::getUseStipple(void)
{
	return ldPrefs->getUseStipple();
}

bool LDViewPreferences::getSortTransparent(void)
{
	return ldPrefs->getSortTransparent();
}

bool LDViewPreferences::getTextureStuds(void)
{
	return ldPrefs->getTextureStuds();
}

bool LDViewPreferences::getTexmaps(void)
{
	return ldPrefs->getTexmaps();
}

COLORREF LDViewPreferences::getBackgroundColor(void)
{
	int r, g, b;

	ldPrefs->getBackgroundColor(r, g, b);
	return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}

COLORREF LDViewPreferences::getDefaultColor(void)
{
	int r, g, b;

	ldPrefs->getDefaultColor(r, g, b);
	return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}

void LDViewPreferences::setDrawConditionalHighlights(bool value)
{
	if (value != ldPrefs->getDrawConditionalHighlights())
	{
		ldPrefs->setDrawConditionalHighlights(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_CONDITIONAL_HIGHLIGHTS, value);
		}
	}
}

void LDViewPreferences::setShowAllConditionalLines(bool value)
{
	if (value != ldPrefs->getShowAllConditionalLines())
	{
		ldPrefs->setShowAllConditionalLines(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_ALL_CONDITIONAL, value);
		}
	}
}

void LDViewPreferences::setShowConditionalControlPoints(bool value)
{
	if (value != ldPrefs->getShowConditionalControlPoints())
	{
		ldPrefs->setShowConditionalControlPoints(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_CONDITIONAL_CONTROLS, value);
		}
	}
}

void LDViewPreferences::setUseFlatShading(bool value)
{
	if (value != ldPrefs->getUseFlatShading())
	{
		ldPrefs->setUseFlatShading(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_FLAT_SHADING, value);
		}
	}
}

void LDViewPreferences::setBoundingBoxesOnly(bool value)
{
	if (value != ldPrefs->getBoundingBoxesOnly())
	{
		ldPrefs->setBoundingBoxesOnly(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_PART_BOUNDING_BOXES, value);
		}
	}
}

void LDViewPreferences::setTransDefaultColor(bool value)
{
	if (value != ldPrefs->getTransDefaultColor())
	{
		ldPrefs->setTransDefaultColor(value, true, true);
		if (hGeneralPage)
		{
			setCheck(hGeneralPage, IDC_TRANS_DEFAULT_COLOR, value);
		}
	}
}

void LDViewPreferences::setPerformSmoothing(bool value)
{
	if (value != ldPrefs->getPerformSmoothing())
	{
		ldPrefs->setPerformSmoothing(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_SMOOTH_CURVES, value);
		}
	}
}

void LDViewPreferences::setCutawayMode(LDVCutawayMode value)
{
	if (value != ldPrefs->getCutawayMode())
	{
		ldPrefs->setCutawayMode(value, true, true);
		if (hEffectsPage)
		{
			setupCutaway();
		}
	}
}

void LDViewPreferences::setUsePolygonOffset(bool value)
{
	if (value != ldPrefs->getUsePolygonOffset())
	{
		ldPrefs->setUsePolygonOffset(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_QUALITY_LINES, value);
		}
	}
}

void LDViewPreferences::setBlackHighlights(bool value)
{
	if (value != ldPrefs->getBlackHighlights())
	{
		ldPrefs->setBlackHighlights(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_ALWAYS_BLACK, value);
		}
	}
}

void LDViewPreferences::setShowsHighlightLines(bool value)
{
	if (value != ldPrefs->getShowHighlightLines())
	{
		ldPrefs->setShowHighlightLines(value, true, true);
		if (hGeometryPage)
		{
			setupGroupCheckButton(hGeometryPage, IDC_HIGHLIGHTS, value);
			if (value)
			{
				enableEdges();
			}
			else
			{
				disableEdges();
			}
		}
	}
}

void LDViewPreferences::setTextureStuds(bool value)
{
	if (value != ldPrefs->getTextureStuds())
	{
		ldPrefs->setTextureStuds(value, true, true);
		if (hPrimitivesPage)
		{
			setCheck(hPrimitivesPage, IDC_TEXTURE_STUDS, value);
		}
	}
}

void LDViewPreferences::setTexmaps(bool value)
{
	if (value != ldPrefs->getTexmaps())
	{
		ldPrefs->setTexmaps(value, true, true);
		if (hPrimitivesPage)
		{
			setCheck(hPrimitivesPage, IDC_TEXMAPS, value);
		}
	}
}

void LDViewPreferences::setAllowPrimitiveSubstitution(bool value)
{
	if (value != ldPrefs->getAllowPrimitiveSubstitution())
	{
		ldPrefs->setAllowPrimitiveSubstitution(value, true, true);
		if (hPrimitivesPage)
		{
			setupGroupCheckButton(hPrimitivesPage, IDC_PRIMITIVE_SUBSTITUTION,
				value);
			if (value)
			{
				enablePrimitives();
			}
			else
			{
				disablePrimitives();
			}
		}
	}
}

void LDViewPreferences::setRedBackFaces(bool value)
{
	if (value != ldPrefs->getRedBackFaces())
	{
		ldPrefs->setRedBackFaces(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_RED_BACK_FACES, value);
		}
	}
}

void LDViewPreferences::setGreenFrontFaces(bool value)
{
	if (value != ldPrefs->getGreenFrontFaces())
	{
		ldPrefs->setGreenFrontFaces(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_GREEN_FRONT_FACES, value);
		}
	}
}

void LDViewPreferences::setBlueNeutralFaces(bool value)
{
	if (value != ldPrefs->getBlueNeutralFaces())
	{
		ldPrefs->setBlueNeutralFaces(value, true, true);
		if (hGeometryPage)
		{
			setCheck(hGeometryPage, IDC_BLUE_NEUTRAL_FACES, value);
		}
	}
}

void LDViewPreferences::setBfc(bool value)
{
	if (value != ldPrefs->getBfc())
	{
		ldPrefs->setBfc(value, true, true);
		if (hGeometryPage)
		{
			setupGroupCheckButton(hGeometryPage, IDC_BFC, value);
			if (value)
			{
				enableBfc();
			}
			else
			{
				disableBfc();
			}
		}
	}
}

void LDViewPreferences::setShowAxes(bool value)
{
	if (value != ldPrefs->getShowAxes())
	{
		ldPrefs->setShowAxes(value, true, true);
		if (hGeneralPage)
		{
			setCheck(hGeneralPage, IDC_SHOW_AXES, value);
		}
	}
}

void LDViewPreferences::setRandomColors(bool value)
{
	if (value != ldPrefs->getRandomColors())
	{
		ldPrefs->setRandomColors(value, true, true);
		if (hGeneralPage)
		{
			setCheck(hGeneralPage, IDC_RANDOM_COLORS, value);
		}
	}
}

// This is called from LDViewWindow.
void LDViewPreferences::setQualityLighting(bool value)
{
	if (value != ldPrefs->getQualityLighting())
	{
		ldPrefs->setQualityLighting(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_LIGHTING_QUALITY, value);
		}
	}
}

void LDViewPreferences::setSubduedLighting(bool value)
{
	if (value != ldPrefs->getSubduedLighting())
	{
		ldPrefs->setSubduedLighting(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_LIGHTING_SUBDUED, value);
		}
	}
}

void LDViewPreferences::setQualityStuds(bool value)
{
	if (value != ldPrefs->getQualityStuds())
	{
		ldPrefs->setQualityStuds(value, true, true);
		if (hPrimitivesPage)
		{
			setCheck(hPrimitivesPage, IDC_STUD_QUALITY, !value);
		}
	}
}

void LDViewPreferences::setUsesFlatShading(bool value)
{
	if (value != ldPrefs->getUseFlatShading())
	{
		ldPrefs->setUseFlatShading(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_FLAT_SHADING, value);
		}
	}
}

void LDViewPreferences::setUsesSpecular(bool value)
{
	if (value != ldPrefs->getUseSpecular())
	{
		ldPrefs->setUseSpecular(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_SPECULAR, value);
		}
	}
}

void LDViewPreferences::setOneLight(bool value)
{
	if (value != ldPrefs->getOneLight())
	{
		ldPrefs->setOneLight(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_ALTERNATE_LIGHTING, value);
		}
	}
}

void LDViewPreferences::setDrawLightDats(bool value)
{
	if (value != ldPrefs->getDrawLightDats())
	{
		ldPrefs->setDrawLightDats(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_DRAW_LIGHT_DATS, value);
		}
	}
}

void LDViewPreferences::setOptionalStandardLight(bool value)
{
	if (value != ldPrefs->getOptionalStandardLight())
	{
		ldPrefs->setOptionalStandardLight(value, true, true);
		if (hEffectsPage)
		{
			setCheck(hEffectsPage, IDC_OPTIONAL_STANDARD_LIGHT, value);
		}
	}
}

void LDViewPreferences::setUseLighting(bool value)
{
	if (value != ldPrefs->getUseLighting())
	{
		ldPrefs->setUseLighting(value, true, true);
		if (hEffectsPage)
		{
			setupGroupCheckButton(hEffectsPage, IDC_LIGHTING, value);
			if (value)
			{
				enableLighting();
			}
			else
			{
				disableLighting();
			}
		}
	}
}

INT_PTR LDViewPreferences::run(void)
{
	bool wasPaused = true;
	INT_PTR retValue;

	if (modelViewer)
	{
		wasPaused = modelViewer->getPaused() != 0;
		if (!wasPaused)
		{
			modelViewer->pause();
		}
	}
	addPage(IDD_GENERAL_PREFS);
	addPage(IDD_GEOMETRY_PREFS); 
	addPage(IDD_EFFECTS_PREFS);
	addPage(IDD_PRIMITIVES_PREFS);
	addPage(IDD_UPDATES_PREFS);
	addPage(IDD_PREFSETS_PREFS);
	checkAbandon = true;
	retValue = CUIPropertySheet::run();
	if (!wasPaused)
	{
		modelViewer->unpause();
	}
	return retValue;
}

BOOL LDViewPreferences::doDialogNotify(HWND hDlg, int controlId,
									   LPNMHDR notification)
{
//	debugPrintf("LDViewPreferences::doDialogNotify: %d 0x%08X\n",
//		notification->code, notification->code);
	if (notification->code == NM_RELEASEDCAPTURE)
	{
		if (hDlg == hEffectsPage)
		{
			if (controlId == IDC_STEREO_SPACING ||
				controlId == IDC_CUTAWAY_OPACITY ||
				controlId == IDC_CUTAWAY_THICKNESS)
			{
				enableApply(hEffectsPage);
				return FALSE;
			}
		}
		else if (hDlg == hGeometryPage)
		{
			if (controlId == IDC_WIREFRAME_THICKNESS ||
				controlId == IDC_EDGE_THICKNESS)
			{
				enableApply(hGeometryPage);
				return FALSE;
			}
		}
		else if (hDlg == hPrimitivesPage)
		{
			if (controlId == IDC_CURVE_QUALITY)
			{
				enableApply(hPrimitivesPage);
				return FALSE;
			}
			else if (controlId == IDC_ANISO_LEVEL)
			{
				enableApply(hPrimitivesPage);
				setAniso(trackBarGetPos(hAnisoLevelSlider));
				return FALSE;
			}
			else if (controlId == IDC_TEXTURE_OFFSET)
			{
				enableApply(hPrimitivesPage);
				return FALSE;
			}
		}
	}
	else if (notification->code == CBN_SELCHANGE)
	{
		debugPrintf("combo sel\n");
	}
	return CUIPropertySheet::doDialogNotify(hDlg, controlId, notification);
}

UCSTR LDViewPreferences::getLDViewPath(bool useQuotes)
{
	UCCHAR origPath[MAX_PATH];
	UCCHAR newPath[MAX_PATH];

	if (ldviewPath.empty())
	{
		UCSTR commandLine = copyString(GetCommandLine());

		PathRemoveArgs(commandLine);
		PathUnquoteSpaces(commandLine);
		PathRemoveFileSpec(commandLine);
		GetCurrentDirectory(MAX_PATH, origPath);
		if (SetCurrentDirectory(commandLine))
		{
			GetCurrentDirectory(MAX_PATH, newPath);
			PathUnquoteSpaces(newPath);
			ldviewPath = newPath;
			SetCurrentDirectory(origPath);
		}
		else
		{
			ldviewPath = origPath;
		}
		delete commandLine;
	}
	ucstrcpy(newPath, ldviewPath.c_str());
	if (useQuotes)
	{
		PathQuoteSpaces(newPath);
	}
	return copyString(newPath);
}

UCSTR LDViewPreferences::getLDViewPath(CUCSTR helpFilename, bool useQuotes)
{
	UCSTR programPath = getLDViewPath();
	UCCHAR tmpPath[MAX_PATH];

	ucstrcpy(tmpPath, programPath);
	delete[] programPath;
	if (helpFilename)
	{
		PathAppend(tmpPath, helpFilename);
	}
	if (useQuotes)
	{
		PathQuoteSpaces(tmpPath);
	}
	return copyString(tmpPath);
}

DWORD LDViewPreferences::getPageDialogID(HWND hDlg)
{
	if (hDlg == hGeneralPage)
	{
		return IDD_GENERAL_PREFS;
	}
	else if (hDlg == hGeometryPage)
	{
		return IDD_GEOMETRY_PREFS;
	}
	else if (hDlg == hEffectsPage)
	{
		return IDD_EFFECTS_PREFS;
	}
	else if (hDlg == hPrimitivesPage)
	{
		return IDD_PRIMITIVES_PREFS;
	}
	else if (hDlg == hUpdatesPage)
	{
		return IDD_UPDATES_PREFS;
	}
	else if (hDlg == hPrefSetsPage)
	{
		return IDD_PREFSETS_PREFS;
	}
	return 0;
}

BOOL LDViewPreferences::doDialogHelp(HWND hDlg, LPHELPINFO helpInfo)
{
	DWORD dialogId = getPageDialogID(hDlg);
	BOOL retValue = FALSE;

	if (dialogId)
	{
		static bool useWinHelp = false;
		if (useWinHelp)
		{
			UCSTR helpPath = getLDViewPath(ls(_UC("LDView.hlp")));
			DWORD helpId;

			helpId = 0x80000000 | (dialogId << 16) | (DWORD)helpInfo->iCtrlId;
			WinHelp((HWND)helpInfo->hItemHandle, helpPath, HELP_CONTEXTPOPUP,
				helpId);
			retValue = TRUE;
			delete[] helpPath;
		}
		else
		{
			RECT itemRect;
			GetWindowRect((HWND)helpInfo->hItemHandle, &itemRect);
			DWORD helpId;
			HH_POPUP hhp;

			memset(&hhp, 0, sizeof(hhp));
			hhp.cbStruct = sizeof(hhp);
			//hhp.hinst = getLanguageModule();
			hhp.pt = helpInfo->MousePos;
			hhp.clrForeground = hhp.clrBackground = -1;
			memset(&hhp.rcMargins, -1, sizeof(hhp.rcMargins));
			//hhp.pszFont = ",12,,";

			helpId = 0x80000000 | (dialogId << 16) | (DWORD)helpInfo->iCtrlId;
			//hhp.idString = helpId;

			UCCHAR stringBuffer[65536];
			if (LoadString(getLanguageModule(), helpId, stringBuffer, COUNT_OF(stringBuffer)) > 0)
			{
				// If HtmlHelp loads the string, it can be truncated. :-(
				hhp.pszText = stringBuffer;
			}
			else
			{
				hhp.idString = 1; // No Help topic...
			}
			HtmlHelp((HWND)helpInfo->hItemHandle, NULL, HH_DISPLAY_TEXT_POPUP,
				(DWORD_PTR)&hhp);
			retValue = TRUE;
		}
	}
	return retValue;
}

ucstring LDViewPreferences::getPrefSet(int index)
{
	ucstring prefSet;
	listBoxGetText(hPrefSetsList, index, prefSet);
	return prefSet;
}

ucstring LDViewPreferences::getSelectedPrefSet(void)
{
	int selectedIndex = listBoxGetCurSel(hPrefSetsList);

	if (selectedIndex != LB_ERR)
	{
		return getPrefSet(selectedIndex);
	}
	return ucstring();
}

void LDViewPreferences::abandonChanges(void)
{
	int i;
	HWND hPage;

	doReset();
	for (i = 0; i < prefSetsPageNumber; i++)
	{
		hPage = hwndArray->pointerAtIndex(i);
		if (hPage)
		{
			disableApply(hPage);
			hwndArray->replacePointer(NULL, i);
		}
	}
	hPage = hwndArray->pointerAtIndex(prefSetsPageNumber);
	if (hPage)
	{
		disableApply(hPage);
	}
}

INT_PTR LDViewPreferences::runPrefSetApplyDialog(void)
{
	INT_PTR retValue = DialogBoxParam(getLanguageModule(),
		MAKEINTRESOURCE(IDD_PREFSET_APPLY), hWindow, staticDialogProc,
		(LPARAM)this);

	if (retValue == IDC_APPLY)
	{
		PropSheet_Apply(hPropSheet);
	}
	else if (retValue == IDC_ABANDON)
	{
		abandonChanges();
	}
	return retValue;
}

BOOL LDViewPreferences::doPrefSetSelected(bool force)
{
	ucstring selectedPrefSet = getSelectedPrefSet();
	bool needToReselect = false;

	if (checkAbandon && getApplyEnabled() && !force)
	{
		char *savedSession =
			TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
		ucstring ucSavedSession;

		if (!savedSession || !savedSession[0])
		{
			ucSavedSession = DEFAULT_PREF_SET;
		}
		else
		{
			 utf8toucstring(ucSavedSession, savedSession);
		}
		delete[] savedSession;
		if (selectedPrefSet == ucSavedSession)
		{
			return FALSE;
		}
		else
		{
			needToReselect = true;
			selectPrefSet(_UC(""), true);
			if (runPrefSetApplyDialog() == IDCANCEL)
			{
				checkAbandon = true;
				return TRUE;
			}
		}
	}
	if (!selectedPrefSet.empty())
	{
		BOOL enabled = TRUE;

		if (needToReselect)
		{
			selectPrefSet(selectedPrefSet);
		}
		if (selectedPrefSet == DEFAULT_PREF_SET)
		{
			enabled = FALSE;
		}
		EnableWindow(hDeletePrefSetButton, enabled);
	}
	enableApply(hPrefSetsPage);
	checkAbandon = false;
	return FALSE;
}

BOOL LDViewPreferences::doDialogThemeChanged(void)
{
	if (hButtonTheme)
	{
		CUIThemes::closeThemeData(hButtonTheme);
		hButtonTheme = NULL;
	}
/*
	if (hTabTheme)
	{
		CUIThemes::closeThemeData(hTabTheme);
		hTabTheme = NULL;
	}
*/
	if (CUIThemes::isThemeLibLoaded())
	{
		if (hBackgroundColorButton)
		{
			setupBackgroundColorButton();
		}
		if (hDefaultColorButton)
		{
			setupDefaultColorButton();
		}
		if (hGeometryPage)
		{
			setupGroupCheckButton(hGeometryPage, IDC_WIREFRAME,
				ldPrefs->getDrawWireframe());
		}
/*
		if (hLightDirStatic)
		{
			initThemesTab(hLightDirStatic);
		}
*/
	}
	return FALSE;
}

BOOL LDViewPreferences::doDialogCommand(HWND hDlg, int controlId,
										int notifyCode, HWND controlHWnd)
{
	UCCHAR className[1024];

	GetClassName(controlHWnd, className, COUNT_OF(className));
	if (ucstrcmp(className, WC_COMBOBOX) == 0)
	{
		if (notifyCode == CBN_SELCHANGE)
		{
			return doComboSelChange(hDlg, controlId, controlHWnd);
		}
	}
	else if (notifyCode == BN_CLICKED)
	{
		return doClick(hDlg, controlId, controlHWnd);
	}
	else if (notifyCode == EN_CHANGE)
	{
		if (controlId == IDC_FS_RATE || controlId == IDC_FOV ||
			controlId == IDC_PROXY_SERVER || controlId == IDC_PROXY_PORT ||
			controlId == IDC_MISSING_DAYS || controlId == IDC_UPDATED_DAYS)
		{
			enableApply(hDlg);
			return 0;
		}
	}
	else if (notifyCode == LBN_SELCHANGE)
	{
		if (hDlg == hPrefSetsPage)
		{
			return doPrefSetSelected();
		}
	}
	return LDP_UNKNOWN_COMMAND;
}

void LDViewPreferences::setupBackgroundColorButton(void)
{
	setupColorButton(hGeneralPage, hBackgroundColorButton,
		IDC_BACKGROUND_COLOR, hBackgroundColorBitmap,
		getBackgroundColor());
}

void LDViewPreferences::setupDefaultColorButton(void)
{
	setupColorButton(hGeneralPage, hDefaultColorButton,
		IDC_DEFAULT_COLOR, hDefaultColorBitmap,
		getDefaultColor());
}

LRESULT CALLBACK LDViewPreferences::staticIconButtonProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LDViewPreferences *thisPtr =
		(LDViewPreferences *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return thisPtr->iconButtonProc(hWnd, message, wParam, lParam);
}

LRESULT LDViewPreferences::iconButtonProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		if (hMouseOverButton != hWnd)
		{
			if (hMouseOverButton)
			{
				InvalidateRect(hMouseOverButton, NULL, FALSE);
			}
			hMouseOverButton = hWnd;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	return CallWindowProc((WNDPROC)origButtonWindowProc, hWnd, message, wParam,
		lParam);
}

LRESULT CALLBACK LDViewPreferences::staticGroupCheckButtonProc(HWND hWnd,
															   UINT message,
															   WPARAM wParam,
															   LPARAM lParam)
{
	LDViewPreferences *thisPtr =
		(LDViewPreferences *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return thisPtr->groupCheckButtonProc(hWnd, message, wParam, lParam);
}

LRESULT LDViewPreferences::groupCheckButtonProc(HWND hWnd, UINT message,
												WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		if (hMouseOverButton != hWnd)
		{
			if (hMouseOverButton)
			{
				InvalidateRect(hMouseOverButton, NULL, FALSE);
			}
			hMouseOverButton = hWnd;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	return CallWindowProc((WNDPROC)origButtonWindowProc, hWnd, message, wParam,
		lParam);
}

LRESULT CALLBACK LDViewPreferences::staticColorButtonProc(HWND hWnd,
														  UINT message,
														  WPARAM wParam,
														  LPARAM lParam)
{
	LDViewPreferences *thisPtr =
		(LDViewPreferences *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return thisPtr->colorButtonProc(hWnd, message, wParam, lParam);
}

LRESULT LDViewPreferences::colorButtonProc(HWND hWnd, UINT message,
										   WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		if (hMouseOverButton != hWnd)
		{
			if (hMouseOverButton)
			{
				InvalidateRect(hMouseOverButton, NULL, FALSE);
			}
			hMouseOverButton = hWnd;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	return CallWindowProc((WNDPROC)origButtonWindowProc, hWnd, message, wParam,
		lParam);
}

void LDViewPreferences::setupIconButton(HWND hButton)
{
	initThemesButton(hButton);
	if (hButtonTheme)
	{
		LONG_PTR oldWindowProc;
		DWORD dwStyle = GetWindowLong(hButton, GWL_STYLE);
		int buttonType = dwStyle & BS_TYPEMASK;

		buttonTypes[hButton] = buttonType;
		if (buttonType != BS_OWNERDRAW)
		{
			dwStyle = (dwStyle & ~BS_TYPEMASK) | BS_OWNERDRAW;
			buttonSetStyle(hButton, dwStyle);
		}
		// subclass the button so we can know when the mouse has moved over it
		SetWindowLongPtr(hButton, GWLP_USERDATA, (LONG_PTR)this);
		oldWindowProc = SetWindowLongPtr(hButton, GWLP_WNDPROC,
			(LONG_PTR)staticIconButtonProc);
		if (!origButtonWindowProc)
		{
			origButtonWindowProc = oldWindowProc;
		}
	}
	else
	{
		if (origButtonWindowProc)
		{
			if (buttonTypes.find(hButton) != buttonTypes.end())
			{
				DWORD dwStyle = GetWindowLong(hButton, GWL_STYLE);

				dwStyle = (dwStyle & ~BS_TYPEMASK) | buttonTypes[hButton];
				buttonSetStyle(hButton, dwStyle);
				// Put the window proc back if we've every overridden one.  Maybe
				// the user disabled themes.
				SetWindowLongPtr(hButton, GWLP_WNDPROC,
					(LONG_PTR)origButtonWindowProc);
			}
		}
	}
}

void LDViewPreferences::setupColorButton(HWND hPage, HWND &hColorButton,
										 int controlID, HBITMAP &hButtonBitmap,
										 COLORREF color)
{
	int imageWidth;
	int imageHeight;
	HDC hPageDC;
	RECT clientRect;

	initThemesButton(hColorButton);
	if (hButtonBitmap)
	{
		DeleteObject(hButtonBitmap);
		if (origButtonWindowProc)
		{
			SetWindowLongPtr(hColorButton, GWLP_WNDPROC,
				(LONG_PTR)origButtonWindowProc);
		}
	}
	hColorButton = GetDlgItem(hPage, controlID);
	GetClientRect(hColorButton, &clientRect);
	if (hButtonTheme)
	{
		RECT contentRect;

		// subclass the Wizard button so we can know when the mouse has moved
		// over it
		SetWindowLongPtr(hColorButton, GWLP_USERDATA, (LONG_PTR)this);
		if (origButtonWindowProc)
		{
			SetWindowLongPtr(hColorButton, GWLP_WNDPROC,
				(LONG_PTR)staticColorButtonProc);
		}
		else
		{
			origButtonWindowProc = SetWindowLongPtr(hColorButton, GWLP_WNDPROC,
				(LONG_PTR)staticColorButtonProc);
		}
		CUIThemes::getThemeBackgroundContentRect(hButtonTheme, NULL,
			BP_PUSHBUTTON, PBS_HOT, &clientRect, &contentRect);
		LONG margin = scalePoints(6);
		imageWidth = contentRect.right - contentRect.left - margin;
		imageHeight = contentRect.bottom - contentRect.top - margin;
	}
	else
	{
		LONG margin = scalePoints(10);
		imageWidth = clientRect.right - clientRect.left - margin;
		imageHeight = clientRect.bottom - clientRect.top - margin;
	}
	hPageDC = GetDC(hPage);
	if (!hButtonColorDC)
	{
		hButtonColorDC = CreateCompatibleDC(hPageDC);
	}
	hButtonBitmap = CreateCompatibleBitmap(hPageDC, imageWidth, imageHeight);
	ReleaseDC(hPage, hPageDC);
	SetBitmapDimensionEx(hButtonBitmap, imageWidth, imageHeight, NULL);
	redrawColorBitmap(hColorButton, hButtonBitmap, color);
	CUIDialog::buttonSetBitmap(hPage, controlID, hButtonBitmap);
}

void LDViewPreferences::enableSeams(void)
{
	UCCHAR seamWidthString[128];

	EnableWindow(hSeamSpin, TRUE);
	setCheck(hGeometryPage, IDC_SEAMS, true);
	sucprintf(seamWidthString, COUNT_OF(seamWidthString), _UC("%0.2f"),
		ldPrefs->getSeamWidth() / 100.0f);
	CUIDialog::windowSetText(hGeometryPage, IDC_SEAM_WIDTH_FIELD, seamWidthString);
}

void LDViewPreferences::disableSeams(void)
{
	EnableWindow(hSeamSpin, FALSE);
	setCheck(hGeometryPage, IDC_SEAMS, false);
	CUIDialog::windowSetText(hGeometryPage, IDC_SEAM_WIDTH_FIELD, _UC(""));
}

void LDViewPreferences::enableTextureFiltering(void)
{
	int activeTextureFilter;

	EnableWindow(hTextureNearestButton, TRUE);
	EnableWindow(hTextureBilinearButton, TRUE);
	EnableWindow(hTextureTrilinearButton, TRUE);
	EnableWindow(hAnisoLevelLabel, TRUE);
	if (TREGLExtensions::haveAnisoExtension())
	{
		EnableWindow(hTextureAnisoButton, TRUE);
	}
	else
	{
		EnableWindow(hTextureAnisoButton, FALSE);
	}
	setCheck(hPrimitivesPage, IDC_TEXTURE_NEAREST, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_BILINEAR, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_TRILINEAR, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_ANISO, false);
	switch (ldPrefs->getTextureFilterType())
	{
	case GL_NEAREST_MIPMAP_NEAREST:
		activeTextureFilter = IDC_TEXTURE_NEAREST;
		break;
	case GL_LINEAR_MIPMAP_NEAREST:
		activeTextureFilter = IDC_TEXTURE_BILINEAR;
		break;
	case GL_LINEAR_MIPMAP_LINEAR:
		activeTextureFilter = IDC_TEXTURE_TRILINEAR;
		break;
	default:
		ldPrefs->setTextureFilterType(GL_LINEAR_MIPMAP_LINEAR);
		activeTextureFilter = IDC_TEXTURE_TRILINEAR;
		break;
	}
	if (activeTextureFilter == IDC_TEXTURE_TRILINEAR &&
		ldPrefs->getAnisoLevel() > 1.0)
	{
		EnableWindow(hAnisoLevelSlider, TRUE);
		activeTextureFilter = IDC_TEXTURE_ANISO;
	}
	else
	{
		EnableWindow(hAnisoLevelSlider, FALSE);
	}
	setCheck(hPrimitivesPage, activeTextureFilter, true);
}

void LDViewPreferences::disableTextureFiltering(void)
{
	EnableWindow(hTextureNearestButton, FALSE);
	EnableWindow(hTextureBilinearButton, FALSE);
	EnableWindow(hTextureTrilinearButton, FALSE);
	EnableWindow(hTextureAnisoButton, FALSE);
	EnableWindow(hAnisoLevelSlider, FALSE);
	EnableWindow(hAnisoLevelLabel, FALSE);
	setCheck(hPrimitivesPage, IDC_TEXTURE_NEAREST, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_BILINEAR, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_TRILINEAR, false);
	setCheck(hPrimitivesPage, IDC_TEXTURE_ANISO, false);
}

void LDViewPreferences::enableTexmaps(void)
{
	EnableWindow(hTexmapsAfterTransparentButton, TRUE);
	EnableWindow(hTextureOffsetLabel, TRUE);
	EnableWindow(hTextureOffsetSlider, TRUE);
	setCheck(hPrimitivesPage, IDC_TRANSPARENT_TEXTURES_LAST, ldPrefs->getTexturesAfterTransparent());
}

void LDViewPreferences::disableTexmaps(void)
{
	EnableWindow(hTexmapsAfterTransparentButton, FALSE);
	EnableWindow(hTextureOffsetLabel, FALSE);
	EnableWindow(hTextureOffsetSlider, FALSE);
	setCheck(hPrimitivesPage, IDC_TRANSPARENT_TEXTURES_LAST, false);
}

void LDViewPreferences::enablePrimitives(void)
{
	EnableWindow(hTextureStudsButton, TRUE);
	EnableWindow(hCurveQualityLabel, TRUE);
	EnableWindow(hCurveQualitySlider, TRUE);
	setCheck(hPrimitivesPage, IDC_TEXTURE_STUDS,ldPrefs->getTextureStuds());
	updateTextureFilteringEnabled();
	updateTexmapsEnabled();
}

void LDViewPreferences::disablePrimitives(void)
{
	EnableWindow(hTextureStudsButton, FALSE);
	EnableWindow(hCurveQualityLabel, FALSE);
	EnableWindow(hCurveQualitySlider, FALSE);
	setCheck(hPrimitivesPage, IDC_TEXTURE_STUDS, false);
	updateTextureFilteringEnabled();
	updateTexmapsEnabled();
}

void LDViewPreferences::setupSeamWidth(void)
{
	UDACCEL accels[] = {{0, 1}, {1, 10}, {2, 20}};

	hSeamSpin = GetDlgItem(hGeometryPage, IDC_SEAM_SPIN);
	SendDlgItemMessage(hGeometryPage, IDC_SEAM_SPIN, UDM_SETRANGE, 0,
		MAKELONG((short)500, (short)0)); 
	SendDlgItemMessage(hGeometryPage, IDC_SEAM_SPIN, UDM_SETPOS, 0,
		ldPrefs->getSeamWidth());
	SendDlgItemMessage(hGeometryPage, IDC_SEAM_SPIN, UDM_SETACCEL,
		COUNT_OF(accels), (LPARAM)accels);
	if (ldPrefs->getUseSeams())
	{
		enableSeams();
	}
	else
	{
		disableSeams();
	}
}

void LDViewPreferences::setupFullScreenRefresh(void)
{
	int fullScreenRefresh = ldPrefs->getFullScreenRefresh();

	hFullScreenRateField = GetDlgItem(hGeneralPage, IDC_FS_RATE);
	SendDlgItemMessage(hGeneralPage, IDC_FS_RATE, EM_SETLIMITTEXT, 3, 0);
	if (fullScreenRefresh)
	{
		UCCHAR buf[128];

		sucprintf(buf, COUNT_OF(buf), _UC("%d"), fullScreenRefresh);
		buf[3] = 0;
		CUIDialog::windowSetText(hGeneralPage, IDC_FS_RATE, buf);
		setCheck(hGeneralPage, IDC_FS_REFRESH, true);
		EnableWindow(hFullScreenRateField, TRUE);
	}
	else
	{
		CUIDialog::windowSetText(hGeneralPage, IDC_FS_RATE, _UC(""));
		setCheck(hGeneralPage, IDC_FS_REFRESH, false);
		EnableWindow(hFullScreenRateField, FALSE);
	}
}

void LDViewPreferences::redrawColorBitmap(HWND hColorButton,
										  HBITMAP hButtonBitmap, COLORREF color)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	RECT bitmapRect;
	SIZE bitmapSize;
	HBITMAP hOldBitmap;
	HBRUSH hOldBrush;

	GetBitmapDimensionEx(hButtonBitmap, &bitmapSize);
	bitmapRect.left = 0;
	bitmapRect.top = 0;
	bitmapRect.right = bitmapSize.cx;
	bitmapRect.bottom = bitmapSize.cy;
	hOldBrush = (HBRUSH)SelectObject(hButtonColorDC, hBrush);
	hOldBitmap = (HBITMAP)SelectObject(hButtonColorDC, hButtonBitmap);
	FillRect(hButtonColorDC, &bitmapRect, hBrush);
	if (CUIThemes::isThemeLibLoaded() && hButtonTheme)
	{
		CUIThemes::drawThemeEdge(hButtonTheme, hButtonColorDC, BP_PUSHBUTTON,
			PBS_PRESSED, &bitmapRect, EDGE_SUNKEN, BF_SOFT | BF_RECT, NULL);
	}
	else
	{
		DrawEdge(hButtonColorDC, &bitmapRect, EDGE_SUNKEN,
			BF_BOTTOMLEFT | BF_TOPRIGHT);
	}
	SelectObject(hButtonColorDC, hOldBrush);
	SelectObject(hButtonColorDC, hOldBitmap);
	DeleteObject(hBrush);
	InvalidateRect(hColorButton, NULL, 1);
}

void LDViewPreferences::applyPrefSetsChanges(void)
{
	if (hPrefSetsPage)
	{
		TCStringArray *oldPrefSetNames = TCUserDefaults::getAllSessionNames();
		int i;
		int count = oldPrefSetNames->getCount();
		ucstring ucPrefSetName;
		const char *sessionName = TCUserDefaults::getSessionName();
		bool changed = false;

		for (i = 0; i < count; i++)
		{
			char *oldPrefSetName = oldPrefSetNames->stringAtIndex(i);
			ucstring ucOldPrefSetName;
			utf8toucstring(ucOldPrefSetName, oldPrefSetName);
			int index = listBoxFindStringExact(hPrefSetsList, ucOldPrefSetName);

			if (index == LB_ERR)
			{
				TCUserDefaults::removeSession(oldPrefSetName);
			}
		}
		count = listBoxGetCount(hPrefSetsList);
		for (i = 1; i < count; i++)
		{
			ucPrefSetName = getPrefSet(i);
			std::string prefSetName;
			ucstringtoutf8(prefSetName, ucPrefSetName);
			if (oldPrefSetNames->indexOfString(prefSetName.c_str()) < 0)
			{
				TCUserDefaults::setSessionName(prefSetName.c_str(),
					PREFERENCE_SET_KEY);
			}
		}
		oldPrefSetNames->release();
		ucPrefSetName = getSelectedPrefSet();
		if (ucPrefSetName == DEFAULT_PREF_SET)
		{
			if (sessionName && sessionName[0])
			{
				TCUserDefaults::setSessionName(NULL, PREFERENCE_SET_KEY);
				changed = true;
			}
		}
		else
		{
			ucstring ucSessionName;
			utf8toucstring(ucSessionName, sessionName);
			if (ucPrefSetName != ucSessionName)
			{
				std::string prefSetName;
				ucstringtoutf8(prefSetName, ucPrefSetName);
				TCUserDefaults::setSessionName(prefSetName.c_str(),
					PREFERENCE_SET_KEY);
				changed = true;
			}
		}
		if (changed)
		{
			loadSettings();
			applySettings();
			if (hGeneralPage)
			{
				setupPage(generalPageNumber);
			}
			if (hGeometryPage)
			{
				setupPage(geometryPageNumber);
			}
			if (hEffectsPage)
			{
				setupPage(effectsPageNumber);
			}
			if (hPrimitivesPage)
			{
				setupPage(primitivesPageNumber);
			}
			if (hUpdatesPage)
			{
				setupPage(updatesPageNumber);
			}
		}
	}
}

void LDViewPreferences::applyGeneralChanges(void)
{
	if (hGeneralPage)
	{
		int iTemp = 0;
		float fTemp;

		ldPrefs->setLineSmoothing(getCheck(hGeneralPage, IDC_LINE_AA));
		ldPrefs->setTransDefaultColor(getCheck(hGeneralPage,
			IDC_TRANS_DEFAULT_COLOR));
		ldPrefs->setProcessLdConfig(getCheck(hGeneralPage,
			IDC_PROCESS_LDCONFIG));
		ldPrefs->setRandomColors(getCheck(hGeneralPage, IDC_RANDOM_COLORS));
		ldPrefs->setShowFps(getCheck(hGeneralPage, IDC_FRAME_RATE));
		ldPrefs->setShowAxes(getCheck(hGeneralPage, IDC_SHOW_AXES));
		ldPrefs->setShowErrors(getCheck(hGeneralPage, IDC_SHOW_ERRORS));
		ldPrefs->setMemoryUsage(CUIDialog::comboGetCurSel(hGeneralPage,
			IDC_MEMORY_COMBO));
		if (getCheck(hGeneralPage, IDC_FS_REFRESH))
		{
			if (CUIDialog::windowGetValue(hGeneralPage, IDC_FS_RATE, iTemp))
			{
				if (!iTemp)
				{
					iTemp = -1;
				}
			}
			else
			{
				iTemp = -1;
			}
		}
		if (iTemp >= 0)
		{
			ldPrefs->setFullScreenRefresh(iTemp);
		}
		if (CUIDialog::windowGetValue(hGeneralPage, IDC_FOV, fTemp))
		{
			if (fTemp >= getMinFov() && fTemp <= getMaxFov())
			{
				ldPrefs->setFov(fTemp);
			}
			else
			{
				setupFov(true);
			}
		}
		else
		{
			setupFov(true);
		}
		ldPrefs->setSnapshotsDirMode(snapshotDirMode);
		if (snapshotDirMode == LDPreferences::DDMSpecificDir)
		{
			windowGetText(hSnapshotDirField, snapshotDir);
			if (snapshotDir.length() > 0)
			{
				std::string utf8Dir;
				ucstringtoutf8(utf8Dir, snapshotDir);
				ldPrefs->setSnapshotsDir(utf8Dir.c_str());
			}
			else
			{
				ldPrefs->setSnapshotsDirMode(LDPreferences::DDMLastDir);
			}
		}
		ldPrefs->setPartsListsDirMode(partsListDirMode);
		if (partsListDirMode == LDPreferences::DDMSpecificDir)
		{
			windowGetText(hPartsListDirField, partsListDir);
			if (partsListDir.length() > 0)
			{
				std::string utf8Dir;
				ucstringtoutf8(utf8Dir, partsListDir);
				ldPrefs->setPartsListsDir(utf8Dir.c_str());
			}
			else
			{
				ldPrefs->setPartsListsDirMode(LDPreferences::DDMLastDir);
			}
		}
		ldPrefs->setSaveDirMode(LDPreferences::SOExport, exportDirMode);
		if (exportDirMode == LDPreferences::DDMSpecificDir)
		{
			windowGetText(hExportDirField, exportDir);
			if (exportDir.length() > 0)
			{
				std::string utf8Dir;
				ucstringtoutf8(utf8Dir, exportDir);
				ldPrefs->setSaveDir(LDPreferences::SOExport, utf8Dir.c_str());
			}
			else
			{
				ldPrefs->setSaveDirMode(LDPreferences::SOExport,
					LDPreferences::DDMLastDir);
			}
		}
		ldPrefs->applyGeneralSettings();
	}
	ldPrefs->commitGeneralSettings();
}

void LDViewPreferences::applyGeometryChanges(void)
{
	if (hGeometryPage)
	{
		ldPrefs->setUseSeams(getCheck(hGeometryPage, IDC_SEAMS));
		ldPrefs->setBoundingBoxesOnly(getCheck(hGeometryPage,
			IDC_PART_BOUNDING_BOXES));
		ldPrefs->setDrawWireframe(getCachedCheck(hGeometryPage, IDC_WIREFRAME));
		ldPrefs->setUseWireframeFog(getCheck(hGeometryPage, IDC_WIREFRAME_FOG));
		ldPrefs->setRemoveHiddenLines(getCheck(hGeometryPage,
			IDC_REMOVE_HIDDEN_LINES));
		ldPrefs->setWireframeThickness(CUIDialog::trackBarGetPos(hGeometryPage,
			IDC_WIREFRAME_THICKNESS));
		ldPrefs->setBfc(getCachedCheck(hGeometryPage, IDC_BFC));
		ldPrefs->setRedBackFaces(getCheck(hGeometryPage, IDC_RED_BACK_FACES));
		ldPrefs->setGreenFrontFaces(getCheck(hGeometryPage,
			IDC_GREEN_FRONT_FACES));
		ldPrefs->setBlueNeutralFaces(getCheck(hGeometryPage,
			IDC_BLUE_NEUTRAL_FACES));
		ldPrefs->setShowHighlightLines(getCachedCheck(hGeometryPage,
			IDC_HIGHLIGHTS));
		if (ldPrefs->getShowHighlightLines())
		{
			ldPrefs->setEdgesOnly(getCheck(hGeometryPage, IDC_EDGES_ONLY));
			ldPrefs->setDrawConditionalHighlights(getCheck(hGeometryPage,
				IDC_CONDITIONAL_HIGHLIGHTS));
			if (ldPrefs->getDrawConditionalHighlights())
			{
				ldPrefs->setShowAllConditionalLines(getCheck(hGeometryPage,
					IDC_ALL_CONDITIONAL));
				ldPrefs->setShowConditionalControlPoints(getCheck(hGeometryPage,
					IDC_CONDITIONAL_CONTROLS));
			}
			ldPrefs->setUsePolygonOffset(getCheck(hGeometryPage,
				IDC_QUALITY_LINES));
			ldPrefs->setBlackHighlights(getCheck(hGeometryPage,
				IDC_ALWAYS_BLACK));
		}
		ldPrefs->setEdgeThickness(CUIDialog::trackBarGetPos(hGeometryPage,
			IDC_EDGE_THICKNESS));
		ldPrefs->applyGeometrySettings();
	}
	ldPrefs->commitGeometrySettings();
}

LDPreferences::LightDirection LDViewPreferences::getSelectedLightDirection(void)
{
	LDPreferences::LightDirection lightDirection =
		LDPreferences::CustomDirection;
	for (IntIntMap::const_iterator it = lightDirIndexToId.begin()
		; it != lightDirIndexToId.end(); it++)
	{
		if (getCachedCheck(hEffectsPage, it->second))
		{
			lightDirection =
				(LDPreferences::LightDirection)(it->first + 1);
			break;
		}
	}
	return lightDirection;
}

void LDViewPreferences::applyEffectsChanges(void)
{
	if (hEffectsPage)
	{
		ldPrefs->setUseLighting(getCachedCheck(hEffectsPage, IDC_LIGHTING));
		if (ldPrefs->getUseLighting())
		{
			LDPreferences::LightDirection lightDirection =
				getSelectedLightDirection();
			ldPrefs->setQualityLighting(getCheck(hEffectsPage,
				IDC_LIGHTING_QUALITY));
			ldPrefs->setSubduedLighting(getCheck(hEffectsPage,
				IDC_LIGHTING_SUBDUED));
			ldPrefs->setUseSpecular(getCheck(hEffectsPage, IDC_SPECULAR));
			ldPrefs->setOneLight(getCheck(hEffectsPage,
				IDC_ALTERNATE_LIGHTING));
			if (lightDirection != LDPreferences::CustomDirection)
			{
				ldPrefs->setLightDirection(lightDirection);
			}
			if (getCheck(hEffectsPage, IDC_DRAW_LIGHT_DATS))
			{
				ldPrefs->setDrawLightDats(true);
				ldPrefs->setOptionalStandardLight(getCheck(hEffectsPage,
					IDC_OPTIONAL_STANDARD_LIGHT));
			}
			else
			{
				ldPrefs->setDrawLightDats(false);
			}
		}
		// NOTE: the following setting doesn't require lighting to be enabled.
		ldPrefs->setNoLightGeom(getCheck(hEffectsPage, IDC_HIDE_LIGHT_DAT));
		ldPrefs->setStereoEyeSpacing(CUIDialog::trackBarGetPos(hEffectsPage,
			IDC_STEREO_SPACING));
		ldPrefs->setCutawayAlpha(CUIDialog::trackBarGetPos(hEffectsPage,
			IDC_CUTAWAY_OPACITY));
		ldPrefs->setCutawayThickness(CUIDialog::trackBarGetPos(hEffectsPage,
			IDC_CUTAWAY_THICKNESS));
		ldPrefs->setUseStipple(getCheck(hEffectsPage, IDC_STIPPLE));
		ldPrefs->setSortTransparent(getCheck(hEffectsPage, IDC_SORT));
		ldPrefs->setUseFlatShading(getCheck(hEffectsPage, IDC_FLAT_SHADING));
		ldPrefs->setPerformSmoothing(getCheck(hEffectsPage, IDC_SMOOTH_CURVES));
		ldPrefs->applyEffectsSettings();
	}
	ldPrefs->commitEffectsSettings();
}

void LDViewPreferences::applyPrimitivesChanges(void)
{
	if (hPrimitivesPage)
	{
		ldPrefs->setAllowPrimitiveSubstitution(getCachedCheck(hPrimitivesPage,
			IDC_PRIMITIVE_SUBSTITUTION));
		if (ldPrefs->getAllowPrimitiveSubstitution())
		{
			ldPrefs->setTextureStuds(getCheck(hPrimitivesPage,
				IDC_TEXTURE_STUDS));
			ldPrefs->setCurveQuality(CUIDialog::trackBarGetPos(hPrimitivesPage,
				IDC_CURVE_QUALITY));
		}
		ldPrefs->setTexmaps(getCheck(hPrimitivesPage, IDC_TEXMAPS));
		if (getTexmaps())
		{
			ldPrefs->setTexturesAfterTransparent(getCheck(hPrimitivesPage,
				IDC_TRANSPARENT_TEXTURES_LAST));
			ldPrefs->setTextureOffsetFactor(textureOffsetFromSliderValue(
				trackBarGetPos(hTextureOffsetSlider)));
		}
		ldPrefs->setQualityStuds(!getCheck(hPrimitivesPage, IDC_STUD_QUALITY));
		ldPrefs->setHiResPrimitives(getCheck(hPrimitivesPage, IDC_HI_RES));
		ldPrefs->applyPrimitivesSettings();
	}
	ldPrefs->commitPrimitivesSettings();
}

void LDViewPreferences::applyUpdatesChanges(void)
{
	if (hUpdatesPage)
	{
		ucstring tempString;
		int tempNum;

		ldPrefs->setCheckPartTracker(getCheck(hUpdatesPage,
			IDC_CHECK_PART_TRACKER));
		if (ldPrefs->getCheckPartTracker())
		{
			UCCHAR buf[128];

			if (windowGetValue(hMissingParts, tempNum))
			{
				if (tempNum > 0)
				{
					ldPrefs->setMissingPartWait(tempNum);
				}
			}
			sucprintf(buf, COUNT_OF(buf), _UC("%0d"), ldPrefs->getMissingPartWait());
			windowSetText(hMissingParts, buf);
			if (windowGetValue(hUpdatedParts, tempNum))
			{
				if (tempNum > 0)
				{
					ldPrefs->setUpdatedPartWait(tempNum);
				}
			}
			sucprintf(buf, COUNT_OF(buf), _UC("%0d"), ldPrefs->getUpdatedPartWait());
			windowSetText(hUpdatedParts, buf);
		}
		windowGetText(hProxyServer, tempString);
		if (!tempString.empty())
		{
			std::string utf8Temp;
			ucstringtoutf8(utf8Temp, tempString);
			ldPrefs->setProxyServer(utf8Temp.c_str());
		}
		if (windowGetValue(hProxyPort, tempNum))
		{
			ldPrefs->setProxyPort(tempNum);
		}
		ldPrefs->applyUpdatesSettings();
	}
	ldPrefs->commitUpdatesSettings();
}

void LDViewPreferences::applyChanges(void)
{
	applyGeneralChanges();
	applyGeometryChanges();
	applyEffectsChanges();
	applyPrimitivesChanges();
	applyUpdatesChanges();
	applyPrefSetsChanges();	// Note that if there are any pref sets changes,
							// there can't be any other changes.
}

void LDViewPreferences::saveDefaultView(void)
{
	ldPrefs->saveDefaultView();
}

void LDViewPreferences::resetDefaultView(void)
{
	ldPrefs->resetDefaultView();
}

void LDViewPreferences::getRGB(int color, int &r, int &g, int &b)
{
	r = color & 0xFF;
	g = (color >> 8) & 0xFF;
	b = (color >> 16) & 0xFF;
}

void LDViewPreferences::chooseBackgroundColor(void)
{
	COLORREF backgroundColor = getBackgroundColor();
	int r, g, b;

	chooseColor(hBackgroundColorButton, hBackgroundColorBitmap,
		backgroundColor);
	r = GetRValue(backgroundColor);
	g = GetGValue(backgroundColor);
	b = GetBValue(backgroundColor);
	ldPrefs->setBackgroundColor(r, g, b);
}

void LDViewPreferences::chooseDefaultColor(void)
{
	COLORREF defaultColor = getDefaultColor();
	int r, g, b;

	chooseColor(hDefaultColorButton, hDefaultColorBitmap, defaultColor);
	r = GetRValue(defaultColor);
	g = GetGValue(defaultColor);
	b = GetBValue(defaultColor);
	ldPrefs->setDefaultColor(r, g, b);
}

void LDViewPreferences::chooseColor(HWND hColorButton, HBITMAP hColorBitmap,
									COLORREF &color)
{
	CHOOSECOLOR chooseColor;
	int i;
	COLORREF customColors[16];
	int r, g, b;

	for (i = 0; i < 16; i++)
	{
		ldPrefs->getCustomColor(i, r, g, b);
		customColors[i] = RGB(r, g, b);
	}
	memset(&chooseColor, 0, sizeof CHOOSECOLOR);
	chooseColor.lStructSize = sizeof CHOOSECOLOR;
	chooseColor.hwndOwner = hPropSheet;
	chooseColor.rgbResult = color;
	chooseColor.lpCustColors = customColors;
	chooseColor.Flags = CC_ANYCOLOR | CC_RGBINIT;
	EnableWindow(hPropSheet, FALSE);
	if (ChooseColor(&chooseColor))
	{
		color = chooseColor.rgbResult;
		redrawColorBitmap(hColorButton, hColorBitmap, color);
	}
	for (i = 0; i < 16; i++)
	{
		r = GetRValue(customColors[i]);
		g = GetGValue(customColors[i]);
		b = GetBValue(customColors[i]);
		ldPrefs->setCustomColor(i, r, g, b);
	}
	EnableWindow(hPropSheet, TRUE);
}

void LDViewPreferences::browseForDir(
	CUCSTR prompt,
	HWND hTextField,
	ucstring &dir)
{
	ucstring newDir = LDViewWindow::browseForDir(prompt, dir.c_str());

	if (!newDir.empty())
	{
		dir = newDir;
		SetWindowText(hTextField, dir.c_str());
		enableApply(GetParent(hTextField));
	}
}

void LDViewPreferences::doGeneralClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
		case IDC_FS_REFRESH:
			doFSRefresh();
			break;
		case IDC_BACKGROUND_COLOR:
			chooseBackgroundColor();
			break;
		case IDC_DEFAULT_COLOR:
			chooseDefaultColor();
			break;
		case IDC_GENERAL_RESET:
			ldPrefs->loadDefaultGeneralSettings(false);
			setupGeneralPage();
			break;
		case IDC_BROWSE_SNAPSHOTS_DIR:
			browseForDir(ls(_UC("BrowseForSnapshotDir")), hSnapshotDirField,
				snapshotDir);
			break;
		case IDC_BROWSE_PARTS_LIST_DIR:
			browseForDir(ls(_UC("BrowseForPartsListDir")), hPartsListDirField,
				partsListDir);
			break;
		case IDC_BROWSE_EXPORT_DIR:
			browseForDir(ls(_UC("BrowseForExportListDir")), hExportDirField,
				exportDir);
			break;
	}
	enableApply(hGeneralPage);
}

void LDViewPreferences::doGeometryClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
		case IDC_HIGHLIGHTS:
			doHighlights();
			break;
		case IDC_CONDITIONAL_HIGHLIGHTS:
			doConditionals();
			break;
		case IDC_WIREFRAME:
			doWireframe();
			break;
		case IDC_BFC:
			doBfc();
			break;
		case IDC_SEAMS:
			doSeams();
			break;
		case IDC_GEOMETRY_RESET:
			ldPrefs->loadDefaultGeometrySettings(false);
			setupGeometryPage();
			break;
	}
	enableApply(hGeometryPage);
}

void LDViewPreferences::doEffectsClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
		case IDC_LIGHTING:
			doLighting();
			break;
		case IDC_DRAW_LIGHT_DATS:
			doDrawLightDats();
			break;
		case IDC_STIPPLE:
			doStipple();
			break;
		case IDC_SORT:
			doSort();
			break;
		case IDC_FLAT_SHADING:
			doFlatShading();
			break;
		case IDC_SMOOTH_CURVES:
			doSmoothCurves();
			break;
		case IDC_STEREO:
			doStereo();
			break;
		case IDC_HARDWARE_STEREO:
			ldPrefs->setStereoMode(LDVStereoHardware);
			break;
		case IDC_CROSS_EYED_STEREO:
			ldPrefs->setStereoMode(LDVStereoCrossEyed);
			break;
		case IDC_PARALLEL_STEREO:
			ldPrefs->setStereoMode(LDVStereoParallel);
			break;
		case IDC_CUTAWAY:
			doCutaway();
			break;
		case IDC_CUTAWAY_COLOR:
			ldPrefs->setCutawayMode(LDVCutawayWireframe);
			setupOpacitySlider();
			break;
		case IDC_CUTAWAY_MONOCHROME:
			ldPrefs->setCutawayMode(LDVCutawayStencil);
			setupOpacitySlider();
			break;
		case IDC_EFFECTS_RESET:
			ldPrefs->loadDefaultEffectsSettings(false);
			setupEffectsPage();
			break;
	}
	if (lightDirIdToIndex.find(controlId) != lightDirIdToIndex.end())
	{
		if (!getCachedCheck(hEffectsPage, controlId))
		{
			for (IntIntMap::const_iterator it = lightDirIndexToId.begin()
				; it != lightDirIndexToId.end(); it++)
			{
				if (getCachedCheck(hEffectsPage, it->second))
				{
					getCachedCheck(hEffectsPage, it->second, true);
					InvalidateRect(lightAngleButtons[it->first], NULL, TRUE);
				}
			}
			getCachedCheck(hEffectsPage, controlId, true);
		}
	}
	enableApply(hEffectsPage);
}

void LDViewPreferences::doDeletePrefSet(void)
{
	ucstring selectedPrefSet = getSelectedPrefSet();

	if (!selectedPrefSet.empty())
	{
		int selectedIndex = listBoxFindStringExact(hPrefSetsList, selectedPrefSet);

		if (checkAbandon && getApplyEnabled())
		{
			if (messageBoxUC(hWindow,
				ls(_UC("PrefSetAbandonConfirm")),
				ls(_UC("AbandonChanges")),
				MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				abandonChanges();
			}
			else
			{
				return;
			}
		}
		checkAbandon = false;
		listBoxDeleteString(hPrefSetsList, selectedIndex);
		while (selectedIndex >= listBoxGetCount(hPrefSetsList) &&
			selectedIndex > 0)
		{
			--selectedIndex;
		}
		selectedPrefSet = getPrefSet(selectedIndex);
		selectPrefSet(selectedPrefSet, true);
	}
}

void LDViewPreferences::doNewPrefSet(void)
{
	newPrefSetName.clear();
	if (DialogBoxParam(getLanguageModule(), MAKEINTRESOURCE(IDD_NEW_PREF_SET),
		hPropSheet, staticDialogProc, (LPARAM)this) == IDOK)
	{
		if (!newPrefSetName.empty())
		{
			listBoxAddString(hPrefSetsList, newPrefSetName);
			selectPrefSet(newPrefSetName);
		}
	}
}

ucstring LDViewPreferences::getHotKey(int index)
{
	char key[128];

	sprintf(key, "%s/Key%d", HOT_KEYS_KEY, index);
	UCSTR hotKeyTemp = TCUserDefaults::stringForKeyUC(key, NULL, false);
	ucstring hotKey = hotKeyTemp;
	delete[] hotKeyTemp;
	return hotKey;
}

int LDViewPreferences::getHotKey(const ucstring& currentPrefSetName)
{
	int i;
	int retValue = -1;

	for (i = 0; i < 10 && retValue == -1; i++)
	{
		ucstring prefSetName = getHotKey(i);

		if (!prefSetName.empty())
		{
			if (currentPrefSetName == prefSetName)
			{
				retValue = i;
			}
		}
	}
	return retValue;
}

int LDViewPreferences::getCurrentHotKey(void)
{
	ucstring currentPrefSetName = getSelectedPrefSet();
	int retValue = -1;

	if (!currentPrefSetName.empty())
	{
		retValue = getHotKey(currentPrefSetName);
	}
	return retValue;
}

bool LDViewPreferences::performHotKey(int lhotKeyIndex)
{
	ucstring hotKeyPrefSetName = getHotKey(lhotKeyIndex);
	bool retValue = false;

	if (!hotKeyPrefSetName.empty() && !hPropSheet)
	{
		const char *currentSessionName = TCUserDefaults::getSessionName();
		bool hotKeyIsDefault = hotKeyPrefSetName == DEFAULT_PREF_SET;

		if (currentSessionName)
		{
			ucstring ucCurrentSessionName;
			utf8toucstring(ucCurrentSessionName, currentSessionName);
			if (ucCurrentSessionName == hotKeyPrefSetName)
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

				std::string utf8Name;
				ucstringtoutf8(utf8Name, hotKeyPrefSetName);
				if (sessionNames->indexOfString(utf8Name.c_str()) != -1)
				{
					TCUserDefaults::setSessionName(utf8Name.c_str(),
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
				applySettings();
				retValue = true;
			}
		}
	}
	return retValue;
}

void LDViewPreferences::saveCurrentHotKey(void)
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
		ucstring currentSessionName = getSelectedPrefSet();

		sprintf(key, "%s/Key%d", HOT_KEYS_KEY, hotKeyIndex % 10);
		TCUserDefaults::setStringForKey(currentSessionName.c_str(), key, false);
	}
}

void LDViewPreferences::doPrefSetHotKey(void)
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
	if (DialogBoxParam(getLanguageModule(), MAKEINTRESOURCE(IDD_HOTKEY),
		hPropSheet, staticDialogProc, (LPARAM)this) == IDOK)
	{
		if (hotKeyIndex != CB_ERR)
		{
			saveCurrentHotKey();
		}
	}
}

void LDViewPreferences::doPrefSetsClick(int controlId, HWND /*controlHWnd*/)
{
	switch (controlId)
	{
	case IDC_DELETE_PREF_SET:
		doDeletePrefSet();
		break;
	case IDC_NEW_PREF_SET:
		doNewPrefSet();
		break;
	case IDC_PREF_SET_HOT_KEY:
		doPrefSetHotKey();
		break;
	}
	enableApply(hPrefSetsPage);
}

void LDViewPreferences::doOtherClick(HWND hDlg, int controlId,
									 HWND /*controlHWnd*/)
{
	if (controlId == IDC_NEW_PREF_SET_CANCEL)
	{
		EndDialog(hDlg, IDCANCEL);
	}
	else if (controlId == IDC_NEW_PREF_SET_OK)
	{
		ucstring editText;

		CUIDialog::windowGetText(hDlg, IDC_NEW_PREF_SET_FIELD, editText);
		if (!editText.empty())
		{
			int index = listBoxFindStringExact(hPrefSetsList, editText);

			if (index == LB_ERR)
			{
				if (editText.find('/') < editText.size() || editText.find('\\') < editText.size())
				{
					messageBoxUC(hDlg,
						ls(_UC("PrefSetNameBadChars")),
						ls(_UC("InvalidName")),
						MB_OK | MB_ICONWARNING);
				}
				else
				{
					newPrefSetName = editText;
					EndDialog(hDlg, IDOK);
				}
			}
			else
			{
				messageBoxUC(hDlg,
					ls(_UC("PrefSetAlreadyExists")),
					ls(_UC("DuplicateName")),
					MB_OK | MB_ICONWARNING);
			}
		}
		else
		{
			messageBoxUC(hDlg,
				ls(_UC("PrefSetNameRequired")),
				ls(_UC("EmptyName")), MB_OK | MB_ICONWARNING);
		}
	}
	else if (controlId == IDC_HOTKEY_OK)
	{
		hotKeyIndex = CUIDialog::comboGetCurSel(hDlg, IDC_HOTKEY_COMBO);
		EndDialog(hDlg, IDOK);
	}
	else if (controlId == IDC_APPLY || controlId == IDC_ABANDON ||
		controlId == IDCANCEL)
	{
		EndDialog(hDlg, controlId);
	}
}

void LDViewPreferences::setAniso(int value)
{
	UCCHAR label[128];
	float level = anisoFromSliderValue(value);
	int intLevel = (int)(level + 0.5);

	ldPrefs->setAnisoLevel(level);
	if (intLevel >= 2)
	{
		sucprintf(label, COUNT_OF(label), ls(_UC("AnisoNx")), intLevel);
	}
	else
	{
		label[0] = 0;
	}
	windowSetText(hAnisoLevelLabel, label);
}

void LDViewPreferences::doPrimitivesClick(int controlId, HWND /*controlHWnd*/)
{
	bool bDisableAniso = false;

	switch (controlId)
	{
		case IDC_PRIMITIVE_SUBSTITUTION:
			doPrimitives();
			break;
		case IDC_TEXTURE_STUDS:
			doTextureStuds();
			break;
		case IDC_TEXMAPS:
			doTexmaps();
			break;
		case IDC_TEXTURE_NEAREST:
			ldPrefs->setTextureFilterType(GL_NEAREST_MIPMAP_NEAREST);
			bDisableAniso = true;
			break;
		case IDC_TEXTURE_BILINEAR:
			ldPrefs->setTextureFilterType(GL_LINEAR_MIPMAP_NEAREST);
			bDisableAniso = true;
			break;
		case IDC_TEXTURE_TRILINEAR:
			ldPrefs->setTextureFilterType(GL_LINEAR_MIPMAP_LINEAR);
			bDisableAniso = true;
			break;
		case IDC_TEXTURE_ANISO:
			{
				ldPrefs->setTextureFilterType(GL_LINEAR_MIPMAP_LINEAR);
				setAniso(trackBarGetPos(hAnisoLevelSlider));
				EnableWindow(hAnisoLevelSlider, TRUE);
			}
			break;
		case IDC_PRIMITIVES_RESET:
			ldPrefs->loadDefaultPrimitivesSettings(false);
			setupPrimitivesPage();
			break;
	}
	if (bDisableAniso)
	{
		setAniso(0);
		EnableWindow(hAnisoLevelSlider, FALSE);
	}
	enableApply(hPrimitivesPage);
}

void LDViewPreferences::doCheckPartTracker(void)
{
	if (getCheck(hUpdatesPage, IDC_CHECK_PART_TRACKER))
	{
		enableCheckPartTracker();
	}
	else
	{
		disableCheckPartTracker();
	}
}

void LDViewPreferences::doUpdatesClick(int controlId, HWND /*controlHWnd*/)
{
	ucstring tempString;
	int tempNum;

	windowGetText(hProxyServer, tempString);
	if (!tempString.empty())
	{
		std::string utf8Temp;
		ucstringtoutf8(utf8Temp, tempString);
		ldPrefs->setProxyServer(utf8Temp.c_str());
	}
	if (windowGetValue(hProxyPort, tempNum))
	{
		ldPrefs->setProxyPort(tempNum);
	}
	switch (controlId)
	{
	case IDC_PROXY_NONE:
		ldPrefs->setProxyType(0);
		disableProxyServer();
		break;
	case IDC_PROXY_WINDOWS:
		ldPrefs->setProxyType(1);
		disableProxyServer();
		break;
	case IDC_PROXY_MANUAL:
		ldPrefs->setProxyType(2);
		enableProxyServer();
		break;
	case IDC_UPDATES_RESET:
		ldPrefs->loadDefaultUpdatesSettings(false);
		setupUpdatesPage();
		break;
	case IDC_CHECK_PART_TRACKER:
		doCheckPartTracker();
		break;
	}
	enableApply(hUpdatesPage);
}

DWORD LDViewPreferences::doComboSelChange(HWND hPage, int controlId,
										  HWND /*controlHWnd*/)
{
	ucstring selectedString;
	switch (controlId)
	{
	case IDC_FSAA_COMBO:
		int fsaaMode;

		CUIDialog::windowGetText(hPage, controlId, selectedString);
		if (selectedString == ls(_UC("FsaaNone")))
		{
			fsaaMode = 0;
		}
		else
		{
			sucscanf(selectedString.c_str(), _UC("%d"), &fsaaMode);
			if (fsaaMode > 4)
			{
				fsaaMode = fsaaMode << 3;
			}
			else if (selectedString.find(ls(_UC("FsaaEnhanced"))) <
				selectedString.size())
			{
				fsaaMode |= 1;
			}
		}
		ldPrefs->setFsaaMode(fsaaMode);
		enableApply(hPage);
		break;
	case IDC_MEMORY_COMBO:
		enableApply(hPage);
		break;
	case IDC_SNAPSHOTS_DIR_COMBO:
		snapshotDirMode = (LDPreferences::DefaultDirMode)comboGetCurSel(
			hSnapshotDirCombo);
		updateSaveDir(hSnapshotDirField, hSnapshotBrowseButton, snapshotDirMode,
			snapshotDir);
		enableApply(hPage);
		break;
	case IDC_PARTS_LIST_DIR_COMBO:
		partsListDirMode = (LDPreferences::DefaultDirMode)comboGetCurSel(
			hPartsListDirCombo);
		updateSaveDir(hPartsListDirField, hPartsListBrowseButton,
			partsListDirMode, partsListDir);
		enableApply(hPage);
		break;
	case IDC_EXPORT_DIR_COMBO:
		exportDirMode = (LDPreferences::DefaultDirMode)comboGetCurSel(
			hExportDirCombo);
		updateSaveDir(hExportDirField, hExportBrowseButton, exportDirMode,
			exportDir);
		enableApply(hPage);
		break;
	}
	return 0;
}

DWORD LDViewPreferences::doClick(HWND hPage, int controlId, HWND controlHWnd)
{
	bool wasPaused = true;

	if (modelViewer)
	{
		wasPaused = modelViewer->getPaused() != 0;
		if (!wasPaused)
		{
			modelViewer->pause();
		}
	}
	if (hPage == hGeneralPage)
	{
		doGeneralClick(controlId, controlHWnd);
	}
	else if (hPage == hGeometryPage)
	{
		doGeometryClick(controlId, controlHWnd);
	}
	else if (hPage == hEffectsPage)
	{
		doEffectsClick(controlId, controlHWnd);
	}
	else if (hPage == hPrimitivesPage)
	{
		doPrimitivesClick(controlId, controlHWnd);
	}
	else if (hPage == hUpdatesPage)
	{
		doUpdatesClick(controlId, controlHWnd);
	}
	else if (hPage == hPrefSetsPage)
	{
		doPrefSetsClick(controlId, controlHWnd);
	}
	else
	{
		doOtherClick(hPage, controlId, controlHWnd);
	}
	if (!wasPaused)
	{
		modelViewer->unpause();
	}
	return 0;
}

void LDViewPreferences::doFSRefresh(void)
{
	if (getCheck(hGeneralPage, IDC_FS_REFRESH))
	{
		EnableWindow(hFullScreenRateField, TRUE);
	}
	else
	{
		CUIDialog::windowSetText(hGeneralPage, IDC_FS_RATE, _UC(""));
		EnableWindow(hFullScreenRateField, FALSE);
	}
}

void LDViewPreferences::doStipple(void)
{
	if (getCheck(hEffectsPage, IDC_STIPPLE))
	{
		setCheck(hEffectsPage, IDC_SORT, false);
	}
}

void LDViewPreferences::doSort(void)
{
	if (getCheck(hEffectsPage, IDC_SORT))
	{
		setCheck(hEffectsPage, IDC_STIPPLE, false);
	}
}

void LDViewPreferences::doFlatShading(void)
{
	if (getCheck(hEffectsPage, IDC_FLAT_SHADING))
	{
		setCheck(hEffectsPage, IDC_SMOOTH_CURVES, false);
	}
}

void LDViewPreferences::doSmoothCurves(void)
{
	if (getCheck(hEffectsPage, IDC_SMOOTH_CURVES))
	{
		setCheck(hEffectsPage, IDC_FLAT_SHADING, false);
	}
}

void LDViewPreferences::doHighlights(void)
{
	if (getCachedCheck(hGeometryPage, IDC_HIGHLIGHTS, true))
	{
		enableEdges();
	}
	else
	{
		disableEdges();
	}
}

void LDViewPreferences::doConditionals(void)
{
	if (getCheck(hGeometryPage, IDC_CONDITIONAL_HIGHLIGHTS))
	{
		enableConditionals();
	}
	else
	{
		disableConditionals();
	}
}

void LDViewPreferences::setCheck(HWND hPage, int buttonId, bool value)
{
	CUIDialog::buttonSetChecked(hPage, buttonId, value);
}

bool LDViewPreferences::getCheck(HWND hPage, int buttonId)
{
	return CUIDialog::buttonIsChecked(hPage, buttonId);
}

bool LDViewPreferences::getCachedCheck(HWND hPage, int buttonId, bool action)
{
	if (hButtonTheme)
	{
		HWND hButton = GetDlgItem(hPage, buttonId);

		if (action)
		{
			checkStates[hButton] = !checkStates[hButton];
		}
		return checkStates[hButton];
	}
	else
	{
		return getCheck(hPage, buttonId);
	}
}

void LDViewPreferences::doWireframe(void)
{
	if (getCachedCheck(hGeometryPage, IDC_WIREFRAME, true))
	{
		enableWireframe();
	}
	else
	{
		disableWireframe();
	}
}

void LDViewPreferences::doBfc(void)
{
	if (getCachedCheck(hGeometryPage, IDC_BFC, true))
	{
		enableBfc();
	}
	else
	{
		disableBfc();
	}
}

void LDViewPreferences::doLighting(void)
{
	if (getCachedCheck(hEffectsPage, IDC_LIGHTING, true))
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
}

void LDViewPreferences::doDrawLightDats(void)
{
	BOOL enabled = FALSE;
	BOOL checked = FALSE;

	if (getCachedCheck(hEffectsPage, IDC_DRAW_LIGHT_DATS, true))
	{
		enabled = TRUE;
		checked = ldPrefs->getOptionalStandardLight() ? TRUE : FALSE;
	}
	EnableWindow(hLightOptionalStandardButton, enabled);
	buttonSetCheck(hLightOptionalStandardButton, checked);
}

void LDViewPreferences::doStereo(void)
{
	if (getCachedCheck(hEffectsPage, IDC_STEREO, true))
	{
		ldPrefs->setStereoMode(LDVStereoCrossEyed);
		enableStereo();
	}
	else
	{
		ldPrefs->setStereoMode(LDVStereoNone);
		disableStereo();
	}
}

void LDViewPreferences::doCutaway(void)
{
	if (getCachedCheck(hEffectsPage, IDC_CUTAWAY, true))
	{
		ldPrefs->setCutawayMode(LDVCutawayWireframe);
		enableCutaway();
	}
	else
	{
		ldPrefs->setCutawayMode(LDVCutawayNormal);
		disableCutaway();
	}
}

void LDViewPreferences::doSeams(void)
{
	if (getCheck(hGeometryPage, IDC_SEAMS))
	{
		enableSeams();
	}
	else
	{
		disableSeams();
	}
}

void LDViewPreferences::doPrimitives(void)
{
	if (getCachedCheck(hPrimitivesPage, IDC_PRIMITIVE_SUBSTITUTION, true))
	{
		enablePrimitives();
	}
	else
	{
		disablePrimitives();
	}
}

void LDViewPreferences::updateTextureFilteringEnabled(void)
{
	if (getCheck(hPrimitivesPage, IDC_TEXMAPS) ||
		(getCachedCheck(hPrimitivesPage, IDC_PRIMITIVE_SUBSTITUTION, false) &&
		getCheck(hPrimitivesPage, IDC_TEXTURE_STUDS)))
	{
		enableTextureFiltering();
	}
	else
	{
		disableTextureFiltering();
	}
}

void LDViewPreferences::updateTexmapsEnabled(void)
{
	if (getCheck(hPrimitivesPage, IDC_TEXMAPS))
	{
		enableTexmaps();
	}
	else
	{
		disableTexmaps();
	}
}

void LDViewPreferences::doTextureStuds(void)
{
	updateTextureFilteringEnabled();
}

void LDViewPreferences::doTexmaps(void)
{
	updateTextureFilteringEnabled();
	updateTexmapsEnabled();
}

void LDViewPreferences::doReset(void)
{
	loadSettings();
}

BOOL LDViewPreferences::doDialogVScroll(HWND hDlg, int scrollCode, int position,
										HWND hScrollBar)
{
	if (scrollCode == SB_THUMBPOSITION && hScrollBar == hSeamSpin)
	{
		UCCHAR seamWidthString[128];
		int seamWidth = ldPrefs->getSeamWidth();

		seamWidth += (position - seamWidth);
		ldPrefs->setSeamWidth(seamWidth);
		sucprintf(seamWidthString, COUNT_OF(seamWidthString), _UC("%0.2f"),
			seamWidth / 100.0f);
		CUIDialog::windowSetText(hDlg, IDC_SEAM_WIDTH_FIELD, seamWidthString);
		SendDlgItemMessage(hDlg, IDC_SEAM_SPIN, UDM_SETPOS, 0, seamWidth);
		setCheck(hGeometryPage, IDC_SEAMS, seamWidth != 0);
		enableApply(hGeometryPage);
	}
	return TRUE;
}

void LDViewPreferences::setupPage(int pageNumber)
{
	if (pageNumber == generalPageNumber)
	{
		setupGeneralPage();
	}
	else if (pageNumber == geometryPageNumber)
	{
		setupGeometryPage();
	}
	else if (pageNumber == effectsPageNumber)
	{
		setupEffectsPage();
	}
	else if (pageNumber == primitivesPageNumber)
	{
		setupPrimitivesPage();
	}
	else if (pageNumber == updatesPageNumber)
	{
		setupUpdatesPage();
	}
	else if (pageNumber == prefSetsPageNumber)
	{
		setupPrefSetsPage();
	}
	disableApply(hwndArray->pointerAtIndex(pageNumber));
}

void LDViewPreferences::setupFov(bool warn)
{
	UCCHAR buf[1024];
	TCFloat minFov = getMinFov();
	TCFloat maxFov = getMaxFov();

	SendDlgItemMessage(hGeneralPage, IDC_FOV, EM_SETLIMITTEXT, 5, 0);
	sucprintf(buf, COUNT_OF(buf), _UC("%.4g"), ldPrefs->getFov());
	CUIDialog::windowSetText(hGeneralPage, IDC_FOV, buf);
	sucprintf(buf, COUNT_OF(buf), _UC("(%g - %g)"), minFov, maxFov);
	CUIDialog::windowSetText(hGeneralPage, IDC_FOV_RANGE_LABEL, buf);
	if (warn)
	{
		sucprintf(buf, COUNT_OF(buf), ls(_UC("FovRangeError")), minFov, maxFov);
		messageBoxUC(hPropSheet, buf, ls(_UC("InvalidValue")),
			MB_OK | MB_ICONWARNING);
	}
}

void LDViewPreferences::setupMemoryUsage(void)
{
	CUIDialog::comboResetContent(hGeneralPage, IDC_MEMORY_COMBO);
	CUIDialog::comboAddString(hGeneralPage, IDC_MEMORY_COMBO, ls(_UC("Low")));
	CUIDialog::comboAddString(hGeneralPage, IDC_MEMORY_COMBO, ls(_UC("Medium")));
	CUIDialog::comboAddString(hGeneralPage, IDC_MEMORY_COMBO, ls(_UC("High")));
	CUIDialog::comboSetCurSel(hGeneralPage, IDC_MEMORY_COMBO, ldPrefs->getMemoryUsage());
}

void LDViewPreferences::updateSaveDir(
	HWND hTextField,
	HWND hBrowseButton,
	LDPreferences::DefaultDirMode dirMode,
	const ucstring &filename)
{
	BOOL enable = FALSE;

	if (dirMode == LDPreferences::DDMSpecificDir)
	{
		SetWindowText(hTextField, filename.c_str());
		enable = TRUE;
	}
	else
	{
		SetWindowText(hTextField, _UC(""));
	}
	EnableWindow(hTextField, enable);
	EnableWindow(hBrowseButton, enable);
}

void LDViewPreferences::setupSaveDir(
	HWND hComboBox,
	HWND hTextField,
	HWND hBrowseButton,
	LDPreferences::DefaultDirMode dirMode,
	const ucstring &filename,
	CUCSTR nameKey)
{
	ucstring lastSaved = ls(_UC("LastSaved"));
	
	lastSaved += ls(nameKey);
	comboResetContent(hComboBox);
	comboAddString(hComboBox, ls(_UC("Model")));
	comboAddString(hComboBox, lastSaved.c_str());
	comboAddString(hComboBox, ls(_UC("Specified")));
	comboSetCurSel(hComboBox, dirMode);
	updateSaveDir(hTextField, hBrowseButton, dirMode, filename);
}

void LDViewPreferences::setupSaveDirs(void)
{
	hSnapshotDirCombo = GetDlgItem(hGeneralPage, IDC_SNAPSHOTS_DIR_COMBO);
	hSnapshotDirField = GetDlgItem(hGeneralPage, IDC_SNAPSHOTS_DIR);
	hSnapshotBrowseButton = GetDlgItem(hGeneralPage, IDC_BROWSE_SNAPSHOTS_DIR);
	snapshotDirMode = ldPrefs->getSaveDirMode(LDPreferences::SOSnapshot);
	utf8toucstring(snapshotDir,
		ldPrefs->getSaveDir(LDPreferences::SOSnapshot));
	setupSaveDir(hSnapshotDirCombo, hSnapshotDirField, hSnapshotBrowseButton,
		snapshotDirMode, snapshotDir, _UC("Snapshot"));

	hPartsListDirCombo = GetDlgItem(hGeneralPage, IDC_PARTS_LIST_DIR_COMBO);
	hPartsListDirField = GetDlgItem(hGeneralPage, IDC_PARTS_LIST_DIR);
	hPartsListBrowseButton = GetDlgItem(hGeneralPage,
		IDC_BROWSE_PARTS_LIST_DIR);
	partsListDirMode = ldPrefs->getSaveDirMode(LDPreferences::SOPartsList);
	utf8toucstring(partsListDir,
		ldPrefs->getSaveDir(LDPreferences::SOPartsList));
	setupSaveDir(hPartsListDirCombo, hPartsListDirField, hPartsListBrowseButton,
		partsListDirMode, partsListDir, _UC("PartsList"));

	hExportDirCombo = GetDlgItem(hGeneralPage, IDC_EXPORT_DIR_COMBO);
	hExportDirField = GetDlgItem(hGeneralPage, IDC_EXPORT_DIR);
	hExportBrowseButton = GetDlgItem(hGeneralPage, IDC_BROWSE_EXPORT_DIR);
	exportDirMode = ldPrefs->getSaveDirMode(LDPreferences::SOExport);
	utf8toucstring(exportDir,
		ldPrefs->getSaveDir(LDPreferences::SOExport));
	setupSaveDir(hExportDirCombo, hExportDirField, hExportBrowseButton,
		exportDirMode, exportDir, _UC("Export"));
}

void LDViewPreferences::setupGeneralPage(void)
{
	hGeneralPage = hwndArray->pointerAtIndex(generalPageNumber);
	setupAntialiasing();
	setCheck(hGeneralPage, IDC_TRANS_DEFAULT_COLOR,
		ldPrefs->getTransDefaultColor());
	setCheck(hGeneralPage, IDC_PROCESS_LDCONFIG,
		ldPrefs->getProcessLdConfig());
	setCheck(hGeneralPage, IDC_RANDOM_COLORS, ldPrefs->getRandomColors());
	setCheck(hGeneralPage, IDC_FRAME_RATE, ldPrefs->getShowFps());
	setCheck(hGeneralPage, IDC_SHOW_AXES, ldPrefs->getShowAxes());
	setCheck(hGeneralPage, IDC_SHOW_ERRORS, ldPrefs->getShowErrors());
	setupFov();
	setupFullScreenRefresh();
	setupBackgroundColorButton();
	setupDefaultColorButton();
	setupMemoryUsage();
	setupSaveDirs();
}

void LDViewPreferences::enableWireframe(BOOL enable /*= TRUE*/)
{
	EnableWindow(hWireframeFogButton, enable);
	EnableWindow(hRemoveHiddenLinesButton, enable);
	EnableWindow(hWireframeThicknessLabel, enable);
	EnableWindow(hWireframeThicknessSlider, enable);
}

void LDViewPreferences::disableWireframe(void)
{
	enableWireframe(FALSE);
}

void LDViewPreferences::enableBfc(BOOL enable /*= TRUE*/)
{
	EnableWindow(hRedBackFacesButton, enable);
	EnableWindow(hGreenFrontFacesButton, enable);
	EnableWindow(hBlueNeutralFacesButton, enable);
}

void LDViewPreferences::disableBfc(void)
{
	enableBfc(FALSE);
}

void LDViewPreferences::initThemesButton(HWND hButton)
{
	if (CUIThemes::isThemeLibLoaded() && !hButtonTheme)
	{
		hButtonTheme = CUIThemes::openThemeData(hButton, L"Button");
	}
}

/*
void LDViewPreferences::initThemesTab(HWND hStatic)
{
	if (CUIThemes::isThemeLibLoaded() && !hTabTheme)
	{
		hTabTheme = CUIThemes::openThemeData(hStatic, L"Tab");
	}
}
*/

void LDViewPreferences::setupGroupCheckButton(HWND hPage, int buttonId,
											  bool state)
{
	if (IsWindowsVistaOrGreater())
	{
		// This is really only useful in Windows XP, since all it does
		// is make the button text color match XP's group box text color.
		// Later versions of Windows don't use a special group box text
		// color. And in Windows 10, for some reason the check box comes
		// out too big on a screen set to 96DPI.
		return;
	}
	bool done = false;
	if (CUIThemes::isThemeLibLoaded())
	{
		HWND hButton = GetDlgItem(hPage, buttonId);

		if (hButton)
		{
			initThemesButton(hButton);
			if (hButtonTheme)
			{
				DWORD dwStyle = GetWindowLong(hButton, GWL_STYLE);

				if ((dwStyle & BS_TYPEMASK) != BS_OWNERDRAW)
				{
					dwStyle = (dwStyle & ~BS_TYPEMASK) | BS_OWNERDRAW;
					buttonSetStyle(hButton, dwStyle);
				}
				if (GetWindowLongPtr(hButton, GWLP_WNDPROC) !=
					(LONG_PTR)staticGroupCheckButtonProc)
				{
					SetWindowLongPtr(hButton, GWLP_USERDATA, (LONG_PTR)this);
					if (origButtonWindowProc)
					{
						SetWindowLongPtr(hButton, GWLP_WNDPROC,
							(LONG_PTR)staticGroupCheckButtonProc);
					}
					else
					{
						origButtonWindowProc = SetWindowLongPtr(hButton,
							GWLP_WNDPROC, (LONG_PTR)staticGroupCheckButtonProc);
					}
				}
				checkStates[hButton] = state;
				fixControlSize(hButton);
				InvalidateRect(hButton, NULL, TRUE);
				done = true;
			}
			else if (origButtonWindowProc)
			{
				SetWindowLongPtr(hButton, GWLP_WNDPROC,
					(LONG_PTR)origButtonWindowProc);
			}
		}
	}
	if (!done)
	{
		setCheck(hPage, buttonId, state);
	}
}

void LDViewPreferences::setupWireframe(void)
{
	setupGroupCheckButton(hGeometryPage, IDC_WIREFRAME,
		ldPrefs->getDrawWireframe());
	hWireframeFogButton = GetDlgItem(hGeometryPage, IDC_WIREFRAME_FOG);
	hRemoveHiddenLinesButton = GetDlgItem(hGeometryPage,
		IDC_REMOVE_HIDDEN_LINES);
	hWireframeThicknessLabel = GetDlgItem(hGeometryPage,
		IDC_WIREFRAME_THICKNESS_LABEL);
	hWireframeThicknessSlider = GetDlgItem(hGeometryPage,
		IDC_WIREFRAME_THICKNESS);
	setCheck(hGeometryPage, IDC_WIREFRAME_FOG, ldPrefs->getUseWireframeFog());
	setCheck(hGeometryPage, IDC_REMOVE_HIDDEN_LINES,
		ldPrefs->getRemoveHiddenLines());
	CUIDialog::trackBarSetup(hGeometryPage, IDC_WIREFRAME_THICKNESS, 0, 5, 1,
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

void LDViewPreferences::setupBfc(void)
{
	setupGroupCheckButton(hGeometryPage, IDC_BFC, ldPrefs->getBfc());
	hRedBackFacesButton = GetDlgItem(hGeometryPage, IDC_RED_BACK_FACES);
	hGreenFrontFacesButton = GetDlgItem(hGeometryPage, IDC_GREEN_FRONT_FACES);
	hBlueNeutralFacesButton = GetDlgItem(hGeometryPage, IDC_BLUE_NEUTRAL_FACES);
	setCheck(hGeometryPage, IDC_RED_BACK_FACES, ldPrefs->getRedBackFaces());
	setCheck(hGeometryPage, IDC_GREEN_FRONT_FACES,
		ldPrefs->getGreenFrontFaces());
	setCheck(hGeometryPage, IDC_BLUE_NEUTRAL_FACES,
		ldPrefs->getBlueNeutralFaces());
	if (ldPrefs->getBfc())
	{
		enableBfc();
	}
	else
	{
		disableBfc();
	}
}

void LDViewPreferences::enableConditionals(void)
{
	EnableWindow(hShowAllConditionalButton, TRUE);
	EnableWindow(hShowConditionalControlsButton, TRUE);
	setCheck(hGeometryPage, IDC_ALL_CONDITIONAL,
		ldPrefs->getShowAllConditionalLines());
	setCheck(hGeometryPage, IDC_CONDITIONAL_CONTROLS,
		ldPrefs->getShowConditionalControlPoints());
}

void LDViewPreferences::disableConditionals(void)
{
	EnableWindow(hShowAllConditionalButton, FALSE);
	EnableWindow(hShowConditionalControlsButton, FALSE);
	setCheck(hGeometryPage, IDC_ALL_CONDITIONAL, false);
	setCheck(hGeometryPage, IDC_CONDITIONAL_CONTROLS, false);
}

void LDViewPreferences::enableEdges(void)
{
	EnableWindow(hConditionalHighlightsButton, TRUE);
	EnableWindow(hEdgesOnlyButton, TRUE);
	EnableWindow(hQualityLinesButton, TRUE);
	EnableWindow(hAlwaysBlackButton, TRUE);
	EnableWindow(hEdgeThicknessLabel, TRUE);
	EnableWindow(hEdgeThicknessSlider, TRUE);
	if (ldPrefs->getDrawConditionalHighlights())
	{
		enableConditionals();
	}
	else
	{
		disableConditionals();
	}
	setCheck(hGeometryPage, IDC_EDGES_ONLY, ldPrefs->getEdgesOnly());
	setCheck(hGeometryPage, IDC_CONDITIONAL_HIGHLIGHTS,
		ldPrefs->getDrawConditionalHighlights());
	setCheck(hGeometryPage, IDC_QUALITY_LINES, ldPrefs->getUsePolygonOffset());
	setCheck(hGeometryPage, IDC_ALWAYS_BLACK, ldPrefs->getBlackHighlights());
}

void LDViewPreferences::disableEdges(void)
{
	EnableWindow(hConditionalHighlightsButton, FALSE);
	EnableWindow(hEdgesOnlyButton, FALSE);
	EnableWindow(hQualityLinesButton, FALSE);
	EnableWindow(hAlwaysBlackButton, FALSE);
	EnableWindow(hEdgeThicknessLabel, FALSE);
	EnableWindow(hEdgeThicknessSlider, FALSE);
	disableConditionals();
	setCheck(hGeometryPage, IDC_EDGES_ONLY, false);
	setCheck(hGeometryPage, IDC_CONDITIONAL_HIGHLIGHTS, false);
	setCheck(hGeometryPage, IDC_QUALITY_LINES, false);
	setCheck(hGeometryPage, IDC_ALWAYS_BLACK, false);
}

void LDViewPreferences::setupEdgeLines(void)
{
	setupGroupCheckButton(hGeometryPage, IDC_HIGHLIGHTS,
		ldPrefs->getShowHighlightLines());
	hConditionalHighlightsButton = GetDlgItem(hGeometryPage,
		IDC_CONDITIONAL_HIGHLIGHTS);
	hShowAllConditionalButton = GetDlgItem(hGeometryPage, IDC_ALL_CONDITIONAL);
	hShowConditionalControlsButton = GetDlgItem(hGeometryPage,
		IDC_CONDITIONAL_CONTROLS);
	hEdgesOnlyButton = GetDlgItem(hGeometryPage, IDC_EDGES_ONLY);
	hQualityLinesButton = GetDlgItem(hGeometryPage, IDC_QUALITY_LINES);
	hAlwaysBlackButton = GetDlgItem(hGeometryPage, IDC_ALWAYS_BLACK);
	hEdgeThicknessLabel = GetDlgItem(hGeometryPage, IDC_EDGE_THICKNESS_LABEL);
	hEdgeThicknessSlider = GetDlgItem(hGeometryPage, IDC_EDGE_THICKNESS);
	CUIDialog::trackBarSetup(hGeometryPage, IDC_EDGE_THICKNESS, 0, 5, 1,
		ldPrefs->getEdgeThickness());
	if (ldPrefs->getShowHighlightLines())
	{
		enableEdges();
	}
	else
	{
		disableEdges();
	}
}

void LDViewPreferences::setupGeometryPage(void)
{
	hGeometryPage = hwndArray->pointerAtIndex(geometryPageNumber);
	setCheck(hGeometryPage, IDC_PART_BOUNDING_BOXES,
		ldPrefs->getBoundingBoxesOnly());
	setupSeamWidth();
	setupWireframe();
	setupBfc();
	setupEdgeLines();
}

void LDViewPreferences::setupOpacitySlider(void)
{
	if (ldPrefs->getCutawayMode() == LDVCutawayWireframe &&
		!LDVExtensionsSetup::haveAlpha())
	{
		ldPrefs->setCutawayAlpha(100);
		EnableWindow(hCutawayOpacitySlider, FALSE);
		EnableWindow(hCutawayOpacityLabel, FALSE);
		CUIDialog::trackBarSetPos(hEffectsPage, IDC_CUTAWAY_OPACITY, 100);
	}
	else
	{
		EnableWindow(hCutawayOpacitySlider, TRUE);
		EnableWindow(hCutawayOpacityLabel, TRUE);
	}
}

void LDViewPreferences::enableCutaway(void)
{
	int activeCutaway = 0;

	EnableWindow(hCutawayColorButton, TRUE);
	if (LDVExtensionsSetup::haveStencil())
	{
		EnableWindow(hCutawayMonochromButton, TRUE);
	}
	else
	{
		if (ldPrefs->getCutawayMode() == LDVCutawayStencil)
		{
			ldPrefs->setCutawayMode(LDVCutawayWireframe);
		}
		EnableWindow(hCutawayMonochromButton, FALSE);
	}
	setupOpacitySlider();
	EnableWindow(hCutawayThicknessSlider, TRUE);
	EnableWindow(hCutawayThicknessLabel, TRUE);
	setCheck(hEffectsPage, IDC_CUTAWAY_COLOR, false);
	setCheck(hEffectsPage, IDC_CUTAWAY_MONOCHROME, false);
	switch (ldPrefs->getCutawayMode())
	{
	case LDVCutawayWireframe:
		activeCutaway = IDC_CUTAWAY_COLOR;
		break;
	case LDVCutawayStencil:
		activeCutaway = IDC_CUTAWAY_MONOCHROME;
		break;
	}
	setCheck(hEffectsPage, activeCutaway, true);
}

void LDViewPreferences::disableCutaway(void)
{
	EnableWindow(hCutawayColorButton, FALSE);
	EnableWindow(hCutawayMonochromButton, FALSE);
	EnableWindow(hCutawayOpacityLabel, FALSE);
	EnableWindow(hCutawayOpacitySlider, FALSE);
	EnableWindow(hCutawayThicknessLabel, FALSE);
	EnableWindow(hCutawayThicknessSlider, FALSE);
	setCheck(hEffectsPage, IDC_CUTAWAY_COLOR, false);
	setCheck(hEffectsPage, IDC_CUTAWAY_MONOCHROME, false);
}

void LDViewPreferences::setupCutaway(void)
{
	setupGroupCheckButton(hEffectsPage, IDC_CUTAWAY,
		ldPrefs->getCutawayMode() != LDVCutawayNormal);
	hCutawayColorButton = GetDlgItem(hEffectsPage, IDC_CUTAWAY_COLOR);
	hCutawayMonochromButton = GetDlgItem(hEffectsPage, IDC_CUTAWAY_MONOCHROME);
	hCutawayOpacityLabel = GetDlgItem(hEffectsPage, IDC_CUTAWAY_OPACITY_LABEL);
	hCutawayOpacitySlider = GetDlgItem(hEffectsPage, IDC_CUTAWAY_OPACITY);
	hCutawayThicknessLabel = GetDlgItem(hEffectsPage,
		IDC_CUTAWAY_THICKNESS_LABEL);
	hCutawayThicknessSlider = GetDlgItem(hEffectsPage, IDC_CUTAWAY_THICKNESS);
	CUIDialog::trackBarSetup(hEffectsPage, IDC_CUTAWAY_OPACITY, 1, 100, 10,
		ldPrefs->getCutawayAlpha());
	CUIDialog::trackBarSetup(hEffectsPage, IDC_CUTAWAY_THICKNESS, 0, 5, 1,
		ldPrefs->getCutawayThickness());
	if (ldPrefs->getCutawayMode() == LDVCutawayNormal)
	{
		disableCutaway();
	}
	else
	{
		enableCutaway();
	}
}

void LDViewPreferences::enableStereo(void)
{
	int activeStereo = 0;

	EnableWindow(hHardwareStereoButton, TRUE);
	EnableWindow(hCrossEyedStereoButton, TRUE);
	EnableWindow(hParallelStereoButton, TRUE);
	EnableWindow(hStereoSpacingSlider, TRUE);
	EnableWindow(hStereoSpacingLabel, TRUE);
	setCheck(hEffectsPage, IDC_HARDWARE_STEREO, false);
	setCheck(hEffectsPage, IDC_CROSS_EYED_STEREO, false);
	setCheck(hEffectsPage, IDC_PARALLEL_STEREO, false);
	switch (ldPrefs->getStereoMode())
	{
	case LDVStereoHardware:
		activeStereo = IDC_HARDWARE_STEREO;
		break;
	case LDVStereoCrossEyed:
		activeStereo = IDC_CROSS_EYED_STEREO;
		break;
	case LDVStereoParallel:
		activeStereo = IDC_PARALLEL_STEREO;
		break;
	}
	setCheck(hEffectsPage, activeStereo, true);
}

void LDViewPreferences::disableStereo(void)
{
	EnableWindow(hHardwareStereoButton, FALSE);
	EnableWindow(hCrossEyedStereoButton, FALSE);
	EnableWindow(hParallelStereoButton, FALSE);
	EnableWindow(hStereoSpacingSlider, FALSE);
	EnableWindow(hStereoSpacingLabel, FALSE);
	setCheck(hEffectsPage, IDC_HARDWARE_STEREO, false);
	setCheck(hEffectsPage, IDC_CROSS_EYED_STEREO, false);
	setCheck(hEffectsPage, IDC_PARALLEL_STEREO, false);
}

void LDViewPreferences::setupStereo(void)
{
	setupGroupCheckButton(hEffectsPage, IDC_STEREO,
		ldPrefs->getStereoMode() != LDVStereoNone);
	hHardwareStereoButton = GetDlgItem(hEffectsPage, IDC_HARDWARE_STEREO);
	hCrossEyedStereoButton = GetDlgItem(hEffectsPage, IDC_CROSS_EYED_STEREO);
	hParallelStereoButton = GetDlgItem(hEffectsPage, IDC_PARALLEL_STEREO);
	hStereoSpacingSlider = GetDlgItem(hEffectsPage, IDC_STEREO_SPACING);
	hStereoSpacingLabel = GetDlgItem(hEffectsPage, IDC_STEREO_SPACING_LABEL);
	CUIDialog::trackBarSetup(hEffectsPage, IDC_STEREO_SPACING, 0, 100, 10,
		ldPrefs->getStereoEyeSpacing());
	if (ldPrefs->getStereoMode() == LDVStereoNone)
	{
		disableStereo();
	}
	else
	{
		enableStereo();
	}
}

void LDViewPreferences::uncheckLightDirections(void)
{
	for (IntIntMap::const_iterator it = lightDirIndexToId.begin()
		; it != lightDirIndexToId.end(); it++)
	{
		checkStates[GetDlgItem(hEffectsPage, it->second)] = false;
		if (!CUIThemes::isThemeLibLoaded())
		{
			setCheck(hEffectsPage, it->second, false);
		}
	}
}

void LDViewPreferences::enableLighting(void)
{
	int lightDirButton = -1;
	BOOL enabled = TRUE;

	EnableWindow(hLightQualityButton, TRUE);
	EnableWindow(hLightSubduedButton, TRUE);
	EnableWindow(hLightSpecularButton, TRUE);
	EnableWindow(hLightAlternateButton, TRUE);
	EnableWindow(hLightDrawLightDatsButton, TRUE);
	if (!ldPrefs->getDrawLightDats())
	{
		enabled = FALSE;
	}
	EnableWindow(hLightOptionalStandardButton, enabled);
	EnableWindow(GetDlgItem(hEffectsPage, IDC_LIGHT_DIR), TRUE);
	setCheck(hEffectsPage, IDC_LIGHTING_QUALITY, ldPrefs->getQualityLighting());
	setCheck(hEffectsPage, IDC_LIGHTING_SUBDUED, ldPrefs->getSubduedLighting());
	setCheck(hEffectsPage, IDC_SPECULAR, ldPrefs->getUseSpecular());
	setCheck(hEffectsPage, IDC_ALTERNATE_LIGHTING, ldPrefs->getOneLight());
	setCheck(hEffectsPage, IDC_DRAW_LIGHT_DATS, ldPrefs->getDrawLightDats());
	setCheck(hEffectsPage, IDC_OPTIONAL_STANDARD_LIGHT,
		ldPrefs->getOptionalStandardLight());
	lightDirButton = lightDirIndexToId[(int)ldPrefs->getLightDirection() - 1];
	uncheckLightDirections();
	if (lightDirButton != 0)
	{
		checkStates[GetDlgItem(hEffectsPage, lightDirButton)] = true;
		if (!CUIThemes::isThemeLibLoaded())
		{
			setCheck(hEffectsPage, lightDirButton, true);
		}
	}
	for (int i = 0; i < (int)lightAngleButtons.size(); i++)
	{
		EnableWindow(lightAngleButtons[i], TRUE);
	}
}

void LDViewPreferences::disableLighting(void)
{
	EnableWindow(hLightQualityButton, FALSE);
	EnableWindow(hLightSubduedButton, FALSE);
	EnableWindow(hLightSpecularButton, FALSE);
	EnableWindow(hLightAlternateButton, FALSE);
	EnableWindow(hLightDrawLightDatsButton, FALSE);
	EnableWindow(hLightOptionalStandardButton, FALSE);
	EnableWindow(GetDlgItem(hEffectsPage, IDC_LIGHT_DIR), FALSE);
	// Don't disable IDC_HIDE_LIGHT_DAT.
	for (int i = 0; i < (int)lightAngleButtons.size(); i++)
	{
		EnableWindow(lightAngleButtons[i], FALSE);
	}
	setCheck(hEffectsPage, IDC_LIGHTING, false);
	setCheck(hEffectsPage, IDC_LIGHTING_QUALITY, false);
	setCheck(hEffectsPage, IDC_LIGHTING_SUBDUED, false);
	setCheck(hEffectsPage, IDC_SPECULAR, false);
	setCheck(hEffectsPage, IDC_ALTERNATE_LIGHTING, false);
	setCheck(hEffectsPage, IDC_DRAW_LIGHT_DATS, false);
	setCheck(hEffectsPage, IDC_OPTIONAL_STANDARD_LIGHT, false);
	uncheckLightDirections();
}

void LDViewPreferences::setupLightAngleButtons(void)
{
	lightAngleButtons.clear();
	double scaleFactor = getScaleFactor();
	HIMAGELIST hImageList = NULL;
	SIZE size;
	size.cx = size.cy = scalePoints(16);
	hImageList = ImageList_Create(size.cx, size.cy, ILC_COLOR32, 9, 0);
	for (IntIntMap::const_iterator it = lightDirIndexToId.begin()
		; it != lightDirIndexToId.end(); it++)
	{
		HWND hButton = GetDlgItem(hEffectsPage, it->second);

		if (hButton)
		{
			HICON hIcon = NULL;
			if (scaleFactor > 1.0)
			{
				TCImage *image = TCImage::createFromResource(NULL, it->second, 4, true, scaleFactor);
				if (image != NULL)
				{
					int index = addImageToImageList(hImageList, image, size);
					UINT flags = ILD_TRANSPARENT;
					if (!CUIScaler::use32bit())
					{
						// Note: this doesn't work quite right, but it's close.
						flags = ILD_IMAGE;
					}
					hIcon = ImageList_GetIcon(hImageList, index, flags);
					image->release();
				}
			}
			if (!hIcon)
			{
				hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(it->second),
					IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			}
			if (hIcon)
			{
				lightAngleButtons.push_back(hButton);
				HICON hOldIcon = buttonSetIcon(hButton, hIcon);
				if (hOldIcon != NULL)
				{
					DestroyIcon(hOldIcon);
				}
				setupIconButton(hButton);
			}
		}
	}
	ImageList_Destroy(hImageList);
}

void LDViewPreferences::setupLighting(void)
{
	setupGroupCheckButton(hEffectsPage, IDC_LIGHTING,
		ldPrefs->getUseLighting());
	hLightQualityButton = GetDlgItem(hEffectsPage, IDC_LIGHTING_QUALITY);
	hLightSubduedButton = GetDlgItem(hEffectsPage, IDC_LIGHTING_SUBDUED);
	hLightSpecularButton = GetDlgItem(hEffectsPage, IDC_SPECULAR);
	hLightAlternateButton = GetDlgItem(hEffectsPage, IDC_ALTERNATE_LIGHTING);
	hLightDrawLightDatsButton = GetDlgItem(hEffectsPage, IDC_DRAW_LIGHT_DATS);
	checkStates[hLightDrawLightDatsButton] = ldPrefs->getDrawLightDats();
	hLightOptionalStandardButton = GetDlgItem(hEffectsPage,
		IDC_OPTIONAL_STANDARD_LIGHT);
	setupLightAngleButtons();
	if (ldPrefs->getUseLighting())
	{
		enableLighting();
	}
	else
	{
		disableLighting();
	}
	setCheck(hEffectsPage, IDC_HIDE_LIGHT_DAT, ldPrefs->getNoLightGeom());
}

void LDViewPreferences::setupEffectsPage(void)
{
	hEffectsPage = hwndArray->pointerAtIndex(effectsPageNumber);
	setupLighting();
	setupStereo();
	setupCutaway();
	setCheck(hEffectsPage, IDC_STIPPLE, ldPrefs->getUseStipple());
	setCheck(hEffectsPage, IDC_SORT, ldPrefs->getSortTransparent());
	setCheck(hEffectsPage, IDC_FLAT_SHADING, ldPrefs->getUseFlatShading());
	setCheck(hEffectsPage, IDC_SMOOTH_CURVES, ldPrefs->getPerformSmoothing());
}

int LDViewPreferences::sliderValueFromTextureOffset(double value)
{
	return (int)(value * 10.0);
}

TCFloat32 LDViewPreferences::textureOffsetFromSliderValue(int value)
{
	return (TCFloat32)(value / 10.0f);
}

int LDViewPreferences::sliderValueFromAniso(double value)
{
	return (int)(log(value) / log(2.0) + 0.5f);
}

TCFloat32 LDViewPreferences::anisoFromSliderValue(int value)
{
	return (TCFloat32)(1 << value);
}

void LDViewPreferences::setupSubstitution(void)
{
	setupGroupCheckButton(hPrimitivesPage, IDC_PRIMITIVE_SUBSTITUTION,
		ldPrefs->getAllowPrimitiveSubstitution());
	hCurveQualityLabel = GetDlgItem(hPrimitivesPage, IDC_CURVE_QUALITY_LABEL);
	hCurveQualitySlider = GetDlgItem(hPrimitivesPage, IDC_CURVE_QUALITY);
	CUIDialog::trackBarSetup(hPrimitivesPage, IDC_CURVE_QUALITY, 1, 12, 1,
		ldPrefs->getCurveQuality());
	if (ldPrefs->getAllowPrimitiveSubstitution())
	{
		enablePrimitives();
	}
	else
	{
		disablePrimitives();
	}
}

void LDViewPreferences::setupTextures(void)
{
	GLfloat maxAniso = TREGLExtensions::getMaxAnisoLevel();
	short numAnisoLevels = (short)sliderValueFromAniso(maxAniso);
	TCFloat32 anisoLevel = ldPrefs->getAnisoLevel();
	TCFloat32 textureOffset = ldPrefs->getTextureOffsetFactor();

	hTextureStudsButton = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_STUDS);
	hTexmapsAfterTransparentButton = GetDlgItem(hPrimitivesPage, IDC_TRANSPARENT_TEXTURES_LAST);
	hTextureOffsetLabel = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_OFFSET_LABEL);
	hTextureOffsetSlider = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_OFFSET);
	hTextureNearestButton = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_NEAREST);
	hTextureBilinearButton = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_BILINEAR);
	hTextureTrilinearButton = GetDlgItem(hPrimitivesPage,
		IDC_TEXTURE_TRILINEAR);
	hTextureAnisoButton = GetDlgItem(hPrimitivesPage, IDC_TEXTURE_ANISO);
	hAnisoLevelSlider = GetDlgItem(hPrimitivesPage, IDC_ANISO_LEVEL);
	hAnisoLevelLabel = GetDlgItem(hPrimitivesPage, IDC_ANISO_LEVEL_LABEL);
	CUIDialog::trackBarSetup(hPrimitivesPage, IDC_TEXTURE_OFFSET, 10, 100, 10,
		sliderValueFromTextureOffset(textureOffset));
	if (anisoLevel > maxAniso)
	{
		anisoLevel = (TCFloat32)maxAniso;
	}
	setAniso(sliderValueFromAniso(anisoLevel));
	if (anisoLevel < 2.0f)
	{
		anisoLevel = 2.0f;
	}
	CUIDialog::trackBarSetup(hPrimitivesPage, IDC_ANISO_LEVEL, 1, numAnisoLevels, 1,
		sliderValueFromAniso(anisoLevel));
	setCheck(hPrimitivesPage, IDC_TEXMAPS, ldPrefs->getTexmaps());
	updateTextureFilteringEnabled();
	updateTexmapsEnabled();
}

void LDViewPreferences::setupPrimitivesPage(void)
{
	hPrimitivesPage = hwndArray->pointerAtIndex(primitivesPageNumber);
	setupTextures();
	setupSubstitution();
	setCheck(hPrimitivesPage, IDC_STUD_QUALITY, !ldPrefs->getQualityStuds());
	setCheck(hPrimitivesPage, IDC_HI_RES, ldPrefs->getHiResPrimitives());
}

void LDViewPreferences::enableProxyServer(void)
{
	UCCHAR proxyPortString[128];

	EnableWindow(hProxyServerLabel, TRUE);
	EnableWindow(hProxyServer, TRUE);
	EnableWindow(hProxyPortLabel, TRUE);
	EnableWindow(hProxyPort, TRUE);
	ucstring ucProxyServerString;
	utf8toucstring(ucProxyServerString, ldPrefs->getProxyServer());
	windowSetText(hProxyServer, ucProxyServerString);
	sucprintf(proxyPortString, COUNT_OF(proxyPortString), _UC("%d"),
		ldPrefs->getProxyPort());
	windowSetText(hProxyPort, proxyPortString);
}

void LDViewPreferences::disableProxyServer(void)
{
	EnableWindow(hProxyServerLabel, FALSE);
	EnableWindow(hProxyServer, FALSE);
	EnableWindow(hProxyPortLabel, FALSE);
	EnableWindow(hProxyPort, FALSE);
	windowSetText(hProxyServer, _UC(""));
	windowSetText(hProxyPort, _UC(""));
}

void LDViewPreferences::setupProxy(void)
{
	int activeProxyType = IDC_PROXY_NONE;

	setCheck(hUpdatesPage, IDC_PROXY_NONE, false);
	setCheck(hUpdatesPage, IDC_PROXY_WINDOWS, false);
	setCheck(hUpdatesPage, IDC_PROXY_MANUAL, false);
	switch (ldPrefs->getProxyType())
	{
	case 0:
		activeProxyType = IDC_PROXY_NONE;
		disableProxyServer();
		break;
	case 1:
		activeProxyType = IDC_PROXY_WINDOWS;
		disableProxyServer();
		break;
	case 2:
		activeProxyType = IDC_PROXY_MANUAL;
		enableProxyServer();
		break;
	}
	setCheck(hUpdatesPage, activeProxyType, true);
}

void LDViewPreferences::enableCheckPartTracker(void)
{
	UCCHAR tmpString[128];

	EnableWindow(hMissingPartsLabel, TRUE);
	EnableWindow(hMissingParts, TRUE);
	EnableWindow(hUpdatedPartsLabel, TRUE);
	EnableWindow(hUpdatedParts, TRUE);
	sucprintf(tmpString, COUNT_OF(tmpString), _UC("%0d"),
		ldPrefs->getMissingPartWait());
	windowSetText(hMissingParts, tmpString);
	sucprintf(tmpString, COUNT_OF(tmpString), _UC("%0d"),
		ldPrefs->getUpdatedPartWait());
	windowSetText(hUpdatedParts, tmpString);
}

void LDViewPreferences::disableCheckPartTracker(void)
{
	EnableWindow(hMissingPartsLabel, FALSE);
	EnableWindow(hMissingParts, FALSE);
	EnableWindow(hUpdatedPartsLabel, FALSE);
	EnableWindow(hUpdatedParts, FALSE);
	windowSetText(hMissingParts, _UC(""));
	windowSetText(hUpdatedParts, _UC(""));
}

void LDViewPreferences::setupUpdatesPage(void)
{
	hUpdatesPage = hwndArray->pointerAtIndex(updatesPageNumber);
	hProxyServerLabel = GetDlgItem(hUpdatesPage, IDC_PROXY_SERVER_LABEL);
	hProxyServer = GetDlgItem(hUpdatesPage, IDC_PROXY_SERVER);
	hProxyPortLabel = GetDlgItem(hUpdatesPage, IDC_PROXY_PORT_LABEL);
	hProxyPort = GetDlgItem(hUpdatesPage, IDC_PROXY_PORT);
	hCheckPartTracker = GetDlgItem(hUpdatesPage, IDC_CHECK_PART_TRACKER);
	hMissingPartsLabel = GetDlgItem(hUpdatesPage, IDC_MISSING_DAYS_LABEL);
	hMissingParts = GetDlgItem(hUpdatesPage, IDC_MISSING_DAYS);
	hUpdatedPartsLabel = GetDlgItem(hUpdatesPage, IDC_UPDATED_DAYS_LABEL);
	hUpdatedParts = GetDlgItem(hUpdatesPage, IDC_UPDATED_DAYS);
	if (ldPrefs->getCheckPartTracker())
	{
		enableCheckPartTracker();
	}
	else
	{
		disableCheckPartTracker();
	}
	setCheck(hUpdatesPage, IDC_CHECK_PART_TRACKER,
		ldPrefs->getCheckPartTracker());
	setupProxy();
}

void LDViewPreferences::selectPrefSet(const ucstring& prefSet, bool force)
{
	if (!prefSet.empty())
	{
		listBoxSelectString(hPrefSetsList, prefSet);
		doPrefSetSelected(force);
	}
	else
	{
		char *savedSession =
			TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);

		if (savedSession && savedSession[0])
		{
			ucstring ucSavedSession;
			utf8toucstring(ucSavedSession, savedSession);
			selectPrefSet(ucSavedSession, force);
		}
		else
		{
			selectPrefSet(DEFAULT_PREF_SET, force);
		}
		delete savedSession;
	}
}

void LDViewPreferences::setupPrefSetsList(void)
{
	TCStringArray *sessionNames = TCUserDefaults::getAllSessionNames();
	int i;
	int count = sessionNames->getCount();

	hPrefSetsList = GetDlgItem(hPrefSetsPage, IDC_PREFSETS_LIST);
	listBoxAddString(hPrefSetsList, DEFAULT_PREF_SET);
	for (i = 0; i < count; i++)
	{
		ucstring ucSessionName;
		utf8toucstring(ucSessionName, sessionNames->stringAtIndex(i));
		listBoxAddString(hPrefSetsList, ucSessionName);
	}
	selectPrefSet();
	sessionNames->release();
}

void LDViewPreferences::setupPrefSetsPage(void)
{
	hPrefSetsPage = hwndArray->pointerAtIndex(prefSetsPageNumber);
	hDeletePrefSetButton = GetDlgItem(hPrefSetsPage, IDC_DELETE_PREF_SET);
	hNewPrefSetButton = GetDlgItem(hPrefSetsPage, IDC_NEW_PREF_SET);
	hPrefSetHotKeyButton = GetDlgItem(hPrefSetsPage, IDC_PREF_SET_HOT_KEY);
	setupPrefSetsList();
}

void LDViewPreferences::setupAntialiasing(void)
{
	TCIntArray *fsaaModes = LDVExtensionsSetup::getFSAAModes();
	UCCHAR modeString[1024];

	// Remove all items from FSAA combo box list.
	CUIDialog::comboResetContent(hGeneralPage, IDC_FSAA_COMBO);
	// Add "None" to FSAA combo box list as only item.
	CUIDialog::comboAddString(hGeneralPage, IDC_FSAA_COMBO, ls(_UC("FsaaNone")));
	// Select "None", just in case something else doesn't get selected later.
	CUIDialog::comboSetCurSel(hGeneralPage, IDC_FSAA_COMBO, 0);
	// The following array should always exist, even if it is empty, but check
	// just to be sure.
	if (fsaaModes)
	{
		int i;
		int count = fsaaModes->getCount();

		// Note that fsaaModes contains a sorted array of unique FSAA factors.
		for (i = 0; i < count; i++)
		{
			int value = (*fsaaModes)[i];

			sucprintf(modeString, COUNT_OF(modeString),
				ls(_UC("FsaaNx")), value);
			CUIDialog::comboAddString(hGeneralPage, IDC_FSAA_COMBO, modeString);
			// nVidia hardare supports Quincunx and 9-box pattern, so add an
			// "Enhanced" item to the list if the extension is supported and
			// the current factor is 2 or 4.
			if ((value == 2 || value == 4) &&
				TREGLExtensions::haveNvMultisampleFilterHintExtension())
			{
				sucprintf(modeString, COUNT_OF(modeString),
					ls(_UC("FsaaNx")), value);
				ucstrcat(modeString, _UC(" "));
				ucstrcat(modeString, ls(_UC("FsaaEnhanced")));
				CUIDialog::comboAddString(hGeneralPage, IDC_FSAA_COMBO, modeString);
			}
		}
	}
	if (ldPrefs->getFsaaMode())
	{
		sucprintf(modeString, COUNT_OF(modeString),
			ls(_UC("FsaaNx")), getFSAAFactor());
		if (getUseNvMultisampleFilter())
		{
			ucstrcat(modeString, _UC(" "));
			ucstrcat(modeString, ls(_UC("FsaaEnhanced")));
		}
		if (CUIDialog::comboSelectString(hGeneralPage, IDC_FSAA_COMBO, 0,
			modeString) == CB_ERR)
		{
			// We didn't find the selected mode, so reset to none.
			ldPrefs->setFsaaMode(0);
		}
	}
	setCheck(hGeneralPage, IDC_LINE_AA, ldPrefs->getLineSmoothing());
}

bool LDViewPreferences::doApply(void)
{
	applyChanges();
	checkAbandon = true;
	return true;
}

void LDViewPreferences::clear(void)
{
	if (hBackgroundColorBitmap)
	{
		DeleteObject(hBackgroundColorBitmap);
		hBackgroundColorBitmap = NULL;
	}
	if (hDefaultColorBitmap)
	{
		DeleteObject(hDefaultColorBitmap);
		hDefaultColorBitmap = NULL;
	}
	if (hButtonColorDC)
	{
		DeleteDC(hButtonColorDC);
		hButtonColorDC = NULL;
	}
	hGeneralPage = NULL;
	hGeometryPage = NULL;
	hEffectsPage = NULL;
	hPrimitivesPage = NULL;
	hUpdatesPage = NULL;
	hPrefSetsPage = NULL;
	CUIPropertySheet::clear();
}

BOOL LDViewPreferences::doNewPrefSetInit(HWND /*hDlg*/, HWND hNewPrefSetField)
{
	SendMessage(hNewPrefSetField, EM_LIMITTEXT, 32, 0);
	return TRUE;
}

BOOL LDViewPreferences::doHotKeyInit(HWND hDlg, HWND /*hHotKeyCombo*/)
{
	int i;
	ucstring prefSetName = getSelectedPrefSet();

	if (!prefSetName.empty())
	{
		windowSetText(hDlg, prefSetName);
	}
	else
	{
		windowSetText(hDlg, _UC("???"));
	}
	CUIDialog::comboAddString(hDlg, IDC_HOTKEY_COMBO, ls(_UC("<None>")));
	for (i = 1; i <= 10; i++)
	{
		UCCHAR numString[5];

		sucprintf(numString, COUNT_OF(numString), _UC("%d"), i % 10);
		CUIDialog::comboAddString(hDlg, IDC_HOTKEY_COMBO, numString);
	}
	CUIDialog::comboSetCurSel(hDlg, IDC_HOTKEY_COMBO, hotKeyIndex);
	return TRUE;
}

bool LDViewPreferences::handleDpiChange(void)
{
	if (!lightAngleButtons.empty())
	{
		setupLightAngleButtons();
	}
	if (hGeneralPage != NULL)
	{
		setupBackgroundColorButton();
		setupDefaultColorButton();
	}
	return true;
}

BOOL LDViewPreferences::doDialogInit(
	HWND hDlg,
	HWND hFocusWindow,
	LPARAM lParam)
{
	CUIPropertySheet::doDialogInit(hDlg, hFocusWindow, lParam);
	HWND hNewPrefSetField = NULL;
	HWND hHotKeyCombo = NULL;

	hNewPrefSetField = GetDlgItem(hDlg, IDC_NEW_PREF_SET_FIELD);
	if (hNewPrefSetField)
	{
		return doNewPrefSetInit(hDlg, hNewPrefSetField);
	}
	hHotKeyCombo = GetDlgItem(hDlg, IDC_HOTKEY_COMBO);
	if (hHotKeyCombo)
	{
		return doHotKeyInit(hDlg, hHotKeyCombo);
	}
	return TRUE;
}

bool LDViewPreferences::shouldSetActive(int index)
{
	if (applyEnabledArray->itemAtIndex(prefSetsPageNumber) &&
		index != prefSetsPageNumber)
	{
		if (!setActiveWarned)
		{
			setActiveWarned = true;
			messageBoxUC(hWindow,
				ls(_UC("PrefSetApplyBeforeLeave")),
				ls(_UC("Error")), MB_OK | MB_ICONWARNING);
		}
		return false;
	}
	setActiveWarned = false;
	return true;
}

int LDViewPreferences::getFSAAFactor(void)
{
	int fsaaMode = ldPrefs->getFsaaMode();

	if (fsaaMode && LDVExtensionsSetup::haveMultisampleExtension())
	{
		if (fsaaMode <= 5)
		{
			return fsaaMode & 0x6; // Mask off bottom bit
		}
		else
		{
			return fsaaMode >> 3;
		}
	}
	else
	{
		return 0;
	}
}

bool LDViewPreferences::getUseNvMultisampleFilter(void)
{
	int fsaaMode = ldPrefs->getFsaaMode();

	if ((fsaaMode & 0x1) &&
		TREGLExtensions::haveNvMultisampleFilterHintExtension())
	{
		return true;
	}
	else
	{
		return false;
	}
}
BOOL LDViewPreferences::doDrawIconPushButton(
	HWND hWnd, HTHEME hTheme, LPDRAWITEMSTRUCT drawItemStruct)
{
	HDC hDrawItemDC = drawItemStruct->hDC;
	bool bIsPressed = (drawItemStruct->itemState & ODS_SELECTED) != 0;
	bool bIsFocused = (drawItemStruct->itemState & ODS_FOCUS) != 0;
	bool bDrawFocusRect = (drawItemStruct->itemState & ODS_NOFOCUSRECT) == 0;
	bool bIsDisabled = (drawItemStruct->itemState & ODS_DISABLED) != 0;
	bool bIsChecked = checkStates[hWnd];
	RECT itemRect = drawItemStruct->rcItem;
	HICON hIcon;

	SetBkMode(hDrawItemDC, TRANSPARENT);
	// Prepare draw... paint button background
	if (CUIThemes::isThemeLibLoaded() && hTheme)
	{
		DWORD state;

		if (bIsPressed)
		{
			state = PBS_PRESSED;
		}
		else if (!bIsDisabled)
		{
			if (hMouseOverButton == hWnd)
			{
				state = PBS_HOT;
			}
			else
			{
				if (bIsChecked)
				{
					state = PBS_PRESSED;
				}
				else
				{
					state = PBS_NORMAL;
				}
			}
		}
		else
		{
			state = PBS_DISABLED;
		}
		CUIThemes::drawThemeBackground(hTheme, hDrawItemDC, BP_PUSHBUTTON, state,
			&itemRect, NULL);
	}
	else
	{
		if (bIsFocused)
		{
			HBRUSH br = CreateSolidBrush(RGB(0,0,0));  
			FrameRect(hDrawItemDC, &itemRect, br);
			InflateRect(&itemRect, -1, -1);
			DeleteObject(br);
		} // if		

		COLORREF crColor = GetSysColor(COLOR_BTNFACE);

		HBRUSH	brBackground = CreateSolidBrush(crColor);

		FillRect(hDrawItemDC, &itemRect, brBackground);

		DeleteObject(brBackground);

		// Draw pressed button
		if (bIsPressed)
		{
			HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			FrameRect(hDrawItemDC, &itemRect, brBtnShadow);
			DeleteObject(brBtnShadow);
		}

		else // ...else draw non pressed button
		{
			UINT uState = DFCS_BUTTONPUSH |
                  ((hMouseOverButton == hWnd) ? DFCS_HOT : 0) |
                  ((bIsPressed) ? DFCS_PUSHED : 0);

			DrawFrameControl(hDrawItemDC, &itemRect, DFC_BUTTON, uState);
		} // else
	}

	// Draw the focus rect
	if (bIsFocused && bDrawFocusRect)
	{
		RECT focusRect = itemRect;

		InflateRect(&focusRect, -3, -3);
		DrawFocusRect(hDrawItemDC, &focusRect);
	}

	// Draw the icon
	hIcon = buttonGetIcon(hWnd);
	if (hIcon)
	{
		ICONINFO iconInfo;

		//if (GetBitmapDimensionEx(hBitmap, &bitmapSize))
		if (GetIconInfo(hIcon, &iconInfo))
		{
			SIZE iconSize;
			RECT clientRect;
			RECT contentRect;
			int lwidth;
			int lheight;
			BITMAPINFO bmi;

			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof (bmi.bmiHeader);
			if (GetDIBits(hDrawItemDC, iconInfo.hbmColor, 0, 0, NULL, &bmi,
				DIB_RGB_COLORS))
			{
				iconSize.cx = bmi.bmiHeader.biWidth;
				iconSize.cy = bmi.bmiHeader.biHeight;
			}
			else
			{
				iconSize.cx = 32;
				iconSize.cy = 32;
			}
			GetClientRect(hWnd, &clientRect);
			if (CUIThemes::isThemeLibLoaded() && hTheme)
			{
				CUIThemes::getThemeBackgroundContentRect(hTheme, NULL,
					BP_PUSHBUTTON, PBS_HOT, &clientRect, &contentRect);
			}
			else
			{
				contentRect = clientRect;
			}
			lwidth = contentRect.right - contentRect.left;
			lheight = contentRect.bottom - contentRect.top;
			if (bIsDisabled)
			{
				HBITMAP hOldBitmap;
				int dstX, dstY, srcX, srcY;
				COLORREF shadowColor;
				COLORREF highlightColor;
				HBRUSH hHighlightBrush;
				HBRUSH hShadowBrush;
				HANDLE hOldBrush;
				HBITMAP hInvMask = CreateBitmap(iconSize.cx, iconSize.cy, 1, 1,
					NULL);
				HDC hTempDC = CreateCompatibleDC(hDrawItemDC);

				CUIThemes::getThemeColor(hTheme, BP_CHECKBOX,
					CBS_CHECKEDDISABLED, TMT_TEXTCOLOR, &shadowColor);
				CUIThemes::getThemeColor(hTheme, BP_PUSHBUTTON, PBS_NORMAL,
					TMT_EDGEHIGHLIGHTCOLOR, &highlightColor);
				hShadowBrush = CreateSolidBrush(shadowColor);
				hHighlightBrush = CreateSolidBrush(highlightColor);
				SelectObject(hTempDC, iconInfo.hbmMask);
				hOldBitmap = (HBITMAP)SelectObject(hButtonColorDC, hInvMask);
				BitBlt(hButtonColorDC, 0, 0, iconSize.cx, iconSize.cy, hTempDC,
					0, 0, NOTSRCCOPY);
				if (iconSize.cx > lwidth)
				{
					dstX = contentRect.left;
					srcX = (iconSize.cx - lwidth) / 2;
				}
				else
				{
					dstX = contentRect.left + (lwidth - iconSize.cx) / 2;
					lwidth = iconSize.cx;
					srcX = 0;
				}
				if (iconSize.cy > lheight)
				{
					dstY = contentRect.top;
					srcY = (iconSize.cy - lwidth) / 2;
				}
				else
				{
					dstY = contentRect.top + (lheight - iconSize.cy) / 2;
					lheight = iconSize.cy;
					srcY = 0;
				}
				hOldBrush = SelectObject(hDrawItemDC, hHighlightBrush);
				// The raster op we're using is somewhat voodoo magic.  However,
				// it means DSPDxax, according to this page here:
				//
				// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/pantdraw_6n77.asp
				//
				// In any event DSPDxax means:
				// dst XOR src AND pat XOR dst
				// Where pat is the current brush in the destination DC.
				// What that means is that everywhere that the source is white,
				// the destination ends up filled with its brush.  Everywhere
				// the source is black (1-bit bitmap), the destination remains
				// unchanged.
				BitBlt(hDrawItemDC, dstX + 1, dstY + 1, lwidth, lheight, hButtonColorDC,
					srcX, srcY, 0x00E20746L);
				SelectObject(hDrawItemDC, hShadowBrush);
				BitBlt(hDrawItemDC, dstX, dstY, lwidth, lheight, hButtonColorDC,
					srcX, srcY, 0x00E20746L);
				SelectObject(hDrawItemDC, hOldBrush);
				SelectObject(hButtonColorDC, hOldBitmap);
				DeleteDC(hTempDC);
				DeleteObject(hHighlightBrush);
				DeleteObject(hShadowBrush);
				DeleteObject(hInvMask);
			}
			else
			{
				int dstX = contentRect.left + (lwidth - iconSize.cx) / 2;
				int dstY = contentRect.top + (lheight - iconSize.cy) / 2;

				if (bIsChecked)
				{
					dstX++;
					dstY++;
				}
				DrawIconEx(hDrawItemDC, dstX, dstY, hIcon, iconSize.cx, iconSize.cy,
					0, NULL, DI_NORMAL);
				//DrawIcon(hDrawItemDC, dstX, dstY, hIcon);
			}
		}
	}
	return TRUE;
}

BOOL LDViewPreferences::doDrawGroupCheckBox(HWND hWnd, HTHEME hTheme,
											LPDRAWITEMSTRUCT drawItemStruct)
{
	HDC hDrawItemDC = drawItemStruct->hDC;
	bool bIsPressed = (drawItemStruct->itemState & ODS_SELECTED) != 0;
	bool bIsFocused = (drawItemStruct->itemState & ODS_FOCUS) != 0;
	bool bDrawFocusRect = (drawItemStruct->itemState & ODS_NOFOCUSRECT) == 0;
	bool bIsDisabled = (drawItemStruct->itemState & ODS_DISABLED) != 0;
	bool bIsChecked = checkStates[hWnd];
	bool bHidePrefix =
		(SendMessage(hWnd, WM_QUERYUISTATE, 0, 0) & UISF_HIDEACCEL) != 0;
	RECT itemRect = drawItemStruct->rcItem;
	ucstring title;

	windowGetText(hWnd, title);
	SetBkMode(hDrawItemDC, TRANSPARENT);
	// Prepare draw... paint button background
	if (CUIThemes::isThemeLibLoaded() && hTheme)
	{
		DWORD state;
		DWORD textFlags = DT_LEFT;
		SIZE boxSize;
		RECT boxRect = itemRect;
		RECT textRect;

		if (bIsPressed)
		{
			if (bIsChecked)
			{
				state = CBS_CHECKEDPRESSED;
			}
			else
			{
				state = CBS_UNCHECKEDPRESSED;
			}
		}
		else if (!bIsDisabled)
		{
			if (hMouseOverButton == hWnd)
			{
				if (bIsChecked)
				{
					state = CBS_CHECKEDHOT;
				}
				else
				{
					state = CBS_UNCHECKEDHOT;
				}
			}
			else
			{
				if (bIsChecked)
				{
					state = CBS_CHECKEDNORMAL;
				}
				else
				{
					state = CBS_UNCHECKEDNORMAL;
				}
			}
		}
		else
		{
			state = CBS_UNCHECKEDDISABLED;
		}
		if (bHidePrefix)
		{
			textFlags |= DT_HIDEPREFIX;
		}
		CUIThemes::getThemePartSize(hTheme, hDrawItemDC, BP_CHECKBOX, state, NULL,
			TS_TRUE, &boxSize);
		boxRect.right = itemRect.left + boxSize.cx;
		CUIThemes::drawThemeParentBackground(hWnd, hDrawItemDC, &itemRect);
		CUIThemes::drawThemeBackground(hTheme, hDrawItemDC, BP_CHECKBOX, state,
			&boxRect, NULL);
		CUIThemes::getThemeTextExtent(hTheme, hDrawItemDC, BP_CHECKBOX, state,
			title.c_str(), -1, textFlags, &itemRect, &textRect);
		OffsetRect(&textRect, boxSize.cx + 3, 1);
		// Draw the focus rect
		if (bIsFocused && bDrawFocusRect)
		{
			RECT focusRect = textRect;

			InflateRect(&focusRect, 1, 1);
			DrawFocusRect(hDrawItemDC, &focusRect);
		}
		// All this so that we can draw the text in the font and color of the
		// group box text instead of the check box text.  Here's where we do
		// that.
		if (bIsDisabled)
		{
			CUIThemes::drawThemeText(hTheme, hDrawItemDC, BP_CHECKBOX, CBS_DISABLED,
				title.c_str(), -1, textFlags, NULL, &textRect);
		}
		else
		{
			CUIThemes::drawThemeText(hTheme, hDrawItemDC, BP_GROUPBOX, GBS_NORMAL,
				title.c_str(), -1, textFlags, NULL, &textRect);
		}
	}
	return TRUE;
}

BOOL LDViewPreferences::doDrawColorButton(HWND hDlg, HWND hWnd, HTHEME hTheme,
										  LPDRAWITEMSTRUCT drawItemStruct)
{
	HDC hDrawItemDC = drawItemStruct->hDC;
	bool bIsPressed = (drawItemStruct->itemState & ODS_SELECTED) != 0;
	bool bIsFocused = (drawItemStruct->itemState & ODS_FOCUS) != 0;
	bool bDrawFocusRect = (drawItemStruct->itemState & ODS_NOFOCUSRECT) == 0;
	RECT itemRect = drawItemStruct->rcItem;
	HBITMAP hBitmap;

	SetBkMode(hDrawItemDC, TRANSPARENT);
	// Prepare draw... paint button background
	if (CUIThemes::isThemeLibLoaded() && hTheme)
	{
		DWORD state = (bIsPressed) ? PBS_PRESSED : PBS_NORMAL;

		if (state == PBS_NORMAL)
		{
			if (bIsFocused)
			{
				state = PBS_DEFAULTED;
			}
			if (hMouseOverButton == hWnd)
			{
				state = PBS_HOT;
			}
		}
		CUIThemes::drawThemeBackground(hTheme, hDrawItemDC, BP_PUSHBUTTON, state,
			&itemRect, NULL);
	}
	else
	{
		if (bIsFocused)
		{
			HBRUSH br = CreateSolidBrush(RGB(0,0,0));  
			FrameRect(hDrawItemDC, &itemRect, br);
			InflateRect(&itemRect, -1, -1);
			DeleteObject(br);
		} // if		

		COLORREF crColor = GetSysColor(COLOR_BTNFACE);

		HBRUSH	brBackground = CreateSolidBrush(crColor);

		FillRect(hDrawItemDC, &itemRect, brBackground);

		DeleteObject(brBackground);

		// Draw pressed button
		if (bIsPressed)
		{
			HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
			FrameRect(hDrawItemDC, &itemRect, brBtnShadow);
			DeleteObject(brBtnShadow);
		}

		else // ...else draw non pressed button
		{
			UINT uState = DFCS_BUTTONPUSH |
                  ((hMouseOverButton == hWnd) ? DFCS_HOT : 0) |
                  ((bIsPressed) ? DFCS_PUSHED : 0);

			DrawFrameControl(hDrawItemDC, &itemRect, DFC_BUTTON, uState);
		} // else
	}

	// Draw the focus rect
	if (bIsFocused && bDrawFocusRect)
	{
		RECT focusRect = itemRect;

		InflateRect(&focusRect, -3, -3);
		DrawFocusRect(hDrawItemDC, &focusRect);
	}

	// Draw the bitmap
	hBitmap = CUIDialog::buttonGetBitmap(hDlg, drawItemStruct->CtlID);
	if (hBitmap)
	{
		SIZE bitmapSize;

		if (GetBitmapDimensionEx(hBitmap, &bitmapSize))
		{
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hButtonColorDC,
				hBitmap);
			RECT clientRect;
			RECT contentRect;
			int lwidth;
			int lheight;

			GetClientRect(hWnd, &clientRect);
			if (CUIThemes::isThemeLibLoaded() && hTheme)
			{
				CUIThemes::getThemeBackgroundContentRect(hTheme, NULL,
					BP_PUSHBUTTON, PBS_HOT, &clientRect, &contentRect);
			}
			else
			{
				contentRect = clientRect;
			}
			lwidth = contentRect.right - contentRect.left;
			lheight = contentRect.bottom - contentRect.top;
			BitBlt(hDrawItemDC, contentRect.left + (lwidth - bitmapSize.cx) / 2,
				contentRect.top + (lheight - bitmapSize.cy) / 2, bitmapSize.cx,
				bitmapSize.cy, hButtonColorDC, 0, 0, SRCCOPY);
			SelectObject(hButtonColorDC, hOldBitmap);
		}
	}
	return TRUE;
}

BOOL LDViewPreferences::doDrawItem(HWND hDlg, int itemId,
								   LPDRAWITEMSTRUCT drawItemStruct)
{
	HWND hWnd = GetDlgItem(hDlg, itemId);

	if (itemId == IDC_DEFAULT_COLOR || itemId == IDC_BACKGROUND_COLOR)
	{
		return doDrawColorButton(hDlg, hWnd, hButtonTheme, drawItemStruct);
	}
	else
	{
		switch (itemId)
		{
		case IDC_WIREFRAME:
		case IDC_BFC:
		case IDC_HIGHLIGHTS:
		case IDC_LIGHTING:
		case IDC_STEREO:
		case IDC_CUTAWAY:
		case IDC_PRIMITIVE_SUBSTITUTION:
			return doDrawGroupCheckBox(hWnd, hButtonTheme, drawItemStruct);
			break;
		default:
			return doDrawIconPushButton(hWnd, hButtonTheme, drawItemStruct);
			break;
		}
	}
	return FALSE;
}

INT_PTR LDViewPreferences::dialogProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		if (CUIThemes::isThemeLibLoaded())
		{
			if (hMouseOverButton)
			{
				InvalidateRect(hMouseOverButton, NULL, TRUE);
				hMouseOverButton = NULL;
			}
		}
		break;
	case WM_DRAWITEM:
		return doDrawItem(hDlg, (int)wParam, (LPDRAWITEMSTRUCT)lParam);
		break;
	}
	return CUIPropertySheet::dialogProc(hDlg, message, wParam, lParam);
}

TCFloat LDViewPreferences::getMinFov(void)
{
	return 0.1f;
}

TCFloat LDViewPreferences::getMaxFov(void)
{
	return 90.0f;
}

void LDViewPreferences::checkLightVector(void)
{
	if (hEffectsPage)
	{
		LDPreferences::LightDirection selectedDirection =
			getSelectedLightDirection();
		LDPreferences::LightDirection lightDirection =
			ldPrefs->getLightDirection();

		if (selectedDirection != lightDirection)
		{
			int lightDirButton =
				lightDirIndexToId[(int)lightDirection - 1];

			uncheckLightDirections();
			if ((int)selectedDirection > 0)
			{
				InvalidateRect(lightAngleButtons[(int)selectedDirection - 1], NULL,
					TRUE);
			}
			if (lightDirButton != 0)
			{
				checkStates[GetDlgItem(hEffectsPage, lightDirButton)] = true;
				InvalidateRect(lightAngleButtons[(int)lightDirection - 1], NULL,
					TRUE);
			}
		}
	}
}
