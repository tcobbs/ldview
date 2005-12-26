// EnglishUS.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <stdio.h>

HINSTANCE g_hModule;
static bool versionsPopulated = false;
static DWORD versionMS = 0;
static DWORD versionLS = 0;

// populateVersions
//
// This function reads the DLL's Product Version, and stores the MS and LS
// DWORDs into global variables for use in the getCUIAppLanguageModuleVersion
// function.  It also sets the versionsPopulated variable to true so that it
// will only load the information once.
void populateVersions(void)
{
	if (!versionsPopulated)
	{
		char moduleFilename[1024];

		if (GetModuleFileName(g_hModule, moduleFilename,
			sizeof(moduleFilename)) > 0)
		{
			DWORD zero;
			DWORD versionInfoSize = GetFileVersionInfoSize(moduleFilename,
				&zero);
			
			if (versionInfoSize > 0)
			{
				BYTE *versionInfo = new BYTE[versionInfoSize];

				if (GetFileVersionInfo(moduleFilename, NULL, versionInfoSize,
					versionInfo))
				{
					VS_FIXEDFILEINFO *fixedVersionInfo;
					UINT versionLength;

					if (VerQueryValue(versionInfo, "\\",
						(void**)&fixedVersionInfo, &versionLength))
					{
						versionMS = fixedVersionInfo->dwProductVersionMS;
						versionLS = fixedVersionInfo->dwProductVersionLS;
						versionsPopulated = true;
					}
				}
				delete versionInfo;
			}
		}
	}
}

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  /*ul_reason_for_call*/,
                       LPVOID /*lpReserved*/
					 )
{
	g_hModule = (HINSTANCE)hModule;
	populateVersions();
    return TRUE;
}

extern "C"
{

// getCUIAppLanguageModuleVersion
//
// Parameters:
//   appName:	The name of the app requesting the version number.  This must
//              match the expected name ("LDView").
//   index:		The index of the desired version number.  0 is the most
//				significant DWORD; 1 is the least significant DWORD; all other
//				values are errors.
//
// Return Value:
//   Returns the requested version info, or 0 if there is an error.
//
// You really shouldn't change this function.  It is designed to return the
// DLL's Product Version.  The DLL's product version needs to match LDView's
// product version in order for the language module to work properly.  When you
// update the DLL for a new LDView release, update its version resource to match
// the LDView release it's designed for.
__declspec(dllexport) DWORD getCUIAppLanguageModuleVersion(const char *appName,
														   int index)
{
	if (strcmp(appName, "LDView") == 0)
	{
		if (index == 0)
		{
			return versionMS;
		}
		else if (index == 1)
		{
			return versionLS;
		}
	}
	return 0;
}

}
