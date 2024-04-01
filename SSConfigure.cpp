#include "SSConfigure.h"
#include <CUI/CUIDialog.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <shlobj.h>
#pragma warning(pop)
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
	hSSPage(NULL)
{
	ucstring installPath = SSConfigure::getInstallPath();

	if (!installPath.empty())
	{
		ldviewPath = installPath;
	}
	generalPageNumber++;
	ldrawPageNumber = -1;
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
	CUIPropertySheet::dealloc();
}

void SSConfigure::loadDefaultSSSettings(void)
{
	ssFilename = defaultFilename();
	ssDirectory = defaultDir();
	ssSize = DEFAULT_SS_SIZE;
	ssSpeed = DEFAULT_SS_SPEED;
	ssRotationSpeed = DEFAULT_SS_ROT_SPEED;
	ssFileMode = DEFAULT_SS_FILE_MODE;
	ssSleepWorkaround = DEFAULT_SS_SLEEP_WORKAROUND;
	ssRandomPrefSet = DEFAULT_SS_RANDOM_PREF_SET;
	ldPrefs->setDefaultZoom(1.0f / 1.01f);
}

void SSConfigure::loadSSSettings(void)
{
	loadDefaultSSSettings();
	UCSTR tempString = TCUserDefaults::stringForKeyUC(SS_FILENAME_KEY,
		ssFilename.c_str());
	ssFilename = tempString;
	delete[] tempString;
	tempString = TCUserDefaults::stringForKeyUC(SS_DIRECTORY_KEY,
		ssDirectory.c_str());
	ssDirectory = tempString;
	delete[] tempString;
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

int CALLBACK SSConfigure::pathBrowserCallback(HWND hwnd, UINT uMsg,
											  LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_SELCHANGED)
	{
		UCCHAR path[MAX_PATH+10];
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
	UCCHAR displayName[MAX_PATH];
	LPITEMIDLIST itemIdList;

	browseInfo.hwndOwner = hPropSheet; //hWindow;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = displayName;
	browseInfo.lpszTitle = ls(_UC("SSModelDirPrompt"));
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	browseInfo.lpfn = pathBrowserCallback;
	browseInfo.lParam = (LPARAM)ssDirectory.c_str();
	browseInfo.iImage = 0;
	if ((itemIdList = SHBrowseForFolder(&browseInfo)) != NULL)
	{
		UCCHAR path[MAX_PATH+10];

		if (SHGetPathFromIDList(itemIdList, path))
		{
			stripTrailingPathSeparators(path);
			CUIDialog::windowSetText(hSSPage, IDC_SS_DIRECTORY_FIELD, path);
			return true;
		}
	}
	return false;
}

bool SSConfigure::doBrowseFilename(void)
{
	OPENFILENAME openStruct;
	UCCHAR fileTypes[1024];
	UCCHAR openFilename[1024] = _UC("");
	UCSTR lastOpenPath =
		TCUserDefaults::stringForKeyUC(SS_LAST_OPEN_PATH_KEY, NULL, false);
	bool retValue = false;
	ucstring initialDir;

	if (lastOpenPath != NULL)
	{
		initialDir = lastOpenPath;
		delete[] lastOpenPath;
	}
	else
	{
		utf8toucstring(initialDir, LDViewWindow::getLDrawDir());
	}
	if (!initialDir.empty())
	{
		memset(fileTypes, 0, 2 * sizeof(fileTypes[0]));
		addFileType(fileTypes, ls(_UC("LDrawFileTypes")), _UC("*.ldr;*.dat;*.mpd"));
		addFileType(fileTypes, ls(_UC("LDrawModelFileTypes")), _UC("*.ldr;*.dat"));
		addFileType(fileTypes, ls(_UC("LDrawMpdFileTypes")), _UC("*.mpd"));
		addFileType(fileTypes, ls(_UC("AllFilesTypes")), _UC("*.*"));
		memset(&openStruct, 0, sizeof(OPENFILENAME));
		openStruct.lStructSize = getOpenFilenameSize(false);
		openStruct.hwndOwner = hWindow;
		openStruct.lpstrFilter = fileTypes;
		openStruct.nFilterIndex = 1;
		openStruct.lpstrFile = openFilename;
		openStruct.nMaxFile = COUNT_OF(openFilename);
		openStruct.lpstrInitialDir = initialDir.c_str();
		openStruct.lpstrTitle = ls(_UC("SelectModelFile"));
		openStruct.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
			OFN_HIDEREADONLY;
		openStruct.lpstrDefExt = _UC("ldr");
		if (GetOpenFileName(&openStruct))
		{
			std::string utf8Filename;
			ucstringtoutf8(utf8Filename, openStruct.lpstrFile);
			LDViewWindow::setLastOpenFile(utf8Filename.c_str(),
				SS_LAST_OPEN_PATH_KEY);
			CUIDialog::windowSetText(hSSPage, IDC_SS_FILENAME_FIELD,
				openStruct.lpstrFile);
			enableApply(hSSPage);
			retValue = true;
		}
	}
	return retValue;
}

bool SSConfigure::applyChanges(void)
{
	ssSize = CUIDialog::trackBarGetPos(hSSPage, IDC_SS_SIZE_SLIDER);
	ssSpeed = CUIDialog::trackBarGetPos(hSSPage, IDC_SS_SPEED_SLIDER);
	ssRotationSpeed = CUIDialog::trackBarGetPos(hSSPage,
		IDC_SS_ROTATION_SPEED_SLIDER);
	if (ssFileMode == 0)
	{
		CUIDialog::windowGetText(hSSPage, IDC_SS_FILENAME_FIELD, ssFilename);
	}
	else
	{
		CUIDialog::windowGetText(hSSPage, IDC_SS_DIRECTORY_FIELD, ssDirectory);
	}
	ssSleepWorkaround = CUIDialog::buttonGetCheck(hSSPage, IDC_SS_SLEEP);
	ssRandomPrefSet = CUIDialog::buttonGetCheck(hSSPage,
		IDC_SS_RANDOM_PREF_SET);

	TCUserDefaults::setLongForKey(ssSize, SS_SIZE_KEY);
	TCUserDefaults::setLongForKey(ssSpeed, SS_SPEED_KEY);
	TCUserDefaults::setLongForKey(ssRotationSpeed, SS_ROTATION_SPEED_KEY);
	TCUserDefaults::setStringForKey(ssFilename.c_str(), SS_FILENAME_KEY);
	TCUserDefaults::setStringForKey(ssDirectory.c_str(), SS_DIRECTORY_KEY);
	TCUserDefaults::setLongForKey(ssFileMode, SS_FILE_MODE_KEY);
	TCUserDefaults::setLongForKey(ssSleepWorkaround, SS_SLEEP_WORKAROUND_KEY);
	TCUserDefaults::setLongForKey(ssRandomPrefSet, SS_RANDOM_PREF_SET_KEY,
		false);
	return LDViewPreferences::applyChanges();
}

BOOL SSConfigure::doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification)
{
#pragma warning(push)
#pragma warning(disable: 26454)
	if (hDlg == hSSPage && notification->code == NM_RELEASEDCAPTURE)
#pragma warning(pop)
	{
		enableApply(hSSPage);
		return FALSE;
	}
	return LDViewPreferences::doDialogNotify(hDlg, controlId, notification);
}

ucstring SSConfigure::defaultFilename(void)
{
	ucstring path = LDViewWindow::getLDrawDirUC();
	path += _UC("\\models\\car.dat");
	return path;
}

ucstring SSConfigure::defaultDir(void)
{
	ucstring path = LDViewWindow::getLDrawDirUC();
	path += _UC("\\models");
	return path;
}

void SSConfigure::updateFileControls(bool includePaths)
{
	if (ssFileMode == 0)
	{
		CUIDialog::buttonSetChecked(hSSPage, IDC_SS_FILENAME_RADIO, true);
		CUIDialog::buttonSetChecked(hSSPage, IDC_SS_DIRECTORY_RADIO, false);
		if (includePaths)
		{
			CUIDialog::windowSetText(hSSPage, IDC_SS_FILENAME_FIELD, ssFilename);
		}
		CUIDialog::windowSetText(hSSPage, IDC_SS_DIRECTORY_FIELD, _UC(""));
	}
	else
	{
		CUIDialog::buttonSetChecked(hSSPage, IDC_SS_FILENAME_RADIO, false);
		CUIDialog::buttonSetChecked(hSSPage, IDC_SS_DIRECTORY_RADIO, true);
		CUIDialog::windowSetText(hSSPage, IDC_SS_FILENAME_FIELD, _UC(""));
		if (includePaths)
		{
			CUIDialog::windowSetText(hSSPage, IDC_SS_DIRECTORY_FIELD, ssDirectory);
		}
	}
}

void SSConfigure::setupSSPage(void)
{
	hSSPage = hwndArray->pointerAtIndex(0);
	updateFileControls();
	CUIDialog::trackBarSetup(hSSPage, IDC_SS_SIZE_SLIDER, 100, 1000, 50, ssSize);
	CUIDialog::trackBarSetup(hSSPage, IDC_SS_SPEED_SLIDER, 1, 20, 1, ssSpeed);
	CUIDialog::trackBarSetup(hSSPage, IDC_SS_ROTATION_SPEED_SLIDER, 1, 1000, 50,
		ssRotationSpeed);
	CUIDialog::buttonSetChecked(hSSPage, IDC_SS_RANDOM_PREF_SET,
		ssRandomPrefSet);
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

ucstring SSConfigure::getInstallPath(void)
{
	UCSTR installPath;
	char *oldAppName = copyString(TCUserDefaults::getAppName());
	char *sessionName;

	TCUserDefaults::setAppName("Travis Cobbs/LDView");
	installPath = TCUserDefaults::stringForKeyUC(INSTALL_PATH_4_1_KEY, NULL,
		false);
	if (installPath == NULL)
	{
		installPath = TCUserDefaults::stringForKeyUC(INSTALL_PATH_KEY, NULL,
			false);
	}
	TCUserDefaults::setAppName(oldAppName);
	sessionName =
		TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	if (sessionName != NULL && sessionName[0])
	{
		TCUserDefaults::setSessionName(sessionName, NULL, false);
	}
	delete[] sessionName;
	delete[] oldAppName;
	ucstring retValue;
	if (installPath != NULL)
	{
		retValue = installPath;
		delete[] installPath;
	}
	return retValue;
}
