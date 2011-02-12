#include "SSConfigure.h"
#include <shlobj.h>
#include "LDViewPreferences.h"
#include "LDViewWindow.h"
#include "LDVExtensionsSetup.h"
#include "Resource.h"
#include <LDLib/LDPreferences.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/myString.h>
#include <TCFoundation/TCLocalStrings.h>
#include <commctrl.h>
#include <LDLib/LDUserDefaultsKeys.h>

#include <stdio.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

SSConfigure::SSConfigure(HINSTANCE hInstance):
	LDViewPreferences(hInstance),
//	CUIPropertySheet("LDView Screensaver Settings", hInstance),
	hSSPage(NULL),
	ssFilename(NULL),
	ssDirectory(NULL)
{
	char *installPath = SSConfigure::getInstallPath();

	if (installPath)
	{
		strcpy(ldviewPath, installPath);
		delete installPath;
	}
	generalPageNumber++;
	geometryPageNumber++;
	effectsPageNumber++;
	primitivesPageNumber++;
	prefSetsPageNumber++;
	updatesPageNumber++;
	loadSSSettings();
	// Our superclass already applied, but we changed at least one setting, so
	// re-apply.
	ldPrefs->applySettings();
}

SSConfigure::~SSConfigure(void)
{
}

void SSConfigure::dealloc(void)
{
	delete ssFilename;
	CUIPropertySheet::dealloc();
}

void SSConfigure::loadDefaultSSSettings(void)
{
	delete ssFilename;
	ssFilename = defaultFilename();
	delete ssDirectory;
	ssDirectory = defaultDir();
	ssSize = DEFAULT_SS_SIZE;
	ssSpeed = DEFAULT_SS_SPEED;
	ssRotationSpeed = DEFAULT_SS_ROT_SPEED;
	ssFileMode = DEFAULT_SS_FILE_MODE;
	ssSleepWorkaround = DEFAULT_SS_SLEEP_WORKAROUND;
	ssRandomPrefSet = DEFAULT_SS_RANDOM_PREF_SET;
	ldPrefs->setDefaultZoom(1.0f / 1.01f);
	//defaultZoom = 1.0f / 1.01f;
}

void SSConfigure::loadSSSettings(void)
{
	char *oldString = ssFilename;

	loadDefaultSSSettings();
	ssFilename = TCUserDefaults::stringForKey(SS_FILENAME_KEY, ssFilename);
	if (ssFilename != oldString)
	{
		delete oldString;
	}
	oldString = ssDirectory;
	ssDirectory = TCUserDefaults::stringForKey(SS_DIRECTORY_KEY, ssDirectory);
	if (ssDirectory != oldString)
	{
		delete oldString;
	}
	ssSize = TCUserDefaults::longForKey(SS_SIZE_KEY, ssSize);
	ssSpeed = TCUserDefaults::longForKey(SS_SPEED_KEY, ssSpeed);
	ssRotationSpeed = TCUserDefaults::longForKey(SS_ROTATION_SPEED_KEY,
		ssRotationSpeed);
	ssFileMode = TCUserDefaults::longForKey(SS_FILE_MODE_KEY, ssFileMode);
	ssSleepWorkaround = TCUserDefaults::longForKey(SS_SLEEP_WORKAROUND_KEY,
		ssSleepWorkaround) != 0;
	ssRandomPrefSet = TCUserDefaults::longForKey(SS_RANDOM_PREF_SET_KEY,
		ssRandomPrefSet, false) != 0;
}

INT_PTR SSConfigure::run(void)
{
	LDVExtensionsSetup::setup(hInstance);
	addPage(IDD_SS_SETTINGS);
	return LDViewPreferences::run();
}

DWORD SSConfigure::doClick(HWND hPage, int controlId, HWND controlHWnd)
{
	if (hPage == hSSPage)
	{
		switch (controlId)
		{
		case IDC_SS_FILENAME_RADIO:
			ssFileMode = 0;
			updateFileControls();
			break;
		case IDC_SS_DIRECTORY_RADIO:
			ssFileMode = 1;
			updateFileControls();
			break;
		case IDC_SS_BROWSE_FILE_BUTTON:
			if (doBrowseFilename())
			{
				ssFileMode = 0;
				updateFileControls(false);
			}
			break;
		case IDC_SS_BROWSE_DIR_BUTTON:
			if (doBrowseDirectory())
			{
				ssFileMode = 1;
				updateFileControls(false);
			}
			break;
		case IDC_SS_RESET:
			loadDefaultSSSettings();
			setupSSPage();
			enableApply(hSSPage);
			break;
		}
		return 0;
	}
	else
	{
		return LDViewPreferences::doClick(hPage, controlId, controlHWnd);
	}
}

/*
BOOL SSConfigure::doDialogCommand(HWND hDlg, int controlId, int notifyCode,
								  HWND controlHWnd)
{
	if (hDlg == hSSPage)
	{
		return TRUE;
	}
	return LDViewPreferences::doDialogCommand(hDlg, controlId, notifyCode,
		controlHWnd);
}
*/

int CALLBACK SSConfigure::pathBrowserCallback(HWND hwnd, UINT uMsg,
											  LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_SELCHANGED)
	{
		char path[MAX_PATH+10];
		ITEMIDLIST* itemIdList = (ITEMIDLIST*)lParam;

		// For some reason, computers on the network are considered directories,
		// and the ok button is enabled for them.  We don't want to allow that,
		// and if one is selected, the following method will fail.
		if (!SHGetPathFromIDList(itemIdList, path))
		{
			SendMessage(hwnd, BFFM_ENABLEOK, 0, 0);
		}
	}
	else if (lpData && uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

bool SSConfigure::doBrowseDirectory(void)
{
	BROWSEINFO browseInfo;
	char displayName[MAX_PATH];
	LPITEMIDLIST itemIdList;

	browseInfo.hwndOwner = hPropSheet; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = TCLocalStrings::get("SSModelDirPrompt");
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)ssDirectory;
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		char path[MAX_PATH+10];

		if (SHGetPathFromIDList(itemIdList, path))
		{
			stripTrailingPathSeparators(path);
			SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_FIELD, WM_SETTEXT, 0,
				(LPARAM)path);
			return true;
		}
	}
	return false;
}

bool SSConfigure::doBrowseFilename(void)
{
	OPENFILENAME openStruct;
	char fileTypes[1024];
	char openFilename[1024] = "";
	char* initialDir =
		TCUserDefaults::stringForKey(SS_LAST_OPEN_PATH_KEY, NULL, false);
	bool retValue = false;

	if (!initialDir)
	{
		initialDir = LDViewWindow::getLDrawDir();
	}
	if (initialDir)
	{
		memset(fileTypes, 0, 2);
		addFileType(fileTypes, ls("LDrawFileTypes"), "*.ldr;*.dat;*.mpd");
		addFileType(fileTypes, ls("LDrawModelFileTypes"), "*.ldr;*.dat");
		addFileType(fileTypes, ls("LDrawMpdFileTypes"), "*.mpd");
		addFileType(fileTypes, ls("AllFilesTypes"), "*.*");
		memset(&openStruct, 0, sizeof(OPENFILENAME));
		openStruct.lStructSize = getOpenFilenameSize(false);
		openStruct.hwndOwner = hWindow;
		openStruct.lpstrFilter = fileTypes;
		openStruct.nFilterIndex = 1;
		openStruct.lpstrFile = openFilename;
		openStruct.nMaxFile = 1024;
		openStruct.lpstrInitialDir = initialDir;
		openStruct.lpstrTitle = TCLocalStrings::get("SelectModelFile");
		openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
			OFN_HIDEREADONLY;
		openStruct.lpstrDefExt = "ldr";
		if (GetOpenFileName(&openStruct))
		{
			LDViewWindow::setLastOpenFile(openStruct.lpstrFile,
				SS_LAST_OPEN_PATH_KEY);
			SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_FIELD, WM_SETTEXT, 0,
				(LPARAM)openStruct.lpstrFile);
			enableApply(hSSPage);
			retValue = true;
		}
		delete initialDir;
	}
	return retValue;
}

void SSConfigure::applyChanges(void)
{
	char filename[1024];

	ssSize = (long)SendDlgItemMessage(hSSPage, IDC_SS_SIZE_SLIDER, TBM_GETPOS,
		0, 0);
	ssSpeed = (long)SendDlgItemMessage(hSSPage, IDC_SS_SPEED_SLIDER, TBM_GETPOS, 0,
		0);
	ssRotationSpeed = (long)SendDlgItemMessage(hSSPage,
		IDC_SS_ROTATION_SPEED_SLIDER, TBM_GETPOS, 0, 0);
	if (ssFileMode == 0)
	{
		SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_FIELD, WM_GETTEXT, 1024,
			(LPARAM)filename);
		delete ssFilename;
		ssFilename = copyString(filename);
	}
	else
	{
		SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_FIELD, WM_GETTEXT, 1024,
			(LPARAM)filename);
		delete ssDirectory;
		ssDirectory = copyString(filename);
	}
	ssSleepWorkaround = SendDlgItemMessage(hSSPage, IDC_SS_SLEEP, BM_GETCHECK,
		0, 0) != 0;
	ssRandomPrefSet = SendDlgItemMessage(hSSPage, IDC_SS_RANDOM_PREF_SET,
		BM_GETCHECK, 0, 0) != 0;

	TCUserDefaults::setLongForKey(ssSize, SS_SIZE_KEY);
	TCUserDefaults::setLongForKey(ssSpeed, SS_SPEED_KEY);
	TCUserDefaults::setLongForKey(ssRotationSpeed, SS_ROTATION_SPEED_KEY);
	TCUserDefaults::setStringForKey(ssFilename, SS_FILENAME_KEY);
	TCUserDefaults::setStringForKey(ssDirectory, SS_DIRECTORY_KEY);
	TCUserDefaults::setLongForKey(ssFileMode, SS_FILE_MODE_KEY);
	TCUserDefaults::setLongForKey(ssSleepWorkaround, SS_SLEEP_WORKAROUND_KEY);
	TCUserDefaults::setLongForKey(ssRandomPrefSet, SS_RANDOM_PREF_SET_KEY,
		false);
	LDViewPreferences::applyChanges();
}

BOOL SSConfigure::doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification)
{
	if (hDlg == hSSPage && notification->code == NM_RELEASEDCAPTURE)
	{
		enableApply(hSSPage);
		return FALSE;
	}
	return LDViewPreferences::doDialogNotify(hDlg, controlId, notification);
}

char* SSConfigure::defaultFilename(void)
{
	char *path = LDViewWindow::getLDrawDir();
	char buf[2048];

	sprintf(buf, "%s\\%s", path, "models\\car.dat");
	delete path;
	return copyString(buf);
}

char* SSConfigure::defaultDir(void)
{
	char *path = LDViewWindow::getLDrawDir();
	char buf[2048];

	sprintf(buf, "%s\\%s", path, "models");
	delete path;
	return copyString(buf);
}

void SSConfigure::updateFileControls(bool includePaths)
{
	if (ssFileMode == 0)
	{
		SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_RADIO, BM_SETCHECK, 1, 0);
		SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_RADIO, BM_SETCHECK, 0, 0);
		if (includePaths)
		{
			SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_FIELD, WM_SETTEXT, 0,
				(LPARAM)ssFilename);
		}
		SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_FIELD, WM_SETTEXT, 0,
			(LPARAM)"");
	}
	else
	{
		SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_RADIO, BM_SETCHECK, 0, 0);
		SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_RADIO, BM_SETCHECK, 1, 0);
		SendDlgItemMessage(hSSPage, IDC_SS_FILENAME_FIELD, WM_SETTEXT, 0,
			(LPARAM)"");
		if (includePaths)
		{
			SendDlgItemMessage(hSSPage, IDC_SS_DIRECTORY_FIELD, WM_SETTEXT, 0,
				(LPARAM)ssDirectory);
		}
	}
}

void SSConfigure::setupSSPage(void)
{
	OSVERSIONINFO osvi;
	bool runningOnNT;
	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	runningOnNT = (GetVersionEx(&osvi) &&
		osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);

	hSSPage = hwndArray->pointerAtIndex(0);
	updateFileControls();
	setupDialogSlider(hSSPage, IDC_SS_SIZE_SLIDER, 100, 1000, 50, ssSize);
	setupDialogSlider(hSSPage, IDC_SS_SPEED_SLIDER, 1, 20, 1, ssSpeed);
	setupDialogSlider(hSSPage, IDC_SS_ROTATION_SPEED_SLIDER, 1, 1000, 50,
		ssRotationSpeed);
	if (!runningOnNT)
	{
		int powerSaveTimeout;

		if (SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT, 0, &powerSaveTimeout,
			0))
		{
			// If we got here, we aren't running in NT, and we successfully read
			// the power save timeout value, which should mean we're running on
			// Windows 98.
			EnableWindow(GetDlgItem(hSSPage, IDC_SS_SLEEP), TRUE);
			SendDlgItemMessage(hSSPage, IDC_SS_SLEEP, BM_SETCHECK,
				ssSleepWorkaround ? 1 : 0, 0);
		}
	}
	SendDlgItemMessage(hSSPage, IDC_SS_RANDOM_PREF_SET, BM_SETCHECK,
		ssRandomPrefSet ? 1 : 0, 0);
}

void SSConfigure::setupPage(int pageNumber)
{
	switch (pageNumber)
	{
	case 0:
		setupSSPage();
		break;
	default:
		LDViewPreferences::setupPage(pageNumber);
		break;
	}
}

void SSConfigure::setupGeneralPage(void)
{
	LDViewPreferences::setupGeneralPage();
	EnableWindow(GetDlgItem(hGeneralPage, IDC_FRAME_RATE), FALSE);
	EnableWindow(GetDlgItem(hGeneralPage, IDC_SHOW_ERRORS), FALSE);
	EnableWindow(GetDlgItem(hGeneralPage, IDC_FS_REFRESH), FALSE);
}

void SSConfigure::setupPrefSetsPage(void)
{
	LDViewPreferences::setupPrefSetsPage();
	ShowWindow(hPrefSetHotKeyButton, SW_HIDE);
}

void SSConfigure::enableCutaway(void)
{
	disableCutaway();
}

void SSConfigure::disableCutaway(void)
{
	LDViewPreferences::disableCutaway();
	EnableWindow(GetDlgItem(hEffectsPage, IDC_CUTAWAY), FALSE);
}

void SSConfigure::enableStereo(void)
{
	disableStereo();
}

void SSConfigure::disableStereo(void)
{
	LDViewPreferences::disableStereo();
	EnableWindow(GetDlgItem(hEffectsPage, IDC_STEREO), FALSE);
}

/*
float SSConfigure::getMaxFov(void)
{
	return 90.0f;
}
*/

DWORD SSConfigure::getPageDialogID(HWND hDlg)
{
	if (hDlg == hSSPage)
	{
		return IDD_SS_SETTINGS;
	}
	else
	{
		return LDViewPreferences::getPageDialogID(hDlg);
	}
}

char *SSConfigure::getInstallPath(void)
{
	char *installPath;
	char *oldAppName = copyString(TCUserDefaults::getAppName());
	char *sessionName;

	TCUserDefaults::setAppName("Travis Cobbs/LDView");
	installPath = TCUserDefaults::stringForKey(INSTALL_PATH_4_1_KEY, NULL, false);
	if (installPath == NULL)
	{
		installPath = TCUserDefaults::stringForKey(INSTALL_PATH_KEY, NULL, false);
	}
	TCUserDefaults::setAppName(oldAppName);
	sessionName =
		TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	if (sessionName && sessionName[0])
	{
		TCUserDefaults::setSessionName(sessionName, NULL, false);
	}
	delete sessionName;
	delete oldAppName;
	return installPath;
}
