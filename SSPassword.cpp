#include "SSPassword.h"
#include <windows.h>
#include <windowsx.h> 
#include <regstr.h> 

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

void debugOut(char *fmt, ...);

SSPassword::SSPassword(void)
	:hInstPwdDLL(NULL),
	verifyPasswordProc(NULL),
	checkingPassword(false),
	lastCheckTime(0)
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(push)
#pragma warning(disable:4996)
	runningOnNT = (GetVersionEx(&osvi) &&
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
#pragma warning(pop)
	loadPasswordDLL();
}

SSPassword::~SSPassword(void)
{
}

void SSPassword::dealloc(void)
{
	unloadPasswordDLL();
	TCObject::dealloc();
}

void SSPassword::loadPasswordDLL(void)
{
	HKEY hKey;

	if (runningOnNT)
	{
		return;
	}

	if (hInstPwdDLL)
	{
		unloadPasswordDLL();
	}

	// look in registry to see if password turned on, otherwise don't
	// bother to load password handler DLL
	if (RegOpenKey(HKEY_CURRENT_USER, REGSTR_PATH_SCREENSAVE, &hKey) ==
		ERROR_SUCCESS)
	{
		DWORD dwVal,dwSize=sizeof(dwVal);

		if ((RegQueryValueEx(hKey, REGSTR_VALUE_USESCRPASSWORD,
			NULL,NULL,(BYTE *) &dwVal,&dwSize) == ERROR_SUCCESS)
			&& dwVal)
		{

			// try to load the DLL that contains password proc.
			hInstPwdDLL = LoadLibrary(_UC("PASSWORD.CPL"));
			if (hInstPwdDLL)
			{
				verifyPasswordProc = (VERIFYPWDPROC) GetProcAddress(hInstPwdDLL,
					"VerifyScreenSavePwd");

				if (verifyPasswordProc)
				{
					hogMachine(TRUE);
				}
				else
				{
					unloadPasswordDLL();
				}
			}
		}

		RegCloseKey(hKey);
	}
}

void SSPassword::unloadPasswordDLL(void)
{
	if (runningOnNT)
	{
		return;
	}

	if (hInstPwdDLL)
	{
		FreeLibrary(hInstPwdDLL);
		hInstPwdDLL = NULL;

		if(verifyPasswordProc)
		{
			verifyPasswordProc = NULL;
			hogMachine(FALSE);
		}
	}
}

//----------------------------------------------------------------------------
// Local reboot and hotkey control (on Win95/98)
void SSPassword::hogMachine(BOOL value)
{
    BOOL dummy;

    //
    // NT is always secure, therefore we don't need to call this on Cairo/NT
    //
    if (!runningOnNT)
	{
        SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, value, &dummy, 0);
    }
}

bool SSPassword::verifyPassword(HWND hWindow)
{
	debugOut("SSPassword::verifyPassword\n");
	if (runningOnNT)
	{
		return true;
	}
	else if (verifyPasswordProc)
	{
		if (!checkingPassword)
		{
			DWORD curTime = GetTickCount();

			debugOut("SSPassword::verifyPassword %d %d %d\n", lastCheckTime,
				curTime, curTime - lastCheckTime);
			if (curTime - lastCheckTime < 200 && curTime >= lastCheckTime)
			{
//				lastCheckTime = curTime;
				return false;
			}
			debugOut("SSPassword::verifyPassword checking...\n");
			lastCheckTime = curTime;
			checkingPassword = true;
			ShowCursor(TRUE);
			if (verifyPasswordProc(hWindow))
			{
				ShowCursor(FALSE);
				checkingPassword = false;
				return true;
			}
			else
			{
				ShowCursor(FALSE);
				checkingPassword = false;
				lastCheckTime = GetTickCount();
				debugOut("SSPassword::verifyPassword failed\n");
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}
