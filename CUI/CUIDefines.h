#ifndef __CUIDEFINES_H__
#define __CUIDEFINES_H__

#ifdef _BUILDING_CUI
#define CUIExport __declspec(dllexport)
#elif defined _BUILDING_CUI_LIB || defined _TC_STATIC
#define CUIExport
#else
#define CUIExport __declspec(dllimport)
#endif

#endif // __CUIDEFINES_H__
