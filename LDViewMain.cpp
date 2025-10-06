//#define NTDDI_VERSION NTDDI_WIN7
//#include <shobjidl.h>
#include <TCFoundation/TCDefines.h>
#include <windows.h>
#include <windowsx.h>
#include "ModelLoader.h"
#include "SSConfigure.h"
#include "WinWebClientPlugin.h"
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/mystring.h>
#include <CUI/CUIWindow.h>
#include "SSPreview.h"
#include "Resource.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>

//#include <TCFoundation/TCImage.h>
//#include <TCFoundation/TCJpegOptions.h>

#include <png.h>
#include <jpeglib.h>
#include <gl2ps.h>
#include <tinyxml.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

static bool debugToFile = false;

typedef BOOL (__stdcall *PFNATTACHCONSOLE)(DWORD dwProcessId);
typedef HRESULT (__stdcall *PFNDLLREGISTERSERVER)(void);

void createConsole(void)
{
	if (AllocConsole())
	{
		COORD size = {80, 1000};
		SMALL_RECT rect = {0, 0, 79, 24};
//		HANDLE hOrigStdOut;
		HANDLE hStdOut;
		SECURITY_ATTRIBUTES securityAttributes;

		securityAttributes.nLength = sizeof SECURITY_ATTRIBUTES;
		securityAttributes.lpSecurityDescriptor = NULL;
		securityAttributes.bInheritHandle = TRUE;
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
//		hStdOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
//			FILE_SHARE_READ | FILE_SHARE_WRITE, &securityAttributes,
//			CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleScreenBufferSize(hStdOut, size);
		SetConsoleWindowInfo(hStdOut, TRUE, &rect);
		SetConsoleActiveScreenBuffer(hStdOut);
//		SetStdHandle(STD_OUTPUT_HANDLE, hStdOut);
#pragma warning(push)
#pragma warning(disable: 6031)
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
#pragma warning(pop)
	}
}

bool isScreenSaver(void)
{
	UCSTR programName;
	bool retVal;
	UCSTR commandLine = NULL;
	UCSTR spot;

	commandLine = copyString(GetCommandLine());
	if (commandLine[0] == '"')
	{
		if ((spot = ucstrchr(commandLine + 1, '"')) != NULL)
		{
			*spot = 0;
		}
		programName = commandLine + 1;
	}
	else
	{
		if ((spot = ucstrchr(commandLine, ' ')) != NULL)
		{
			*spot = 0;
		}
		programName = commandLine;
	}
	retVal = ucstrlen(programName) > 4 &&
		ucstrcasecmp(programName + ucstrlen(programName) - 4, _UC(".scr")) == 0;
	delete commandLine;
	return retVal;
}

void debugOut(char *fmt, ...)
{
	if (!debugToFile)
	{
		return;
	}
	static const char* userProfile = getenv("USERPROFILE");
	std::string debugPath = "C:\\LDViewDebug.txt";
	if (userProfile != NULL)
	{
		debugPath = std::string(userProfile) + "\\LDViewDebug.txt";
	}
	FILE* debugFile = ucfopen(debugPath.c_str(), "a+");

	if (debugFile)
	{
		va_list marker;

		std::time_t t = std::time(nullptr);
		char mbstr[100];
		if (std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&t)))
		{
			fprintf(debugFile, "%s: ", mbstr);
		}
		va_start(marker, fmt);
		vfprintf(debugFile, fmt, marker);
		va_end(marker);
		fclose(debugFile);
	}
}

int mainLoop()
{
	MSG msg;
	HACCEL hAccel;
	bool screenSaver = isScreenSaver();
//	DWORD startTickCount = GetTickCount();

	hAccel = LoadAccelerators(CUIWindow::getLanguageModule(),
		MAKEINTRESOURCE(IDR_ACCELERATORS));
	while (1)
	{
		HWND parentWindow;
		HWND topParent;
		HWND newParent;
#pragma warning(push)
#pragma warning(disable: 28159)
		DWORD tickCount = GetTickCount();
#pragma warning(pop)

		//debugOut("%d\n", tickCount);
/*
		if (tickCount > startTickCount + 3000 || tickCount < startTickCount)
		{
			OleUninitialize();
			return 0;
		}
*/
		if (!GetMessage(&msg, NULL, 0, 0))
		{
			OleUninitialize();
			return (int)msg.wParam;
		}
#ifdef _DEBUG
//			_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "hWnd: 0x%6X msg: 0x%X\n", msg.hwnd, msg.message);
#endif // _DEBUG
		parentWindow = GetParent(msg.hwnd);
		topParent = msg.hwnd;
		while ((newParent = GetParent(topParent)) != NULL)
		{
			topParent = newParent;
		}
		if (screenSaver || !TranslateAccelerator( 
		    topParent,     // handle to receiving window 
			hAccel,        // handle to active accelerator table 
			&msg))
		{
			if (!parentWindow || !IsDialogMessage(parentWindow, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (screenSaver && msg.message == WM_DESTROY)
				{
					debugOut("WM_DESTROY\n", tickCount);
					PostQuitMessage(0);
				}
			}
		}
		TCAutoreleasePool::processReleases();
	}
}

BOOL flushPreviewEvents(HWND hWindow)
{
    // In preview mode, "pause" (enter a limited message loop) briefly 
    // before proceeding, so the display control panel knows to update itself.
    BOOL waitForInputIdle = TRUE;

    // Post a message to mark the end of the initial group of window messages
    PostMessage( hWindow, WM_USER, 0, 0 );

    MSG msg;
    while( waitForInputIdle )
    {
        // If GetMessage returns FALSE, it's quitting time.
        if( !GetMessage( &msg, NULL, 0, 0 ) )
        {
			return TRUE;
        }

        TranslateMessage( &msg );
		if (msg.message == WM_USER && msg.hwnd == hWindow)
		{
			waitForInputIdle = FALSE;
		}
		else
		{
			DispatchMessage( &msg );
		}
    }
	return TRUE;
}

void screenSaverLoop(HINSTANCE /*hInstance*/, HWND hWindow)
{
    MSG msg;
	BOOL bGotMsg;

    msg.message = WM_NULL;
    while ( msg.message != WM_QUIT && msg.message != WM_DESTROY )
    {
        bGotMsg = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
        if( bGotMsg )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
			debugOut("Message: 0x%08X\n", msg.message);
        }
        else
        {
			debugOut("No Message\n");
			if (!GetParent(hWindow))
			{
				PostQuitMessage(0);
			}
            Sleep(100);
        }
    }
/*
    while( 1 )
    {
		FILE* debugFile;
        // If GetMessage returns FALSE, it's quitting time.
        if( !GetMessage( &msg, NULL, 0, 0 ) )
        {
			debugFile = ucfopen("LDViewDebug.txt", "a+");

			if (debugFile)
			{
				fprintf(debugFile, "EXIT\n");
				fclose(debugFile);
			}
            // Post the quit message to handle it later
			return;
        }

        TranslateMessage( &msg );
		DispatchMessage( &msg );
		debugFile = ucfopen("LDViewDebug.txt", "a+");
		if (debugFile)
		{
			fprintf(debugFile, "Message: 0x%08X\n", msg.message);
			fclose(debugFile);
		}
		if (msg.message == WM_DESTROY)
		{
			debugFile = ucfopen("LDViewDebug.txt", "a+");

			if (debugFile)
			{
				fprintf(debugFile, "WM_DESTROY\n");
				fclose(debugFile);
			}
			return;
//			PostQuitMessage(0);
		}
    }
*/
}

int doPreview(HINSTANCE hInstance, LPCSTR lpCmdLine)
{
	const char *spot = strchr(lpCmdLine, ' ');

	debugOut("Command line: %s\n", lpCmdLine);
	if (spot)
	{
		long long parentWindowNum;

		spot++;
		if (sscanf(spot, "%lld", &parentWindowNum) == 1)
		{
			HWND hParentWindow = (HWND)parentWindowNum;
			SSPreview* ssPreview = new SSPreview(hParentWindow, hInstance);

			if (ssPreview->run())
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	return 1;
}

static void loadLocalStrings(const char *filename, UINT resourceId, bool replace)
{
	HMODULE hModule = (HMODULE)CUIWindow::getLanguageModule();
	HRSRC hLocalStringsResource = FindResource(hModule,
		MAKEINTRESOURCE(resourceId),
		RT_RCDATA);
	bool done = false;

	if (hLocalStringsResource)
	{
		HGLOBAL hLocalStrings = LoadResource(hModule, hLocalStringsResource);

		if (hLocalStrings)
		{
			TCByte *data = (TCByte *)LockResource(hLocalStrings);

			if (data)
			{
				DWORD length = SizeofResource(hModule, hLocalStringsResource);

				if (length)
				{
					//char *localStrings = new char[length + 1];

					//memcpy(localStrings, data, length);
					//localStrings[length] = 0;
					TCLocalStrings::setStringTable(data, length, replace);
					//delete localStrings;
					done = true;
				}
				UnlockResource(hLocalStrings);
			}
		}
	}
	if (!done)
	{
		TCLocalStrings::loadStringTable(filename, replace);
	}
}

static void setupLocalStrings(void)
{
	loadLocalStrings("LDViewMessages.ini", IDR_LOCAL_STRINGS, true);
	loadLocalStrings("LDExportMessages.ini", IDR_EXPORTER_STRINGS, false);
	//TCLocalStrings::dumpTable("C:\\Temp\\ST-ASCII.txt", "ASCII");
}

static void loadLanguageModule(void)
{
	// This function is something of a hack.  We want to force the language
	// module to load with certain pre-conditions.  The first is that prior to
	// going into this function, the app name was set to LDView.  The next is
	// that we want to change into the LDView directory, so that the
	// LoadLibrary call will find language modules in that directory, so that
	// if we're running as a screensaver, it will still find the language
	// modules.
	UCCHAR originalPath[1024];
	DWORD maxPath = COUNT_OF(originalPath);
	DWORD dirResult = GetCurrentDirectory(maxPath, originalPath);
	bool dirChange = false;

	if (dirResult > 0 && dirResult <= maxPath)
	{
		UCSTR installPath =
			TCUserDefaults::stringForKeyUC(INSTALL_PATH_4_1_KEY, NULL, false);

		if (installPath)
		{
			SetCurrentDirectory(installPath);
			delete[] installPath;
			dirChange = true;
		}
	}
	// The following forces CUIWindow to load (and cache) the language module.
	CUIWindow::getLanguageModule();
	if (dirChange)
	{
		SetCurrentDirectory(originalPath);
	}
}

static bool setupUserDefaults(
	LPCSTR lpCmdLine,
	bool screenSaver,
	bool removableDrive)
{
	char *appName = "Travis Cobbs/LDView";
	char *sessionName;
	bool retValue = true;

	TCUserDefaults::setCommandLine(lpCmdLine);
	WinWebClientPlugin::registerPlugin();
	debugToFile = TCUserDefaults::boolForKey("DebugToFile", false, false);
	std::string haveStdOut =
		TCUserDefaults::commandLineStringForKey("HaveStdOut");
	if (!haveStdOut.empty())
	{
		if (std::strtol(haveStdOut.c_str(), NULL, 10) != 0)
		{
			runningWithConsole();
		}
	}
	// IniFile can be specified on the command line; if so, don't load a
	// different one.
	if (removableDrive && !TCUserDefaults::isIniFileSet())
	{
		if (!TCUserDefaults::setIniFile("LDView.ini"))
		{
			retValue = false;
		}
	}
	TCUserDefaults::setAppName(appName);
	// The language module needs to be loaded using LDView as the app name.  So
	// if we're running in screensaver mode, we'll take care of changing our
	// app name after that is done.
	loadLanguageModule();
	if (screenSaver)
	{
		UCSTR ldrawDir =
			TCUserDefaults::stringForKeyUC(LDRAWDIR_KEY, NULL, false);

		appName = "Travis Cobbs/LDView Screen Saver";
		TCUserDefaults::setAppName(appName);
		if (ldrawDir)
		{
			TCUserDefaults::setStringForKey(ldrawDir, LDRAWDIR_KEY, false);
			delete ldrawDir;
		}
	}
#ifdef _DEBUG
	// Set the debug level before selecting a pref set.
	setDebugLevel((int)TCUserDefaults::longForKey(DEBUG_LEVEL_KEY, 1, false));
#endif // _DEBUG
	sessionName =
		TCUserDefaults::getSavedSessionNameFromKey(PREFERENCE_SET_KEY);
	if (sessionName && sessionName[0])
	{
		TCUserDefaults::setSessionName(sessionName, NULL, false);
	}
	delete sessionName;
	return retValue;
}

static bool isRemovableDrive(HINSTANCE hInstance)
{
	UCCHAR filename[2048];

	if (GetModuleFileName(hInstance, filename, COUNT_OF(filename)) > 0)
	{
		if (isalpha(filename[0]) && filename[1] == ':')
		{
			UCCHAR driveRoot[4] = _UC("");

			ucstrncpy(driveRoot, filename, 2);
			driveRoot[2] = '\\';
			if (GetDriveType(driveRoot) == DRIVE_REMOVABLE)
			{
				return true;
			}
		}
	}
	return false;
}

//static void Win7JumpListStuff(void)
//{
//	HMODULE hShell32 = LoadLibrary("shell32.dll");
//
//	if (hShell32 != NULL)
//	{
//		typedef HRESULT (__stdcall *PSetCurrentProcessExplicitAppUserModelID)(PCWSTR AppID);
//		PSetCurrentProcessExplicitAppUserModelID
//			pSetCurrentProcessExplicitAppUserModelID = 
//			(PSetCurrentProcessExplicitAppUserModelID)GetProcAddress(hShell32,
//			"SetCurrentProcessExplicitAppUserModelID");
//
//		if (pSetCurrentProcessExplicitAppUserModelID != NULL)
//		{
//			PCWSTR appID = L"Travis.Cobbs.LDView";
//			pSetCurrentProcessExplicitAppUserModelID(appID);
//			ICustomDestinationList *cdl;
//			// Using CLSID_DestinationList from the header file auto-links
//			// an incompatible copy of uuid.lib, causing link failure.
//			// "77f10cf0-3db5-4966-b520-b7c54fd35ed6"
//			IID CLSID_DestinationList = {
//				0x77f10cf0, 0x3db5, 0x4966,
//				{ 0xb5, 0x20, 0xb7, 0xc5, 0x4f, 0xd3, 0x5e, 0xd6 }
//			};
//			CoCreateInstance(CLSID_DestinationList,NULL,CLSCTX_INPROC_SERVER,
//				__uuidof(ICustomDestinationList),(void**)&cdl);
//			if (cdl != NULL)
//			{
//				cdl->SetAppID(appID);
//				// Recent Items begin list
//				UINT maxCount = 0;
//				IObjectArray* oa = 0;
//				HRESULT hr = cdl->BeginList(&maxCount,__uuidof(IObjectArray),(void**)&oa);
//
//				if (SUCCEEDED(hr) && oa != NULL)
//				{
//					// Add known category
//					hr = cdl->AppendKnownCategory(KDC_RECENT);
//					oa->Release();
//					//odl->Release();
//					//tb->Release();
//				}
//				cdl->Release();
//			}
//		}
//		FreeLibrary(hShell32);
//	}
//}

#ifdef TC_NO_UNICODE
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR lpCmdLine, int nCmdShow)
#else // TC_NO_UNICODE
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif // !TC_NO_UNICODE
{
#ifdef _DEBUG
//	MessageBox(NULL, _UC("Attach a debugger now..."), _UC("Debug"), MB_OK);
#endif
	ModelLoader* modelLoader;
	bool screenSaver = isScreenSaver();
	int retValue;
	STARTUPINFO startupInfo;

	debugOut("Command Line: <<%ls>>\n", lpCmdLine);
	std::string utf8CmdLine;
	ucstringtoutf8(utf8CmdLine, lpCmdLine);
	bool udok = setupUserDefaults(utf8CmdLine.c_str(), screenSaver,
		isRemovableDrive(hInstance));
	debugOut("Command Line: <<%ls>>\n", lpCmdLine);
#ifdef _DEBUG
	int _debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	_debugFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(_debugFlag);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	if (!haveConsole())
	{
		createConsole();
	}
#endif // _DEBUG
	setupLocalStrings();
	if (TCUserDefaults::boolForKey(DEBUG_COMMAND_LINE_KEY, false, false))
	{
		ucstring message = _UC("Command Line:\n");

		message += lpCmdLine;
		MessageBox(NULL, message.c_str(), _UC("LDView"), MB_OK);
	}
	if (TCUserDefaults::boolForKey("libver", false, false))
	{
		ucstring message;
		message = _UC("Library version:\n");
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
		message += _UC("libjpeg-turbo:\t") + std::to_wstring(LIBJPEG_TURBO_VERSION_NUMBER/1000000) + _UC(".");
		message += std::to_wstring((LIBJPEG_TURBO_VERSION_NUMBER/1000)%1000) + _UC(".") + std::to_wstring(LIBJPEG_TURBO_VERSION_NUMBER%1000)+_UC("\n");
#endif
#ifdef JPEG_LIB_VERSION_MINOR
		message += _UC("libjpeg:\t\t")+std::to_wstring(JPEG_LIB_VERSION_MAJOR);
		message.push_back(96+JPEG_LIB_VERSION_MINOR);
		message += _UC("\n");
#endif
#ifdef PNG_LIBPNG_VER_MAJOR
		message += _UC("libpng:\t\t")+std::to_wstring(PNG_LIBPNG_VER_MAJOR) + _UC(".") + std::to_wstring(PNG_LIBPNG_VER_MINOR) + _UC(".") + std::to_wstring(PNG_LIBPNG_VER_RELEASE) + _UC("\n");
#endif
#ifdef ZLIB_VER_MAJOR
		message += _UC("libz:\t\t")+std::to_wstring(ZLIB_VER_MAJOR) + _UC(".") + std::to_wstring (ZLIB_VER_MINOR) + _UC(".") + std::to_wstring(ZLIB_VER_REVISION) + _UC("\n");
#endif
#ifdef GL2PS_MAJOR_VERSION
		message += _UC("gl2ps:\t\t")+std::to_wstring(GL2PS_MAJOR_VERSION) + _UC(".") + std::to_wstring(GL2PS_MINOR_VERSION) + _UC(".") + std::to_wstring(GL2PS_PATCH_VERSION) + _UC("\n");
#endif
#ifdef TINYXML_INCLUDED
		message += _UC("tinyxml:\t\t")+std::to_wstring(TIXML_MAJOR_VERSION) + _UC(".") + std::to_wstring(TIXML_MINOR_VERSION) + _UC(".") + std::to_wstring(TIXML_PATCH_VERSION) + _UC("\n");
#endif
		MessageBox(NULL, message.c_str(), _UC("LDView"), MB_OK);
	}
	if (!udok && !TCUserDefaults::longForKey("IniFailureShown", 0, 0))
	{
		UCCHAR message[2048];
		ucstring iniPath;
		utf8toucstring(iniPath, TCUserDefaults::getIniPath());

		sucprintf(message, COUNT_OF(message),
			TCLocalStrings::get(_UC("IniFailure")), iniPath.c_str());
		CUIWindow::messageBoxUC(NULL, message, _UC("LDView"), MB_OK);
		TCUserDefaults::setLongForKey(1, "IniFailureShown", false);
	}
	if (screenSaver)
	{
		if (strncasecmp(utf8CmdLine.c_str(), "/p", 2) == 0 ||
			strncasecmp(utf8CmdLine.c_str(), "-p", 2) == 0 ||
			strncasecmp(utf8CmdLine.c_str(), "p", 1) == 0)
		{
			// preview mode
			int retValue = doPreview(hInstance, utf8CmdLine.c_str());
			return retValue;
		}
		if (strncasecmp(utf8CmdLine.c_str(), "/c", 2) == 0 ||
			strncasecmp(utf8CmdLine.c_str(), "-c", 2) == 0 ||
			strncasecmp(utf8CmdLine.c_str(), "c", 1) == 0 ||
			utf8CmdLine.empty())
		{
			SSConfigure *configure;

			configure = new SSConfigure(hInstance);
#ifdef _DEBUG
			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
			createConsole();
#endif // _DEBUG
			configure->run();
			// configure mode
			return 1;
		}
		// This shouldn't be necessary, but I've received a report of a whole
		// bunch of copies of the LDView screensaver running at once.  This
		// might not fix things entirely, but it will at least prevent it
		// from launching multiple times concurrently.
		CreateMutex(NULL, FALSE, _UC("LDView Screensaver"));
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return 0;
		}
	}
#ifdef _LOG_PERFORMANCE
	LARGE_INTEGER frequency;
	if (QueryPerformanceFrequency(&frequency))
	{
		debugPrintf("Frequency: %I64d\n", frequency.QuadPart);
	}
#endif // _LOG_PERFORMANCE
	if (OleInitialize(NULL) == NULL)
	{
		debugPrintf("OleInitialize FAILED!\n");
	}

	//Win7JumpListStuff();

	modelLoader = new ModelLoader(CUIWindow::getLanguageModule(), nCmdShow,
		screenSaver);
	retValue = mainLoop();
	modelLoader->release();
	return retValue;
} // WinMain
