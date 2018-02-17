#ifndef __CUIMODULEHOLDER_H__
#define __CUIMODULEHOLDER_H__

#include <CUI/CUIWindow.h>

class CUIExport CUIModuleHolder
{
public:
	CUIModuleHolder(LPCTSTR libraryName)
	{
		m_hModule = LoadLibrary(libraryName);
	}
	~CUIModuleHolder(void)
	{
		if (m_hModule != NULL)
		{
			FreeLibrary(m_hModule);
		}
	}
protected:
	HMODULE m_hModule;
};

#endif // __CUIMODULEHOLDER_H__