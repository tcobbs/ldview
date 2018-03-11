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

#if _MSC_VER < 1400	// VC < VC 2005
#define _tcscpy_s(dst, bufSize, src) _tcscpy(dst, src)
#define _tcscat_s(dst, bufSize, src) _tcscat(dst, src)
#define _tcsncpy_s(dst, bufSize, src, len) _tcsncpy(dst, src, len)
#define __w64
#endif // VC < VC 2005

int launchExe(_TCHAR *appName, _TCHAR *exeFilename, int argc, _TCHAR* argv[])
{
	int i;
	size_t len = _tcslen(exeFilename);
	_TCHAR *commandLine;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	DWORD priority = NORMAL_PRIORITY_CLASS;
	_TCHAR title[1024];
	SECURITY_ATTRIBUTES sa;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hChildStdOutRd;
	HANDLE hChildStdOutRdDup;
	HANDLE hChildStdOutWr;

	memset(&sa, 0, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hChildStdOutRd, &hChildStdOutWr, &sa, 0))
	{
		_tprintf(_T("Error creating pipe.\n"));
		return 1;
	}
	if (!DuplicateHandle(GetCurrentProcess(), hChildStdOutRd,
		GetCurrentProcess(), &hChildStdOutRdDup, 0, FALSE,
		DUPLICATE_SAME_ACCESS))
	{
		_tprintf(_T("Error restricting pipe.\n"));
		return 1;
	}
	CloseHandle(hChildStdOutRd);
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
#if _MSC_VER < 1400	// VC < VC 2005
	_stprintf(title, _T("Command Line %s"), appName);
#else // VC < VC 2005
	_stprintf_s(title, sizeof(title) / sizeof(title[0]), _T("Command Line %s"),
		appName);
#endif // VC < VC 2005
	startupInfo.lpTitle = title;
	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput = hChildStdOutWr;
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	if (CreateProcess(exeFilename, commandLine, NULL, NULL, TRUE,
		priority, NULL, NULL, &startupInfo, &processInfo))
	{
		CloseHandle(processInfo.hThread);
		CloseHandle(hChildStdOutWr);
		// while (true) now generates a warning.
		for ( ; ; )
		{
			CHAR chBuf[4096];
			DWORD dwRead = 0;
			DWORD dwWritten = 0;
			DWORD exitCode;

			if (ReadFile(hChildStdOutRdDup, chBuf, sizeof(chBuf), &dwRead, NULL)
				&& dwRead > 0)
			{
				WriteConsole(hStdOut, chBuf, dwRead / sizeof(TCHAR), &dwWritten, NULL);
			}
			else
			{
				Sleep(50);
			}
			GetExitCodeProcess(processInfo.hProcess, &exitCode);
			if (exitCode != STILL_ACTIVE)
			{
				CloseHandle(processInfo.hProcess);
				return 0;
			}
		}
	}
	else
	{
		DWORD error = GetLastError();
		_TCHAR *buf;

		_tprintf(_T("Failed to launch %s\n"), exeFilename);
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buf,
			0, NULL);
		_tprintf(_T("%s\n"), buf);
		LocalFree(buf);
		return 1;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	size_t appLen = _tcslen(argv[0]);
	_TCHAR *dotSpot = &argv[0][appLen - 4];

	if (dotSpot[0] != '.')
	{
		dotSpot = NULL;
	}
	if (!dotSpot)
	{
		dotSpot = &argv[0][_tcslen(argv[0])];
	}
	size_t len = dotSpot - argv[0];
	size_t fullLen = len + 7;
	_TCHAR *appName = new _TCHAR[len + 1];
	_TCHAR *exeFilename = new _TCHAR[fullLen];
	_tcsncpy_s(appName, len + 1, argv[0], len);
	appName[len] = 0;
	_tcsncpy_s(exeFilename, fullLen, argv[0], len);
	_tcscpy_s(&exeFilename[len], fullLen - len, _T("64.exe"));
	if (!PathFileExists(exeFilename))
	{
		_tcscpy_s(&exeFilename[len], fullLen - len, _T(".exe"));
	}
	return launchExe(appName, exeFilename, argc, argv);
}

