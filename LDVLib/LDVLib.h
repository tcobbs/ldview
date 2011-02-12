#pragma once

#include <windows.h>

#ifdef BUILDING_LDVLIB
#define LDVExport __declspec(dllexport)
#else
#define LDVExport __declspec(dllimport)
#endif

typedef enum LDVTextureFilterType
{
	LDVTFTNearest,
	LDVTFTBilinear,
	LDVTFTTrilinear
} LDVTextureFilterType;

typedef enum LDVViewingAngle
{
	Default,
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom,
	Iso
} LDVViewingAngle;

#ifdef __cplusplus
extern "C" {
#endif // __c_plus_plus

LDVExport void *LDVInit(HWND hwnd);
// Use: Company/App format for app name; defaults will go into
// HKEY_CURRENT_USER\Software\Company\App
// Note: Use forward slash (/), NOT backslash (\).
LDVExport void SetRegistryAppName(const char *appName);
LDVExport BOOL LDVGLInit(void *pLDV);
LDVExport void LDVDeInit(void *pLDV);
LDVExport void LDVSetSize(void *pLDV, int width, int height);
LDVExport int LDVGetWidth(void *pLDV);
LDVExport int LDVGetHeight(void *pLDV);
LDVExport void LDVSetFilename(void *pLDV, const char *filename);
LDVExport BOOL LDVLoadModel(void *pLDV, BOOL resetViewpoint);
LDVExport void LDVUpdate(void *pLDV);
LDVExport void LDVSetBackgroundRGB(void *pLDV, int r, int g, int b);
LDVExport void LDVGetBackgroundRGB(void *pLDV, int *r, int *g, int *b);
LDVExport void LDVSetDefaultRGB(void *pLDV, int r, int g, int b,
	BOOL transparent);
LDVExport void LDVGetDefaultRGB(void *pLDV, int *r, int *g, int *b,
	BOOL *transparent);
LDVExport void LDVResetView(void *pLDV, LDVViewingAngle viewingAngle);
LDVExport void LDVEnableInput(void *pLDV, BOOL enable);
LDVExport BOOL LDVIsInputEnabled(void *pLDV);
LDVExport void LDVZoomToFit(void *pLDV);
LDVExport BOOL LDVHasFocus(void *pLDV);
LDVExport BOOL LDVGetUsesSpecular(void *pLDV);
LDVExport void LDVSetUsesSpecular(void *pLDV, BOOL value);
LDVExport float LDVGetSeamWidth(void *pLDV);
LDVExport void LDVSetSeamWidth(void *pLDV, float value);
LDVExport BOOL LDVGetShowsEdges(void *pLDV);
LDVExport void LDVSetShowsEdges(void *pLDV, BOOL value);
LDVExport BOOL LDVGetBlackEdges(void *pLDV);
LDVExport void LDVSetBlackEdges(void *pLDV, BOOL value);
LDVExport BOOL LDVGetShowsConditionalEdges(void *pLDV);
LDVExport void LDVSetShowsConditionalEdges(void *pLDV, BOOL value);
LDVExport BOOL LDVGetLowQualityStuds(void *pLDV);
LDVExport void LDVSetLowQualityStuds(void *pLDV, BOOL value);
LDVExport BOOL LDVGetLighting(void *pLDV);
LDVExport void LDVSetLighting(void *pLDV, BOOL value);
LDVExport BOOL LDVGetSubduedLighting(void *pLDV);
LDVExport void LDVSetSubduedLighting(void *pLDV, BOOL value);
LDVExport BOOL LDVGetTextureStuds(void *pLDV);
LDVExport void LDVSetTextureStuds(void *pLDV, BOOL value);
LDVExport LDVTextureFilterType LDVGetTextureFilterType(void *pLDV);
LDVExport void LDVSetTextureFilterType(void *pLDV, LDVTextureFilterType value);
LDVExport float LDVGetAnisoLevel(void *pLDV);
LDVExport void LDVSetAnisoLevel(void *pLDV, float value);
LDVExport float LDVGetMaxAnisoLevel(void);
LDVExport BOOL LDVGetDrawWireframe(void *pLDV);
LDVExport void LDVSetDrawWireframe(void *pLDV, BOOL value);
LDVExport BOOL LDVGetWireframeFog(void *pLDV);
LDVExport void LDVSetWireframeFog(void *pLDV, BOOL value);
LDVExport BOOL LDVGetRemoveHiddenLines(void *pLDV);
LDVExport void LDVSetRemoveHiddenLines(void *pLDV, BOOL value);
LDVExport BOOL LDVGetPrimitiveSubstitution(void *pLDV);
LDVExport void LDVSetPrimitiveSubstitution(void *pLDV, BOOL value);
LDVExport BOOL LDVGetSmoothing(void *pLDV);
LDVExport void LDVSetSmoothing(void *pLDV, BOOL value);
LDVExport BOOL LDVGetLineSmoothing(void *pLDV);
LDVExport void LDVSetLineSmoothing(void *pLDV, BOOL value);
LDVExport BOOL LDVGetEdgesOnly(void *pLDV);
LDVExport void LDVSetEdgesOnly(void *pLDV, BOOL value);
LDVExport BOOL LDVGetBFC(void *pLDV);
LDVExport void LDVSetBFC(void *pLDV, BOOL value);
LDVExport BOOL LDVGetRedBackFaces(void *pLDV);
LDVExport void LDVSetRedBackFaces(void *pLDV, BOOL value);
LDVExport BOOL LDVGetGreenFrontFaces(void *pLDV);
LDVExport void LDVSetGreenFrontFaces(void *pLDV, BOOL value);
LDVExport BOOL LDVGetBlueNeutralFaces(void *pLDV);
LDVExport void LDVSetBlueNeutralFaces(void *pLDV, BOOL value);
LDVExport BOOL LDVGetCheckPartsTracker(void *pLDV);
LDVExport void LDVSetCheckPartsTracker(void *pLDV, BOOL value);
LDVExport BOOL LDVGetShowsAxes(void *pLDV);
LDVExport void LDVSetShowsAxes(void *pLDV, BOOL value);
LDVExport BOOL LDVGetShowsBoundingBox(void *pLDV);
LDVExport void LDVSetShowsBoundingBox(void *pLDV, BOOL value);
LDVExport BOOL LDVGetBoundingBoxesOnly(void *pLDV);
LDVExport void LDVSetBoundingBoxesOnly(void *pLDV, BOOL value);
LDVExport BOOL LDVGetOBI(void *pLDV);
LDVExport void LDVSetOBI(void *pLDV, BOOL value);
LDVExport BOOL LDVGetRandomColors(void *pLDV);
LDVExport void LDVSetRandomColors(void *pLDV, BOOL value);
LDVExport BOOL LDVGetTexmaps(void *pLDV);
LDVExport void LDVSetTexmaps(void *pLDV, BOOL value);
LDVExport float LDVGetFOV(void *pLDV);
LDVExport void LDVSetFOV(void *pLDV, float value);
LDVExport BOOL LDVGetShowsAllConditionalEdges(void *pLDV);
LDVExport void LDVSetShowsAllConditionalEdges(void *pLDV, BOOL value);

LDVExport void LDVSetLDrawDir(const char *path);

#ifdef __cplusplus
}
#endif // __c_plus_plus
