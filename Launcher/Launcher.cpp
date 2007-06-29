// Launcher.cpp : Defines the entry point for the console application.
//
// This program is designed to be a console wrapper program for other graphical
// programs that want to produce console output.  It's very basic; all it does
// is look at argv[0], replace the extension (if present) with .exe (or append
// .exe if no extension is present), and then launch the .exe with the same
// name.  The program is given a .com extension (even though it's really an exe
// file so that the command interpreter will find it first when run from the
// command line.  Note that this program is only really useful in Windows XP and
// later Windows OSes, since it only works in conjunction with the AttachConsole
// system call, which was introduced in Windows XP.  Setting up pipes for the
// child program to use (with threads feeding/monitoring them here) would be
// required in the absense of the AttachConsole system call.

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <stdio.h>

int launchExe(_TCHAR *appName, _TCHAR *exeFilename, int argc, _TCHAR* argv[])
{
	int i;
	size_t len = _tcslen(exeFilename);
	_TCHAR *commandLine;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	DWORD priority = NORMAL_PRIORITY_CLASS;
	_TCHAR title[1024];

	for (i = 1; i < argc; i++)
	{
		len += _tcslen(argv[i]) + 1;
	}
	len++;
	commandLine = new _TCHAR[len];
	_tcscpy_s(commandLine, len, exeFilename);
	for (i = 1; i < argc; i++)
	{
		_tcscat_s(commandLine, len, _T(" "));
		_tcscat_s(commandLine, len, argv[i]);
	}
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	_stprintf_s(title, sizeof(title) / sizeof(title[0]), _T("Command Line %s"),
		appName);
	startupInfo.lpTitle = title;
	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USEFILLATTRIBUTE;
	startupInfo.dwFillAttribute = FOREGROUND_RED | BACKGROUND_RED |
		BACKGROUND_GREEN | BACKGROUND_BLUE;
	if (CreateProcess(exeFilename, commandLine, NULL, NULL, FALSE,
		priority, NULL, NULL, &startupInfo, &processInfo))
	{
		// while (true) now generates a warning.
		for ( ; ; )
		{
			DWORD exitCode;

			GetExitCodeProcess(processInfo.hProcess, &exitCode);
			if (exitCode != STILL_ACTIVE)
			{
				return 0;
			}
			Sleep(50);
		}
	}
	else
	{
		DWORD error = GetLastError();
		_TCHAR *buf;

		_tprintf(_T("Failed to launch %s\n"), exeFilename);
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf,
			0, NULL);
		_tprintf(_T("%s\n"), buf);
		LocalFree(buf);
		return 1;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	_TCHAR *dotSpot = _tcsrchr(argv[0], '.');

	if (!dotSpot)
	{
		dotSpot = &argv[0][_tcslen(argv[0])];
	}
	__w64 int len = dotSpot - argv[0];
	__w64 int fullLen = len + 5;
	_TCHAR *appName = new _TCHAR[len + 1];
	_TCHAR *exeFilename = new _TCHAR[fullLen];

	_tcsncpy_s(appName, len + 1, argv[0], len);
	appName[len] = 0;
	_tcsncpy_s(exeFilename, fullLen, argv[0], len);
	_tcscpy_s(&exeFilename[len], fullLen - len, _T(".exe"));
	return launchExe(appName, exeFilename, argc, argv);
}

