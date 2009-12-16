#pragma once

#include <windows.h>

#ifdef BUILDING_LDVLIB
#define LDVExport __declspec(dllexport)
#else
#define LDVExport __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif // __c_plus_plus

LDVExport void *LDVInit(HWND hwnd);
LDVExport BOOL LDVGLInit(void *pLDV);
LDVExport void LDVDeInit(void *pLDV);
LDVExport void LDVSetSize(void *pLDV, int width, int height);
LDVExport int LDVGetWidth(void *pLDV);
LDVExport int LDVGetHeight(void *pLDV);
LDVExport void LDVSetFilename(void *pLDV, const char *filename);
LDVExport BOOL LDVLoadModel(void *pLDV, BOOL resetViewpoint);
LDVExport void LDVUpdate(void *pLDV);
LDVExport void LDVSetBackgroundRGB(void *pLDV, int r, int g, int b);
LDVExport void LDVResetView(void *pLDV, int viewingAngle);
LDVExport void LDVEnableInput(void *pLDV, BOOL enable);
LDVExport BOOL LDVIsInputEnabled(void *pLDV);
LDVExport void LDVZoomToFit(void *pLDV);

LDVExport void LDVSetLDrawDir(const char *path);

#ifdef __cplusplus
}
#endif // __c_plus_plus
